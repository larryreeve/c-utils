/*  lbipcmsg.cpp
 *
 *  IPC Message object
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
 *      08/09/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <lbport.h>
#include <lbipcmsg.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/

/*
 *
**/
LB_IpcMsg_T::LB_IpcMsg_T()
  {
  m_ID = IPCMSG_SYS_NONE;
#ifdef LIB_MTC
  m_ReqExch = (THREAD *) NULL;
  m_RspExch = (THREAD *) NULL;
#endif
  m_Data.Clear();
  };

#ifdef LIB_MTC
/*
 *
**/
LB_IpcMsg_T::SetMsgParams(LB_Word_T       pa_ID,
                          THREAD *        pa_ReqExch;
                          THREAD *        pa_RspExch;
                          LB_String_T&    pa_Data)
  {
  //--- Assign initial class values
  m_ID      = IPCMSG_SYS_NONE;
  m_ReqExch = (THREAD *) NULL;
  m_RspExch = (THREAD *) NULL;
  m_Data.Clear();

  //--- Validate parameter values
  switch(pa_ID)
    {
    case IPCMSG_SYS_LOGON:
    case IPCMSG_SYS_FORCEDLOGON:
    case IPCMSG_SYS_LOGOFF:
    case IPCMSG_CON_LOGMSG:
    case IPCMSG_CON_FILEON:
    case IPCMSG_CON_FILEOFF:
        m_ID = pa_ID;
        break;

    default:
        m_ID = IPCMSG_SYS_NONE;
    }

  if (pa_ReqExch != ((THREAD *) NULL)
    m_ReqExch = pa_ReqExch;

  if (pa_ReqExch != ((THREAD *) NULL)
    m_RspExch = pa_RspExch;

  m_Data = pa_Data;
  }

                          
/*
 *
**/
LB_IpcMsg_T::LB_IpcMsg_T(LB_Word_T       pa_ID,
                         THREAD *        pa_ReqExch;
                         THREAD *        pa_RspExch;
                         LB_String_T&    pa_Data);
  {
  SetMsgParams(pa_ID, pa_ReqExch, pa_RspExch, pa_Data);
  };


/*
 *
**/
LB_IpcMsg_T::~LB_IpcMsg_T()
  {
  m_ID      = IPCMSG_SYS_NONE;
  m_ReqExch = (THREAD *) NULL;
  m_RspExch = (THREAD *) NULL;
  m_Data.Clear();
  };
#endif
