#ifndef  LB_CONREQ_HPP
#define  LB_CONREQ_HPP

#include <stdio.h>

#include <lbport.h>
#include <lbipcmsg.hpp>
#include <lbstring.hpp>

#ifdef LIB_MTC
#include <mtc.h>
#endif


class LB_ConsoleReq_T
  {
  public:
    LB_Console_T();

    ~LB_Console_T();

    void                Log(LB_String_T& pa_LogMsg);


  private:
    LB_IpcMsg_T         m_IpcMsg;
  };
#endif
