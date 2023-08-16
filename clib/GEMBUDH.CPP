/*  gembudh.cpp
 *
 *  Gem Budget History File Processing routines
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
 *      03/21/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------*/
#include <stdio.h>
#include <lbport.h>
#include <gembudh.hpp>


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------*/
GemBudHist_T::GemBudHist_T()
  {
  m_DataFile.ColumnDefine("Acct",          32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Date",           3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Year",           1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Month",          1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Desc",          24, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Ref",            3, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Type",           1, MF_COLFLG_STRING);
  //
  //--- Types
  //
  //    A = Budget Appropriation, Original Appropriation - ADD
  //    P = PO Encumbrance
  //    N = NonPO Encumbrance
  //    L = NonPO Liquidation
  //    E = Expense
  //    C = PO Liquidation from AP
  //    R = Reverse PO
  //    F = Receipt of Funds
  //
  m_DataFile.ColumnDefine("PONum",          9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("InvNum",         9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("VenNum",         3, MF_COLFLG_COMP);
	m_DataFile.ColumnDefine("POLine",         2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Filler1",        1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Amount",         5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CurMTDBal",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CurYTDBal",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("RcptNum",       10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("JrnlNum",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Dist",           2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Filler2",        1, MF_COLFLG_STRING);
  }


GemBudHist_T::~GemBudHist_T()
  {
  Close();
  }


void                    GemBudHist_T::Close()
  {
  m_DataFile.Close();
  }


LB_Bool_T               GemBudHist_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   DataFileName;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetBudgetPath(DataFileName) == LB_FALSE)
    return (LB_FALSE);

  DataFileName = DataFileName + "budhist.dat";

  m_DataFile.SetTableName(DataFileName);

  return (m_DataFile.Open());
  }


LB_Bool_T               GemBudHist_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


LB_Bool_T               GemBudHist_T::Restart()
  {
  if (m_DataFile.RowMoveTop() == LB_TRUE)
    return (LB_TRUE);

  return (LB_FALSE);
  }


LB_Bool_T               GemBudHist_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                     LB_String_T& pa_Value)
  {
  LB_String_T ColumnName;

  switch(pa_ColID)
    {
    case BUDH_COL_ACCT:         ColumnName = "Acct";              break;
    case BUDH_COL_DATE:         ColumnName = "Date";              break;
    case BUDH_COL_YEAR:         ColumnName = "Year";              break;
    case BUDH_COL_MONTH:        ColumnName = "Month";             break;
    case BUDH_COL_DESC:         ColumnName = "Desc";              break;
    case BUDH_COL_REF:          ColumnName = "Ref";               break;
    case BUDH_COL_TYPE:         ColumnName = "Type";              break;
    case BUDH_COL_PONUM:        ColumnName = "PONum";             break;
    case BUDH_COL_INVNUM:       ColumnName = "InvNum";            break;
    case BUDH_COL_VENNUM:       ColumnName = "VenNum";            break;
    case BUDH_COL_POLINE:       ColumnName = "POLine";            break;
    case BUDH_COL_AMOUNT:       ColumnName = "Amount";            break;
    case BUDH_COL_CURMTDBAL:    ColumnName = "CurMTDBal";         break;
    case BUDH_COL_CURYTDBAL:    ColumnName = "CurYTDBal";         break;
    case BUDH_COL_RCPTNUM:      ColumnName = "RcptNum";           break;
    case BUDH_COL_JRNLNUM:      ColumnName = "JrnlNum";           break;
    case BUDH_COL_DIST:         ColumnName = "Dist";              break;

    default:
      pa_Value.Clear();
      return (LB_FALSE);
    }

  return (m_DataFile.GetColumnValue(ColumnName, pa_Value));
  }
