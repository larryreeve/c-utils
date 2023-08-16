#ifndef LB_MONEY_HPP
#define LB_MONEY_HPP

#include <lbport.h>

#ifdef __TCPLUSPLUS__
#include <bcd.h>
#endif

class LB_Money_T
  {
  public:
    LB_Money_T();


    ~LB_Money_T();

    LB_Money_T operator + (LB_FloatPt_T pa_Value);
    void operator = (LB_FloatPt_T pa_Value);

    LB_FloatPt_T AsFloatPt();

  private:
    bcd                         m_Value;
};
#endif
