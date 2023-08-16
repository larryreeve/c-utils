/*  xbcolumn.cpp
 *
 *  X-Base Column manipulation routines
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
 *            COPYRIGHT (C) 1995-1997 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      01/10/96        LR      -Initial Release
 *      03/22/97        LR      -Update to match MicroFocus column
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <lbport.h>
#include <lbdate.hpp>
#include <lbtime.hpp>
#include <xbcolumn.hpp>


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
XB_Column_T::XB_Column_T()
{
  Clear();
}


XB_Column_T::~XB_Column_T()
{
  Clear();
}


void            XB_Column_T::Clear()
{
  m_ColName.Clear();
  m_ColSize  = 0;
  m_ColFlags = 0;
  m_ColData.Clear();
}


LB_Bool_T       XB_Column_T::Define(LB_String_T&  pa_Name,
                                    LB_Word_T     pa_Size,
                                    LB_Word_T     pa_Flags)
{
  m_ColName  = pa_Name;
  m_ColName.UpperCase();
  m_ColSize  = pa_Size;
  m_ColFlags = pa_Flags;

  return (LB_TRUE);
}


LB_Bool_T       XB_Column_T::GetValue(LB_String_T& pa_Value)
{
  LB_Date_T     CurDate;
  LB_Time_T     CurTime;
  char          TimeStampBuf[128];

  if (m_ColFlags & XB_COLFLG_TIMESTAMP)
    {
    CurDate.SetToToday();
    CurTime.SetToNow();

    //--- Format is YYYYMMDDHHMMSS
    sprintf(TimeStampBuf,"%04d%02d%02d%02d%02d%02d",
            CurDate.GetYear(), CurDate.GetMonth(),  CurDate.GetDay(),
            CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());
      m_ColData.GetBytes((LB_Byte_T *) TimeStampBuf, 14);
    }
  else pa_Value = m_ColData;

  return (LB_TRUE);
}


void            XB_Column_T::GetName(LB_String_T& pa_Name)
{
  pa_Name = m_ColName;
}


LB_Word_T         XB_Column_T::GetSize()
{
  return (m_ColSize);
}


LB_Word_T       XB_Column_T::GetType()
{
  if (m_ColFlags & XB_COLFLG_STRING)
    return (XB_COLFLG_STRING);
  else if (m_ColFlags & XB_COLFLG_NUMERIC)
    return (XB_COLFLG_NUMERIC);
  else if (m_ColFlags & XB_COLFLG_TIMESTAMP)
    return (XB_COLFLG_TIMESTAMP);
  else
    return (0);
}


LB_Bool_T       XB_Column_T::IsNull()
{
  if (m_ColData.Size() == 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
}


LB_Bool_T       XB_Column_T::IsRequired()
{
  if (m_ColFlags & XB_COLFLG_REQUIRED)
    return (LB_TRUE);
  else
    return (LB_FALSE);
}


LB_Bool_T       XB_Column_T::IsIndex()
{
  if (m_ColFlags & XB_COLFLG_INDEX)
    return (LB_TRUE);
  else
    return (LB_FALSE);
}


LB_Bool_T       XB_Column_T::SetValue(LB_String_T &pa_Value)
  {
  m_ColData = pa_Value;

  return (LB_TRUE);
  }
