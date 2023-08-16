/*  gempaym.cpp
 *
 *  Gem Payroll Master Processing routines
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
 * Version Information:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      01/25/96        LR      -Initial Release
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
#include <gempaym.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemPayMast_T::GemPayMast_T()
  {
  m_DataFile.ColumnDefine("Number",         3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CostCenter",     3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StatusCode",     1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Name",          30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address1",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address2",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Address3",      30, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ShortName",     10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("ZipCode",       10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("SocSec",         9, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("NumberK4",       3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Location",       3, MF_COLFLG_COMP);

  m_DataFile.ColumnDefine("M_S",            1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("W_T",            1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("MS_T",           1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Unem_T",         1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("JobClass",       3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("WrkCmp",         2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Dept",           3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Sex",            1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Race",           1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("HireDate",       3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("TermDate",       3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("BirthDate",      3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("LastRaiseDate",  3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("TermCode",       1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Union",          3, MF_COLFLG_COMP);

  m_DataFile.ColumnDefine("DirDepBank",     2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("DirDepAcct",    15, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("WageClass",      1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("Period",         2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Code",           2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Rate",           4, MF_COLFLG_COMP + MF_COLFLG_4DEC + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ConPay",         1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StdHrs",         3, MF_COLFLG_COMP + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("OverRt",         2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("Quarter",        1, MF_COLFLG_COMP);

  m_DataFile.ColumnDefine("EIC",            1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("FedEx",          1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StateEx",        1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CityEx",         1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("FedAd",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateAd",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateTaxCode",   1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("CityTaxCode",    3, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("FedAdPct",       1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("StateAdPct",     1, MF_COLFLG_COMP);


  m_DataFile.ColumnDefine("PenRetPlan",     1, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PenID",          2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PenRate1",       3, MF_COLFLG_COMP + MF_COLFLG_4DEC);
  m_DataFile.ColumnDefine("PenRate2",       3, MF_COLFLG_COMP + MF_COLFLG_4DEC );
  m_DataFile.ColumnDefine("PenLimit",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenAmt1",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenAmt2",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenNum",        12, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PenCode1",      10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PenCode2",      10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PenCode3",      10, MF_COLFLG_STRING);
  m_DataFile.ColumnDefine("PenCode4",      10, MF_COLFLG_STRING);

  m_DataFile.ColumnDefine("SckCode",        2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("PerCode",        2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("VacCode",        2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OT1Code",        2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OT2Code",        2, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("OT3Code",        2, MF_COLFLG_COMP);

  m_DataFile.ColumnDefine("SckAL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PerAL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("VacAL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT1AL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT2AL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT3AL",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("SckUDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("SckUDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("SckUDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("SckUDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("SckUDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("PerUDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PerUDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PerUDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PerUDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PerUDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("VacUDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("VacUDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("VacUDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("VacUDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("VacUDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("OT1UDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT1UDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT1UDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT1UDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT1UDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("OT2UDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT2UDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT2UDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT2UDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT2UDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("OT3UDY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT3UDQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT3UDP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT3UDF",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OT3UDC",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("OthDedY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthDedQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthDedP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthDedF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthDedC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("OthExtY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthExtQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthExtP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthExtF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthExtC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("FedErnY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FedErnQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FedErnP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FedTaxY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FedTaxQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FedTaxP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("StateErnY",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateErnQ",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateErnP",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateTaxY",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateTaxQ",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("StateTaxP",      4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("CityErnY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CityErnQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CityErnP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CityTaxY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CityTaxQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("CityTaxP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("FicaErnY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FicaErnQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FicaErnP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FicaTaxY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FicaTaxQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("FicaTaxP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("MedErnY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("MedErnQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("MedErnP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("MedTaxY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("MedTaxQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("MedTaxP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("O1ErnY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O1ErnQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O1ErnP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O1TaxY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O1TaxQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O1TaxP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("O2ErnY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O2ErnQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O2ErnP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O2TaxY",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O2TaxQ",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("O2TaxP",         4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("EicPayY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("EicPayQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("EicPayP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("TotErnY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("TotErnQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("TotErnF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("TotErnP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("TotErnC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NConErnY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NConErnQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NConErnF",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NConErnP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("NConPenC",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ContErnY",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ContErnQ",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ContErnF",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ContErnP",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("ContErnC",       4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthErnY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthErnQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthErnF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthErnP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OthErnC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenErnY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenErnQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenErnP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenErnF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenErnC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenConY",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenConQ",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenConP",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenConF",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("PenConC",        4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("WeeksWrkY",      3, MF_COLFLG_COMP + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("WeeksWrkQ",      3, MF_COLFLG_COMP + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("WeeksWrkP",      3, MF_COLFLG_COMP + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("WeeksWrkF",      3, MF_COLFLG_COMP + MF_COLFLG_2DEC);
  m_DataFile.ColumnDefine("WeeksWrkC",      3, MF_COLFLG_COMP + MF_COLFLG_2DEC);

  m_DataFile.ColumnDefine("ContPayY",       1, MF_COLFLG_COMP);
  m_DataFile.ColumnDefine("RHRSY",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("RHRSQ",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("RHRSP",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("RHRSF",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("RHRSC",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OTHRY",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OTHRQ",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OTHRP",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OTHRF",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);
  m_DataFile.ColumnDefine("OTHRC",          4, MF_COLFLG_COMP + MF_COLFLG_2DEC  + MF_COLFLG_SIGNED);

  m_DataFile.ColumnDefine("Filler1",       47, MF_COLFLG_STRING);
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
  m_DataFile.ColumnDefine("Filler2",       78, MF_COLFLG_STRING);
  }


/*
 *
**/
GemPayMast_T::~GemPayMast_T()
  {
  Close();
  }


/*
 *
**/
void                    GemPayMast_T::Close()
  {
  m_DataFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemPayMast_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T   l_String;

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  if (m_GemCtl.GetPayrollPath(l_String) == LB_FALSE)
    return (LB_FALSE);

  l_String = l_String + "paymast.dat";

  m_DataFile.SetTableName(l_String);

  return (m_DataFile.Open());
  }


/*
 *
**/
LB_Bool_T               GemPayMast_T::ReadNext()
  {
  return (m_DataFile.RowMoveNext());
  }


/*
 *
**/
LB_Bool_T               GemPayMast_T::Restart()
  {
  if (m_DataFile.RowMoveTop() == LB_TRUE)
    return (LB_TRUE);

  return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemPayMast_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                     LB_String_T& pa_Value)
  {
  LB_String_T l_String;

  pa_Value.Clear();

  switch(pa_ColID)
    {
    case PAYM_COL_EMPNAME:      l_String = "Name";              break;
    case PAYM_COL_EMPNUM:       l_String = "Number";            break;
    case PAYM_COL_DDACCT:       l_String = "DirDepAcct";        break;
    case PAYM_COL_DDBANKNUM:    l_String = "DirDepBank";        break;
    case PAYM_COL_USER1:        l_String = "User1";             break;
    case PAYM_COL_USER10:       l_String = "User10";            break;

    default:
      return (LB_FALSE);
    }

  return (m_DataFile.GetColumnValue(l_String,pa_Value));
  }
