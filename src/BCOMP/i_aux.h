/******************************* CLEARSY **************************************
* Fichier : $Id: i_aux.h,v 2.0 2002-09-24 09:39:55 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		B0 Checker, Fonctions auxiliaires
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
#ifndef _I_AUX_H_
#define _I_AUX_H_

// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est une expression B0
// de maplet
// Elle renvoie TRUE dans ce cas.
extern int
	syntax_check_list_is_a_maplet(T_item *list_items,
								  T_list_ident **list_ident,
								  T_B0_context context = B0CTX_ARRAY_CONTEXT);

// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est un terme
// Elle renvoie TRUE dans ce cas.
extern int syntax_check_list_is_a_term(T_item *list_items,
												T_list_ident **list_ident);

// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est un terme simple
// (verification syntaxique)
// Elle renvoie TRUE dans ce cas.
//
// Le parametre context est utilise pour verifier (context = 1) dans ce cas,
// si chaque element de la liste est un scalaire (verification supplementaire
// venant s'ajouter a la verification d'un terme simple)
extern int
	syntax_check_list_is_a_simple_term(T_item *list_items,
									   T_B0_context context = B0CTX_DEFAULT);


// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est une expression B0
// de parametre effectif d'entree d'operation (terme, tableau,
// chaine literale)
// Elle renvoie TRUE dans ce cas.

extern int
	syntax_check_list_is_an_op_in_param(T_item *list_items,
										T_list_ident **list_ident);

// Fonction prenant en argument une liste de T_record_item
// Elle parcourt la liste, et vérifie que chaque champ est un terme ou une
// expression de tableau.
// Elle renvoie TRUE dans ce cas.
extern int
	syntax_check_list_is_a_record_item(T_record_item *list_items,
									   T_list_ident **list_ident);


/////////////////////////////////////////////////////////////////////////////
//
// Fonctions auxiliaires pour la verification des types des donnees concretes
//
/////////////////////////////////////////////////////////////////////////////

// Fonction prenant en argument une liste de T_label_type
// Elle parcourt la liste, et vérifie que chaque element est un type conforme
// au B0
// Elle renvoie TRUE dans ce cas.
extern int type_check_list_is_a_label_type(T_label_type *list_labels);

// Fonction prenant en argument une liste de T_ident
// Elle parcourt la liste, et vérifie si chaque type B est valide pour
// un type de variable locale
extern void local_variable_type_check_list(T_ident *list_ident);

// Fonction prenant en argument une liste de T_record_item
// Elle parcourt la liste, et vérifie que chaque champ est une expression de
// typage par appartenance d'une constante concrete
// Elle renvoie TRUE dans ce cas.
extern int
	syntax_check_list_is_a_record_item_type(T_record_item *list_items);


// Fonction prenant en argument une liste d'instances de T_used_machine
// Elle parcourt la liste, et effectue pour chaque element les verifications
// sur les instanciations B0
extern void instanciation_check_list(T_used_machine *used_machine);

// Fonction prenant en argument une liste d'instances de T_item
// Elle parcourt la liste, et verifie pour chaque element si ce dernier est
// valide pour une expression d'instanciation B0
extern void instanciation_check_list(T_item *item);


// Prend en parametre une liste d'item sur laquelle on effectue le controle
// RINIT 1-1 (pas d'identificateur non initialise en partie droite d'une
// affectation)
extern void initialisation_check_list(T_item *item,
											   T_list_ident **list_ident);

// Prend en parametre une liste d'item sur laquelle on effectue le controle
// RINIT 1-1 (pas d'identificateur non initialise en partie droite d'une
// affectation)
// Mais dans ce cas, la T_list_ident retournee est l'intersection des
// differentes T_list_ident
extern void initialisation_get_intersection(T_item *item,
													 T_list_ident **inter);

#endif /* _I_AUX_H_ */

