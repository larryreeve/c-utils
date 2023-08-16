#ifndef  LB_PHONICS_HPP
#define  LB_PHONICS_HPP

#include <lbport.h>
#include <lbstring.hpp>

class LB_Phonics_T
  {
  public:
    LB_Phonics_T();

    LB_String_T         Soundex(LB_String_T& pa_InputStr);

  private:
    LB_Byte_T           m_SndxTable[26];
  };
#endif
