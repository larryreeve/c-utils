#ifndef  LB_FIO_HPP
#define  LB_FIO_HPP


#include <lbport.h>
#include <stdio.h>
#include <lbstring.hpp>


//--- Open Modes
#define FIO_OM_RO_BUF           0
#define FIO_OM_WO_BUF           1
#define FIO_OM_RW_BUF           2
#define FIO_OM_RO_NOBUF         10
#define FIO_OM_WO_NOBUF         11
#define FIO_OM_RW_NOBUF         12
#define FIO_OM_EXCLUSIVE        20

#define FIO_MAX_PATH            512


class LB_Fio_T
{
    public:
        LB_Fio_T();

        ~LB_Fio_T();

        void            Close();

        LB_Bool_T       Create();

        LB_DWord_T      CurrentPosition();

        LB_Bool_T       Exists();

        void            Flush();

        LB_Bool_T       GetPathAsString(LB_String_T& pa_Buf);

        LB_Bool_T       GetDateAsString(LB_String_T& pa_Buf);

        LB_Bool_T       GetTimeAsString(LB_String_T& pa_Buf);

        LB_Bool_T       GetSizeAsString(LB_String_T& pa_Buf);

        LB_Bool_T       IsEOF();

        LB_Bool_T       IsOpen();

        LB_Bool_T       Lock();

        LB_Bool_T       LockRange(LB_DWord_T pa_Offset, LB_DWord_T pa_Length);

        LB_Bool_T       Open(LB_Word_T pa_OpenMode);

        LB_Word_T       Read(LB_String_T& pa_String);

        LB_Word_T       ReadLine(LB_String_T& pa_String);

        LB_Bool_T       Remove();

        LB_Bool_T       SeekFromCurrent(LB_DWord_T pa_Offset);

        LB_Bool_T       SeekFromEOF(LB_DWord_T pa_Offset);

        LB_Bool_T       SeekFromStart(LB_DWord_T pa_Offset);

        void            SetFileName(LB_String_T&  pa_Filename);

        void            SetFileName(char * pa_Filename);

        LB_DWord_T      Size();

        LB_Bool_T       Unlock();

        LB_Bool_T       UnlockRange(LB_DWord_T pa_Offset, LB_DWord_T pa_Length);

        LB_Word_T       Write(LB_String_T& pa_String);

        LB_Word_T       WriteLine(LB_String_T& pa_String);


    private:
        //--- Variables
        int                 m_FileHandle;
        LB_Bool_T           m_FileOpenFlag;
        char                m_FileName[FIO_MAX_PATH];


        //--- Functions
        int                 m_GetClusterSize();
};

#endif
