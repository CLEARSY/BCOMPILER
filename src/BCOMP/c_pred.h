/******************************* CLEARSY **************************************
* Fichier : $Id: c_pred.h,v 2.0 2004-01-08 09:33:01 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des predicats
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
#ifndef _C_PREDIC_H_
#define _C_PREDIC_H_


#include "c_item.h"
#include "c_pretyp.h"
#include "c_expr.h"

#ifdef B0C
#include "i_ctx.h"

class T_list_ident;
#endif //B0C

class T_predicate : public T_item
{
protected :
  T_predicate(T_node_type new_node_type) : T_item(new_node_type) {} ;
public :
  T_predicate() : T_item(NODE_PREDICATE) {} ;
  T_predicate(T_node_type new_node_type,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem) : T_item(new_node_type,
											   adr_first,
											   adr_last,
											   new_father,
											   new_betree,
											   new_ref_lexem) {} ;
  virtual ~T_predicate(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_predicate" ; }  ;

  //	Fonction qui fabrique un item a partir d'une item parenthesee
  virtual T_item *new_paren_item(T_betree *betree,
										  T_lexem *opn,
										  T_lexem *clo) ;

  // Les predicats sont des predicats !!!
  virtual int is_a_predicate(void) ;

  // fonction de vérification B0 OM
  virtual void B0_OM_check(void);

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_predicate_with_parenthesis : public T_predicate
{
  T_predicate		*predicate ;

public :
  T_predicate_with_parenthesis():T_predicate(NODE_PREDICATE_WITH_PARENTHESIS) {} ;
  // Constructeur classique
  T_predicate_with_parenthesis(T_predicate *new_predicate,
							   T_item **adr_first,
							   T_item **adr_last,
							   T_item *new_father,
							   T_betree *new_betree,
							   T_lexem *new_ref_lexem) ;

  // Constructeur a partir d'un T_expr_with_parenthesis deja fait
  // (et contenant un predicat ...)
  T_predicate_with_parenthesis(T_expr_with_parenthesis *ref_parexpr,
							   T_item **adr_first,
							   T_item **adr_last,
							   T_item *new_father,
							   T_betree *new_betree,
							   T_lexem *new_ref_lexem) ;
  // Destructeur
  virtual ~T_predicate_with_parenthesis(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_predicate_with_parenthesis" ; }  ;
  T_predicate *get_predicate(void) { return predicate ; } ;

  virtual int is_a_machine_param_typing_predicate(void)
	{ return predicate->is_a_machine_param_typing_predicate() ; } ;
  virtual int is_a_constant_typing_predicate(void)
	{ return predicate->is_a_constant_typing_predicate() ; } ;
  virtual int is_a_variable_typing_predicate(void)
	{ return predicate->is_a_variable_typing_predicate() ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Est-ce une formule typante ??
  // Par defaut : non !
  // Vrai que pour une conjonction de ':', '<:' ou '='
  virtual int is_a_typing_formula(void) ;

#ifdef B0C
  // On teste si le predicat est un predicat B0 (une condition)
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident);

  // fonction de vérification B0 OM
  virtual void B0_OM_check(void);

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#endif //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_not_predicate : public T_predicate
{
  T_predicate		*predicate ;

public :
  T_not_predicate() : T_predicate(NODE_NOT_PREDICATE) {} ;
  T_not_predicate(T_predicate *new_predicate,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *new_father,
				  T_betree *new_betree,
				  T_lexem *new_ref_lexem) ;
  virtual ~T_not_predicate(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_not_predicate" ; }  ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // fonction de vérification B0 OM
  virtual void B0_OM_check(void);

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);


#ifdef B0C

  // On teste si le predicat est un predicat B0 (une condition)
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident);
#endif //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_binary_predicate : public T_predicate
{
  // Predicats
  T_predicate				*pred1 ;
  T_predicate	   			*pred2 ;

  // Operateur
  T_binary_predicate_type	predicate_type ;

public :
  T_binary_predicate() : T_predicate(NODE_BINARY_PREDICATE) {} ;
  T_binary_predicate(T_item *new_pred1,
					 T_item *new_pred2,
					 T_binary_predicate_type new_predicate_type,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;
  virtual ~T_binary_predicate(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_binary_predicate" ; }  ;
  T_predicate *get_pred1(void)		{ return pred1 ; } ;
  T_predicate *get_pred2(void)		{ return pred2 ; } ;
  T_binary_predicate_type get_predicate_type(void)
	{ return predicate_type ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // "Indirections"
  virtual int is_a_machine_param_typing_predicate(void)
	{ return pred1->is_a_machine_param_typing_predicate() ; } ;
  virtual int is_a_constant_typing_predicate(void)
	{ return pred1->is_a_constant_typing_predicate() ; } ;
  virtual int is_a_variable_typing_predicate(void)
	{ return pred1->is_a_variable_typing_predicate() ; } ;

  //
  //	}{	Fonction auxiliaire qui extrait une liste de parametres
  //
  virtual void extract_params(int separator,
									   T_item *new_father,
									   T_item **adr_first,
									   T_item **adr_last) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // fonction de vérification B0 OM
  virtual void B0_OM_check(void);

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

  // Est-ce une formule typante ??
  // Par defaut : non !
  // Vrai que pour une conjonction de ':', '<:' ou '='
  virtual int is_a_typing_formula(void) ;

#ifdef B0C
  // On teste si le predicat est un predicat B0 (une condition)
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident);
#endif //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_expr_predicate : public T_predicate
{
  // Expressions
  T_expr					*expr1 ;
  T_expr					*expr2 ;

  // Operateur
  T_expr_predicate_type	predicate_type ;

  //
  // Methodes privees de type check
  //
  void type_check_order(void) ;
  void type_check_not_included(void) ;
  void type_check_different(void) ;
  void type_check_not_belongs(void) ;

public :
  T_expr_predicate() : T_predicate(NODE_PREDICATE) {} ;

  // Constructeur classique
  T_expr_predicate(T_item *new_expr1,
				   T_item *new_expr2,
				   T_expr_predicate_type new_predicate_type,
				   T_item **adr_first,
				   T_item **adr_last,
				   T_item *new_father,
				   T_betree *new_betree,
				   T_lexem *new_ref_lexem) ;

  virtual ~T_expr_predicate(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_expr_predicate" ; }  ;
  T_expr *get_expr1(void)			{ return expr1 ; } ;
  T_expr *get_expr2(void)			{ return expr2 ; } ;
  T_expr_predicate_type get_predicate_type(void)
	{ return predicate_type ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  virtual void B0_OM_check();

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#ifdef B0C

  // On teste si le predicat est un predicat B0 (une condition)
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident);



#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_universal_predicate : public T_predicate
{
  // Variables
  T_ident					*first_variable ;
  T_ident					*last_variable ;

  // Predicats
  T_predicate				*predicate ;

public :
  T_universal_predicate() : T_predicate(NODE_UNIVERSAL_PREDICATE) {} ;
  T_universal_predicate(T_item *new_variable,
						T_item *new_expr,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem *new_ref_lexem) ;
  virtual ~T_universal_predicate(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_universal_predicate" ; }  ;
  T_ident *get_variables(void)			{ return first_variable ; } ;
  T_predicate *get_predicate(void)		{ return predicate ; } ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_existential_predicate : public T_predicate
{
  // Variables
  T_ident					*first_variable ;
  T_ident					*last_variable ;

  // Predicats
  T_predicate				*predicate ;

public :
  T_existential_predicate() : T_predicate(NODE_EXISTENTIAL_PREDICATE) {} ;
  T_existential_predicate(T_item *new_variable,
						  T_item *new_expr,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem *new_ref_lexem) ;
  virtual ~T_existential_predicate(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_existential_predicate" ; }  ;
  T_ident *get_variables(void)			{ return first_variable ; } ;
  T_predicate *get_predicate(void)		{ return predicate ; } ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_typing_predicate : public T_predicate
{
  friend class T_becomes_such_that ;

  // Identifiants a typer
  T_expr					*first_ident ;
  T_expr					*last_ident ;

  // Type
  T_item					*type ;

  // Separateur d'ident
  T_binary_operator separator;

  // Operateur
  T_typing_predicate_type	typing_predicate_type ;

  // Verification d'integrite syntaxique
  void check_params(void) ;

public :
  T_typing_predicate() : T_predicate(NODE_TYPING_PREDICATE) {} ;

  // Constructeur classique
  T_typing_predicate(T_item *new_pred1,
					 T_item *new_pred2,
					 T_typing_predicate_type new_type,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;

  // Destructeur
  virtual ~T_typing_predicate(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // fonction de vérification B0 OM
  virtual void B0_OM_check(void);

  // test de référencement: retourne la première reference
  // à la variable ident, NULL s'il elle n'est pas
  // référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Ajout d'un parametre en tete de liste
  void insert_param(T_ident *new_param) ;

  // Pour savoir si un predicat est un predicat de typage de
  // parametre de machine - A FAIRE :: affiner !
  virtual int is_a_machine_param_typing_predicate(void)
	{ return TRUE ; } ;

  // Pour savoir si un predicat est un predicat de typage de
  // constante - A FAIRE :: affiner !
  virtual int is_a_constant_typing_predicate(void)
	{ return TRUE ; } ;

  // Pour savoir si un predicat est un predicat de typage de
  // variable - A FAIRE :: affiner !
  virtual int is_a_variable_typing_predicate(void)
	{ return TRUE ; } ;

  // Est-ce une formule typante ??
  // Par defaut : non !
  // Vrai que pour une conjonction de ':', '<:' ou '='
  virtual int is_a_typing_formula(void) ;

#ifdef B0C

  // On teste si le predicat est un predicat B0 (une condition)
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident);
#endif // B0C

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_typing_predicate" ; }  ;
  T_expr *get_identifiers(void) 		{ return first_ident ; } ;
  T_item *get_type(void)				{ return type ; } ;
  T_typing_predicate_type get_typing_predicate_type(void)
    { return typing_predicate_type ; } ;

  T_binary_operator get_separator() {return separator;}

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Fonction qui parse les predicates
extern T_predicate *syntax_get_predicate(T_item **adr_first,
												  T_item **adr_last,
												  T_item *father,
												  T_betree *betree,
												  T_lexem **adr_cur_lexem) ;

// Fonction qui parse les predicats ou les typages de parametres de machine
extern T_predicate *
syntax_get_machine_param_type_or_predicate(T_item **adr_first,
										   T_item **adr_last,
										   T_item *father,
										   T_betree *betree,
										   T_lexem **adr_cur_lexem) ;

// Fonction qui parse les predicats ou les typages de constantes
extern T_predicate *
syntax_get_constant_type_or_predicate(T_item **adr_first,
									  T_item **adr_last,
									  T_item *father,
									  T_betree *betree,
									  T_lexem **adr_cur_lexem) ;

// Fonction qui parse les predicats ou les typages de variables
extern T_predicate *
syntax_get_variable_type_or_predicate(T_item **adr_first,
									  T_item **adr_last,
									  T_item *father,
									  T_betree *betree,
									  T_lexem **adr_cur_lexem) ;

#endif /* _C_PREDIC_H_ */

