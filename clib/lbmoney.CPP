/*  lbmoney.cpp
 *
 *  Money routines
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
**/


#include <lbmoney.hpp>

LB_Money_T::LB_Money_T()
{
  m_Value = (LB_FloatPt_T) 0.00;
}

void LB_Money_T::operator = (LB_FloatPt_T pa_Value)
{
  m_Value = pa_Value;
}



LB_Money_T::~LB_Money_T()
{
  m_Value = (LB_FloatPt_T) 0.00;
}


LB_Money_T LB_Money_T::operator + (LB_FloatPt_T pa_Value)
{
  m_Value = m_Value + pa_Value;

  return (*this);
}


LB_FloatPt_T LB_Money_T::AsFloatPt()
{
  return ((LB_FloatPt_T) real(m_Value));
}

