/******************************* CLEARSY **************************************
* Fichier : $Id: v_ldreq.cpp,v 2.0 1999-03-15 11:49:19 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyseur semantique
*					Chargement des machines requises
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
RCS_ID("$Id: v_ldreq.cpp,v 1.18 1999-03-15 11:49:19 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"

// Fonction auxiliaire qui charge une machine requise
// (via le T_used_machine) dans le Betree
static T_betree *load_single_machine(T_used_machine *reference,
                                              const char* converterName,
											  int *adr_already)
{
  TRACE0("ici load_singled_machine") ;
  TRACE2("load_single_machine(%x : \"%s\")",
		 reference,
		 reference->get_name()->get_name()->get_value()) ;

  // On cherche si l'on trouve le Betree dans le disk manager
  T_betree *new_betree = betree_manager_get_betree(reference->get_pathname()) ;
  T_symbol *betree_name = reference->get_name()->get_name() ;

  if (new_betree != NULL)
	{
	  TRACE0("deja charge ! Mise a jour des liens") ;
	  *adr_already = TRUE ;
	  return new_betree ;
	}

  // Il faut faire l'analyse syntaxique ...
  *adr_already = FALSE ;

  // Recherche avec le suffixe "mch"
  char *input_file = get_1Ko_string() ;  // A FAIRE :: dynamique

  // Recherche avec le suffixe "mod"
  char *input_file2 = get_1Ko_string() ;  // A FAIRE :: dynamique
  sprintf(input_file, "%s.mch", reference->get_component_name()->get_value()) ;
  sprintf(input_file2, "%s.mod", reference->get_component_name()->get_value()) ;
  char *input_file3 = get_1Ko_string() ;  // A FAIRE :: dynamique
  sprintf(input_file3, "%s.sys", reference->get_component_name()->get_value()) ;

  TRACE2("il faut charger le betree dans le fichier %s ou %s",
		 input_file,
		 input_file2) ;

  new_betree = compiler_syntax_analysis(input_file,
                                        converterName,
										input_file2,
										input_file3,
										reference->get_ref_lexem()) ;


  // Il faut retrouver le betree Par construction, c'est le premier en
  // partant de la fin qui a le bon nom de composant
  new_betree = get_betree_manager()->get_last_betree() ;
  while (     (new_betree != NULL)
		   && (new_betree->get_betree_name() != reference->get_component_name()) )
	{
	  TRACE2("new_betree->get_betree_name %x:%s",
			 new_betree->get_betree_name(),
			 new_betree->get_betree_name()->get_value()) ;
	  TRACE2("reference->get_component_name %x:%s",
			 reference->get_component_name(),
			 reference->get_component_name()->get_value()) ;
	  new_betree = new_betree->get_prev_betree() ;
	}

  if (new_betree == NULL)
	{
	  // Retour d'erreur
	  TRACE0("ici RETOUR D'ERREUR") ;
	  set_exit_handler(NULL) ;
	  return NULL ;
	}

  // On met le nom complet avec des "."
  new_betree->set_betree_name(reference->get_name()->get_name()) ;

  TRACE2("APRES CHARGEMENT : reference  %s, betree %s",
		 (betree_name == NULL) ? "" : betree_name->get_value(),
		 (new_betree == NULL)
		 ? "NULL" : new_betree->get_betree_name()->get_value()) ;

  new_betree->set_tmp2(FALSE) ;

  if (new_betree == NULL)
	{
	  // Erreur : le composant requis est absent
	  semantic_error(reference,
					 CAN_CONTINUE,
					 get_error_msg(E_REQUIRED_COMPONENT_MISSING),
					 reference->get_name()->get_name()->get_value()) ;
	}

  return new_betree ;
}

// Fonction auxiliaire de parcours de liste les machines sont
// chargees, et on verifie qu'il s'agit de specifications
// On met a jour les ref_machine au passage
static int check_list(T_used_machine *list,
                               const char* converterName)
{
  TRACE0("check_list") ;
  T_used_machine *cur = list ;

  while (cur != NULL)
	{
	  cur->make_pathname() ;
	  int already ; // Pour savoir si la machine etait deja chargee
          T_betree *new_betree = load_single_machine(cur, converterName, &already) ;

	  if (new_betree == NULL)
		{
		  // Probleme de chargement !
		  TRACE0("PANIC ! probleme de chargement") ;
		  return FALSE ;
		}

	  // Mise a jour de la reference used_machine -> machine (ref_machine) ...
	  TRACE2("mise a jour du ref_machine de %x avec %x",
			 cur,
			 new_betree->get_root()) ;
	  cur->set_ref_machine(new_betree->get_root()) ;

	  // ... etc machine -> used_machine (context)
	  new_betree->get_root()->set_context(cur) ;

	  if (already == FALSE)
		{
		  // Mise a jour des liens
		  T_machine *mach = new_betree->get_root() ;
		  if (mach == NULL)
			{
			  return FALSE ;
			}
		  if (mach->get_machine_type() != MTYPE_SPECIFICATION &&
				  mach->get_machine_type() != MTYPE_SYSTEM)
			  {
				// On ne peut referencer qu'une specification
				// CTRL Ref : PROJ 1-1
				semantic_error(cur,
							   CAN_CONTINUE,
							   get_error_msg(E_REQ_COMP_NOT_A_SPEC)) ;
			  }

		  if (new_betree != NULL)
			{
                          new_betree = load_required_machines(new_betree, converterName) ;
			}

		}

	  // Mise a jour du lien "seen_by" ou "included_by" ou xxx_by
	  new_betree->add_list(cur->get_use_type(),
						   cur->get_betree(),
						   cur->get_ref_lexem()) ;

	  cur = (T_used_machine *)cur->get_next() ;
	}

  return TRUE ;
}

// Chargement des machines requises avec mise a jour des references
// (via les T_used_machine) dans le Betree
//
// CETTE FONCTION UTILISE LE CHAMP tmp2 COMME MARQUEUR POUR NE PAS
// S'EXECUTER DEUX FOIS SUR LE MEME COMPOSANT
//
// Si tmp2 == TRUE la fonction a deja ete appelee, on ne fait rien
// Si tmp2 == FALSE la fonction n'a jamais ete appelee : on fait le traitement,
// et on met tmp2 a TRUE
//
T_betree *load_required_machines(T_betree *betree,
                                          const char* converterName)
{
  TRACE2("DEBUT load_required_machines(%x,%s)",
		 betree, betree->get_betree_name()->get_value()) ;
  ENTER_TRACE ;

  if (betree->get_tmp2() != FALSE)
	{
	  // On a deja effectue cette operation pour ce betree
	  TRACE1("deja fait pour %s", betree->get_betree_name()->get_value()) ;
	  return betree ;
	}

  // Mise en place du marqueur qui dit que load_required_machines a
  // deja ete fait
  betree->set_tmp2(TRUE) ;

  T_machine *root = betree->get_root() ;

  // On charge l'abstraction, s'il y en a une
  if (root->get_abstraction_name() != NULL)
	{
	  T_betree *abstract = NULL ;

	  // On regarde si le betree n'a pas deja ete charge
	  TRACE1("recherche de l'abstraction %s",
			 root->get_abstraction_name()->get_name()->get_value()) ;
	  abstract =betree_manager_get_betree(root->get_abstraction_name()->get_name());
	  TRACE1("resultat = %x", abstract) ;

	  if (abstract == NULL)
		{
		  // On le charge
		  char *input_file_spec = get_1Ko_string() ;  // A FAIRE :: dynamique
		  sprintf(input_file_spec,
				  "%s.mch",
				  root->get_abstraction_name()->get_name()->get_value()) ;
		  char *input_file_ref = get_1Ko_string() ;  // A FAIRE :: dynamique
		  sprintf(input_file_ref,
				  "%s.ref",
				  root->get_abstraction_name()->get_name()->get_value()) ;
		  char *input_file_sys = get_1Ko_string() ;  // A FAIRE :: dynamique
		  sprintf(input_file_sys,
				  "%s.sys",
				  root->get_abstraction_name()->get_name()->get_value()) ;

                  compiler_syntax_analysis(input_file_ref, converterName, input_file_spec, input_file_sys) ;
		  // abstraction : il faut demander au betree manager car le
		  // fichier peut contenir plusieurs composants
		  //
		  abstract = betree_manager_get_betree(
				root->get_abstraction_name()->get_name()) ;
		}
	  if (abstract == NULL)
		{
		  // Erreur : le composant requis est absent
		  semantic_error(root->get_refines_clause(),
						 CAN_CONTINUE,
						 get_error_msg(E_REQUIRED_COMPONENT_MISSING),
						 root->get_abstraction_name()->get_name()->get_value()) ;
		}
	  else
		{
		  // Mise a jour de la reference
		  root->set_ref_abstraction(abstract->get_root()) ;
		  TRACE6("root %x:%s -> set_abstract %x:%s -> root %x:%s",
				 root,
				 root->get_machine_name()->get_name()->get_value(),
				 abstract,
				 abstract->get_betree_name()->get_value(),
				 abstract->get_root(),
				 abstract->get_root()->get_machine_name()->get_name()->get_value());
		  abstract->get_root()->set_ref_refinement(root) ;

                  abstract = load_required_machines(abstract, converterName) ;

		  T_machine *spec = betree->get_root()->get_ref_abstraction() ;

		  // Recherche de la specification
		  while (spec->get_ref_abstraction() != NULL)
			{
			  TRACE4("spec %x:%s -> get_ref_abstraction %x:%s",
					 spec,
					 spec->get_machine_name()->get_name()->get_value(),
					 spec->get_ref_abstraction(),
					 spec->get_ref_abstraction()
					 ->get_machine_name()->get_name()->get_value()) ;
			  if (spec == spec->get_ref_abstraction())
				{
				  // Reprise sur erreur (composant se raffine lui-meme,
				  // deja detecte dans v_compo.cpp (component_syntax_analysis)
				  TRACE0("ERREUR compo se raffine lui meme, on sort") ;
				  stop() ;
				}
			  spec = spec->get_ref_abstraction() ;
			  TRACE1("ici : spec = %x", spec) ;
			}

		  betree->get_root()->set_ref_specification(spec) ;
		  TRACE1("fin : spec = %x", spec) ;

		  T_machine_type root_type = root->get_machine_type() ;
		  T_machine_type abstract_type = abstract->get_root()->get_machine_type() ;

		  TRACE2("root_type %d, abstract_type %d", root_type, abstract_type) ;

		  // Verification du type de l'abstraction
		  if ( 	(root_type == MTYPE_REFINEMENT)
				&& (abstract_type == MTYPE_IMPLEMENTATION) )
			{
			  semantic_error(root->get_abstraction_name(),
							 CAN_CONTINUE,
							 get_error_msg(E_REF_COMP_NOT_A_SPEC)) ;
			  semantic_error_details(abstract->get_root(),
									 get_catalog(C_IT_IS_AN_IMPLEMENTATION)) ;
			}
		  else if ( 	  (abstract_type != MTYPE_SPECIFICATION)
						  && (abstract_type != MTYPE_REFINEMENT)
						  && (abstract_type != MTYPE_SYSTEM) )
			{
			  // Cas d'une implementation
			  semantic_error(root->get_abstraction_name(),
							 CAN_CONTINUE,
							 get_error_msg(E_REF_COMP_NEITHER_SPEC_NOR_REF)) ;
			  semantic_error_details(abstract->get_root(),
									 (abstract_type == MTYPE_IMPLEMENTATION)
									 ? get_catalog(C_IT_IS_AN_IMPLEMENTATION)
									 : get_catalog(C_IT_IS_A_REFINEMENT)) ;
			}
		} // Fin chargement de l'abstraction

	}

  // Il faut parcourir toutes les clauses
  // SEES INCLUDES IMPORTS EXTENDS USES

  // Parcours des listes
  check_list(root->get_sees(), converterName) ;
  check_list(root->get_includes(), converterName) ;
  check_list(root->get_imports(), converterName) ;
  check_list(root->get_extends(), converterName) ;
  check_list(root->get_uses(), converterName) ;

  EXIT_TRACE ;
  TRACE1("FIN   load_required_machines(%x)", betree) ;

  return betree ;
}

