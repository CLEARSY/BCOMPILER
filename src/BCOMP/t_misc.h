/******************************* CLEARSY **************************************
* Fichier : $Id: t_misc.h,v 2.0 2000-06-21 09:26:26 fl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Fonctions utiles pour le typage
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
#ifndef _T_MISC_H_
#define _T_MISC_H_

// Vérifie que tous les identificateurs de list_idents ont été typés.
extern void check_type_ident_list(T_ident* list_idents) ;


// Vérifie que tous les identificateurs de list_idents ont été typés.
// Affiche un message d'erreur avec localisation du lieu ou la
// variable aurait du etre typée.
extern void check_type_ident_list(T_ident* list_idents,
										   const char *loc1,
										   const char *loc2) ;

#endif // _T_MISC_H_

//
//	}{	Fin du fichier
//

