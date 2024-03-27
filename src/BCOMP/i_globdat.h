/******************************* CLEARSY **************************************
* Fichier : $Id: i_globdat.h,v 2.0 1999-07-29 13:02:59 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Interface des donnees globales necessaires a la
*                   verification de B0
*
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
#ifdef B0C

#ifndef _I_GLOBDAT_H_
#define _I_GLOBDAT_H_

class T_list_ident;

typedef enum
{
  /* 00 */
  B0CHK_DEFAULT = 0,
  /* 01 */
  B0CHK_INIT,
  /* 02 */
  B0CHK_VAL
} T_B0_check ;

//
// Variable globale : flag servant a determiner si :
//        - on effectue les verification RINIT 1-1 (check = 1)
//        - on effectue les verification RVAL 0-1 et 0-2 (check = 2)
//        - on effectue aucune verification (check = 0)
//
//extern int check;
extern T_B0_check get_check(void);
extern void set_check(T_B0_check value);
//
// T_list_ident globale servant a memoriser les identificateurs a valuer
//                                                           ou a initialiser
//
//extern T_list_ident *current_list_ident;
extern T_list_ident * get_current_list_ident(void);
extern void set_current_list_ident(T_list_ident * value);

#endif /* _I_GLOBDAT_H_ */

#endif // B0C
