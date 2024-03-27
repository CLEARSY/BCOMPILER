/******************************* CLEARSY **************************************
* Fichier : $Id: c_env.h,v 2.0 2002-07-17 10:07:50 cm Exp $
* (C) 2008 CLEARSY
*
* Description :			Parametrage du compilateur B par des variables
*						d'environnement
*
* Compilation :  -DDEBUG_PARAM pour tracer les tests
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
#ifndef _C_ENV_H_
#define _C_ENV_H_

// (Ré)initialisation de la gestion du paramétrage
extern void init_env(void) ;

// Largeur des tabulations : variable d'environnement "BC_TAB_WIDTH"
// Valeur par defaut : 8
extern size_t get_tab_width(void) ;

// SEES entendu : variable BC_EXTENDED_SEES
// Valeur par defaut : FALSE
extern int get_extended_sees(void) ;

// Autorisation du xx en implementation : variable BC_xx
// Valeur par defaut : FALSE
extern int get_B0_allow_parallel(void) ;
extern int get_B0_allow_pre(void) ;
extern int get_B0_allow_choice(void) ;
extern int get_B0_allow_select(void) ;
extern int get_B0_allow_any(void) ;
extern int get_B0_allow_let(void) ;
extern int get_B0_allow_becomes_member_of(void) ;
extern int get_B0_disable_predicate_syntax_check(void) ;
extern int get_B0_disable_expression_syntax_check(void) ;
extern int get_B0_disable_formal_params_type_check(void) ;
extern int get_B0_disable_concrete_constants_type_check(void) ;
extern int get_B0_disable_concrete_variables_type_check(void) ;
extern int get_B0_disable_operation_input_parameters_type_check(void);
extern int get_B0_disable_operation_output_parameters_type_check(void);
extern int get_B0_disable_locale_variables_type_check(void) ;

extern int get_B0_disable_variables_initialisation_check(void) ;
extern int get_B0_disable_array_compatibility_check(void) ;
extern int get_B0_disable_parameter_instanciation_check(void) ;
extern int get_B0_disable_valuation_check(void) ;
extern int get_B0_generate_eventb_non_divergence_po(void) ;

// Autorisation de allow_becomes_member_of
// Option positionnée par le flattener
extern void set_B0_allow_becomes_member_of(int new_val);

// B0 Checker adapte a la traduction HIA, i.e. acceptant le typage
// par identificateur
// A DEMANDER EXPLICITEMENT : ce n'est pas le fonctionnement par defaut
extern void set_B0_enable_typing_identifiers(int new_val) ;
extern int get_B0_enable_typing_identifiers(void) ;

// B0 Checker adapte a la traduction ALSTOM : Accepter les devient_tel
// que en implémentation
// A DEMANDER EXPLICITEMENT : ce n'est pas le fonctionnement par defaut
extern void set_B0_allow_becomes_such_that(int new_val) ;
extern int get_B0_allow_becomes_such_that(void) ;

// B0 Checker, controle supplementaire (ex : traduction HIA) :
// verifier que les instanciations de machines requses, i.e. les
// parametres formels effectifs, ne font pas intervenir de parametres
// formels de la machine
// A DEMANDER EXPLICITEMENT : ce n'est pas le fonctionnement par defaut
extern void set_B0_enable_restricted_formal_param_instanciation(int new_val) ;
extern int get_B0_enable_restricted_formal_param_instanciation(void) ;

// Detection des identificateurs de 1 caractere : erreur ?
// warning par defaut (i.e. is_an_error = FALSE et is_a_warning = TRUE)
extern void set_kernel_joker_is_an_error(int is_an_error) ;
extern int get_kernel_joker_is_an_error(void) ;

// Detection des identificateurs de 1 caractere : warning ?
extern void set_kernel_joker_is_a_warning(int is_a_warning) ;
extern int get_kernel_joker_is_a_warning(void) ;

// Detection des conflits "caches" pour la preuve ?
extern void set_proof_clash_detection(int new_proof_clash_detection) ;
extern int get_proof_clash_detection(void) ;

// Non-vérification que les sélecteurs de case sont des littéraux ?
extern int get_disable_case_selector_check(void) ;

// Mise en place/lecture du mode "typage par identificateur"
// Utilise par les clients de traduction
extern int get_typing_ident_mode(void) ;
extern void set_typing_ident_mode(int new_typing_ident_mode) ;

// Valeur par defaut pour MININT/MAXINT
extern T_symbol *get_minint_value(void) ;
extern T_symbol *get_maxint_value(void) ;

// Pour savoir si l'on a demande une compilation "stricte" B
extern int get_strict_B_demanded(void) ;
// Pour demander une compilation "stricte" B
extern void set_strict_B_demanded(void) ;

// Lecture du path de production des fichiers
extern const char *get_output_path(void) ;
extern size_t get_output_path_len(void) ;

// Mise a jour du path de production des fichiers
extern void set_output_path(const char *new_output_path) ;

// Mode calcul de dependances: set/get
extern void set_dep_mode(int new_dep_mode) ;
extern int get_dep_mode(void) ;

// Mode ou l'on n'effectue pas de copie privees pour les types
// records afin que toutes les expressions records aient tous
// les labels positionnes
// (mode special pour le GNF, incompatible avec le typage par
// identificateur de HIA)
// Par defaut : FALSE
extern int get_unique_record_type_mode(void) ;
extern void set_unique_record_type_mode(int new_mode) ;

// Autorisation des déclarations de variables locales inutiles
// Option utilisée par le traducteur PSC.
// Valeur par défaut : FALSE
extern int get_allow_unused_local_variables(void) ;
extern void set_allow_unused_local_variables(int new_val) ;

// Autorisation des types builtin infinis (INTEGER, NATURAL et NATURAL1)
// Option utilisée par le traducteur PSC.
// Valeur par défaut : FALSE
extern int get_allow_unbounded_builtin_types(void) ;
extern void set_allow_unbounded_builtin_types(int new_val) ;

// Suppression des contrôles B0 sur la valuation d'une constante concrète
// représentant un ensemble de scalaires.
// Option utilisée par le traducteur PSC.
// Valeur par défaut : FALSE
extern int get_disable_scalar_set_constant_valuation_b0_check(void);
extern void set_disable_scalar_set_constant_valuation_b0_check(int new_val);

// Autorisation des tableaux de tableaux en B0
// Option utilisée par le traducteur PSC.
// Valeur par défaut : TRUE
extern int get_B0_allow_arrays_of_arrays(void) ;
extern void set_B0_allow_arrays_of_arrays(int new_val) ;

// Utilisation des déclarations concrètes
// Option urilisée par le flattener
// Valuer par défaut : FALSE
extern int get_Use_B0_Declaration(void);
extern void set_Use_B0_Declaration(int new_val) ;

// Autorisation des préconditions dans des substitutions
// Valeur par défaut : TRUE
extern int get_allow_pre_in_substitution(void) ;
extern void set_allow_pre_in_substitution(int new_val) ;

extern int get_allow_pragma_in_def(void);
extern void set_allow_pragma_in_def(int new_val) ;

//Autorisation de la substitution While partout
extern int get_allow_while_sub_everywhere(void);
extern void set_allow_while_sub_everywhere(int new_val);

//Autorisation de la substitution séquentielle dans la spécification
extern int get_allow_sequential_sub_in_machine(void);
extern void set_allow_sequential_sub_in_machine(int new_val);

//Autorisation de la substitution VAR dans la spécification
extern int get_allow_var_sub_in_machine(void);
extern void set_allow_var_sub_in_machine(int new_val);

//Autorisation des relations totales <<->, surjectives <->>, et surjectives totales <<->>
// Valeur par défaut : FALSE
extern int get_allow_total_and_surjective_relation_operators(void);
extern void set_allow_total_and_surjective_relation_operators(int new_val) ;

//// Utilisation du B evenementiel
//// Valeur par défaut : FALSE
//extern int get_Event_B(void);
//extern void set_Event_B(int new_val) ;

#endif /* _C_ENV_H_ */
