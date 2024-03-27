/******************************* CLEARSY **************************************
* Fichier : $Id: c_defini.h,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des definitions
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
#ifndef _C_DEFINI_H_
#define _C_DEFINI_H_

#include "c_item.h"

//
// Definition literale
//
class T_definition : public T_item
{
  friend void expand_DEFINITIONS_in_definitions(T_definition *first_def,
														 T_definition *last_def) ;

  // Nom de la definition
  T_symbol	*name ;

  // Arguments de la definition
  int			nb_params ;
  T_ident		*first_param ;
  T_ident		*last_param ;

  // Regle de reecriture
  T_lexem		*first_rewrite_rule ;
  T_lexem		*last_rewrite_rule ;

  // Position du ';' de fin le cas echeant
  T_lexem		*ref_scol_lexem ;

  //
  // Methodes privees
  //
  // Fonction qui cherche un param de nom donne
  T_ident *find_param(const char *value,
							   size_t value_len) ;

  // Fonction qui renvoie le dernier lexem de la definition à partir du '=='
  T_lexem *find_end_of_def(T_lexem *first_lexem);

  // Fonction d'analyse syntaxique
  void syntax_analysis(T_lexem **adr_ref_lexem, T_lexem *last_code_lexem) ;

public :
  T_definition() : T_item(NODE_DEFINITION) {} ;
  T_definition(T_item **adr_first,
			   T_item **adr_last,
			   T_item *new_father,
			   T_betree *new_betree,
			   T_lexem **adr_ref_lexem,
			   T_lexem *last_code_lexem) ;
  virtual ~T_definition(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)	   { return "T_definition" ; }  ;
  T_symbol *get_name(void) 			   { return name ; } ;
  int get_nb_params(void) 			   { return nb_params ; } ;
  T_ident *get_params(void)			   { return first_param ; } ;
  T_lexem *get_rewrite_rule(void) 	   { return first_rewrite_rule ; } ;
  T_lexem *get_last_rewrite_rule(void) { return last_rewrite_rule ; } ;
  T_lexem **get_adr_first_rewrite_rule(void)
	{ return &first_rewrite_rule ; } ;
  T_lexem **get_adr_last_rewrite_rule(void)
	{ return &last_rewrite_rule ; } ;
  T_lexem *get_ref_scol_lexem(void) { return ref_scol_lexem ; } ;

  // Methodes d'ecriture
  void set_first_rewrite_rule(T_lexem *new_first_rewrite_rule)
	{ first_rewrite_rule = new_first_rewrite_rule ; } ;
  void set_last_rewrite_rule(T_lexem *new_last_rewrite_rule)
	{ last_rewrite_rule = new_last_rewrite_rule ; } ;
  void set_ref_scol_lexem(T_lexem *new_ref_scol_lexem)
	{ ref_scol_lexem = new_ref_scol_lexem ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

//
// Fichier de definitions
//
class T_file_definition : public T_item
{
  // Nom du fichier
  T_symbol	*name ;

  // Path du fichier
  T_symbol	*path ;

  // Checksum du fichier
  T_symbol 	*checksum ;

public :
  T_file_definition() : T_item(NODE_FILE_DEFINITION) {} ;
  T_file_definition(T_symbol *new_name,
					T_symbol *new_checksum,
					T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem *ref_lexem) ;
  virtual ~T_file_definition(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void){ return "T_file_definition" ; };
  T_symbol *get_name(void) { return name ; } ;
  T_symbol *get_path(void) { return path ; } ;
  T_symbol *get_checksum(void) { return checksum ; } ;

  // Methodes d'ecriture
  void set_checksum(T_symbol *new_checksum) { checksum = new_checksum ; } ;
  void set_path(T_symbol *new_path) { path = new_path ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

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

// Parsing de la clause DEFINITIONS
extern void parse_DEFINITIONS(T_lexem *first_lexem,
                                       const char* converterName,
									   T_definition **adr_first_def,
									   T_definition **adr_last_def,
									   T_flag **adr_def_clause) ;

// Obtention d'un ordre de dependance des definitions
// ATTENTION EFFET DE BORD :: Utilise les champs usr1 et usr2 et tmp
extern void order_DEFINITIONS(T_definition **adr_first_definition,
				T_definition **adr_last_definitionvoid) ;

// Expansion des definitions dans la clause DEFINITIONS
extern void expand_DEFINITIONS_in_definitions(void) ;

// Expansion des definitions dans le reste du code source
extern void expand_DEFINITIONS_in_source(void) ;

#ifdef __BCOMP__
// Mise a jour du pointeur su la clause DEFINITIONS
extern void set_definitions_clause(T_lexem *ref_lexem) ;
// Lecture de ce pointeur
extern T_lexem *get_definitions_clause(void) ;

// Mise a jour/lecture de l'expand_state
// Lecture : Renvoie TRUE ssi une expansion a eu lieu a cours de la derniere
// phase d'expansion de definitions
extern void set_expand_state(int new_expand_state) ;
extern int get_expand_state(void) ;
#endif

// First_def :
// Pile des definitions : reset/push/pop/vide ?
extern void reset_first_def_stack(void) ;
extern void delete_first_def_stack(void) ;
//extern void push_first_def_stack(T_definition *def) ;
#define push_first_def_stack(x) _push_first_def_stack(x, __FILE__, __LINE__) ;
extern void _push_first_def_stack(T_definition *def, const char *file, int line) ;
extern T_definition *pop_first_def_stack(void) ;
extern int is_first_def_stack_empty(void) ;

// Pile de travail
extern void reset_tmp_first_def_stack(void) ;
extern void push_tmp_first_def_stack(T_definition *def) ;
extern void copy_tmp_first_def_stack(void) ;

// Last_def
// Pile des definitions : reset/push/pop/vide ?
extern void reset_last_def_stack(void) ;
extern void delete_last_def_stack(void) ;
extern void push_last_def_stack(T_definition *def) ;
extern T_definition *pop_last_def_stack(void) ;
extern int is_last_def_stack_empty(void) ;

// Echange des piles first/last
extern void swap_def_stacks(void) ;

// Pile de travail
extern void reset_tmp_last_def_stack(void) ;
extern void push_tmp_last_def_stack(T_definition *def) ;
extern void copy_tmp_last_def_stack(void) ;

// First_def :
// Pile des definitions : reset/push/pop/vide ?
extern void reset_first_file_def_stack(void) ;
extern void delete_first_file_def_stack(void) ;
//extern void push_first_def_stack(T_definition *def) ;
#define push_first_file_def_stack(x) \
	_push_first_file_def_stack(x, __FILE__, __LINE__) ;
extern void _push_first_file_def_stack(T_file_definition *def,
												const char *file, int line) ;
extern T_file_definition *pop_first_file_def_stack(void) ;
extern int is_first_file_def_stack_empty(void) ;


// Last_def
// Pile des definitions : reset/push/pop/vide ?
extern void reset_last_file_def_stack(void) ;
extern void delete_last_file_def_stack(void) ;
extern void push_last_file_def_stack(T_file_definition *def) ;
extern T_file_definition *pop_last_file_def_stack(void) ;
extern int is_last_file_def_stack_empty(void) ;

// def_clause
// Pile des definitions : reset/push/pop/vide ?
extern void reset_def_clause_stack(void) ;
extern void delete_def_clause_stack(void) ;
extern void push_def_clause_stack(T_flag *def) ;
extern T_flag *pop_def_clause_stack(void) ;
extern int is_def_clause_stack_empty(void) ;


// Liberation des piles de l'analyseur de definitions
extern void lex_unlink_def_stacks(void) ;

#endif /* _C_DEFINI_H_ */


