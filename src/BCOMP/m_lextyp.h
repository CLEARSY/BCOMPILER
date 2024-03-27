/******************************* CLEARSY **************************************
* Fichier : $Id: m_lextyp.h,v 2.0 2000-10-19 08:28:00 cc Exp $
*
* (C) 2008 CLEARSY
*
* Description : Definition des lexemes du fichier d'etat d'un composant
*
This file is part of B_COMPILER
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    B_COMPILER is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    B_COMPILER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with B_COMPILER; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#ifdef  ACTION_NG

#ifndef _M_LEXTYP_H_
#define _M_LEXTYP_H_

// type des lexemes du fichier d'etat
typedef enum
{
  /* 00 */
  SLT_OPEN_BRACES = 0, //"{"
  /* 01 */
  SLT_CLOSE_BRACES, //"}"
  /* 02 */
  SLT_PROJECTCHECK, //"_ProjectCheck"
  /* 03 */
  SLT_COMPONENTCHECK, //"_ComponentCheck"
  /* 04 */
  SLT_POGENERATE, //"_POGenerate"
  /* 05 */
  SLT_PROVE, //"_Prove"
  /* 06 */
  SLT_CTRANS, //"_CTrans"
  /* 07 */
  SLT_CPPTRANS, //"_CPPTrans"
  /* 08 */
  SLT_ADATRANS, //"_ADATrans"
  /* 09 */
  SLT_PSCTRANSCVG, //"_PscTransCvg"
  /* 10 */
  SLT_PSCTRANSDVG, //"_PscTransDvg"
  /* 11 */
  SLT_NEWPSCTRANSCONV, //"_NewPscTransConv"
  /* 12 */
  SLT_NEWPSCTRANS, //"_NewPscTrans"
  /* 13 */
  SLT_LATEX, //"_Latex"
  /* 14 */
  SLT_INTERLEAF, //"_Interleaf"
  /* 15 */
  SLT_OK, //"_ok"
  /* 16 */
  SLT_KO, //"_ko"
  /* 17 */
  SLT_NUMBER, //checksum d'un composant : nombre hexadecimal précédé de "@"
  /* 18 */
  SLT_IDENT, //identificateur
  //(les idents B ne commencent jamais par un "_" ou un "@")
  /* 19 */
  SLT_EOF, //End-Of-File
  /* 20 */
  SLT_HIATRANS, //"_HIATrans"
  /* 21 */
  SLT_ALATRANS, //"_ALATrans"
  /* 22 */
  SLT_ALBTRANS, //"_ALBTrans"
  /* 22 */
  SLT_BPP //"_bpp"
} T_state_lex_type ;

#endif /*  _M_LEXTYP_H_ */

#endif /* ACTION_NG */
