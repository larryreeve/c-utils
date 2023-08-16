#ifndef  LB_LICENSE_HPP
#define  LB_LICENSE_HPP


#include <lbport.h>
#include <lbcrypt.hpp>
#include <lbdigsig.hpp>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <lbstring.hpp>


#define GEMLIC_NOLICFILE        1
#define GEMLIC_NOCTLFILE        2
#define GEMLIC_CREATOK          3
#define GEMLIC_CREATFAILED      4


class LB_License_T
  {
  public:
    LB_License_T();

    ~LB_License_T();

    LB_Bool_T           GemVerify(LB_String_T& pa_FileName);

    LB_Word_T           GemCreate(LB_String_T& pa_FileName);

    LB_Bool_T           GemGetAcctName(LB_String_T& pa_FileName,
                                       LB_String_T& pa_LicenseeStr);

  private:
    LB_Fio_T            m_LicFile;
    GemControl_T        m_GemCtlFile;
    LB_Crypt_T          m_CryptEngine;
    LB_DigSig_T         m_DigSig;
    LB_DWord_T          m_LicCrc;
    LB_String_T         m_LicBuf;
    LB_String_T         m_LicAccountName;
    LB_String_T         m_CtlAccountName;

    struct GEMLICENSE_T
      {
      LB_Byte_T         SignatureA[2];
      LB_Byte_T         AccountName[40];
      LB_DWord_T        Crc32;
      LB_Byte_T         Filler[80];
      LB_Byte_T         SignatureB[2];
      } m_GemLicense;

    void                DumpStruct();
  };
#endif
