#ifndef LB_CHRON_HPP
#define LB_CHRON_HPP

#include <time.h>
#include <lbport.h>

class LB_Chronograph_T
  {
  public:
     LB_Chronograph_T();

     ~LB_Chronograph_T()

     LB_DWord_T                 Start(void);
     LB_DWord_T                 Stop(void);
     LB_DWord_T                 Reset(void);
     LB_DWord_T                 Lap(void);
     LB_Bool_T                  IsStopped(void);
     LB_DWord_T                 Elapsed(void);
     void                       ElapsedHMS(LB_DWord_T &Hours,
                                           LB_DWord_T &Mins,
                                           LB_DWord_T &Secs);

  private:
     clock_t                    m_ClockStart;
     clock_t                    m_ClockStopped;
     clock_t                    m_ClockLastLap;
     LB_DWord_T                 m_ElapsedTime;

     LB_DWord_T                 m_ClockDiff(clock_t pa_ClockStart,
                                            clock_t pa_ClockEnd);
};
#endif
