/******************************* CLEARSY **************************************
* Fichier : $Id: c_ident.h,v 2.0 2002-07-16 07:48:49 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des identificateurs
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
#ifndef _C_IDENT_H_
#define _C_IDENT_H_

#ifdef B0C
#include "i_ctx.h"

class T_list_ident;
#endif //B0C

class T_keyword ;
class T_type ;

//
//	}{	Classe T_ident
//
class T_ident : public T_expr
{
  // Nom
  T_symbol		*name ;

  // Dans le cas d'un identificateur de type "xx$0", acces au prefixe
  // i.e. "xx" et au suffixe i.e. "0".  La valeur du suffixe n'est
  // significative que si "prefix" est non NULL.
  T_symbol		*prefix ;
  int   		suffix_number ;

  // Type d'indentificateur
  T_ident_type	ident_type ;

  // Reference consultative sur la definition
  T_ident			*def ;

  // Machine de definition
  // Reference consultative
  T_machine		*ref_machine ;

  // Operation de definition
  // Reference consultative
  T_op			*ref_op ;

  // Dans le cas d'un identificateur colle,
  // reference sur l'identificateur "de definition"
  T_ident			*ref_glued ;

  // Si l'identificateur herite d'un autre, reference sur cet ident
  // (par exemple le cas avec INCLUDES). Sinon NULL
  T_ident			*ref_inherited ;

  // Anneau des identificateurs representant la meme donnee, i.e. de tous
  // les identificateurs lies par une suite de ref_glued ou de
  // ref_inherited avec l'identificateur
  T_ident			*ref_glued_ring ;

  // Ident implémentant éventuellement this
  T_ident           *implemented_by ;

  // pointeur sur l'homonyme éventuel de l'abstraction
  // pour un ident dans une machine ce pointeur est null
  T_ident           *homonym_in_abstraction ;

  // pointeur sur l'homonyme éventuel venant des machines requises
  T_ident           *homonym_in_required_mach ;

  // boolean : == true <=> l'ident est explicite dans le source
  int                explicitly_declared ;

  // Valeur (cas d'une constante ou d'un SETS enumere)
  T_expr			*first_value ;
  T_expr			*last_value ;

  // Chainage dans la liste des identificateurs
  T_ident			*next_ident ;
  T_ident			*prev_ident ;

  // Chainage dans la liste des identificateurs non resolus le cas echeant
  // ie si ident_type = ITYPE_UNKONWN
  T_ident			*next_unsolved_ident ;

  // Localisation du typage
  T_lexem			*typing_location ;

  // Localisation du typage "originel"
  // Non NULL seulement en cas de type "plonge" : typing_location donne
  // la localisation du plongement, et original_typing_location la localisation
  // du typage original
  T_lexem			*original_typing_location ;

  // Type B0
  T_B0_type			*B0_type ;

  // Représente LE prédicat de typage (ou LA substitution de typage
  // pour les variables locales et les paramètres de sortie
  // d'opération) de l'ident s'il (elle) existe. Le prédicat de typage
  // d'une donnée est LE premier prédicat (ou LA première
  // substitution) typant(e) qui contient la donnée en partie gauche
  T_item                        *typing_pred_or_subst ;

  //Relation entre la machine qui defini l'ident et celle
  //qui l'utilise
  T_access_entity_location access_entity_location ;

  // Pour savoir si l'identifiant est herite ou pas :
  // pas herite = defini explicitement dans le source
  // herite = provient de l'abstraction ou d'une machine incluse
  int				inherited ;

  // Le prédicat de typage a-t-il été vérifié pour le B0 ?
  int				 B0_type_checked ;

  // Lexem pour la déclaration B0 typée (':' ou '=')
  T_lexem                        *BOM_lexem_type;

  // Type donnée dans les déclaration B0 typée
  T_item                         *BOM_type;

  // Lexem de l'initialisation toujour '=' utilisé uniquement pour
  // positionner les messages d'erreur
  T_lexem                        *BOM_lexem_init_value;
  // valeur initiale pour les variables locales
  T_item                         *BOM_init_value;

  // Initialisation (fonction auxiliaire utilisee par les constructeurs)
  void init(const char *new_name, T_lexem *new_ref_lexem) ;

  // Calcul du T_access_entity_type correspodant a l'identificateur
  // (fonction auxilaire utilisee au cours de la resolution)
  T_access_entity_type make_entity_type(T_ident *cur_ident) ;

  // Calcul du T_access_from_context correspodant a l'identificateur
  // (fonction auxilaire utilisee au cours de la resolution)
  T_access_from_context make_context(T_access_entity_type etype) ;

  // Verifie que les droits obtenus sont suffisants
  int check_rights(T_ident *def_ident,
							T_access_authorisation auth) ;

  // Mise a jour du type sans verification
  void internal_set_B_type(T_type *new_B_type, T_lexem *localisation) ;

  // Pour tous les abstract_type comme atype, propagatation de la valuation
  // Idem pour tous les identificateurs values avec ces types
  void fix_atype_list(T_setof_type *setof,
							   T_abstract_type *atype,
							   T_abstract_type *list_atype) ;

protected :
  T_ident(T_node_type new_node_type) : T_expr(new_node_type) {} ;
  //  void set_name(T_symbol *new_name) { name = new_name ; } ;



public :
  T_ident() : T_expr(NODE_IDENT) {} ;

  // Constructeur a partir d'un nom et d'un type
  T_ident(const char *new_name,
		  T_ident_type new_ident_type,
		  T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father,
		  T_betree *new_betree,
		  T_lexem *new_ref_lexem) ;

  // Constructeur pour identificateur builtin
  T_ident(T_keyword *builtin_name) ;

  // Constructeur de clonage
  T_ident(T_ident *ref_ident,
		  T_item **adr_first,
		  T_item **adr_last,
		  T_item *new_father) ;

  // Destructeur
  virtual ~T_ident(void) ;

  void set_name(T_symbol *new_name) { name = new_name ; } ;

  // Parse la valeur d'un ensemble en extension
  // Prend la main sur le '{'
  // Rend la main apres le '}'
  void parse_set_values(T_lexem **adr_cur_lexem) ;

  // Verification de typage
  // Utilise pour verifier qu'un identificateur est type avant d'etre
  // accede. Ne fait rien dans le cas general
  virtual void check_type() ;


  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Verification que l'identificateur pourra etre utilise sans probleme
  // par le couple Kernel/Prouveur en aval du compilateur
  // Detecte en particulier une partie des "conflits caches"
  // si get_proof_clash_detection() vaut TRUE
  void check_kernel_compatibility(void) ;

  // Recherche de l'operation et de la machine de definition d'un item
  virtual void find_machine_and_op_def(T_machine **adr_ref_machine,
												T_op **adr_ref_op) ;

  // Provoquer une nouvelle recherche de l'operation et de la machine
  // de definition
  virtual void reset_machine_and_op_def(void) ;

  // Dans le cas de "xx$0", verification de la validite
  // Controles syntaxiques puis semantiques
  void syntaxic_check_dollar_zero_integrity(void) ;
  void semantic_check_dollar_zero_integrity(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si un item  est un identificateur
  // Les identificateurs sont des identificateurs
  virtual int is_an_ident(void) 	;

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

  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Pour savoir si l'identifiant est herite ou pas :
  // pas herite = defini explicitement dans le source
  // herite = provient de l'abstraction ou d'une machine incluse
  int get_inherited(void) { return inherited ; } ;
  void set_inherited(int new_inherited) { inherited = new_inherited ; } ;

  // Pour savoir si le prédicat de typage a été vérifié pour le B0.
  int get_B0_type_checked(void)
	{ return B0_type_checked ; } ;
  void set_B0_type_checked(int new_B0_type_checked)
	{ B0_type_checked = new_B0_type_checked ; } ;

  void set_BOM_lexem_type(T_lexem *lt){BOM_lexem_type = lt;}
  T_lexem *get_BOM_lexem_type(){return BOM_lexem_type;}
  void set_BOM_type (T_item *item){BOM_type = item;}
  T_item *get_BOM_type(void){return BOM_type;}
  void set_BOM_init_value(T_lexem *lex, T_item *item)
	{
	  BOM_lexem_init_value = lex;
	  BOM_init_value = item;
	}
  T_lexem *get_BOM_lexem_init_value(void)
	{return BOM_lexem_init_value;}
  T_item *get_BOM_init_value(void){return BOM_init_value;}


  // Fonction sémantiques
  // est-ce un type
  int is_a_BOM_type(void);

  int is_a_BOM_complete_type(void);

  int is_an_array(void);
  int is_an_array_type(void);
  int is_an_operator(void);

  T_ident *get_dst_type(void);
  T_ident *get_src1_type(void);
  T_ident *get_src2_type(void);

  // Est-ce une constante ?
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

  //GP 22/01/99
  //Fonction qui verife que si un item est typee avec STRING
  //alors c'est un parametre d'entree de fonction.
  //Ne fait rien dans le cas general
  //Dans ce cas fait la verifiaction
  virtual void check_string_uses(int is_a_belong_pre) ;

  //GP 25/01/99
  //Fonction qui dans le cas general leve une erreur
  //quand une expression est du type string
  //Sauf dans ce cas particulier
  virtual void prohibit_strings_use(void) ;

   // GP 16/11/98 pour EREN 1 & 2
  //Verifi que l'ident n'est pas importee d'une instance renommée
  //def_ident: ident de definition de l'ident actuel
  virtual T_ident* check_rename(T_ident * def_ident) ;

#ifdef B0C
  // est-ce un term simple : les identificateurs sont des termes simples
  // (syntaxiquement)
  //
  // le parametre context est utilise dans le cadre de la verification
  // RTYPETAB 1-4 : si il vaut 1, la verification est effectue
  virtual int syntax_check_is_a_simple_term(T_list_ident **list_ident,
													 T_B0_context context = B0CTX_DEFAULT) ;

  // Est-ce une expression arithmetique
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_an_arith_expr(T_list_ident **list_ident,
														   T_B0_context context = B0CTX_DEFAULT) ;

  // Est-ce une expression de tableau
  virtual int syntax_check_is_an_array(T_list_ident **list_ident,
											   T_B0_context = B0CTX_ARRAY_CONTEXT);

  // Est-ce une plage
  // si context = 1, on verifie une expression de tableau, et on verifie
  // RTYPETAB 1-4
  virtual int syntax_check_is_a_range(T_list_ident **list_ident,
											   T_B0_context context = B0CTX_DEFAULT);

  // Dans le cas d'un tableau (RTYPETAB 1-4)
  // On verifie si l'identificateur n'est pas une variable concrete,
  // un parametre formel, ou un parametre d'E/S d'operation
  void check_allowed_for_array_index(void);

  // Est-ce l'expression "STRING"
  virtual int check_is_STRING(void);

  // Est-ce l'expression "NATURAL"
  virtual int check_is_NATURAL(void);

  // Est-ce l'expression "BOOL"
  virtual int check_is_BOOL(void);

  // Est-ce l'expression "FLOAT"
  virtual int check_is_FLOAT(void);

  // Test si l'identificateur figure dans la liste des identificateurs a
  // traiter
  // (liste globale current_list_ident) et si il n'est pas encore traite (ie
  // value ou initialise suivant la valeur du flag global check)
  // Dans ce cas il y a une erreur
  void check_cycle(T_list_ident **list_ident);

  // Fonctions de compatibilite des tableaux pour les expressions
  // Parsent les bornes des ensembles indices
  virtual int is_array_compatible_with(T_item *ref_item);

  // Parcours de tous les get_def jusqu'a avoir la  definition, sans s'occuper
  // du collage eventuelle
  T_ident *B0_get_definition(void);
#endif //B0C

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_ident" ; }  ;
  T_symbol *get_name(void)			{ return name ; } ;
  T_symbol *get_base_name(void)
	{ return (prefix == NULL) ? name : prefix ; } ;
  int has_suffix_number(void) const
	{ return prefix != NULL ; } ;
  size_t get_suffix_number(void) const
	{ ASSERT(prefix != NULL) ; return suffix_number ; } ;
  T_ident_type get_ident_type(void)	{ return ident_type ; } ;
  T_machine *get_ref_machine(void) ;
  T_op *get_ref_op(void)				{ return ref_op ; } ;
  T_ident *get_ref_glued(void)		{ return ref_glued ; } ;
  T_ident **get_adr_ref_glued(void)		{ return &ref_glued ; } ;
  T_expr *get_values(void)			{ return first_value ; } ;
  T_ident *get_ref_glued_ring(void)		   { return ref_glued_ring ; } ;
  T_ident *get_implemented_by(void)           { return implemented_by ; } ;
  T_ident *get_homonym_in_abstraction(void)   { return homonym_in_abstraction ; } ;
  T_ident *get_homonym_in_required_mach(void) { return homonym_in_required_mach ; } ;
  int get_explicitly_declared(void)           { return explicitly_declared ; } ;
  int is_in_glued_ring(T_ident *ident2) ;
#ifdef __BCOMP__
  T_expr **get_adr_first_value(void)	{ return &first_value ; } ;
  T_expr **get_adr_last_value(void)	{ return &last_value ; } ;
  T_expr *get_first_value(void) { return first_value ; } ;
  T_expr *get_last_value(void) { return last_value ; } ;
#endif

  T_ident *get_def(void)				{ return def ; } ;
  T_ident *get_next_unsolved_ident(void) ;
  T_ident *get_next_ident(void) { return next_ident ; } ;
  T_ident *get_prev_ident(void) { return prev_ident ; } ;
  T_lexem *get_typing_location(void) ;
  T_lexem *get_original_typing_location(void) ;
  virtual T_type *get_B_type(void) ;
  virtual const char *get_def_type_name(void) ;
  T_B0_type *get_B0_type(void) ;

  T_item *get_typing_pred_or_subst(void) ;

  virtual T_type *get_original_B_type(void) ;
  T_ident *get_ref_inherited(void) { return ref_inherited ; } ;

  int is_a_concrete_ident(void) const;

  // Methodes d'ecriture
  virtual void set_suffix_number(size_t new_suffix_number) ;
  virtual void unset_suffix_number(void) ;
  void set_ref_op(T_op *new_ref_op)	{ ref_op = new_ref_op ; } ;
  void set_ref_glued(T_ident *new_ref_glued) ;
  void set_ref_glued_ring(T_ident *new_ref_glued_ring)
	{ ref_glued_ring = new_ref_glued_ring ; } ;
  void set_implemented_by(T_ident *new_implemented_by)
	{ implemented_by = new_implemented_by ; } ;
  void set_homonym_in_abstraction (T_ident *homonym)
	{ homonym_in_abstraction = homonym ; } ;
  void set_homonym_in_required_mach (T_ident *homonym)
	{
	  TRACE1("affectation de homonym_in_required_mach pour l'ident: %s",name->get_value());
	  homonym_in_required_mach = homonym ; } ;
  void set_explicitly_declared(int explicitly)
	{ explicitly_declared = explicitly ; } ;
  void insert_in_glued_ring(T_ident *new_ref_glued) ;

  void set_def(T_ident *new_def)
	{
	  TRACE2("T_ident::set_def(%x, %x)", this, new_def);
	  ASSERT(def != this) ;
	  def = new_def ;
	  insert_in_glued_ring(new_def);
	} ;
  void set_ident_type(T_ident_type new_ident_type)
	{ ident_type = new_ident_type ; } ;
  void set_ref_machine(T_machine *new_ref_machine)
	{ ref_machine = new_ref_machine ; } ;
  void set_next_unsolved_ident(T_ident *new_next_unsolved_ident)
	{ next_unsolved_ident = new_next_unsolved_ident ; } ;
  void set_next_ident(T_ident *new_next_ident)
	{ next_ident = new_next_ident ; } ;
  void set_prev_ident(T_ident *new_prev_ident)
	{ prev_ident = new_prev_ident ; } ;
  virtual void set_B_type(T_type *new_B_type, T_lexem *localisation) ;
  void set_B0_type(T_B0_type *new_B0_type) ;


  void set_typing_pred_or_subst(T_item *new_pred)  ;


  void set_ref_inherited(T_ident *new_ref_inherited) ;

  // Obtention du nom correspondant au type d'identificateur
  const char *get_ident_type_name(void) ;

  // Recherched de la definition de l'identificateur
  void solve_ident(void) ;

#ifdef __BCOMP__
  // Reset du type B
  void reset_B_type(void) ;
#endif

  // Parcours de tous les get_def jusqu'a avoir la definition
  T_ident *get_definition(void) ;

  // Fonction de check pour les déclarations d'un identificateur
  void B0_check_decl(void);

  // Fonction de controle de type des expressions
  virtual T_ident *B0_check_expr(T_ident *expected_type,
										  T_lexem *err_pos,
										  T_lexem *invalid_err_pos,
										  T_B0_type_context ctx);

  // test de référencement: retourne la première reference à la
  // variable ident, NULL s'il elle n'est pas référencée
  virtual T_ident *B0_OM_refer_to (T_ident *ident);

  // fonction de controle des expressions statiques
  virtual int is_static(void);

  // Corrections et verifications supplementaires
  void extra_checks(void) ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Recherche descendante d'un ident
  T_ident *lookup_ident(T_ident *name);

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

  // Rattrapages eventuels sur le clonage : rien dans le cas general
  // Redefini dans la classe T_ident (maj liste unsolved_ident)
  virtual void fix_clone(T_object *ref_object) ;

  //GP 22/12/98
  //Renvoie TRUE quand l'item est autorise dans la branche d'un case
  virtual int is_expected_in_case_branch(void)
	{ return (def == NULL) ? FALSE : def->is_expected_in_case_branch() ;} ;

  //GP 12/01/99
  T_access_entity_location get_access_entity_location(void)
	{return access_entity_location ;}

  void set_access_entity_location(T_access_entity_location new_acces)
	{ access_entity_location = new_acces ; }

  T_symbol *get_prefix(int delete_dot,
								T_used_machine *ref_context) ;
  T_symbol *get_full_name(int delete_dot,
								   T_used_machine *ref_context = NULL);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

} ;

//
//	}{	Classe T_renamed_ident
//
class T_renamed_ident : public T_ident
{
  // Nom d'instance
  T_symbol		*instance_name ;

  // Nom de composant
  T_symbol		*component_name ;



public :
  T_renamed_ident() : T_ident(NODE_RENAMED_IDENT) {} ;
  T_renamed_ident(const char *new_name,
				  T_ident_type new_ident_type,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *new_father,
				  T_betree *new_betree,
				  T_lexem *new_ref_lexem) ;
  // Constructeur de clonage
  T_renamed_ident(T_ident *ref_ident,
				  T_symbol *prefix,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *new_father) ;
  // Destructeur
  virtual ~T_renamed_ident(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_renamed_ident" ; } ;
  T_symbol *get_instance_name(void)	{ return instance_name ; } ;
  T_symbol *get_component_name(void)	{ return component_name ; } ;

  // GP 16/11/98 pour EREN 1 & 2
  //Verifi que l'ident n'est pas importee d'une instance renommée
  //def_ident: ident de definition de l'ident actuel
  virtual T_ident* check_rename(T_ident * def_ident) ;

  // Methodes d'ecriture
  virtual void set_suffix_number(size_t new_suffix_number) ;
  virtual void unset_suffix_number(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

//
//	}{	Classe T_literal_enumerated_value
//
// Compatibilite versions precedentes
#define T_litteral_enumerated_value T_literal_enumerated_value
class T_literal_enumerated_value : public T_ident
{
  // Valuation
  size_t	   			  		value ;

public :
  T_literal_enumerated_value() : T_ident(NODE_LITERAL_ENUMERATED_VALUE) {} ;
  T_literal_enumerated_value(const char *new_name,
							  T_item **adr_first,
							  T_item **adr_last,
							  T_item *new_father,
							  T_betree *new_betree,
							  T_lexem *new_ref_lexem) ;
  // Constructeur de clonage
  T_literal_enumerated_value(T_literal_enumerated_value *ref_evalue,
							  T_item **adr_first,
							  T_item **adr_last,
							  T_item *new_father) ;

  virtual ~T_literal_enumerated_value(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_literal_enumerated_value" ; }  ;
  size_t get_value(void)					{ return value ; } ;

  //GP 22/12/98
  //Renvoie TRUE quand l'item est autorise dans la branche d'un case
  virtual int is_expected_in_case_branch(void) {return TRUE ;};

  //GP 28/12/98
  //retourne TRUE si case_barnch est du type T_LITERAL_ENUMERATED_VALUE
  //et que case_barnch->value = this->value
  virtual int is_same_value(T_item* case_branch) ;



  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Verifie qu'une liste d'identificateurs ne comporte pas de doublons.
// Emet un message d'erreur le cas echeant
extern void check_no_double_definition(T_ident *list_ident) ;

// Verifie qu'une liste d'items ne comporte pas deux fois le meme
// identficateur.
// Emet un message d'erreur le cas echeant
extern void check_no_double_ident(T_item *list_item);

// Pour dire si l'on est dans la clause VALUES
extern void set_inside_values(int new_inside_values) ;

// Fonctions de haut niveau de verification des $0
extern void syntaxic_check_dollar_zero(T_betree *betree) ;
extern void semantic_check_dollar_zero(T_betree *betree) ;

// Construit un nouveau symbole en ajoutant un suffixe numérique.
extern T_symbol *add_suffix_to_name(T_symbol *prefix,
											 size_t suffix_number) ;

#endif /* _C_IDENT_H_ */

//
//	}{	Fin du fichier
//

