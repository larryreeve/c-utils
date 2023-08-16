#ifndef  LB_DATE_HPP
#define  LB_DATE_HPP


#include <lbport.h>
#include <lbstring.hpp>


class LB_Date_T
  {
  public:
    LB_Date_T();

    LB_Bool_T           IsNothing();

    LB_Bool_T   	SetTo(LB_Word_T pa_Month, LB_Word_T pa_Day, LB_Word_T pa_Year);

    LB_Bool_T   	SetToToday(void);

    void        	SetToNothing(void);

    LB_Word_T   	GetMonth(void);
    void                GetMonth(LB_String_T& pa_String);

    LB_Word_T   	GetDay(void);
    void                GetDay(LB_String_T& pa_String);

    LB_Word_T   	GetYear(void);
    void                GetYear(LB_String_T& pa_String);

    LB_Word_T   	GetYearShort(void);
    void                GetYearShort(LB_String_T& pa_String);

    void                GetLongMonth(LB_String_T& pa_String);
    void                GetShortMonth(LB_String_T& pa_String);

    LB_Word_T   	GetDayOfWeekAsWord();
    void                GetDayOfWeek(LB_String_T& pa_String);

    void                GetSortDateShort(LB_String_T& pa_String);
    void                GetSortDateLong(LB_String_T& pa_String);

  private:
    LB_Word_T           m_Month;
    LB_Word_T           m_Day;
    LB_Word_T           m_Year;
    char                m_ShortMonths[12][4];
  };
#endif
