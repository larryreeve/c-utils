/*  lbtime.cpp
 *
 *  Time Processing routines
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
 *      02/21/95        LR      -Added GetSortTimeAsString
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <lbport.h>
#include <lbtime.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_Bool_T               LB_Time_T::IsNothing()
  {
  if ((m_Hour   == 0)   &&
      (m_Minute == 0)   &&
      (m_Second == 0))
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


/*
 *
**/
LB_Word_T         	LB_Time_T::GetHour()
  {
  return (m_Hour);
  }


/*
 *
**/
LB_Word_T         	LB_Time_T::GetMinute()
  {
  return (m_Minute);
  }


/*
 *
**/
LB_Word_T         	LB_Time_T::GetSecond()
  {
  return (m_Second);
  }


/*
 *
**/
LB_Word_T         	LB_Time_T::GetMillisecond()
  {
  return (m_Millisecond);
  }


/*
 *
 * Returns time as hhmmssmm
 *
**/
void                    LB_Time_T::GetSortTimeLong(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d%02d%02d%02d", GetHour(), GetMinute(),
                                    GetSecond(), GetMillisecond());

  pa_String = l_Buf;
  }


/*
 *
 * Returns time as hhmm
 *
**/
void                    LB_Time_T::GetSortTimeShort(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d%02d", GetHour(), GetMinute());

  pa_String = l_Buf;
  }


/*
 *
**/
LB_Bool_T 		LB_Time_T::SetTo(LB_Word_T pa_Hour,
					 LB_Word_T pa_Minute,
					 LB_Word_T pa_Second,
					 LB_Word_T pa_Millisecond)
  {
  m_Hour        = pa_Hour;
  m_Minute      = pa_Minute;
  m_Second      = pa_Second;
  m_Millisecond = pa_Millisecond;

  return (LB_TRUE);
  }


/*
 *
**/
void    		LB_Time_T::SetToNothing(void)
  {
  m_Hour        = 0;
  m_Minute      = 0;
  m_Second      = 0;
  m_Millisecond = 0;
  }


/*
 *
**/
LB_Bool_T 		LB_Time_T::SetToNow(void)
  {
  time_t        timesecs;
  struct tm     *tmblock;

  timesecs = time((time_t *) NULL);

  tmblock  = localtime(&timesecs);

  m_Hour   = tmblock->tm_hour;
  m_Minute = tmblock->tm_min;
  m_Second = tmblock->tm_sec;

  return (LB_TRUE);
  }
