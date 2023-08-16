#ifndef LB_THREAD
#define LB_THREAD

#include <setjmp.h>
#include <lbport.h>

typedef void (*LB_ThreadFuncPtr_T)(void *pa_ArgPtr);           // thread function pointer

struct LB_Thread_T
  {
  jmp_buf       JmpBuffer;          // jump state
  LB_Bool_T     Used;               // used or free
  LB_Word_T     Size;               // size of block
  LB_Thread_T    *NextThreadPtr;     // pointer to next control block
  LB_ThreadFuncPtr_T FuncPtr;            // pointer to task function
  void          *ArgPtr;            // argument to task function
  LB_Word_T     StackSize;          // requested stack size

  LB_Thread_T   *ChainPtr;             // next task in ready or semaphore queue
  };


class LB_ThreadQueue_T
  {
  public:
    LB_ThreadQueue_T();
    LB_Thread_T         *Head();
    void                Append(LB_Thread_T *pa_ThreadPtr);

  private:
    LB_Thread_T *HeadPtr;
    LB_Thread_T *TailPtr;          // pointer to first and last element
  };


class LB_ThreadSema_T : private LB_ThreadQueue_T
  {
  public:
    LB_ThreadSema_T();
    void                Signal();
    void                Wait();

  private:
    int                 Count;                  // semaphore counter
  };


extern LB_Thread_T m_MainThread, *m_CurThread;


void                    Thread_Initialize(LB_Word_T pa_TotalStack, LB_Word_T pa_MainStack);

LB_Bool_T               Thread_New(LB_ThreadFuncPtr_T pa_FnPtr, void *pa_ArgPtr, LB_Word_T pa_StackSize);

void                    Thread_Yield();


#endif
