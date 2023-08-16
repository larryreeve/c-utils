/*  gembudlp.cpp
 *
 *  Gem Budget Laser Check Print Processing routines
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
 *      06/19/96        LR      -Revised for changed budget print
 *
 * Notes:
 *
 * Format of check print file is:
 *
 *      Field Name              Line    Column  Length
 *      Stub.VenNumber          1        9      7
 *      Stub.VenName            1       17      30
 *      Stub.Description        4..18    1      24
 *      Stub.Date               4..18   25      6
 *      Stub.Invoice            4..18   33      9
 *      Stub.AmtGross           4..18   42      13
 *      Stub.AmtDisc            4..18   54      12
 *      Stub.AmtNet             4..18   67      13
 *      Stub.TtlGross           19      42      13
 *      Stub.TtlDisc            19      54      12
 *      Stub.TtlNet             19      67      13
 *      Body.Date               28      25      8
 *      Body.Number             28      41      7
 *      Body.Amount             28      63      15
 *      Body.SpellAmt1          30      ALL
 *      Body.SpellAmt2          31      ALL
 *      Body.VenName            33      ALL
 *      Body.VenAddress         34..42  ALL
 *
 *      Total Lines = 42
 *
 *      Procedure is to call Start, then repeatedly call NextCheck until
 *      STAT_EOF is returned. Between calls to NextCheck, repeatedly make
 *      calls to NextPrintLine until STAT_EOC is returned. Finally, call
 *      Finish.
 *
 *      Synchronization word is VENDOR at start of every check stub at
 *      line position 2.
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <memory.h>
#include <stdio.h>
#include <lbport.h>
#include <lbdebug.hpp>
#include <gembudlp.hpp>

#define MAX_LINESIZE            256

#define STUB_VENNUMBER_OFFSET   9
#define STUB_VENNUMBER_SIZE     7

#define STUB_VENNAME_OFFSET     17
#define STUB_VENNAME_SIZE       30

#define STUB_DESCRIPTION_OFFSET  1
#define STUB_DESCRIPTION_SIZE   24

#define STUB_DATE_OFFSET        25
#define STUB_DATE_SIZE          6

#define STUB_INVOICE_OFFSET     33
#define STUB_INVOICE_SIZE       9

#define STUB_AMTGROSS_OFFSET    42
#define STUB_AMTGROSS_SIZE      13

#define STUB_AMTDISC_OFFSET     54
#define STUB_AMTDISC_SIZE       12

#define STUB_AMTNET_OFFSET      67
#define STUB_AMTNET_SIZE        13

#define STUB_TTLGROSS_OFFSET    42
#define STUB_TTLGROSS_SIZE      13

#define STUB_TTLDISC_OFFSET     54
#define STUB_TTLDISC_SIZE       12

#define STUB_TTLNET_OFFSET      67
#define STUB_TTLNET_SIZE        13

#define BODY_DATE_OFFSET        25
#define BODY_DATE_SIZE          8

#define BODY_NUMBER_OFFSET      41
#define BODY_NUMBER_SIZE        7

#define BODY_AMOUNT_OFFSET      63
#define BODY_AMOUNT_SIZE        15

#define BODY_SPELLAMT1_OFFSET    0
#define BODY_SPELLAMT1_SIZE     60

#define BODY_SPELLAMT2_OFFSET    0
#define BODY_SPELLAMT2_SIZE     60

#define BODY_VENNAME_OFFSET      0
#define BODY_VENNAME_SIZE       60

#define BODY_VENADDRESS_OFFSET   0
#define BODY_VENADDRESS_SIZE    60


LB_ASSERTFILE(__FILE__)


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void                    GemBudLP_T::m_ClearCheck()
  {
  LB_Word_T     l_Index;

  //-----* Clear stub area
  m_Check.Stub.VenNumber.Clear();
  m_Check.Stub.VenName.Clear();

  for (l_Index=0; l_Index < BUDLP_MAX_STUB_LINES; l_Index++)
    {
    m_Check.Stub.Lines[l_Index].Description.Clear();
    m_Check.Stub.Lines[l_Index].Date.Clear();
    m_Check.Stub.Lines[l_Index].Invoice.Clear();
    m_Check.Stub.Lines[l_Index].AmtGross.Clear();
    m_Check.Stub.Lines[l_Index].AmtDisc.Clear();
    m_Check.Stub.Lines[l_Index].AmtNet.Clear();
    }
  m_Check.Stub.LineCur = 0;

  //-----* Clear stub totals
  m_Check.Stub.TotalGross.Clear();
  m_Check.Stub.TotalDisc.Clear();
  m_Check.Stub.TotalNet.Clear();

  //-----* Clear check body
  m_Check.Body.Date.Clear();
  m_Check.Body.Number.Clear();
  m_Check.Body.Amount.Clear();
  m_Check.Body.SpellAmt[0].Clear();
  m_Check.Body.SpellAmt[1].Clear();
  m_Check.Body.VenName.Clear();
  for (l_Index=0; l_Index < BUDLP_MAX_VENADDR; l_Index++)
    m_Check.Body.VenAddr[l_Index].Clear();
  m_Check.Body.VenAddrLine = 0;
  }


/*
 *
**/
void                    GemBudLP_T::m_ClearTemplate()
  {
  LB_Word_T     l_Index;

  for (l_Index=0; l_Index < BUDLP_MAX_CKT_LINES; l_Index++)
    m_Template.Lines[l_Index].Clear();

  m_Template.LineCnt = 0;
  m_Template.LineCur = 0;
  }


/*
 *
**/
void                    GemBudLP_T::m_Clear()
  {
  m_ClearTemplate();

  m_ClearCheck();

  m_CheckFile.Close();
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemBudLP_T::GemBudLP_T()
  {
  m_Clear();

  m_CktInfo.FileName.Clear();
  m_CktInfo.Name.Clear();
  m_CktInfo.Desc.Clear();
  m_CktInfo.Comments.Clear();
  }


/*
 *
**/
GemBudLP_T::~GemBudLP_T()
  {
  Finish();
  }


/*
 *
**/
LB_Bool_T               GemBudLP_T::CktGetInfo(LB_String_T& pa_FN,
                                               LB_String_T& pa_Name,
                                               LB_String_T& pa_Desc,
                                               LB_String_T& pa_Comments)
  {
  LB_Fio_T      l_File;
  LB_String_T   l_SettingsInfo(sizeof(m_CktFileInfo));
  LB_String_T   l_SettingsFN;

  pa_FN.Clear();
  pa_Name.Clear();
  pa_Desc.Clear();
  pa_Comments.Clear();

  l_SettingsFN = BUDLP_SETFILENAME;
  l_File.SetFileName(l_SettingsFN);
  if (l_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  //--* Initialize settings structure
  memset(&m_CktFileInfo,' ',sizeof(m_CktFileInfo));
  m_CktFileInfo.Cr         = 0x0D;
  m_CktFileInfo.DeleteFlag = 0x0A;

  //--* Open  settings file
  if (l_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--* Read  settings file
  l_SettingsInfo.PadRight((LB_Word_T) sizeof(m_CktFileInfo),' ');
  if (l_File.Read(l_SettingsInfo) != sizeof(m_CktFileInfo))
    {
    l_File.Close();
    return (LB_FALSE);
    }

  //--* Close settings file
  l_File.Close();

  //--* Parse settings structure to get template file name
  l_SettingsInfo.GetBytes((LB_Byte_T *) &m_CktFileInfo,sizeof(m_CktFileInfo));
  m_CktInfo.FileName.SetBytes(m_CktFileInfo.CktFileName,sizeof(m_CktFileInfo.CktFileName));
  m_CktInfo.FileName.Trim();

  //--* Open template file
  l_File.SetFileName(m_CktInfo.FileName);

  if (l_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (l_File.Open(FIO_OM_RO_NOBUF) == LB_FALSE)
    return (LB_FALSE);

  //--* Read template descriptive name
  m_CktInfo.Name.PadRight(128,' ');
  l_File.ReadLine(m_CktInfo.Name);
  if (m_CktInfo.Name.Size() > 0)
    m_CktInfo.Name[0] = ' ';
  m_CktInfo.Name.Trim();

  //--* Read template description
  m_CktInfo.Desc.PadRight(128,' ');
  l_File.ReadLine(m_CktInfo.Desc);
  if (m_CktInfo.Desc.Size() > 0)
    m_CktInfo.Desc[0] = ' ';
  m_CktInfo.Desc.Trim();

  //--* Read template comments
  m_CktInfo.Comments.PadRight(128,' ');
  l_File.ReadLine(m_CktInfo.Comments);
  if (m_CktInfo.Comments.Size() > 0)
    m_CktInfo.Comments[0] = ' ';
  m_CktInfo.Comments.Trim();

  //--* Close template file
  l_File.Close();

  //--* Store information into return parameters
  pa_FN       = m_CktInfo.FileName;
  pa_Name     = m_CktInfo.Name;
  pa_Desc     = m_CktInfo.Desc;
  pa_Comments = m_CktInfo.Comments;

  return (LB_TRUE);
  }


/*
 * If no check template file specified, then read the budget print
 * settings file for the name of the file. If the settings file
 * does not exist, then check if the standard check template is available.
 * If it is, then create a settings file with the default template and
 * return TRUE; FALSE otherwise.
**/
LB_Bool_T               GemBudLP_T::CktStoreFileName(LB_String_T& pa_CktFileName)
  {
  LB_Fio_T      l_File;
  LB_String_T   l_SettingsInfo(sizeof(m_CktFileInfo));
  LB_String_T   l_SettingsFN;

  m_CktInfo.FileName = pa_CktFileName;

  //--* No check template file specified
  if (m_CktInfo.FileName.Size() == 0)
    {
    //--* Check to see if standard template exists
    m_CktInfo.FileName = BUDLP_CKTFILENAME;
    l_File.SetFileName(m_CktInfo.FileName);
    if (l_File.Exists() == LB_FALSE)
      //--* Standard template does not exist
      return (LB_FALSE);

    //--* Initialize settings structure
    memset(&m_CktFileInfo,' ',sizeof(m_CktFileInfo));
    m_CktFileInfo.Cr         = 0x0D;
    m_CktFileInfo.DeleteFlag = 0x0A;

    l_SettingsFN = BUDLP_SETFILENAME;
    l_File.SetFileName(l_SettingsFN);

    //--* Settings file does not exist
    if (l_File.Exists() == LB_FALSE)
      {
      //--* Create default settings file
      m_CktInfo.FileName.GetBytes(m_CktFileInfo.CktFileName,sizeof(m_CktFileInfo.CktFileName));
      l_SettingsInfo.SetBytes((LB_Byte_T *) &m_CktFileInfo,(LB_Word_T) sizeof(m_CktFileInfo));

      if (l_File.Create() == LB_FALSE)
        {
        m_CktInfo.FileName.Clear();
        return (LB_FALSE);
        }

      if (l_File.Open(FIO_OM_RW_NOBUF) == LB_FALSE)
        {
        m_CktInfo.FileName.Clear();
        return (LB_FALSE);
        }
      l_File.Write(l_SettingsInfo);
      l_File.Close();
      }
    else
      {
      //--* Settings file does exist, so read in the template name
      l_SettingsFN = BUDLP_SETFILENAME;
      l_File.SetFileName(l_SettingsFN);

      if (l_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
        return (LB_FALSE);

      l_SettingsInfo.PadRight((LB_Word_T) sizeof(m_CktFileInfo),' ');
      if (l_File.Read(l_SettingsInfo) != sizeof(m_CktFileInfo))
        {
        l_File.Close();
        return (LB_FALSE);
        }

      l_File.Close();

      l_SettingsInfo.GetBytes((LB_Byte_T *) &m_CktFileInfo,sizeof(m_CktFileInfo));

      m_CktInfo.FileName.SetBytes(m_CktFileInfo.CktFileName,sizeof(m_CktFileInfo.CktFileName));
      m_CktInfo.FileName.Trim();
      }
    }
  else
    {
    //--- A template name was specified, so save it.

    //--* Check to see if template file exists before storing it
    l_File.SetFileName(m_CktInfo.FileName);
    if (l_File.Exists() == LB_FALSE)
      return (LB_FALSE);

    //--* Initialize settings structure
    memset(&m_CktFileInfo,' ',sizeof(m_CktFileInfo));
    m_CktFileInfo.Cr         = 0x0D;
    m_CktFileInfo.DeleteFlag = 0x0A;

    //--* Create new default template
    m_CktInfo.FileName.GetBytes(m_CktFileInfo.CktFileName,sizeof(m_CktFileInfo.CktFileName));
    l_SettingsInfo.SetBytes((LB_Byte_T *) &m_CktFileInfo,(LB_Word_T) sizeof(m_CktFileInfo));

    l_SettingsFN = BUDLP_SETFILENAME;
    l_File.SetFileName(l_SettingsFN);

    if (l_File.Exists() == LB_FALSE)
      {
      if (l_File.Create() == LB_FALSE)
        {
        m_CktInfo.FileName.Clear();
        return (LB_FALSE);
        }
      }

    if (l_File.Open(FIO_OM_RW_NOBUF) == LB_FALSE)
      {
      m_CktInfo.FileName.Clear();
      return (LB_FALSE);
      }

    l_File.Write(l_SettingsInfo);
    l_File.Close();
    }

  return (LB_TRUE);
  }


/*
 *
**/
void                    GemBudLP_T::Finish()
  {
  m_Clear();

  m_CheckFile.Close();
  }


/*
 *
**/
LB_Bool_T               GemBudLP_T::Start()
  {
  LB_String_T   l_Line;
  LB_String_T   l_ChkFN;
  LB_String_T   l_InfoFN;
  LB_String_T   l_InfoName;
  LB_String_T   l_InfoDesc;
  LB_String_T   l_InfoComments;
  LB_Fio_T      l_TFile;
  LB_Word_T     l_Index;

  m_CheckFile.Close();

  m_ClearTemplate();

  if (m_CktInfo.FileName.Size() == 0)
    {
    if (CktGetInfo(l_InfoFN,l_InfoName,l_InfoDesc,l_InfoComments) == LB_FALSE)
      return (LB_FALSE);
    }

  l_TFile.SetFileName(m_CktInfo.FileName);

  if (l_TFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (l_TFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--* Read template
  while (l_TFile.IsEOF() == LB_FALSE)
    {
    l_Line.PadRight(MAX_LINESIZE,' ');

    l_TFile.ReadLine(l_Line);

    //--* If not a blank line, then check to see if the line is a comment
    //--* and if so then skip the line.
    if (l_Line.Size() > 0)
      {
      if (l_Line[0] == ';')
        continue;
      }

    if (m_Template.LineCnt < BUDLP_MAX_CKT_LINES)
      {
      m_Template.Lines[m_Template.LineCnt] = l_Line;
      m_Template.LineCnt++;
      }
    }

  //--* Close Template File
  l_TFile.Close();

  if (m_Template.LineCnt == 0)
    {
    m_ClearTemplate();
    return (LB_FALSE);
    }

  m_Template.LineCur = m_Template.LineCnt + 1;

  l_ChkFN = BUDLP_CHKFILENAME;

  m_CheckFile.SetFileName(l_ChkFN);

  return (m_CheckFile.Open(FIO_OM_RO_BUF));
  }


/*
 *
**/
LB_Word_T               GemBudLP_T::NextCheck(LB_String_T& pa_ChkNum)
  {
  LB_Word_T     l_Index;
  LB_String_T   l_Line;
  LB_String_T   l_String;

  pa_ChkNum.Clear();

  //--* Clear prior check info
  m_ClearCheck();

  if (m_CheckFile.IsEOF() == LB_TRUE)
    return (BUDLP_STAT_EOF);

  while (m_CheckFile.IsEOF() == LB_FALSE)
    {
    l_Line.PadRight(MAX_LINESIZE,' ');
    m_CheckFile.ReadLine(l_Line);
    l_Line.PadRight(MAX_LINESIZE,' ');

    l_String = l_Line.SubstrMid(2,6);
    if (l_String == "VENDOR")
      break;
    }

   if (l_String != "VENDOR")
     return (BUDLP_STAT_EOF);

  //--* Load vendor information
  l_Line.PadRight(MAX_LINESIZE,' ');
  m_Check.Stub.VenNumber = l_Line.SubstrMid(STUB_VENNUMBER_OFFSET,STUB_VENNUMBER_SIZE);
  m_Check.Stub.VenName   = l_Line.SubstrMid(STUB_VENNAME_OFFSET,  STUB_VENNAME_SIZE);

  //--* Skip blank lines
  l_Line.PadRight(MAX_LINESIZE,' '); m_CheckFile.ReadLine(l_Line);
  l_Line.PadRight(MAX_LINESIZE,' '); m_CheckFile.ReadLine(l_Line);

  //--* Read stub lines
  for (l_Index=0; l_Index < BUDLP_MAX_STUB_LINES; l_Index++)
    {
    l_Line.PadRight(MAX_LINESIZE,' ');
    m_CheckFile.ReadLine(l_Line);
    l_Line.PadRight(MAX_LINESIZE,' ');

    m_Check.Stub.Lines[l_Index].Description = l_Line.SubstrMid(STUB_DESCRIPTION_OFFSET,STUB_DESCRIPTION_SIZE);
    m_Check.Stub.Lines[l_Index].Date        = l_Line.SubstrMid(STUB_DATE_OFFSET,       STUB_DATE_SIZE);
    m_Check.Stub.Lines[l_Index].Invoice     = l_Line.SubstrMid(STUB_INVOICE_OFFSET,    STUB_INVOICE_SIZE);
    m_Check.Stub.Lines[l_Index].AmtGross    = l_Line.SubstrMid(STUB_AMTGROSS_OFFSET,   STUB_AMTGROSS_SIZE);
    m_Check.Stub.Lines[l_Index].AmtDisc     = l_Line.SubstrMid(STUB_AMTDISC_OFFSET,    STUB_AMTDISC_SIZE);
    m_Check.Stub.Lines[l_Index].AmtNet      = l_Line.SubstrMid(STUB_AMTNET_OFFSET,     STUB_AMTNET_SIZE);
    }

  //--* Read stub totals
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);
  l_Line.PadRight(MAX_LINESIZE,' ');

  m_Check.Stub.TotalGross = l_Line.SubstrMid(STUB_TTLGROSS_OFFSET,STUB_TTLGROSS_SIZE);
  m_Check.Stub.TotalDisc  = l_Line.SubstrMid(STUB_TTLDISC_OFFSET, STUB_TTLDISC_SIZE);
  m_Check.Stub.TotalNet   = l_Line.SubstrMid(STUB_TTLNET_OFFSET,  STUB_TTLNET_SIZE);

  //--* Advance to check from stub
  for (l_Index=0; l_Index < 8; l_Index++)
    {
    l_Line.PadRight(MAX_LINESIZE,' ');
    m_CheckFile.ReadLine(l_Line);
    }

  //--* Read date, number, amount
  l_Line.PadRight(MAX_LINESIZE,' '); m_CheckFile.ReadLine(l_Line);
  l_Line.PadRight(MAX_LINESIZE,' ');
  m_Check.Body.Date   = l_Line.SubstrMid(BODY_DATE_OFFSET,   BODY_DATE_SIZE);
  m_Check.Body.Number = l_Line.SubstrMid(BODY_NUMBER_OFFSET, BODY_NUMBER_SIZE);
  m_Check.Body.Amount = l_Line.SubstrMid(BODY_AMOUNT_OFFSET, BODY_AMOUNT_SIZE);

  //--* Voided checks print a void message, which the following fields only
  //--* partially receive. Therefore, ignore the voided message which will
  //--* be picked up the spell amt fields.
  if (m_Check.Body.Number.IsAllSpaces() == LB_TRUE)
    {
    m_Check.Body.Date.Fill(' ');
    m_Check.Body.Number.Fill(' ');
    m_Check.Body.Amount.Fill(' ');
    }

  //--* Skip blank line
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);

  //--* Read spelled amount (1st line)
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);
  m_Check.Body.SpellAmt[0] = l_Line;

  //--* Read spelled amount (2nd line)
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);
  m_Check.Body.SpellAmt[1] = l_Line;

  //--* Skip blank line
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);

  //--* Read vendor name
  l_Line.PadRight(MAX_LINESIZE,' ');  m_CheckFile.ReadLine(l_Line);
  m_Check.Body.VenName = l_Line;

  //--* Read vendor address
  for (l_Index=0; l_Index < BUDLP_MAX_VENADDR; l_Index++)
    {
    l_Line.PadRight(MAX_LINESIZE,' ');
    m_CheckFile.ReadLine(l_Line);
    m_Check.Body.VenAddr[l_Index] = l_Line;
   }

  //--* If last address line contains a VOID CHECK message, then move it
  //--* to the last spelled amount line
  l_Line = "void";
  if (m_Check.Body.VenAddr[0].Search(l_Line,LB_TRUE))
    {
    m_Check.Body.SpellAmt[1] = m_Check.Body.VenAddr[0];
    m_Check.Body.VenAddr[0].Fill(' ');
    }
  //--* Apply Final Formatting to each field
  m_Check.Stub.VenNumber.Trim(); m_Check.Stub.VenNumber.PadLeft(STUB_VENNUMBER_SIZE,' ');

  m_Check.Stub.VenName.Trim();   m_Check.Stub.VenName.PadRight(STUB_VENNAME_SIZE,' ');

  for (l_Index=0; l_Index < BUDLP_MAX_STUB_LINES; l_Index++)
    {
    m_Check.Stub.Lines[l_Index].Description.Trim();
    m_Check.Stub.Lines[l_Index].Description.PadRight(STUB_DESCRIPTION_SIZE,' ');

    m_Check.Stub.Lines[l_Index].Date.Trim();
    m_Check.Stub.Lines[l_Index].Date.PadRight(STUB_DATE_SIZE,' ');

    m_Check.Stub.Lines[l_Index].Invoice.Trim();
    m_Check.Stub.Lines[l_Index].Invoice.PadRight(STUB_INVOICE_SIZE,' ');

    m_Check.Stub.Lines[l_Index].AmtGross.Trim();
    m_Check.Stub.Lines[l_Index].AmtGross.PadLeft(STUB_AMTGROSS_SIZE,' ');

    m_Check.Stub.Lines[l_Index].AmtDisc.Trim();
    m_Check.Stub.Lines[l_Index].AmtDisc.PadLeft(STUB_AMTDISC_SIZE,' ');

    m_Check.Stub.Lines[l_Index].AmtNet.Trim();
    m_Check.Stub.Lines[l_Index].AmtNet.PadLeft(STUB_AMTNET_SIZE,' ');
    }

  m_Check.Stub.TotalGross.Trim(); m_Check.Stub.TotalGross.PadLeft(STUB_TTLGROSS_SIZE,' ');
  m_Check.Stub.TotalDisc.Trim();  m_Check.Stub.TotalDisc.PadLeft (STUB_TTLDISC_SIZE, ' ');
  m_Check.Stub.TotalNet.Trim();   m_Check.Stub.TotalNet.PadLeft  (STUB_TTLNET_SIZE,  ' ');

  m_Check.Body.Date.Trim();

  m_Check.Body.Number.Trim();     m_Check.Body.Number.PadLeft(BODY_NUMBER_SIZE,' ');

//--* Amount is already formatted
//  m_Check.Body.Amount.Trim();
//  m_Check.Body.Amount.PadLeft(15,' ');

  m_Check.Body.SpellAmt[0].Trim();
  m_Check.Body.SpellAmt[1].Trim();

  m_Check.Body.VenName.Trim();

  for (l_Index=0; l_Index < BUDLP_MAX_VENADDR; l_Index++)
    m_Check.Body.VenAddr[l_Index].Trim();

  //--* Reset template to top for next check
  m_Template.LineCur = 0;

  pa_ChkNum = m_Check.Body.Number;
  pa_ChkNum.Trim();

  return (BUDLP_STAT_OK);
  }


/*
 *  Returns the next formatted check print line
**/
LB_Word_T               GemBudLP_T::NextPrintLine(LB_String_T& pa_Line)
  {
  LB_String_T   l_BldLine;        //--* Build    Text Line
  LB_String_T   l_CktLine;        //--* Template Text Line
  LB_Word_T     l_CktLineIndex;   //--* Template
  LB_String_T   l_FldName;
  LB_String_T   l_FldValue;
  LB_Word_T     l_Index;
  LB_Bool_T     StubIncFlag;


  pa_Line.Clear();

  //--* Check if no template
  if (m_Template.LineCnt == 0)
    return (BUDLP_STAT_EOF);

  //--* Check if need next check
  //-----* LineCur ranges from 0..LineCnt-1
  if (m_Template.LineCur >=  m_Template.LineCnt)
    return (BUDLP_STAT_EOC);

  l_BldLine.Clear();

  l_CktLine = m_Template.Lines[m_Template.LineCur];

  StubIncFlag = LB_FALSE;

  //--* Loop through template line until a template field is found
  l_CktLineIndex = 0;
  while (l_CktLineIndex < l_CktLine.Size())
    {
    //--* Loop until field found
    while (l_CktLineIndex < l_CktLine.Size())
      {
      if (l_CktLine[l_CktLineIndex] == '<')
        break;
      else
        {
        l_BldLine.Append((LB_Byte_T) l_CktLine[l_CktLineIndex]);
        l_CktLineIndex++;
        }
      }

    if (l_CktLineIndex < l_CktLine.Size())
      {
      //--* Get field name
      l_FldName.Clear();
      if (l_CktLine[l_CktLineIndex] == '<')
        {
        l_CktLineIndex++;  //--* Skip past < char

        while (l_CktLineIndex < l_CktLine.Size())
          {
          if (l_CktLine[l_CktLineIndex] == '>')
            break;
          else
            {
            l_FldName.Append((LB_Byte_T) l_CktLine[l_CktLineIndex]);
            l_CktLineIndex++;
            }
          }

        //--* Get Field Value
        l_FldValue = "";
        l_FldName.UpperCase();

        if (l_FldName == "STUB.VENNUMBER")
          l_FldValue = m_Check.Stub.VenNumber;

        else if (l_FldName == "STUB.VENNAME")
          l_FldValue = m_Check.Stub.VenName;

        else if (l_FldName == "STUB.DESCRIPTION")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].Description;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.DATE")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].Date;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.INVOICE")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].Invoice;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.AMTGROSS")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].AmtGross;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.AMTDISC")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].AmtDisc;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.AMTNET")
          {
          l_FldValue  = m_Check.Stub.Lines[m_Check.Stub.LineCur].AmtNet;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.TTLGROSS")
          {
          l_FldValue  = m_Check.Stub.TotalGross;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.TTLDISC")
          {
          l_FldValue  = m_Check.Stub.TotalDisc;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "STUB.TTLNET")
          {
          l_FldValue  = m_Check.Stub.TotalNet;
          StubIncFlag = LB_TRUE;
          }

        else if (l_FldName == "BODY.DATE")
          l_FldValue = m_Check.Body.Date;

        else if (l_FldName == "BODY.NUMBER")
          l_FldValue = m_Check.Body.Number;

        else if (l_FldName == "BODY.AMOUNT")
          l_FldValue = m_Check.Body.Amount;

        else if (l_FldName == "BODY.SPELLAMT1")
          l_FldValue = m_Check.Body.SpellAmt[0];

        else if (l_FldName == "BODY.SPELLAMT2")
          l_FldValue = m_Check.Body.SpellAmt[1];

        else if (l_FldName == "BODY.VENNAME")
          l_FldValue = m_Check.Body.VenName;

        else if (l_FldName == "BODY.VENADDRESS")
          {
          l_FldValue    = m_Check.Body.VenAddr[m_Check.Body.VenAddrLine];
          m_Check.Body.VenAddrLine++;
          }

        //--* Move Field Value to build line
        l_BldLine.Append(l_FldValue);

        //--* Skip past end of field symbol ">"
        l_CktLineIndex++;
        }
      }
    }

  m_Template.LineCur++;

  //--* If a stub field was used, move to the next stub line
  if (StubIncFlag == LB_TRUE)
    {
    m_Check.Stub.LineCur++;
    if (m_Check.Stub.LineCur > BUDLP_MAX_STUB_LINES)
      m_Check.Stub.LineCur = 0;
    StubIncFlag = LB_FALSE;
    }

//printf("\nBuild  line:"); l_BldLine.DiagDisplay();
//printf("\nTPlate line:"); m_Template.Lines[m_Template.LineCur-1].DiagDisplay();
//getchar();


  pa_Line = l_BldLine;

  return (BUDLP_STAT_OK);
  }
