#ifndef  LB_CRYPT_HPP
#define  LB_CRYPT_HPP


#include <lbport.h>
#include <lbstring.hpp>

class LB_Crypt_T
  {
  public:
    LB_Crypt_T();

    LB_String_T         Encrypt(LB_String_T& pa_InputStr);
    LB_String_T         Decrypt(LB_String_T& pa_InputStr);

  private:
    LB_String_T         m_Key;
    LB_String_T         m_Crypt(LB_String_T& pa_InputStr);
  };
#endif
