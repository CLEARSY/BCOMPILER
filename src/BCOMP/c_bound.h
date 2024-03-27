/******************************* CLEARSY **************************************
* Fichier : $Id: c_bound.h,v 2.0 1998-11-30 10:51:32 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Description des bornes
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
#ifndef _C_BOUND_H_
#define _C_BOUND_H_

// A FAIRE :: plus la faire heriter de T_expr
class T_bound : public T_expr
{
  // Identificateur associe
  T_ident		*ref_ident ;

  // Valeur associee, si disponible
  T_expr		*value ;

  // Est-ce la borne superieure ? TRUE si oui, FALSE sinon
  int			is_upper_bound ;

  public :
  T_bound(void) : T_expr(NODE_BOUND) {} ;
  T_bound(T_ident *new_ref_ident,
		  int new_is_upper_bound,
		  T_item **adr_first,
		  T_item **adr_last,
		  T_item *father,
		  T_betree *betree,
		  T_lexem *ref_lexem) ;

  virtual ~T_bound() ;

  virtual const char *get_class_name(void) { return "T_bound" ; } ;

  // Methodes de lecture
  T_ident *get_ref_ident(void) { return ref_ident ; } ;
  T_expr *get_value(void) { return value ; } ;
  int is_upper(void) { return is_upper_bound ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Methodes d'ecriture
  void set_value(T_expr *new_value) { value = new_value ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_BOUND_H_ */
