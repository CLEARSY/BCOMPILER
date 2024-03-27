/******************************* CLEARSY **************************************
* Fichier : $Id: c_item.h,v 2.0 2002-07-16 07:35:58 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe de base T_item
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
#ifndef _C_ITEM_H_
#define _C_ITEM_H_

#include "c_object.h"
#include "v_vistbl.h"

#ifdef B0C
#include "i_ctx.h"
#endif //B0C

class T_betree ;
class T_lexem ;
class T_comment ;
class T_symbol ;
class T_disk_manager ;
class T_lexem ;
class T_expr ;
class T_ident ;
class T_op ;
class T_machine ;
class T_type ;
class T_pragma ;


#ifdef B0C
class T_list_ident;
#endif //B0C


class T_item : public T_object
{
  // Betree d'appartenance
  // Reference consultative
  T_betree			*betree ;

  // Pere dans l'arbre
  // Reference consultative
  T_item   			*father ;

  // Chainage
  T_item				*next ;
  T_item				*prev ;

  // Nombre de liens
  int					links ;

  // Localisation dans le fichier
  T_lexem				*ref_lexem ;

  // Commentaires precedents
  // Reference allocative
  T_comment			*first_comment ;
  T_comment			*last_comment ;

  // Pragmas asscocies
  // Reference allocative
  T_pragma			*first_pragma ;
  T_pragma			*last_pragma ;

  //
  // Methodes privees
  //

  // Fonction qui verifie la validite des commentaires a accrocher
  // a l'item. Par defaut, ne fait rien. Redefini pour les definitions
  // afin que tous les commentaires de debut de fichier ne soit
  // accroches a la premiere definition
  // On ne peut pas, a ce niveau, utiliser de fonction virtuelle
  // car on est en train de construire l'instance et les pointeurs
  // de fonction virtuelle ne sont pas encore a jour
  T_lexem *check_comment(T_lexem *cur_comment) ;

protected:
  T_item(void) {} ;

  // Initialisation
  void init(T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;


public :
  T_item(T_node_type new_node_type) : T_object(new_node_type) {} ;
  // Constructeur avec chainage en queue
  T_item(T_node_type new_node_type,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem *new_ref_lexem) ;
  // Constructeur avec chainage apres un element
  T_item(T_node_type new_node_type,
		 T_item *after_this_item,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem *new_ref_lexem) ;
  virtual ~T_item() ;

  // Poser, retirer un lien
  T_item *link(void) ;
  void unlink(void) ;

	// Methodes d'acces
  virtual const char *get_class_name(void)	{ return "T_item" ; } ;
  T_betree *get_betree(void) 		{ return betree ; } ;
  T_item *get_father(void) 		{ return father ; } ;
  T_item *get_next(void) 			{ return next ; } ;
  T_item *get_prev(void) 			{ return prev ; } ;
  int get_links(void)				{ return links ; } ;
  T_lexem *get_ref_lexem(void) 	{ return ref_lexem ; } ;
  T_comment *get_comments(void)	{ return first_comment ; } ;
  // Liste complete des pragmas
  T_pragma *get_real_pragmas(void)	{ return first_pragma ; } ;
  // Liste des pragmas non traduits
  T_pragma *get_pragmas(void) ;

#if (defined (__BCOMP__) || defined (__INTERFACE__) )
  T_item **get_adr_father(void) { return &father ; } ;
  T_comment *get_first_comment(void) { return first_comment ; } ;
  T_comment *get_last_comment(void) { return last_comment ; } ;
  T_comment **get_adr_first_comment(void) { return &first_comment ; } ;
  T_comment **get_adr_last_comment(void) { return &last_comment ; } ;
  T_pragma **get_adr_first_pragma(void) { return &first_pragma ; } ;
  T_pragma **get_adr_last_pragma(void) { return &last_pragma ; } ;
  void set_first_comment(T_comment *new_first_comment)
	{ first_comment = new_first_comment ; } ;
  void set_last_comment(T_comment *new_last_comment)
	{ last_comment = new_last_comment ; } ;
#endif

	// Methodes d'ecriture
  void set_next(T_item *new_next) 	{ next = new_next ; } ;
  void set_prev(T_item *new_prev) 	{ prev = new_prev ; } ;
  void set_father(T_item *new_father) {father = new_father ;} ;
  void set_betree(T_betree *new_betree)
	{ betree = new_betree ; } ;
  void set_ref_lexem(T_lexem *new_ref_lexem)
	{ ref_lexem = new_ref_lexem ; } ;

	// Acces au lexeme precedent dans le fichier
  T_lexem *get_before_ref_lexem(void) ;

  //
  //	Methodes de gestion de liste chainee
  //

  // Fonction qui enleve un item d'une liste chainee
  void remove_from_list(T_item **adr_first,
								 T_item **adr_last) ;

  // Fonction qui remplace par un autre item item d'une liste chainee
  void exchange_in_list(T_item *new_item,
								 T_item **adr_first,
								 T_item **adr_last) ;

  // Chainage en tete d'une liste
  void head_insert(T_item **adr_first, T_item **adr_last) ;

  // Chainage en queue d'une liste
  void tail_insert(T_item **adr_first, T_item **adr_last) ;

  // Chainage apres un element (eventuellement NULL pour tete de liste)
  void insert_after(T_item *after_this_item,
							 T_item **adr_first,
							 T_item **adr_last) ;

  // Chainage avant un element (eventuellement NULL pour queue de liste)
  void insert_before(T_item *after_this_item,
							  T_item **adr_first,
							  T_item **adr_last) ;

  // Inversion d'une liste
  // IL FAUT QUE this SOIT LE PREMIER OBJET DE LA LISTE
  void reverse_list(T_item **adr_first, T_item **adr_last) ;

  // Demande de l'acces necessaire pour le fils ref
  // Ex: si this est un T_affect et que ref est en partie gauche,
  // alors la fonction renvoie AUTH_READ_WRITE
  // Ex: si this est un predicat, renvoie AUTH_READ
  // Par defaut, renvoie AUTH_READ
  virtual T_access_authorisation get_auth_request(T_item *ref) ;

  //jfm modif pour bug 2247
  //rend true si la substitution ou l'operation ne mofifie pas sa
  //machine, FALSE sinon
  int is_read_only(void) ;

  // Verification de typage
  // Utilise pour verifier qu'un identificateur est type avant d'etre
  // accede. Ne fait rien dans le cas general
  virtual void check_type() ;

  // Obtention du type B
  // get_B_type est virtuelle car redefinie pour les identificateurs
  // (qui cherchent leur type chez leur def)
  virtual T_type *get_B_type(void) ;

  // Mise a jour du type
  virtual void set_B_type(T_type *new_B_type, T_lexem *typing_location) ;

  // Fonction generique de calcul de type
  virtual void make_type(void) ;

  // Est-ce un ensemble ?
  virtual int is_a_set(void) ;

  // Est-ce une sequence ?
  virtual int is_a_seq(void) ;

  // Est-ce une sequence non vide ?
  virtual int is_a_non_empty_seq(void) ;

  // Est-ce un ensemble d'entiers ?
  virtual int is_an_integer_set(void) ;

  // Est-ce un ensemble de réels ?
  virtual int is_a_real_set(void) ;

  // Est-ce une relation ?
  virtual int is_a_relation(void) ;

  // Est-ce une formule typante ??
  // Par defaut : non !
  // Vrai que pour une conjonction de ':', '<:' ou '='
  virtual int is_a_typing_formula(void) ;

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

  // Verifications a posteriori pour les expressions
  // Doit aussi etre ici pour les indirections par T_list_link
  virtual void post_check(void) ;

  //
  //	Autres methodes
  //

  //	Fonction auxiliaire qui extrait une liste de parametres,
  //	separes par separator
  virtual void extract_params(int separator,
									   T_item *new_father,
									   T_item **adr_first,
									   T_item **adr_last) ;

  // Fonction qui transfere vole les commentaires de ref_item pour
  // les placer dans this
  void fetch_comments(T_item *ref_item) ;

  // Fonction auxiliaire qui extrait une liste de record_item,
  // separes par separator
  virtual void extract_record_items(T_item *new_father,
											 int allow_empty_label,
											 T_item **adr_first,
											 T_item **adr_last) ;

  //	Fonction qui fabrique un item a partir d'une item parenthesee
  virtual T_item *new_paren_item(T_betree *betree,
										  T_lexem *opn,
										  T_lexem *clo) ;

  // Recherche recursive d'un identificateur
  // Attention, au niveau du client, l'appel de find_ident doit
  // obligatoirement etre precede d'un appel a next_timestamp
  // afin de reinitialiser le mecanisme de non-bouclage
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de vérification B0
  virtual void B0_check();

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Inférence de type
  virtual void set_B_type_rec(T_type *);
  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

	// Recherche de l'operation et de la machine de definition d'un item
  virtual void find_machine_and_op_def(T_machine **adr_ref_machine,
												T_op **adr_ref_op) ;

  // Ajout dans la liste (adr_first, adr_last) des noms d'operations locales
  // appellees (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les instructions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms d'operations
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  // virtual void build_local_op_call_list(T_op *ref_op,
  // 												 T_item **adr_first,
  // 												 T_item **adr_last) ;

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


  //
  //	Methodes de test
  //

	// Pour savoir si un objet est un item
  virtual int is_an_item(void) ;

  // Pour savoir si un objet est une operation
  virtual int is_an_operation(void) ;

  // Pour savoir si un item  est une expression
  virtual int is_an_expr(void) ;

  // Pour savoir si un item  est une substitution
  virtual int is_a_substitution(void) ;

  // Pour savoir si un item  est un predicate
  virtual int is_a_predicate(void) ;

  // Pour savoir si un item  est un identificateur
  virtual int is_an_ident(void) ;

  // Pour savoir si un predicat est un predicat de typage de
  // parametre de machine
  virtual int is_a_machine_param_typing_predicate(void) ;

	// Pour savoir si un predicat est un predicat de typage de
	// constante
  virtual int is_a_constant_typing_predicate(void) ;

  // Pour savoir si un predicat est un predicat de typage de
  // variable
  virtual int is_a_variable_typing_predicate(void) ;

  // GP: 22/12/98
  //Pour savoir si l'item est autorise dans la branche d'un case
  virtual int is_expected_in_case_branch(void) {return FALSE ; }

  //Pour les types autorises dans une branche case:
  //retourne TRUE si la valeur du case_branch est égale à la valeur de this.
  //Retourne FALSE sinon
  //retourne FALSE pour tous les types non autorises dans la branche d'un case
  //pour la reprise sur erreur
  virtual int is_same_value(T_item* case_branch) {return FALSE;}

  //GP 22/01/99
  //Fonction qui verife que si un item est typee avec STRING
  //alors c'est un parametre d'entree de fonction.
  //Ne fait rien dans le cas general
  virtual void check_string_uses(int is_a_belong_pre) {return ; }

#ifdef B0C
  // Fonction de vérification : est-ce un term ?
  virtual int syntax_check_is_a_term(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce un terme simple ?
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int
  	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context = B0CTX_DEFAULT);

  // Fonction de vérification : est-ce un record en extension ?
  virtual int syntax_check_is_an_extension_record(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce un acces a un record en extension ?
  virtual int syntax_check_is_an_extension_record_access(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce un tableau ?
  virtual int syntax_check_is_an_array(T_list_ident **list_ident,
											   T_B0_context context = B0CTX_ARRAY_CONTEXT) ;

  // Fonction de vérification : est-ce un acces a un tableau ?
  virtual int syntax_check_is_an_array_access(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce une plage ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int
  	syntax_check_is_a_range(T_list_ident **list_ident,
							T_B0_context context = B0CTX_DEFAULT) ;

  // Fonction de vérification : est-ce un maplet ?
  virtual int
  	syntax_check_is_a_maplet(T_list_ident **list_ident,
							 T_B0_context context = B0CTX_ARRAY_CONTEXT) ;

  // Fonction de vérification : est-ce un singleton ?
  virtual int syntax_check_is_a_singleton(T_list_ident **list_ident,
												   T_B0_context context) ;

  // Fonction de vérification : est-ce une expression arithmetique ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int
  	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context = B0CTX_DEFAULT) ;

  // Fonction de vérification : est-ce une expression booleenne ?
  virtual int syntax_check_is_a_bool_expr(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce une expression B0 ?
  virtual int syntax_check_is_a_B0_expression(T_list_ident **list_ident) ;

  // Fonction de vérification : est-ce un predicat B0 ?
  virtual int syntax_check_is_a_B0_predicate(T_list_ident **list_ident) ;

  // Dans le cas d'une instance de T_binary_op representant un operateur operat
  // (TRUE -> BOP_TIMES) ou (FALSE -> BOP_MAPLET),
  // trouve l'element suivant
  T_item * get_next_expr(int operat);

  // On teste si l'item est defini dans une implementation
  // renvoie TRUE dans ce cas, FALSE sinon.
  virtual int check_is_in_an_implementation();

  //
  // Est-ce un ensemble simple ?
  virtual int syntax_check_is_a_simple_set(void);

  // Est-ce un ensemble de fonctions totales ?
  virtual int syntax_check_is_a_total_function_set(void);

  // Est-ce l'expression "BOOL"
  virtual int check_is_BOOL(void);

  // Est-ce l'expression "FLOAT"
  virtual int check_is_FLOAT(void);

  // Est-ce l'expression "STRING"
  virtual int check_is_STRING(void);

  // Est-ce l'expression "NATURAL"
  virtual int check_is_NATURAL(void);

  // Est-ce un ensemble de record (struct conforme au B0)
  virtual int syntax_check_is_a_record_set(void);

  // Est-ce un ensemble en extension de termes simples pour
  // typage_appartenance_donnee_concrete
  virtual int syntax_check_is_a_simple_term_set(void);

  // Dans ce cas renvoie this (cas par defaut)
  virtual  T_item * get_expr(int operat);

  //
  // Verification des expressions de typage des donnees concretes
  // en fonction des predicats de typage
  //

  // Cas d'une constante concrete
  // Predicat de typage =
  virtual int is_equal_concrete_constant_expr(void);

  // Predicat de typage :
  virtual int is_belong_concrete_data_expr(void);

  // Cas d'une variable concrete
  // Predicat de typage =
  virtual int is_equal_concrete_variable_expr(void);

  // Cas d'un parametre formel scalaire de machine
  // Predicat de typage =
  virtual int is_equal_scalar_formal_param_expr(void);

  // Predicat de typage :
  virtual int is_belong_scalar_formal_param_expr(void);

  // Cas d'un parametre d'entree d'operation
  // Predicat de typage =
  virtual int is_equal_in_op_param_expr(void);

  // Predicat de typage :
  virtual int is_belong_in_op_param_expr(void);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);
#endif  //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;


// Fonction qui fabrique un operateur binaire
extern T_item *new_binary_item(T_item *op1,
										T_item *op2,
										T_lexem *oper,
										T_betree *betree) ;

// Fonction qui fabrique un operateur unaire
extern T_item *new_unary_item(T_item *operand,
									   T_lexem *oper,
									   T_betree *betree) ;

// Fonction qui fabrique le type d'un item
extern T_symbol *make_type_name(T_item *item) ;

#ifdef STOP_AT_LINK
// Mode qui demande un assert lors du link
// dont le numero est passe en parametre
extern void set_stop_link_at_number(int new_number) ;
#endif

#ifdef STOP_AT_LINK
// Mode qui demande un assert lors du unlink
// dont le numero est passe en parametre
extern void set_stop_unlink_at_number(int new_number) ;
#endif

#endif /* _C_ITEM_H_ */
