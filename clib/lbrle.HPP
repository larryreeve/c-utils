#ifndef  LB_RLE_HPP
#define  LB_RLE_HPP

#include <lbport.h>
#include <lbfio.hpp>
#include <lbstring.hpp>

class LB_RLE_T
  {
  public:
    LB_RLE_T();

    ~LB_RLE_T();

    LB_Bool_T           Compress(LB_String_T& pa_InpFileName,
                                 LB_String_T& pa_OutFileName);

    LB_Bool_T           Decompress(LB_String_T& pa_InpFileName,
                                   LB_String_T& pa_OutFileName);

    void                GetErrorMessage(LB_String_T& pa_ErrMsg);


  private:
    LB_String_T         m_SIGNATURE;
    LB_Byte_T           m_SENTINEL;

    LB_String_T         m_ErrMsg;
    LB_DWord_T          m_BytesInp;
    LB_DWord_T          m_BytesOut;
    LB_Fio_T            m_InpFile;
    LB_Fio_T            m_OutFile;
  };
#endif
