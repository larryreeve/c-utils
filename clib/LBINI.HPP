#ifndef  LB_INIFILE_HPP
#define  LB_INIFILE_HPP


#include <lbport.h>
#include <lbcrypt.hpp>
#include <lbfio.hpp>
#include <lbstring.hpp>


class LB_IniFile_T
  {
  public:
    LB_IniFile_T();

    ~LB_IniFile_T();

    LB_Bool_T           Create();

    LB_Bool_T           CreateSection(LB_String_T& pa_Section);

    LB_Bool_T           DeleteSection(LB_String_T& pa_Section);

    void                Decode();

    void                Encode();

    LB_Bool_T           Exists();

    LB_Bool_T           Load();

    LB_Bool_T           ReadValue(LB_String_T& pa_Section,
                                  LB_String_T& pa_Ident,
                                  LB_String_T& pa_Value);

    LB_Bool_T           Remove();


    LB_Bool_T           WriteValue(LB_String_T& pa_Section,
                                   LB_String_T& pa_Ident,
                                   LB_String_T& pa_Value);

    void                SetFileName(LB_String_T&  pa_Filename);

  private:
    LB_String_T         m_FileName;
    LB_String_T         m_FileBuf;
    LB_Fio_T            m_File;
    LB_Word_T           m_CurPos;
    LB_Crypt_T          m_CryptEngine;

    LB_Bool_T           m_NextLine(LB_String_T& pa_NextLine);
  };
#endif
