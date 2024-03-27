/******************************* CLEARSY **************************************
* Fichier : $Id: c_lstlnk.h,v 2.0 2001-08-10 09:00:06 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interfaces des elements de chainage
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
#ifndef _C_LIST_LINK_H_
#define _C_LIST_LINK_H_

#ifdef B0C
#include "i_ctx.h"

class T_list_ident;
#endif //B0C

#include "c_item.h"

class T_list_link : public T_item
{
  // Element chaine
  T_object				*object ;

  // Faut-il liberer l'objet contenu ?
  int					do_free ;

  // Type de list_link
  T_list_link_type   	list_link_type ;

public :
  T_list_link() : T_item(NODE_LIST_LINK) {} ;
  // Constructeur avec chainage en queue
  T_list_link(T_object *new_object,
			  T_list_link_type new_list_link_type,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father = NULL,
			  T_betree *new_betree = NULL,
			  T_lexem *new_ref_lexem = NULL) ;

  // Constructeur avec chainage apres un element
  T_list_link(T_object *new_object,
			  T_list_link_type new_list_link_type,
			  T_item *after_this_item,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father = NULL,
			  T_betree *new_betree = NULL,
			  T_lexem *new_ref_lexem = NULL) ;

  // Constructeur de clonage
  T_list_link(T_list_link *ref_list_link,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem) ;

  // Destructeur
  virtual ~T_list_link(void) ;

  // Dire qu'il faut liberer l'objet contenu
  void set_do_free(void) { do_free = TRUE ; } ;
  // Dire qu'il ne faut pas liberer l'objet contenu
  void unset_do_free(void) { do_free = FALSE ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_list_link" ; }  ;
  T_object *get_object(void) { return object ; } ;
  T_list_link_type get_list_link_type(void) { return list_link_type ; } ;

  // Méthode de modification
  void set_object(T_object *new_object) { object = new_object; };

  // Methode qui suit une indirection (T_list_link) dans tous les cas
  virtual T_object *follow_indirection(void)
	{
	  return object ;
	}

  // Recherche recursive d'un identificateur
  // Attention, au niveau du client, l'appel de find_ident doit
  // obligatoirement etre precede d'un appel a next_timestamp
  // afin de reinitialiser le mecanisme de non-bouclage
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Obtention du type B
  // get_B_type est virtuelle car redefinie pour les identificateurs
  // (qui cherchent leur type chez leur def)
  virtual T_type *get_B_type(void) ;

  // Mise a jour du type
  virtual void set_B_type(T_type *new_B_type, T_lexem *typing_location) ;

  // Est-ce un ensemble ?
  virtual int is_a_set(void) ;

  // Est-ce un ident ?
  virtual int is_an_ident(void) ;

  // Est-ce une sequence ?
  virtual int is_a_seq(void) ;

  // Est-ce une sequence non vide ?
  virtual int is_a_non_empty_seq(void) ;

  // Est-ce un ensemble d'entiers ?
  virtual int is_an_integer_set(void) ;

  // Est-ce une relation ?
  virtual int is_a_relation(void) ;

  // Fonction generique de calcul de type
  virtual void make_type(void) ;

  // GP 7/01/99
  // Fonction generique qui verifie que l'objet est type
  virtual void check_type(void) ;

  // Verifications a posteriori pour l'expression contenue
  virtual void post_check(void) ;

  // Est-ce une constante/variable ?
  // Par defaut, NON
  // OUI ssi classe T_ident avec le bon ident_type
  // Si ask_def vaut TRUE on parcourt les champs def pour connaitre le type
  virtual int is_a_constant(int ask_def = TRUE) ;
  virtual int is_a_variable(int ask_def = TRUE) ;

  // Renvoie l'identificateur "contenu" dans this
  // i.e. this pour la classe T_ident et sous-classes
  //      this->object pour T_list_link qui contient un ident, ...
  // prerequis : is_an_ident == TRUE
  virtual T_ident *make_ident(void) ;

  // Renvoie l'expression "contenu" dans this
  // i.e. this pour la classe T_expr et sous-classes
  //      this->object pour T_list_link qui contient une expr, ...
  // prerequis : is_an_expr == TRUE
  virtual T_expr *make_expr(void) ;

  // Methode qui suit une indirection (T_list_link) ssi elle est
  // d'un type donne, et rend l'objet pointe, ou this sinon
  // Cas general : on rend this
  virtual T_object *follow_indirection(T_list_link_type ref_type) ;

  //
  //	}{	Fonction qui fabrique un item a partir d'une item parenthesee
  //
  virtual T_item *new_paren_item(T_betree *betree,
										  T_lexem *opn,
										  T_lexem *clo) ;

  // "Indirections"
  virtual int is_an_expr(void)
	{ return ((T_item *)object)->is_an_expr() ; } ;
  virtual int is_a_machine_param_typing_predicate(void)
	{ return ((T_item *)object)->is_a_machine_param_typing_predicate() ; } ;
  virtual int is_a_constant_typing_predicate(void)
	{ return ((T_item *)object)->is_a_constant_typing_predicate() ; } ;
  virtual int is_a_variable_typing_predicate(void)
	{ return ((T_item *)object)->is_a_variable_typing_predicate() ; } ;

#ifdef B0C

  // Fonction virtuelle : Est-ce une plage ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_a_range(T_list_ident **list_ident,
											   T_B0_context context = B0CTX_DEFAULT);

  // Fonction de vérification : est-ce un term ?
  virtual int syntax_check_is_a_term(T_list_ident **list_ident)
	{return ((T_item *)object)->syntax_check_is_a_term(list_ident);};

  // Fonction de vérification : est-ce un terme simple ?
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int syntax_check_is_a_simple_term(T_list_ident **list_ident,
													 T_B0_context context = B0CTX_DEFAULT);

  // Fonction de vérification : est-ce un record en extension ?
  virtual int syntax_check_is_an_extension_record(T_list_ident **list_ident)
	{return ((T_item *)object)->syntax_check_is_an_extension_record(list_ident);};

  // Fonction de vérification : est-ce un acces a un record en extension ?
  virtual int syntax_check_is_an_extension_record_access(T_list_ident **list_ident)
  	{return ((T_item *)object)->syntax_check_is_an_extension_record_access(list_ident);};

  // Fonction de vérification : est-ce un tableau ?
  virtual int syntax_check_is_an_array(T_list_ident **list_ident,
											   T_B0_context = B0CTX_ARRAY_CONTEXT);

  // Fonction de vérification : est-ce un acces a un tableau ?
  virtual int syntax_check_is_an_array_access(T_list_ident **list_ident)
  	{return ((T_item *)object)->syntax_check_is_an_array_access(list_ident);};

  // Fonction de vérification : est-ce un maplet ?
  virtual int syntax_check_is_a_maplet(T_list_ident **list_ident,
												T_B0_context context = B0CTX_ARRAY_CONTEXT);

  // Fonction de vérification : est-ce un singleton ?
  virtual int syntax_check_is_a_singleton(T_list_ident **list_ident,
												   T_B0_context context)
  	{return ((T_item *)object)->syntax_check_is_a_singleton(list_ident,
															context);};

  // Fonction de vérification : est-ce une expression arithmetique ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
														   T_B0_context context = B0CTX_DEFAULT);

  // Fonction de vérification : est-ce une expression booleenne ?
  virtual int syntax_check_is_a_bool_expr(T_list_ident **list_ident)
	{return ((T_item *)object)->syntax_check_is_a_bool_expr(list_ident);};

  // Fonction de vérification : est-ce un predicat B0 ?
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
	{return ((T_item *)object)->syntax_check_is_a_B0_predicate(list_ident);};


  // Est-ce l'expression "BOOL"
  virtual int check_is_BOOL(void);

  // Est-ce l'expression "FLOAT"
  virtual int check_is_FLOAT(void);

  // Est-ce l'expression "STRING"
  virtual int check_is_STRING(void);

  // Est-ce l'expression "NATURAL"
  virtual int check_is_NATURAL(void);

  // Est-ce un ensemble simple ?
  virtual int syntax_check_is_a_simple_set(void)
	{return ((T_item *)object)->syntax_check_is_a_simple_set();};

  // Est-ce un ensemble de fonctions totales ?
  virtual int syntax_check_is_a_total_function_set(void)
    {return ((T_item *)object)->syntax_check_is_a_total_function_set();};

  // Est-ce un ensemble de record (struct conforme au B0)
  virtual int syntax_check_is_a_record_set(void)
    {return ((T_item *)object)->syntax_check_is_a_record_set();}

  // Est-ce un ensemble en extension de termes simples pour
  // typage_appartenance_donnee_concrete
  virtual int syntax_check_is_a_simple_term_set(void)
    { return ((T_item *)object)->syntax_check_is_a_simple_term_set(); };

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item)
	{ return ((T_item *)object)->is_array_compatible_with(ref_item); };
#endif // B0C



	// Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_LIST_LINK_H_ */

