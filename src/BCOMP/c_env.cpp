/******************************* CLEARSY **************************************
* Fichier : $Id: c_env.cpp,v 2.0 2004-01-15 09:35:39 cm Exp $
* (C) 2008 CLEARSY
*
* Description :			Parametrage du compilateur B par des variables
*						d'environnement
*
* Compilation :  -DDEBUG_PARAM pour tracer les tests

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
#include "c_port.h"
RCS_ID("$Id: c_env.cpp,v 1.44 2004-01-15 09:35:39 cm Exp $") ;


/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

//Gestionnaire de ressources
#include <p_api.h>

/* Includes Composant Local */
#include "c_api.h"

//
//  Liste de tous les paramètres du BCOMP.
//
typedef enum
{
  /* 00 */ tab_width ,
  /* 01 */ extended_sees ,
  /* 02 */ strict_B,
  /* 03 */ disable_proof_clash_detection,
  /* 04 */ disable_case_selector_check ,
  /* 05 */ B0_allow_parallel ,
  /* 06 */ B0_allow_pre ,
  /* 07 */ B0_allow_choice ,
  /* 08 */ B0_allow_select ,
  /* 09 */ B0_allow_any ,
  /* 10 */ B0_allow_let ,
  /* 11 */ B0_allow_becomes_member_of ,
  /* 12 */ B0_allow_becomes_such_that ,
  /* 13 */ // Cette ressource n'est plus utilisée.
           B0_allow_read_cst_in_values ,
  /* 14 */ B0_disable_predicate_syntax_check ,
  /* 15 */ B0_disable_expression_syntax_check ,
  /* 16 */ B0_disable_formal_parameter_type_check ,
  /* 17 */ B0_disable_concrete_constants_type_check ,
  /* 18 */ B0_disable_concrete_variables_type_check ,
  /* 19 */ B0_disable_operation_input_parameters_type_check ,
  /* 20 */ B0_disable_operation_output_parameters_type_check ,
  /* 21 */ B0_disable_local_variables_type_check ,
  /* 22 */ B0_disable_variables_initialisation_check ,
  /* 23 */ B0_disable_array_compatibility_check ,
  /* 24 */ B0_disable_parameter_instanciation_check ,
  /* 25 */ B0_disable_valuation_check ,
  /* 26 */ B0_enable_typing_identifier ,
  /* 27 */ minint_value ,
  /* 28 */ maxint_value ,
  /* 29 */ kernel_joker_is_an_error ,
  /* 30 */ typing_ident_mode ,
  /* 31 */ B0_enable_restricted_formal_parameter_instanciation ,
  /* 32 */ dep_mode,  // Analyse des dépendances
  /* 33 */ unique_record_type_mode,
  /* 34 */ output_path,
  /* 35 */ allow_unused_local_variables,
  /* 36 */ allow_unbounded_builtin_types,
  /* 37 */ disable_scalar_set_constant_valuation_b0_check,
  /* 38 */ B0_allow_arrays_of_arrays,
  /* 39 */ Use_B0_Declaration,
  /* 40 */ Generate_EventB_Non_Divergence_PO,
  /* 41 */ Use_PRE_In_Substitution,
  /* 42 */ allow_pragma_in_def,
  /* 43 */ kernel_joker_is_a_warning,
  /* 44 */ allow_while_sub_everywhere,
  /* 45 */ allow_sequential_sub_in_machine,
  /* 46 */ allow_var_sub_in_machine,
  /* 47 */ allow_total_and_surjective_relation_operators,
  // Last param DOIT TOUJOURS RESTER EN DERNIERE POSITION!!!
  /* 48 */ last_param
} T_param ;

//
// Tableau de caractéristiques des paramètres
//
struct T_param_carac
{
  // Nom de la ressource associée
  const char *res_name;

  // Nom de la variable d'environnement associée
  const char *env_var;

  // Valeur par défaut du paramètre
  const char *default_value ;

  // Nom du paramètre
  const char *param_name ;
};

// Tableau de constantes :
// Valeur des paramètres par défaut.
static struct T_param_carac param_carac_st[] =
{
  /* tab_width */
  { "Tab_Width", "BC_TAB_WIDTH", "8", "tab_width"},
  /* extended_sees */
  { "Extended_SEES", "BC_EXTENDED_SEES", "FALSE", "extended_sees"},
  /* strict_B */
  { "Strict_B", NULL , "TRUE", "strict_B" } ,
  /* disable_proof_clash_detection */
  { "Disable_Proof_Clash_Detection",NULL , "FALSE",
    "disable_proof_clash_detection" } ,
  /* disable_case_selector_check */
  { "Disable_CASE_Selector_Check", NULL, "FALSE",
    "disable_case_selector_check" } ,
  /* B0_allow_parallel */
  { "Allow_Parallel", "BC_B0_ALLOW_PARALLEL", "FALSE",
  "B0_allow_parallel" },
  /* B0_allow_pre */
  { "Allow_PRE", "BC_B0_ALLOW_PRE", "FALSE",
  "B0_allow_pre" },
  /* B0_allow_choice */
  { "Allow_CHOICE", "BC_B0_ALLOW_CHOICE", "FALSE",
  "B0_allow_choice" },
  /* B0_allow_select */
  { "Allow_SELECT", "BC_B0_ALLOW_SELECT", "FALSE",
  "B0_allow_select" },
  /* B0_allow_any */
  { "Allow_ANY", "BC_B0_ALLOW_ANY", "FALSE",
  "B0_allow_any" },
  /* B0_allow_let */
  { "Allow_LET", "BC_B0_ALLOW_LET", "FALSE",
  "B0_allow_let" },
  /* B0_allow_becomes_member_of */
  { "Allow_Becomes_Member_Of", "BC_B0_ALLOW_BECOMES_MEMBER_OF", "FALSE",
  "B0_allow_becomes_member_of" },
  /* B0_allow_becomes_such_that */
  { "Allow_Becomes_Such_That", "BC_B0_ALLOW_BECOMES_SUCH_THAT", "FALSE",
  "B0_allow_becomes_such_that" },
  /* B0_allow_read_cst_in_values */
  /* Cette ressource n'est plus utilisée. */
  { "Allow_Read_Cst_In_Values", "BC_B0_ALLOW_READ_CST_IN_VALUES", "FALSE",
  "B0_allow_read_cst_in_values" },
  /* B0_disable_predicate_syntax_check */
  { "Disable_Predicate_Syntax_Check", NULL, "FALSE",
  "B0_disable_predicate_syntax_check" },
  /* B0_disable_expression_syntax_check */
  { "Disable_Expression_Syntax_Check", NULL, "FALSE",
  "B0_disable_expression_syntax_check" },
  /* B0_disable_formal_parameter_type_check */
  { "Disable_Formal_Params_Type_Check", NULL, "FALSE",
  "B0_disable_formal_parameter_type_check" },
  /* B0_disable_concrete_constants_type_check */
  { "Disable_Concrete_Constants_Type_Check", NULL, "FALSE",
  "B0_disable_concrete_constants_type_check" },
  /* B0_disable_concrete_variables_type_check */
  { "Disable_Concrete_Variables_Type_Check", NULL, "FALSE",
    "B0_disable_concrete_variables_type_check" },
  /* B0_disable_operation_input_parameters_type_check */
  { "Disable_Operation_Input_Parameters_Type_Check", NULL, "FALSE",
  "B0_disable_operation_input_parameters_type_check" },
  /* B0_disable_operation_output_parameters_type_check */
  { "Disable_Operation_Output_Parameters_Type_Check", NULL, "FALSE",
  "B0_disable_operation_output_parameters_type_check" },
  /* B0_disable_locale_variables_type_check */
  { "Disable_Local_Variables_Type_Check", NULL, "FALSE",
  "B0_disable_local_variables_type_check" },
  /* B0_disable_variables_initialisation_check */
  { "Disable_Variables_Initialisation_Check", NULL, "FALSE",
  "B0_disable_variables_initialisation_check" },
  /* B0_disable_array_compatibility_check */
  { "Disable_Array_Compatibility_Check", NULL, "FALSE",
  "B0_disable_array_compatibility_check" },
  /* B0_disable_parameter_instanciation_check */
  { "Disable_Parameter_Instanciation_Check", NULL, "FALSE",
  "B0_disable_parameter_instanciation_check" },
  /* B0_disable_valuation_check */
  { "Disable_Valuation_Check", NULL, "FALSE",
  "B0_disable_valuation_check" },
  /* B0_enable_typing_identifier */
  { "Enable_Typing_Identifiers", NULL, "FALSE",
  "B0_enable_typing_identifier" },
  /* minint_value */
  { "MININT_value", NULL, "-2147483647",
  "minint_value" },
  /* maxint_value */
  { "MAXINT_value", NULL, "2147483647",
  "maxint_value" },
  /* kernel_joker_is_an_error */
  { "Kernel_joker_is_an_error", NULL, "FALSE","kernel_joker_is_an_error" },
  /* typing_ident_mode */
  { NULL, NULL, "FALSE", "typing_ident_mode" },
  /* B0_enable_restricted_formal_parameter_instanciation */
  { NULL, NULL,"FALSE","B0_enable_restricted_formal_parameter_instanciation" },
  /* dep_mode */
  { NULL, NULL, "FALSE", "dep_mode"},
  /* unique_record_type_mode */
  { "Unique_Record_Type_Mode", NULL, "FALSE", "unique_record_type_mode"},
  /* output_path */
#ifndef WIN32
  { NULL, NULL, "./", "output_path" },
#else
  { NULL, NULL, ".\\", "output_path" },
#endif
  /* allow_unused_local_variables */
  { "Allow_Unused_Local_Variables", NULL, "FALSE",
    "allow_unused_local_variables" },
  /* allow_unbounded_builtin_types */
  { "Allow_Unbounded_Builtin_Types", NULL, "FALSE",
    "allow_unbounded_builtin_types" },
  /* disable_scalar_set_constant_valuation_b0_check */
  { "Disable_Scalar_Set_Constant_Valuation_B0_Check", NULL, "FALSE",
    "disable_scalar_set_constant_valuation_b0_check" },
  { "B0_Allow_Arrays_Of_Arrays", NULL, "TRUE",
    "B0_allow_arrays_of_arrays" },
  { "Use_B0_Declaration", NULL, "FALSE",
    "Use_b0_declaration" },
  { "Generate_EventB_Non_Divergence_PO", NULL, "FALSE",
    "generate_eventb_non_divergence_po" },
  { "Use_PRE_In_Substitution", NULL, "TRUE", "use_pre_in_substitution"},
  { "allow_pragma_in_def", NULL, "FALSE",
    "allow_pragma_in_def" },
  /* kernel_joker_is_a_warning */
  { "Kernel_joker_is_a_warning", NULL, "TRUE","kernel_joker_is_a_warning" },
  /* allow_while_sub_everywhere*/
  { "Allow_WHILE_sub_everywhere", "BC_ALLOW_WHILE", "FALSE",
  "allow_while_sub_everywhere" },
  /* allow_sequential_sub_in_machine */
  { "Allow_SEQUENTIAL_sub_in_machine", "BC_ALLOW_SEQUENTIAL", "FALSE",
  "allow_sequential_sub_in_machine" },
  /*allow_var_sub_in_machine */
  { "Allow_VAR_sub_in_machine", "BC_ALLOW_VAR_SUB", "FALSE",
  "allow_var_sub_in_machine" },
  { "Allow_total_and_surjective_relation_operators", NULL, "FALSE",
    "allow_total_and_surjective_relation_operators" },
  /* last param */
  { NULL, NULL, NULL, NULL }
};

// Tableau de modification des paramètres :
// On conserve ici la valeur du paramètre dès que celui-ci est lu ou modifié
T_symbol *modified_env[last_param] ;

// Pseudo-constantes couramment utilisées
static T_symbol *TRUE_sop ;
static T_symbol *FALSE_sop ;

//  Flag indiquant qu'on a vérifié la cohérence de MININT et MAXINT
static int minint_maxint_checked_si = FALSE;


static T_symbol *get_param_value( T_param param );


// Initialisation de l'environnement
void init_env(void)
{
  TRACE0("init_env") ;

  // Initialisation des symboles
  TRUE_sop = lookup_symbol ("TRUE", 4);
  FALSE_sop = lookup_symbol ("FALSE", 5);


  // Initialisation du tableau des modifications
  size_t cur_param = 0 ;
  while ((T_param)cur_param != last_param)
    {
      modified_env[(T_param)(cur_param++)] = NULL ;
    }

#ifdef STOP_AT_LINK
  if (getenv("_BC_STOP_AT_LINK") != NULL)
    {
      set_stop_link_at_number(atoi(getenv("_BC_STOP_AT_LINK"))) ;
    }
#endif

#ifdef STOP_AT_UNLINK
  if (getenv("_BC_STOP_AT_UNLINK") != NULL)
    {
      set_stop_unlink_at_number(atoi(getenv("_BC_STOP_AT_UNLINK"))) ;
    }
#endif

  // Il faudra refaire le test puisque les ressources peuvent changer
  // les valeurs de ces paramètres.
  minint_maxint_checked_si = FALSE;

}


// Fonction de recherche de la valeur d'une ressource. Renvoie NULL si
// la ressource n'est pas positionnée
static T_symbol *get_resource_value(T_param param)
{
  const char *resource_name = param_carac_st[param].res_name ;
  T_symbol *result ;

  if (resource_name != NULL)
    {
      const char *res_value = lookup_resource("BCOMP", resource_name) ;

      if (res_value != NULL)
        {
          result = lookup_symbol (res_value) ;
        }
      else
        {
          result = NULL ;
        }
    }
  else
    {
      result = NULL ;
    }
  return result ;
}

// Fonction de recherche de la valeur d'une variable d'environnement
static T_symbol *get_env_value(T_param param)
{
  const char *env_name = param_carac_st[param].env_var ;
  T_symbol *result ;

  if (env_name != NULL)
    {
      const char *env_value = getenv(env_name) ;

      if (env_value != NULL)
        {
          result = lookup_symbol(env_value) ;
        }
      else
        {
          result = NULL ;
        }
    }
  else
    {
      result = NULL ;
    }
  return result ;
}

// Fonction de recherche de la valeur actuelle d'un paramètre.
// On considère un ordre de priorité dans les affectations :
//      - variable positionnée directement (par un set)
//      - variable positionnée dans les ressources
//      - variable positionnée par une variable d'environnement
//      - variable positionnée par sa valeur par défaut
static T_symbol *get_param_value( T_param param )
{
  T_symbol *value ;

  if ( (value = modified_env[param]) != NULL )
    {
#ifdef DEBUG_PARAM
      TRACE1("Valeur du paramètre %s prise dans le cache",
             param_carac_st[param].param_name) ;
#endif
    }
  // On regarde dans les ressources  else
  else if ((value = get_resource_value(param)) != NULL)
    {
#ifdef DEBUG_PARAM
      TRACE1("Valeur du paramètre %s prise dans les ressources",
             param_carac_st[param].param_name) ;
#endif
    }
  // On regarde dans les variables d'environnement
  else if ( (value = get_env_value(param)) != NULL )
    {
#ifdef DEBUG_PARAM
      TRACE1("Valeur du paramètre %s prise dans les variables d'environnement",
             param_carac_st[param].param_name) ;
#endif
      toplevel_warning(EXTENDED_WARNING,
                       get_warning_msg(W_ENVIRONMENT_VARIABLE_IS_OUT_OF_DATE),
                       param_carac_st[param].env_var,
                       param_carac_st[param].res_name) ;
    }
  else
    {
#ifdef DEBUG_PARAM
      TRACE1("Valeur du paramètre %s prise par défaut",
             param_carac_st[param].param_name) ;
#endif
      // On prend la valeur par défaut
      value = lookup_symbol (param_carac_st[param].default_value) ;
    }

  assert (value != NULL) ;

  //  Mémorisation de la valeur -> cache
  modified_env[param] = value ;
  return value ;
}

static void set_param( T_param param , T_symbol *new_value )
{
  modified_env[param] = new_value ;
#ifdef DEBUG_PARAM
  TRACE1("Paramètre %s positionné explicitement",
         param_carac_st[param].param_name) ;
#endif
}


static int get_param_int(T_param param)
{
  return strtol( get_param_value(param)->get_value(), NULL, 10 ) ;
}


static int get_param_bool(T_param param)
{
  return get_param_value(param) == TRUE_sop ;
}

static void set_param_bool( T_param param , int new_value )
{
//  modified_env[param] = new_value ? TRUE_sop : FALSE_sop ;
  set_param(param, new_value ? TRUE_sop : FALSE_sop) ;
#ifdef DEBUG_PARAM
  TRACE1("Paramètre %s positionné explicitement",
         param_carac_st[param].param_name) ;
#endif
}

// Fonctions permettant de modidier la valeur par défaut du paramètre
static void set_default_value( T_param param , const char *new_value )
{
  param_carac_st[param].default_value = new_value ;
#ifdef DEBUG_PARAM
  TRACE1("Modification de la valeur par défaut du paramètre %s",
         param_carac_st[param].param_name) ;
#endif
}

static void set_default_value_bool(T_param param,
                                            int new_default_value)
{
  param_carac_st[param].default_value = new_default_value ? "TRUE" : "FALSE" ;
#ifdef DEBUG_PARAM
  TRACE1("Modification de la valeur par défaut du paramètre %s",
         param_carac_st[param].param_name) ;
#endif
}

//----------------------------------------------------------------
// Fonctions externes permettant de lire et de positionner chaque
// paramètre.
//----------------------------------------------------------------

extern size_t get_tab_width(void)
{
  return (size_t)get_param_int(tab_width) ;
}

// SEES entendu
extern int get_extended_sees(void)
{
  return get_param_bool(extended_sees) ;
}

// Autorisation du xx en implementation
extern int get_B0_allow_parallel(void)
{
  return get_param_bool(B0_allow_parallel) ;
}

extern int get_B0_allow_pre(void)
{
  return get_param_bool(B0_allow_pre) ;
}

extern int get_B0_allow_choice(void)
{
  return get_param_bool(B0_allow_choice) ;
}

extern int get_B0_allow_select(void)
{
  return get_param_bool(B0_allow_select) ;
}

extern int get_B0_allow_any(void)
{
  return get_param_bool(B0_allow_any) ;
}

extern int get_B0_allow_let(void)
{
  return get_param_bool(B0_allow_let) ;
}

extern int get_B0_allow_becomes_member_of(void)
{
  return get_param_bool(B0_allow_becomes_member_of) ;
}

extern int get_B0_allow_becomes_such_that(void)
{
  return get_param_bool(B0_allow_becomes_such_that) ;
}

extern int get_B0_disable_predicate_syntax_check(void)
{
  return get_param_bool(B0_disable_predicate_syntax_check) ;
}

extern int get_B0_disable_expression_syntax_check(void)
{
  return get_param_bool(B0_disable_expression_syntax_check) ;
}

extern int get_B0_disable_formal_params_type_check(void)
{
  return get_param_bool(B0_disable_formal_parameter_type_check) ;
}

extern int get_B0_disable_concrete_constants_type_check(void)
{
  return get_param_bool(B0_disable_concrete_constants_type_check) ;
}

extern int get_B0_disable_concrete_variables_type_check(void)
{
  return get_param_bool(B0_disable_concrete_variables_type_check) ;
}

extern int get_B0_disable_operation_input_parameters_type_check(void)
{
  return get_param_bool(B0_disable_operation_input_parameters_type_check) ;
}

extern int get_B0_disable_operation_output_parameters_type_check(void)
{
  return get_param_bool(B0_disable_operation_output_parameters_type_check) ;
}

extern int get_B0_disable_locale_variables_type_check(void)
{
  return get_param_bool(B0_disable_local_variables_type_check) ;
}

extern int get_B0_disable_variables_initialisation_check(void)
{
  return get_param_bool(B0_disable_variables_initialisation_check) ;
}

extern int get_B0_disable_array_compatibility_check(void)
{
  return get_param_bool(B0_disable_array_compatibility_check) ;
}

extern int get_B0_disable_parameter_instanciation_check(void)
{
  return get_param_bool(B0_disable_parameter_instanciation_check) ;
}

extern int get_B0_disable_valuation_check(void)
{
  return get_param_bool(B0_disable_valuation_check) ;
}

extern int get_B0_generate_eventb_non_divergence_po(void)
{
  return get_param_bool(Generate_EventB_Non_Divergence_PO) ;
}


// B0 Checker adapte a la traduction HIA, i.e. acceptant le typage
// par identificateur
extern void set_B0_enable_typing_identifiers(int new_val)
{
  set_default_value_bool(B0_enable_typing_identifier, new_val) ;
}

extern int get_B0_enable_typing_identifiers(void)
{
  return get_param_bool(B0_enable_typing_identifier) ;
}

// B0 Checker, controle supplementaire (e.g., pour le traducteur HIA) :
// verifier que les instanciations de machines requises, i.e. les
// parametres formels effectifs, ne font pas intervenir de parametres
// formels de la machine
extern void set_B0_enable_restricted_formal_param_instanciation(int new_val)
{
  set_default_value_bool(B0_enable_restricted_formal_parameter_instanciation,
                         new_val) ;
}

// B0 Checker, controle supplementaire pour le traducteur ALSTOM
extern void set_B0_allow_becomes_such_that(int new_val)
{
  set_default_value_bool(B0_allow_becomes_such_that, new_val) ;
}

extern int get_B0_enable_restricted_formal_param_instanciation(void)
{
  return get_param_bool(B0_enable_restricted_formal_parameter_instanciation) ;
}

// Detection des identificateurs de 1 caractere : warning ou erreur ?
// warning par defaut (i.e. is_an_error = FALSE)
extern void set_kernel_joker_is_an_error(int is_an_error)
{
  set_default_value_bool (kernel_joker_is_an_error, is_an_error) ;
}

extern int get_kernel_joker_is_an_error(void)
{
  return get_param_bool(kernel_joker_is_an_error) ;
}

extern void set_kernel_joker_is_a_warning(int is_a_warning)
{
  set_default_value_bool (kernel_joker_is_a_warning, is_a_warning) ;
}

extern int get_kernel_joker_is_a_warning(void)
{
  return get_param_bool(kernel_joker_is_a_warning) ;
}

// Detection des conflits "caches" pour la preuve ?
extern void set_proof_clash_detection(int new_proof_clash_detection)
{
  set_default_value_bool (disable_proof_clash_detection,
                          new_proof_clash_detection) ;
}


extern int get_proof_clash_detection(void)
{
  return !get_param_bool(disable_proof_clash_detection);
}

// Non-vérification que les sélecteurs de case sont des littéraux ?
extern int get_disable_case_selector_check(void)
{
  return get_param_bool(disable_case_selector_check) ;
}

// Mode "typage par identificateur"
// Utilise par les clients de traduction (e.g., traducteur HIA)
extern int get_typing_ident_mode(void)
{
  return get_param_bool(typing_ident_mode) ;
}

extern void set_typing_ident_mode(int new_typing_ident_mode)
{
  set_default_value_bool (typing_ident_mode, new_typing_ident_mode) ;
}

// Valeurs pour MININT/MAXINT
  // Vérification de la cohérence des valeurs maxint / minint.
  //
  //  Conséquence: les valeurs de minint et maxint sont figées ici, et
  //  non pas sur l'appel � la fonction "get_..." comme pour les
  //  autres paramètres.
static void check_minint_maxint(T_symbol *minint, T_symbol *maxint)
{
  if (minint_maxint_checked_si == FALSE)
    {
      if (maxint->is_less_than(minint))
        {
          toplevel_error(CAN_CONTINUE,
                         get_error_msg(E_MININT_IS_GREATER_THAN_MAXINT),
                         modified_env[minint_value]->get_value(),
                         modified_env[maxint_value]->get_value()) ;
        }
      minint_maxint_checked_si = TRUE ;
    }
}

extern T_symbol *get_minint_value(void)
{
  T_symbol *minint = get_param_value(minint_value);
  T_symbol *maxint = get_param_value(maxint_value);

  check_minint_maxint(minint, maxint);

  return minint ;
}

extern T_symbol *get_maxint_value(void)
{
  T_symbol *minint = get_param_value(minint_value);
  T_symbol *maxint = get_param_value(maxint_value);

  check_minint_maxint(minint, maxint);

  return maxint ;
}

// Pour savoir si l'on a demande une compilation "stricte" B
extern int get_strict_B_demanded(void)
{
  return get_param_bool(strict_B) ;
}

// Pour demander une compilation "stricte" B
extern void set_strict_B_demanded(void)
{
  set_default_value(strict_B, "TRUE") ;
}

// Active / désactive le mode de dépenances
extern void set_dep_mode(int new_dep_mode)
{
  set_param_bool(dep_mode,new_dep_mode) ;
}

extern int get_dep_mode(void)
{
#if 1
  // J'ai changé la méthode T_item::T_item pour qu'elle n'appelle plus
  // get_dep_mode() sur la création d'un nouveau symbole.
  return get_param_bool(dep_mode) ;
#else
  // On ne peut pas utiliser la fonction générique get_param_bool car
  // elle appelle lookup_symbol qui appelle un constructeur de symbole
  // qui fait un appel à get_dep_mode -> boucle infinie.
  if (modified_env[dep_mode] == NULL)
    {
      return strtol(param_carac_st[dep_mode].default_value, NULL, 10) ;
    }
  else
    {
      return (modified_env[dep_mode]->get_value(), NULL, 10) ;
    }
#endif
}

// Mode ou l'on n'effectue pas de copie privees pour les types
// records afin que toutes les expressions records aient tous
// les labels positionnes
// (mode special pour le GNF, incompatible avec le typage par
// identificateur de HIA)
// Par defaut : FALSE
int get_unique_record_type_mode(void)
{
  return get_param_bool(unique_record_type_mode) ;
}
void set_unique_record_type_mode(int new_mode)
{
  set_default_value_bool(unique_record_type_mode, new_mode) ;
}


// Lecture du path de production des fichiers
const char *get_output_path(void)
{
  return get_param_value(output_path)->get_value() ;
}

size_t get_output_path_len(void)
{
  return get_param_value(output_path)->get_len() ;
}

// Mise a jour du path de production des fichiers
void set_output_path(const char *new_output_path)
{
  set_param(output_path, lookup_symbol(new_output_path)) ;
}

// Autorisation des déclarations de variables locales inutiles
extern int get_allow_unused_local_variables(void)
{
  return get_param_bool(allow_unused_local_variables) ;
}

extern void set_allow_unused_local_variables(int new_val)
{
  set_default_value_bool(allow_unused_local_variables, new_val) ;
}

// Autorisation des types builtin infinis (INTEGER, NATURAL et NATURAL1)
extern int get_allow_unbounded_builtin_types(void)
{
  return get_param_bool(allow_unbounded_builtin_types) ;
}

extern void set_allow_unbounded_builtin_types(int new_val)
{
  set_default_value_bool(allow_unbounded_builtin_types, new_val) ;
}

// Suppression des contrôles B0 sur la valuation d'une constante concrète
// représentant un ensemble de scalaires.
extern int get_disable_scalar_set_constant_valuation_b0_check(void)
{
  return get_param_bool(disable_scalar_set_constant_valuation_b0_check) ;
}

extern void
   set_disable_scalar_set_constant_valuation_b0_check(int new_val)
{
  set_default_value_bool(disable_scalar_set_constant_valuation_b0_check,
                         new_val) ;
}

// Autorisation des tableaux de tableaux en B0
extern int get_B0_allow_arrays_of_arrays(void)
{
  return get_param_bool(B0_allow_arrays_of_arrays) ;
}

extern void set_B0_allow_arrays_of_arrays(int new_val)
{
  set_default_value_bool(B0_allow_arrays_of_arrays, new_val) ;
}

// Mode BOM
extern int get_Use_B0_Declaration(void)
{
  return get_param_bool(Use_B0_Declaration) ;
}
extern void set_Use_B0_Declaration(int new_val)
{
  set_default_value_bool(Use_B0_Declaration, new_val) ;
}

extern void set_B0_allow_becomes_member_of(int new_val)
{
  set_default_value_bool(B0_allow_becomes_member_of, new_val);
}
extern int get_allow_pre_in_substitution(void)
{
  return get_param_bool(Use_PRE_In_Substitution);
}
extern void set_allow_pre_in_substitution(int new_val)
{
  return set_default_value_bool(Use_PRE_In_Substitution, new_val);
}

extern int get_allow_pragma_in_def(void)
{
  return get_param_bool(allow_pragma_in_def) ;
}
extern void set_allow_pragma_in_def(int new_val)
{
  set_default_value_bool(allow_pragma_in_def, new_val) ;
}
extern int get_allow_while_sub_everywhere(void)
{
  return get_param_bool(allow_while_sub_everywhere) ;
}

extern void set_allow_while_sub_everywhere(int new_val)
{
  set_default_value_bool(allow_while_sub_everywhere, new_val) ;
}

extern int get_allow_sequential_sub_in_machine(void)
{
  return get_param_bool(allow_sequential_sub_in_machine) ;
}

extern void set_allow_sequential_sub_in_machine(int new_val)
{
  set_default_value_bool(allow_sequential_sub_in_machine,new_val) ;
}

extern int get_allow_var_sub_in_machine(void)
{
  return get_param_bool(allow_var_sub_in_machine) ;
}

extern void set_allow_var_sub_in_machine(int new_val)
{
  set_default_value_bool(allow_var_sub_in_machine,new_val) ;
}

extern int get_allow_total_and_surjective_relation_operators(void)
{
  return get_param_bool(allow_total_and_surjective_relation_operators) ;
}
extern void set_allow_total_and_surjective_relation_operators(int new_val)
{
  set_default_value_bool(allow_total_and_surjective_relation_operators, new_val) ;
}


//
//extern int get_Event_B(void)
//{
//  return get_param_bool(EventB_Component) ;
//}
//extern void set_Event_B(int new_val)
//{
//  set_default_value_bool(EventB_Component, new_val) ;
//}
