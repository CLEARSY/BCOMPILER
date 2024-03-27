/******************************* CLEARSY **************************************
* Fichier : $Id: c_flag.h,v 2.0 1999-06-23 13:15:29 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_flag
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
#ifndef _C_FLAG_H_
#define _C_FLAG_H_

class T_flag : public T_item
{
public :
  T_flag() : T_item(NODE_FLAG) {} ;
  T_flag(T_item *new_father,
		 T_betree *new_betree,
		 T_lexem *new_ref_lexem) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_flag" ; }  ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_FLAG_H_ */


