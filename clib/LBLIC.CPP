/*  lblic.cpp
 *
 *  Licensing routines
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
 *      04/02/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
**/

#define LICENSE_FILENAME        "united.dat"

/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <memory.h>
#include <stdio.h>
#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <lblic.hpp>


/*
 *
**/
void LB_License_T::DumpStruct()
  {
  int i;

  printf("\nSignatureA: [");
  for (i=0; i < 2; i++)
    printf("%c",m_GemLicense.SignatureA[i]);
  printf("]");

  printf("\n   Account: [");
  for (i=0; i < 40; i++)
    printf("%c",m_GemLicense.AccountName[i]);
  printf("]");

  printf("\nSignatureB: [");
  for (i=0; i < 2; i++)
    printf("%c",m_GemLicense.SignatureB[i]);
  printf("]");

  getchar();
  }

/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_License_T::LB_License_T()
  {
  }


/*
 *
**/
LB_License_T::~LB_License_T()
  {
  m_LicFile.Close();
  }


/*
 *
**/
LB_Word_T               LB_License_T::GemCreate(LB_String_T& pa_FileName)
  {
  LB_Bool_T     l_LicExist;
  LB_String_T   l_Str;

  l_Str = LICENSE_FILENAME;
  m_LicFile.SetFileName(pa_FileName);

  if (m_LicFile.Exists() == LB_FALSE)
    m_LicFile.Create();

  if (m_LicFile.Exists() == LB_FALSE)
    return (GEMLIC_NOLICFILE);

  if (m_LicFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (GEMLIC_NOLICFILE);

  memset(&m_GemLicense,' ',sizeof(m_GemLicense));

  //--* Read file to see if it already licensed
  if (m_LicFile.SeekFromStart(m_LicFile.Size() - sizeof(m_GemLicense)) == LB_FALSE)
    {
    m_LicFile.Close();
    return (LB_FALSE);
    }

  m_LicBuf.PadRight(sizeof(m_GemLicense),' ');
  m_LicBuf.Fill(' ');

  if (m_LicFile.Read(m_LicBuf) != sizeof(m_GemLicense))
    {
    m_LicFile.Close();
    return (GEMLIC_NOLICFILE);
    }

  m_LicBuf = m_CryptEngine.Decrypt(m_LicBuf);

  m_LicBuf.GetBytes((LB_Byte_T *) &m_GemLicense,sizeof(m_GemLicense));

  //--* Is file already licensed?
  if ((m_GemLicense.SignatureA[0] == 'L') &&
      (m_GemLicense.SignatureA[1] == 'R') &&
      (m_GemLicense.SignatureB[0] == 'C') &&
      (m_GemLicense.SignatureB[1] == 'R'))
    l_LicExist = LB_TRUE;
  else
    l_LicExist = LB_FALSE;

  memset(&m_GemLicense,' ',sizeof(m_GemLicense));

  //--* Get Installation Account Name
  if (m_GemCtlFile.Read(1) == LB_FALSE)
    return (GEMLIC_NOCTLFILE);

  m_GemCtlFile.GetAccountName(m_CtlAccountName);
  m_CtlAccountName.PadRight(sizeof(m_GemLicense.AccountName),' ');

  //--* Set signatures
  m_GemLicense.SignatureA[0] = 'L';  m_GemLicense.SignatureA[1] = 'R';
  m_GemLicense.SignatureB[0] = 'C';  m_GemLicense.SignatureB[1] = 'R';

  //--* Set account name
  m_CtlAccountName.GetBytes(m_GemLicense.AccountName,sizeof(m_GemLicense.AccountName));

  //--* Calculate CRC
  m_GemLicense.Crc32 = m_DigSig.Crc32(m_CtlAccountName);

  //--* Write out license
  if (l_LicExist == LB_TRUE)
    m_LicFile.SeekFromStart(m_LicFile.Size() - sizeof(m_GemLicense));
  else
    m_LicFile.SeekFromEOF(0);

  m_LicBuf.Clear();
  m_LicBuf.SetBytes((LB_Byte_T *) &m_GemLicense,sizeof(m_GemLicense));

  m_LicBuf = m_CryptEngine.Encrypt(m_LicBuf);

  if (m_LicFile.Write(m_LicBuf) != sizeof(m_GemLicense))
    {
    m_LicFile.Close();
    return (GEMLIC_CREATFAILED);
    }

  m_LicFile.Close();

  return (GEMLIC_CREATOK);
  }


LB_Bool_T               LB_License_T::GemGetAcctName(LB_String_T& pa_FileName,
                                                     LB_String_T& pa_LicenseeStr)
  {
  //--* Read License from File
  m_LicFile.SetFileName(pa_FileName);

  if (m_LicFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_LicFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  memset(&m_GemLicense,' ',sizeof(m_GemLicense));

  if (m_LicFile.SeekFromStart(m_LicFile.Size() - sizeof(m_GemLicense)) == LB_FALSE)
    {
    m_LicFile.Close();
    return (LB_FALSE);
    }

  m_LicBuf.PadRight(sizeof(m_GemLicense),' ');
  m_LicBuf.Fill(' ');
  if (m_LicFile.Read(m_LicBuf) != sizeof(m_GemLicense))
    {
    m_LicFile.Close();
    return (LB_FALSE);
    }
  m_LicFile.Close();

  m_LicBuf = m_CryptEngine.Decrypt(m_LicBuf);

  m_LicBuf.GetBytes((LB_Byte_T *) &m_GemLicense,sizeof(m_GemLicense));

  //--* Verify signatures
  if ((m_GemLicense.SignatureA[0] != 'L') ||
      (m_GemLicense.SignatureA[1] != 'R') ||
      (m_GemLicense.SignatureB[0] != 'C') ||
      (m_GemLicense.SignatureB[1] != 'R'))
    return (LB_FALSE);

  //--* Get Licensed Account Name
  m_LicAccountName.SetBytes(m_GemLicense.AccountName,sizeof(m_GemLicense.AccountName));

  //--* Verify CRC
  m_LicCrc = m_DigSig.Crc32(m_LicAccountName);

  if (m_LicCrc != m_GemLicense.Crc32)
    return(LB_FALSE);

  //--* Get Installation Account Name
  if (m_GemCtlFile.Read(1) == LB_FALSE)
    return (LB_FALSE);

  m_GemCtlFile.GetAccountName(m_CtlAccountName);

  //--* Does installation name match
  if (m_LicAccountName != m_CtlAccountName)
    return (LB_FALSE);

  pa_LicenseeStr = m_LicAccountName;

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               LB_License_T::GemVerify(LB_String_T& pa_FileName)
  {
  m_LicFile.SetFileName(pa_FileName);

  if (m_LicFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_LicFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  memset(&m_GemLicense,' ',sizeof(m_GemLicense));

  //--* Read file to see if it already licensed
  if (m_LicFile.SeekFromStart(m_LicFile.Size() - sizeof(m_GemLicense)) == LB_FALSE)
    {
    m_LicFile.Close();
    return (LB_FALSE);
    }

  m_LicBuf.PadRight(sizeof(m_GemLicense),' ');
  m_LicBuf.Fill(' ');

  if (m_LicFile.Read(m_LicBuf) != sizeof(m_GemLicense))
    {
    m_LicFile.Close();
    return (GEMLIC_NOLICFILE);
    }

  m_LicBuf = m_CryptEngine.Decrypt(m_LicBuf);

  m_LicBuf.GetBytes((LB_Byte_T *) &m_GemLicense,sizeof(m_GemLicense));

  //--* Verify signatures
  if ((m_GemLicense.SignatureA[0] != 'L') ||
      (m_GemLicense.SignatureA[1] != 'R') ||
      (m_GemLicense.SignatureB[0] != 'C') ||
      (m_GemLicense.SignatureB[1] != 'R'))
    return (LB_FALSE);

  //--* Get Licensed Account Name
  m_LicAccountName.SetBytes(m_GemLicense.AccountName,sizeof(m_GemLicense.AccountName));

  //--* Verify CRC
  m_LicCrc = m_DigSig.Crc32(m_LicAccountName);

  if (m_LicCrc != m_GemLicense.Crc32)
    return(LB_FALSE);

  //--* Get Installation Account Name
  if (m_GemCtlFile.Read(1) == LB_FALSE)
    return (LB_FALSE);

  m_GemCtlFile.GetAccountName(m_CtlAccountName);

  //--* Does installation name match
  if (m_LicAccountName != m_CtlAccountName)
    return (LB_FALSE);

  return (LB_TRUE);
  }
