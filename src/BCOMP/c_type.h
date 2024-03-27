/******************************* CLEARSY **************************************
* Fichier : $Id: c_type.h,v 2.0 1999-11-04 18:08:04 lv Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Description des types B resolus
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
#ifndef _C_TYPE_H_
#define _C_TYPE_H_

class T_expr ;
class T_abstract_type ;
class T_B0_type ;

// Classe generique, qui modelise un type
class T_type : public T_item
{
  // Type B0 associe (valide apres l'etape de B0 CHECK)
  // Information generee seulement pour le typage des identificateurs
  T_B0_type		*B0_type ;

  // Nom "en ascii" du type
  // Calcule lors de l'appel de make_type_name
  T_symbol	   	*type_name ;

  // Type avant valuation
  T_type		*before_valuation_type ;

  // Identificateur typant
  T_ident			*typing_ident ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) = 0 ;

public :
  T_type(T_node_type new_node_type) : T_item(new_node_type) {} ;
  T_type(T_node_type new_node_type,
		 T_item *father,
		 T_betree *betree,
		 T_lexem *ref_lexem) ;
  virtual ~T_type(void) ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) = 0 ;// virtuelle pure


  // Fabrication du type B0 associe
  // Si force_creation == TRUE on force la creation d'un type B0,
  // i.e. on ne veut pas recevoir le type B0 existant s'il
  // existe. Utilise pour les calculs B0 des records qui contiennent
  // des tableaux, ...
  T_B0_type *make_B0_type(T_ident *ident,
								   int use_valuation_type,
								   T_item **adr_first,
								   T_item **adr_last,
								   int force_creation = FALSE)  ;
  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) = 0 ;
  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) = 0 ;
  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_type" ; } ;
  T_symbol *get_type_name(void) { return type_name ; } ;
  T_B0_type *get_B0_type(void) { return B0_type ; } ;
  T_type *get_before_valuation_type(void)
	{ return before_valuation_type ;} ;
  void set_before_valuation_type(T_type *new_before_valuation_type)
	{ before_valuation_type = new_before_valuation_type ; } ;

  // Obtention du type suivant dans la liste des types
  T_type *get_next_type(void) ;
  T_type *get_next_spec_type(void) ;

  // Methodes d'ecriture
  void set_B0_type(T_B0_type *new_B0_type) ;

  // Remise a NULL des bornes
  // Ne fait rien dans le cas general
  virtual void reset_bounds(void) ;

  // Fabrication du nom
  // si bounds = TRUE les bornes sont affichees
  T_symbol *make_type_name(int bounds = FALSE) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Calcul du nombre de types : 1 dans le cas general, sauf
  // pour les T_product_type ou cette methode est redefinie
  virtual int get_nb_types(void) ;

  // Surcharge get_next/get_prev
  T_type *get_next(void) ;
  T_type *get_prev(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  // Fonction virtuelle pure
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) = 0 ;

  // Pour savoir si deux types sont egaux
  // Dans le cas general : non !
  // Fonction virtuelle pure
  virtual int is_equal_to(T_type *ref_type) = 0 ;

  // ref_type est-il une valuation possible de T_type (plongement)
  // ident est le nom du type
  // cas par defaut : non
  virtual int can_be_valuated_by(T_type *ref_type,
										  T_ident *ident,
										  T_abstract_type **adr_atype) ;

  // Pour savoir si un type est un ensemble index large
  // Dans le cas general : non
  virtual int is_an_extended_index_set(void) ;

  // Est-ce un type ?
  virtual int is_a_type(void) ;

  // Pour savoir si type est un type de base
  // Dans le cas general : oui
  virtual int is_a_base_type(void) ;

  // Pour savoir si le type est un type entier
  // Par defaut : non !
  virtual int is_an_integer(void) ;

  // Pour savoir si le type est un type réel
  // Par defaut : non !
  virtual int is_a_real(void) ;

  // Pour savoir si le type est un type float
  // Par defaut : non !
  virtual int is_a_float(void) ;

  // Obtention des types apres valuation
  virtual T_type *get_real_type(void) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) = 0 ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) = 0 ;

  //GP 22/01/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir du type string
  virtual int is_based_on_string(void) ;

  // Est-ce le type P(P(Z*Z)*T) (T quelconque)
  // FALSE par défaut
  virtual int is_a_tree_type(void) ;

  // Est-ce le type P(Z*Z)*T (T quelconque)
  // FALSE par défaut
  virtual int is_a_tree_node_type(void) ;

  //GP 11/02/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir d'un type généric
  virtual int is_based_on_generic_type(void) ;

  // Pour le typage par identificateur :
  // est-ce une def de type tableau ?
  // est-ce une def de type record ?
  // est-ce une def de type entier ?
  virtual int is_an_array_type_definition(void) ;
  virtual int is_a_record_type_definition(void) ;
  virtual int is_an_integer_type_definition(void) ;

#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un type chaine de caractere (non dans le cas general)
  virtual int is_a_string(void);

  // Est-ce un type booleen (non dans le cas general)
  virtual int is_a_boolean(void);

  // Est-ce un type entier/booleen concret (non dans le cas general)
  virtual int is_a_concrete_integer(void);
  virtual int is_a_concrete_boolean(void);

  // Est-ce un element d'ensemble abstrait (non dans le cas general)
  virtual int is_an_abstract_element(void);

  // Est-ce un element d'un ensemble abstrait ou enumere
  // (non dans le cas general)
  virtual int is_an_abstract_or_enumerated_set_element(void);

  // Est-ce un ensemble abstrait ou enumere
  // (non dans le cas general)
  virtual int is_an_abstract_or_enumerated_set(void);

  // Est-ce un intervalle d'entiers ou abstrait
  // (non dans le cas general)
  virtual int is_an_integer_or_abstract_interval(void);

  // Est-ce un type tableau (non dans le cas general)
  virtual int is_an_array(void);

  // Est-ce un type record (non dans le cas general)
  virtual int is_a_record(void);

  // Est-ce un type d'element d'ensemble simple
  virtual int is_a_simple_set_element(void);


  //
  // Fonctions virtuelles de verification des types de chaque donnee concrete
  //

  // Type d'une constante concrete
  virtual int is_concrete_constant_type(void);

  // Type d'une variable concrete
  virtual int is_concrete_variable_type(void);

  // Type d'un parametre scalaire de machine
  virtual int is_scalare_parameter_type(void);

  // Type d'un parametre d'entree d'operation
  virtual int is_entry_parameter_type(void);

  // Type d'un parametre de sortie d'operation
  virtual int is_exit_parameter_type(void);

  // Type d'une variable locale
  virtual int is_local_variable_type(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);
#endif // B0C

  T_ident *get_typing_ident(void) ;
  void set_typing_ident(T_ident *new_typing_ident) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_base_type : public T_type
{
  // Nom du type (reference consultative)
  T_ident				*name ;

  // Bornes du type
  T_expr				*lower_bound ;
  T_expr				*upper_bound ;

public :
  T_base_type(T_node_type node_type) : T_type(node_type) {} ;
  T_base_type(T_node_type new_node_type,
			  T_ident *new_name,
			  T_expr *new_lower_bound,
			  T_expr *new_upper_bound,
			  T_item *father,
			  T_betree *betree,
			  T_lexem *ref_lexem) ;
  virtual ~T_base_type(void) ;

  // Generation des bornes
  void internal_gene_bounds(char *&value,
									 size_t used,
									 size_t allocated) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces
  int is_bounded(void)
	{ return (lower_bound == NULL) ? FALSE : TRUE ; } ;
  T_ident *get_name(void) { return name ; } ;
  T_expr *get_lower_bound(void)	{ return lower_bound ; } ;
  T_expr *get_upper_bound(void)	{ return upper_bound ; } ;
  virtual void set_bounds(T_expr *new_lower_bound,
								   T_expr *new_upper_bound) ;

  // Fonctions d'ecriture
  void set_name(T_ident *new_name) { name = new_name ; } ;

  // Remise a NULL des bornes
  // Ne fait rien dans le cas general
  virtual void reset_bounds(void) ;

  // Fonction qui dit si les bornes sont positionnes a une valeur literale
  int are_bounds_set(void) ;

  // Un type de base est un type de base
  virtual int is_a_base_type(void) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_constructor_type : public T_type
{
public :
  T_constructor_type(T_node_type node_type) : T_type(node_type) {} ;
  T_constructor_type(T_node_type new_node_type,
					 T_item *father,
					 T_betree *betree,
					 T_lexem *ref_lexem) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;


// Constructeur de type : produit cartesien
class T_product_type : public T_constructor_type
{
  // Les types
  T_type			*type1 ;
  T_type			*type2 ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public:
  T_product_type(void) : T_constructor_type(NODE_PRODUCT_TYPE) {} ;
  T_product_type(T_type *new_type1,
				 T_type *new_type2,
				 T_item *father,
				 T_betree *betree,
				 T_lexem *ref_lexem) ;
  virtual ~T_product_type(void) ;

	// Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Remise a NULL des bornes
  // Ne fait rien dans le cas general
  virtual void reset_bounds(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_product_type" ; } ;
  T_type *get_type1(void) ;
  T_type *get_type2(void) ;
  T_type *get_spec_type1(void) { return type1 ; } ;
  T_type *get_spec_type2(void) { return type2 ; } ;

  // Calcul du nombre de types contenus dans le produit cartesien
  virtual int get_nb_types(void) ;
  // idem types src/dst
  virtual int get_nb_src_types(void) ;
  virtual int get_nb_dst_types(void) ;

  // Obtention de la liste des types
  T_type *get_types(void) ;
  T_type *get_spec_types(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  //GP 22/01/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir du type string
  virtual int is_based_on_string(void) ;

  //GP 11/02/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir d'un type généric
  virtual int is_based_on_generic_type(void) ;

  // Est-ce le type P(Z*Z)*T (T quelconque)
  virtual int is_a_tree_node_type(void) ;

#ifdef B0C
  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);
#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Constructeur de type : Setof
class T_enumerated_type ;
class T_setof_type : public T_constructor_type
{
  T_type			*base_type ;

  // Fabrication du type B0 associe
  virtual T_B0_type *make_B0_abstract_type(T_abstract_type *ref_type,
										   T_ident *ident,
										   T_enumerated_type **adr_ref_enum,
										   T_item **adr_first,
										   T_item **adr_last) ;
  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public :
  T_setof_type(T_node_type new_node_type) : T_constructor_type(new_node_type) {} ;
  T_setof_type(void) : T_constructor_type(NODE_SETOF_TYPE) {} ;
  T_setof_type(T_type *new_base_type,
			   T_item *father,
			   T_betree *betree,
			   T_lexem *ref_lexem) ;
  T_setof_type(T_node_type new_node_type,
			   T_type *new_base_type,
			   T_item *father,
			   T_betree *betree,
			   T_lexem *ref_lexem) ;
  virtual ~T_setof_type(void) ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;


  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;


  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;
  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_setof_type" ; } ;
  virtual T_type *get_base_type(void) ;
  T_type *get_spec_base_type(void)	{ return base_type ; } ;

  // Methodes d'ecriture
  void set_base_type(T_type *new_base_type) ;

  // Remise a NULL des bornes
  // Ne fait rien dans le cas general
  virtual void reset_bounds(void) ;

  // Est-ce un ensemble ? Oui !
  virtual int is_a_set(void) ;

  // Est-ce une sequence ?Peut etre
  virtual int is_a_seq(void) ;

  // Est-ce une sequence non vide
  virtual int is_a_non_empty_seq(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour obtenir le type des elements d'une sequence
  // Attention le type doit etre une sequence i.e. l'appelant
  // doit le verifier avant d'appeler cette fonction
  // (avec is_a_seq() par exemple)
  T_type *get_seq_base_type(void) ;

  // Pour obtenir l'ensemble de depart (resp. arrivee) d'une relation
  // Attention le type doit etre une relation (a verifier
  // par l'appelant)
  virtual T_type *get_relation_src_type(void) ;
  virtual T_type *get_relation_dst_type(void) ;

  // ref_type est-il une valuation possible de T_type (plongement)
  // ident est le nom du type
  // Si oui, remplit **adr_atype avec le type abstrait a plonger
  virtual int can_be_valuated_by(T_type *ref_type,
										  T_ident *ident,
										  T_abstract_type **adr_atype) ;

  // Pour savoir si un type est un ensemble index large
  // Dans le cas general : non
  // TRUE ssi ensemble de base, ensemble en extension ou identificateur,
  // i.e. si le type est Setof(K(T, U, V))
  virtual int is_an_extended_index_set(void) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  //GP 22/01/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir du type string
  virtual int is_based_on_string(void) ;

  // Est-ce le type P(P(Z*Z)*T) (T quelconque)
  // FALSE par défaut
  virtual int is_a_tree_type(void) ;

  //GP 11/02/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir d'un type généric
  virtual int is_based_on_generic_type(void) ;

  // Pour le typage par identificateur :
  // est-ce une def de type tableau ?
  // est-ce une def de type record ?
  // est-ce une def de type entier ?
  virtual int is_an_array_type_definition(void) ;
  virtual int is_a_record_type_definition(void) ;
  virtual int is_an_integer_type_definition(void) ;


#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un ensemble abstrait ou enumere
  virtual int is_an_abstract_or_enumerated_set(void);

  // Est-ce un intervalle d'entiers ou abstrait
  virtual int is_an_integer_or_abstract_interval(void);

  // Est-ce un tableau
  virtual int is_an_array(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type de base associe a un ensemble (atype)
class T_abstract_type : public T_base_type
{
  // Ensemble de base
  T_ident			*set ;

  // Chainage dans la liste des types abstraits
  T_abstract_type	*next_abstract_type ;
  T_abstract_type	*prev_abstract_type ;

  // Type apres valuation
  T_type			*after_valuation_type ;

  // Identificateur avec lequel on value (ou NULL)
  T_ident			*valuation_ident ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public :
  T_abstract_type(void) : T_base_type(NODE_ABSTRACT_TYPE) {} ;
  T_abstract_type(T_ident *set,
				  T_expr *new_lower_bound,
				  T_expr *new_upper_bound,
				  T_item *father,
				  T_betree *betree,
				  T_lexem *ref_lexem) ;

  virtual ~T_abstract_type(void) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;
  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Mise a jour des bornes (valable pour un ensemble value !)
  virtual void set_bounds(T_expr *new_lower_bound,
								   T_expr *new_upper_bound) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_abstract_type" ; } ;
  T_ident *get_set(void) { return set ; } ;
  T_abstract_type *get_next_abstract_type(void)
	{ return next_abstract_type ; } ;
  T_abstract_type *get_prev_abstract_type(void)
	{ return prev_abstract_type ; } ;
  T_abstract_type **get_adr_next_abstract_type(void)
	{ return &next_abstract_type ; } ;
  T_type *get_after_valuation_type(void)
	{ return after_valuation_type ; } ;
  T_ident *get_valuation_ident(void) { return valuation_ident ; } ;

  // Methodes d'ecriture
  void set_next_abstract_type(T_abstract_type *new_next_abstract_type)
	{ next_abstract_type = new_next_abstract_type ; } ;
  void set_prev_abstract_type(T_abstract_type *new_prev_abstract_type)
	{ prev_abstract_type = new_prev_abstract_type ; } ;
  void set_after_valuation_type(T_type *new_after_valuation_type) ;
  void set_valuation_ident(T_ident *new_valuation_ident)
	{ valuation_ident = new_valuation_ident ; } ;

  // Obtention des types apres valuation
  virtual T_type *get_real_type(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un element d'ensemble abstrait
  virtual int is_an_abstract_element(void);

  // Est-ce un element d'un ensemble abstrait ou enumere
  virtual int is_an_abstract_or_enumerated_set_element(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type enumere (etype)
class T_enumerated_type : public T_base_type
{
  // Definition du type
  T_ident				*type_definition ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public :
  T_enumerated_type(void) : T_base_type(NODE_ENUMERATED_TYPE) {} ;
  T_enumerated_type(T_ident *new_type_definition,
					T_expr *lower_bound,
					T_expr *upper_bound,
					T_item *father,
					T_betree *betree,
					T_lexem *ref_lexem) ;

  virtual ~T_enumerated_type(void) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_enumerated_type" ; } ;
  T_ident *get_type_definition(void) { return type_definition ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un element d'un ensemble abstrait ou enumere
  virtual int is_an_abstract_or_enumerated_set_element(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type de base predefini
typedef enum
{
  BTYPE_INTEGER,
  BTYPE_BOOL,
  BTYPE_STRING,
  BTYPE_REAL,
  BTYPE_FLOAT
} T_builtin_type ;

class T_predefined_type : public T_base_type
{
  T_builtin_type	type ;

  // Dans le cas d'une constante intervalle,
  // reference sur l'intervalle
  T_ident			*ref_interval ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public :
  T_predefined_type(void) : T_base_type(NODE_PREDEFINED_TYPE) {} ;
  T_predefined_type(T_builtin_type new_type,
					T_expr *lower_bound,
					T_expr *upper_bound,
					T_item *father,
					T_betree *betree,
					T_lexem *ref_lexem) ;

  virtual ~T_predefined_type(void) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Methodes d'ecriture
  void set_ref_interval(T_ident *new_ref_interval) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_predefined_type" ; } ;
  T_builtin_type get_type(void) { return type ; } ;
  T_ident *get_ref_interval(void) { return ref_interval ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type est un type entier
  virtual int is_an_integer(void) ;

  // Pour savoir si le type est un type réel
  virtual int is_a_real(void) ;

  // Pour savoir si le type est un type float
  virtual int is_a_float(void) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  //GP 22/01/99
  //Fonction qui retourne TRUE si le type est construit
  //à partir du type string
  virtual int is_based_on_string(void) ;


#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un type chaine de caractere
  virtual int is_a_string(void);

  // Est-ce un type booleen
  virtual int is_a_boolean(void);

  // Est-ce un type entier/booleen concret
  virtual int is_a_concrete_integer(void);
  virtual int is_a_concrete_boolean(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);
  //  virtual int is_array_compatible_with(T_type *ref_type);



#endif //B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Pour l'inference de type, type de relation
typedef enum
{
  INF_RTYPE_ORDER,		// relation d'ordre : '=', '/=', '<', '>', '<=', '>='
  INF_RTYPE_BELONGS,		// appartenance		: ':'
  INF_RTYPE_INCLUDED		// inclusion 		: '<:'
} T_infer_relation_type ;


// Type générique
class T_generic_type : public T_type
{
  // Type complet apres instanciation
  // NULL tant que joker
  T_type				*type ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public :
  T_generic_type(void) : T_type(NODE_GENERIC_TYPE) {} ;
  T_generic_type(T_item *father, T_betree *betree, T_lexem *ref_lexem) ;
  virtual ~T_generic_type(void) ;

  virtual const char *get_class_name(void)
	{ return "T_generic_type" ; } ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  T_type *get_type(void) { return type ; } ;

  // Obtention des types apres valuation
  virtual T_type *get_real_type(void) ;

  // Type compatible ?
  virtual int is_compatible_with(T_type *ref_type, int low_ctrl=FALSE) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type est un type entier
  // Par defaut : non !
  virtual int is_an_integer(void) ;

  // Pour savoir si le type est un type réel
  // Par defaut : non !
  virtual int is_a_real(void) ;

  // Pour savoir si le type est un type float
  // Par defaut : non !
  virtual int is_a_float(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type record
class T_label_type : public T_type
{
  // Nom du label
  T_ident	*name ;

  // Type associe
  T_type	*type ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public:
  T_label_type(void) : T_type(NODE_LABEL_TYPE) {} ;
  T_label_type(T_ident *new_label_name,
			   T_setof_type *ref_setof,
			   T_type *new_label_type,
			   T_item **adr_first,
			   T_item **adr_last,
			   T_item *new_father,
			   T_betree *new_betree,
			   T_lexem *new_ref_lexem) ;

  virtual ~T_label_type(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_label_type" ; } ;
  T_ident *get_name(void) { return name ; } ;
  T_type *get_type(void) { return type ; } ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_record_type : public T_constructor_type
{
  // Les types
  T_label_type		*first_label ;
  T_label_type		*last_label ;

  // Fabrication du type B0 associe
  virtual T_B0_type *internal_make_B0_type(T_ident *ident,
													int use_valuation_type,
													T_item **adr_first,
													T_item **adr_last) ;

public:
  T_record_type(void) : T_constructor_type(NODE_RECORD_TYPE) {} ;
  T_record_type(T_item *father, T_betree *betree, T_lexem *ref_lexem) ;
  virtual ~T_record_type(void) ;

  // Clonage
  virtual T_type *clone_type(T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_type_name(char *&value,
												size_t &used,
												size_t &allocated,
												int bounds) ;

  // Fabrication du nom
  // La chaine est Valable jusqu'a l'appel suivant
  virtual void internal_make_readable_type_name(char *&value,
                                                size_t &used,
                                                size_t &allocated,
                                                int bounds) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_record_type" ; } ;
  T_label_type *get_labels(void) { return first_label ; } ;
  T_label_type **get_adr_first_label(void) { return &first_label ; } ;
  T_label_type **get_adr_last_label(void) { return &last_label ; } ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // pour savoir si deux types sont compatibles
  // Dans le cas general : non !
  virtual int is_compatible_with(T_type *ref_type,
										  int low_ctrl=FALSE) ;

  // Pour savoir si deux types sont egaux
  virtual int is_equal_to(T_type *ref_type) ;

  // Pour savoir si le type a des jokers (utilise pour les records)
  virtual int has_jokers(void) ;

  // Pour savoir si le type est defini (comprend un type * non instancie)
  virtual int has_wildcards(int ignore_valuation = FALSE) ;


#ifdef B0C
  // Fonctions virtuelles de verification de type B0

  // Est-ce un record
  virtual int is_a_record(void);

  // Fonctions de compatibilte des tableaux
  virtual int is_array_compatible_with(T_type *ref_type,
												T_item *array);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_TYPE_H_ */

