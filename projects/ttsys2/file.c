#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "file.h"


struct TFile
{
  TMemPool    *pool;
  int          filedes;
  char        *fname;
  int          flags;
};

static int FileCleanup(void *file_)
{
  TFile  *file = (TFile *)file_;
  int rv = TTS_SUCCESS;
  
  if (close(file->filedes) == 0)
  {
    file->filedes = -1;
    if (file->flags & TFILE_CLEANUP)
    {
      unlink(file->fname);
    }
  }
  else
  {
    rv = errno;
  }

  return rv;
}

int TmpFileOpen(TFile **file,const char *base,const char *path, int flag,TMemPool *pool)
{
  TFile      *new;
  int         oflags;
  static int  cnt = 0;
  
  if (flag == O_RDONLY)
  {
    oflags = O_RDONLY;
  }
  else if (flag & O_RDWR) 
  {
    oflags = O_RDWR;
  }
  else if (flag & O_WRONLY) 
  {
    oflags = O_WRONLY;
  }
  else 
  {
    return TTS_EACCES; 
  }

  if (flag & O_CREAT) 
  {
    oflags |= O_CREAT;

    if (flag & O_EXCL)
    {
      oflags |= O_EXCL;
    }
  }
    
  if ((flag & O_EXCL) && !(flag & O_CREAT)) 
  {
    return TTS_EACCES;
  }   

  if (flag & O_APPEND) 
  {
    oflags |= O_APPEND;
  }
  
  if (flag & O_TRUNC) 
  {
    oflags |= O_TRUNC;
  }

  new = (TFile *)MemPoolAllocEx(pool, sizeof(TFile));
  new->flags = flag;
  new->fname = (char *)m_sprintf(pool,"%s/%s.%d.%d.tmp",path,base,getpid(),++cnt);

  new->filedes = open(new->fname, oflags, 0600);
  if(new->filedes < 0)
  {
    return errno;
  }
  
  if ( flag & TFILE_CLEANUP )
  {
    MemPoolRegisterCleanup(pool,(void *)new,FileCleanup);
  }

  *file = new;

  return TTS_SUCCESS;
}

int FileOpen(TFile **file,const char *fname, int flag,TMemPool *pool)
{
  int     fd;
  TFile  *new;
  int     oflags;

  if (flag == O_RDONLY)
  {
    oflags = O_RDONLY;
  }
  else if (flag & O_RDWR) 
  {
    oflags = O_RDWR;
  }
  else if (flag & O_WRONLY) 
  {
    oflags = O_WRONLY;
  }
  else 
  {
    return TTS_EACCES; 
  }

  if (flag & O_CREAT) 
  {
    oflags |= O_CREAT;

    if (flag & O_EXCL)
    {
      oflags |= O_EXCL;
    }
  }
    
  if ((flag & O_EXCL) && !(flag & O_CREAT)) 
  {
    return TTS_EACCES;
  }   

  if (flag & O_APPEND) 
  {
    oflags |= O_APPEND;
  }
  
  if (flag & O_TRUNC) 
  {
    oflags |= O_TRUNC;
  }

  fd = open(fname, oflags, 0600);
  if(fd < 0)
  {
    return errno;
  }

  new = (TFile *)MemPoolAllocEx(pool, sizeof(TFile));
  new->filedes = fd;
  new->flags = flag;
  new->fname = (char*)m_strdup(pool, fname);
  
  if ( flag & TFILE_CLEANUP )
  {
    MemPoolRegisterCleanup(pool,(void *)new,FileCleanup);
  }

  *file = new;

  return TTS_SUCCESS;
}

int FileClose(TFile *file)
{
  return MemPoolRunCleanup(file->pool, file, FileCleanup);
}

int FileRemove(const char *path)
{
  if (unlink(path) == 0)
  {
    return TTS_SUCCESS;
  }
  else 
  {
    return errno;
  }
}

int FileRename(const char *from_path, const char *to_path)
{
  if (rename(from_path, to_path) != 0) 
  {
    return errno;
  }

  return TTS_SUCCESS;
}

int FileRead(TFile *thefile, void *buf, uint32 *nbytes)
{
  int  rv,bytes_read;

  bytes_read = 0;

  do
  {
    rv = read(thefile->filedes, buf, *nbytes);
  }
  while (rv == -1 && errno == EINTR);

  *nbytes = bytes_read;

  if ( rv == 0 )
  {
    return TTS_EOF;
  }
  else if ( rv > 0)
  {
    *nbytes += rv;
    return TTS_SUCCESS;
  }

  return errno;
  
}

int FileWrite(TFile *thefile, const void *buf, uint32 *nbytes)
{
  int  rv;

  do
  {
    rv = write(thefile->filedes, buf, *nbytes);
  }
  while (rv == -1 && errno == EINTR);

  if (rv == -1)
  {
    *nbytes = 0;
    return errno;
  }
  
  *nbytes = rv;

  return TTS_SUCCESS;
}

int FileWriteEx(TFile *thefile, const char *buf)
{
  int    rv;
  int    nbytes;

  nbytes = strlen(buf);

  do
  {
    rv = write(thefile->filedes, buf, nbytes);
  }
  while (rv == -1 && errno == EINTR);

  if (rv == -1)
  {
    return errno;
  }
  
  return TTS_SUCCESS;
}
