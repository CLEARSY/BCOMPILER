/******************************* CLEARSY **************************************
* Fichier : $Id: c_pragma.h,v 2.0 1998-12-02 13:33:25 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_pragma qui modelise un pragma
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
**************************************************************************/
#ifndef _C_PRAGMA_H_
#define _C_PRAGMA_H_

class T_pragma_lexem ;

class T_pragma : public T_item
{
  // Nom du pragma
  T_symbol   	     	*name ;

  // Parametres
  // Was : T_ident
  T_expr				*first_param ;
  T_expr				*last_param ;

  // Le pragma a-t-il ete traduit, i.e pris en compte par le traducteur ?
  // Par defaut : FALSE
  int					translated ;

  // Commentaire de provenance
  T_comment 			*ref_comment ;

  // Machine d'appartenance
  T_machine				*ref_machine ;

  // Pragma "local" suivant/precedent (les champs herites de T_item servent pour
  // les listes globales)
  T_pragma				*next_pragma ;
  T_pragma				*prev_pragma ;

public :
  // Constructeur, destructeur
  T_pragma(void) : T_item(NODE_PRAGMA) {} ;
  T_pragma(const char *new_name,
		   T_comment *new_ref_comment,
		   T_machine *new_ref_machine,
		   T_pragma_lexem *new_pragma_lexem,
		   T_pragma **adr_first_local_pragma,
		   T_pragma **adr_last_local_pragma,
		   T_item **adr_first_global_pragma,
		   T_item **adr_last_global_pragma,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem *new_ref_lexem) ;

  virtual ~T_pragma(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_pragma" ; } ;

  T_symbol *get_name(void) { return name ; } ;
  T_expr *get_params(void) { return first_param ; } ;
  T_comment *get_ref_comment(void) { return ref_comment ; } ;
  T_machine *get_ref_machine(void) { return ref_machine ; } ;
  // Pragma NON TRADUIT suivant/precedent
  T_pragma *get_next_pragma(void) ;
  T_pragma *get_prev_pragma(void) ;
  // Pragma suivant/precedent
  T_pragma *get_real_next_pragma(void) { return next_pragma ; } ;
  T_pragma *get_real_prev_pragma(void) { return prev_pragma ; } ;
  int get_translated(void) { return translated ; } ;

  // Le pragma a ete traduit
  void set_translated(void) { translated = TRUE ; } ;

  // Verifier qu'un pragma n'a pas de parametre
  void check_no_param(void) ;

  // Verifier qu'il n'y a pas d'autre pragma local
  // Renvoie TRUE/FALSE
  int check_no_other_local_pragma(void) ;

  // Verifier qu'un pragma n'a qu'un seul parametre, et de type string
  // Retourne le symbole correspondant
  T_symbol *check_single_string_param(void) ;

  // Verifier qu'un pragma n'a qu'un seul parametre, et de type string
  // Retourne les symboles correspondant
  void check_two_string_params(T_symbol **adr_p1, T_symbol **adr_p2) ;


  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;


#endif /* _C_PRAGMA_H_ */



