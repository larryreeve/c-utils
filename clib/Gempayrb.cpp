/*  gempayrb.cpp
 *
 *  Gem Payroll Reference - Bank Processing routines
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
 *      02/27/96        LR      -Initial Release
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
#include <lbport.h>
#include <gempayrb.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemPayRefBank_T::GemPayRefBank_T()
  {
  m_DataFile.ColumnDefine("ID",             2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Rec",            2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Index",          2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("BankRTID",       9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Rest",           2, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("BankName",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address1",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address2",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("City",          30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Filler",       163, MF_COLFLG_STRING);
  }


/*
 *
**/
GemPayRefBank_T::~GemPayRefBank_T()
  {
  Close();
  }


/*
 *
**/
void                    GemPayRefBank_T::Close()
  {
  m_DataFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemPayRefBank_T::Find(LB_String_T& pa_BankNum)
  {
  LB_String_T   l_ID;
  LB_String_T   l_Index;
  LB_String_T   l_Rec;

  m_DataFile.RowMoveTop();

  while (m_DataFile.RowMoveNext() == LB_TRUE)
    {
    if (GetColumnValue(PAYRB_COL_ID,l_ID) == LB_FALSE)
      return (LB_FALSE);
    if (l_ID != "9")
      continue;

    if (GetColumnValue(PAYRB_COL_INDEX,l_Index) == LB_FALSE)
      return (LB_FALSE);
    if (l_Index != "0")
      continue;

    if (GetColumnValue(PAYRB_COL_REC,l_Rec) == LB_FALSE)
      return (LB_FALSE);

    if (l_Rec == pa_BankNum)
      return (LB_TRUE);
    }

  return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemPayRefBank_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   l_String;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetPayrollPath(l_String) == LB_FALSE)
    return (LB_FALSE);

  l_String = l_String + "payref.dat";

  m_DataFile.SetTableName(l_String);

  return (m_DataFile.Open());
  }


/*
 *
**/
LB_Bool_T               GemPayRefBank_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


/*
 *
**/
LB_Bool_T               GemPayRefBank_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                        LB_String_T& pa_Value)
  {
  LB_String_T l_String;

  switch(pa_ColID)
    {
    case PAYRB_COL_ID:          l_String = "ID";                break;
    case PAYRB_COL_REC:         l_String = "Rec";               break;
    case PAYRB_COL_INDEX:       l_String = "Index";             break;
    case PAYRB_COL_BANKRTID:    l_String = "BankRTID";          break;
    case PAYRB_COL_BANKNAME:    l_String = "BankName";          break;

    default:
      pa_Value.Clear();
      return (LB_FALSE);
    }

  if (m_DataFile.GetColumnValue(l_String,pa_Value) == LB_FALSE)
    {
    pa_Value.Clear();
    return (LB_FALSE);
    }

  return (LB_TRUE);
  }
