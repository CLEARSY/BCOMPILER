/******************************* CLEARSY **************************************
* Fichier : $Id: v_toplvl.cpp,v 2.0 2002-07-12 12:55:30 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de haut niveau de l'analyseur semantique
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
#include "c_port.h"
RCS_ID("$Id: v_toplvl.cpp,v 1.57 2002-07-12 12:55:30 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_tstamp.h"

// Forwards
// Analyse semantique du niveau "composant"
extern T_betree *component_seman_analysis(T_betree *betree) ;

// Analyse d'anticollision
extern T_betree *collision_analysis(T_betree *betree) ;
extern T_betree *proof_clash_analysis(T_betree *betree) ;

// Fonction qui rajoute une initialisation qui ne contient qu'un skip
// quand il n'y a pas d'initialisation dans la machine.
extern void insert_blank_init(T_betree *betree) ;

// Fonction qui fait l'analyse semantique d'un Betree syntaxique
// Le Betree enrichi est retourne en cas de succes. En cas d'erreur,
// la fonction renvoie NULL
T_betree *compiler_semantic_analysis(T_betree *syntaxic_betree, const char* converterName)
{
  TRACE3("DEBUT ANALYSE SEMANTIQUE (%x:%s status %d)",
		 syntaxic_betree,
		 syntaxic_betree->get_betree_name()->get_value(),
		 syntaxic_betree->get_status()) ;

  ENTER_TRACE ; ENTER_TRACE ;

  if (get_no_exit_mode() == TRUE)
	{
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans compiler_syntax_analysis") ;
		  return NULL ;
		}

	  set_exit_handler(&env) ;
	}

  // Initialisation des tables de visibilite
  // Plus fait dans init_compiler (cf commentaire dans c_api.cpp)
  init_visibility_tables() ;

  ASSERT(syntaxic_betree->get_status() == BST_SYNTAXIC) ;

  verbose_describe(get_catalog(C_SEMANTIC_ANALYSIS),
				   syntaxic_betree->get_root()->get_machine_name()
				   ->get_name()->get_value()) ;

  // Chargement des machines requises
  syntaxic_betree->set_tmp2(FALSE) ; // on ne l'a pas encore fait ...

  TRACE0("--> DEBUT CHARGEMENT DES MACHINES REQUISES") ;
  ENTER_TRACE ;
  T_betree *betree = get_betree_manager()->get_betrees() ;

  while (betree != NULL)
	{
          load_required_machines(betree, converterName) ;
	  insert_blank_init(betree) ;


	  betree = betree->get_next_betree() ;

	  // Attention il faut remettre le handler d'exit qui a pu etre
	  // ecrase par compiler_syntax_analysis
	  if (get_no_exit_mode() == TRUE)
		  {
			  jmp_buf env ;			// Pour retour en cas d'erreur

			  if (setjmp(env) != 0)
				  {
					  // On est en retour d'erreur ! */
					  TRACE0("retour d'erreur dans compiler_syntax_analysis") ;
					  return NULL ;
				  }

			  set_exit_handler(&env) ;
		  }
	}

  T_betree *res = get_betree_manager()->get_betrees() ;

  EXIT_TRACE ;
  TRACE0("<-- DEBUT CHARGEMENT DES MACHINES REQUISES") ;

  // Verification de l'abscence d'erreurs
  if (get_error_count() != 0)
	{
	  return syntaxic_betree ;
	}

  //
  //	I) ANALYSE DE CONFORMITE
  //
  // Analyse du semantique niveau "composant"
  res = component_seman_analysis(syntaxic_betree) ; // Ne fait rien !

  // Controles au niveau du projet Correction bug 2386 : Il faut faire
  // l'analyse du projet sur tout l'arbre de betrees !
  T_betree *cur_betree = res ;
  while (cur_betree != NULL)
	{
	  cur_betree = project_analysis(cur_betree) ;
	  cur_betree = cur_betree->get_next_betree() ;
	}


  //
  //	II) CONTROLES D'ANTICOLLISION
  //

  // Pour tous les betree charges : heritage de l'environnement des
  // machines incluses
  // On le fait a l'envers (ordre de dependance)
  // En mode EVOL_INCLUDES, on importe aussi pour toutes les machines
  // l'environnement de leur abstraction
  T_betree *cur = res->get_manager()->get_last_betree() ;


  while (cur != NULL)
	{
	  T_machine *root = cur->get_root() ;

	  T_used_machine *cur_includes = root->get_includes() ;
	  //GP 20/11/98. T_list_link pour memoriser les machines
	  //incluses
	  T_list_link* first_already_included = NULL ;
	  T_list_link* last_already_included = NULL ;

	  while (cur_includes != NULL)
		{
#ifndef NO_CHECKS
		  if (cur_includes->get_ref_machine() == NULL)
			{
			  fprintf(stderr,
                                          "%s:%d:%d: PANIC machine %p:%s, ref_machine = NULL\n",
					  cur_includes->get_ref_lexem()->get_file_name()->get_value(),
					  cur_includes->get_ref_lexem()->get_file_line(),
					  cur_includes->get_ref_lexem()->get_file_column(),
                                          cur_includes,
					  cur_includes->get_name()->get_name()->get_value()) ;
			  TRACE0("Panic ! REF_MACHINE = NULL") ;
			}
		  else
			{
#endif // NO_CHECKS

			  //GP 20/11/98 was:
			  //root->import_included_environment(cur_includes->get_ref_machine())
			  //;
			  root->import_included_environment(cur_includes->get_ref_machine(),
												cur_includes->get_ref_lexem(),
												&first_already_included,
												&last_already_included) ;
#ifndef NO_CHECKS
			}
#endif // NO_CHECKS
		  cur_includes = (T_used_machine *)cur_includes->get_next() ;
		}

	  // En spec et en raffinement, EXTENDS = INCLUDES + PROMOTES donc il
	  // faut faire le meme travail que pour l'INCLUDES
	  // Par contre, en implementation, EXTENDS = IMPORTS + PROMOTES donc
	  // on ne recupere pas l'environnement
	  if (root->get_machine_type() != MTYPE_IMPLEMENTATION)
		{
		  T_used_machine *cur_extends = root->get_extends() ;

		  while (cur_extends != NULL)
			{
			  //GP 20/11/98 was:
			  //root->import_included_environment(cur_includes->get_ref_machine())
			  //;
			  root->import_included_environment(cur_extends->get_ref_machine(),
												cur_extends->get_ref_lexem(),
												&first_already_included,
												&last_already_included) ;
			  cur_extends = (T_used_machine *)cur_extends->get_next() ;
			}
		}

	  cur = cur->get_prev_betree() ;

	}


  T_machine *root = res->get_root() ;
  T_machine *cur_mach =
	(root->get_ref_specification() == NULL) ? root : root->get_ref_specification() ;

  while (cur_mach != NULL)
	{
      if (cur_mach->get_machine_type() != MTYPE_SPECIFICATION &&
              cur_mach->get_machine_type() != MTYPE_SYSTEM)
		{
		  cur_mach->lookup_glued_data() ;
		  T_lexem *ref_lexem = cur_mach->get_abstraction_name()->get_ref_lexem();
		  cur_mach->import_abstract_environment(ref_lexem) ;
		}

	  cur_mach = cur_mach->get_ref_refinement() ;
	}

  // Analyse d'anticollision "composant"
  // Il faut faire une analyse par betree present
  // Celle du betree courant ne suffit pas car par exemple
  // dans le cas d'une implementation, on ne fait pas
  // les controles de collision de la spec
  // (on rappelle que l'on utilise ici les algos de
  // verification de collision du manu.ref B)
  cur = res->get_manager()->get_betrees() ;

  while (cur != NULL)
	{
      if (!cur->get_collision_analysed()) {
          cur = collision_analysis(cur) ;
          cur->set_collision_analysed(true);
      }

	  if (get_proof_clash_detection() == TRUE)
		{
		  // Recherche des conflits "caches"
		  // On ne le fait que pour le point d'entree de l'analyse courante
		  // Heuristique :
		  // Pour le savoir, on verifie que c'est le premier betree de la liste
		  if (cur->get_prev_betree() == NULL)
			{
			  cur = proof_clash_analysis(cur) ;
			}
		}

	  cur = cur->get_next_betree() ;
	}

  // S'il y a des erreurs, on ne peut pas continuer
  // On renvoie res et pas NULL pour que le client exploite s'il veut
  // le bout de Betree qu'on a reussi a construire
  if (get_error_count() != 0)
	{
	  return res ;
	}

  //
  //	III) RESOLUTION DES IDENTIFICATEURS
  //

  // Resolution des identificateurs + correction de l'arbre
  TRACE1("------------ DEBUT RESOLUTION DES IDENTIFICATEURS BETREE %s ------------",
		 res->get_file_name()->get_value()) ;
  ENTER_TRACE ; ENTER_TRACE ;

  T_ident *cur_ident = get_object_manager()->get_unsolved_idents() ;

  while (cur_ident != NULL)
	{
#ifdef DEBUG_FIND
	  TRACE5("DEBUT RESOLUTION DE %x:%s (%s:%d:%d)",
			 cur_ident,
			 cur_ident->get_name()->get_value(),
			 cur_ident->get_ref_lexem()->get_file_name()->get_value(),
			 cur_ident->get_ref_lexem()->get_file_line(),
			 cur_ident->get_ref_lexem()->get_file_column()) ;
	  ENTER_TRACE ;
#endif
	  cur_ident->solve_ident() ;

	  // Autres verifications eventuelles en cas de succes
	  if (cur_ident->get_def() != NULL)
		{
		  cur_ident->extra_checks() ;
		}

	  get_object_manager()->delete_unsolved_ident(cur_ident) ;

#ifdef DEBUG_FIND
	  EXIT_TRACE ;
	  TRACE1("FIN RESOLUTION DE %s", cur_ident->get_name()->get_value()) ;
#endif

	  cur_ident = cur_ident->get_next_unsolved_ident() ;
	}

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE0("-------------- FIN RESOLUTION DES IDENTIFICATEURS --------------") ;

  // S'il y a des erreurs, on ne peut pas continuer
  if (get_error_count() != 0)
	{
	  return res ;
	}

  //
  //	IV) Correction de l'arbre
  //
  TRACE0("---------------- DEBUT CORRECTION ARBRE -----------") ;
  ENTER_TRACE ; ENTER_TRACE ;
  TRACE0("1) Verification des T_op_res") ;
  ENTER_TRACE ;
  T_op_result *cur_op_res = get_object_manager()->get_op_results() ;

  while (cur_op_res != NULL)
	{
	  T_op_result *next = cur_op_res->get_next_op_result() ;
	  cur_op_res->check_is_an_op_result() ;
	  //	  get_object_manager()->remove_op_result(cur_op_res) ;
	  cur_op_res = next ;
	}

  // Reset de la liste
  get_object_manager()->reset_op_result_list() ;

  EXIT_TRACE ;
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE0("----------------  FIN  CORRECTION ARBRE -----------") ;



  // Verification des T_ident avec des $0
  // Correction bug 2386 : Il faut faire
  // la v�rification sur tout l'arbre de betrees !
  cur_betree = res ;
  while (cur_betree != NULL)
		 {
		   semantic_check_dollar_zero(cur_betree) ;
		   cur_betree = cur_betree->get_next_betree() ;
		 }

  //
  //	V) Typage des donnees
  //
  TRACE0("---------------- DEBUT TYPE CHECK -------------") ;
  ENTER_TRACE ; ENTER_TRACE ;
  // Correction bug 2386 : Il faut faire le type_check sur tout
  // l'arbre de betrees !
  cur_betree = res ;
  while (cur_betree != NULL)
		 {
		   cur_betree = type_check(cur_betree) ;
		   cur_betree = cur_betree->get_next_betree() ;
		 }
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE0("---------------- FIN TYPE CHECK -------------") ;

  //
  //
  // CONTROLES SUPPLEMENTAIRE POUR LES OPERATIONS LOCALES :
  //
  if (    (res->get_root()->get_machine_type() == MTYPE_IMPLEMENTATION)
	   && (res->get_root()->get_local_operations() != NULL) )
	{
	  //
	  // a) pas de cycle (SEMLOC 3)
	  //
	  TRACE0("--- DEBUT VERIF CYCLES OPERATIONS LOCALES") ;
	  ENTER_TRACE ;
	  T_op *cur_op = res->get_root()->get_operations() ;
	  while (cur_op != NULL)
		{
		  cur_op->check_local_op_cycle() ;
		  cur_op = (T_op *)cur_op->get_next() ;
		}
	  EXIT_TRACE ;
	  TRACE0("--- FIN VERIF CYCLES OPERATIONS LOCALES") ;
	  //
	  // b) pas d'appel simultane dans une spec d'op loc de deux op d'une meme
	  // instance de machine importee (SEMLOC 5)
	  //
	  if (    (res->get_root()->get_imports() != NULL)
	       || (res->get_root()->get_extends() != NULL) )
		{
		  TRACE0("--- DEBUT VERIF OPCALL SPEC OPERATIONS LOCALES") ;
		  ENTER_TRACE ;
		  T_op *cur_op = res->get_root()->get_local_operations() ;
		  while (cur_op != NULL)
			{
			  cur_op->check_local_op_calls() ;
			  cur_op = (T_op *)cur_op->get_next() ;
			}
		  EXIT_TRACE ;
		  TRACE0("--- FIN VERIF OPCALL SPEC OPERATIONS LOCALES") ;
		}
	}

  //
  //	VI) VERIFICATION DE L'EXISTENCE ET DE LA LOCALISATION DU TYPAGE/INIT
  //
  TRACE0("--- DEBUT VERIF EXISTENCE ET LOCALISATION DES TYPAGES/INIT  ---") ;
  ENTER_TRACE ; ENTER_TRACE ;
  res = check_typing_location(res) ;
  res = check_init_location(res) ;
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE0("---  FIN  VERIF EXISTENCE ET LOCALISATION DES TYPAGES/INIT  ---") ;

  //
  // ET C'EST FINI !
  //
  res->set_status(BST_SEMANTIC) ;

  if (get_no_exit_mode() == TRUE)
	{
	  set_exit_handler(NULL) ;
	}

  return res ;
}

// Fonction interne qui verifie que ident est type dans
// betree dans la clause lex_type
static void internal_check_typing_location(T_ident *ident,
													T_betree *betree,
													T_lex_type lex_type)
{
  TRACE4(">> internal_check_typing_location(%x:%s, %x, %d)",
		 ident,
		 ident->get_name()->get_value(),
		 betree,
		 lex_type) ;
  ENTER_TRACE ;
  // Il faut prendre d'abord essayer get_original_typing_location() et
  // pas tout simplement ident->get_typing_location() car dans le cas
  // d'une constante intervalle valuee, le get_typing_location donne
  // la valuation (clause VALUES) et pas le typage (clause PROPERTIES)


  T_lexem *loc = ident->get_original_typing_location() ;
  if (loc == NULL)
	{
	  loc = ident->get_typing_location() ;
	}
  TRACE3("ident %x->get_definition %x->get_typing_location %x",
		 ident,
		 ident->get_definition(),
		 ident->get_definition()->get_typing_location()) ;

  if (loc == NULL)
	{
	  semantic_error(ident,
					 CAN_CONTINUE,
					 get_error_msg(E_NO_TYPE_FOR_IDENT_LOC_EXPECTED),
					 ident->get_ident_type_name(),
					 ident->get_name()->get_value(),
					 ident->get_name()->get_value(),
					 get_lex_type_ascii(lex_type),
					 betree->get_betree_name()->get_value()) ;
	  return ;
	}

  // On cherche la clause
  // GP sauvegarde de loc pour le message d'erreur
  T_lexem *original_loc = loc ;
  while (is_a_clause(loc) == FALSE)
	{
	  loc = loc->get_prev_lexem() ;
	  ASSERT(loc != NULL) ; // on ne peut pas typer "hors clause"
	}

  if (loc->get_lex_type() != lex_type)
	{
	  TRACE4("on ecrie loc->get_lex_type() %x:%s, lex_type %d:%s",
			 loc->get_lex_type(),
			 get_lex_type_ascii(loc->get_lex_type()),
			 lex_type,
			 get_lex_type_ascii(lex_type)) ;
	  //GP ici remplace loc par original_loc
	  // A cause du parcours precedent loc ne pointait plus sur
	  // l'expression de typage mais sur la clause ou elle etait typee
	  // GP WAS: semantic_error(loc,
	  semantic_error(original_loc,
					 CAN_CONTINUE,
					 get_error_msg(E_IDENT_CAN_NOT_BE_TYPED_HERE),
					 ident->get_ident_type_name(),
					 ident->get_name()->get_value(),
					 get_lex_type_ascii(loc->get_lex_type()),
					 get_lex_type_ascii(lex_type)) ;

	}

  EXIT_TRACE ;
  TRACE3("<< internal_check_typing_location(%x, %x, %d)", ident, betree, lex_type) ;
}


// Analyse semantique : verification de l'existence et de la
// localisation du typage
T_betree *check_typing_location(T_betree *betree)
{
  TRACE2("-> check_typing_location(%x:%s)",
		 betree,
		 betree->get_betree_name()->get_value()) ;
  ENTER_TRACE ;

  T_machine *machine = betree->get_root() ;

  // ELTYPE1 : les parametres formels scalaires doivent etre types
  // dans la clause CONSTRAINTS
  TRACE0("parametres formels scalaires, CONSTRAINTS") ;
  T_ident *cur_fscal = machine->get_params() ;

  T_lex_type location;
  if (get_Use_B0_Declaration() == TRUE)
	{
	  location = machine->get_ref_lexem()->get_lex_type();
	}
  else
	{
	  location = L_CONSTRAINTS;
	}
  while (cur_fscal != NULL)
	{
	  if (cur_fscal->get_ident_type() == ITYPE_MACHINE_SCALAR_FORMAL_PARAM)
		{
		  internal_check_typing_location(cur_fscal, betree, location) ;
		}
	  cur_fscal = (T_ident *)cur_fscal->get_next() ;
	}

  // ELTYPE2 : les constantes doivent etre typees
  // dans la clause PROPERTIES
  // en mode BOM les constantes concr�te doivent etre typ�es dans la
  // clause CONCRETE_CONSTANTS
  TRACE0("constantes, PROPERTIES") ;

  T_lex_type clause;
  if (get_Use_B0_Declaration() == TRUE)
	{
	  clause = L_CONCRETE_CONSTANTS;
	}
  else
	{
	  clause = L_PROPERTIES;
	}

  T_ident *cur_cst = machine->get_concrete_constants() ;
  while (cur_cst != NULL)
	{
	  internal_check_typing_location(cur_cst, betree, clause) ;
	  cur_cst = (T_ident *)cur_cst->get_next() ;
	}

  cur_cst = machine->get_abstract_constants() ;
  while (cur_cst != NULL)
	{
	  internal_check_typing_location(cur_cst, betree, L_PROPERTIES) ;
	  cur_cst = (T_ident *)cur_cst->get_next() ;
	}



  // ELTYPE3 : les variables doivent etre typees
  // dans la clause INVARANT
  // en mode BOM les variables concr�te doivent etre typ�es dans la
  // clause CONCRETE_VARIABLES
  TRACE0("variables, INVARIANT") ;
  if (get_Use_B0_Declaration() == TRUE)
	{
	  clause = L_CONCRETE_VARIABLES;
	}
  else
	{
	  clause = L_INVARIANT;
	}
  T_ident *cur_var = machine->get_concrete_variables() ;
  while (cur_var != NULL)
	{
	  internal_check_typing_location(cur_var, betree, clause) ;
	  cur_var = (T_ident *)cur_var->get_next() ;
	}

  cur_var = machine->get_abstract_variables() ;
  while (cur_var != NULL)
	{
	  internal_check_typing_location(cur_var, betree, L_INVARIANT) ;
	  cur_var = (T_ident *)cur_var->get_next() ;
	}


  EXIT_TRACE ;
  TRACE2("<- check_typing_location(%x:%s)",
		 betree,
		 betree->get_betree_name()->get_value()) ;

  return betree ;
}


// Fonction auxiliaire qui verifie qu'un identificateur est initialise
// dans une valuation
static void internal_check_init(T_ident *cur,
										 T_valuation *values,
										 T_machine *machine)
{
  TRACE5("internal_check_init(%x:%s, %x, %x:%s)",
		 cur,
		 cur->get_name()->get_value(),
		 values,
		 machine,
		 machine->get_machine_name()->get_name()->get_value()) ;

  // Il faut trouver l'implementation ...
  T_machine *cur_machine = cur->get_ref_machine() ;
  while (cur_machine->get_ref_refinement() != NULL)
	{
	  cur_machine = cur_machine->get_ref_refinement() ;
	}

  // ... puis parcourir sa clause VALUES
  T_valuation *cur_value = cur_machine->get_values() ;

  TRACE4("cur %x:%s ref_machine %x:%s",
		 cur,
		 cur->get_name()->get_value(),
		 cur_machine,
		 cur_machine->get_machine_name()->get_name()->get_value()) ;
  TRACE1("init_loop : cur_value %x", cur_value) ;
  while (cur_value != NULL)
	{
	  TRACE6("cur_value->get_ident %x:%s def %x, cur %x:%s def %x",
			 cur_value->get_ident(),
			 cur_value->get_ident()->get_name()->get_value(),
			 cur_value->get_ident()->get_definition(),
			 cur,
			 cur->get_name()->get_value(),
			 cur->get_definition()) ;
	  if (cur_value->get_ident()->get_definition() == cur->get_definition())
		{
		  // Ok c'est la bonne valuation
		  return ;
		}

	  cur_value = (T_valuation *)cur_value->get_next() ;
	}

  // Si on arrive ici, c'est que cur n'est pas value
  semantic_error((values == NULL) ? (T_item *)machine : (T_item *)values,
  				 CAN_CONTINUE,
				 get_error_msg(E_IDENT_HAS_NOT_BEEN_VALUATED),
				 cur->get_ident_type_name(),
				 cur->get_name()->get_value(),
				 machine->get_machine_name()->get_name()->get_value()) ;

  if (values != NULL)
	{
	  semantic_error(values,
					 MULTI_LINE,
					 get_error_msg(E_LOCATION_OF_VALUES_CLAUSE)) ;
	}
}

// Fonction auxiliaire qui verifie qu'un identificateur est initialise
// dans une initialisaion
static void internal_check_init(T_ident *cur,
										 T_substitution *init,
										 T_machine *machine)
{
  TRACE5("internal_check_init(%x:%s, %x, %x:%s)",
		 cur,
		 cur->get_name()->get_value(),
		 init,
		 cur->get_definition()->get_ref_machine(),
		 cur->get_definition()->get_ref_machine()->get_machine_name()->get_name()->get_value()) ;

  T_substitution *cur_init;
  if (machine->get_machine_type() == MTYPE_IMPLEMENTATION) {
      // Pour les implémentations, il faut que les variables soient initialisées localement
      cur_init = init;
  } else {
      T_machine* spec = cur->get_definition()->get_ref_machine();
      if (machine->refines(spec)) {
          // Pour les raffinements, les variables déclarées dans une machine raffinées doivent être initialisées localement
          cur_init = init;
      } else {
      // Pour les autres, il est suffisant que les variables soient initialisées dans le composant où elles sont déclarées
          cur_init = spec->get_initialisation() ;
      }
  }

  TRACE1("cur_init %x", cur_init) ;
  while (cur_init != NULL)
	{
	  if (cur_init->modifies(cur->get_definition()) == TRUE)
		{
		  // Ok c'est l'initialisation
		  return ;
		}

	  cur_init = (T_substitution *)cur_init->get_next() ;
	  TRACE1("cur_init %x", cur_init) ;
	}

  // Si on arrive ici, c'est que cur n'est pas value
  // GP WAS: semantic_error((init == NULL) ? machine : init,
  semantic_error(cur,
				 CAN_CONTINUE,
				 get_error_msg(E_IDENT_HAS_NOT_BEEN_INITIALISED),
				 cur->get_ident_type_name(),
				 cur->get_name()->get_value(),
				 machine->get_machine_name()->get_name()->get_value()) ;

  if (init != NULL)
	{
	  semantic_error(init,
					 MULTI_LINE,
					 get_error_msg(E_LOCATION_OF_INITIALISATION_CLAUSE)) ;
	}
}


// Analyse semantique : verification de l'existence et de la
// localisation de l'initialisation
T_betree *check_init_location(T_betree *betree)
{
  TRACE2("-> check_init_location(%x:%s)",
		 betree,
		 betree->get_betree_name()->get_value()) ;
  ENTER_TRACE ;

  T_machine *machine = betree->get_root() ;

  if (get_B0_disable_valuation_check() != TRUE){
      // ELINIT1 : en implementation, les ensembles abstraits
      // doivent etre values
      if (machine->get_machine_type() == MTYPE_IMPLEMENTATION)
      {
          TRACE0("valuation des ensembles abstraits") ;

          T_ident *cur_set = machine->get_sets() ;
          while (cur_set != NULL)
          {
              if ( (cur_set->get_ident_type() == ITYPE_ABSTRACT_SET)
                      &&
                      (cur_set->get_implemented_by() == NULL) )
              {
                  // C'est un ensemble abstrait qui n'est pas implant� par
                  // homonymie, il faut donc qu'il soit valu�.
                  internal_check_init(cur_set, machine->get_values(), machine) ;
              }
              else
              {
                  // Ensemble �num�r� ou implant� par homonymie,
                  // pas de valuation n�cessaire.
              }

              cur_set = (T_ident *)cur_set->get_next() ;
          }
      }

      // ELINIT2 : en implementation, les constantes non implant�es par
      // homonymies doivent etre valu�es
      if (machine->get_machine_type() == MTYPE_IMPLEMENTATION)
      {
          TRACE0("valuation des constantes") ;

          T_ident *cur_cst = machine->get_concrete_constants() ;
          while (cur_cst != NULL)
          {
              if (cur_cst->get_implemented_by() == NULL)
              {
                  // C'est une constante concrete qui n'est pas implant�e
                  // par homonymie.  Elle doit donc etre valuee
                  internal_check_init(cur_cst, machine->get_values(), machine) ;
              }
              else
              {
                  // Constante implant�e par homonymie
              }
              cur_cst = (T_ident *)cur_cst->get_next() ;
          }
      }
  }

  // ELINIT3 : les variables qui ne sont pas implant�es par homonymie
  // doivent etre initialis�.

  TRACE0("init des variables") ;

  T_machine *spec = machine->get_specification() ;
  T_ident *cur_var = machine->get_concrete_variables() ;
  while (cur_var != NULL)
	{
	  if ((cur_var->get_ref_glued() == NULL)
		  ||
		  (cur_var->get_ref_glued()->get_ref_machine()->get_specification() == spec))

		{
		  // c'est une variable concrete qui n'est pas implant�e par
		  // homonymie
		  internal_check_init(cur_var, machine->get_initialisation(), machine) ;
		}
	  else
		{
		  // Variable concrete implant�e par homonymie
		}
	  cur_var = (T_ident *)cur_var->get_next() ;
	}

  cur_var = machine->get_abstract_variables() ;
  while (cur_var != NULL)
	{
	  if ((cur_var->get_ref_glued() == NULL)
		  ||
		  (cur_var->get_ref_glued()->get_ref_machine()->get_specification() == spec))
		{
		  // variable abstraite qui n'est pas implant�e par homonymie,
		  // elle doit donc etre initialis�e
		  internal_check_init(cur_var, machine->get_initialisation(), machine) ;
		}
	  else
		{
		  // variable abstraite implant�e par homonymie
		}
	  cur_var = (T_ident *)cur_var->get_next() ;
	}

  EXIT_TRACE ;
  TRACE2("<- check_init_location(%x:%s)",
		 betree,
		 betree->get_betree_name()->get_value()) ;

  return betree ;
}


// Fonction qui rajoute une initialisation qui ne contient qu'un skip
// quand il n'y a pas d'initialisation dans la machine. (Correction Bug 2781)
extern void insert_blank_init(T_betree *betree)
{
  TRACE1("--> insert_blank_init (%x)", betree) ;
   T_machine *cur_mach = betree->get_root() ;

   if (cur_mach->get_initialisation_clause() == NULL)
	 {
	   TRACE1("Ajout d'une initialisation vide (%s)",
			  cur_mach->get_machine_name()->get_name()->get_value()) ;
	   // Dans le cas o� on n'a ni op�ration ni initialisation,
	   // il faut cr�er une initialisation vide
	   cur_mach->initialisation_clause = new T_flag(cur_mach,
													cur_mach->get_betree(),
													(T_lexem *)NULL) ;

	   T_ident *ident = new T_ident("INITIALISATION",
									ITYPE_BUILTIN,
									NULL,
									NULL,
									cur_mach->get_initialisation_clause(),
									cur_mach->get_betree(),
									(T_lexem *)NULL) ;

	   T_op *op = new T_op(ident,
						   // in params
						   (T_ident *)NULL,
						   (T_ident *)NULL,
						   // out params
						   (T_ident *)NULL,
						   (T_ident *)NULL,
						   // body
						   cur_mach->first_initialisation,
						   cur_mach->last_initialisation,
						   // first, last
						   (T_item **)NULL,
						   (T_item **)NULL,
						   // father
						   (T_item *)cur_mach->get_initialisation_clause(),
						   // betree
						   cur_mach->get_betree()) ;


	   ident->set_father(op) ;


	   T_item **adr_first = (T_item **)&cur_mach->first_initialisation ;
	   T_item **adr_last  = (T_item **)&cur_mach->last_initialisation ;

	   // On remplace le syntax_get_substitution :
	   //T_substitution *skip_substi = new T_skip(adr_first, adr_last, op, betree, NULL) ;
	   new T_skip(adr_first, adr_last, op, betree, NULL) ;
	   op->set_body((T_substitution *)*adr_first, (T_substitution *)*adr_last) ;

	   cur_mach->initialisation_clause_operation = op ;
	   cur_mach->initialisation_clause_ident = ident ;
	   ident->link() ; // car ident utilise par l'op initialisation_clause_operation
	   // et en champ propre initialisation_clause_ident

	   // Mise � jour du checksum de l'initialisation.
	   // On met 00000000... pour bien marquer que c'est une initialisation "virtuelle"
	   T_symbol *new_checksum = lookup_symbol("00000000000000000000000000000000", 32) ;
	   cur_mach->initialisation_clause_operation->set_body_checksum(new_checksum) ;

	 }

   TRACE1("<-- insert_blank_init (%x)", betree) ;
}



//
// }{ Fin du fichier
//
