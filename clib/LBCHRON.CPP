/*  lbchron.cpp
 *
 *  Chronograph (stopwatch) routines
 *
 *
 *                       * PROPRIETARY PROGRAM MATERIAL *
 *
 *      NOTICE: THIS MATERIAL IS PROPRIETARY TO UNITED COMPUTER
 *      AND IS NOT TO BE REPRODUCED, USED OR DISCLOSED EXCEPT
 *      UPON WRITTEN AUTHORIZATION OF:
 *
 *                      UNITED COMPUTER SALES & SERVICE, INC.
 *                      100 DOBBS LANE, SUITE 208
 *                      CHERRY HILL, NEW JERSEY 08034
 *                      609.795.7330
 *
 *            COPYRIGHT (C) 1995-1996 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * This class provides a Chronograph for an application.  It works like a
 * stopwatch. It provides timing for laps (time since last lap).
 * It can be stopped, restarted and reset to zero.
 *
 * This class uses the ANSI C clock() function to get the time.
 * The clock() function returns a clock_t that is the number of clock ticks
 * since a program started. The lap and elapsed times are calculated as the
 * difference between two clock_t values.  If the program is a
 * long-running one, the clock_t may overflow. So, don't use this for timing
 * longer than the value of MAX_LONG divided by the ANSI C macro
 * CLOCKS_PER_SECOND.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      08/21/96        LR      -Initial Release
**/


#include <lb_chron.hpp>

LB_DWord_T                      LB_Chronograph_T::m_ClockDiff(clock_t pa_ClockStart,
                                                              clock_t pa_ClockEnd)
  {
  return ((pa-ClockEnd - pa_ClockStart) / CLOCKS_PER_SEC);
  }


LB_Chronograph_T::Chronograph()
  {
  Reset();
  Start();
  }


LB_Chronograph_T::~Chronograph()
  {
  }

/*
 * Start() either starts the Chronograph at the current time or restarts the
 * Chronograph, adjusting the start and lastlap times by the amount of time
 * that the chrono was stopped.
 *
 * Calling Start() while the Chronograph is running restarts it at 0.
 *
 */
LB_DWord_T                      LB_Chronograph_T::Start(void)
  {
  clock_t ClockCurrent;
  clock_t ClockStoppedLength;

  ClockCurrent = clock();
  if (ClockCurrent == (clock_t) 0)
    ClockCurrent++;

  if (m_ClockStopped != (clock_t) 0)
    {
    ClockStoppedLength = ClockCurrent - m_ClockStopped;
    m_ClockStart   += ClockStoppedLength;
    m_ClockLastLap += ClockStoppedLength;

    m_ClockStopped = (clock_t) 0;

    return Elapsed();
    }
  else
    {
    m_ClockLastLap     = ClockCurrent;
    m_ClockStart       = ClockCurrent;
    m_ClockElapsedTime = 0;

    return (m_ClockElapsed);
    }
  }

/*
 *
 * Stop() records the clock value when the Chronograph is stopped.
 * When stopped, the recorded value is used in the elapsed time
 * calculation.  The Chronograph remains stopped until the Start()
 * function is called to restart it.
 *
 */
LB_DWord_T              LB_Chronograph_T::Stop(void)
  {
  if (m_ClockStopped != (clock_t) 0)
    {
    m_ClockStopped = clock();

    if (m_ClockStopped == (clock_t) 0)
      m_ClockStopped++;
    }

  return (Elapsed());
  }

/*
 * Reset() initializes all times to the current time, and stops the
 * Chronograph. The Start() function must be called to restart it.
 *
 */
LB_DWord_T              LB_Chronograph_T::Reset(void)
  {
  clock_t ClockCurrent;

  ClockCurrent = clock();

  if (ClockCurrent == (clock_t) 0)
    ClockCurrent++;

  m_ClockLastLap = ClockCurrent;
  m_ClockStart   = ClockCurrent;
  m_ClockStopped = ClockCurrent;
  m_ElapsedTime  = 0;

  return (m_ElapsedTime);
  }

/*
 * Lap() returns the number of seconds since Lap() was last called.
 *
 */
LB_DWord_T                      LB_Chronograph_T::Lap(void)
  {
  clock_t           ClockCurrent;
  LB_DWord_T        LapTime;

  if (m_ClockStopped != (clock_t) 0)
    {
    ClockCurrent = m_ClockStopped;
    }
  else
    {
    ClockCurrent = clock();

    if (ClockCurrent == (clock_t) 0)
      ClockCurrent++;
    }

  LapTime = m_ClockDiff(m_lClockLastLap, ClockCurrent);

  if (m_ClockStopped != (clock_t) 0)
    m_ClockLastLap = ClockCurrent;

  return (LapTime);
  }

 /*
  * IsStopped() returns the LB_TRUE if the Chronograph is stopped,
  * and LB_FALSE if it is running.
  *
  */
LB_Bool_T                      LB_Chronograph_T::IsStopped(void)
  {
  if (m_ClockStopped == (clock_t) 0)
    return (LB_FALSE);
  else
    return (LB_TRUE);
  }

/*
 * Elapsed() returns the elapsed time in seconds that the Chronograph has
 * been running.
 *
 */
LB_DWord_T                      LB_Chronograph_T::Elapsed(void)
  {
  clock_t ClockCurrent;

  if (m_ClockStopped != (clock_t) 0)
    {
    ClockCurrent = m_ClockStopped;
    }
  else
    {
    ClockCurrent = clock();

    if (ClockCurrent == (clock_t) 0)
      ClockCurrent++;
    }

    m_ElapsedTime = m_ClockDiff(m_ClockStart, ClockCurrent);

   return (m_ElapsedTime);
   }

/*
 * ElapsedHMS() returns the elapsed time in seconds that the Chronograph
 * has been running.  In addition, it breaks out the elapsed time into
 * hours, minutes, and seconds.  Those are returned in the parameters
 * passed in.
 *
 */
LB_DWord_T              LB_Chronograph_T::ElapsedHMS(LB_DWord_T &pa_Hours,
                                                     LB_DWord_T &pa_Mins,
                                                     LB_DWord_T &pa_Secs)
  {
  const LB_DWord_T c_Hour    = 3600L;
  const LB_DWord_T c_Minute  = 60L;
  LB_DWord_T ElapsedTime     = 0;
  LB_DWord_T Work            = 0;

  ElapsedTime = Elapsed();

  if (ElapsedTime < c_Minute)
    {
    *pa_Hours = 0;
    *pa_Mins  = 0;
    *pa_Secs  = ElapsedTime;
    }
  else if (ElapsedTime < c_Hour)
    {
    *pa_Hours = 0;
    *pa_Mins  = (LB_DWord_T) (ElapsedTime / c_Minute);
    *pa_Secs  = ElapsedTime - (*pa_Mins * c_Minute);
    }
  else
    {
    Work = ElapsedTime;
    *pa_Hours = (LB_DWord_T) (ElapsedTime / c_Hour);
    Work     -= *pa_Hours * c_Hour;
    *pa_Mins  = (LB_DWord_T) (Work / c_Minute);
    Work     -= *pa_Mins * c_Minute;
    *pa_Secs  = Work;
    }

  return (ElapsedTime);
  }
