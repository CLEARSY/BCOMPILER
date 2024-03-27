/******************************* CLEARSY **************************************
* Fichier : $Id: i_toplvl.cpp,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Interface du B0 Checker
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
RCS_ID("$Id: i_toplvl.cpp,v 1.28 2007-07-27 15:19:46 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "i_toplvl.h"
#include "i_valchk.h"
#include "i_subst.h"
#include "i_decl.h"

//
// Ensemble par defaut typant les constantes littérales entières
//
T_ident *literal_integer_type;
int literal_integer_type_checked = FALSE;

//
// Flag pour le B0 check des machines
//
#define set_B0_checked set_tmp2
#define get_B0_checked get_tmp2


extern void lex_unload(void);
//
// Référence avant
//
static T_B0_OM_status
B0_OM_tree_check(T_machine *cur_mach,
				 int static_part,
				 int dynamic_part,
				 T_B0_OM_status status);


static void betree_B0_check(T_betree *betree)
{  //
  // Traduction de tous les types des identificateurs en types B0
  //

  TRACE0(">> Creation des types B0 des identificateurs");
  ENTER_TRACE;

  // AB le 05/03 : la construction des types B0 ne doit se faire que s'il n'y a
  // plus d'erreurs.
  if (get_error_count() != 0)
	{
	  // On ne peut pas lancer la phase de construction des types B0
	  // si il reste des erreurs.
	  TRACE0("fin prematuree, Construction des types B0 impossible") ;
	  return;
	}

  // Avant ca mise a jour de tous les tmp2 des machines a 0
  // (c'est le compteur de type tableaux)
  // EFFET DE BORD : utilise le champ tmp2 des T_machine
  T_betree *cur_betree = get_betree_manager()->get_betrees() ;
  while (cur_betree != NULL)
	{
	  cur_betree->get_root()->set_tmp2(0) ;
	  cur_betree = cur_betree->get_next_betree() ;
	}

  T_ident *cur_ident = get_object_manager()->get_identifiers() ;

  while (cur_ident != NULL)
    {
      // On ne calcule les types B0 que pour les données concrètes
      if (cur_ident->is_a_concrete_ident() == TRUE)
	{
	  T_ident *cur_def = cur_ident->B0_get_definition();
	  T_type *cur_type = cur_def->get_original_B_type() ;
#ifdef DUMP_TYPES
	  TRACE1("cur_type %p", cur_type) ;
#endif
	  if (cur_type != NULL)
	    {
	      T_B0_type *b0_type = cur_type->get_B0_type() ;

	      if (b0_type == NULL)
		{
		  TRACE2("transformation du type %p de %s en B0",
			 cur_type,
			 cur_def->get_name()->get_value()) ;
		  ENTER_TRACE ;

		  // Cree b0type, l'accroche a cur_type,
		  // et met les liens adéquats
		  b0_type = cur_type->make_B0_type(cur_def,
						   FALSE,
						   NULL,
						   NULL) ;
		  if (b0_type != NULL)
		    {
		      // b0_type == NULL en cas d'erreur ou
		      // en cas de type non B0
		      b0_type->set_B_type(cur_type) ;
		      b0_type->set_father(cur_def) ;
		    }

		  EXIT_TRACE ;
		  TRACE2("fin transformation du type %x en B0 -> b0_type=%x",
			 cur_type, b0_type) ;
		}

	      // On met à jour le type B0 de l'ident que celui-ci soit
	      // nouveau ou déjà calculé auparavant.
	      cur_def->set_B0_type(b0_type) ;
	    }
	}

      cur_ident = cur_ident->get_next_ident() ;
    }

  EXIT_TRACE;
  TRACE0("<< Creation des types B0 des identificateurs");


#ifdef B0C
  // SL 30/08/1999 : partie deplacee apres le calcul des types B0 (was
  // : avant) car on a besoin des types B0 pour effectuer des
  // controles avances du type : definition de types pour HIA

  //
  // Verification B0 du typage des donnees concretes
  //
  T_betree *current_betree = get_betree_manager()->get_betrees() ;
  ASSERT(current_betree != NULL);
  while (current_betree != NULL)
	{
	  TRACE1("BETREE %s",current_betree->get_betree_name()->get_value());
	  ASSERT(current_betree->get_root() != NULL);

	  // Verifications semantiques sur le typage des constantes concretes
	  if (get_B0_disable_concrete_constants_type_check() != TRUE)
		{
		  current_betree->concrete_constant_type_check();
		}

	  // Verifications semantiques sur le typage des variables concretes
	  if (get_B0_disable_concrete_variables_type_check() != TRUE)
		{
		  current_betree->concrete_variable_type_check();
		}

	  // Verifications semantiques sur le typage des parametres scalaires de
	  // machine
	  if (get_B0_disable_formal_params_type_check() != TRUE)
		{
		  current_betree->scalar_parameter_type_check();
		}

	  // Controles sur les valuations des constantes concretes et des ensembles
	  // abstraits.
	  // Plus controle sur l'existence de cycles dans les valuations
	  if (get_B0_disable_valuation_check() != TRUE)
		{
		  current_betree->valuation_check(NULL);
		}

	  // Controles sur les instanciations de parametres formels de machine
	  if (get_B0_disable_parameter_instanciation_check() != TRUE)
		{
		  current_betree->instanciation_check();
		}

	  // Controles sur l'existence de cycles dans les initialisations
	  if (get_B0_disable_variables_initialisation_check() != TRUE)
		{
		  current_betree->initialisation_check(NULL);
		}

	  // vérification B0 des opérations
	  current_betree->operation_B0_check();

	  current_betree = current_betree->get_next_betree() ;
	}

#endif // B0C

}

/***************************************************************************
*
* FONCTIONS de controle du B0_OM
*
*
***************************************************************************/

//
// Fonction de controle des pragma inline
//
static void B0_OM_pragma_inline_check(T_machine *mach)
{
  T_pragma *inline_pragma = mach->lookup_pragma(lookup_symbol("INLINE"),
												NULL);

  T_pragma *inline_all_pragma =
	mach->lookup_pragma(lookup_symbol("INLINE_ALL"),
						NULL);

  if (mach->get_machine_type() == MTYPE_REFINEMENT
	  &&
	  (inline_pragma != NULL
	   ||
	   inline_all_pragma != NULL))
	{
	  B0_semantic_error(inline_pragma,
						CAN_CONTINUE,
						get_error_msg(E_B0_INLINE_ONLY_IN_IMPL_OR_MACH));
	}
  if (inline_pragma != NULL
	  &&
	  inline_all_pragma != NULL)
	{
	  B0_semantic_error(inline_pragma,
						CAN_CONTINUE,
						get_error_msg(E_B0_INLINE_AND_INLINE_ALL));
	}
  // On vérifie qu'il y en a au plus un
  T_pragma *secund = mach->lookup_pragma(lookup_symbol("INLINE"),
										 inline_pragma);
  if (secund != NULL)
	{
	  B0_semantic_error(secund,
						CAN_CONTINUE,
						get_error_msg(E_B0_REDEFINITION_OF_INLINE));
	}

  secund = mach->lookup_pragma(lookup_symbol("INLINE_ALL"),
							   inline_all_pragma);
  if (secund != NULL)
	{
	  B0_semantic_error(secund,
						CAN_CONTINUE,
						get_error_msg(E_B0_REDEFINITION_OF_INLINE_ALL));
	}

  // traitement du pragma éventuel INLINE
  if (inline_pragma != NULL)
	{
	  // On traite les paramètres on recherche la 'définition' du paramètre
	  T_expr *cur_param = inline_pragma->get_params();
	  if (cur_param == NULL)
		{
		  B0_semantic_error(inline_pragma,
							CAN_CONTINUE,
							get_error_msg(E_B0_INLINE_SHOULD_PARAM));
		}
	  while (cur_param != NULL)
		{
		  if (cur_param->is_an_ident() == TRUE)
			{
			  T_ident *op_name = cur_param->make_ident();
			  T_generic_op *op;

			  // Suivant le type de composant ont inline une opération
			  // locale ou non
			  const char *message;
			  if (mach->get_machine_type() == MTYPE_IMPLEMENTATION)
				{
				  op = mach->find_local_op(op_name->get_name());
				  message = get_error_msg(E_B0_IS_NOT_OP_FROM_IMPL);
				}
			  else
				{
				  op = mach->find_op(op_name->get_name());
				  message = get_error_msg(E_B0_IS_NOT_OP_FROM_MCH);
				}
			  if (op == NULL)
				{
				  B0_semantic_error(op_name,
									CAN_CONTINUE,
									message,
									op_name->get_name()->get_value(),
									mach->get_machine_name()->get_name()->get_value());
				}
			  else
				{
				  TRACE1("mise à jours inline op %s",
						 op->get_op_name()->get_value());
				  ((T_op *)op)->set_is_inlined(TRUE);
				  op_name->set_def(op->get_name());
				}
			}

		  cur_param = (T_expr *)cur_param->get_next();
		}
	}

  // traitement du pragma éventuel INLINE_ALL

  if (inline_all_pragma != NULL)
	{
	  T_op *op;
	  if (mach->get_machine_type() == MTYPE_IMPLEMENTATION)
		{
		  op = mach->get_local_operations();
		}
	  else
		{
		  op = mach->get_operations();
		}
	  while (op != NULL)
		{
		  op->set_is_inlined(TRUE);
		  op = (T_op *) op->get_next();
		}
	}
}

//
// Fonction d'évaluationn du status d'un module
//

static T_B0_OM_status
B0_OM_status_module_check(T_machine *cur_mach,
						  T_B0_OM_status cur_status)
{
  T_B0_OM_status new_status = cur_status;

  if (cur_status == B0_OM_UNDETERMINED_ST)
	{
	  if (cur_mach->get_machine_type() == MTYPE_IMPLEMENTATION)
		{
		  new_status = B0_OM_CONCRETE_ST;
		}
	  else if (cur_mach->get_concrete_variables() != NULL
			   ||
			   cur_mach->get_concrete_constants() != NULL)
		{
		  // Le module est concret
		  new_status = B0_OM_CONCRETE_ST ;
		}
	  else
		{
		  // Il faut tester si une opération a un statut non
		  // ambigue.

		  T_op *cur_op = cur_mach->get_operations();

		  while (cur_op != NULL
				 &&
				 new_status == B0_OM_UNDETERMINED_ST)
			{
			  new_status = cur_op->get_status();
			  cur_op = (T_op *)cur_op->get_next();
			}
		}
	}

  return new_status;
}

//
// Fonction de vérification d'une liste d'ident
//

static void B0_OM_ident_list_check(T_ident *first_ident)
{
  T_ident *cur_ident = first_ident;
  while (cur_ident != NULL)
	{
	  cur_ident->B0_check_decl();
	  cur_ident = (T_ident *) cur_ident->get_next();
	}
}

//
// Fonctionde vérification des entetes d'opérations
//
static void B0_OM_operations_header_check(T_list_link *first_op,
										  T_B0_OM_status expec_status)
{
  T_list_link *cur_linked_op = first_op;
  while (cur_linked_op != NULL)
	{
	  T_generic_op *op = (T_generic_op *)cur_linked_op->get_object();
	  op->B0_OM_check_header_decl(expec_status);
	  cur_linked_op = (T_list_link *)cur_linked_op->get_next();
	}
}

//
// Fonction de vérification d'une liste de T_used_machine
//
static T_B0_OM_status
B0_OM_used_machines_check (T_used_machine *first_used_mach,
						   int static_part,
						   int dynamic_part,
						   T_B0_OM_status ext_status)
{
  TRACE0("B0_OM_used_machines_check");
  T_B0_OM_status cur_status = ext_status;
  T_B0_OM_status glob_status = ext_status;
  T_used_machine *cur_used_mach = first_used_mach;
  while (cur_used_mach != NULL)
	{
	  T_machine *mach = cur_used_mach->get_ref_machine();
	  cur_status = B0_OM_tree_check(mach,
									static_part,
									dynamic_part,
									ext_status);
	  if (cur_status == B0_OM_CONCRETE_ST)
		{
		  glob_status = cur_status;
		}
	  cur_used_mach = (T_used_machine *)cur_used_mach->get_next();
	}

  return glob_status;
}

//
// Fonction de vérification B0_OM pour un sous arbre d'un graphe local
//
static T_B0_OM_status
B0_OM_tree_check(T_machine *cur_mach,
						  int static_part,
						  int dynamic_part,
						  T_B0_OM_status ext_status)
{
  TRACE1("B0_OM_tree_check(%s)",
		 cur_mach->get_machine_name()->get_name()->get_value());

  if (cur_mach->get_B0_checked() == TRUE)
	{
	  return ext_status;
	}
  if (dynamic_part == TRUE)
	{
	  cur_mach->set_B0_checked(TRUE);
	}

  T_B0_OM_status loc_status = ext_status;

  // Appel sur l'abstraction
  T_machine *abs_mach = cur_mach->get_ref_abstraction();
  if (abs_mach != NULL)
	{
	  loc_status = B0_OM_tree_check(abs_mach,
									TRUE,
									TRUE,
									loc_status);
	}

  // les machines incluses
  T_B0_OM_status incl_status;
  incl_status = B0_OM_used_machines_check(cur_mach->get_includes(),
										  static_part,
										  dynamic_part,
										  B0_OM_UNDETERMINED_ST);

  // les machines étendues au sens de l'inclusion
  incl_status = B0_OM_used_machines_check(cur_mach->get_extends(),
										  static_part,
										  dynamic_part,
										  B0_OM_UNDETERMINED_ST);

  // Evaluation du status de la machine courante
  loc_status = B0_OM_status_module_check(cur_mach,
										 loc_status);

  // Si le status du composant courant est déterminé on vérifie qu'il
  // y a compatibilité avec les inclusions
  if (incl_status == B0_OM_CONCRETE_ST)
	{
	  if (loc_status == B0_OM_ABSTRACT_ST)
		{
		  B0_semantic_error (cur_mach,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_ABST_MACH_AND_CONC_INCL),
							 cur_mach->get_machine_name()->
							    get_name()->get_value());
		}
	  else
		{
		  loc_status = B0_OM_CONCRETE_ST;
		}
	}

  // les machines vues
  B0_OM_used_machines_check(cur_mach->get_sees(),
							TRUE,
							TRUE,
							// Un module vu est concidéré concret
							B0_OM_CONCRETE_ST);

  // VERIFICATIONS LOCALES
  // Partie statique
  if (static_part == TRUE)
	{
	  // Control que le nom 'a pas de '__'
	  B0_OM_name_check(cur_mach->get_machine_name());

	  // Vérification des ensembles
	  B0_OM_ident_list_check(cur_mach->get_sets());

	  // Vérification des constantes concrètes
	  B0_OM_ident_list_check(cur_mach->get_concrete_constants());
	}
  if (dynamic_part == TRUE)
	{
	  // Vérification des paramètres de machine
	  // Interdit en BOM
	  if (cur_mach->get_params() != NULL)
		{
		  B0_semantic_error(cur_mach,
							CAN_CONTINUE,
							get_error_msg(E_B0_PARAM_FORBI_BOM));
		}
	  // Les pragama inline
	  B0_OM_pragma_inline_check(cur_mach);

	  // Fonction de vérification des variables concrètes
	  B0_OM_ident_list_check(cur_mach->get_concrete_variables());

	  // Vérification des entetes d'opérations
	  B0_OM_operations_header_check(cur_mach->get_op_list(),
									loc_status);

	}
  return loc_status;
}

//
// Fonction de  vérification des corps d'opérations
//
static void B0_OM_operations_body_check(T_op *first_op)
{
  T_op *cur_op = first_op;
  while (cur_op != NULL)
	{
	  cur_op->B0_OM_check_body_decl();
	  cur_op = (T_op *)cur_op->get_next();
	}
}

//
// Fonction de vérification de la partie statique d'une machine
//

void B0_OM_static_check(T_machine *mach)
{

  // Aucune vérification sur les ensembles

  // Vérification des machines vues
  T_used_machine *cur_used_mach = mach->get_sees();
  while (cur_used_mach != NULL)
	{
	  B0_OM_static_check(cur_used_mach->get_ref_machine());
	  cur_used_mach = (T_used_machine *)cur_used_mach->get_next();
	}

}


//
// Fonction de vérification d'une machine racine d'un graph local
// C'est le point d'entrée des vérificationn B0 d'un module.  Le
// statut peut être forcé par l'appelant. C'est le cas pour un
// traducteur qui sait si le module est un module de base.
//

static void betree_B0_OM_check(T_betree *betree,
										int must_be_concrete)
{
  TRACE0("betree_B0_check");
  T_machine *root = betree->get_root();

  if (get_error_count() != 0)
	{
	  // On ne peut pas lancer la phase de construction des types B0
	  // si il reste des erreurs.
	  TRACE0("fin prematuree, Construction des types B0 impossible") ;
	  return;
	}

  // Posiotionnement des flags de B0_check pour éviter de crier
  // plusieurs fois dans le cas de machines renommées et utilisée deux
  // fois
  T_betree *cur_betree = get_betree_manager()->get_betrees() ;
  while (cur_betree != NULL)
	{
	  cur_betree->get_root()->set_B0_checked(FALSE) ;
	  cur_betree = cur_betree->get_next_betree() ;
	}

  // Vérification B0 du nom de la machine: le nom de doit pas avoir de
  // '__'
  B0_OM_name_check (root->get_machine_name());

  // Vérification des paramètres de machine
  // Interdit en BOM
  if (root->get_params() != NULL)
	{
	  B0_semantic_error(root,
						CAN_CONTINUE,
						get_error_msg(E_B0_PARAM_FORBI_BOM));
	}

  T_B0_OM_status loc_status = B0_OM_UNDETERMINED_ST;
  // Si la machine courante est une implémentation ou si c'est imposé
  // par le client alors on est sur du concret
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION
	  ||
	  must_be_concrete == TRUE)
	{
	  loc_status = B0_OM_CONCRETE_ST;
	}

  // Type check de l'abstraction
  T_machine *abs_mach = root->get_ref_abstraction();
  if (abs_mach != NULL)
	{
	  // Si le status est connu avant d emonté sur l'abstraction alors
	  // il nous redescant inchangé sinon on récupère le status de la
	  // colonne de raffinnement
	  loc_status = B0_OM_tree_check(abs_mach,
									TRUE, // partie static
									TRUE, // partie dynamic
									loc_status); // Status connu à ce point
	}

  // Type check des machine Vues
  B0_OM_used_machines_check(root->get_sees(),
							// parie static
							TRUE,
							// partie dynamic
							TRUE,
							// Le module est concret
							B0_OM_CONCRETE_ST);

  // Type check de la partie static des machines importées
  B0_OM_used_machines_check(root->get_imports(),
							TRUE, // partie static
							FALSE, //partie dynamic
							// Le module est cocret
							B0_OM_CONCRETE_ST);

  // Le statut du composant n'influe pas sur le statut de l'inclusion
  T_B0_OM_status incl_status = B0_OM_UNDETERMINED_ST;
  incl_status = B0_OM_used_machines_check(root->get_includes(),
										  TRUE, // partie static
										  TRUE, //partie dynamic
										  // L'inclusion ne détermine
										  // pas le status de la
										  // machine inclue
										  B0_OM_UNDETERMINED_ST);


  // Les extends en implémentation équivalent à l'import =>
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  B0_OM_used_machines_check(root->get_extends(),
						   TRUE, // partie static
						   FALSE, //partie dynamic
							// Le module est concret
							B0_OM_CONCRETE_ST);
	}
  else
	{
	  incl_status =
		B0_OM_used_machines_check(root->get_extends(),
								  TRUE, // partie static
								  TRUE, //partie dynamic
								  // L'inclusion ne détermine pas le
								  // status de la machine inclue
								  B0_OM_UNDETERMINED_ST);
	  if (incl_status == B0_OM_CONCRETE_ST)
		{
		  loc_status = B0_OM_CONCRETE_ST;
		}
	}

  // On n'a maintenant le maximum d'information pour évaluer le status
  // de la machine courante. Cependant le status peut etre encore
  // indéterminé
  loc_status = B0_OM_status_module_check(root, loc_status);

  // Si le status du composant courant est déterminé on vérifie qu'il
  // y a compatibilité avec les inclusions
  if (incl_status == B0_OM_CONCRETE_ST)
	{
	  if (loc_status == B0_OM_ABSTRACT_ST)
		{
		  B0_semantic_error (root,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_ABST_MACH_AND_CONC_INCL),
							 root->get_machine_name()->
							    get_name()->get_value());
		}
	  else
		{
		  loc_status = B0_OM_CONCRETE_ST;
		}
	}

  // Controle des pragmas inline
  B0_OM_pragma_inline_check(root);

  // Vérification des ensembles
  B0_OM_ident_list_check(root->get_sets());

  // Controle B0 sur les valuations d'ensembles
  B0_OM_sets_valuation_check(root->get_values());

  // Controle B0 sur les constantes concrètes
  B0_OM_ident_list_check(root->get_concrete_constants());

  // Controle B0 sur les valuations de constantes
  B0_OM_constants_valuation_check(root->get_values());


  // IMPORTS.2
  // Type check de la partie dynamique des machines importées
  B0_OM_used_machines_check(root->get_imports(),
						   FALSE, // partie static
						   TRUE, //partie dynamic
						   B0_OM_CONCRETE_ST);


  // Les extends en implémentation équivalent à l'import =>
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  B0_OM_used_machines_check(root->get_extends(),
							   FALSE, // partie static
							   TRUE, //partie dynamic
							   B0_OM_CONCRETE_ST);
	}

  // Controle B0 sur les variables concrètes
  B0_OM_ident_list_check(root->get_concrete_variables());

  // L'initialisation en implémentation doit etre du B0
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  B0_check_substitution_list(root->get_initialisation());
	}
  // Les entetes d'opérations
  B0_OM_operations_header_check(root->get_op_list(),
								loc_status);

  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  // Les corps d'opérations en implémentation
	  B0_OM_operations_body_check(root->get_operations());
	}
}

//
// Fonction pour la compatibilité assendante
//
T_betree *compiler_B0_check(T_betree *betree)
{
  return compiler_B0_check(betree,FALSE);
}

// Fonction qui fait l'analyse semantique d'un Betree syntaxique
// Le Betree enrichi est retourne en cas de succes. En cas d'erreur,
// la fonction renvoie NULL
T_betree *compiler_B0_check(T_betree *betree,
									 int must_be_concrete)
{
  TRACE2("DEBUT B0 CHECK (%x:%s)",
		 betree,
		 betree->get_file_name()->get_value()) ;

  ENTER_TRACE ; ENTER_TRACE ;

  if (betree->get_status() == BST_B0_CHECKED)
	{
	  // Deja fait
	  return betree ;
	}
  else if (betree->get_status() != BST_SEMANTIC)
	{
	  // On ne peut pas lancer la phase de B0 Check
	  TRACE0("fin prematuree, Betree non semantique") ;
	  return betree ;
	}

  verbose_describe(get_catalog(C_B0_CHECK),
				   betree->get_root()->get_machine_name()->get_name()->get_value()) ;

  if (get_no_exit_mode() == TRUE)
	{
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans compiler_syntax_analysis") ;
		  TRACE0("debut menage") ;
		  lex_unload() ;
		  lex_unlink_lex_stacks() ;
		  lex_unlink_def_stacks() ;
		  TRACE0("fin menage") ;
		  return NULL ;
		}

	  set_exit_handler(&env) ;
	}

  if (get_Use_B0_Declaration() == FALSE)
	{
	  betree_B0_check(betree);
	}
  else
	{
	  betree_B0_OM_check(betree, must_be_concrete);
	}


  EXIT_TRACE ; EXIT_TRACE ;
  TRACE2("FIN B0 CHECK (%x:%s)", betree, betree->get_file_name()->get_value()) ;

  betree->set_status(BST_B0_CHECKED) ;

  if (get_no_exit_mode() == TRUE)
	{
	  set_exit_handler(NULL) ;
	}

  return betree ;
}

//
// Fin du fichier
//
