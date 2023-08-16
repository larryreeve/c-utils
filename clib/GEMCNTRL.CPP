/*  gemcntrl.cpp
 *
 *  GEM Control File processing routines
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
 *      11/01/95        LR      -Initial Release
 *      11/20/95        LR      -Now uses string library class
 *      01/29/96        LR      -Read now requires an organization ID
 *      02/05/96        LR      -Calls now use string type for parameters
 *
 * Notes:
 *      GEM Control File Record Layout:
 *
 *      Paths                   = PIC X(24) OCCURS 30
 *                      1 = Payroll
 *                      2 = Budget
 *                      3 = Utility Billing
 *                      4 = Tax Accounting
 *                      6 = Voter Registration
 *                      8 = Report Writer
 *      PrinterPort             = PIC X(6)
 *      AccountName             = PIC X(40)
 *      Purchase                = PIC 99 COMP OCCURS 30
 *              PUR(29) 1 = MegaFrame (5000/70)
 *                      2 = Sperry 5000/30/50
 *                      3 = Sperry 5000/35/55, U6000/50
 *                      4 = Sperry 5000/85
 *                      5 = Sperry 5000/95
 *
 *              PUR(30) 0 = Live CB3
 *                      1 = Demo CB3
 *                      2 = Live CB5
 *                      3 = Demo CB5
 *      Ann-Chk                 = PIC S9(6) COMP
 *      Ann-DT                  = PIC S9(6) COMP
 *      Sum                     = PIC S9(6) COMP
 *      Passwords               = PIC X(4) OCCURS 30
 *                      1 = Budget
 *                      2 = Utility Billing
 *                      3 = Payroll
 *                      4 = Tax Accounting
 *                      6 = Voter Registration
 *                      8 = Report Writer
 *                     30 = Master Password
 *      Filler1A                = PIC X(40)
 *      Filler1B                = PIC X(41)
 *      Sum2                    = PIC S9(6) COMP
 *      Filler2                 = PIC X(7)
 *
 *
 *       BUDGETARY      PAYROLL        UTILITY        TAX BILLING
 *       ------------   -----------    -------------  ------------------
 *       1. A/P CHECK  2. CHECK/NOD    8. BILLS       21. BILLS
 *      13. SCH. PAY   3. W2           9. REMINDER    22. RECEIPT CARDS
 *      14. P.O. SP.   4. 941         10. DISCONNECT  23. REMINDERS
 *      15. ABR BUDG   5. UNEMP       11. METER BOOK  24. VALUATION BOOK
 *      16. REV & EXP  6. PENSION     12. MAIL LABEL  25. LABELS
 *                     7. CITY TAX    17. BILLING REG
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <memory.h>
#include <lbport.h>
#include <lbfio.hpp>
#include <lbstring.hpp>
#include <gemcntrl.hpp>


/*---------------------------------------------------------------
  ---                   Module Constants                      ---
  ---------------------------------------------------------------*/
#define GEMCTL_PAYROLL  0
#define GEMCTL_BUDGET   1
#define GEMCTL_FN       "GEMCNTRL.DAT"


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *--- Class constructor
**/
GemControl_T::GemControl_T()
  {
  m_DataReadFlag = LB_FALSE;
  }


/*
 *--- Class destructor
**/
GemControl_T::~GemControl_T()
  {
  m_File.Close();
  }


/*
 *--- Read GemControl, returning False if file could not be found, opened,
 *--- or read. Otherwise TRUE. GEMCNTRL.DAT is located only in the current
 *--- directory, normally \GEM as specified by Gemini Systems Inc.
**/
LB_Bool_T       GemControl_T::Read(LB_Word_T pa_OrgID)
  {
  LB_Word_T     l_Index;
  LB_Word_T     l_BytesRead;
  LB_String_T   l_String;

  m_DataReadFlag = LB_FALSE;

  memset(&m_GemCtlRec,'\0',sizeof(m_GemCtlRec));

  if (pa_OrgID == 0)
    return (LB_FALSE);

  l_String = GEMCTL_FN;
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  l_String.Clear();
  l_String.PadRight(sizeof(m_GemCtlRec),0x20);

  m_File.SeekFromStart(0);

  for (l_Index=0; l_Index < pa_OrgID; l_Index++)
    l_BytesRead = m_File.Read(l_String);

  m_File.Close();

  l_String.GetBytes((LB_Byte_T *) &m_GemCtlRec,sizeof(m_GemCtlRec));

  if ((l_BytesRead != sizeof(m_GemCtlRec)) || (m_GemCtlRec.DeletedFlag == 0x00))
    {
    memset(&m_GemCtlRec,'\0',sizeof(m_GemCtlRec));
    return (LB_FALSE);
    }
  else
    {
    m_DataReadFlag = LB_TRUE;
    return (LB_TRUE);
    }
  }


/*
 *--- Get authorized account name
**/
LB_Bool_T               GemControl_T::GetAccountName(LB_String_T& pa_Buf)
  {
  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  pa_Buf.SetBytes((LB_Byte_T *) m_GemCtlRec.AccountName, sizeof(m_GemCtlRec.AccountName));

  pa_Buf.Proper();

  return (LB_TRUE);
  }


/*
 *
 *--- Get file location of budget data
 *
**/
LB_Bool_T               GemControl_T::GetBudgetPath(LB_String_T& pa_Buf)
  {
  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  pa_Buf.SetBytes((LB_Byte_T *) m_GemCtlRec.Paths[GEMCTL_BUDGET],
                  (LB_Word_T) sizeof(m_GemCtlRec.Paths[GEMCTL_BUDGET]));

  pa_Buf.TrimRight();

  return (LB_TRUE);
  }


/*
 *
 *--- Get file location of payroll data
 *
**/
LB_Bool_T               GemControl_T::GetPayrollPath(LB_String_T& pa_Buf)
  {
  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  pa_Buf.SetBytes((LB_Byte_T *) m_GemCtlRec.Paths[GEMCTL_PAYROLL],
                  (LB_Word_T) sizeof(m_GemCtlRec.Paths[GEMCTL_PAYROLL]));

  pa_Buf.TrimRight();

  return (LB_TRUE);
  }


/*
 *
 *--- Return the default printer string
 *
**/
LB_Bool_T               GemControl_T::GetPrinter(LB_String_T& pa_Buf)
  {
  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  pa_Buf.SetBytes((LB_Byte_T *) m_GemCtlRec.Printer,
                  sizeof(m_GemCtlRec.Printer));

  return (LB_TRUE);
  }
