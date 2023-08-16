/*  lbfio.cpp
 *
 *  Low-level File Input/Output routines
 *
 *
 *                       * PROPRIETARY PROGRAM MATERIAL *
 *
 *      NOTICE: THIS MATERIAL IS PROPRIETARY TO UNITED COMPUTER
 *      AND IS NOT TO BE REPRODUCED, USED OR DISCLOSED EXCEPT
 *      UPON WRITTEN AUTHORIZATION OF:
 *
 *                      UNITED COMPUTER SALES & SERVICE, INC.
 *                      100 DOBBS LANE, SUITE 208
 *                      CHERRY HILL, NEW JERSEY 08034
 *                      609.795.7330
 *
 *            COPYRIGHT (C) 1995-1996 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      12/01/95        LR      -Initial Release
 *      02/06/96        LR      -Added information (GetAsString) methods
 *      02/15/96        LR      -Read/Write require strings as parameters
 *      03/14/96        LR      -Added ansi f_ file methods for buffering
 *      03/26/96        LR      -Implemented readline function
 *      12/07/96        LR      -Removed automatic Flush after every write;
 *                               application should call Flush
 *                              -Fixed Close bug; the file handle was being
 *                               closed before the stream handle and any
 *                               contents in the stream buffer were lost.
 *      03/22/97        LR      -Renamed Delete method to Remove
 *      04/23/97        LR      -Updates for VC++
 *      06/22/97        LR      -See Visual SourceSafe History
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <ctype.h>
#include <dos.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <memory.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __TURBOC__
  #include <locking.h>
  #include <stat.h>
#endif

#ifdef _MSC_VER
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/locking.h>
#endif

#include <lbport.h>
#include <lbdebug.hpp>
#include <lbfio.hpp>


//---------------------------------------------------------------
//---                   Private Constants                     ---
//---------------------------------------------------------------
LB_ASSERTFILE(__FILE__)


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
int                     LB_Fio_T::m_GetClusterSize()
{
    #ifdef __TURBOC__
    struct fatinfo FATInfo;

    memset(&FATInfo,0,sizeof(FATInfo));
    getfatd(&FATInfo);
    return (FATInfo.fi_bysec * FATInfo.fi_sclus);
    #else
        return (4096);
    #endif


}


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
LB_Fio_T::LB_Fio_T()
{
    m_FileHandle   = (-1);
    m_FileOpenFlag = LB_FALSE;
    memset(m_FileName,'\0',sizeof(m_FileName));
}


LB_Fio_T::~LB_Fio_T()
{
    Close();
    memset(m_FileName,'\0',sizeof(m_FileName));
}


void                    LB_Fio_T::Close()
{
    if (m_FileOpenFlag == LB_TRUE)
    {
        Unlock();
        if (m_FileHandle != (-1))    
            close(m_FileHandle);
        m_FileHandle = (-1);
    }

    m_FileOpenFlag = LB_FALSE;
}


LB_Bool_T               LB_Fio_T::Create()
{
    Close();

    m_FileHandle = creat(m_FileName, S_IREAD | S_IWRITE);

    if (m_FileHandle == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "creat failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    close(m_FileHandle);
    return (LB_TRUE);
}


LB_DWord_T              LB_Fio_T::CurrentPosition()
{
    long FilePos;

    if (m_FileOpenFlag  == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "file %s is not open", m_FileName);
        return (0L);
    }

    FilePos = tell(m_FileHandle);

    if (FilePos == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "tell failed, reason %s", sys_errlist[errno]);
        return (0L);
    }
    else
        return ((LB_DWord_T) FilePos);
}


LB_Bool_T               LB_Fio_T::Exists()
{
    if (access(m_FileName, 0) != 0)
        return (LB_FALSE);
    else
        return (LB_TRUE);
}


void                    LB_Fio_T::Flush()
{
}


//--- Returns path name of file.
LB_Bool_T               LB_Fio_T::GetPathAsString(LB_String_T& pa_Buf)
{
    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    pa_Buf = m_FileName;

    return (LB_TRUE);
}


//--- Returns date of file.
LB_Bool_T               LB_Fio_T::GetDateAsString(LB_String_T& pa_Buf)
{
    #ifdef __TURBOC__
    struct stat FileStatus;
    #endif

    #ifdef _MSC_VER
    struct _stat FileStatus;
    #endif

    struct tm*  FileTime;
    char        DateStr[16];

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    #ifdef __TURBOC__
    if (stat(m_FileName,&FileStatus) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "stat failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }
    #endif

    #ifdef __MSC_VER__
    if (_stat(m_FileName,&FileStatus) != 0)
        LB_Debug_ErrorLog(__FILE__, __LINE__, "stat failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    #endif

    FileTime = localtime(&FileStatus.st_mtime);

    sprintf(DateStr,"%02d/%02d/%04d",
            FileTime->tm_mon+1,
            FileTime->tm_mday,
            FileTime->tm_year + 1900);

    pa_Buf = DateStr;

    return (LB_TRUE);
}


//--- Returns time of file.
LB_Bool_T               LB_Fio_T::GetTimeAsString(LB_String_T& pa_Buf)
{
    struct stat FileStatus;
    struct tm * FileTime;
    char        TimeStr[16];

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    if (stat(m_FileName,&FileStatus) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "stat failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    FileTime = localtime(&FileStatus.st_mtime);

    if (FileTime->tm_hour > 12)
    {
        FileTime->tm_hour = FileTime->tm_hour - 12;
        sprintf(TimeStr,"%02d:%02d pm",FileTime->tm_hour,FileTime->tm_min);
    }
    else
        sprintf(TimeStr,"%02d:%02d am",FileTime->tm_hour,FileTime->tm_min);

    pa_Buf = TimeStr;

    return (LB_TRUE);
}


//--- Returns size of file.
LB_Bool_T               LB_Fio_T::GetSizeAsString(LB_String_T& pa_Buf)
{
    struct stat FileStatus;
    char        SizeStr[16];

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    if (stat(m_FileName,&FileStatus) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "stat failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    sprintf(SizeStr,"%lu",FileStatus.st_size);

    pa_Buf = SizeStr;

    return (LB_TRUE);
}


LB_Bool_T               LB_Fio_T::IsEOF()
{
    if (m_FileOpenFlag == LB_FALSE)
        return (LB_TRUE);

    if (eof(m_FileHandle) == 1)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               LB_Fio_T::IsOpen()
{
    return (m_FileOpenFlag);
}


LB_Bool_T               LB_Fio_T::Lock()
{
    int Erc;

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    #ifdef __TURBOC__
    Erc = lock(m_FileHandle, 0L, Size());
    #endif

    #ifdef _MSC_VER
    if (lseek(m_FileHandle, 0L, SEEK_SET) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "lseek failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    Erc = _locking(m_FileHandle, _LK_NBLCK, Size());
    #endif

    if (Erc == 0)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               LB_Fio_T::LockRange(LB_DWord_T pa_Offset,
                                            LB_DWord_T pa_Length)
{
    int Erc;

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    #ifdef __TURBOC__
    Erc = lock(m_FileHandle,pa_Offset,pa_Length);
    #endif

    #ifdef _MSC_VER
    if (lseek(m_FileHandle, pa_Offset, SEEK_SET) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "lseek failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    Erc = _locking(m_FileHandle, _LK_NBLCK, pa_Length);
    #endif


    if (Erc == 0)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               LB_Fio_T::Open(LB_Word_T pa_OpenMode)
{
    LB_Word_T     OpenMode;
    LB_Word_T     ShareMode;
    LB_Bool_T     BufferedFlag;

    Close();

    //-- check if file exists
    if (Exists() == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "File %s does not exist.", m_FileName);
        return (LB_FALSE);
    }

    BufferedFlag = LB_TRUE;

    //--- Determine open & share mode, set buffering flag
    switch(pa_OpenMode)
    {
        case FIO_OM_RO_BUF:
        case FIO_OM_RO_NOBUF:
            OpenMode  = O_RDONLY | O_BINARY;
            ShareMode = SH_DENYNO;
            if (pa_OpenMode == FIO_OM_RO_NOBUF)
                BufferedFlag = LB_FALSE;
            break;

        case FIO_OM_WO_BUF:
        case FIO_OM_WO_NOBUF:
            OpenMode  = O_WRONLY | O_BINARY;
            ShareMode = SH_DENYNO;
            if (pa_OpenMode == FIO_OM_WO_NOBUF)
                BufferedFlag = LB_FALSE;
            break;

        case FIO_OM_RW_BUF:
        case FIO_OM_RW_NOBUF:
            OpenMode  = O_RDWR | O_BINARY;
            ShareMode = SH_DENYNO;
            if (pa_OpenMode == FIO_OM_RW_NOBUF)
                BufferedFlag = LB_FALSE;
            break;

        case FIO_OM_EXCLUSIVE:
            OpenMode  = O_RDWR | O_BINARY;
            ShareMode = SH_DENYRW;
            break;

        default:
            OpenMode  = O_RDWR | O_BINARY;
            ShareMode = SH_DENYNO;
    }

    m_FileHandle = open(m_FileName, OpenMode | ShareMode, S_IREAD | S_IWRITE);

    //--- Test if file open failed
    if (m_FileHandle == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "File %s open failed. Reason: %s", m_FileName, sys_errlist[errno]);
        Close();
        return (LB_FALSE);
    }

    if (BufferedFlag == LB_TRUE)
    {
    }

    m_FileOpenFlag = LB_TRUE;

    return (LB_TRUE);
}


//--- Will read up to the string size bytes
LB_Word_T               LB_Fio_T::Read(LB_String_T& pa_String)
{
    int BytesRead;

    if (m_FileOpenFlag == LB_FALSE)
        return (0);

    BytesRead = read(m_FileHandle, &pa_String[0], pa_String.Size());

    if (BytesRead == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "read failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    return ((LB_Word_T) BytesRead);
}


LB_Word_T               LB_Fio_T::ReadLine(LB_String_T& pa_String)
{
    LB_Word_T     CntCharsRead;
    LB_Word_T     CntCtlRead;
    LB_Bool_T     FoundEol;
    long          FilePos;
    int           BytesRead;

    if (m_FileOpenFlag == LB_FALSE)
        return (0);

    FilePos = tell(m_FileHandle);

    BytesRead = read(m_FileHandle, &pa_String[0], pa_String.Size());

    //Trace3("lbfio: read %d bytes out of buffer max of %d at filepos %ld", BytesRead, pa_String.Size(), FilePos);

    if (BytesRead == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "read failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    FoundEol     = LB_FALSE;
    CntCharsRead = 0;
    CntCtlRead   = 0;

    for (int Idx=0; Idx < BytesRead; Idx++)
    {
        if (pa_String[Idx] == '\r')
        {
            CntCtlRead = 1;
            FoundEol = LB_TRUE;

            if ((Idx+1) < BytesRead)
            {
                if (pa_String[Idx+1] == '\n')
                   CntCtlRead = 2;
            }

            break;
        }
        else if (pa_String[Idx] == '\n')
        {
            CntCtlRead = 1;
            FoundEol = LB_TRUE;
            break;
        }
        else
        {
            CntCharsRead++;
        }
    }

    //Trace2("lbfio: newline found after %d chars (%d CntlChars)", CntCharsRead, CntCtlRead);

    //--- Handle case if last line in file is not CR/LF terminated
    if (FoundEol == LB_FALSE)
        pa_String.TrimTo(BytesRead);
    else
        pa_String.TrimTo(CntCharsRead);

    FilePos = FilePos + pa_String.Size() + CntCtlRead;

    IFTRACE(pa_String.Trace(__FILE__, __LINE__));
    //Trace1("lbfio: adjusted size of buffer = %d", pa_String.Size());
    //Trace2("lbfio: new filepos = %ld, size of file=%lu", FilePos, Size());
    //Trace1("lbfio: eof indicator before lseek = %d", eof(m_FileHandle));

    if (lseek(m_FileHandle, FilePos, SEEK_SET) == (-1))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "lseek failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    //Trace1("lbfio: eof indicator after lseek = %d", eof(m_FileHandle));

    return (pa_String.Size());
}


LB_Bool_T               LB_Fio_T::Remove()
{
    Close();

    if (remove(m_FileName) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "remove failed, last errstr=%s", strerror(errno));
        return (LB_FALSE);
    }
    else
    {
        return (LB_TRUE);
    }
}



LB_Bool_T               LB_Fio_T::SeekFromCurrent(LB_DWord_T pa_Offset)
{
    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    if (lseek(m_FileHandle, pa_Offset, SEEK_CUR) == (-1L))
        return (LB_FALSE);
    else
        return (LB_TRUE);
}


LB_Bool_T               LB_Fio_T::SeekFromEOF(LB_DWord_T pa_Offset)
{
    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    if (lseek(m_FileHandle, pa_Offset, SEEK_END) == (-1L))
        return (LB_FALSE);
    else
        return (LB_TRUE);
}


LB_Bool_T               LB_Fio_T::SeekFromStart(LB_DWord_T pa_Offset)
{
    int   Erc;
    long  CurPos;
    long  OffsetCnt;

    if (m_FileOpenFlag == LB_FALSE)
        return (LB_FALSE);

    //CurPos = tell(m_FileHandle);

    //if (CurPos == pa_Offset)
    //   return (LB_TRUE);

    //if (pa_Offset > (LB_DWord_T) CurPos)
    //{
    //    OffsetCnt = (long) (pa_Offset - (LB_DWord_T) CurPos);
    //    Erc = lseek(m_FileHandle, OffsetCnt, SEEK_CUR);
    //}
    //else
    //{
        Erc = lseek(m_FileHandle, pa_Offset, SEEK_SET);
    //}

    if (Erc != 0)
        return (LB_FALSE);

    return (LB_TRUE);
}


void                    LB_Fio_T::SetFileName(LB_String_T& pa_FileName)
{
    memset(m_FileName, 0, sizeof(m_FileName));
    pa_FileName.AsAsciiZ(m_FileName, sizeof(m_FileName));
}


void                    LB_Fio_T::SetFileName(char * pa_FileName)
{
    memset(m_FileName, 0, sizeof(m_FileName));

    if (strlen(pa_FileName) < (sizeof(m_FileName) - 1))
        strcpy(m_FileName, pa_FileName);
}


LB_DWord_T              LB_Fio_T::Size()
{
    struct stat   FileStatus;

    if (m_FileOpenFlag == LB_FALSE)
        return (0);

    if (stat(m_FileName, &FileStatus) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "stat failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    return (FileStatus.st_size);
}


LB_Bool_T               LB_Fio_T::Unlock()
{
    int Erc;

    if (IsOpen() == LB_FALSE)
        return (LB_FALSE);

    #ifdef __TURBOC__
    Erc = unlock(m_FileHandle, 0L, Size());
    #endif

    #ifdef _MSC_VER
    if (lseek(m_FileHandle, 0L, SEEK_SET) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "lseek failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    Erc = _locking(m_FileHandle, _LK_UNLCK, Size());
    #endif

    if (Erc == 0)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               LB_Fio_T::UnlockRange(LB_DWord_T pa_Offset, 
                                              LB_DWord_T pa_Length)
{
    int Erc;

    if (IsOpen() == LB_FALSE)
        return (LB_FALSE);

    #ifdef __TURBOC__
    Erc = unlock(m_FileHandle, pa_Offset, pa_Length);
    #endif

    #ifdef _MSC_VER
    if (lseek(m_FileHandle, pa_Offset, SEEK_SET) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "lseek failed, reason %s", sys_errlist[errno]);
        return (LB_FALSE);
    }

    Erc = _locking(m_FileHandle, _LK_UNLCK, pa_Length);
    #endif

    if (Erc == 0)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


//--- Will write up to the string size bytes
LB_Word_T               LB_Fio_T::Write(LB_String_T& pa_String)
{
    int BytesWritten;

    if (m_FileOpenFlag == LB_FALSE)
        return (0);

    BytesWritten = write(m_FileHandle, &pa_String[0], pa_String.Size());

    if ((BytesWritten == (-1)) ||
        (BytesWritten != pa_String.Size()))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "write failed, reason %s", sys_errlist[errno]);
        return (0);
    }
    else
        return ((LB_Word_T) BytesWritten);
}


LB_Word_T               LB_Fio_T::WriteLine(LB_String_T& pa_String)
{
    int BytesWritten;
    static char NewLine[] = "\r\n";

    BytesWritten = write(m_FileHandle, &pa_String[0], pa_String.Size());

    if ((BytesWritten == (-1)) ||
        (BytesWritten != pa_String.Size()))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "write failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    BytesWritten = write(m_FileHandle, NewLine, 2);

    if ((BytesWritten == (-1)) ||
        (BytesWritten != 2))
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "write failed, reason %s", sys_errlist[errno]);
        return (0);
    }

    return (pa_String.Size());
}
