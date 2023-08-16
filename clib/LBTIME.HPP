#ifndef  LB_TIME_HPP
#define  LB_TIME_HPP


#include <lbport.h>
#include <lbstring.hpp>


class LB_Time_T
  {
  public:
    LB_Bool_T           IsNothing();

    LB_Bool_T           SetTo(LB_Word_T pa_Hour,  LB_Word_T pa_Minute,
                              LB_Word_T pa_Second,LB_Word_T pa_Millisecond);

    LB_Bool_T           SetToNow(void);

    void                SetToNothing(void);

    LB_Word_T           GetHour();

    LB_Word_T           GetMinute();

    LB_Word_T           GetSecond();

    LB_Word_T           GetMillisecond();

    void                GetSortTimeLong(LB_String_T& pa_String);

    void                GetSortTimeShort(LB_String_T& pa_String);


  private:
    LB_Word_T           m_Hour;
    LB_Word_T           m_Minute;
    LB_Word_T           m_Second;
    LB_Word_T           m_Millisecond;
  };
#endif
