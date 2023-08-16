#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <lbthread.hpp>


#define MIN_STACKSIZE           400     // minimum stack size


LB_Thread_T m_MainThread;
LB_Thread_T *m_CurThread;        // main and current thread

static LB_ThreadQueue_T m_ReadyQ;                     // ready queue

static jmp_buf m_TmpJmpBuffer;                // temporary jump buffer


static void Schedule()                  // run next task
  {
  m_CurThread = m_ReadyQ.Head();

  if(m_CurThread == NULL )                // no task to run
    {
    puts("Deadlock!");
    exit(1);
    }

  longjmp(m_CurThread->JmpBuffer, 1);         // restore state of next task
  }


static LB_Word_T StackDistance(LB_Thread_T *pa_FromPtr, LB_Thread_T *pa_ToPtr)
  {
  char *c1, *c2;

  c1 = (char *) pa_FromPtr;
  c2 = (char *) pa_ToPtr;
  if (c1 > c2)                         // stack grows down
    return((LB_Word_T) (c1 - c2));
  else                                  // stack grows up
    return((LB_Word_T) (c2 - c1));
  }


static void StackEat(LB_Thread_T *pa_ThreadPtr, LB_Word_T pa_Size )
  {
  LB_Word_T  Distance;
  LB_Thread_T Thread;

  Distance = StackDistance(pa_ThreadPtr, &Thread );

  if (Distance < pa_Size )                        // eat stack
    StackEat(pa_ThreadPtr, pa_Size);

  Thread.Size = pa_ThreadPtr->Size - Distance;                 // set sizes
  pa_ThreadPtr->Size = Distance;
  Thread.Used = LB_FALSE;
  Thread.NextThreadPtr = pa_ThreadPtr->NextThreadPtr;                     // set link pointers
  pa_ThreadPtr->NextThreadPtr = &Thread;

  if (setjmp(Thread.JmpBuffer) == 0 )           // wait
    longjmp(pa_ThreadPtr->JmpBuffer, 1);

  for ( ;; )
    {
    if (Thread.StackSize + MIN_STACKSIZE <= Thread.Size )        // test size
      {
      if (setjmp(Thread.JmpBuffer) == 0)       // split block
        StackEat(&Thread,Thread.StackSize );
      }

    Thread.Used = LB_TRUE;                      // mark as used

    if (setjmp(Thread.JmpBuffer) == 0)         // wait
      longjmp(m_TmpJmpBuffer, 1);

    (*Thread.FuncPtr)(Thread.ArgPtr);                  // run task

    Thread.Used = LB_FALSE;                     // mark as free
    if ((Thread.NextThreadPtr != NULL) && (Thread.NextThreadPtr->Used == LB_FALSE))
      {
      Thread.Size = Thread.Size + Thread.NextThreadPtr->Size;           // merge with following block
      Thread.NextThreadPtr = Thread.NextThreadPtr->NextThreadPtr;
      }

    pa_ThreadPtr = m_MainThread.NextThreadPtr;                // loop through list
    if( pa_ThreadPtr != &Thread )                       // if not first block
      {
      while (pa_ThreadPtr->NextThreadPtr != &Thread )            // locate previous block
        pa_ThreadPtr = pa_ThreadPtr->NextThreadPtr;

      if(pa_ThreadPtr->Used == LB_FALSE)                    // if free
        {
        pa_ThreadPtr->Size = pa_ThreadPtr->Size + Thread.Size;              // then merge
        pa_ThreadPtr->NextThreadPtr = Thread.NextThreadPtr;
        }
      }

    if (setjmp(Thread.JmpBuffer) == 0 )         // save state
      Schedule();
    }
  }


void Thread_Initialize(LB_Word_T pa_TotalStack, LB_Word_T pa_MainStack)
  {
  LB_Thread_T Thread;

  Thread.Size           = pa_TotalStack;               // initialize total stack area
  Thread.NextThreadPtr  = NULL;

  if (setjmp(Thread.JmpBuffer) == 0 )         // reserve main stack
    StackEat(&Thread,pa_MainStack);

  m_MainThread = Thread;                      // copy to global variable
  m_MainThread.Used = LB_TRUE;
  m_CurThread = &m_MainThread;
  }


LB_Bool_T Thread_New(LB_ThreadFuncPtr_T pa_FuncPtr, void *pa_ArgPtr, LB_Word_T pa_StackSize)
  {
  LB_Thread_T *ThreadPtr;

  for (ThreadPtr = &m_MainThread; ThreadPtr != NULL; ThreadPtr = ThreadPtr->NextThreadPtr)     // find free block
    {
    if ((ThreadPtr->Used == LB_FALSE) && (ThreadPtr->Size >= pa_StackSize))
      {
      ThreadPtr->FuncPtr   = pa_FuncPtr;                     // set task parameters
      ThreadPtr->ArgPtr    = pa_ArgPtr;
      ThreadPtr->StackSize = pa_StackSize;

      if (setjmp(m_TmpJmpBuffer) == 0 )     // activate control block
        longjmp(ThreadPtr->JmpBuffer, 1);

      m_ReadyQ.Append(ThreadPtr);

      return(LB_TRUE);
      }
    }
  return (LB_FALSE);                      // not enough stack
  }


void Thread_Yield()
  {
  if (setjmp(m_CurThread->JmpBuffer ) == 0)  // save state
    {
    m_ReadyQ.Append(m_CurThread);
    Schedule();                         // run next task
    }
  }


LB_ThreadQueue_T::LB_ThreadQueue_T()
  {
  HeadPtr = NULL;
  }


LB_Thread_T             *LB_ThreadQueue_T::Head()                  // return first element in queue
  {
  LB_Thread_T *ThreadPtr;

  ThreadPtr = HeadPtr;

  if (ThreadPtr != NULL)
    HeadPtr = HeadPtr->ChainPtr;

  return (ThreadPtr);
  }


void                    LB_ThreadQueue_T::Append(LB_Thread_T *pa_ThreadPtr)         // append element to queue
  {
  if (HeadPtr == NULL )
    HeadPtr = pa_ThreadPtr;
  else
    TailPtr->ChainPtr = pa_ThreadPtr;

  TailPtr = pa_ThreadPtr;

  pa_ThreadPtr->ChainPtr = NULL;
  }


LB_ThreadSema_T::LB_ThreadSema_T()                // initialize semaphore
  {
  Count = 0;
  }


void                    LB_ThreadSema_T::Signal()              // signal on semaphore
  {
  LB_Thread_T *ThreadPtr;

  ThreadPtr = Head();

  if (ThreadPtr != NULL )                       // a task was waiting
    m_ReadyQ.Append(ThreadPtr);
  else                                  // no task was waiting
    Count++;
  }


void                    LB_ThreadSema_T::Wait()                // wait on semaphore
  {
  if (Count > 0 )                       // don't block
    Count--;
  else                                  // block on semaphore
    {
    if (setjmp(m_CurThread->JmpBuffer) == 0)
      {
      Append(m_CurThread);
      Schedule();
      }
    }
  }
