/******************************* CLEARSY **************************************
* Fichier : $Id: i_toplvl.h,v 2.0 2002-07-12 09:59:15 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de haut niveau du B0 Checker
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
#ifndef _I_TOPLVL_H_
#define _I_TOPLVL_H_

// Fabrication d'un type B0 a partir d'un type B
T_B0_type *new_B0_type(T_type *ref_type,
								T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_machine *new_machine) ;

//
// référencement du type des entiers littéraux pour le BOM. Il est
// positionné lors de la passe sémantique est reste à null si aucun
// type n'a été déclaré par le pragma literal_innteger_type
extern T_ident *literal_integer_type;

// Booleen positionné à vrai une fois le type check de la variable
// literal_integer_type fait: cela évite de signaler l'erreur pour
// tous les entiers littéraux
extern int literal_integer_type_checked;


#endif /* _I_TOPLVL_H_ */
