/*  lbtstamp.cpp
 *
 *  Time Stamp routines
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
 *      12/14/95        LR      Initial Release
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <lbport.h>
#include <lbdate.hpp>
#include <lbtime.hpp>


/*---------------------------------------------------------------
  ---                   Module Constants                      ---
  ---------------------------------------------------------------*/



/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/



/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_Bool_T               LB_TStamp_T::IsNothing()
  {
  if ((m_Date.IsNothing() == LB_TRUE) &&
      (m_Time.IsNothing() == LB_TRUE))
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


/*
 *
**/
void            SetDate(LB_Date_T pa_Date)
  {
  m_Date = pa_Date;
  }


/*
 *
**/
void            SetTime(LB_Time_T pa_Time);
  {
  m_Time = pa_Time;
  }


/*
 *
**/
void                    LB_TStamp_T::SetToNothing()
  {
  m_Date.SetToNothing();
  m_Time.SetToNothing();
  }


/*
 *
**/
void                    LB_TStamp_T::SetToNow()
  {
  m_Date.SetToNow();
  m_Time.SetToNow();
  }
