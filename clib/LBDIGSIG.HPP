#ifndef  LB_DIGSIG_HPP
#define  LB_DIGSIG_HPP


#include <lbport.h>
#include <lbstring.hpp>


class LB_DigSig_T
  {
  public:
    LB_DigSig_T();

    ~LB_DigSig_T();

    LB_DWord_T          Crc32(LB_String_T& pa_InputStr);

    void                Crc32_Reset();
    void                Crc32_Add(LB_Byte_T pa_Byte);
    LB_DWord_T          Crc32();

  private:
    LB_DWord_T          m_Crc32Table[256];
    LB_DWord_T          m_Crc32;
  };
#endif
