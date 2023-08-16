#ifndef  LB_FCALC_HPP
#define  LB_FCALC_HPP


#include <bcd.h>
#include <lbport.h>


class LB_FinCalc_T
  {
  public:
    LB_FloatPt_T        Add(LB_FloatPt_T pa_Op1,
                            LB_FloatPt_T pa_Op2);
  };
#endif
