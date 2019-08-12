/**
  @file mre_xml.c

  @brief  Recursive-descent XML parser for Modem Rules Engine.

  This handles a limited subset of XML that is sufficient for the Modem Rules
  Engine; when a fuller XML parser is generally available within modem, MRE
  should switch to that.

  This parser is not optimized for performance (it uses a fair amount of
  lookahead and string comparison rather than tokenizing the XML).  This is
  not expected to be a significant performance issue, though, since the
  documents being parsed are expected to be small.
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_xml.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_xml.h"

#include <stringl/stringl.h>



/*=============================================================================
===============================================================================

  ABNF for XML that is parsed:

  document    :=  xml_header [comment] element
  xml_header  :=  processing
  processing  :=  '<?' CHAR* '?>'
  comment     :=  '<!--' CHAR* '-->'
  element     :=  empty_elem | content_elem
  empty_elem  :=  '<' name (WS+ attribute)* WS* '/>'
  content_elem:=  '<' name (WS+ attribute)* WS* '>'
                  (element | comment)* | text
                  '</' name WS* '>'           ;same name as start tag
  name        :=  ALPHA ALPHANUM*
  text        :=  TEXT_CHAR+
  attribute   :=  name WS* '=' WS* '"' CHAR* '"'
  WS          :=  ' ' | '\t' | '\n'

===============================================================================
=============================================================================*/


/*=============================================================================
  Macro to log XML parsing in unit tests.
=============================================================================*/

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*=============================================================================
===============================================================================

  Structure definitions.  These are hidden from consumers of the parser,
  who only see pointers to typedefs.

===============================================================================
=============================================================================*/


/*-------- mre_xml_attribute --------*/
/**
@brief  Structure in which attribute information is stored.
*/
typedef struct
{
  char  *pName;
  char  *pValue;
} mre_xml_attribute;



/*-------- mre_xml_element_t --------*/
/**
@brief  Structure for describing an element of an XML document.  This
        is linked into a document tree (the root of which is an
        mre_xml_root_t) through the "children" member.
*/
struct mre_xml_element_t
{
  char            *pTag;
  char            *pText;
  size_t           lineNum;
  mre_set_t       *pChildren;    /* set of mre_xml_element_t */
  mre_set_t       *pAttributes;  /* set of mre_xml_attribute */
  mre_xml_root_t  *pRoot;        /* pointer to root of document tree */
};


/*-------- mre_xml_root_t --------*/
/**
@brief  The reference-counted root of a parsed XML document.
*/
struct mre_xml_root_t
{
  _REF_CNT_OBJ;

  mre_xml_element_t *pHeader;
  mre_xml_element_t *pDocument;
  size_t             lineNum;
};





/*=============================================================================
===============================================================================

  Forward declarations.

===============================================================================
=============================================================================*/

static mre_status_t mre_optional_element(
  char const        **ppDoc,
  mre_xml_element_t **ppElem,
  mre_xml_root_t     *pRoot
);

static mre_status_t mre_optional_attributes(
  char const        **ppDoc,
  mre_xml_element_t  *pElem,
  mre_xml_root_t     *pRoot
);



/*=============================================================================
===============================================================================

  Macros that are used within parsing code.  These assume that the function
  in which they are used satisfy:

  - There is a parameter named "ppDoc" that is a pointer to the pointer to
    XML document being parsed.  This is advanced as we parse.
  - There is a local variable in the function named "status" that contains
    the status to be returned to the caller.
  - There is a label named "Done" in the function that is the common exit point
    that cleans up and returns.

===============================================================================
=============================================================================*/


/*-------- NEXT_CHAR --------*/
/**
@brief  Return the next character in the document.
*/
#define NEXT_CHAR             **ppDoc


/*-------- END_OF_DOC --------*/
/**
@brief  Evaluates to whether the current pointer to the document is at the
        the end of the document.
*/
#define END_OF_DOC            (NEXT_CHAR == '\0')


/*-------- UPDATE_LINE_NUM --------*/
/**
@brief  Update the line number in the root element if the current character
        is a newline.
*/
#define UPDATE_LINE_NUM       if ('\n' == NEXT_CHAR) ++pRoot->lineNum


/*-------- ADVANCE --------*/
/**
@brief  Advance the document pointer by one character.
*/
#define ADVANCE               UPDATE_LINE_NUM; ++*ppDoc


/*-------- DONE_IF --------*/
/**
@brief  Go to the "Done" label if the predicate is TRUE.
*/
#define DONE_IF(pred)         if (pred) goto Done


/*-------- DONE_IF_SET_STATUS --------*/
/**
@brief  Similar to DONE_IF, but also set the status to the value given.
*/
#define DONE_IF_SET_STATUS(p, s)  \
  if (p)                          \
  {                               \
    status = s;                   \
    goto Done;                    \
  }


/*-------- SUCCESS_IF --------*/
/**
@brief  Set status to "success" and go to "Done"
*/
#define SUCCESS_IF(pred)      DONE_IF_SET_STATUS(pred, MRE_STATUS_SUCCESS)


/*-------- NOT_PRESENT_IF --------*/
/**
@brief  Set status to "not present" and go to "Done"
*/
#define NOT_PRESENT_IF(pred)  DONE_IF_SET_STATUS(pred, MRE_STATUS_ERR_NOT_PRESENT)


/*-------- MALFORMED_IF --------*/
/**
@brief  Set status to "malformed XML" and go to "Done"
*/
#define MALFORMED_IF(pred)    DONE_IF_SET_STATUS(pred, MRE_STATUS_ERR_MALFORMED_XML)


#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */

#define UT_VALIDATE(pred, str)

#endif /* -TEST_FRAMEWORK. */



/*=============================================================================
===============================================================================

  Internal parsing functions

===============================================================================
=============================================================================*/


/*-------- mre_element_free --------*/
/**
@brief  Free the memory associated with an attribute.
*/
static void mre_attribute_free(
  void  *pSetElem
)
{
  mre_xml_attribute *pAttrib = (mre_xml_attribute *) pSetElem;

  MRE_MEM_FREE_IF(pAttrib->pName);
  MRE_MEM_FREE_IF(pAttrib->pValue);
}


/*-------- mre_element_free --------*/
/**
@brief  Free the memory associated with an element of the document tree.
        This walks the tree recursively, freeing the elements.
*/
static void mre_element_free(
  mre_xml_element_t *pElem
)
{
  if (pElem == NULL)  { return; }

  MRE_MEM_FREE_IF(pElem->pTag);
  MRE_MEM_FREE_IF(pElem->pText);

  REF_CNT_OBJ_RELEASE_IF(pElem->pAttributes);
  REF_CNT_OBJ_RELEASE_IF(pElem->pChildren);

  mre_mem_free(pElem);
}


/*-------- mre_root_element_dtor --------*/
/**
@brief  Destructor for a document tree.
*/
static void mre_root_element_dtor(
  void  *pObj
)
{
  mre_xml_root_t  *pMe = (mre_xml_root_t *) pObj;

  mre_element_free(pMe->pHeader);
  mre_element_free(pMe->pDocument);

  mre_mem_free(pMe);
}


/*-------- mre_root_element_new --------*/
/**
@brief  Create a new (reference-counted) document tree root node.
*/
static mre_xml_root_t * mre_root_element_new(
  void
)
{
  mre_xml_root_t  *pRoot;

  pRoot = (mre_xml_root_t *) mre_mem_alloc(sizeof(mre_xml_root_t));
  ref_cnt_obj_init(pRoot, mre_root_element_dtor);

  pRoot->pHeader    = NULL;
  pRoot->pDocument  = NULL;
  pRoot->lineNum    = 1;

  return pRoot;
}



/*-------- mre_is_alpha --------*/
/**
@brief  Return whether a character is an alpha character in ASCII.
        Note that this doesn't handle either UTF-8 or ISO-8859-x.
        This is only used in parsing element tags, which are thus
        required to be ASCII characters only.
*/
static boolean mre_is_alpha(
  char  c
)
{
  return    ('A' <= c && c <= 'Z')
         || ('a' <= c && c <= 'z');
}


/*-------- mre_is_tag_char --------*/
/**
@brief  Return whether a character is an allowed tag character.
        See mre_is_alpha() for the constraints this places on tags.
*/
static boolean mre_is_tag_char(
  char  c
)
{
  return    mre_is_alpha(c)
         || ('0' <= c && c <= '9')
         || c == '_'
         || c == '.'
         || c == '-'
         || c == ':'  /* treat namespace delimiters as part of tag */
         ;
}


/*-------- mre_is_whitespace --------*/
/**
@brief  Return whether a character is whitespace.
*/
static boolean mre_is_whitespace(
  char  c
)
{
  return    c == ' '
         || c == '\t'
         || c == '\n'
         || c == '\r'
         ;
}



/*-------- mre_consume --------*/
/**
@brief  "Consume" a string from the document, advancing the document pointer
        past the string if it is the next string in the document.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pStr    Pointer to a string.

@return
  TRUE if the document pointer has been advanced past the string.
  FALSE if the document pointer does not point to the string.
*/
static boolean mre_consume(
  char const **ppDoc,
  char const  *pStr
)
{
  boolean consumed;
  size_t  len = strlen(pStr);

  consumed = strncmp(*ppDoc, pStr, len) == 0;
  if (consumed)
  {
    *ppDoc += len;
  }

  return consumed;
}



/*-------- mre_consume_past --------*/
/**
@brief  "Consume" the document until a particular string is encountered,
        and then consume that string from the document.  This is used to
        advance past XML elements that we don't actually handle (like
        processing directives or comments).

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pStr    Pointer to a string.

@return
  TRUE if the document pointer has been advanced past the string.
  FALSE if the document pointer does not point to the string.
*/
static boolean mre_consume_past(
  char const     **ppDoc,
  char const      *pStr,
  mre_xml_root_t  *pRoot
)
{
  size_t  len = strlen(pStr);

  while (!END_OF_DOC && strncmp(*ppDoc, pStr, len) != 0)
  {
    ADVANCE;
  }

  return mre_consume(ppDoc, pStr);
}


/*-------- mre_consume_whitespace --------*/
/**
@brief  Advance the document pointer past all whitespace.
*/
static void mre_consume_whitespace(
  char const     **ppDoc,
  mre_xml_root_t  *pRoot
)
{
  while (mre_is_whitespace(NEXT_CHAR))
  {
    ADVANCE;
  };
}


/*-------- mre_terminate_at_whitespace --------*/
/**
@brief  Trim trailing whitespace from a string.  This is designed for tags,
        which cannot contain spaces, so it terminates the string at the
        first space.
*/
static void mre_terminate_at_whitespace(
  char  *pStr
)
{
  while (*pStr != '\0')
  {
    if (mre_is_whitespace(*pStr))
    {
      *pStr = '\0';
      break;
    }

    ++pStr;
  }
}


/*-------- mre_trim_trailing_whitespace --------*/
/**
@brief  Remove whitespace from the end of a string.
*/
static void mre_trim_trailing_whitespace(
  char  *pStr
)
{
  char  *pEnd;

  if (NULL == pStr)
  {
    return;
  }

  /*  Point to end of the string.
   */
  pEnd = pStr + strlen(pStr) - 1;

  /*  Move back in the string until we get to non-whitespace or the beginning.
   */
  while ((pEnd > pStr) && mre_is_whitespace(*pEnd))
  {
    --pEnd;
  }

  /*  Point to the next character (either whitespace or the terminating NUL)
   *  and terminate the string there.
   */
  ++pEnd;
  *pEnd = '\0';
}


/*-------- mre_consume_comment --------*/
/**
@brief  If the document pointer is pointing at an XML comment, consume that
        comment.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.

@return
  TRUE if a comment was consumed.
  FALSE if the document pointer was not pointing at a comment.
*/
static boolean mre_consume_comment(
  char const     **ppDoc,
  mre_xml_root_t  *pRoot
)
{
  mre_consume_whitespace(ppDoc, pRoot);

  if (!mre_consume(ppDoc, "<!--"))
  {
    return FALSE;
  }

  return mre_consume_past(ppDoc, "-->", pRoot);
}


/*-------- mre_optional_tag --------*/
static boolean mre_optional_tag(
  char const     **ppDoc,
  char           **ppTag,
  mre_xml_root_t  *pRoot
)
{
  boolean     haveTag = FALSE;
  char        c       = **ppDoc;
  char const  *pTag   = *ppDoc;
  size_t      len;

  DONE_IF(!mre_is_alpha(c));
  ADVANCE;

  while (mre_is_tag_char(c))
  {
    ADVANCE;
    c = NEXT_CHAR;
  }

  /*  Allocate memory for the tag and copy it into that memory.
   *  NUL termination is ensured by over-allocating zero-filled memory.
   */
  len = *ppDoc - pTag;
  *ppTag = (char *) mre_mem_alloc(len + 1);
  memscpy(*ppTag, len + 1, pTag, len);

  haveTag = TRUE;
  MRE_XML_MSG("processing %s", *ppTag, 0, 0);

Done:
  return haveTag;
}



/*-------- mre_require_tag --------*/
/**
@brief  Get the start tag of an XML element.

@param[in/out]  ppDoc Pointer to a pointer into the XML document.
@param[out]     ppTag Pointer to storage for a pointer to a NUL-terminated
                string that is the tag.

@return
  TRUE if the document pointer points to the start on an XML tag.
  FALSE if not.
*/
static boolean mre_require_tag(
  char const     **ppDoc,
  char           **ppTag,
  mre_xml_root_t  *pRoot
)
{
  boolean haveTag = mre_optional_tag(ppDoc, ppTag, pRoot);

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  return haveTag;
}


/*-------- mre_optional_processing --------*/
/**
@brief  Handle a processing element if the next element in the document is
        such an element.  The handling here is just to store the element
        information in a mre_xml_element_t.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     ppElem  Pointer to storage for a mre_xml_element_t pointer.

@return
  MRE_STATUS_SUCCESS if a processing element was handled.
  MRE_STATUS_ERR_NOT_PRESENT if the next element is not a processing element.
  MRE_STATUS_ERR_MALFORMED_XML if there is a start tag for processing, but
    no end tag.
*/
static mre_status_t mre_optional_processing(
  char const        **ppDoc,
  mre_xml_element_t **ppElem,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t status;

  mre_consume_whitespace(ppDoc, pRoot);

  NOT_PRESENT_IF(!mre_consume(ppDoc, "<?"));

  *ppElem = (mre_xml_element_t *) mre_mem_alloc(sizeof(mre_xml_element_t));
  LINE_NUM(*ppElem) = LINE_NUM(pRoot);

  MALFORMED_IF(!mre_require_tag(ppDoc, &(*ppElem)->pTag, pRoot));

  status = mre_optional_attributes(ppDoc, *ppElem, pRoot);
  DONE_IF(status == MRE_STATUS_ERR_MALFORMED_XML);

  MALFORMED_IF(!mre_consume_past(ppDoc, "?>", pRoot));

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- mre_require_processing --------*/
/**
@brief  Require that the next element in the document be a processing element.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     ppElem  Pointer to storage for a mre_xml_element_t pointer.

@return
  MRE_STATUS_SUCCESS if a processing element was handled.
  MRE_STATUS_ERR_MALFORMED_XML if the next element is not a processing
    element or if the processing element is present but malformed.
*/
static mre_status_t mre_require_processing(
  char const        **ppDoc,
  mre_xml_element_t **ppElem,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t status = mre_optional_processing(ppDoc, ppElem, pRoot);

  return (status == MRE_STATUS_SUCCESS) ? MRE_STATUS_SUCCESS
                                        : MRE_STATUS_ERR_MALFORMED_XML;
}


/*-------- mre_collect_text --------*/
/**
@brief  Collects text that falls between the current document pointer and
        the specified terminator character.

@param[in/out]  ppDoc       Pointer to a pointer into the XML document.
@param[out]     ppStr       Pointer to storage for a pointer to a NUL-terminated
                            string that is the text.
@param[in]      terminator  The character at which to stop collecting text.

@return
  Nothing.
*/
static void mre_collect_text(
  char const     **ppDoc,
  char           **ppStr,
  char             terminator,
  mre_xml_root_t  *pRoot
)
{
  size_t       len;
  char const  *pStr = *ppDoc;

  while (!END_OF_DOC && NEXT_CHAR != terminator)
  {
    ADVANCE;
  }

  /*  Allocate memory for the text and copy it into that memory.
   *  NUL termination is ensured by over-allocating zero-filled memory.
   */
  len = *ppDoc - pStr;
  if (len != 0)
  {
    *ppStr = (char *) mre_mem_alloc(len + 1);
    memscpy(*ppStr, len + 1, pStr, len);
  }

//  MRE_XML_MSG("mre_collect_text returns %s", *ppStr, 0, 0);
}



/*-------- mre_require_string --------*/
/**
@brief  Require that the next part of the document is a string enclosed
        in quotes, and return that string.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     ppStr   Pointer to storage for a pointer to a NUL-terminated
                        string that is returned.

@return
  TRUE if the next part of the document was a string.
  FALSE otherwise.
*/
static boolean mre_require_string(
  char const     **ppDoc,
  char           **ppStr,
  mre_xml_root_t  *pRoot
)
{
  boolean     success = FALSE;

  mre_consume_whitespace(ppDoc, pRoot);

  DONE_IF(NEXT_CHAR != '"');
  ADVANCE;

  mre_collect_text(ppDoc, ppStr, '"', pRoot);

  DONE_IF(NEXT_CHAR != '"');
  ADVANCE;

  success = TRUE;

Done:
  return success;
}


/*-------- mre_optional_attribute --------*/
/**
@brief  If the next part of the document is an attribute, get the attribute.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     pAttrib Pointer to the attribute structure to fill with
                        the name/value pair.

@return
  MRE_STATUS_SUCCESS if a properly formed attribute was found.
  MRE_STATUS_ERR_NOT_PRESENT if no attribute was found.
  MRE_STATUS_ERR_MALFORMED_XML if an attribute is not at this position.
*/
static mre_status_t mre_optional_attribute(
  char const        **ppDoc,
  mre_xml_attribute  *pAttrib,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t  status;

  pAttrib->pName = NULL;
  pAttrib->pValue = NULL;

  mre_consume_whitespace(ppDoc, pRoot);

  NOT_PRESENT_IF  (!mre_optional_tag(ppDoc, &pAttrib->pName, pRoot));

  mre_consume_whitespace(ppDoc, pRoot);

  MALFORMED_IF    (!mre_consume(ppDoc, "="));

  mre_consume_whitespace(ppDoc, pRoot);

  MALFORMED_IF    (!mre_require_string(ppDoc, &pAttrib->pValue, pRoot));

  status = MRE_STATUS_SUCCESS;

Done:
  if (status != MRE_STATUS_SUCCESS)
  {
    mre_attribute_free(pAttrib);
  }
/*
  else
  {
    MRE_XML_MSG("mre_optional_attribute got '%s' = '%s'", pAttrib->pName, pAttrib->pValue, 0);
  }
*/

  return status;
}


/*-------- mre_optional_attributes --------*/
/**
@brief  Get a series of attributes, storing them in the attribute
        array of an element node.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pElem   Pointer to the element node in which to store the
                        attributes.

@return
  MRE_STATUS_SUCCESS if a properly formed attribute was found.
  MRE_STATUS_ERR_MALFORMED_XML the XML is malformed.
*/
static mre_status_t mre_optional_attributes(
  char const        **ppDoc,
  mre_xml_element_t  *pElem,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t      status = MRE_STATUS_SUCCESS;
  mre_xml_attribute attrib;

  while (TRUE)
  {
    status = mre_optional_attribute(ppDoc, &attrib, pRoot);

    SUCCESS_IF(status == MRE_STATUS_ERR_NOT_PRESENT);
    DONE_IF(status != MRE_STATUS_SUCCESS);

    /*  Create the attributes set if it's not present
     */
    if (pElem->pAttributes == NULL)
    {
      pElem->pAttributes = mre_set_new( sizeof(mre_xml_attribute),
                                        5,
                                        MRE_SET_NO_MAX_SIZE,
                                        NULL,
                                        mre_attribute_free );
      LINE_NUM(pElem) = LINE_NUM(pRoot);
    }

    /*  Add the attribute to the set.
     */
    mre_set_add(pElem->pAttributes, &attrib);
  }

Done:
  return status;
}



/*-------- mre_lookahead --------*/
/**
@brief  Look ahead to see if an string is present at the current document pointer.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pStr    Pointer to the string to check for.

@return
  TRUE if the string passed in is a prefix of the remaining document.
*/
static boolean mre_lookahead(
  char const  **ppDoc,
  char const   *pStr
)
{
  return strncmp(*ppDoc, pStr, strlen(pStr)) == 0;
}


/*-------- mre_child_free --------*/
static void mre_child_free(
  void  *pChild
)
{
  if (pChild != NULL)
  {
    mre_element_free(*((mre_xml_element_t **) pChild));
  }
}


/*-------- mre_optional_elements --------*/
/**
@brief  Handle a sequence of elements (either comments or elements) between
        the start and end tags of an element.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pElem   Pointer to the element into which to put any element
                        nodes as children.

@return
  Nothing.
*/
static void mre_optional_elements(
  char const        **ppDoc,
  mre_xml_element_t  *pElem,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t       status;
  mre_xml_element_t *pNewElem;

  mre_consume_whitespace(ppDoc, pRoot);

  /*  Loop while the next part of the document isn't an end tag.
   */
  while (!END_OF_DOC && !mre_lookahead(ppDoc, "</"))
  {
    /*  If the next element isn't a comment, continue.
     */
    if (mre_consume_comment(ppDoc, pRoot))
    {
      mre_consume_whitespace(ppDoc, pRoot);
      continue;
    }

    /*  Try and get an element.
     */
    status = mre_optional_element(ppDoc, &pNewElem, pRoot);

    /*  If we weren't successful, we're done.
     */
    if (status != MRE_STATUS_SUCCESS)
    {
      break;
    }

    /*  Otherwise, add the successfully parsed element to the children of the
     *  enclosing element.
     */
    if (pElem->pChildren == NULL)
    {
      pElem->pChildren = mre_set_new( sizeof(mre_xml_element_t *),
                                      10,
                                      MRE_SET_NO_MAX_SIZE,
                                      NULL,
                                      mre_child_free );
    }
    mre_set_add(pElem->pChildren, &pNewElem);

    mre_consume_whitespace(ppDoc, pRoot);
  }

  /*  Consume any whitespace and comments that follow the last element.
   */
  do
  {
    mre_consume_whitespace(ppDoc, pRoot);
  } while (mre_consume_comment(ppDoc, pRoot));
}


/*-------- mre_require_element_content --------*/
/**
@brief  The document pointer is past the start tag of an element.  Get the
        content of that element, which can be either text or a sequence of
        comments and elements.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[in]      pElem   Pointer to the element into which to put the content.

@return
  MRE_STATUS_SUCCESS if properly formed element content was found.
  MRE_STATUS_ERR_MALFORMED_XML if there is not valid content at this position.
*/
static mre_status_t mre_require_element_content(
  char const        **ppDoc,
  mre_xml_element_t  *pElem,
  mre_xml_root_t     *pRoot
)
{
  char         *pTag = NULL;
  mre_status_t  status = MRE_STATUS_ERR_MALFORMED_XML;

  mre_consume_whitespace(ppDoc, pRoot);

  /*  If the next character is not '<', treat the content as text; otherwise
   *  look for node elements.
   */
  if (NEXT_CHAR != '<')
  {
    mre_collect_text(ppDoc, &pElem->pText, '<', pRoot);
    mre_trim_trailing_whitespace(pElem->pText);
  }
  else
  {
    mre_optional_elements(ppDoc, pElem, pRoot);
  }

  /*  We require an end tag when we're done processing elements.
   */
  DONE_IF(!mre_consume(ppDoc, "</"));

  /*  Get the tag in the end tag and compare to the tag that started the
   *  element.  If they don't match, the XML is malformed.
   */
  mre_collect_text(ppDoc, &pTag, '>', pRoot);
  mre_terminate_at_whitespace(pTag);
  if (strcmp(pTag, pElem->pTag) != 0)
  {
    MRE_XML_MSG("expected end tag %s, but got %s", pElem->pTag, pTag, 0);
    goto Done;
  }

  mre_consume(ppDoc, ">");

  status = MRE_STATUS_SUCCESS;

Done:
  MRE_MEM_FREE_IF(pTag);
  return status;
}


/*-------- mre_optional_element --------*/
/**
@brief  Try getting an element node at this document position.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     pElem   Pointer to the element into which to add an element.

@return
  MRE_STATUS_SUCCESS if a properly formed element was found.
  MRE_STATUS_ERR_NOT_PRESENT if no element was found.
  MRE_STATUS_ERR_MALFORMED_XML the XML is malformed.
*/
static mre_status_t mre_optional_element(
  char const        **ppDoc,
  mre_xml_element_t **ppElem,
  mre_xml_root_t     *pRoot
)
{
  boolean           succeeded;
  mre_status_t      status;

  mre_consume_whitespace(ppDoc, pRoot);

  /*  If the next character is not a '<', this isn't an element...
   */
  NOT_PRESENT_IF(!mre_consume(ppDoc, "<"));

  /*  Get memory for the element.
   */
  *ppElem = (mre_xml_element_t *) mre_mem_alloc(sizeof(mre_xml_element_t));
  LINE_NUM(*ppElem) = LINE_NUM(pRoot);

  /*  Get the tag name of the element.
   */
  MALFORMED_IF(!mre_require_tag(ppDoc, &(*ppElem)->pTag, pRoot));

  /*  Get any attributes.
   */
  status = mre_optional_attributes(ppDoc, *ppElem, pRoot);
  DONE_IF(status != MRE_STATUS_SUCCESS);

  mre_consume_whitespace(ppDoc, pRoot);

  /*  See if we're at the end of the element (see if this is an empty
   *  element).  If so, we're done.
   */
  SUCCESS_IF(mre_consume(ppDoc, "/>"));

  /*  If this isn't an empty element, it should close with a '>'.
   */
  succeeded = mre_consume(ppDoc, ">");
  if (!succeeded)
  {
    status = MRE_STATUS_ERR_MALFORMED_XML;
    MRE_XML_MSG("expected '>' but got '%c'", NEXT_CHAR, 0, 0);
    goto Done;
  }

  status = mre_require_element_content(ppDoc, *ppElem, pRoot);

Done:
  return status;
}



/*-------- mre_require_element --------*/
/**
@brief  Require an element at the current document position.

@param[in/out]  ppDoc   Pointer to a pointer into the XML document.
@param[out]     ppElem  Pointer to storage into which to put a pointer to
                        a new element node.

@return
  MRE_STATUS_SUCCESS if a properly formed element was found.
  MRE_STATUS_ERR_MALFORMED_XML if there is no element at this position or
    the XML is malformed.
*/
static mre_status_t mre_require_element(
  char const        **ppDoc,
  mre_xml_element_t **ppElem,
  mre_xml_root_t     *pRoot
)
{
  mre_status_t status = mre_optional_element(ppDoc, ppElem, pRoot);

  return (status == MRE_STATUS_SUCCESS) ? MRE_STATUS_SUCCESS
                                        : MRE_STATUS_ERR_MALFORMED_XML;
}






/*=============================================================================
===============================================================================

  Public APIs.

  Documentation for these is in mre_xml.h

===============================================================================
=============================================================================*/



/*-------- mre_xml_parse --------*/
mre_status_t mre_xml_parse(
  char            const *pDocument,
  mre_xml_root_t  const **ppRoot
)
{
  mre_status_t     status;
  char const     **ppDoc  = &pDocument;
  mre_xml_root_t  *pRoot  = mre_root_element_new();
  static char      bom[]  = {0xef, 0xbb, 0xbf, 0};

  *ppRoot = pRoot;

  MRE_XML_MSG("###################################################", 0, 0, 0);
  MRE_UT_MSG_0("Beginning XML parsing");

  /*  Optionally consume a UTF-8 BOM.
   */
  if (mre_consume(ppDoc, bom))
  {
    MRE_UT_MSG_0("Skipped BOM at start of XML");
  }

  /*  Get the XML information and validate it.
   */
  status = mre_require_processing(ppDoc, &pRoot->pHeader, pRoot);
  DONE_IF(status != MRE_STATUS_SUCCESS);
  {
    char const  *pStr;

    /*  This must be an "xml" tag.
     */
    pStr = mre_xml_get_tag(pRoot->pHeader);
    UT_VALIDATE((pStr != NULL && strcmp(pStr, "xml") == 0),
                "File must start with an \"?xml\" element" );

    MALFORMED_IF(pStr == NULL || strcmp(pStr, "xml") != 0);

    /*  Version must be "1.0".
     */
    pStr = mre_xml_get_attribute(pRoot->pHeader, "version");
    UT_VALIDATE((pStr != NULL && strcmp(pStr, "1.0") == 0),
                "XML version must be 1.0" );
    MALFORMED_IF(pStr == NULL || strcmp(pStr, "1.0") != 0);

    /*  The encoding must be something we can handle as ASCII.
     */
    pStr = mre_xml_get_attribute(pRoot->pHeader, "encoding");
    UT_VALIDATE((pStr != NULL && (strcmp(pStr, "UTF-8") == 0 || strcmp(pStr, "ISO-8859-1") == 0)),
                "encoding must be UTF-8 or ISO-8859-1" );
    MALFORMED_IF(pStr == NULL || (strcmp(pStr, "UTF-8") != 0 && strcmp(pStr, "ISO-8859-1") != 0));
  }

  /*  Consume any comments that preceed the actual content of the document.
   */
  while (mre_consume_comment(ppDoc, pRoot))
  {
    /* nothing */
  }

  /*  Get the element that is the document.
   */
  status = mre_require_element(ppDoc, &pRoot->pDocument, pRoot);

Done:
  if (status != MRE_STATUS_SUCCESS && *ppRoot != NULL)
  {
    ref_cnt_obj_release((void *) *ppRoot);
    *ppRoot = NULL;
  }

  {
    MRE_UT_MSG_S1("XML parsing %s", (MRE_SUCCEEDED(status) ? "succeeded" : "FAILED"));
    MRE_XML_MSG("###################################################", 0, 0, 0);
  }

  return status;
}


/*-------- mre_xml_scan_for_xml_tag --------*/
static boolean mre_xml_scan_for_xml_tag(
  char const  *pDocument,
  char const **ppXml
)
{
  while ('\0' != *pDocument)
  {
    if (strncmp(pDocument, "<?xml", 5) == 0)
    {
      *ppXml = pDocument;
      return TRUE;
    }

    ++pDocument;
  }

  return FALSE;
}


/*-------- mre_xml_parse_from_text --------*/
mre_status_t mre_xml_parse_from_text(
  const char            *pDocument,
  const mre_xml_root_t **ppRoot
)
{
  mre_status_t  status = MRE_STATUS_ERR_NOT_PRESENT;
  char const    *pXml;

  if (mre_xml_scan_for_xml_tag(pDocument, &pXml))
  {
    status = mre_xml_parse(pXml, ppRoot);
  }

  return status;
}


/*-------- mre_xml_get_root_element --------*/
const mre_xml_element_t * mre_xml_get_root_element(
  mre_xml_root_t const  *pRoot
)
{
  return pRoot->pDocument;
}



/*-------- mre_xml_get_tag --------*/
char const * mre_xml_get_tag(
  mre_xml_element_t const *pElement
)
{
  if (pElement == NULL)
  {
    MRE_ERR_FATAL("NULL element passed to mre_xml_get_tag", 0, 0, 0);
  }

  return pElement->pTag;
}



/*-------- mre_xml_tag_is --------*/
boolean mre_xml_tag_is(
  mre_xml_element_t const *pElement,
  char              const *pStr
)
{
  return    pElement != NULL
         && pElement->pTag != NULL
         && pStr != NULL
         && strcmp(pElement->pTag, pStr) == 0
         ;
}


/*-------- mre_cmp_attributes --------*/
static boolean mre_cmp_attributes(
  void  *pData1,
  void  *pData2
)
{
  mre_xml_attribute *pAttr = (mre_xml_attribute *) pData1;
  char const        *pName = (char const *) pData2;

  return strcmp(pName, pAttr->pName) == 0;
}


/*-------- mre_xml_get_line_number --------*/
size_t  mre_xml_get_line_number(
  const mre_xml_element_t *pElement
)
{
  return pElement->lineNum;
}


/*-------- mre_xml_get_attribute --------*/
char const * mre_xml_get_attribute(
  const mre_xml_element_t *pElement,
  char const              *pName
)
{
  mre_xml_attribute *pAttr;
  char const        *pValue = NULL;

  if (pName == NULL || pElement == NULL || pElement->pAttributes == NULL)
  {
    goto Done;
  }

  /*  Look up the attribute in this element - if not present, return NULL
   */
  pAttr = mre_set_find(pElement->pAttributes, mre_cmp_attributes, (void *) pName);
  if (pAttr == NULL)
  {
    goto Done;
  }

  pValue = pAttr->pValue;

Done:
  return pValue;
}



/*-------- mre_xml_get_text --------*/
char const * mre_xml_get_text(
  mre_xml_element_t const *pElement
)
{
  if (pElement == NULL)
  {
    MRE_ERR_FATAL("NULL element passed to mre_xml_get_text", 0, 0, 0);
  }

  return pElement->pText;
}



/*-------- mre_xml_num_children --------*/
size_t mre_xml_num_children(
  mre_xml_element_t const *pElement
)
{
  return (pElement == NULL || pElement->pChildren == NULL) ? 0
                                                           : mre_set_num_elems(pElement->pChildren);
}


/*-------- mre_xml_get_child --------*/
const mre_xml_element_t * mre_xml_get_child(
  mre_xml_element_t const *pParent,
  size_t                   nChild
)
{
  mre_xml_element_t *pChild = NULL;

  if (
        pParent == NULL
     || pParent->pChildren == NULL
     || nChild >= mre_set_num_elems(pParent->pChildren)
     )
  {
    goto Done;
  }

  pChild = *((mre_xml_element_t **) mre_set_elem_ptr(pParent->pChildren, nChild));

Done:
  return pChild;
}

/*-------- mre_xml_process_children --------*/
mre_status_t  mre_xml_process_children(
  mre_xml_element_t const    *pParent,
  mre_xml_process_child_fn_t  pfn,
  void                       *pCtx
)
{
  mre_status_t       status = MRE_STATUS_SUCCESS;
  size_t             childNum;
  size_t             numChildren;

  numChildren = mre_xml_num_children(pParent);
  for (childNum = 0 ; MRE_SUCCEEDED(status) && childNum < numChildren ; ++childNum)
  {
    mre_xml_element_t const *pChild;

    pChild = mre_xml_get_child(pParent, childNum);
    status = pfn(pChild, pCtx);
  }

  return status;
}


/*-------- mre_xml_get_text_from_child --------*/
char const *mre_xml_get_text_from_child(
  mre_xml_element_t const *pElem,
  char const              *pTag
)
{
  size_t            numChildren;
  size_t            nChild;

  numChildren = mre_xml_num_children(pElem);
  for (nChild = 0 ; nChild < numChildren ; ++nChild)
  {
    mre_xml_element_t const *pChild;

    pChild = mre_xml_get_child(pElem, nChild);
    if (NULL != pChild)
    {
      char const *pStr;

      pStr = mre_xml_get_tag(pChild);
      if (strcmp(pTag, pStr) == 0)
      {
        return mre_xml_get_text(pChild);
      }
    }
  }

  return NULL;
}
