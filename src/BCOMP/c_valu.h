/******************************* CLEARSY **************************************
* Fichier : $Id: c_valu.h,v 2.0 1999-06-23 13:15:32 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_valuation
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
#ifndef _C_VALU_H_
#define _C_VALU_H_

#include "c_item.h"

#ifdef B0C
class T_list_ident;
#endif // B0C

class T_valuation : public T_item
{
  // Identificateur
  T_ident			*ident ;

  // Valeur
  T_expr			*value ;

  // Fonction d'analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_valuation() : T_item(NODE_VALUATION) {} ;

  // Constructeur classique avec parsing
  T_valuation(T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem **adr_ref_lexem) ;

  // Constructeur a partir d'un T_typing_predicate avec
  // typing_predicate_type = EQUAL
  T_valuation(T_typing_predicate *ref_typing_predicate,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree) ;

  // Destructeur
  virtual ~T_valuation(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Demande de l'acces necessaire pour le fils ref
  virtual T_access_authorisation get_auth_request(T_item *ref) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_valuation" ; }  ;
  T_ident *get_ident(void)			{ return ident ; } ;
  T_expr *get_value(void)			{ return value ; } ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

#ifdef B0C
  // Controles sur les valuations des constantes concretes et des ensembles
  // abstraits.
  virtual void valuation_check(T_list_ident **list_ident);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Bascule de fonctionnement pour T_valuation::type_check
// Si new_mode == TRUE : le TC de la clause values ne traite que les SETS
// Si new_mode == FALSE : le TC de la clause values ne traite que les CST
extern void set_values_mode_sets(int new_mode) ;
// Lecture de la bascule
extern int get_values_mode_sets(void) ;

//
//	}{	Fin du fichier
//

#endif /* _C_VALU_H_ */
