#ifndef  LB_TSTAMP_HPP
#define  LB_TSTAMP_HPP


#include <lbport.h>
#include <lbdate.hpp>
#include <lbtime.hpp>


class LB_TStamp_T
  {
  public:
        LB_Bool_T       IsNothing();
        void            SetDate(LB_Date_T pa_Date);
        void            SetTime(LB_Time_T pa_Time);
        void            SetToNothing();
        void            SetToNow();

  private:
        LB_Date_T       m_Date;
        LB_Time_T       m_Time;
  };
#endif
