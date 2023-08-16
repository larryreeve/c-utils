/*  lbfcalc.cpp
 *
 *  Financial Calculation Engine
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
 *            COPYRIGHT (C) 1995 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      02/26/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <bcd.h>

#include <lbport.h>
#include <lbdebug.hpp>
#include <lbfcalc.hpp>

LB_ASSERTFILE(__FILE__)


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_FloatPt_T            LB_FinCalc_T:: Add(LB_FloatPt_T  pa_Op1,
                                           LB_FloatPt_T  pa_Op2)
  {
  bcd   l_Result;
  bcd   l_Op1(pa_Op1,2);
  bcd   l_Op2(pa_Op2,2);

  l_Result = pa_Op1 + pa_Op2;

  return (real(l_Result));
  }
