/******************************* CLEARSY **************************************
* Fichier : $Id: c_record.h,v 2.0 2000-11-06 16:46:59 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interfaces des records
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
#ifndef _C_RECORD_H_
#define _C_RECORD_H_



#include "c_expr.h"

#ifdef B0C
#include "i_ctx.h"

class T_list_ident;
#endif //B0C

// Forward : element de record
class T_record_item ;

// Modelisation d'un record
class T_record : public T_expr
{
  // Liste des couples (label, valeur)
  T_record_item 	*first_record_item ;
  T_record_item		*last_record_item ;

  //
  // Methodes privees
  //
  // Verifie que les labels sont deux a deux distincts
  // Emet un message d'erreur le cas echeant
  //  void check_no_double_definition(void) ;

public :
  T_record() : T_expr(NODE_RECORD) {} ;
  T_record(T_item *args,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem *new_ref_lexem) ;
  virtual ~T_record(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_record" ; }  ;
  T_record_item *get_record_items(void) { return first_record_item ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction : si relation = NULL alors il faut
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

#ifdef B0C
  // Fonction virtuelle : Est-ce un record en extension ?
  virtual int syntax_check_is_an_extension_record(T_list_ident **list_ident);
#endif // B0C

  // Ajout d'un record item en fin de liste
  void add_record_item(T_record_item *item);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Modelisation d'un struct
class T_struct : public T_expr
{
  // Liste des couples (label, valeur)
  T_record_item 	*first_record_item ;
  T_record_item		*last_record_item ;

public :
  T_struct() : T_expr(NODE_STRUCT) {} ;
  T_struct(T_item *args,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem *new_ref_lexem) ;
  virtual ~T_struct(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_struct" ; }  ;
  T_record_item *get_record_items(void) { return first_record_item ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

#ifdef B0C

  // Est-ce un ensemble de record (struct conforme au B0)
  virtual int syntax_check_is_a_record_set(void);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Modelisation d'un element de record
class T_record_item : public T_expr
{
  // Label
  T_ident			*label ;

  // Valeur
  T_expr			*value ;

public :
  T_record_item() : T_expr(NODE_RECORD_ITEM) {} ;
  T_record_item(T_item *new_label,
				T_item *new_value,
				T_item **adr_first,
				T_item **adr_last,
				T_item *new_father,
				T_betree *new_betree,
				T_lexem *new_ref_lexem) ;
  virtual ~T_record_item(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_record_item" ; }  ;
  T_ident *get_label(void) { return label ; } ;
  T_expr *get_value(void) { return value ; } ;

  T_access_authorisation get_auth_request(T_item *ref) ;


  // Methodes d'ecriture
  void set_label(T_ident *new_label) ;

  // Fonction auxiliaire qui extrait une liste de record_item,
  // separes par separator
  virtual void extract_record_items(T_item *new_father,
											 int allow_empty_label,
											 T_item **adr_first,
											 T_item **adr_last) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction : si relation = NULL alors il faut
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Modelisation d'un element de record
class T_record_access : public T_expr
{
  // Record accede
  T_expr			*record ;

  // Label
  T_ident			*label ;

public :
  T_record_access() : T_expr(NODE_RECORD_ACCESS) {} ;
  T_record_access(T_item *new_label,
				T_item *new_value,
				T_item **adr_first,
				T_item **adr_last,
				T_item *new_father,
				T_betree *new_betree,
				T_lexem *new_ref_lexem) ;
  virtual ~T_record_access(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_record_access" ; } ;
  T_expr *get_record(void) { return record ; } ;
  T_ident *get_label(void) { return label ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

#ifdef B0C

  // Suivant la valeur du parametre d'entree (TRUE ou FALSE),
  // On teste si le premier record d'un acces (eventuellement recursif) a un
  // record est un T_ident (parametre vaut TRUE) ou un record en extension.
  // (parametre vaut FALSE) (fonction recursive)
  // Exemple : si parametre vaut TRUE : Ident_ren('ident) est valide
  // (la fonction rend TRUE)
  //   Mais    Terme_record('Ident) n'est pas valide (la fonction rend FALSE)
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  int check_first_record(int first_record,
								  T_list_ident **list_ident,
								  T_B0_context context = B0CTX_DEFAULT);

  // Est-ce un terme simple
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int syntax_check_is_a_simple_term(T_list_ident **list_ident,
													 T_B0_context context = B0CTX_DEFAULT);

  // Est-ce un acces a un record en extension
  virtual int syntax_check_is_an_extension_record_access(T_list_ident **list_ident);

  // Est-ce une expression arithmetique
  // AB 13/01/99 suite a modif Spec B0C (FB)
  virtual int
    syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);

  // Fonction permettant de remonter au record dans un acces a ce
  // dernier ; l'acces pouvant etre recursif
  T_expr * get_first_record(void);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_RECORD_H_ */
