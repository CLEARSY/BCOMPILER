/******************************* CLEARSY **************************************
* Fichier : $Id: c_umach.h,v 2.0 1999-06-09 12:38:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des machines utilisees
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
#ifndef _C_UMACH_H_
#define _C_UMACH_H_

#include "c_item.h"

class T_machine ;
class T_list_link ;

class T_used_machine : public T_item
{
  // Nom de la machine (== name si pas de renomage)
  T_symbol			*component_name ;

  // Nom de l'instance (dans le cas d'une utilisation avec renommage)
  T_symbol			*instance_name ;

  // Nom pointe complet
  T_ident			*name ;

  // Chemin d'acces depuis la racine (i.e. le composant en cours d'analyse)
  T_symbol			*pathname ;

  // Pointeur sur la definition
  // Reference consultative
  T_machine			*ref_machine ;

  // Liste des parametres d'utilisation : liste d'expressions Ce sont
  // des expressions, mais on ne peut pas dire que ce sont des T_expr
  // car certains sont des T_list_link. Donc on specifie en T_item, et
  // l'utilisateur DOIT utiliser make_expr() pour recuperer
  // l'expression attendue
  T_item			   	*first_use_param ;
  T_item			   	*last_use_param ;

  // Type d'utilisation
  T_use_type			use_type ;

  // Fonction d'analyse syntaxique
  void syntax_analysis(int parse_params,
								T_use_type new_use_type,
								T_lexem **adr_ref_lexem) ;

public :
  T_used_machine() : T_item(NODE_USED_MACHINE) {}  ;
  T_used_machine(int parse_params,
				 T_use_type new_use_type,
				 T_item **adr_first,
				 T_item **adr_last,
				 T_item *father,
				 T_betree *betree,
				 T_lexem **adr_cur_lexem) ;
  virtual ~T_used_machine(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_used_machine" ; }  ;

  T_symbol *get_component_name(void)	{ return component_name ; } ;
  T_symbol *get_instance_name(void)	{ return instance_name ; } ;
  T_symbol *get_pathname(void) { return pathname ; } ;
  T_ident *get_name(void) 			{ return name ;	}
  T_machine *get_ref_machine(void)	{ return ref_machine ; }
  T_item	*get_use_params(void)		{ return first_use_param ; } ;
  T_use_type get_use_type(void)		{ return use_type ; } ;

  // Methodes d'ecriture
  void set_ref_machine(T_machine *new_ref_machine)
	{
	  TRACE2("T_used_machine(%x)::set_ref_machine(%x)", this, new_ref_machine) ;
	  ref_machine = new_ref_machine ;
	} ;

  void set_component_name(T_symbol *new_component_name)
	{ component_name = new_component_name ; } ;

  // Calcul du pathname
  void make_pathname(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

#ifdef B0C
  // Controles sur les instanciations de parametres formels de machines
  virtual void instanciation_check(void);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  // Fonction qui enleve les "liens sortant"
  // Utile pour que recursive_set_block_state ne rende pas persistant
  // des objets qu'on souhaite liberer avec s_memory_clean_up
  void make_standalone(void) ;
} ;

#endif /* _C_UMACH_H_ */

