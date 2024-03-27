/******************************* CLEARSY **************************************
* Fichier : $Id: c_expr.h,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des expressions
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
#ifndef _C_EXPR_H_
#define _C_EXPR_H_


#include "c_item.h"
#include "c_exptyp.h"
#include "c_type.h"

#include "i_ctx.h"


class T_op ;
class T_predicate ;
class T_ident ;
class T_machine ;
class T_integer ;
class T_list_link ;
class T_keyword ;
class T_generic_op ;
class T_used_machine ;

#ifdef B0C
class T_list_ident;
#endif //B0C

class T_expr : public T_item
{
  // Type B
  // Disponible apres l'analyse semantique
  T_type			*B_type ;

  // Pour savoir si le type a deja ete calcule
  int				type_inferred ;

  // Indique si une erreur doit être levée en cas d'identificateur non typé dans l'expression
  static int               warn_ident;

  public :
  T_expr() : T_item(NODE_EXPR) { B_type = NULL ; type_inferred = FALSE ; }
  T_expr(T_node_type new_node_type) : T_item(new_node_type)
	{
	  B_type = NULL ;
	  type_inferred = FALSE ;
    }
  T_expr(T_node_type new_node_type,
		 T_item **adr_first,
		 T_item **adr_last,
		 T_item *new_father,
		 T_betree *new_betree,
		 T_lexem *new_ref_lexem) : T_item(new_node_type,
										  adr_first,
										  adr_last,
										  new_father,
										  new_betree,
										  new_ref_lexem)
	{
	  B_type = NULL ;
	  type_inferred = FALSE ;     
    }

  virtual ~T_expr(void) ;

	// Methodes de lecture
  virtual const char *get_class_name(void) { return "T_expr" ; }
  // get_B_type est virtuelle car redefinie pour les identificateurs
  // (qui cherchent leur type chez leur def)
  virtual T_type *get_B_type(void) ;
  virtual T_type *get_original_B_type(void) ;

  // Mise a jour du type: on sait qu'il est compatible avec l'existant
  // Récursif sur les sous-expressions
  // Ne s'applique que lorsque le type de l'expression n'est pas défini
  // ou que le type contient des variables de type.
  virtual void set_B_type(T_type *new_B_type, T_lexem *localisation) ;
  virtual void set_B_type_rec(T_type *new_B_type);

#ifdef __BCOMP__
  // Reset du type B
  void reset_B_type(void) ;
#endif

  // Positionne la valeur indiquant si une erreur doit être levée pour les identificateurs non typés
  static void set_warn_ident(int w);

  // Donne la valeur indiquant si une erreur doit être levée pour les identificateurs non typés
  static int get_warn_ident();

  // Verifications a posteriori
  virtual void post_check(void) ;

  // Les sous-classes de T_expr sont des expressions
  // (redefini dans T_binary_op pour les constructions temporaires)
  virtual int is_an_expr(void) ;

  // Est-ce un ensemble ?
  virtual int is_a_set(void) ;

  // Est-ce un ensemble d'entiers ?
  virtual int is_an_integer_set(void) ;

  // Est-ce un ensemble de réels ?
  virtual int is_a_real_set(void) ;

  // Est-ce une sequence ?
  virtual int is_a_seq(void) ;

  // Est-ce une sequence non vide ?
  virtual int is_a_non_empty_seq(void) ;

  // Est-ce une relation ?
  virtual int is_a_relation(void) ;

  // Est-ce une constante/variable ?
  // Par defaut, NON
  // OUI ssi classe T_ident avec le bon ident_type
  // Si ask_def vaut TRUE on parcourt les champs def pour connaitre le type
  virtual int is_a_constant(int ask_def = TRUE) ;
  virtual int is_a_variable(int ask_def = TRUE) ;

  // Renvoie l'expression "contenue" dans this
  // i.e. this pour la classe T_expr et sous-classes
  //      this->object pour T_list_link qui contient une expr, ...
  // prerequis : is_an_expr == TRUE
  virtual T_expr *make_expr(void) ;

  // Fonction de typage et de verification de type
  // Pour une expression, type_check = make_type (cf ci-dessous)
  virtual void type_check(void) ;

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);

  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

  // Fonction generique de calcul de type
  virtual void make_type(void) ;
  virtual void internal_make_type(void) = 0 ;

  //
  //	}{	Fonction qui fabrique un item a partir d'une item parenthesee
  //
  virtual T_item *new_paren_item(T_betree *betree,
										  T_lexem *opn,
										  T_lexem *clo) ;

  // Fonction qui rend l'expression sans les parentheses qui l'entourent
  // Cas general : on rend l'objet lui-meme
  // (redefini dans la classe T_expr_with_parenthesis)
  virtual T_expr *strip_parenthesis(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonction auxiliaire qui extrait une liste de record_item,
  // separes par separator
  virtual void extract_record_items(T_item *new_father,
											 int allow_empty_label,
											 T_item **adr_first,
											 T_item **adr_last) ;
  //GP 25/01/99
  //Fonction qui dans le cas general leve une erreur
  //quand une expression est du type string
  virtual void prohibit_strings_use(void) ;

#ifdef B0C
  // On verifie que l'expression est un produit cartesien d'ensembles simples
  // et est donc valide pour etre l'ensemble source d'une fonction totale
  virtual void syntax_check_is_a_total_function_source_set(void);


#endif // B0C

  // Acces au typing_ident
  // En fait les infos sont stockees dans le type, ces methodes sont des raccourcis
  T_ident *get_typing_ident(void) ;
  void set_typing_ident(T_ident *new_typing_ident) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  static void check_field_offsets();
} ;

class T_op_call ;
class T_array_item ;
class T_op_result : public T_expr
{
  friend class T_op_call ;
  friend class T_array_item ;

  // Nom de l'operation
  // 2 cas possibles :
  // - cas simple : op_name est un identifiant (ex : 'f' dans 'f(x)')
  // - cas complique : op_name est un T_op_result (ex : 'f(x)' dans 'f(x)(y))
  //   voire une expression (ex : '(s\|/1)(x)')
  //   voire un T_record_access (ex : record'tableau(1))
  //   bref un item dans le cas general
  T_item		   	*op_name ;

  // Si opération builtin, mot-clef associé
  T_keyword		   	*ref_builtin ;

  // Parametres d'entree
  // Instances de T_item car peuvent etre des expr, des predicats, des
  // conditions, ...
  // Ex : bool(Predicat)
  T_item		   	*first_in_param ;
  T_item		   	*last_in_param ;

  // Nombre de parametres
  size_t		   	nb_in_params ;

  // Definition de l'operation appellee
  // Attention c'est une reference consultative
  T_generic_op 	   	*ref_op ;

  // op_result suivant (chainage simple au sein de l'object
  // manager, utilise pour changer les T_op_result en
  // T_array_item le cas echeant)
  T_op_result		*next_op_result ;

  // Si en fait c'est un T_array_item et pas un T_op_result,
  // pointe vers le T_array_item corrige
  T_array_item		*ref_array_item ;

  //
  //	Methodes privees
  //
  // Fonction auxiliaire qui verifie qu'il n'y a qu un seul argument
  // a la fonction et qui calcule son type.
  // Renvoie TRUE si ok, FALSE sinon
  int check_single_param(T_keyword *keyword) ;
  // Methodes de construction de type
  void vtype_pow_fin(T_keyword *keyword) ;
  void vtype_min_max(T_keyword *keyword) ;
  void vtype_card(T_keyword *keyword) ;
  void vtype_seq_iseq_perm(T_keyword *keyword) ;
  void vtype_front_tail_rev(T_keyword *keyword) ;
  void vtype_first_last(T_keyword *keyword) ;
  void vtype_size(T_keyword *keyword) ;
  void vtype_id(T_keyword *keyword) ;
  void vtype_dom(T_keyword *keyword) ;
  void vtype_ran(T_keyword *keyword) ;
  void vtype_closure(T_keyword *keyword) ;
  void vtype_iterate(T_keyword *keyword) ;
  void vtype_prj(T_keyword *keyword, int is_prj1) ;
  void vtype_bool(void) ;
  void vtype_conc(T_keyword *keyword) ;
  void vtype_succ_pred(T_keyword *keyword) ;
  void vtype_rel(T_keyword *keyword) ;
  void vtype_fnc(T_keyword *keyword) ;
  void vtype_tree(T_keyword *keyword) ;
  void vtype_const(T_keyword *keyword) ;
  void vtype_top(T_keyword *keyword) ;
  void vtype_sons(T_keyword *keyword) ;
  void vtype_pre_post_fix(T_keyword *keyword) ;
  void vtype_sizet(T_keyword *keyword) ;
  void vtype_mirror(T_keyword *keyword) ;
  void vtype_rank(T_keyword *keyword) ;
  void vtype_father(T_keyword *keyword) ;
  void vtype_son(T_keyword *keyword) ;
  void vtype_subtree(T_keyword *keyword) ;
  void vtype_arity(T_keyword *keyword) ;
  void vtype_bin(T_keyword *keyword) ;
  void vtype_left_right(T_keyword *keyword) ;
  void vtype_infix(T_keyword *keyword) ;
  void vtype_from_integer_to_real(T_keyword *keyword);
  void vtype_from_real_to_integer(T_keyword *keyword);


  public :
  T_op_result() : T_expr(NODE_OP_RESULT) {} ;
  T_op_result(T_item *,
			  T_item *params,
			  T_ident *ref_builtin_op, // Si != NULL : op builtin
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem) ;

  virtual ~T_op_result(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Verifications a posteriori
  virtual void post_check(void) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_op_result" ; }  ;
  virtual T_item *get_op_name(void)			{ return op_name ; } ;
  virtual T_item *get_in_params(void)		{ return first_in_param ; } ;
  virtual T_item *get_out_params(void)		{ return NULL ; } ;
  virtual T_generic_op *get_ref_op(void)	   	{ return ref_op ; } ;
  T_op_result *get_next_op_result(void) { return next_op_result ; } ;
  T_array_item *get_ref_array_item(void) { return ref_array_item ; } ;
  T_keyword *get_ref_builtin(void) { return ref_builtin ; } ;

#ifdef B0C
  size_t get_nb_in_params(void) { return nb_in_params ; } ;
#endif // B0C

  // Methodes d'ecriture
  void set_op_name(T_item *new_op_name) { op_name = new_op_name ; } ;
  void set_next_op_result(T_op_result *new_next_op_result) ;

  // Fonction de verfication (est-ce un T_op_result ou un T_array_item ?)
  // Corrige l'arbre le cas echeant
  void check_is_an_op_result(void) ;

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);

#ifdef B0C

  // Fonction de vérification : est-ce une expression arithmetique ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
													 T_B0_context = B0CTX_DEFAULT);

  // Fonction de vérification : est-ce une expression booleenne ?
  virtual int syntax_check_is_a_bool_expr(T_list_ident **list_ident);

  // Fonction de vérification : est-ce un acces a un tableau ?
  virtual int syntax_check_is_an_array_access(T_list_ident **list_ident);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);

  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int
  	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context = B0CTX_DEFAULT) ;


  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#endif  //B0C

  // Rattrapages eventuels sur le clonage : rien dans le cas general
  // Redefini dans la classe T_op_result (maj liste op_result)
  virtual void fix_clone(T_object *ref_object) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_expr_with_parenthesis : public T_expr
{
  // Expression a l'interieur de la parenthese
  T_expr			*expr ;

  public :
  T_expr_with_parenthesis() : T_expr(NODE_EXPR_WITH_PARENTHESIS) {} ;
  T_expr_with_parenthesis(T_expr *new_expr,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem *new_ref_lexem) ;

  virtual ~T_expr_with_parenthesis(void) ;

 	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  void set_B_type_rec(T_type* type);

  // Methodes de lecture
  virtual const char *get_class_name(void)
  { return "T_expr_with_parenthesis" ; }  ;
  T_expr *get_expr(void) 			{ return expr ; } ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

#ifdef COMPAT
  // Methode qui permet de dire qu'un lexeme doit etre ignore par le
  // decompilateur. Si le lexeme provient de l'expansion d'une
  // definition, on propage la correction a la definition elle-meme
  void mark_as_void(void) ;
#endif

  //Ajout CT
  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction qui rend l'expression sans les parentheses qui l'entourent
  virtual T_expr *strip_parenthesis(void) ;

#ifdef B0C
  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);

  //
  // Fonctions de vérification : on suit l'indirection...
  //

  // Fonction de vérification : est-ce un term ?
  virtual int syntax_check_is_a_term(T_list_ident **list_ident)
  {return expr->syntax_check_is_a_term(list_ident);};

  // Fonction de vérification : est-ce un terme simple ?
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int
  	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context = B0CTX_DEFAULT) ;

  // Fonction de vérification : est-ce un record en extension ?
  virtual int syntax_check_is_an_extension_record(T_list_ident **list_ident)
	{return expr->syntax_check_is_an_extension_record(list_ident);};

  // Fonction de vérification : est-ce un acces a un record en extension ?
  virtual int
  syntax_check_is_an_extension_record_access(T_list_ident **list_ident)
  	{return expr->syntax_check_is_an_extension_record_access(list_ident);};

  // Fonction de vérification : est-ce un tableau ?
  virtual int syntax_check_is_an_array(T_list_ident **list_ident,
												T_B0_context context
												= B0CTX_ARRAY_CONTEXT) ;


  // Fonction de vérification : est-ce un acces a un tableau ?
  virtual int syntax_check_is_an_array_access(T_list_ident **list_ident)
  	{return expr->syntax_check_is_an_array_access(list_ident);};

  // Fonction de vérification : est-ce une plage ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_a_range(T_list_ident **list_ident,
											   T_B0_context context
											   = B0CTX_DEFAULT) ;


  // Fonction de vérification : est-ce un maplet ?
  virtual int syntax_check_is_a_maplet(T_list_ident **list_ident,
												T_B0_context context
												= B0CTX_ARRAY_CONTEXT) ;

  // Fonction de vérification : est-ce un singleton ?
  virtual int syntax_check_is_a_singleton(T_list_ident **list_ident,
												   T_B0_context context)
  	{return expr->syntax_check_is_a_singleton(list_ident, context);};

  // Fonction de vérification : est-ce une expression arithmetique ?
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
													 T_B0_context context
													 = B0CTX_DEFAULT) ;

  // Fonction de vérification : est-ce une expression booleenne ?
  virtual int syntax_check_is_a_bool_expr(T_list_ident **list_ident)
	{return expr->syntax_check_is_a_bool_expr(list_ident);};

  // Est-ce un ensemble simple ?
  virtual int syntax_check_is_a_simple_set(void)
	{return expr->syntax_check_is_a_simple_set();};

  // Est-ce un ensemble de fonctions totales ?
  virtual int syntax_check_is_a_total_function_set(void)
    {return expr->syntax_check_is_a_total_function_set();};

  // Est-ce l'expression "BOOL"
  virtual int check_is_BOOL(void)
    {return expr->check_is_BOOL();};

  // Est-ce l'expression "FLOAT"
  virtual int check_is_FLOAT(void)
    {return expr->check_is_FLOAT();};

  // Est-ce un ensemble de record (struct conforme au B0)
  virtual int syntax_check_is_a_record_set(void)
    {return expr->syntax_check_is_a_record_set();};

  // Est-ce un ensemble en extension de termes simples pour
  // typage_appartenance_donnee_concrete
  virtual int syntax_check_is_a_simple_term_set(void)
    {return expr->syntax_check_is_a_simple_term_set();};


  // On verifie que l'expression est un produit cartesien d'ensembles simples
  // et est donc valide pour etre l'ensemble source d'une fonction totale
  virtual void syntax_check_is_a_total_function_source_set(void);

  // Dans ce cas renvoie, on suit l'indirection
  virtual  T_item * get_expr(int operat);

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);
  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#endif  //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_image : public T_expr
{
  // Relation
  T_expr			*relation ;

	// Expression dont on cherche l'image
  T_expr			*expr ;

  public :
  T_image() : T_expr(NODE_IMAGE) {} ;
  T_image(T_expr *new_expr,
		  T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father,
		  T_betree *new_betree,
		  T_lexem *new_ref_lexem) ;

  virtual ~T_image(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
	// "fin" du constructeur : on met en place la relation
  void end_constructor(T_item *new_relation) ;

  // Phase de correction : si relation = NULL alors il faut
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_image" ; } ;
  T_expr *get_relation(void)			{ return relation ; } ;
  T_expr *get_expr(void) 		   	{ return expr ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;


class T_sigma : public T_expr
{
protected:
  // Elements dont on fait la somme
  T_ident			*first_variable ;
  T_ident			*last_variable ;

  // Predicat
  T_predicate		*predicate ;

  // Expression
  T_expr			*expr ;

  public :
  T_sigma() : T_expr(NODE_SIGMA) {} ;

  T_sigma(T_node_type new_node_type) : T_expr(new_node_type) {};

  T_sigma(T_node_type new_node_type,
          T_item *new_variable,
		  T_item *new_expr,
		  T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father,
		  T_betree *new_betree,
		  T_lexem *new_ref_lexem) ;

  virtual ~T_sigma(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_sigma" ; } ;
  T_ident *get_variables(void)		{ return first_variable ; } ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;
  T_expr *get_expr(void)				{ return expr ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void);

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

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


class T_extensive_seq : public T_expr
{
  // Elements
  T_expr			*first_item ;
  T_expr			*last_item ;

  public :
  T_extensive_seq() : T_expr(NODE_EXTENSIVE_SEQ) {} ;
  // Constructeur a partir d'un T_image
  T_extensive_seq(T_image *ref_image,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *new_father,
				  T_betree *new_betree,
				  T_lexem *new_ref_lexem) ;

  virtual ~T_extensive_seq(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual void set_B_type_rec(T_type* type);

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_extensive_seq" ; } ;
  T_expr *get_items(void)			{ return first_item ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction
  virtual T_item *check_tree(T_item **ref_this) ;

	// Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_lambda_expr : public T_expr
{
  // Variable
  T_ident			*first_variable ;
  T_ident			*last_variable ;

  // Typage
  T_predicate		*predicate ;

  // Expression
  T_expr			*expr ;

  public :
  T_lambda_expr() : T_expr(NODE_LAMBDA_EXPR) {} ;
  T_lambda_expr(T_item *new_variable,
				T_item *new_expr,
				T_item **adr_first,
				T_item **adr_last,
				T_item *new_father,
				T_betree *new_betree,
				T_lexem *new_ref_lexem) ;

  virtual ~T_lambda_expr(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual void set_B_type_rec(T_type* type);

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_lambda_expr" ; }  ;
  T_ident *get_variables(void)		{ return first_variable ; } ;
  T_predicate *get_predicate(void)	{ return predicate ; } ;
  T_expr *get_expr(void)				{ return expr ; } ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_unary_op : public T_expr
{
  // Operandes
  T_expr				*operand ;

	// Operateur
  T_unary_operator	oper ;

  public :
  T_unary_op() : T_expr(NODE_UNARY_OP) {} ;
  T_unary_op(T_item *new_operand,
			 T_unary_operator new_oper,
			 T_item **adr_first,
			 T_item **adr_last,
			 T_item *new_father,
			 T_betree *new_betree,
			 T_lexem *new_ref_lexem) ;

#if ( defined(__INTERFACE__) || defined(__BCOMP__) )
  // Constructeur pour l'interface B0Tree->Betree
  T_unary_op(T_predicate *new_operand,
			 T_unary_operator new_oper,
			 T_item **adr_first,
			 T_item **adr_last,
			 T_item *new_father,
			 T_betree *new_betree,
			 T_lexem *new_ref_lexem) ;
#endif

  virtual ~T_unary_op(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) 	{ return "T_unary_op" ; }  ;
  T_expr *get_operand(void)				{ return operand ; } ;
  T_unary_operator get_operator(void)	{ return oper ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

#ifdef B0C
  // Est-ce une expression arithmetique
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
														   T_B0_context context = B0CTX_DEFAULT);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_binary_op : public T_expr
{
  // Operandes
  T_item				*op1 ;
  T_item				*op2 ;

  // Operandes n-aires
  T_list_link			*first_operand ;
  T_list_link	   		*last_operand ;

	// Nombre d'operandes
  int				nb_operands ;

  // Operateur
  T_binary_operator	oper ;

  //
  //	Methodes privees
  //
  // Mise en place des operandes n-aires
  void fetch_operands(void) ;

	// Fabrication du type dans chacun des cas de oper
  void make_bop_interval_type(void) ;
  void make_bop_divides_plus_type(void) ;
  void make_bop_mod_type(void) ;
  void make_bop_power_type(void) ;
  void make_bop_minus_type(void) ;
  void make_bop_times_type(void) ;
  void make_bop_set_relation_type(void) ;
  void make_bop_composition_type(void) ;
  void make_bop_maplet_type(void) ;
  void make_bop_union_intersect_type(void) ;
  void make_bop_head_insert_type(void) ;
  void make_bop_tail_insert_type(void) ;
  void make_bop_head_tail_restrict_type(void) ;
  void make_bop_restrict_antirestrict_type(void) ;
  void make_bop_corestrict_anticorestrict_type(void) ;
  void make_bop_direct_product_type(void) ;
  void make_bop_parallel_product_type(void) ;
  void make_bop_left_overload_type(void) ;
  void make_bop_concat_type(void) ;

  public :
  T_binary_op() : T_expr(NODE_BINARY_OP) {} ;
  T_binary_op(T_item *new_op1,
			  T_item *new_op2,
			  T_binary_operator new_oper,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem) ;

  virtual ~T_binary_op(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual void set_B_type_rec(T_type* type);

  // Forcer un re-calcul de la liste des operandes
  void fix_operand_list(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes de lecture
  virtual const char *get_class_name(void) 	{ return "T_binary_op" ; }  ;
  T_item *get_op1(void)					{ return op1 ; } ;
  T_item *get_op2(void)					{ return op2 ; } ;
#ifdef ENABLE_PAREN_IDENT
  T_item **get_adr_op1(void)					{ return &op1 ; } ;
  T_item **get_adr_op2(void)					{ return &op2 ; } ;
#endif
  T_list_link *get_operands(void)		   		{ return first_operand ; } ;
  T_binary_operator get_operator(void)	{ return oper ; } ;

#ifdef FIX_MAPLETS
  void set_operator(T_binary_operator new_oper) { oper = new_oper ; } ;
#endif // FIX_MAPLETS

  // Reset des operandes
  void reset_operands(void)
	{
	  op1 = op2 = NULL ;
	  first_operand = last_operand = NULL ;
	}

  // Est-ce une expression valide ? (refuse les constructions temporaires)
  virtual int is_an_expr(void) ;

	// Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  //
  //	}{	Fonction qui fabrique un item a partir d'une item parenthesee
  //
  virtual T_item *new_paren_item(T_betree *betree,
										  T_lexem *opn,
										  T_lexem *clo) ;

  //
  //	}{	Fonction auxiliaire qui extrait une liste de parametres
  //
  virtual void extract_params(int separator,
									   T_item *new_father,
									   T_item **adr_first,
									   T_item **adr_last) ;

  // Fonction auxiliaire qui extrait une liste de record_item,
  // separes par separator
  virtual void extract_record_items(T_item *new_father,
											 int allow_empty_label,
											 T_item **adr_first,
											 T_item **adr_last) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

#ifdef B0C
  // Est-ce une expression arithmetique
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
														   T_B0_context context = B0CTX_DEFAULT);

  // Est-ce une expression de tableau
  virtual int syntax_check_is_an_array(T_list_ident **list_ident,
											   T_B0_context context = B0CTX_ARRAY_CONTEXT);

  // Est-ce une expression de maplet
  virtual int syntax_check_is_a_maplet(T_list_ident **list_ident,
												T_B0_context context = B0CTX_ARRAY_CONTEXT);

  // Est-ce une plage
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_a_range(T_list_ident **list_ident,
											   T_B0_context context = B0CTX_DEFAULT);

  // Dans le cas d'une instance de T_op_result representant un operateur
  // operat,(TRUE -> BOP_TIMES) ou (FALSE -> BOP_MAPLET),
  // trouve recursivement le premier element
  virtual  T_item * get_expr(int operat);

  // On verifie que l'expression est un produit cartesien d'ensembles simples
  // et est donc valide pour etre l'ensemble source d'une fonction totale
  virtual void syntax_check_is_a_total_function_source_set(void);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);


#endif // B0C

  virtual int is_static(void);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_array_item : public T_expr
{
  // Nom du tableau
  // 2 cas possibles :
  // cas simple : array_name est un identifiant (ex : 'f' dans 'f(x)')
  // cas complique : array_name est un T_array_item (ex:'f(x)' dans 'f(x)(y)')
  //   voire une expression (ex : '(s\|/1)(x)')
  //   bref un item dans le cas general

  // Tableau
  T_item		   	*array_name ;
  // Separateur d'indexes
  T_binary_operator separator;

  // Index
  T_expr			*first_index ;
  T_expr			*last_index ;

  // Nombre d'indexes
  size_t			nb_indexes ;

  // Methodes privees
  // Fonction auxilaire de production du message d'erreur
  void type_check_error(void) ;

  // Calcul du nombre d'indexes
  void compute_nb_indexes(void) ;

  public :
  T_array_item() : T_expr(NODE_ARRAY_ITEM) {} ;
  // Constructeur a partir d'un T_op_result deja construit
  T_array_item(T_op_result *ref_op_result,
			   T_item **adr_first,
			   T_item **adr_last,
			   T_item *new_father,
			   T_betree *new_betree,
			   T_lexem *new_ref_lexem) ;
  virtual ~T_array_item(void) ;

  T_access_authorisation get_auth_request(T_item *ref) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes d'accès
  virtual const char *get_class_name(void)
  { return "T_array_item" ; }  ;
  T_item *get_array_name(void)		{ return array_name ; } ;
  T_expr *get_indexes(void)		{ return first_index ; } ;
  size_t get_nb_indexes(void) 		{ return nb_indexes ; } ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  void set_B_type_rec(T_type* type);

  T_binary_operator get_separator() {return separator;}
#ifdef B0C
  // Fonction virtuelle : Est-ce un acces a un tableau B0
  virtual int syntax_check_is_an_array_access(T_list_ident **list_ident);

  // Fonction de vérification : est-ce une expression arithmetique ?
  // SL 23/09/99 suite a modif Spec B0C (FB)
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
													 T_B0_context = B0CTX_DEFAULT);

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);
  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  virtual int is_static (void);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_empty_seq : public T_expr
{
  public :
  T_empty_seq() : T_expr(NODE_EMPTY_SEQ) {} ;
  T_empty_seq(T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem) ;

  virtual ~T_empty_seq(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_empty_seq" ; } ;

	// Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Compatibilite versions precedentes
#define T_litteral_string T_literal_string
class T_literal_string : public T_expr
{
  T_symbol		*value ;

  public :
  T_literal_string() : T_expr(NODE_LITERAL_STRING) {} ;
  // Construction a partir d'un lexeme
  T_literal_string(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem *new_ref_lexem) ;
  // Construction a partir d'une chaine
  T_literal_string(const char *value,
					int new_value_len,
					T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem *new_ref_lexem) ;

  virtual ~T_literal_string(void) ;

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);

  // test de référecement: retourne vrai ssi l'identificateur ident
  // est référencé dans l'expression
  virtual T_ident *B0_OM_refer_to(T_ident *ident);

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_literal_string" ; }  ;
  T_symbol *get_value(void) 			{ return value ; } ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

   //GP 25/01/99
  //Fonction qui dans le cas general leve une erreur
  //quand une expression est du type string
  //Sauf dans ce cas particulier
  virtual void prohibit_strings_use(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_literal_real : public T_expr
{
  T_symbol		*value ;

  public :
  T_literal_real() : T_expr(NODE_LITERAL_REAL) {}
  // Construction a partir d'un lexeme
  T_literal_real(T_item **adr_first,
                                        T_item **adr_last,
                                        T_item *new_father,
                                        T_betree *new_betree,
                                        T_lexem *new_ref_lexem) ;
  // Construction a partir d'une chaine
  T_literal_real(const char *value,
                                        int new_value_len,
                                        T_item **adr_first,
                                        T_item **adr_last,
                                        T_item *new_father,
                                        T_betree *new_betree,
                                        T_lexem *new_ref_lexem) ;

  virtual ~T_literal_real(void) ;

  // Fonction de vérification de type
  virtual T_ident *B0_check_expr(T_ident *expected_type,
                                                                                  T_lexem *err_pos,
                                                                                  T_lexem *invalid_err_pos,
                                                                                  T_B0_type_context ctx);

  // test de référecement: retourne vrai ssi l'identificateur ident
  // est référencé dans l'expression
  virtual T_ident *B0_OM_refer_to(T_ident *ident);

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_literal_real" ; }
  T_symbol *get_value(void) 			{ return value ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_pi : public T_expr
{
protected:
  // Elements dont on fait le produit
  T_ident			*first_variable ;
  T_ident			*last_variable ;

  // Predicat
  T_predicate		*predicate ;

  // Expression
  T_expr			*expr ;

  public :
  T_pi() : T_expr(NODE_PI) {}
  T_pi(T_node_type new_node_type) : T_expr(new_node_type) {}
  T_pi(   T_node_type new_node_type,
          T_item *new_variable,
	   T_item *new_expr,
	   T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree,
	   T_lexem *new_ref_lexem) ;

  virtual ~T_pi(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_pi" ; }
  T_ident *get_variables(void)		{ return first_variable ; }
  T_predicate *get_predicate(void)	{ return predicate ; }
  T_expr *get_expr(void)				{ return expr ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void);

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Compatibilite versions precedentes
#define T_litteral_boolean T_literal_boolean
class T_literal_boolean : public T_expr
{
  int					value ;

  public :
  T_literal_boolean() : T_expr(NODE_LITERAL_BOOLEAN) {}
  T_literal_boolean(T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;

  virtual ~T_literal_boolean(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_literal_boolean" ; }
  int get_value(void) 				{ return value ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void);

  //GP 22/12/98
  //Renvoie TRUE quand l'item est autorise dans la branche d'un case
  virtual int is_expected_in_case_branch(void) {return TRUE ; }

  //GP 28/12/98
  //retourne TRUE si case_barnch est du type T_LITERAL_BOOLEAN
  //et que case_barnch->value = this->value
  virtual int is_same_value(T_item* case_branch) ;

#ifdef B0C
  // Est-ce un terme simple : oui
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int syntax_check_is_a_simple_term(T_list_ident **list_ident,
													 T_B0_context context = B0CTX_DEFAULT);

  // Function de vérification de type B0
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);

  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  static void check_field_offsets();
} ;

class T_comprehensive_set : public T_expr
{
  // Identifiants
  T_ident			*first_ident ;
  T_ident			*last_ident ;

  // Predicat
  T_predicate		*predicate ;

  public :
  T_comprehensive_set() : T_expr(NODE_COMPREHENSIVE_SET) {}
  T_comprehensive_set(T_expr *new_expr,
					  T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem *new_ref_lexem) ;

  virtual ~T_comprehensive_set(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_comprehensive_set" ; }
  T_ident *get_identifiers(void)		{ return first_ident ; }
  T_predicate *get_predicate(void)	{ return predicate ; }

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Fonction de typage et de verification de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_extensive_set : public T_expr
{
  // Elements
  T_expr			*first_item ;
  T_expr			*last_item ;

  public :
  T_extensive_set() : T_expr(NODE_EXTENSIVE_SET) {}
  T_extensive_set(T_expr *expr,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *new_father,
				  T_betree *new_betree,
				  T_lexem *new_ref_lexem) ;

  virtual ~T_extensive_set(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  virtual void set_B_type_rec(T_type* type);
  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes de lecture
  virtual const char *get_class_name(void)
  { return "T_extensive_set" ; }
  T_expr *get_items(void)			{ return first_item ; }

  // Méthode d'ajout
  void add_item(T_expr *new_item);

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

#ifdef B0C
  // Fonction virtuelle : Est-ce une expression de tableau
  virtual int
  	syntax_check_is_an_array(T_list_ident **list_ident,
							 T_B0_context context = B0CTX_ARRAY_CONTEXT);

  // Fonction virtuelle : Est-ce une expression de singleton
  virtual int syntax_check_is_a_singleton(T_list_ident **list_ident,
												   T_B0_context context);

  // Est-ce un ensemble en extension de termes simples pour
  // typage_appartenance_donnee_concrete
  virtual int syntax_check_is_a_simple_term_set(void);

  virtual T_ident *B0_check_expr(T_ident *expected_type,
								 T_lexem *err_pos,
								 T_lexem *invalid_err_pos,
								 T_B0_type_context ctx);
  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  virtual int is_static(void);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_empty_set : public T_expr
{
  public :
  T_empty_set() : T_expr(NODE_EMPTY_SET) {} ;
  T_empty_set(T_item **adr_first,
			  T_item **adr_last,
			  T_item *new_father,
			  T_betree *new_betree,
			  T_lexem *new_ref_lexem)  ;
  virtual ~T_empty_set(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_empty_set" ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Est-ce une relation ? oui !
  virtual int is_a_relation(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_converse : public T_expr
{
  // Expression
  T_expr		*expr ;

  public :
  T_converse() : T_expr(NODE_CONVERSE) {}
  T_converse(T_expr *new_expr,
			 T_item **adr_first,
			 T_item **adr_last,
			 T_item *new_father,
			 T_betree *new_betree,
			 T_lexem *new_ref_lexem) ;

  virtual ~T_converse(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_converse" ; }
  T_expr *get_expr(void)				{ return expr ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_generalised_intersection : public T_expr
{
  // Expressions
  T_expr			*first_expr ;
  T_expr			*last_expr ;

  public :
  T_generalised_intersection() : T_expr(NODE_GENERALISED_INTERSECTION) {}
  // Constructeur a partir d'un T_op_result
  T_generalised_intersection(T_op_result *ref_op_result,
							 T_item **adr_first,
							 T_item **adr_last,
							 T_item *new_father,
							 T_betree *new_betree,
							 T_lexem *new_ref_lexem) ;

  virtual ~T_generalised_intersection(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
  { return "T_generalised_intersection" ; }
  T_expr *get_expressions(void) 		{ return first_expr ; }

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_quantified_intersection : public T_expr
{
  // Identifiants
  T_ident			*first_variable ;
  T_ident			*last_variable ;

  // Caracterisation
  T_predicate			*predicate ;

  // Expression
  T_expr			*expr ;

  public :
  T_quantified_intersection() : T_expr(NODE_QUANTIFIED_INTERSECTION) {}
  T_quantified_intersection(T_item *new_variable,
							T_item *new_expr,
							T_item **adr_first,
							T_item **adr_last,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *new_ref_lexem) ;

  virtual ~T_quantified_intersection(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
  { return "T_quantified_intersection" ; }
  T_ident *get_variables(void)			{ return first_variable ; }
  T_predicate *get_predicate(void)			{ return predicate ; }
  T_expr *get_expression(void) 			{ return expr ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_generalised_union : public T_expr
{
  T_expr			*first_expr ;
  T_expr			*last_expr ;

  public :
  T_generalised_union() : T_expr(NODE_GENERALISED_UNION) {}
  // Constructeur a partir d'un T_op_result
  T_generalised_union(T_op_result *ref_op_result,
					  T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem *new_ref_lexem) ;

  virtual ~T_generalised_union(void) ;
  // Verifications a posteriori
  virtual void post_check(void) ;
  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_generalised_union" ; }
  T_expr *get_expressions(void) 		{ return first_expr ; }

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_quantified_union : public T_expr
{
  // Identifiants
  T_ident			*first_variable ;
  T_ident			*last_variable ;

  // Caracterisation
  T_predicate  		*predicate ;

  // Expression
  T_expr			*expr ;

  public :
  T_quantified_union() : T_expr(NODE_QUANTIFIED_UNION) {}
  T_quantified_union(T_item *new_variable,
					 T_item *new_expr,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;

  virtual ~T_quantified_union(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Verifications a posteriori
  virtual void post_check(void) ;
  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
  { return "T_quantified_union" ; } ;
  T_ident *get_variables(void)			{ return first_variable ; }
  T_predicate *get_predicate(void)	   	{ return predicate ; }
  T_expr *get_expression(void) 	   	{ return expr ; }

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Phase de correction
  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Compatibilite versions precedentes
#define T_litteral_integer T_literal_integer
class T_literal_integer : public T_expr
{
  // Valeur absolue
  T_integer	   	*value ;

  public :
  T_literal_integer() : T_expr(NODE_LITERAL_INTEGER) {}
  // Constructeur avec valeur explicite
  T_literal_integer(size_t new_value,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;
  // Constructeur avec valeur recuperee dans le lexeme
  T_literal_integer(T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem) ;

  virtual ~T_literal_integer(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
    { return "T_literal_integer" ; }
  T_integer *get_value(void)	{ return value ; }

  // Devient l'oppose de lui-meme
  void set_opposite(void) ;

  //
  // Methodes de comparaison
  //
  // Renvoie TRUE ssi this < ref ou si ref == NULL
  int is_less_than(T_literal_integer *ref) ;

  // Renvoie TRUE ssi this > ref ou si ref == NULL
  int is_greater_than(T_literal_integer *ref) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // GP 22/12/98
  // Renvoie TRUE quand l'item est autorise dans la branche d'un case
  virtual int is_expected_in_case_branch(void) { return TRUE ; }

  //GP 28/12/98
  //retourne TRUE si case_barnch est du type T_LITERAL_INTEGER
  //et que case_barnch->value = this->value
  virtual int is_same_value(T_item* case_branch) ;

  // Verifications a posteriori que les entiers litteraux ne sont pas
  // superieur a MAXINT
  // ou inferieur a MININT

  virtual void post_check(void) ;

#ifdef B0C
  // Est-ce un terme simple : oui
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int syntax_check_is_a_simple_term(T_list_ident **list_ident,
													 T_B0_context context = B0CTX_DEFAULT);

  // Est-ce une expression arithmetique : oui
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int
  	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context = B0CTX_DEFAULT) ;

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);

  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);
  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Fonction qui parse les expressions
extern T_expr *syntax_get_expr(T_item **adr_first,
										T_item **adr_last,
										T_item *father,
										T_betree *betree,
										T_lexem **adr_cur_lexem) ;

// Fonction qui parse les instanciations
extern T_expr *syntax_get_instanciation(T_item **adr_first,
												 T_item **adr_last,
												 T_item *father,
												 T_betree *betree,
												 T_lexem **adr_cur_lexem) ;

// Fonction qui parse les termes simples
extern T_expr *syntax_get_simple_term(T_item **adr_first,
											   T_item **adr_last,
											   T_item *father,
											   T_betree *betree,
											   T_lexem **adr_cur_lexem) ;

// Fonction qui parse les identificateurs
// Le type de l'identificateur recherche est a fournir par le client
// (s'il ne le connait pas, il doit utiliser ITYPE_UNKONWN)
extern T_ident *syntax_get_ident(T_ident_type ident_type,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *father,
										  T_betree *betree,
										  T_lexem **adr_cur_lexem) ;

// Fonction qui fabrique un atome
extern T_item *new_atom(T_betree *betree, T_lexem *cur_lexem) ;

// Fonction qui fabrique un pst
extern T_expr *new_pst(T_expr *operand,
								T_lexem *pst,
								T_betree *betree,
								T_lexem *cur_lexem) ;

// Fonction auxiliaire qui extrait l'identificateur passe en
// parametre si c'est un ident, un op_result ou un array_item, i.e.
// x, f(...) ou tab(...)
// Renvoie NULL en cas d'erreur
extern T_ident *fetch_ident(T_expr *expr) ;

#endif /* _C_EXPR_H_ */
