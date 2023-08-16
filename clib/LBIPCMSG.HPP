#ifndef  LB_IPCMSG_HPP
#define  LB_IPCMSG_HPP

#include <lbport.h>
#include <lbstring.hpp>

//--- Valid Message IDs
//-----* System
#define IPCMSG_SYS_NONE             0
#define IPCMSG_SYS_LOGON            1
#define IPCMSG_SYS_FORCELOGON       2
#define IPCMSG_SYS_LOGOFF           3
//-----* Console
#define IPCMSG_CON_LOGMSG         100
#define IPCMSG_CON_FILEON         101
#define IPCMSG_CON_FILEOFF        102
//-----* Fixed Assets


class LB_IpcMsg_T
  {
  public:
    LB_IpcMsg_T();

#ifdef LIB_MTC
    LB_IpcMsg_T(LB_Word_T       pa_ID,
                THREAD *        pa_ReqExch;
                THREAD *        pa_RspExch;
                LB_String_T&    pa_Data);

    SetMsgParams(LB_Word_T       pa_ID,
                THREAD *        pa_ReqExch;
                THREAD *        pa_RspExch;
                LB_String_T&    pa_Data);
#endif
    ~LB_IpcMsg_T();

    LB_Bool_T   IsValid();

  private:
    LB_Word_T           m_ID;
    LB_String_T         m_Data;
#ifdef LIB_MTC
    THREAD *            m_ReqExch;
    THREAD *            m_RspExch;
#endif
  };
#endif
