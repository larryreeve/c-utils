#ifndef  LB_CONSOLE_HPP
#define  LB_CONSOLE_HPP

#include <stdio.h>

#include <lbport.h>
#include <lbstring.hpp>

class LB_Console_T
  {
  public:
    LB_Console_T();

    ~LB_Console_T();

    void                Log(LB_String_T& pa_LogMsg);

    void                SetFileLoggingOff();

    void                SetFileLoggingOn();

    LB_Bool_T           StartUp(void);

    void                Write(LB_ZString_T pa_Format, ...);

  private:
    FILE*               m_LogFh;
    LB_Bool_T           m_LogOn;
  };
#endif
