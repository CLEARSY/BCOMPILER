/******************************* CLEARSY **************************************
* Fichier : $Id: c_mach.h,v 2.0 2007-06-25 16:48:32 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des composants, machines, implementations
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
#ifndef _C_MACH_H_
#define _C_MACH_H_

#include "c_item.h"

class T_used_machine ;
class T_used_op ;
class T_predicate ;
class T_substitution ;
class T_valuation ;
class T_ident ;
class T_renamed_ident ; //GP 20/11/98 pour defines(T_renamed_ident*)
class T_flag ;
class T_op ;
class T_list_link ;
class T_pragma ;
class T_generic_op ;

#ifdef B0C
class T_list_ident;
#endif // B0C
//
//	}{	Type de machine
//
typedef enum
{
  // L'ordre et les valeurs sont garantis -- ils ne changeront jamais
  MTYPE_SPECIFICATION = 0,
  MTYPE_REFINEMENT = 1,
  MTYPE_IMPLEMENTATION = 2,
  MTYPE_SYSTEM = 3
} T_machine_type ;

class T_machine : public T_item
{
  //
  // Les fonctions de parsing sont amies
  //
  friend void parse_PARAMS(T_machine *, T_lexem **) ;
  friend void parse_OPERATIONS(T_machine *, T_lexem **) ;
  friend void parse_LOCAL_OPERATIONS(T_machine *, T_lexem **) ;
  friend void parse_CONSTRAINTS(T_machine *, T_lexem **) ;
  friend void parse_SEES(T_machine *, T_lexem **) ;
  friend void parse_INCLUDES(T_machine *, T_lexem **) ;
  friend void parse_PROMOTES(T_machine *, T_lexem **) ;
  friend void parse_EXTENDS(T_machine *, T_lexem **) ;
  friend void parse_USES(T_machine *, T_lexem **) ;
  friend void parse_SETS(T_machine *, T_lexem **) ;
  friend void parse_CONCRETE_CONSTANTS(T_machine *, T_lexem **) ;
  friend void parse_ABSTRACT_CONSTANTS(T_machine *, T_lexem **) ;
  friend void parse_PROPERTIES(T_machine *, T_lexem **) ;
  friend void parse_CONCRETE_VARIABLES(T_machine *, T_lexem **) ;
  friend void parse_ABSTRACT_VARIABLES(T_machine *, T_lexem **) ;
  friend void parse_INVARIANT(T_machine *, T_lexem **) ;
  friend void parse_VARIANT(T_machine *, T_lexem **) ;
  friend void parse_ASSERTIONS(T_machine *, T_lexem **) ;
  friend void parse_INITIALISATION(T_machine *, T_lexem **) ;
  friend void parse_VALUES(T_machine *, T_lexem **) ;
  friend void parse_REFINES(T_machine *, T_lexem **) ;
  friend void parse_IMPORTS(T_machine *, T_lexem **) ;
  friend T_betree *create_betree_refinement(T_betree *abstraction,
                                                     const char* converterName,
													 const char *new_name,
													 int create_imp,
													 int do_seman,
														char *svdi,
														int nb_operations,
														char **operations) ;
  friend void insert_blank_init(T_betree *betree) ;

  //
  // Structure de donnees

  //
  // Type de composant
  T_machine_type			machine_type ;

  // Nom de la machine
  T_ident   				*machine_name ;

  // Nom de la machine raffinee
  T_ident					*abstraction_name ;

  // Machine raffinee (SEMAN)
  T_machine					*ref_abstraction ;

  // Specification (SEMAN)
  T_machine					*ref_specification ;

  // Raffinement suivant
  T_machine					*ref_refinement ;

  // Localisation de la clause qui decrit la machine
  T_flag					*machine_clause ;

  // Localisation de la clause de raffinement
  T_flag					*refines_clause ;

  // Parametres formels
  T_ident					*first_param ;
  T_ident					*last_param ;
  T_symbol					*formal_params_checksum ;

  // Pragmas (liste globale)
  T_pragma		  			*first_global_pragma ;
  T_pragma		  			*last_global_pragma ;

  // Clause CONSTRAINTS
  T_flag					*constraints_clause ;
  T_symbol					*constraints_clause_checksum ;
  T_predicate				*constraints ;

  // Clause SEES
  T_flag					*sees_clause ;
  T_symbol					*sees_clause_checksum ;
  T_used_machine			*first_sees ;
  T_used_machine			*last_sees ;

  // Clause INCLUDES
  T_flag					*includes_clause ;
  T_symbol					*includes_clause_checksum ;
  T_used_machine			*first_includes ;
  T_used_machine			*last_includes ;

  // Clause IMPORTS
  T_flag					*imports_clause ;
  T_symbol					*imports_clause_checksum ;
  T_used_machine			*first_imports ;
  T_used_machine			*last_imports ;

  // Clause PROMOTES
  T_flag					*promotes_clause ;
  T_symbol					*promotes_clause_checksum ;
  T_used_op				*first_promotes ;
  T_used_op				*last_promotes ;

  // Clause EXTENDS
  T_flag					*extends_clause ;
  T_symbol					*extends_clause_checksum ;
  T_used_machine			*first_extends ;
  T_used_machine			*last_extends ;

  // Clause USES
  T_flag					*uses_clause ;
  T_symbol					*uses_clause_checksum ;
  T_used_machine			*first_uses ;
  T_used_machine			*last_uses ;

  // Clause SETS
  T_flag					*sets_clause ;
  T_symbol					*sets_clause_checksum ;
  T_ident					*first_set ;
  T_ident					*last_set ;

  // Clause CONCRETE_VARIABLES
  T_flag					*concrete_variables_clause ;
  T_symbol					*concrete_variables_clause_checksum ;
  T_ident					*first_concrete_variable ;
  T_ident					*last_concrete_variable ;

  // Clause ABSTRACT_VARIABLES
  T_flag					*abstract_variables_clause ;
  T_symbol					*abstract_variables_clause_checksum ;
  T_ident					*first_abstract_variable ;
  T_ident					*last_abstract_variable ;

  // Clause CONCRETE_CONSTANTS
  T_flag					*concrete_constants_clause ;
  T_symbol					*concrete_constants_clause_checksum ;
  T_ident					*first_concrete_constant ;
  T_ident					*last_concrete_constant ;

  // Clause ABSTRACT_CONSTANTS
  T_flag					*abstract_constants_clause ;
  T_symbol					*abstract_constants_clause_checksum ;
  T_ident					*first_abstract_constant ;
  T_ident					*last_abstract_constant ;

  // Clause PROPERTIES
  T_flag					*properties_clause ;
  T_symbol					*properties_clause_checksum ;
  T_predicate				*properties ;

  // Clause INVARIANT
  T_flag					*invariant_clause ;
  T_symbol					*invariant_clause_checksum ;
  T_predicate				*invariant ;

  // Clause VARIANT
  T_flag					*variant_clause ;
  T_symbol					*variant_clause_checksum ;
  T_expr                    *variant ;

  // Clause ASSERTIONS
  T_flag					*assertions_clause ;
  T_symbol					*assertions_clause_checksum ;
  T_predicate				*first_assertion ;
  T_predicate				*last_assertion ;

  // Clause INITIALISATION
  // pas de checksum car stocke dans l'operation associe
  T_flag					*initialisation_clause ;
  T_op						*initialisation_clause_operation ;
  T_ident					*initialisation_clause_ident ;
  T_substitution			*first_initialisation ;
  T_substitution			*last_initialisation ;

  // Clause VALUES
  T_flag					*values_clause ;
  T_symbol					*values_clause_checksum ;
  T_valuation				*first_value ;
  T_valuation				*last_value ;

  // Clause OPERATIONS
  T_flag					*operations_clause ;
  T_op						*first_operation ;
  T_op   					*last_operation ;

  // Clause LOCAL_OPERATIONS
  T_flag					*local_operations_clause ;
  T_op						*first_local_operation ;
  T_op   					*last_local_operation ;

  // Clause END
  T_flag					*end_machine ;

  // Commentaires qui suivent la clause end
  T_comment				*first_end_comment ;
  T_comment				*last_end_comment ;

  // Chainage de toutes les operations (promues et propres)
  // DISPONIBLE APRES L'ANALYSE SEMANTIQUE
  T_list_link		   	*first_op_list ;
  T_list_link		  	*last_op_list ;

  // Chainage de toutes les identificateurs avec des $0
  // DISPONIBLE APRES L'ANALYSE SYNTAXIQUE
  T_list_link		   	*first_dollar_zero_ident ;
  T_list_link		  	*last_dollar_zero_ident ;

  // Dans le cas d'une machine requise par une autre machine,
  // reference sur le T_used_machine "de provenance"
  // DISPONIBLE APRES L'ANALYSE SEMANTIQUE
  T_used_machine		*context ;

  // Pour savoir si la machine a deja ete type-checkee
  int 					type_checked ;

  // Pour savoir si la machine a subi l'analyse de projet
  int                   project_checked ;

  // Pour savoir si les op�rations ont d�j� �t� analys�es
  int                   op_analysed ;

  //
  //	Methodes privees
  //
  // Fonction auxiliaire de recherche d'un identificateur dans une liste
  T_ident *browse_list(T_ident *ref_ident,
								T_machine **adr_ref_machine,
								T_used_machine *cur) ;

  // Pour savoir si une machine definit un identificateur
  // Recherche : dans les constantes, les variables et les SETS
  T_ident	*defines(T_ident *def_ident) ;

  // GP 20/11/98
  // Pour savoir si une machine definit un identificateur
  // Avec la contrainte supplementaire: la T_used_machine
  // associee (context), doit avoir le meme prefixe que def_renamed_ident.
  T_ident	*defines(T_renamed_ident *def_ident_renamed) ;


  // Recherche d'un identificateur dans les machines
  // importees, includes, etendues et eventuellement vues
  // adr_ref_mach est mis a jour pour pointer sur la machine qui definit
  T_ident *lookup_ident_in_needed_components(T_ident *ref_ident,
													  T_machine **adr_ref_mach) ;

  // Fonction auxiliaire de recherche d'ident dans une liste
  // par son nom
  T_ident *find_in_list(T_symbol *name, T_ident *list) ;

  // Calcul du checksum de start_lexem a end_lexem
  // Les deux lexemes sont exclus
  T_symbol *internal_compute_checksum(T_item *start_item, bool include_first) ;
public :
  // Constructeurs
  T_machine() : T_item (NODE_MACHINE) {} ;
#ifdef __BCOMP__
  T_machine(T_machine_type new_machine_type,
			const char *new_file_name,
			T_betree *new_betree,
			T_lexem **adr_ref_lexem,
			int do_parse = TRUE) ; // Si FALSE, on ne parse pas
#endif

  // Destructeur
  virtual ~T_machine(void) ;

#ifdef __BCOMP__
  // Methode d'analyse syntaxique
  T_lexem *syntax_analysis(T_lexem *cur_lexem) ;

  // Calcul du checksum expanse
  void compute_checksums(void) ;
#endif

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction qui permet de savoir si c'est une machine qui d�finie
  // des types de base
  int is_basic_translator_machine(void);

  // Verification des instanciations des machines requises
  // Effet de bord : utilise le champ tmp2
  // clock est le timestamp de reference pour savoir si le travail a ete fait
  // si do_complain == FALSE on ne reporte pas d'erreur si un parametre n'est
  // pas encore type
  void check_required_machines_instanciation(void) ;

  // Verification de l'instanciation d'une machine requise
  // clock est le timestamp de reference pour savoir si le travail a ete fait
  // si do_complain == FALSE on ne reporte pas d'erreur si un parametre n'est
  // pas encore type
  void check_single_machine_instanciation(T_used_machine *umach) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_machine" ; }  ;

  T_machine_type get_machine_type(void)	{ return machine_type ; } ;
  T_ident *get_machine_name(void)		{ return machine_name ; } ;
  T_ident *get_abstraction_name(void) 	{ return abstraction_name ; } ;
  T_machine *get_ref_abstraction(void)  	{ return ref_abstraction ; } ;
  T_machine *get_ref_specification(void) { return ref_specification ; };
  T_machine *get_ref_refinement(void) { return ref_refinement ; };
  T_flag *get_machine_clause(void)    	{ return machine_clause ; } ;
  T_flag *get_refines_clause(void)    	{ return refines_clause ; } ;
  T_ident *get_params(void)		   	{ return first_param ; } ;
  T_pragma *get_global_pragmas(void)  	{ return first_global_pragma ; } ;
  T_predicate *get_constraints(void)  	{ return constraints ; } ;
  T_used_machine *get_sees(void) 	   	{ return first_sees ; } ;
  T_used_machine *get_includes(void) 	{ return first_includes ; } ;
  T_used_machine *get_imports(void)   	{ return first_imports ; } ;
  T_used_op *get_promotes(void) 	   	{ return first_promotes ; } ;
  T_used_machine *get_extends(void)   	{ return first_extends ; } ;
  T_used_machine *get_uses(void) 		{ return first_uses ; } ;
  T_ident *get_sets(void) 				{ return first_set ; };
  T_ident *get_concrete_variables(void) { return first_concrete_variable ;} ;
  T_ident *get_abstract_variables(void) { return first_abstract_variable ; };
  T_ident *get_concrete_constants(void) { return first_concrete_constant ; } ;
  T_ident *get_abstract_constants(void) { return first_abstract_constant ; };
  T_predicate *get_properties(void) { return properties ; } ;

  T_predicate *get_invariant(void) 	   	{ return invariant ; } ;
  T_expr *get_variant(void) 	   	{ return variant ; } ;
  T_predicate *get_assertions(void) 		{ return first_assertion ; } ;
  T_substitution *get_initialisation(void) { return first_initialisation ; } ;
  T_substitution *get_last_initialisation(void)
	{ return last_initialisation ; };
T_valuation *get_values(void) 			{ return first_value ; };
T_op *get_operations(void) 			{ return first_operation ; } ;
T_op *get_local_operations(void) 		{ return first_local_operation ; } ;

T_flag *get_constraints_clause(void) 	{ return constraints_clause ; };
T_flag *get_sees_clause(void) 			{ return sees_clause ; } ;
T_flag *get_includes_clause(void) 		{ return includes_clause ; } ;
T_flag *get_imports_clause(void) 		{ return imports_clause ; } ;
T_flag *get_promotes_clause(void) 		{ return promotes_clause ; } ;
T_flag *get_extends_clause(void) 		{ return extends_clause ; } ;
T_flag *get_uses_clause(void) 			{ return uses_clause ; } ;
T_flag *get_sets_clause(void) 			{ return sets_clause ; } ;
T_flag *get_concrete_variables_clause(void)
{ return concrete_variables_clause ; } ;
T_flag *get_abstract_variables_clause(void)
{ return abstract_variables_clause ; } ;
T_flag *get_concrete_constants_clause(void)
{ return concrete_constants_clause ; } ;
T_flag *get_abstract_constants_clause(void)
{ return abstract_constants_clause ; } ;
T_flag *get_properties_clause(void)	{ return properties_clause ; } ;
T_flag *get_invariant_clause(void)		{ return invariant_clause ; } ;
T_flag *get_variant_clause(void)		{ return variant_clause ; } ;
T_flag *get_assertions_clause(void)	{ return assertions_clause ; } ;
T_flag *get_initialisation_clause(void)
{ return initialisation_clause ; } ;
T_ident *get_initialisation_clause_ident(void)
{ return initialisation_clause_ident ; } ;
T_op *get_initialisation_clause_operation(void)
{ return initialisation_clause_operation ; } ;
T_flag *get_values_clause(void)		{ return values_clause ; } ;
T_flag *get_operations_clause(void)	{ return operations_clause ; } ;
T_flag *get_local_operations_clause(void)	{ return local_operations_clause ; } ;
T_flag *get_end_machine(void) 			{ return end_machine ; } ;
T_comment *get_end_comments(void) 		{ return first_end_comment ; } ;
T_list_link *get_op_list(void)			{ return first_op_list ; } ;
T_list_link *get_dollar_zero_identifiers(void)
{ return first_dollar_zero_ident ; } ;
T_used_machine *get_context(void) { return context ; } ;
#ifdef __BCOMP__
T_pragma **get_adr_first_global_pragma(void) { return &first_global_pragma ; } ;
T_pragma **get_adr_last_global_pragma(void) { return &last_global_pragma ; } ;
T_list_link **get_adr_first_op_list(void) { return &first_op_list ; } ;
T_list_link **get_adr_last_op_list(void) { return &last_op_list ; } ;
#endif // __BCOMP__

T_symbol *get_formal_params_checksum(void)
{ return formal_params_checksum ; };
T_symbol *get_constraints_clause_checksum(void)
{ return constraints_clause_checksum ; };
T_symbol *get_sees_clause_checksum(void)
{ return sees_clause_checksum ; } ;
T_symbol *get_includes_clause_checksum(void)
{ return includes_clause_checksum ; } ;
T_symbol *get_imports_clause_checksum(void)
{ return imports_clause_checksum ; } ;
T_symbol *get_promotes_clause_checksum(void)
{ return promotes_clause_checksum ; } ;
T_symbol *get_extends_clause_checksum(void)
{ return extends_clause_checksum ; } ;
T_symbol *get_uses_clause_checksum(void)
{ return uses_clause_checksum ; } ;
T_symbol *get_sets_clause_checksum(void)
{ return sets_clause_checksum ; } ;
T_symbol *get_concrete_variables_clause_checksum(void)

{ return concrete_variables_clause_checksum ; } ;
T_symbol *get_abstract_variables_clause_checksum(void)

{ return abstract_variables_clause_checksum ; } ;
T_symbol *get_concrete_constants_clause_checksum(void)

{ return concrete_constants_clause_checksum ; } ;
T_symbol *get_abstract_constants_clause_checksum(void)

{ return abstract_constants_clause_checksum ; } ;
T_symbol *get_properties_clause_checksum(void)
{ return properties_clause_checksum ; } ;
T_symbol *get_invariant_clause_checksum(void)
{ return invariant_clause_checksum ; } ;
T_symbol *get_variant_clause_checksum(void)
{ return variant_clause_checksum ; } ;
T_symbol *get_assertions_clause_checksum(void)
{ return assertions_clause_checksum ; } ;
T_symbol *get_values_clause_checksum(void)
{ return values_clause_checksum ; } ;


#if (defined (__BCOMP__) || defined (__INTERFACE__) )
T_comment *get_first_end_comment(void) { return first_end_comment ; } ;
T_comment *get_last_end_comment(void) { return last_end_comment ; } ;
T_comment **get_adr_first_end_comment(void) { return &first_end_comment ; } ;
T_comment **get_adr_last_end_comment(void) { return &last_end_comment ; } ;
void set_first_end_comment(T_comment *new_first_end_comment)
{ first_end_comment = new_first_end_comment ; } ;
void set_last_end_comment(T_comment *new_last_end_comment)
{ last_end_comment = new_last_end_comment ; } ;
T_ident **get_adr_first_set(void) { return &first_set ; } ;
T_ident **get_adr_last_set(void) { return &last_set ; } ;
#endif

int get_project_checked(void) { return project_checked ; } ;
int get_op_analysed(void) { return op_analysed ; } ;


// Nom de la spec (information semantique)
T_symbol *get_spec_machine_symbol_name(void) ;
const char *get_spec_machine_name(void) ;
int get_spec_machine_name_len(void) ;

// Acces a la spec (== this pour une machine)
T_machine *get_specification(void)
{ return (ref_specification == NULL)
	? this : ref_specification->get_specification() ; } ;

// Methodes d'ecriture
void remove_initialisation_clause(void)
{
  initialisation_clause  = NULL;
  initialisation_clause_operation  = NULL;
  initialisation_clause_ident  = NULL;
  first_initialisation  = NULL;
  last_initialisation  = NULL;
}
void remove_sets_clause(void){sets_clause = NULL;}
void set_machine_type(T_machine_type new_machine_type)
{ machine_type = new_machine_type ; } ;
void set_ref_abstraction(T_machine *new_ref_abstraction)
{ ref_abstraction = new_ref_abstraction ; } ;
void set_ref_specification(T_machine *new_ref_specification)
{ ref_specification = new_ref_specification ; } ;
void set_ref_refinement(T_machine *new_ref_refinement)
{ ref_refinement = new_ref_refinement ; } ;
void set_op_list(T_list_link *new_first_op_list, T_list_link *new_last_op_list)
{
  first_op_list = new_first_op_list ;
  last_op_list = new_last_op_list ;
} ;
void set_context(T_used_machine *new_context)
{ context = new_context ; } ;
void set_machine_name(T_ident *new_machine_name)
{ machine_name = new_machine_name ; } ;
void set_project_checked(int new_project_checked)
{ project_checked = new_project_checked ; } ;
void set_op_analysed(int new_op_analysed)
{ op_analysed = new_op_analysed ; } ;

// Ajout d'un identificateur avec $0
void add_dollar_zero_ident(T_ident *ident) ;

// Pour savoir si une machine est vue, importee, ...
// rend TRUE si oui, FALSE sinon
int get_sees(T_machine *mach) ;
int get_includes(T_machine *mach) ;
int get_extends(T_machine *mach) ;
int get_imports(T_machine *mach) ;
int get_uses(T_machine *mach) ;

// Obtention du parametre formel scalaire apres cur_param
// Rend le premier de la liste si cur_param = NULL
T_ident *get_scalar_formal_parameter_after(T_ident *cur_param) ;

// Obtention du parametre formel ensembliste apres cur_param
// Rend le premier de la liste si cur_param = NULL
T_ident *get_set_formal_parameter_after(T_ident *cur_param) ;

// Pour savoir si this raffine spec
// rend TRUE si oui, FALSE sinon
int refines(T_machine *spec) ;

// Recuperation de l'environnement de l'abstraction
void import_abstract_environment(T_lexem *ref_lexem) ;

// Recuperation de l'environnement d'une machine incluse
void import_included_environment(T_machine *included_machine,
										  T_lexem *ref_lexem,
										  T_list_link** first_already_include,
										  T_list_link** last_already_include) ;

// Detection des collages
void internal_lookup_glued_data(T_ident *list_ident,
										 T_item **adr_first_conc,
										 T_item **adr_last_conc,
										 T_item **adr_first_abs,
										 T_item **adr_last_abs) ;

void lookup_glued_data(void) ;

#ifdef B0C

// Verification B0 des operations
virtual void operation_B0_check(void);

// Controles sur les valuations des constantes concretes et des ensembles
// abstraits.
virtual void valuation_check(T_list_ident **list_ident);

// Controles sur les instanciations de parametres formels de machines
virtual void instanciation_check(void);

// Controle des cycles dans les initialisations
virtual void initialisation_check(T_list_ident **list_ident) ;

#endif //B0C

// Est-ce une formule typante ?? oui !
virtual int is_a_typing_formula(void) ;

// Fonctions utilisateur
virtual int user_defined_f1(void) ;
virtual T_object *user_defined_f2(void) ;
virtual int user_defined_f3(int arg) ;
virtual T_object *user_defined_f4(T_object *arg) ;
virtual int user_defined_f5(T_object *arg) ;

// Recherche d'une operation dans la liste des operations par son nom
T_generic_op *find_op(T_symbol *op_name) ;

// Recherche d'une operation locale dans la liste des operations par son nom
T_op *find_local_op(T_symbol *op_name) ;

// Recherche d'un ensemble dans la liste par son nom
T_ident *find_set(T_symbol *set_name) ;

// Recherche d'une constante abstraite/concrete dans la liste par son nom
T_ident *find_abstract_constant(T_symbol *const_name) ;
T_ident *find_concrete_constant(T_symbol *const_name) ;

  // Recherche d'une variable abstraite/concrete dans la liste par son nom
T_ident *find_abstract_variable(T_symbol *var_name) ;
T_ident *find_concrete_variable(T_symbol *var_name) ;

  // Recherche d'une variable/constante par son nom, independament
  // de sa nature (concrete/abstraite)
T_ident *find_constant(T_symbol *const_name) ;
T_ident *find_variable(T_symbol *var_name) ;


// Verifie qu'un ident n'est pas une donnee de la machine
// Si c'est le cas, emet un message d'erreur et renvoie FALSE
// Sinon renvoie TRUE
// Doit etre appele avec une constante ou une variable
// prefix est le prefixe de renommage a prendre en compte le cas echeant
int check_no_collision(T_ident *ident, T_symbol *prefix = NULL);

// Recherche d'une operation dans la liste des operations par son nom,
// avec mise a jour d'une reference sur le T_list_link correspondant
T_generic_op *find_op(T_symbol *op_name, T_list_link **adr_ref_link) ;

// Recherche recursive d'un identificateur
virtual T_ident *find_ident(T_ident *name, int root_machine) ;

// Recherche d'un pragma dans la liste globale � partir du pragma
// after_pragma exclu
T_pragma *lookup_pragma(T_symbol *pragma_name,
				 T_pragma *after_pragma);

// Rend TRUE si ref_machine est requise (au premier niveau) par this
int needs(T_machine *ref_machine) ;

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void make_standalone(void) ;
} ;

// Nom du type de la machine
extern const char *const get_machine_type_name(T_machine_type machine_type) ;

// Suffixe de fichiers assoicie au type de la machine
extern const char *const get_machine_suffix_name(T_machine_type machine_type) ;

// Obtention de la machine en cours de construction
extern T_machine *get_cur_machine(void) ;

// Recherche d'une op��ration promue dans une liste de machines requises.
extern T_generic_op* find_op_in_list(T_used_machine *first_umach,
					      T_used_op *operation);

#endif /* _C_MACH_H_ */

