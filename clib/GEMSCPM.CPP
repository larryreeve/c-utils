/*  gemscpm.cpp
 *
 *  Gem Budget Scheduled Payment File Processing routines
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
 *      04/08/96        LR      -Initial Release
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
#include <gemscpm.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemSCPMast_T::GemSCPMast_T()
  {
  m_DataFile.ColumnDefine("Bank",           1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Vendor1",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OTVendor1",      3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("InvNum1",        9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PONum1",         9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("LineNum1",       2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Dist1",          2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("VoucherNum",     3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Vendor2",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OTVendor2",      3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("InvNum2",        9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PONum2",         9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("LineNum2",       2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Dist2",          2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Unit",           2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Vendor3",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OTVendor3",      3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("InvNum3",        9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PONum3",         9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("LineNum3",       2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Dist3",          2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Desc",          24, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Type",           1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Status",         1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("GrossAmt",       5, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("DiscAmt",        5, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("NetAmt",         5, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("LiqAmt",         5, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("ExpGenDebt",    32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ExpGenCrdt",    32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ExpLiqDebt",    32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ExpLiqCrdt",    32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Account",       32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Ref",            3, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Manual",         1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ChkNum",         3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Date",           3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("InvDate",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("DueDate",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("JrnlNum",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Filler",        10, MF_COLFLG_COMP);
  }


/*
 *
**/
GemSCPMast_T::~GemSCPMast_T()
  {
  Close();
  }


/*
 *
**/
void                    GemSCPMast_T::Close()
  {
  m_DataFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemSCPMast_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   l_String;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetBudgetPath(l_String) == LB_FALSE)
    return (LB_FALSE);

  l_String = l_String + "scpmast.dat";

  m_DataFile.SetTableName(l_String);

  return (m_DataFile.Open());
  }


/*
 *
**/
LB_Bool_T               GemSCPMast_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


/*
 *
**/
LB_Bool_T               GemSCPMast_T::Restart()
  {
  if (m_DataFile.RowMoveTop() == LB_TRUE)
    return (LB_TRUE);

  return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemSCPMast_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                     LB_String_T& pa_Value)
  {
  LB_String_T l_String;

  switch(pa_ColID)
    {
    case SCPM_COL_BANK:         l_String = "Bank";              break;

    default:
      pa_Value.Clear();
      return (LB_FALSE);
    }

  return (m_DataFile.GetColumnValue(l_String,pa_Value));
  }
