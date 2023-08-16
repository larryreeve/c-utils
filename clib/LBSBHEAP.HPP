#ifndef LB_SBHEAP_HPP
#define LB_SBHEAP_HPP


#include <lbport.h>

class LB_SBHeap_T
  {
  public:
    LB_SBHeap_T(int (*ComparisonFunction)(void *, void *));
    ~LB_SBHeap_T();
    void                Insert(void *);
    void                *Extract(void);

  private:
    void                **m_elms;
    int                 m_n;
    int                 m_maxsize;
    int                 (*m_comp)(void *, void *);
    void                m_SiftDown();
    void                m_SiftUp();
  };
#endif
