/******************************* CLEARSY **************************************
* Fichier : $Id: i_type.h,v 2.0 2002-07-16 09:37:16 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		B0 checker
*			Conversion des types B en types B0
*                       Fonction de comparaison de types B0_OM
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
#ifndef _I_TYPE_H_
#define _I_TYPE_H_

//
// Fonction de comparaison de deux types nommés
//
int ident_type_semantic_equal(T_ident *type, T_ident *expected_type);

//
// Fonction qui donne le type "suivant" pour un type donné Si le type
// est renomme un autre type on renvoie le type renommé NULL sinon
//
T_ident *get_next_homonym_type(T_ident *type);

//
// Fonction pour comparer sémantiquement deux types
//
int expr_type_semantic_equal(T_expr *expr, T_expr *ref_expr);

#endif //_I_TYPE_H_
