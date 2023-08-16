/*  lbconsol.cpp
 *
 *  Console routines
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
 *      08/03/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
	---------------------------------------------------------------*/
#include <memory.h>
#include <stdarg.h>
#include <time.h>
#include <lbport.h>
#include <lbstring.hpp>
#include <lbconsol.hpp>

#ifdef _Windows
#include <windows.h>
#endif


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/

/*
 *
**/
LB_Console_T::LB_Console_T()
  {
  m_LogFh = NULL;
  m_LogOn = LB_FALSE;
  };


/*
 *
**/
LB_Console_T::~LB_Console_T()
  {
  if (m_LogFh != NULL)
    fclose(m_LogFh);
	};



/*
 *
**/
LB_Bool_T               LB_Console_T::StartUp(void)
  {
  //--- Read BudSrv.ini to determine if logging to file
  //--- is enabled. It is under the section [Console].
	//--- The parameter is LoggingOn=Yes/No

  LB_String_T	Msg;

	Msg = "Console service started.";
	Log(Msg);

  return (LB_TRUE);
  }



/*
 *
**/
void                    LB_Console_T::Log(LB_String_T& pa_Msg)
  {
  struct tm     *TimeNow;
  time_t        SecsNow;
  char          TimeStr[80];
  char          MsgStr[256];

  //--- Retrieve current time
  tzset();
  time(&SecsNow);
  TimeNow = localtime(&SecsNow);

  //--- Format current time into ASCIIZ string
  strftime(TimeStr, 80, "(%m/%d/%Y %I:%M:%S %p)", TimeNow);

  //--- Get msg into ASCIIZ string
	memset(MsgStr,'\0',sizeof(MsgStr));
  pa_Msg.GetBytes(MsgStr,sizeof(MsgStr)-1);

  //--- Display message on console and optionally log to file
  printf("%s: %s\n",TimeStr,MsgStr);

  if (m_LogOn == LB_TRUE)
    {
    if (m_LogFh != NULL)
      fprintf(m_LogFh,"%s: %s\n",TimeStr,MsgStr);
    }
  }


/*
 *
**/
void                    LB_Console_T::Write(LB_ZString_T pa_Format, ...)
  {
  va_list ArgPtr;

  va_start(ArgPtr,pa_Format);
  vfprintf(stderr,pa_Format,ArgPtr);
  va_end(ArgPtr);
  }
