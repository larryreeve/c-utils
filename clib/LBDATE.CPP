/*  lbdate.cpp
 *
 *  Date Processing routines
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
 *      11/01/95        LR      -Initial Release
 *      12/06/95        LR      -Added GetYearShort for no century info
 *                              -Added GetAsString
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <lbport.h>
#include <lbdate.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/

/*
 *
**/
LB_Date_T::LB_Date_T()
  {
  strcpy(m_ShortMonths[ 0], "Jan");  strcpy(m_ShortMonths[ 1], "Feb");
  strcpy(m_ShortMonths[ 2], "Mar");  strcpy(m_ShortMonths[ 3], "Apr");
  strcpy(m_ShortMonths[ 4], "May");  strcpy(m_ShortMonths[ 5], "Jun");
  strcpy(m_ShortMonths[ 6], "Jul");  strcpy(m_ShortMonths[ 7], "Aug");
  strcpy(m_ShortMonths[ 8], "Sep");  strcpy(m_ShortMonths[ 9], "Oct");
  strcpy(m_ShortMonths[10], "Nov");  strcpy(m_ShortMonths[11], "Dec");
  }


/*
 *
**/
LB_Bool_T               LB_Date_T::IsNothing()
  {
  if ((m_Year == 0)     &&
      (m_Month == 0)    &&
      (m_Day == 0))
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               LB_Date_T::SetTo(LB_Word_T pa_Month,
                                         LB_Word_T pa_Day,
                                         LB_Word_T pa_Year)
  {
  if ((pa_Month >= 1) && (pa_Month <= 12))
    m_Month = pa_Month;
  else
    return (LB_FALSE);

  if ((pa_Day >= 1) && (pa_Day <= 31))
    m_Day = pa_Day;

  m_Year = pa_Year;

  if (m_Year < 1900)
    m_Year = m_Year + 1900;

  return (LB_TRUE);
  }


/*
 *
**/
void                    LB_Date_T::SetToNothing(void)
  {
  m_Month   = 0;
  m_Day     = 0;
  m_Year    = 0;
  }


/*
 *
**/
LB_Bool_T               LB_Date_T::SetToToday(void)
  {
  time_t        timesecs;
  struct tm     *tmblock;

  timesecs = time((time_t *) NULL);

  tmblock  = localtime(&timesecs);

  m_Month  = tmblock->tm_mon + 1;       // 0 = january
  m_Day    = tmblock->tm_mday;
  m_Year   = tmblock->tm_year + 1900;

  return (LB_TRUE);
  }


/*
 *
**/
LB_Word_T               LB_Date_T::GetMonth(void)
  {
  return (m_Month);
  }


/*
 *
**/
void                    LB_Date_T::GetMonth(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d",GetMonth());  pa_String = l_Buf;
  }


/*
 *
**/
LB_Word_T               LB_Date_T::GetDay(void)
  {
  return (m_Day);
  }


/*
 *
**/
void                    LB_Date_T::GetDay(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d",GetDay());  pa_String = l_Buf;
  }


/*
 *
**/
LB_Word_T               LB_Date_T::GetYear(void)
  {
  return (m_Year);
  }


/*
 *
**/
void                    LB_Date_T::GetYear(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%04d",GetYear());  pa_String = l_Buf;
  }


/*
 *
**/
LB_Word_T               LB_Date_T::GetYearShort(void)
  {
  LB_Word_T l_Year;

  l_Year = m_Year;

  while (l_Year > 100)
    l_Year = l_Year - 100;

  return (l_Year);
  }


/*
 *
**/
void                    LB_Date_T::GetYearShort(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d",GetYearShort());  pa_String = l_Buf;
  }


/*
 *
**/
void                    LB_Date_T::GetLongMonth(LB_String_T& pa_String)
  {
//  pa_String = m_LongMonths[GetMonth()];
  pa_String = "Not implemented";
  }


/*
 *
**/
void                    LB_Date_T::GetShortMonth(LB_String_T& pa_String)
  {
  pa_String = m_ShortMonths[GetMonth()-1];
  }


/*
 *
**/
void                    LB_Date_T::GetSortDateLong(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%04d%02d%02d",GetYear(), GetMonth(), GetDay());

  pa_String = l_Buf;
  }


/*
 *
**/
void                    LB_Date_T::GetSortDateShort(LB_String_T& pa_String)
  {
  char l_Buf[16];

  sprintf(l_Buf,"%02d%02d%02d",GetYearShort(), GetMonth(), GetDay());

  pa_String = l_Buf;
  }
