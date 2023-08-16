/*  gembudm.cpp
 *
 *  Gem Budget Master File Processing routines
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
 *      03/12/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <stdio.h>
#include <lbport.h>
#include <gembudm.hpp>


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
GemBudMast_T::GemBudMast_T()
  {
  m_DataFile.ColumnDefine("Acct",          32, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Title",         40, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("OrigAppr",       5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Type",           1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Fund",           2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Month",          1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Year",           1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("ApprLYR",        5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Appr2YR",        5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ApprORP",        5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NetLYR",         5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Net2YR",         5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NetORP",         5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ExpendLYR",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Expend2YR",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ExpendORP",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ExpendMTD",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ExpendYTD",      5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("UnencumBal",     5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("UnexpendBal",    5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("EncumMTD",       5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("EncumYTD",       5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NetXFers",       5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NetAppr",        5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ReencAmt",       5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ReceiptsMTD",    5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ReceiptsYTD",    5, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ReencFlag",      1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("GLDistID",       2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Pcts",          36, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Expenditures",  65, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Receipts",      65, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("Encumberances", 65, MF_COLFLG_COMP + MF_COLFLG_2DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("User1",         25, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User2",         25, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User3",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User4",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User5",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User6",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User7",          6, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User8",          6, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User9",          2, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User10",         2, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User11",         1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("User12",         1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Filler",        26, MF_COLFLG_STRING);
  }


GemBudMast_T::~GemBudMast_T()
  {
  Close();
  }


void                    GemBudMast_T::Close()
  {
  m_DataFile.Close();
  }


LB_Bool_T               GemBudMast_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   DataFileName;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetBudgetPath(DataFileName) == LB_FALSE)
    return (LB_FALSE);

  DataFileName = DataFileName + "budmast.dat";

  m_DataFile.SetTableName(DataFileName);

  return (m_DataFile.Open());
  }


LB_Bool_T               GemBudMast_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


LB_Bool_T               GemBudMast_T::Restart()
  {
  if (m_DataFile.RowMoveTop() == LB_TRUE)
    return (LB_TRUE);

  return (LB_FALSE);
  }


LB_Bool_T               GemBudMast_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                     LB_String_T& pa_Value)
  {
  LB_String_T ColumnName;

  switch(pa_ColID)
    {
    case BUDM_COL_ACCT:         ColumnName = "Acct";              break;
    case BUDM_COL_TITLE:        ColumnName = "Title";             break;
    case BUDM_COL_ORIGAPPR:     ColumnName = "OrigAppr";          break;
    case BUDM_COL_TYPE:         ColumnName = "Type";              break;
    case BUDM_COL_FUND:         ColumnName = "Fund";              break;
    case BUDM_COL_EXPENDYTD:    ColumnName = "ExpendYTD";         break;
    case BUDM_COL_UNENCUMBAL:   ColumnName = "UnencumBal";        break;
    case BUDM_COL_UNEXPENDBAL:  ColumnName = "UnexpendBal";       break;
    case BUDM_COL_ENCUMYTD:     ColumnName = "EncumYTD";          break;
    case BUDM_COL_NETXFERS:     ColumnName = "NetXFers";          break;
    case BUDM_COL_NETAPPR:      ColumnName = "NetAppr";           break;
    case BUDM_COL_RECEIPTSYTD:  ColumnName = "ReceiptsYTD";       break;
    case BUDM_COL_REENCAMT:     ColumnName = "ReencAmt";          break;
    case BUDM_COL_GLDISTID:     ColumnName = "GLDistID";          break;
    case BUDM_COL_REENCFLAG:    ColumnName = "ReencFlag";         break;

    default:
      pa_Value.Clear();
      return (LB_FALSE);
    }

  return (m_DataFile.GetColumnValue(ColumnName, pa_Value));
  }
