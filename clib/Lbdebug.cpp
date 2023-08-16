/*  lbdebug.cpp
 *
 *  Debug routines
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
 *            COPYRIGHT (C) 1995-1997 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      11/20/95        LR      -Initial Release
 *      01/17/96        LR      -Added heapleft message
 *      01/04/97        LR      -Added TraceLog function
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#ifdef __TURBOC__
  #include <alloc.h>
  #include <mem.h>
#endif

#ifdef _MSC_VER
  #include <memory.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lbport.h>


//---------------------------------------------------------------
//---                   Memory Functions                      ---
//---------------------------------------------------------------
void    LB_Debug_AssertFailure(LB_ZString_T pa_File, LB_Word_T pa_LineNum)
{
  FILE * FilePtr;

  FilePtr = fopen("errlog.txt","a+");
  if (FilePtr == (FILE *) NULL)
   return;

  fflush(FilePtr);

  fprintf(FilePtr,"\n\nAssert failed in ");
  if (pa_File == (char *) NULL)
    fprintf(FilePtr,"\n\nunknown source file");
  else
    fprintf(FilePtr,"%s",pa_File);

  fprintf(FilePtr,", line %u",pa_LineNum);

  if (FilePtr == stderr)
    {
    fprintf(FilePtr,"\n\nPress a key to exit.");
    fflush(FilePtr);
    getchar();
    }
  else
    fclose(FilePtr);

  exit(LB_EXIT_FAILURE);
}


void    LB_Debug_MemInitialize(LB_Byte_T *pa_MemBlock,
			       LB_Word_T pa_Size,
			       LB_Byte_T pa_InitByte)
{
  //--- fprintf(stderr,"\nHeapleft = %ld",coreleft()); getchar();
  memset(pa_MemBlock,pa_InitByte,pa_Size);
}


//---------------------------------------------------------------
//---                   Trace Functions                       ---
//---------------------------------------------------------------
void   LB_Debug_TraceLog(char * pa_SrcFileName,
                         int    pa_SrcLineNumber,
                         char * pa_FormatStr,
                         ...)
{
  FILE *        TraceFilePtr;

  va_list       ArgPtr;
  char          MesgBuffer[128];

  char          TimeBuffer[36];
  struct tm *   TimeNow;
  time_t        SecsNow;

  TraceFilePtr = fopen("tracelog.txt","a+");
  if (TraceFilePtr == (FILE *) NULL)
    return;

  //--- Write timestamp, source filename, source line number
  time(&SecsNow);
  TimeNow = localtime(&SecsNow);
  strftime(TimeBuffer, sizeof(TimeBuffer), "%m/%d/%Y at %H:%M:%S%p", TimeNow);
  fprintf(TraceFilePtr,"%s - %s, Line %d\n",TimeBuffer, pa_SrcFileName, pa_SrcLineNumber);

  //--- Write message
  va_start(ArgPtr, pa_FormatStr);
  vsprintf(MesgBuffer, pa_FormatStr, ArgPtr);
  va_end(ArgPtr);
  fprintf(TraceFilePtr,"%s\n\n",MesgBuffer);

  fclose(TraceFilePtr);
}


//---------------------------------------------------------------
//---                   Trace Functions                       ---
//---------------------------------------------------------------
void   LB_Debug_ErrorLog(char * pa_SrcFileName,
                         int    pa_SrcLineNumber,
                         char * pa_FormatStr,
                         ...)
{
  FILE *        ErrorFilePtr;

  va_list       ArgPtr;
  char          MesgBuffer[128];

  char          TimeBuffer[36];
  struct tm *   TimeNow;
  time_t        SecsNow;

  ErrorFilePtr = fopen("errlog.txt","a+");
  if (ErrorFilePtr == (FILE *) NULL)
    return;

  //--- Write timestamp, source filename, source line number
  time(&SecsNow);
  TimeNow = localtime(&SecsNow);
  strftime(TimeBuffer, sizeof(TimeBuffer), "%m/%d/%Y at %H:%M:%S%p", TimeNow);
  fprintf(ErrorFilePtr,"%s - %s, Line %d\n",TimeBuffer, pa_SrcFileName, pa_SrcLineNumber);

  //--- Write message
  va_start(ArgPtr, pa_FormatStr);
  vsprintf(MesgBuffer, pa_FormatStr, ArgPtr);
  va_end(ArgPtr);
  fprintf(ErrorFilePtr,"%s\n\n",MesgBuffer);

  fclose(ErrorFilePtr);
}
