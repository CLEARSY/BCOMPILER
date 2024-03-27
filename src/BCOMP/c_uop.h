/******************************* CLEARSY **************************************
* Fichier : $Id: c_uop.h,v 2.0 2002-09-24 09:38:14 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des operations utilisees
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
#ifndef _C_UOP_H_
#define _C_UOP_H_

#include "c_item.h"
#include "c_genop.h"

class T_op ;

class T_used_op : public T_generic_op
{
  // Nom de l'operation (== name si pas de renomage)
  T_symbol			*real_op_name ;

	// Nom de l'instance (dans le cas d'une utilisation avec renommage)
  T_symbol			*instance_name ;

  // Nom pointe complet
  T_ident			*name ;

  // Pointeur sur la definition
  // Reference consultative
  T_generic_op		*ref_op ;

  // Pour savoir si l'operation a deja ete type-checkee
  int 				type_checked ;

	// Fonction d'analyse syntaxique
  void syntax_analysis(T_lexem **adr_ref_lexem) ;

public :
  T_used_op() : T_generic_op(NODE_USED_OP) {}  ;
  // Constructeur avec analyse syntaxique
  T_used_op(T_item **adr_first,
			T_item **adr_last,
			T_item *father,
			T_betree *betree,
			T_lexem **adr_cur_lexem) ;
  // Constructeur simple
  T_used_op(T_symbol *new_real_op_name,
			T_symbol *new_instance_name,
			T_ident *new_name,
			T_generic_op *new_ref_op,
			T_item **adr_first,
			T_item **adr_last,
			T_item *new_father,
			T_betree *new_betree,
			T_lexem *new_ref_lexem) ;
  virtual ~T_used_op(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_used_op" ; }  ;
  T_symbol *get_real_op_name(void)	{ return real_op_name ; } ;
  T_symbol *get_instance_name(void)	{ return instance_name ; } ;
  virtual T_ident *get_name(void) ;
  T_generic_op *get_ref_op(void)		{ return ref_op ; }
  virtual T_symbol *get_op_name(void)	;
  virtual T_ident *get_in_params(void) ;
  virtual T_ident *get_out_params(void)	;

  virtual int get_is_inlined(void) const {return ref_op->get_is_inlined();};
	// Methodes d'ecriture
  void set_ref_op(T_op *new_ref_op) 	{ ref_op = new_ref_op ; } ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de vérification B0
  virtual void B0_check(void);

  // Fonctionn de vérification B0_OM
  virtual void B0_OM_check_header_decl(T_B0_OM_status expected_stat);
  virtual void B0_OM_check_body_decl();

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Cherche le T_op associe en parcourant les ref_op eventuels
  // Pour T_used_op : renvoie ref_op->get_definition()
  virtual T_op *get_definition(void) ;

  // Pour une operation clonee depuis une autre operation, mise a jour
  // recursive des liens pour que les father, ... pointent sur
  // la machine new_machine
  virtual void fix_links(T_machine *new_machine) ;

  //GP 31/12/98
  //Verifie que la signature de la T_used_op
  //est compatible avec la signature de la spec_op:
  //meme parametres, memes types
  virtual void check_op_signature(T_generic_op* spec_op) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_UOP_H_ */

