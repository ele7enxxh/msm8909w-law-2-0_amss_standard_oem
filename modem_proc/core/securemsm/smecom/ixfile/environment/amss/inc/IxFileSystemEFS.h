#ifndef __IXFILESYSTEMEFS_H__
#define __IXFILESYSTEMEFS_H__

#include "IxFileSystem.h"
#include "IxFileTypes.h"

typedef struct
{
  char    path[IXFILE_NAME_MAX+1];
  EFSDIR *dirHandle;
} IxFileMgrDirInfoT;

class IxFileSystemEfs : public IxFileSystem
{
public:
    IxFileSystemEfs();
    virtual ~IxFileSystemEfs();
    virtual int Open( const char *path, int flag, ... );
    virtual int Close( int filedes );
    virtual signed long int Read( int filedes, void *buf, unsigned long int nbyte );
    virtual signed long int Write( int filedes, const void *buf, unsigned long int nbyte );
    virtual int Access( const char *path, int amode );
    virtual int MkDir( const char *path, unsigned short mode );
    virtual int Stat( const char *path, IxFileStatType *buf );
    virtual int FStat( int fildes, IxFileStatType *buf );
    virtual int LStat( const char *path, IxFileStatType *buf );
    virtual int StatVfs( const char *path, IxFileStatFSType *buf );
    virtual int FStatVfs( int fildes, IxFileStatFSType *buf );
    virtual signed long int LSeek( int filedes, signed long int offset, int whence );
    virtual signed long int Truncate( const char *path, signed long int length );
    virtual signed long int FTruncate( int fildes, signed long int length );
    virtual int Unlink( const char *path );
    virtual unsigned long int OpenDir( const char *dirname );
    virtual int ReadDir( unsigned long int dirHandle, IxFileDirEntT *dirEnt );
    virtual int CloseDir( unsigned long int dirhandle );
    virtual int Rename( const char *oldpath, const char *newpath );
    virtual int RmDir( const char *path );
    virtual int ChMod( const char *path, unsigned short mode );
    virtual int SymLink( const char *oldpath, const char *newpath );
    virtual int ReadLink( const char *path, char *buf, unsigned long int bufsiz );
    virtual int Initialize( void );
    virtual int PowerDown( void );
    virtual int Mount( const char *special, const char *dir,
        const char *fstype, int flags, const void *data );
    virtual int Umount( const char *target );
    virtual int Remount( const char *oldtarget, const char *newtarget );
    virtual void Reset( void );
    virtual int ChOwn( const char *path, int uid_val, int gid_val );
    virtual int GetLastError( void );
private:
    uint32 __errno;
};


/*==============================================================
FUNCTION:  IxFile_map_efs_to_ix_error_codes

DESCRIPTION:
   Maps EFS2 error codes to IxFile error codes.
==============================================================*/
uint32 IxFile_map_efs_to_ix_error_codes (int efs_err);

/*==============================================================
FUNCTION:  IxFileConvertFileType

DESCRIPTION:
   Maps EFS2 file types to IxFile file types.
==============================================================*/
int IxFileConvertFileType (int efsFileType);

/*==============================================================
FUNCTION:  IxFile_convert_mode

DESCRIPTION:
   Maps EFS2 mode to IxFile mode.
==============================================================*/
int IxFileConvertMode (IxFileOpenFileMode mode);




extern IxFileSystemEfs g_fs_efs;

#endif /* __IXFILESYSTEMEFS_H__ */
