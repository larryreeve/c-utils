/*  mfcolumn.cpp
 *
 *  MicroFocus Column manipulation routines
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
 *      01/17/96        LR      -Initial Release
 *      02/16/96        LR      -Re-structured to use flags
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <lbport.h>
#include <lbdate.hpp>
#include <lbtime.hpp>
#include <lbcblcmp.hpp>
#include <mfcolumn.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
MF_Column_T::MF_Column_T()
  {
  Clear();
  }


MF_Column_T::~MF_Column_T()
  {
  Clear();
  }


void            MF_Column_T::Clear()
  {
  m_ColName.Clear();
  m_ColSize     = 0;
  m_ColFlags    = 0;
  m_ColData.Clear();
  }


LB_Bool_T       MF_Column_T::Define(LB_String_T&  pa_Name,
                                    LB_Word_T     pa_Size,
                                    LB_Word_T     pa_Flags)
  {
  m_ColName  = pa_Name;
  m_ColName.UpperCase();
  m_ColSize  = pa_Size;
  m_ColFlags = pa_Flags;

  return (LB_TRUE);
  }


LB_Bool_T               MF_Column_T::GetValue(LB_String_T& pa_Value)
  {
  LB_FloatPt_T  l_Value;
  char          l_Buffer[32];

  if (m_ColFlags & MF_COLFLG_COMP)
    {
    if (m_CobComp.Decode(&l_Value,
                         m_ColData,
                         GetDecimalCount(),
                         IsSigned()) == LB_FALSE)
      l_Value = 0.00;

    if (m_ColFlags & MF_COLFLG_0DEC)
      sprintf(l_Buffer,"%.0f",l_Value);
    else if (m_ColFlags & MF_COLFLG_2DEC)
      sprintf(l_Buffer,"%.2f",l_Value);
    else if (m_ColFlags & MF_COLFLG_4DEC)
      sprintf(l_Buffer,"%.4f",l_Value);
    else
      sprintf(l_Buffer,"%.0f",l_Value);

    pa_Value = l_Buffer;
    }
  else if (m_ColFlags & MF_COLFLG_STRING)
    pa_Value = m_ColData;

  return (LB_TRUE);
  }


LB_Word_T               MF_Column_T::GetDecimalCount()
  {
  if (m_ColFlags & MF_COLFLG_0DEC)
    return (0);
  else if (m_ColFlags & MF_COLFLG_2DEC)
    return (2);
  else if (m_ColFlags & MF_COLFLG_4DEC)
    return (4);
  else
    return (0);
  }


void            MF_Column_T::GetName(LB_String_T& pa_Name)
  {
  pa_Name = m_ColName;
  pa_Name.UpperCase();
  }


LB_Word_T         MF_Column_T::GetSize()
  {
  return (m_ColSize);
  }


LB_Word_T       MF_Column_T::GetType()
  {
  if (m_ColFlags & MF_COLFLG_COMP)
    return (MF_COLFLG_COMP);
  else if (m_ColFlags & MF_COLFLG_STRING)
    return (MF_COLFLG_STRING);
  else
    return (MF_COLFLG_STRING);
  }


LB_Bool_T       MF_Column_T::IsNull()
  {
  if (m_ColData.Size() == 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Bool_T       MF_Column_T::IsRequired()
  {
  if (m_ColFlags & MF_COLFLG_REQUIRED)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Bool_T       MF_Column_T::IsSigned()
  {
  if (m_ColFlags & MF_COLFLG_SIGNED)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Bool_T       MF_Column_T::SetValue(LB_String_T &pa_String)
  {
  m_ColData = pa_String;

  return (LB_TRUE);
  }
