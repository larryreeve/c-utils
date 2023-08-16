/*  lbabout.cpp
 *
 *  About routines
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
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      12/19/95        LR      Initial Release
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <winlib.h>
#include <lbport.h>
#include <lbabout.hpp>


/*---------------------------------------------------------------
  ---                   Module Constants                      ---
  ---------------------------------------------------------------*/



/*---------------------------------------------------------------
  ---                   Private Methods                       ---
	---------------------------------------------------------------*/
/*
 *
**/



/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void                    LB_About_T::LB_About_T()
  {
  m_Window = NULL;

  memset(m_Title1,   '\0',sizeof(m_Title1));
  memset(m_Title2,   '\0',sizeof(m_Title2));
  memset(m_Version,  '\0',sizeof(m_Version));
  memset(m_Licensee, '\0',sizeof(m_Licensee));
  memset(m_DataPath, '\0',sizeof(m_DataPath));
  memset(m_DefPrt,   '\0',sizeof(m_DefPrt));
  }


/*
 *
**/
void                    LB_About_T::~LB_About_T()
  {
  if (m_Window != NULL)
    w_close(m_Window);
  }


/*
 *
**/
void                    LB_About_T::SetElement(LB_Word_T     pa_ElementID,
                                               LB_CZString_T pa_Data)
  {
  switch(pa_ElementID)
    {
    case ABOUT_TITLE1:
      memset(m_Title1,'\0',sizeof(m_Title1));
      if (strlen(pa_Data) < (sizeof(m_Title1)-1))
        memcpy(m_Title1,pa_Data,strlen(pa_Data));
      break;

    case ABOUT_TITLE2:
      memset(m_Title2,'\0',sizeof(m_Title2));
      if (strlen(pa_Data) < (sizeof(m_Title2)-1))
        memcpy(m_Title2,pa_Data,strlen(pa_Data));
      break;

    case ABOUT_VERSION:
      memset(m_Version,'\0',sizeof(m_Version));
      if (strlen(pa_Data) < (sizeof(m_Version)-1))
        memcpy(m_Version,pa_Data,strlen(pa_Data));
      break;

    case ABOUT_LICENSEE:
      memset(m_Licensee,'\0',sizeof(m_Licensee));
      if (strlen(pa_Data) < (sizeof(m_Licensee)-1))
        memcpy(m_Licensee,pa_Data,strlen(pa_Data));
      break;

    case ABOUT_DATAPATH:
      memset(m_DataPath,'\0',sizeof(m_DataPath));
      if (strlen(pa_Data) < (sizeof(m_DataPath)-1))
        memcpy(m_DataPath,pa_Data,strlen(pa_Data));
      break;

    case ABOUT_DEFPRT:
      memset(m_DefPrt,'\0',sizeof(m_DefPrt));
      if (strlen(pa_Data) < (sizeof(m_DefPrt)-1))
        memcpy(m_DefPrt,pa_Data,strlen(pa_Data));
      break;
    }
  }


/*
 *
**/
void                    LB_About_T::ShowModal()
  {
  /*--- Open About Box ---*/
  m_Window = w_open(5,5,70,21);
  w_umessage(m_Window," About ");

  /*--- Display Message ---*/
  w_puts_centered(m_Window,1,m_Title1);
	w_puts_centered(m_Window,2,m_Title2);
  w_puts_centered(m_Window,3,m_Version);

  w_puts_centered(m_Window,5,"Licensed to:");
  w_puts_centered(m_Window,6,m_Licensee);

	w_puts_centered(m_Window,8,"Path to Data is:");
	w_puts_centered(m_Window,9,m_DataPath);

	w_puts_centered(m_Window,11,"Default printer output:");
	w_puts_centered(m_Window,12,m_DefPrt);

  w_puts_centered(m_Window,14,"Copyright (C) 1995-1996 by United Computer Sales & Service, Inc.");
  w_puts_centered(m_Window,15,"All rights reserved.");
  w_puts_centered(m_Window,16,"For application support, call 1-800-242-7403");

	c_add_button(m_Window,"  ~OK  ",33,19,_ENTER,BUTTON_SHADOW_RIGHT);

  c_dialog(m_Window);

  /*--- Close About Box ---*/
  w_close(m_Window);
  }
