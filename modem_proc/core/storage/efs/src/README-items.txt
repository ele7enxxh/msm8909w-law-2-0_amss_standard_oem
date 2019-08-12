Discussion of "Item" files.

* Introduction

"Item" files are a special type of file allowing efficient storage of small
amounts of information.  They are designed to alleviate the need for a
resource database, or the existing NV item manager.  These item files
appear in the ordinary directory heirarchy, and appear as limited-use
files.

There are some tradeoffs to their implementation, and as-such, it is
subject to future change.

* Item file semantics.

Item files are intended to be read and written atomically using the efs_get
and efs_put calls.  However, because of the architecture of EFS2, these
items can also be manipulated using ordinary efs_open, efs_close, efs_read,
efs_write, and a few other calls.  The item's behavior under these calls is
slightly different than ordinary files, but is designed to mimic what
ordinary files would do in restrictive conditions.

  - efs_open:  Using O_ITEMFILE indicates to efs_open that this file should
    be opened as an item file.  Combinations of other attributes that make
    sense are as follows:

       - O_WRONLY | O_CREAT | O_TRUNC   (optionally O_EXCL).
       - O_RDONLY

    Semantically, O_TRUNC has no effect on the item file (see the effect of
    efs_write below).

    efs_open on item files _never_ modifies the state of the filesystem.
    Only the efs_write call performs the change.  This allows these
    files to be updated atomically, even if efs_put is not used.

  - efs_read: behaves as a normal file would.

  - efs_write:  It is only permissible to write the file from the
    beginning, and in its entirety.  Other uses will cause write to return
    EINVAL.  This write will replace the entire contents of the file with
    the new block of data.  This indicates an implied truncation, which is
    different than ordinary files.

  - efs_close: closes the file descriptor as normal.

The typical cases for item files are to open them with O_RDONLY (O_ITEMFILE
is not necessary unless O_CREAT is specified), read the data with a
single read call, and close the file.  To write them use O_ITEMFILE |
O_WRONLY | O_CREAT | O_TRUNC and optionally O_EXCL.  Write the contents of
the item using a single call to efs_write, and then close the file.

* File attributes.

Currently, item files contain the following attributes that will show up in
efs_stat:

  mode, uid, gid, attr

In the future, we may implement the ability for items to inherit these
attributes from the containing directory.  uid/gid and mode are used to
determine access permissions of the item.  Attr contains the extended
attributes, including the various immutability protections.
