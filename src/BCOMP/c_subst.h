/******************************* CLEARSY **************************************
* Fichier : $Id: c_subst.h,v 2.0 2002-07-16 08:21:59 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des substitutions
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
#ifndef _C_SUBST_H_
#define _C_SUBST_H_


#include "c_subtyp.h"
#include "c_item.h"

#ifdef B0C
#include "i_ctx.h"
#endif //B0C

class T_typing_predicate ;
class T_list_link ;
class T_flag ;
class T_valuation ;
#ifdef B0C
class T_list_ident;
#endif //B0C

class T_substitution : public T_item
{
  // Type de lien avec la substitution suivante
  // Valide ssi get_next() != NULL
  T_substitution_link_type	link_type ;

protected :
  T_substitution(void) {} ;
public :
  T_substitution(T_node_type new_node_type) : T_item(new_node_type) {} ;
  T_substitution(T_node_type new_node_type,
				 T_item **adr_first,
				 T_item **adr_last,
				 T_item *new_father,
				 T_betree *new_betree,
				 T_lexem **adr_ref_lexem)
	: T_item(new_node_type,
			 adr_first,
			 adr_last,
			 new_father,
			 new_betree,
			 (adr_ref_lexem == NULL) ? (T_lexem *)NULL : *adr_ref_lexem) {} ;

  T_substitution(T_node_type new_node_type,
				 T_item **adr_first,
				 T_item **adr_last,
				 T_item *new_father,
				 T_betree *new_betree,
				 T_lexem *adr_ref_lexem)
	: T_item(new_node_type,
			 adr_first,
			 adr_last,
			 new_father,
			 new_betree,
			 adr_ref_lexem) {} ;
  virtual ~T_substitution(void) ;

  void set_link_type(T_substitution_link_type new_link_type)
	{ link_type = new_link_type ; } ;

  // Les substitutions sont des substitutions
  virtual int is_a_substitution(void) { return TRUE ; } ;

  // On teste si la substitution appara�t dans un contexte
  // d'implantation (i.e. dans du code concret).
  // Renvoie TRUE dans ce cas, FALSE sinon.
  virtual int check_is_in_an_implementation();

  // Fonction de v�rification B0
  virtual void B0_check();

  // Verifier que dans deux subsitiutions en parallele (this et ref_subst),
  // les variables modifiees sont differentes
  void parallel_checks(T_substitution *ref_subst) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methode qui essaie de trouver un end qui manque
  virtual void check_for_missing_end(void)
	{ TRACE0("T_substitution::check_for_missing_end") ; } ;

  // Methodes de lecture
  T_substitution_link_type get_link_type(void) { return link_type ; } ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  virtual int is_read_only(void) =0 ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


  //verifie qu'il n'y a pas de cycle dans l'appel des op loc
  virtual void build_local_op_call_tree(T_list_link * cur_op_call) =0;
  //{TRACE0("On ne doit jamais passer ici"); ASSERT(FALSE);}

  // retourne vrai pour les substitutions closes par le mot clé END
  virtual bool hasEndKeyword() { return false; }

  // retourne vrai pour les subsitutions par lesquelles peut débuter un évenement : BEGIN, ANY, SELECT
  virtual bool isValidHeadSubstitutionForEvent() { return false;}

  virtual T_flag * getFlagForEndKeyword() { return NULL; }

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_begin : public T_substitution
{
  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

  // End de fin
  T_flag				*end_begin ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_begin() : T_substitution(NODE_BEGIN) {} ;
  T_begin(T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father,
		  T_betree *new_betree,
		  T_lexem **adr_ref_lexem) ;
  virtual ~T_begin(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_begin; }

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_begin" ; }  ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_begin(void)		   { return end_begin ; } ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  bool isValidHeadSubstitutionForEvent() { return true;}

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_skip : public T_substitution
{
public :
  T_skip() : T_substitution(NODE_SKIP) {} ;
  T_skip(T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem **adr_ref_lexem) ;
  virtual ~T_skip(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_skip" ; }  ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only()
	{
	  TRACE0("T_skip::is_read_only");
	  return TRUE;
	}
  void build_local_op_call_tree(T_list_link * cur_op_call)
	{TRACE0("appel a skip fin d'appel d'op loc"); }


  bool isValidHeadSubstitutionForEvent() { return true;}


  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_affect : public T_substitution
{
  // nom	: T_ident ou T_op_result (pour tableau)
  T_expr				*first_name ;
  T_expr				*last_name ;

  // value
  T_expr				*first_value ;
  T_expr				*last_value ;

public :
  T_affect() : T_substitution(NODE_AFFECT) {} ;
  T_affect(T_item *lhs,
		   T_item *rsh,
		   T_item **adr_first,
		   T_item **adr_last,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem *cur_lexem) ;
  virtual ~T_affect(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_affect" ; }  ;
  T_expr *get_lvalues(void)     		{ return first_name ; } ;
  T_expr *get_values(void)			{ return first_value ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

	// Demande de l'acces necessaire pour le fils ref
  virtual T_access_authorisation get_auth_request(T_item *ref) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  void build_local_op_call_tree(T_list_link * cur_op_call)
	{TRACE0("T_affect::build_local_op_call_tree, ok c'est fini");}
#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  bool isValidHeadSubstitutionForEvent() { return true;}

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_precond : public T_substitution
{
  // Predicat
  T_predicate			*predicate ;

  // Then
  T_flag				*then_keyword ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

  // Fin
  T_flag				*end_precond ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_precond() : T_substitution(NODE_PRECOND) {} ;
  T_precond(T_item **adr_first,
			T_item **adr_last,
			T_item *new_father,
			T_betree *new_betree,
			T_lexem **adr_ref_lexem) ;
  virtual ~T_precond(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_precond; }

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_precond" ; }  ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_then_keyword(void)		{ return then_keyword ; } ;
  T_flag *get_end_precond(void)		{ return end_precond ; } ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  void build_local_op_call_tree(T_list_link * cur_op_call);

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;

  // LV 22/09/1999 -- Ajout pour GNF
  void set_predicate(T_predicate *new_predicate)
	{ predicate = new_predicate ; } ;
  void set_body(T_substitution *new_body)
	{ first_body = last_body = new_body ; } ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_assert : public T_substitution
{
  // Predicat
  T_predicate			*predicate ;

  // Then
  T_flag				*then ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Fin
  T_flag				*end_assert ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_assert() : T_substitution(NODE_ASSERT) {} ;
  T_assert(T_item **adr_first,
		   T_item **adr_last,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem **adr_ref_lexem) ;
  virtual ~T_assert(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_assert" ; }  ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_assert(void)		{ return end_assert ; } ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_assert; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_label : public T_substitution
{
  // Predicat
  T_expr			*expression;

  // Then
  T_flag				*then ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

        // Fin
  T_flag				*end_assert ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
                                                                T_item **adr_last,
                                                                T_item *new_father,
                                                                T_betree *new_betree,
                                                                T_lexem **adr_ref_lexem) ;

public :
  T_label() : T_substitution(NODE_LABEL) {} ;
  T_label(T_item **adr_first,
                   T_item **adr_last,
                   T_item *new_father,
                   T_betree *new_betree,
                   T_lexem **adr_ref_lexem) ;
  virtual ~T_label(void) ;

        // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_label" ; }  ;
  T_expr *get_expression(void)	{ return expression ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_assert(void)		{ return end_assert ; } ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_assert; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

        // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
                                                                                                           T_item **adr_first,
                                                                                                           T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_jumpif : public T_substitution
{
  // Condition
  T_predicate			*cond;

  // Then
  T_flag				*to ;

  // Label
  T_expr			*label ;

  T_flag			*end_jumpif ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
                                                                T_item **adr_last,
                                                                T_item *new_father,
                                                                T_betree *new_betree,
                                                                T_lexem **adr_ref_lexem) ;

public :
  T_jumpif() : T_substitution(NODE_LABEL) {} ;
  T_jumpif(T_item **adr_first,
                   T_item **adr_last,
                   T_item *new_father,
                   T_betree *new_betree,
                   T_lexem **adr_ref_lexem) ;
  virtual ~T_jumpif(void) ;

        // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_jumpif" ; }  ;
  T_predicate *get_condition(void)	{ return cond ; } ;
  T_expr *get_label(void)	{ return label ; } ;
  T_flag *get_end_jumpif(void)		{ return end_jumpif ; } ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

        // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_jumpif; }


  // Fonction de v�rification B0
  virtual void B0_check();

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
                                                                                                           T_item **adr_first,
                                                                                                           T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_witness : public T_substitution
{
  // Predicat
  T_predicate			*predicate ;

  // Then
  T_flag				*then ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Fin
  T_flag				*end_witness ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_witness() : T_substitution(NODE_WITNESS) {} ;
  T_witness(T_item **adr_first,
		   T_item **adr_last,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem **adr_ref_lexem) ;
  virtual ~T_witness(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_witness" ; }  ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_witness(void)		{ return end_witness ; } ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_witness; }

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_or : public T_item
{
  // Choix
  T_substitution		*first_body ;
  T_substitution		*last_body ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_or() : T_item(NODE_OR) {} ;
  T_or(T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree,
	   T_lexem **adr_ref_lexem) ;
  virtual ~T_or(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_or" ; }  ;
  T_substitution *get_body(void)		{ return first_body ; } ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

	// Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_choice : public T_substitution
{
  // Choix CHOICE
  T_substitution		*first_body ;
  T_substitution		*last_body ;

  // Choix OR
  T_or		 		*first_or ;
  T_or		 		*last_or ;

	// Fin
  T_flag				*end_choice ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_choice() : T_substitution(NODE_CHOICE) {} ;
  T_choice(T_item **adr_first,
		   T_item **adr_last,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem **adr_ref_lexem) ;
  virtual ~T_choice(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_choice; }

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_choice" ; }  ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_or *get_or(void)					{ return first_or ; } ;
  T_flag *get_end_choice(void)		{ return end_choice ; } ;

	// Methode qui essaie de trouver un end qui manque
  virtual void check_for_missing_end(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;


  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;

  void build_local_op_call_tree(T_list_link * cur_op_call);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_else : public T_item
{
  // Condition (si NULL c'est un ELSE, sinon c'est un ELSIF)
  T_predicate		*cond ;

	// Instruction
  T_substitution	*first_body ;
  T_substitution	*last_body ;

	// Analyse syntaxique
  void syntax_analysis(int parse_cond,
								T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_else() : T_item(NODE_ELSE) {} ;
  // Constructeur avec parsing
  T_else(int parse_cond,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem **adr_ref_lexem,
		 T_lexem *else_ref_lexem) ;
  // Constructeur sans parsing
  T_else(T_substitution *new_first_body,
		 T_substitution *new_last_body,
		 T_predicate *new_cond,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem **adr_ref_lexem,
		 T_lexem *else_ref_lexem) ;

  virtual ~T_else(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_else" ; }  ;
  T_predicate *get_cond(void)		{ return cond ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_if : public T_substitution
{
  // Condition
  T_predicate			*cond ;

  // Corps si la condition est vraie
  T_substitution		*first_then_body ;
  T_substitution		*last_then_body ;

	// Alternatives
  T_else				*first_else ;
  T_else				*last_else ;

	// Fin
  T_flag				*end_if ;

  // Fonction d'analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_if() : T_substitution(NODE_IF) {} ;
  T_if(T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree,
	   T_lexem **adr_ref_lexem) ;
  virtual ~T_if(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_if" ; }  ;
  T_predicate *get_cond(void)		{ return cond ; } ;



  //T_expr *get_label(void)		{ return label ; } ;
  T_substitution *get_then_body(void){ return first_then_body ; } ;
  T_else *get_else(void)				{ return first_else ; } ;
  T_flag *get_end_if(void)			{ return end_if ; } ;
  T_else **get_adr_first_else(void) { return &first_else ; } ;
  T_else **get_adr_last_else(void) { return &last_else ; } ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_if; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void);

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_when : public T_item
{
  // Predicat (si NULL c'est un ELSE, sinon c'est un WHEN)
  T_predicate		*cond ;

	// Instruction
  T_substitution	*first_body ;
  T_substitution	*last_body ;

	// Fonction d'analyse syntaxique
  void syntax_analysis(int parse_cond,
								T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_when() : T_item(NODE_WHEN) {}  ;
  T_when(int parse_cond,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem **adr_ref_lexem) ;
  virtual ~T_when(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_when" ; }  ;
  T_predicate *get_cond(void)		{ return cond ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_select : public T_substitution
{
  // Condition
  T_predicate			*cond ;

  // Corps si la condition est vraie
  T_substitution		*first_then_body ;
  T_substitution		*last_then_body ;

	// Alternatives
  T_when				*first_when ;
  T_when				*last_when ;

	// Fin
  T_flag				*end_select ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_select() : T_substitution(NODE_SELECT) {} ;
  T_select(T_item **adr_first,
		   T_item **adr_last,
		   T_item *new_father,
		   T_betree *new_betree,
		   T_lexem **adr_ref_lexem) ;
  virtual ~T_select(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;


  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_select; }

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_select" ; }  ;
  T_predicate *get_cond(void)		{ return cond ; } ;
  T_substitution *get_then_body(void){ return first_then_body ; } ;
  T_when *get_when(void)				{ return first_when ; } ;
  T_flag *get_end_select(void)	   	{ return end_select ; } ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void);

  void build_local_op_call_tree(T_list_link * cur_op_call) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;



  bool isValidHeadSubstitutionForEvent() { return true;}

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_any : public T_substitution
{
  // Clause ANY
  T_ident				*first_ident ;
  T_ident				*last_ident ;

  // Clause WHERE
  T_predicate			*where ;

  // Clause THEN
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Fin
  T_flag				*end_any ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_any() : T_substitution(NODE_ANY) {} ;
  T_any(T_item **adr_first,
		T_item **adr_last,
		T_item *new_father,
		T_betree *new_betree,
		T_lexem **adr_ref_lexem) ;
  virtual ~T_any(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_any" ; }  ;
  T_ident *get_identifiers(void)	   	{ return first_ident ; } ;
  T_predicate *get_where(void)		{ return where ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_any(void)			{ return end_any ; } ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_any; }

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  void build_local_op_call_tree(T_list_link * cur_op_call) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;



  bool isValidHeadSubstitutionForEvent() { return true;}

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_let : public T_substitution
{
  // Identificateurs
  T_ident				*first_ident ;
  T_ident				*last_ident ;

  // Valuation
  T_valuation			*first_valuation ;
  T_valuation			*last_valuation ;

	// In
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Fin
  T_flag				*end_let ;

  // Fonction d'analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_let() : T_substitution(NODE_LET) {} ;
  T_let(T_item **adr_first,
		T_item **adr_last,
		T_item *new_father,
		T_betree *new_betree,
		T_lexem **adr_ref_lexem) ;
  virtual ~T_let(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_let" ; }  ;
  T_ident *get_lvalues(void)			{ return first_ident ; } ;
  T_valuation *get_valuations(void)	{ return first_valuation ; } ;
  T_substitution *get_body(void)		{ return first_body ; } ;
  T_flag *get_end_let(void)			{ return end_let ; }

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_let; }

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void);

  void build_local_op_call_tree(T_list_link * cur_op_call) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_inc,
													  T_list_link** ptr_last_inc,
													  int pass) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_becomes_member_of : public T_substitution
{
  // Identificateurs
  T_ident				*first_ident ;
  T_ident				*last_ident ;

  // Set
  T_expr				*set ;

public :
  T_becomes_member_of() : T_substitution(NODE_BECOMES_MEMBER_OF) {} ;
  T_becomes_member_of(T_item *op1,
					  T_item *op2,
					  T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem *ref_lexem) ;
  virtual ~T_becomes_member_of(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_becomes_member_of" ; } ;
  T_ident *get_lvalues(void)			{ return first_ident ; } ;
  T_expr *get_set(void)				{ return set ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  void build_local_op_call_tree(T_list_link * cur_op_call)
	{TRACE0("ok substituion terminale T_becomes_member_of"); }

  //GP 18/01/99
  virtual T_access_authorisation get_auth_request(T_item *ref);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_becomes_such_that : public T_substitution
{
  // Identificateurs
  T_ident				*first_ident ;
  T_ident				*last_ident ;

  // Predicat
  T_predicate		   	*pred ;

public :
  T_becomes_such_that() : T_substitution(NODE_BECOMES_SUCH_THAT) {} ;
  // Construction a partir d'un T_typing_predicate
  T_becomes_such_that(T_typing_predicate *tpred,
					  T_item *new_father,
					  T_betree *new_betree) ;
  virtual ~T_becomes_such_that(void) ;


	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_becomes_such_that" ; }
  T_ident *get_lvalues(void)			{ return first_ident ; }
  T_predicate *get_predicate(void)	{ return pred ; }

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  void build_local_op_call_tree(T_list_link *)
	{TRACE0("ok substitution terminale T_becomes_such_that"); }

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  //GP 18/01/99
  virtual T_access_authorisation get_auth_request(T_item *ref);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_var : public T_substitution
{
  // Identificateurs
  T_ident				*first_ident ;
  T_ident				*last_ident ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Fin
  T_flag				*end_var ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_var() : T_substitution(NODE_VAR) {}
  T_var(T_item **adr_first,
		T_item **adr_last,
		T_item *new_father,
		T_betree *new_betree,
		T_lexem **adr_ref_lexem) ;
  virtual ~T_var(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_var; }

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_var" ; }
  T_ident *get_lvalues(void)			{ return first_ident ; }
  T_ident *get_identifiers(void)  		{ return first_ident ; }
  T_substitution *get_body(void)		{ return first_body ; }
  T_flag *get_end_var(void)			{ return end_var ; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;


  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_case_item : public T_item
{
  // Valeurs literales qui determinent le choix (ou NULL pour default)
  T_item			*first_literal_value ;
  T_item			*last_literal_value ;

  // Instruction associee
  T_substitution	*first_body ;
  T_substitution	*last_body ;

	// Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_case_item() : T_item(NODE_CASE_ITEM) {}
  T_case_item(T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem **adr_ref_lexem) ;
  // Constructeur public
  T_case_item(T_item *new_first_literal_value,
			  T_item *new_last_literal_value,
			  T_substitution *new_first_body,
			  T_substitution *new_last_body,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree) ;

  virtual ~T_case_item(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Compatibilite ascendante
#define get_litteral_values get_literal_values
#define set_litteral_values set_literal_values

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_case_item" ; }
#define get_litteral_values get_literal_values
  T_item *get_literal_values(void)	{ return first_literal_value ; }
  T_substitution *get_body(void)		{ return first_body ; }

  // Methodes d'ecriture
  void set_literal_value(T_item *new_value)
	{
	  first_literal_value = last_literal_value = new_value ;
    }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();



  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_case : public T_substitution
{
  // Expression
  T_expr				*case_select ;

	// Cas possibles
  T_case_item			*first_case_item ;
  T_case_item			*last_case_item ;

	// Fin (des either)
  T_flag				*end_either ;

  // Fin (du case)
  T_flag				*end_case ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_case() : T_substitution(NODE_CASE) {}
  T_case(T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem **adr_ref_lexem) ;
  virtual ~T_case(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;


  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_case; }

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_case" ; }
  T_expr *get_case_select(void)		{ return case_select ; }
  T_case_item *get_case_items(void)	{ return first_case_item ; }
  T_flag *get_end_either(void)		{ return end_either ; }
  T_flag *get_end_case(void)			{ return end_case ; }
  T_case_item **get_adr_first_case_item(void) { return &first_case_item ; }
  T_case_item **get_adr_last_case_item(void) { return &last_case_item ; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_include, T_list_link** ptr_last_include, int pass) ;

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;


#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_op_call : public T_substitution
{
  // Nom de l'operation
  // 2 cas possibles :
  // - cas simple : op_name est un symbole (ex : 'f' dans 'f(x)')
  // - cas complique : op_name est un T_op_result (ex : 'f(x)' dans 'f(x)(y)')
  T_item				*op_name ;

  // Parametres d'entree
  // Instances de T_item car peuvent etre des expr, des predicats, des
  // conditions, ...
  T_item				*first_in_param ;
  T_item				*last_in_param ;

	// Parametres de sortie
  T_item				*first_out_param ;
  T_item				*last_out_param ;

  // Definition de l'operation appellee (T_op ou T_used_op)
  // Attention c'est une reference consultative
  T_generic_op   		*ref_op ;

  // Fonction utilisee par les constructeurs pour recuperer
  // les champs d'un T_op_result
  void fetch_params(T_op_result *op_res) ;

public :
  // Fonction verifiant la compatibilite de tableaux
  // entre parametres formels et effectifs
  //
  void internal_compatibility_B0_check(T_ident *cur_formal_param,
											 T_expr *cur_effective_param);

  T_op_call() : T_substitution(NODE_OP_CALL) {}
  // Constructeur par copie d'un T_op_res existant
  T_op_call(T_op_result *op_res,
					   T_item *new_father,
					   T_betree *new_betree) ;
  // Constructeur par copie d'un T_ident existant
  T_op_call(T_ident *ident,
					   T_item *new_father,
					   T_betree *new_betree) ;
  // Constructeur a partir du parseur LR
  T_op_call(T_item *op1,
			T_item *op2,
			T_item **adr_first,
			T_item **adr_last,
			T_item *new_father,
			T_betree *new_betree,
			T_lexem *new_ref_lexem) ;
  virtual ~T_op_call(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Demande de l'acces necessaire pour le fils ref
  virtual T_access_authorisation get_auth_request(T_item *ref) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_op_call" ; }
  virtual T_item *get_op_name(void)	  	{ return op_name ; }
  virtual T_item *get_in_params(void)	 { return first_in_param ; }
  virtual T_item *get_out_params(void)  { return first_out_param ; }
  virtual T_generic_op *get_ref_op(void)  	{ return ref_op ; }

  // Vrai nom de l'operation
  T_symbol *get_real_op_name(void) ;

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  //GP 20/01/99
  //Fonction qui permet de verifier que deux operations d'une
  //meme instance de machine incluse ne sont pas appelees en parallele
  //Dans le cas g�n�ral appel recursivement la fonction dans
  //les subtitutions de la substitution.
  //Dans ce cas precis, si op_call est defini dans une instance de machine
  //incluse et que cette instance appartient � la liste ptr_first_include
  //alors produit un message d'erreur. Sinon ajoute le pointeur
  //de l'instance de la machine � la liste.
  virtual void check_op_call_in_substitution(T_list_link** ptr_first_incl,
													  T_list_link** ptr_last_incl,
													  int pass) ;


  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;


  // Controles supplementaires pour les operations locales
  // CTRL REF SEMLOC 4a
  void semloc_4a(void) ;
  // CTRL REF SEMLOC 4b
  void semloc_4b(void) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;


#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_while : public T_substitution
{
  // Condition
  T_predicate		   	*cond ;

	// Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

	// Invariant
  T_flag				*invariant_keyword ;
  T_predicate			*invariant ;

	// Variant
  T_flag				*variant_keyword ;
  T_expr				*variant ;

	// Fin
  T_flag				*end_while ;

  // Analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;

public :
  T_while() : T_substitution(NODE_WHILE) {}
  T_while(T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father,
		  T_betree *new_betree,
		  T_lexem **adr_ref_lexem) ;
  virtual ~T_while(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual bool hasEndKeyword() { return true; }

  virtual T_flag * getFlagForEndKeyword() { return end_while; }

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_while" ; }
  T_predicate *get_cond(void)   		{ return cond ; }
  T_substitution *get_body(void)		{ return first_body ; }
  T_predicate *get_invariant(void)	{ return invariant ; }
  T_expr *get_variant(void)			{ return variant ; }
  T_flag *get_invariant_keyword(void){ return invariant_keyword ; }
  T_flag *get_variant_keyword(void)	{ return variant_keyword ; }
  T_flag *get_end_while(void)		{ return end_while ; }

  // Creation de la liste des variables modifiees par la substitution
  virtual void create_list_of_modvar(int first_phase) ;

	// Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de v�rification B0
  virtual void B0_check();

  // Pour savoir si une substitution modifie un identificateur
  // (passer sa definition en parametre !)
  // Renvoie TRUE si oui, FALSE sinon
  virtual int modifies(T_ident *ident) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne modifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

#ifdef B0C
  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;
#endif // B0C

  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  virtual void build_local_op_call_tree(T_list_link *cur_op_call) ;
  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Fonction qui parse les substitutions
extern T_substitution *syntax_get_substitution(T_item **adr_first,
														T_item **adr_last,
														T_item *father,
														T_betree *betree,
														T_lexem **adr_cur_lexem
#ifdef ENABLE_PAREN_IDENT
														// Parametre supplementaire
														// Si TRUE pas produire
														// de msg d'erreur en cas
														// de pb
														, int no_error_msg = FALSE
#endif
														) ;

// Type check d'une liste de substitutions, avec verification
// liee a l'utilisation eventuelle de '||'
extern void type_check_substitution_list(T_substitution *list) ;
// B0 check d'une liste de substitution
extern void B0_check_substitution_list(T_substitution *list) ;

#endif /* _C_SUBST_H_ */


