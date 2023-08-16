/*  gempayh.cpp
 *
 *  Gem Payroll History Processing routines
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
 *      01/22/96        LR      -Initial Release
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
#include <gempayh.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemPayHist_T::GemPayHist_T()
  {
  m_DataFile.ColumnDefine("EmpNumber",       3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Week",            1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Extra",           1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CostCenter",      3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Location",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("DirectDeposit",   2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("WageClass",       1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PeriodCode",      2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Code",            2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StdHours",        3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("StdRate",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_4DEC);
//--redefinition here
  m_DataFile.ColumnDefine("Date",            3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CheckNo",         3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Manual",          1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Printed",         1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("ConPayNum",       1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("ConPayYN",        1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Qtr",             1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StateTaxCode",    1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CityTaxCode",     1, MF_COLFLG_COMP);

  m_DataFile.ColumnDefine("SickHrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("VactHrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("PersHrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth1HrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth2HrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth3HrsUsed",     3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("SickHrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("VactHrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("PersHrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth1HrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth2HrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Oth3HrsAccum",    3, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);

  m_DataFile.ColumnDefine("FedEarn",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("FedTax",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("StateEarn",       4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("StateTax",        4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("CityEarn",        4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("CityTax",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("FicaEarn",        4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("FicaTax",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("MedEarn",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("MedTax",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OT1Earn",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OT1Tax",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OT2Earn",         4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OT2Tax",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("TotalEarn",       4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("NonContractEarn", 4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("ContractEarn",    4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("PensionEarn",     4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("PensionContr",    4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("EICPay",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("RegHrs",          4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OTHrs",           4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Net",             4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Gross",           4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);

  m_DataFile.ColumnDefine("WeeksWrk",        1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("BudUpdated",      1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Opt",             2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("RMV",             1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("RVX",             1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Filler",        113, MF_COLFLG_STRING);
  }


/*
 *
**/
GemPayHist_T::~GemPayHist_T()
  {
  Close();
  }


/*
 *
**/
void                    GemPayHist_T::Close()
  {
  m_DataFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemPayHist_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   l_String;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetPayrollPath(l_String) == LB_FALSE)
    return (LB_FALSE);

  l_String = l_String + "payhist.dat";

  m_DataFile.SetTableName(l_String);

  return (m_DataFile.Open());
  }


/*
 *
**/
LB_Bool_T               GemPayHist_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


/*
 *
**/
LB_Bool_T               GemPayHist_T::Restart()
  {
  if (m_DataFile.RowMoveTop() == LB_TRUE)
    return (LB_TRUE);

  return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemPayHist_T::SearchNext(LB_String_T& pa_EmpNum,
                                                 LB_String_T& pa_Week,
                                                 LB_String_T& pa_Extra)
  {
  LB_String_T l_String;

  while (ReadNext() == LB_TRUE)
    {
    //--* Check for employee number
    if (GetColumnValue(PAYH_COL_EMPNUM,l_String) == LB_FALSE)
      return (LB_FALSE);
    if (l_String != pa_EmpNum)
      continue;

    //--* Check for week
    if (GetColumnValue(PAYH_COL_WEEK,l_String) == LB_FALSE)
      return (LB_FALSE);
    if (l_String != pa_Week)
      continue;

    //--* Check for extra
    if (GetColumnValue(PAYH_COL_EXTRA,l_String) == LB_FALSE)
      return (LB_FALSE);
    if (l_String != pa_Extra)
      continue;

    return (LB_TRUE);
    }

  return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemPayHist_T::GetColumnValue(LB_Word_T pa_ColID,
                                                       LB_String_T& pa_String)
  {
  LB_String_T l_String;

  switch(pa_ColID)
    {
    case PAYH_COL_EMPNUM:       l_String = "EmpNumber";         break;
    case PAYH_COL_WEEK:         l_String = "Week";              break;
    case PAYH_COL_EXTRA:        l_String = "Extra";             break;
    case PAYH_COL_NET:          l_String = "Net";               break;
    case PAYH_COL_GROSS:        l_String = "Gross";             break;
    case PAYH_COL_REMOVED:      l_String = "RMV";               break;
    case PAYH_COL_MANUAL:       l_String = "Manual";            break;

    default:
      return (LB_FALSE);
    }

  return (m_DataFile.GetColumnValue(l_String,pa_String));
  }
