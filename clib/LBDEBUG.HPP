#ifndef  LB_DEBUG_HPP
#define  LB_DEBUG_HPP

#include <lbport.h>

#define         LB_NEW_INITIALIZER      0xA3
#define         LB_DEL_INITIALIZER      0xA5

    void    LB_Debug_AssertFailure(LB_ZString_T pa_File, LB_Word_T pa_LineNum);

    void    LB_Debug_MemInitialize(LB_Byte_T *pa_MemBlock,
                                   LB_Word_T pa_Size,
                                   LB_Byte_T pa_InitByte);

    void   LB_Debug_TraceLog(char * pa_SrcFileName,
                             int    pa_SrcLineNumber,
                             char * pa_FormatStr,
                             ...);

    void   LB_Debug_ErrorLog(char * pa_SrcFileName,
                             int    pa_SrcLineNumber,
                             char * pa_FormatStr,
                             ...);


    #ifdef LB_NODEBUG
        #define       LB_ASSERTFILE(pa_ZString)
        #define       LB_ASSERT(pa_BoolExpression)
        #define       LB_NEW_INITIALIZE(pa_MemBlock, pa_MemSize)
        #define       LB_DEL_INITIALIZE(pa_MemBlock, pa_MemSize)
        #define       IFTRACE(CallFn)  0
    #else
        #define       LB_ASSERTFILE(pa_ZString)  static char m_AssertFile[] = pa_ZString;

        #define       LB_ASSERT(pa_BoolExpression)    \
                      if (pa_BoolExpression)          \
                        NULL;                         \
                      else                            \
                        LB_Debug_AssertFailure(m_AssertFile,__LINE__)

        #define       LB_NEW_INITIALIZE(pa_MemBlock,pa_Size)  \
                      LB_Debug_MemInitialize(pa_MemBlock,pa_Size,LB_NEW_INITIALIZER)

        #define       LB_DEL_INITIALIZE(pa_MemBlock,pa_Size)  \
                      LB_Debug_MemInitialize(pa_MemBlock,pa_Size,LB_DEL_INITIALIZER)

        #define       IFTRACE(CallFn)  CallFn
    #endif

    #define Trace0(FmtStr)                                   IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr))
    #define Trace1(FmtStr,a1)                                IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1))
    #define Trace2(FmtStr,a1,a2)                             IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2))
    #define Trace3(FmtStr,a1,a2,a3)                          IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3))
    #define Trace4(FmtStr,a1,a2,a3,a4)                       IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4))
    #define Trace5(FmtStr,a1,a2,a3,a4,a5)                    IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4,a5))
    #define Trace6(FmtStr,a1,a2,a3,a4,a5,a6)                 IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4,a5,a6))
    #define Trace7(FmtStr,a1,a2,a3,a4,a5,a6,a7)              IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4,a5,a6,a7))
    #define Trace8(FmtStr,a1,a2,a3,a4,a5,a6,a7,a8)           IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4,a5,a6,a7,a8))
    #define Trace9(FmtStr,a1,a2,a3,a4,a5,a6,a7,a8,a9)        IFTRACE(LB_Debug_TraceLog(__FILE__,__LINE__,FmtStr,a1,a2,a3,a4,a5,a6,a7,a8,a9))

#endif
