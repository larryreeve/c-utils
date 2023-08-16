#ifndef  LB_CBLCMP_HPP
#define  LB_CBLCMP_HPP


#include <lbport.h>
#include <lbstring.hpp>

class LB_CobolComp_T
  {
  public:
    LB_Bool_T           Decode(LB_FloatPt_T   *pa_Result,
                               LB_String_T&   pa_Bytes,
                               LB_Word_T      pa_NumDecimals,
                               LB_Bool_T      pa_SignFlag);

    LB_Bool_T           Encode(LB_FloatPt_T   pa_Value,
                               LB_Byte_T      *pa_Bytes,
                               LB_Size_T      pa_Size,
                               LB_Word_T      pa_NumDecimals,
                               LB_Bool_T      pa_SignFlag);

  private:
    void                m_SwapBytes(LB_Byte_T *pa_Bytes,
                                    LB_Size_T pa_MaxSize);
  };
#endif
