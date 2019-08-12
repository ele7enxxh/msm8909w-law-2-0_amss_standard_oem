/**********************************************************************
 * fs_journal.c
 *
 * File system memory journal.
 * Copyright (C) 2002-2008,2010-2012 QUALCOMM Technologies, Inc.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_journal.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-08-15    nr      Fix Bug to handle the case of an empty journal correctly.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2011-04-04    wek     Make journal size unsigned.
2010-10-20    wek     Make journal independent of page size.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-07-30    wek     Dynamically allocate all buffers dependent on page size.
2008-12-12    rp      Introducing Managed Nand.
2008-11-03    rp      Introduce device abstraction.
2007-10-18    rp      Featurized QEFS2.
2006-12-14    sh      Lint changes.
2006-06-26    yg      Memory reduction effort
2005-10-30    sh      Lint cleanup.
2005-05-26    sd      Compilation fixes for L4.
2004-10-15    dlb     Update copyright header.
2003-06-17    jkl     Clean up code.
2003-03-07    dlb     Fixed iterator to not delete modified node.
2002-08-08    drh     Created by dlb.  Added history header

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#define SIMPLE_JOURNAL_DUMP

#include <stdio.h>
#ifdef DEBUG_WALK_HISTORY
extern FILE *walk_history;
#endif

#undef LOG_JOURNAL_CHANGES

#ifdef LOG_JOURNAL_CHANGES
#define JOURNAL_LOG_FILENAME    "journal.log"
#include <stdio.h>
FILE *journal_log_file;
#endif

#include "fs_device.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_journal.h"
#include "assert.h"
#include "fs_malloc.h"

/* We need journal for both NAND and MNAND moudules. */
#if defined (FEATURE_EFS_EFS2_ON_NAND) || \
    defined (FEATURE_EFS_EFS2_ON_MNAND)

/* Bit mask accessors for the colors. */
#define IS_RED(jr, index) \
        (((jr)->color[(index)>>3] & (1u << ((index) & 7))) != 0)
#define IS_BLACK(jr, index)   (!IS_RED (jr, index))
#define SET_RED(jr, index) \
        ((jr)->color[(index)>>3] |= (1u << ((index) & 7)))
#define SET_BLACK(jr, index) \
        ((jr)->color[(index)>>3] &= ~(1u << ((index) & 7)))

/* Comparison macros. */
#define J_EQUAL(jr, a, b) \
        (((jr)->key[a] == (jr)->key[b]) && \
         ((jr)->key_type[a] == (jr)->key_type[b]))
#define J_LESS(jr, a, b) \
        (((jr)->key_type[a] < (jr)->key_type[b]) || \
         (    ((jr)->key_type[a] == (jr)->key_type[b]) && \
              ((jr)->key[a] < (jr)->key[b])))

static unsigned tree_minimum (fs_journal_t jr, unsigned x);
static unsigned tree_successor (fs_journal_t jr, unsigned x);
static void delete_node (fs_journal_t jr, unsigned z);
#ifdef FS_UNIT_TEST
#if 0
static void fs_journal_raw_tree_dump (fs_journal_t jr, char *prefix);
#endif
#endif

#ifdef FS_UNIT_TEST
int maximum_journal = 0;
#endif

void
fs_journal_init (fs_journal_t jr, uint32 journal_size)
{
  uint32 i;
  uint8 zero;

  jr->left     = fs_malloc (journal_size * sizeof (fs_j_index));
  jr->right    = fs_malloc (journal_size * sizeof (fs_j_index));
  jr->parent   = fs_malloc (journal_size * sizeof (fs_j_index));
  jr->key      = fs_malloc (journal_size * sizeof (uint32));
  jr->value    = fs_malloc (journal_size * sizeof (uint32));
  jr->age      = fs_malloc (journal_size);
  jr->key_type = fs_malloc (journal_size);
  jr->color    = fs_malloc ((journal_size + 7) / 8);

  jr->used = 0;
  jr->size = journal_size;
  jr->nil = 0;
  jr->root = 0;
  jr->free_list = 0;

  zero = 0;
  SET_BLACK (jr, zero);

  /* Create the free list. */
  for (i = 1; i < journal_size; i++) {
    jr->left[i] = jr->free_list;
    jr->free_list = i;
  }

  /* Clear the iterators. */
  for (i = 0; i < FS_JOURNAL_MAX_ITERATORS; i++) {
    jr->iterator_nexts[i] = 0;
    jr->iterators[i] = 0;
  }

#ifdef LOG_JOURNAL_CHANGES
  journal_log_file = fopen (JOURNAL_LOG_FILENAME, "w");
#endif
}

static void fixup_insert (fs_journal_t jr, unsigned int x);

void
fs_journal_add (fs_journal_t jr,
    uint32 key,
    unsigned key_type,
    uint32 value,
    unsigned age)
{
  int x, y, z;

  y = FS_J_NIL;
  x = jr->root;

#ifdef JOURNAL_LOG_FILENAME
  fprintf (journal_log_file,
      "  fs_journal_add (jr, 0x%08x, 0x%02x, 0x%08x, 0x%02x);\n",
      key, key_type, value, age);
  fflush (journal_log_file);
#endif
#ifdef DEBUG_WALK_HISTORY
  if (walk_history)
    fprintf (walk_history, "A %02x %08x %08x %02x\n",
        key_type, key, value, age);
#endif

  /* Make our new node.  If we find it, we'll put this back on the free
   * list. */
  z = jr->free_list;
  if (z == FS_J_NIL) {
    FS_ERR_FATAL ("Journal full", 0, 0, 0);
  }
  jr->free_list = jr->left[jr->free_list];

  jr->left[z] = FS_J_NIL;
  jr->right[z] = FS_J_NIL;
  jr->key[z] = key;
  jr->key_type[z] = key_type;
  jr->value[z] = value;
  jr->age[z] = age;

  /* Search for the place to put this. */
  while (x != FS_J_NIL) {
    y = x;

    /* Special check if the node is already present. */
    if (J_EQUAL (jr, z, x)) {
      /* Just copy over the fields, free up the new one, and forget it. */
      jr->value[x] = jr->value[z];
      jr->age[x] = jr->age[z];
      jr->left[z] = jr->free_list;
      jr->free_list = z;

      return;
    }

    if (J_LESS (jr, z, x))
      x = jr->left[x];
    else
      x = jr->right[x];
  }

  jr->parent[z] = y;
  if (y == FS_J_NIL) {
    jr->root = z;
  } else {
    if (J_LESS (jr, z, y))
      jr->left[y] = z;
    else
      jr->right[y] = z;
  }

  /* Basic insert is finished, now clean up the tree as necessary. */
  fixup_insert (jr, z);

  jr->used++;
#ifdef FS_UNIT_TEST
  if (jr->used > maximum_journal)
    maximum_journal = jr->used;
#endif
}

int
fs_journal_lookup (fs_journal_t jr,
    uint32 key,
    unsigned key_type,
    uint32 *value,
    unsigned *age)
{
  int x = jr->root;

  /* Store the key into the nil node. */
  /* XXX: Don't store this, make new comparison macros. */
  jr->key[FS_J_NIL] = key;
  jr->key_type[FS_J_NIL] = key_type;

  while (x != FS_J_NIL) {
    if (J_EQUAL (jr, FS_J_NIL, x))
      break;
    if (J_LESS (jr, FS_J_NIL, x))
      x = jr->left[x];
    else
      x = jr->right[x];
  }

  if (x == FS_J_NIL) {
    return FALSE;
  } else {
    *value = jr->value[x];
    *age = jr->age[x];
    return TRUE;
  }
}

void fs_journal_setup_age_walk (fs_journal_t jr,
    unsigned age)
{
  if (jr->root == FS_J_NIL)
  {
    jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] = FS_J_NIL;
  }
  else
  {
    jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] = tree_minimum (jr, jr->root);
  }
  jr->age_walk_age = age;
}

int fs_journal_age_advance (fs_journal_t jr,
    int delete_previous)
{
  int x;

  if (delete_previous && jr->iterators[FS_JOURNAL_ITERATOR_AGE] != FS_J_NIL
      && (jr->age[jr->iterators[FS_JOURNAL_ITERATOR_AGE]])
        == jr->age_walk_age)
  {
    /* If the other iterator will use us as the next node, we need to move
     * their iterator on, otherwise their iterator will be pointing to
     * gibberish. */
    if (jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE] ==
        jr->iterators[FS_JOURNAL_ITERATOR_AGE])
    {
      jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE] =
        tree_successor (jr, jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE]);
    }

    delete_node (jr, jr->iterators[FS_JOURNAL_ITERATOR_AGE]);
    jr->iterators[FS_JOURNAL_ITERATOR_AGE] = FS_J_NIL;
  }

  x = jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE];
  while (x != FS_J_NIL) {
    if (jr->age[x] == jr->age_walk_age
        && x != jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE])
    {
      jr->iterators[FS_JOURNAL_ITERATOR_AGE] = x;
      jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] = tree_successor (jr, x);

      return 1;
    } else {
      x = tree_successor (jr, x);
    }
  }

  jr->iterators[FS_JOURNAL_ITERATOR_AGE] = FS_J_NIL;
  jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] = FS_J_NIL;

  return 0;
}

void fs_journal_age_info (fs_journal_t jr,
    uint32 *key, unsigned *key_type,
    uint32 *value, unsigned *age)
{
  int x = jr->iterators[FS_JOURNAL_ITERATOR_AGE];

  if (x == FS_J_NIL)
    FS_ERR_FATAL ("Attempt to get iterator value of nil iterator", 0, 0, 0);

  *key = jr->key[x];
  *key_type = jr->key_type[x];
  *value = jr->value[x];
  *age = jr->age[x];
}

void
fs_journal_setup_key_range_walk (fs_journal_t jr,
    unsigned key_type,
    uint32 key_min, uint32 key_max)
{
  int x;

  jr->key_range_walk_key_type = key_type;
  jr->key_range_walk_key_min = key_min;
  jr->key_range_walk_key_max = key_max;

  /* Search for the smallest node greater or equal to key_min. */
  jr->key[FS_J_NIL] = key_min;
  jr->key_type[FS_J_NIL] = key_type;

  x = jr->root;

  while (x != FS_J_NIL) {
    if (J_EQUAL (jr, FS_J_NIL, x))
      break;
    if (J_LESS (jr, FS_J_NIL, x)) {
      if (jr->left[x] == FS_J_NIL) {
        break;
      }
      x = jr->left[x];
    } else {
      if (jr->right[x] == FS_J_NIL) {
        /* This node is actually less than our key_min, so advance the key.
         */
        x = tree_successor (jr, x);
        break;
      }
      x = jr->right[x];
    }
  }

  jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE] = x;
}

int
fs_journal_key_range_advance (fs_journal_t jr, int delete_previous)
{
  int x;

  if (delete_previous
      && jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE] != FS_J_NIL)
  {
    /* If the other iterator will use us as the next node, we need to move
     * their iterator on, otherwise their iterator will be pointing to
     * gibberish. */
    if (jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] ==
        jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE])
    {
      jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE] =
        tree_successor (jr, jr->iterator_nexts[FS_JOURNAL_ITERATOR_AGE]);
    }

    delete_node (jr, jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE]);
    jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE] = FS_J_NIL;
  }

  x = jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE];

  while (x != FS_J_NIL) {
    if (jr->key_type[x] != jr->key_range_walk_key_type
        || jr->key[x] < jr->key_range_walk_key_min
        || jr->key[x] > jr->key_range_walk_key_max)
    {
      break;
    } else {
      jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE] = x;
      jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE]
        = tree_successor (jr, x);

      if (x != jr->iterators[FS_JOURNAL_ITERATOR_AGE]) {
        return 1;
      }

      x = jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE];
    }
  }

  jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE] = FS_J_NIL;
  jr->iterator_nexts[FS_JOURNAL_ITERATOR_KEY_RANGE] = FS_J_NIL;

  return 0;
}

void
fs_journal_key_range_info (fs_journal_t jr,
    uint32 *key, unsigned *key_type,
    uint32 *value, unsigned *age)
{
  int x = jr->iterators[FS_JOURNAL_ITERATOR_KEY_RANGE];

  if (x == FS_J_NIL)
    FS_ERR_FATAL ("Attempt to get iterator value of nil iterator", 0, 0, 0);

  *key = jr->key[x];
  *key_type = jr->key_type[x];
  *value = jr->value[x];
  *age = jr->age[x];
}

int
fs_journal_free_count (fs_journal_t jr)
{
  return jr->size - jr->used - 1;
}

uint32
fs_journal_get_size (fs_journal_t jr)
{
  return jr->size;
}

static void
left_rotate (fs_journal_t jr, int x)
{
  int y;

  y = jr->right[x];
  jr->right[x] = jr->left[y];
  if (jr->left[y] != FS_J_NIL)
    jr->parent[jr->left[y]] = x;
  jr->parent[y] = jr->parent[x];
  if (jr->parent[x] == FS_J_NIL) {
    jr->root = y;
  } else {
    if (x == jr->left[jr->parent[x]])
      jr->left[jr->parent[x]] = y;
    else
      jr->right[jr->parent[x]] = y;
  }

  jr->left[y] = x;
  jr->parent[x] = y;
}

static void
right_rotate (fs_journal_t jr, int x)
{
  int y;

  y = jr->left[x];
  jr->left[x] = jr->right[y];
  if (jr->right[y] != FS_J_NIL)
    jr->parent[jr->right[y]] = x;
  jr->parent[y] = jr->parent[x];
  if (jr->parent[x] == FS_J_NIL) {
    jr->root = y;
  } else {
    if (x == jr->right[jr->parent[x]])
      jr->right[jr->parent[x]] = y;
    else
      jr->left[jr->parent[x]] = y;
  }

  jr->right[y] = x;
  jr->parent[x] = y;
}

/* Re-balance the tree after inserting node x. */
static void
fixup_insert (fs_journal_t jr, unsigned x)
{
  unsigned int y;

  SET_RED (jr, x);

  while (x != jr->root && IS_RED (jr, jr->parent[x])) {
    if (jr->parent[x] == jr->left[jr->parent[jr->parent[x]]]) {
      y = jr->right[jr->parent[jr->parent[x]]];
      if (IS_RED (jr, y)) {
        SET_BLACK (jr, jr->parent[x]);
        SET_BLACK (jr, y);
        SET_RED (jr, jr->parent[jr->parent[x]]);

        x = jr->parent[jr->parent[x]];
      } else {
        if (x == jr->right[jr->parent[x]]) {
          x = jr->parent[x];
          left_rotate (jr, x);
        }
        SET_BLACK (jr, jr->parent[x]);
        SET_RED (jr, jr->parent[jr->parent[x]]);
        right_rotate (jr, jr->parent[jr->parent[x]]);
      }
    } else {
      y = jr->left[jr->parent[jr->parent[x]]];
      if (IS_RED (jr, y)) {
        SET_BLACK (jr, jr->parent[x]);
        SET_BLACK (jr, y);
        SET_RED (jr, jr->parent[jr->parent[x]]);

        x = jr->parent[jr->parent[x]];
      } else {
        if (x == jr->left[jr->parent[x]]) {
          x = jr->parent[x];
          right_rotate (jr, x);
        }
        SET_BLACK (jr, jr->parent[x]);
        SET_RED (jr, jr->parent[jr->parent[x]]);
        left_rotate (jr, jr->parent[jr->parent[x]]);
      }
    }
  }

  SET_BLACK (jr, jr->root);
}

static unsigned
tree_minimum (fs_journal_t jr, unsigned x)
{
  /* FS_J_NIL should never be passed as the value at jr->left[FS_J_NIL] is
   * invalid in the tree. */
  ASSERT (x != FS_J_NIL);

  while (jr->left[x] != FS_J_NIL)
  {
    /* Avoid an Infinite loop */
    ASSERT (x != jr->left[x]);
    x = jr->left[x];
  }
  return x;
}

static unsigned
tree_successor (fs_journal_t jr, unsigned x)
{
  unsigned int y;

  if (jr->right[x] != FS_J_NIL)
    return tree_minimum (jr, jr->right[x]);
  y = jr->parent[x];
  while (y != FS_J_NIL && x == jr->right[y]) {
    x = y;
    y = jr->parent[y];
  }
  return y;
}

static void
delete_fixup (fs_journal_t jr, unsigned int x)
{
  unsigned int w;

  while (x != jr->root && IS_BLACK (jr, x)) {
    if (x == jr->left[jr->parent[x]]) {
      w = jr->right[jr->parent[x]];
      if (IS_RED (jr, w)) {
        SET_BLACK (jr, w);
        SET_RED (jr, jr->parent[x]);
        left_rotate (jr, jr->parent[x]);
        w = jr->right[jr->parent[x]];
      }
      if (IS_BLACK (jr, jr->left[w]) && IS_BLACK (jr, jr->right[w])) {
        SET_RED (jr, w);
        x = jr->parent[x];
      } else {
        if (IS_BLACK (jr, jr->right[w])) {
          SET_BLACK (jr, jr->left[w]);
          SET_RED (jr, w);
          right_rotate (jr, w);
          w = jr->right[jr->parent[x]];
        }
        if (IS_RED (jr, jr->parent[x]))
          SET_RED (jr, w);
        else
          SET_BLACK (jr, w);

        SET_BLACK (jr, jr->parent[x]);
        SET_BLACK (jr, jr->right[w]);
        left_rotate (jr, jr->parent[x]);
        x = jr->root;
      }
    } else {
      w = jr->left[jr->parent[x]];
      if (IS_RED (jr, w)) {
        SET_BLACK (jr, w);
        SET_RED (jr, jr->parent[x]);
        right_rotate (jr, jr->parent[x]);
        w = jr->left[jr->parent[x]];
      }
      if (IS_BLACK (jr, jr->left[w]) && IS_BLACK (jr, jr->right[w])) {
        SET_RED (jr, w);
        x = jr->parent[x];
      } else {
        if (IS_BLACK (jr, jr->left[w])) {
          SET_BLACK (jr, jr->right[w]);
          SET_RED (jr, w);
          left_rotate (jr, w);
          w = jr->left[jr->parent[x]];
        }
        if (IS_RED (jr, jr->parent[x]))
          SET_RED (jr, w);
        else
          SET_BLACK (jr, w);

        SET_BLACK (jr, jr->parent[x]);
        SET_BLACK (jr, jr->left[w]);
        right_rotate (jr, jr->parent[x]);
        x = jr->root;
      }
    }
  }

  SET_BLACK (jr, x);
}

/* XXX, figure out which node is actually freed, and add it back to free
 * list. */
static void
delete_node (fs_journal_t jr, unsigned z)
{
  unsigned int y;
  unsigned int x;
  unsigned int i;

#ifdef JOURNAL_LOG_FILENAME
  fprintf (journal_log_file,
      "  fs_journal_delete (jr, 0x%08x, 0x%02x);\n",
      jr->key[z], jr->key_type[z]);
  fflush (journal_log_file);
#endif

  if (jr->left[z] == FS_J_NIL || jr->right[z] == FS_J_NIL)
    y = z;
  else
    y = tree_successor (jr, z);

  if (jr->left[y] != FS_J_NIL)
    x = jr->left[y];
  else
    x = jr->right[y];

  jr->parent[x] = jr->parent[y];

  if (jr->parent[y] == FS_J_NIL) {
    jr->root = x;
  } else {
    if (y == jr->left[jr->parent[y]])
      jr->left[jr->parent[y]] = x;
    else
      jr->right[jr->parent[y]] = x;
  }

  if (y != z) {
    /* Copy over any used fields. */
    jr->key[z] = jr->key[y];
    jr->key_type[z] = jr->key_type[y];
    jr->value[z] = jr->value[y];
    jr->age[z] = jr->age[y];

    /* If any of the iterators were pointing to the node we moved, adjust
     * them to point to the original node, since that is now the same node.
     * At this point, y will never be NIL, so we don't have to special
     * check for the iterator not being used. */
    ASSERT (y != FS_J_NIL);

    /* Disable! to be sure we can reproduce the bug. */
    for (i = 0; i < FS_JOURNAL_MAX_ITERATORS; i++) {
      if (jr->iterator_nexts[i] == y)
        jr->iterator_nexts[i] = z;
      if (jr->iterators[i] == y)
        jr->iterators[i] = z;
    }
  }

  if (IS_BLACK (jr, y)) {
    delete_fixup (jr, x);
  }

  jr->parent[y] = FS_J_NIL;
  jr->left[y] = FS_J_NIL;
  jr->right[y] = FS_J_NIL;

  /* Add the node back to the free list. */
  jr->left[y] = jr->free_list;
  jr->free_list = y;

  jr->used--;
}

#ifdef FS_UNIT_TEST
static void
dump_node (fs_journal_t jr, fs_j_index node, int level)
{
  if (node == FS_J_NIL)
    return;

  dump_node (jr, jr->right[node], level+1);

#ifdef SIMPLE_JOURNAL_DUMP
  printf ("J %02x %08x %08x %02x\n",
      jr->key_type[node],
      jr->key[node],
      jr->value[node],
      jr->age[node]);
#else
  for (i = 0; i < level; i++)
    printf (" ");
  if (IS_RED (jr, node))
    printf ("\033[41;37;1m");
  else
    printf ("\033[40;37;1m");
  printf (" X \033[0m");

  /* Print the key information meaningful. */
  for (i = level; i < 8; i++)
    printf (" ");
  printf ("  kt:%02x k:%08x -> v:%08x a=%02x (%d)\n",
      jr->key_type[node],
      jr->key[node],
      jr->value[node],
      jr->age[node],
      node);
#endif

  dump_node (jr, jr->left[node], level+1);
}

void
fs_journal_dump (fs_journal_t jr)
{
  printf ("Tree:\n");
  dump_node (jr, jr->root, 0);
}

static void
walk (fs_journal_t jr, int *max_black,
    fs_j_index node,
    fs_j_index parent,
    int black_count,
    int *tree_node_count)
{
  int child;

  /* Count unless we are null. */
  if (node != FS_J_NIL)
    (*tree_node_count)++;

  /* If we are a leaf, check the black count. */
  if (node == FS_J_NIL) {
    if (*max_black >= 0) {
      if (black_count != *max_black) {
        FS_ERR_FATAL ("Tree is not balanced", 0, 0, 0);
      }
    } else {
      *max_black = black_count;
    }

    /* Nothing more, just return. */
    return;
  }

  /* Make sure this node is in range. */
  if (node <= 0 || node >= jr->size)
    FS_ERR_FATAL ("Node pointer is out of bounds", 0, 0, 0);

  /* Check simple connectivity of the tree. */
  if (jr->parent[node] != parent)
    FS_ERR_FATAL ("Incorrect parent", 0, 0, 0);

  /* Assure readness is proper. */
  if (parent != FS_J_NIL
      && IS_RED (jr, node)
      && IS_RED (jr, parent)) {
    FS_ERR_FATAL ("Adjacent red nodes", 0, 0, 0);
  }

  /* Increment black count if we are black. */
  if (IS_BLACK (jr, node))
    black_count++;

  /* Check out left child. */
  child = jr->left[node];
  if (child != FS_J_NIL) {
    /* Assure value is less. */
    if (!J_LESS (jr, child, node)) {
      // fs_journal_dump (jr);
      FS_ERR_FATAL ("Badly ordered tree %d %d", child, node, 0);
    }
  }
  walk (jr, max_black, child, node, black_count, tree_node_count);

  /* Check out right child. */
  child = jr->right[node];
  if (child != FS_J_NIL) {
    /* Assure value is less. */
    if (!J_LESS (jr, node, child)) {
      // fs_journal_dump (jr);
      FS_ERR_FATAL ("Badly ordered tree %d %d", child, node, 0);
    }
  }
  walk (jr, max_black, child, node, black_count, tree_node_count);
}

void
fs_journal_validate (fs_journal_t jr)
{
  int black_count = -1;
  int tree_node_count = 0;
  uint32 free_count = 0;
  int node;

  if (!IS_BLACK (jr, FS_J_NIL))
    FS_ERR_FATAL ("Root node is no longer black", 0, 0, 0);
  walk (jr, &black_count, jr->root, FS_J_NIL, 0, &tree_node_count);

  for (node = jr->free_list; node != FS_J_NIL; node = jr->left[node])
    free_count++;

  if (free_count + tree_node_count != jr->size - 1) {
    FS_ERR_FATAL ("Sum of tree count (%d), and free list (%d) != total (%d)",
        tree_node_count, free_count, jr->size - 1);
  }

  // printf ("Sum of tree count (%d), and free list (%d) != total (%d)\n",
  //     tree_node_count, free_count, jr->size - 1);
}

/* For debugging, allow us to delete a specific node.  This exercises the
 * deletion/rebalance code at a slightly lower level. */
void
fs_journal_delete (fs_journal_t jr, uint32 key, unsigned key_type)
{
  int x = jr->root;

  /* Store the key into the nil node. */
  jr->key[FS_J_NIL] = key;
  jr->key_type[FS_J_NIL] = key_type;

  while (x != FS_J_NIL) {
    if (J_EQUAL (jr, FS_J_NIL, x))
      break;
    if (J_LESS (jr, FS_J_NIL, x))
      x = jr->left[x];
    else
      x = jr->right[x];
  }

  if (x == FS_J_NIL) {
    FS_ERR_FATAL ("Attempting to delete node not present.", 0, 0, 0);
  }

  delete_node (jr, x);
}

void fs_journal_setup_simple_iterator (
    struct fs_journal_simple_iterator *iter,
    fs_journal_t jr)
{
  iter->jr = jr;
  iter->started = 0;
  iter->node = FS_J_NIL;
}

void fs_journal_simple_next (
    struct fs_journal_simple_iterator *iter,
    uint32      *key,
    unsigned    *key_type,
    uint32      *value,
    unsigned    *age,
    int         *finished)
{
  fs_journal_t jr = iter->jr;

  if (!iter->started) {
    if (jr->root == FS_J_NIL)
      iter->node = FS_J_NIL;
    else
      iter->node = tree_minimum (jr, jr->root);

    iter->started = 1;
  } else {
    iter->node = tree_successor (jr, iter->node);
  }

  if (iter->node == FS_J_NIL)
    *finished = 1;
  else {
    *finished = 0;
    *key = jr->key[iter->node];
    *key_type = jr->key_type[iter->node];
    *value = jr->value[iter->node];
    *age = jr->age[iter->node];
  }
}

void
fs_journal_compare (fs_journal_t a, fs_journal_t b)
{
  uint32 akey, bkey;
  unsigned atype, btype;
  uint32 avalue, bvalue;
  unsigned aage, bage;
  struct fs_journal_simple_iterator aiter, biter;
  int afin, bfin;

  fs_journal_setup_simple_iterator (&aiter, a);
  fs_journal_setup_simple_iterator (&biter, b);

  fs_journal_simple_next (&aiter, &akey, &atype, &avalue, &aage, &afin);
  fs_journal_simple_next (&biter, &bkey, &btype, &bvalue, &bage, &bfin);

  while (1) {
    if (afin && bfin)
      break;
    if (afin) {
      printf ("B has more\n");
      do {
        printf ("add: kt:%02x k:%08x -> v:%08x a=%02x\n",
            btype, bkey, bvalue, bage);
        fs_journal_simple_next (&biter, &bkey, &btype, &bvalue,
            &bage, &bfin);
      } while (!bfin);
      return;
    }
    if (bfin) {
      printf ("A has more\n");
      do {
        printf ("del: kt:%02x k:%08x -> v:%08x a=%02x\n",
            atype, akey, avalue, aage);
        fs_journal_simple_next (&aiter, &akey, &atype, &avalue,
            &aage, &afin);
      } while (!afin);
      return;
    }

    if (akey == bkey && atype == btype) {
      if (avalue != bvalue || aage != bage) {
        printf ("diff: kt:%02x k:%08x -> Av:%08x Bv:%08x Aa=%02x Ba=%02x\n",
            atype, akey, avalue, bvalue, aage, bage);
      }
      fs_journal_simple_next (&aiter, &akey, &atype, &avalue, &aage,
          &afin);
      fs_journal_simple_next (&biter, &bkey, &btype, &bvalue, &bage,
          &bfin);
    } else {
      if (atype < btype || (atype == btype && akey < bkey)) {
        printf ("miss: a-> kt:%02x k:%08x -> v:%08x a=%02x\n",
            atype, akey, avalue, aage);
        fs_journal_simple_next (&aiter, &akey, &atype, &avalue, &aage,
            &afin);
      } else {
        printf ("B wrong\n");
        fs_journal_simple_next (&biter, &bkey, &btype, &bvalue, &bage,
            &bfin);
      }
    }
  }
}
#endif /* FS_UNIT_TEST */

#endif /* defined FEATURE_EFS_EFS2_ON_NAND or FEATURE_EFS_EFS2_ON_MNAND */

#endif /* FEATURE_EFS_QEFS2 */
