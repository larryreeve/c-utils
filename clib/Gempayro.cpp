/*  gempayro.cpp
 *
 *  Gem Payroll Reference - Organization Processing routines
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
 *      01/24/96        LR      -Initial Release
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
#include <gempayro.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemPayRefOrg_T::GemPayRefOrg_T()
  {
  m_DataFile.ColumnDefine("ID",             2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Rec",            2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Ind",            2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Name",          40, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address1",      40, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address2",      40, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("CityState",     40, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ZipCode",       10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Phone",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Interface",      1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("FederalID",     10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("StateID",       10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("FcaID",         10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PruID",          3, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("CgID",           1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("LLI",            1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("EncumSal",       1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("RefMedEmpl",     4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_4DEC);
  m_DataFile.ColumnDefine("RefMedEmpr",     4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_4DEC);
  m_DataFile.ColumnDefine("PayQuarter",     1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PayWeek",        1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PayDate",        3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PayCheckNum",    4, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PayCheckDate",   3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("RefLink",        1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("RefMedERNL",     4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("RefMedTAXL",     4, MF_COLFLG_COMP + MF_COLFLG_SIGNED + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("Filler",        52, MF_COLFLG_STRING);
  }


/*
 *
**/
GemPayRefOrg_T::~GemPayRefOrg_T()
  {
  Close();
  }


/*
 *
**/
void                    GemPayRefOrg_T::Close()
  {
  m_DataFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemPayRefOrg_T::Open(LB_Word_T pa_OrgID)
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
LB_Bool_T               GemPayRefOrg_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


/*
 *
**/
LB_Bool_T               GemPayRefOrg_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                       LB_String_T& pa_Value)
  {
  LB_String_T l_String;

  switch(pa_ColID)
    {
    case PAYRO_COL_ORGNAME:     l_String = "Name";              break;
    case PAYRO_COL_ORGPHONE:    l_String = "Phone";             break;
    case PAYRO_COL_FEDID:       l_String = "FederalID";         break;

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