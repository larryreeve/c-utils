#ifndef  LB_ABOUT_HPP
#define  LB_ABOUT_HPP


#include <lbport.h>

#define ABOUT_TITLE1    1
#define ABOUT_TITLE2    2
#define ABOUT_VERSION   3
#define ABOUT_LICENSEE  4
#define ABOUT_DATAPATH  5
#define ABOUT_DEFPRT    6


class LB_About_T
  {
  public:
    LB_About_T();

    ~LB_About_T();

    void                ShowModal();

    void                SetElement(LB_Word_T     pa_ElementID,
                                   LB_CZString_T pa_Data);


  private:
    Window              m_Window;
    char                m_Title1[64];
    char                m_Title2[64];
    char                m_Version[64];
    char                m_Licensee[64];
    char                m_DataPath[64];
    char                m_DefPrt[64];
  };
#endif

