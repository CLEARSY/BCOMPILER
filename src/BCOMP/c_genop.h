/******************************* CLEARSY **************************************
* Fichier : $Id: c_genop.h,v 2.0 2002-09-24 09:33:36 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de T_generic_op, superclasse commune de
*					T_op et de T_used_op
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
#ifndef _C_GEOP_H_
#define _C_GEOP_H_

#include "c_item.h"

// T_generic_op : superclasse commune a T_op et a T_used_op
// Pas de champ propre, mais factorisation des methodes
class T_generic_op : public T_item
{
public :
  T_generic_op() : T_item(NODE_GENERIC_OP) {}  ;
  T_generic_op(T_node_type new_node_type) : T_item(new_node_type) {}  ;
  T_generic_op(T_node_type new_node_type,
			   T_item **adr_first,
			   T_item **adr_last,
			   T_item *father,
			   T_betree *betree,
			   T_lexem *cur_lexem) ;

  virtual ~T_generic_op(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_generic_op" ; } ;
  virtual T_symbol *get_op_name(void) = 0  ;		// virtuelle pure
  virtual T_ident *get_name(void) = 0  ;		// virtuelle pure
  virtual T_ident *get_in_params(void) = 0 ;		// virtuelle pure
  virtual T_ident *get_out_params(void) = 0 ;	// virtuelle pure

  // Cherche le T_op associe en parcourant les ref_op eventuels
  virtual T_op *get_definition(void) = 0 ;

  // Pour savoir si un objet est une operation
  virtual int is_an_operation(void) ;

  // Pour une operation clonee depuis une autre operation, mise a jour
  // recursive des liens pour que les father, ... pointent sur
  // la machine new_machine
  virtual void fix_links(T_machine *new_machine) = 0 ;

  // evalue si l'op�ration est concrete
  virtual T_B0_OM_status get_status(void);

  // test si l'op�ration est inlin�e
  virtual int get_is_inlined(void) const = 0;

  // Fonction de v�rification B0
  virtual void B0_check(void) = 0;

  // Fonctionn de v�rification B0_OM
  virtual void B0_OM_check_header_decl(T_B0_OM_status expec_sta) = 0;
  virtual void B0_OM_check_body_decl(void) = 0;
  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_GEOP_H_ */

