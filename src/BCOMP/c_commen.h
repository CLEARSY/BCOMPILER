/******************************* CLEARSY **************************************
* Fichier : $Id: c_commen.h,v 2.0 2000-09-29 07:39:03 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des commentaires
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
#ifndef _C_COMMENT_H_
#define _C_COMMENT_H_

#include "c_item.h"

class T_comment : public T_item
{
  // Texte du commentaire
  T_symbol			*comment ;

  // Commentaire a conserver (du type /*? ?*/ ssi TRUE)
  int			   	keep_comment ;

  // Machine en cours d'analyse
  T_machine			*ref_machine ;

public :
  T_comment() : T_item(NODE_COMMENT) {} ;
  T_comment(const char *new_comment,
			int new_keep_comment,
			int analyse_pragmas, // TRUE si on veut analyser les pragmas
			T_machine *new_ref_machine,
			T_item **adr_first,
			T_item **adr_last,
			T_item *new_father,
			T_betree *new_betree,
			T_lexem *new_ref_lexem) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) const { return "T_comment" ; }  ;
  T_symbol *get_comment(void) const { return comment ; } ;
  int get_keep_comment(void) const	{ return keep_comment ; } ;
  T_machine *get_ref_machine(void) const { return ref_machine ; } ;

  // Fonction auxiliaire qui regarde si le commentaire comporte un pragma
  void check_pragma(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;


// Obtention des chaines d'ouverture et de fermeture de commentaire
// i.e. '/*' et '*/' pour un commentaire non traduisible
// et   '/*?' et '?*/' pour un commentaire non traduisible
extern const T_symbol *get_open_comment(T_lex_type lex_type) ;
extern const T_symbol *get_close_comment(T_lex_type lex_type) ;

#ifdef __BCOMP__
// Changement du mode accrochage automatique des commentaires aux item (TRUE/FALSE)
extern void set_solve_comment(int new_solve_comment) ;

// Obtention de ce mode
extern int get_solve_comment(void) ;
#endif /* __BCOMP__ */

#endif /* _C_COMMENT_H_ */

