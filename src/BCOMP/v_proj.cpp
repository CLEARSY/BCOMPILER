/******************************* CLEARSY **************************************
* Fichier : $Id: v_proj.cpp,v 2.0 2002-02-13 15:55:53 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique
*					Controles au niveau d'un projet
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
RCS_ID("$Id: v_proj.cpp,v 1.15 2002-02-13 15:55:53 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_toplvl.h"
#include "v_aux.h"

// Verifier que toutes les machines vues par l'abstraction
// sont vues localement
// CTRL Ref : PROJ 2-6
static void check_sees(T_machine *root,
								T_machine *abstract)
{
  T_used_machine *cur_abs_sees = abstract->get_sees() ;
  while (cur_abs_sees != NULL)
	{
	  T_symbol *name = cur_abs_sees->get_name()->get_name() ;
	  T_used_machine *find = find_used_machine(name, root->get_sees()) ;

	  if (find == NULL)
		{
		  TRACE1("ici : cur_abs_sees %x", cur_abs_sees) ;
		  TRACE1("ici : cur_abs_sees->betree %s",
				 cur_abs_sees->get_betree()->get_file_name()->get_value()) ;

		  semantic_error((root->get_sees_clause() == NULL)
						 ? (T_item *)root : (T_item *)root->get_sees_clause(),
						 CAN_CONTINUE,
						 get_error_msg(E_REF_DOES_NOT_DUPLICATE_SEES),
						 name->get_value(),
						 name->get_value(),
						 root->get_abstraction_name()->get_name()->get_value()) ;
		  semantic_error(cur_abs_sees,
						 MULTI_LINE,
						 get_error_msg(E_SEES_LOCATION),
						 name->get_value()) ;
		}
	  cur_abs_sees = (T_used_machine *)cur_abs_sees->get_next() ;
	}
}

//GP 14/01/99
//Retourne TRUE si name appartient à la liste "machine_list",
//FALSE sinon.
static int is_an_INCLUDES_composant(T_symbol* name,
											 T_used_machine* machine_list)
{
  while(machine_list != NULL)
	{
	  T_symbol* machine_name = machine_list->get_name()->get_name() ;
	  if(name->get_value() == machine_name->get_value())
		{
		  return TRUE ;
		}

	  //Cast vrai par construction
	  machine_list=(T_used_machine*)machine_list->get_next() ;
	}
  return FALSE ;
}

// CTRL Ref : PROJ 2-8
//Verifier que si M1 INCLUDES M2 USES M3 alors M1 INCLUDES M3
static void check_includes_uses(T_machine *root)
{
  // Pas de INCLUDES possible dans une implémentation.
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  return ;
	}

  //Parcours de toutes les machines incluses et extends
  //pour creer la liste des machines qu'elle USES
  //Parcours en 2 passes 1=0..1, pour les includes puis les extends
  T_used_machine* cur_includes = root->get_includes() ;
  for(int i=0 ; i<2 ; i++)
	{
	  //Parcours de toutes les machine de la clause INCLUDES
	  //(resp EXTENDS) pour recuperer les machines qu'elles USES
	  while(cur_includes != NULL)
		{
		  //cur_machine = M2
		  T_machine* cur_machine=cur_includes->get_ref_machine() ;

		  //Parcours de toutes les machines USES
		  //et verification qu'elles sont dans la clause
		  //EXTENDS ou INCLUDES de root
		  //cur_uses = M3
		  T_used_machine* cur_uses=cur_machine->get_uses() ;
		  while(cur_uses != NULL)
			{
			  if(is_an_INCLUDES_composant(cur_uses->get_name()->get_name(),
										  root->get_includes()) == FALSE)
				{
				  if(is_an_INCLUDES_composant(cur_uses->get_name()->get_name(),
											  root->get_extends()) == FALSE)
					{
					  T_item* clause = root->get_includes_clause() ;
					  if(clause == NULL)
						{
						  clause = root->get_extends_clause() ;
						}
					  if(clause == NULL)
						{
						  clause = root->get_end_machine() ;
						}

					  semantic_error(clause,
									 CAN_CONTINUE,
									 get_error_msg(E_MACHINE_MUST_INCLUDE_USED_MACHINE),
									 root->get_machine_name()->get_name()->get_value(),
									 cur_includes->get_name()->get_name()->get_value(),
									 cur_uses->get_name()->get_name()->get_value(),
									 root->get_machine_name()->get_name()->get_value(),
									 cur_uses->get_name()->get_name()->get_value()
									 ) ;
					}
				}
			  //Cast vrai par construction
			  cur_uses=(T_used_machine*)cur_uses->get_next() ;
			}
		  //Cast vrai par construction
		  cur_includes=(T_used_machine*)cur_includes->get_next() ;
		}

	  //Deuxieme passe pour la clause EXTENDS de M1
	  cur_includes = root->get_extends() ;
	}
}

// Fonction auxiliaire
// Verifier qu'aucune machine effectuant un USES n'est
// referencee dans la clause xxx
// Parametres : la machine, la liste, le message d'erreur
static void check_XXX_uses(T_machine *root,
									T_used_machine *list,
									T_error_code error_msg)
{
  TRACE1("check_XXX_uses(%x)", root) ;

  T_used_machine *cur_item = list ;

  while (cur_item != NULL)
	{
	  TRACE5("cur_item %x:%s %s:%d:%d",
			 cur_item,
			 cur_item->get_name()->get_name()->get_value(),
			 cur_item->get_ref_lexem()->get_file_name()->get_value(),
			 cur_item->get_ref_lexem()->get_file_line(),
			 cur_item->get_ref_lexem()->get_file_column()) ;
	  T_machine *imp = cur_item->get_ref_machine() ;

	  TRACE2("machine %x avec%s",
			 imp,
			 imp->get_machine_name()->get_name()->get_value()) ;

	  TRACE2("cur_item %s : uses %x",
			 imp->get_machine_name()->get_name()->get_value(),
			 imp->get_uses()) ;
	  if (imp->get_uses() != NULL)
		{
		  semantic_error(cur_item,
						 CAN_CONTINUE,
						 get_error_msg(error_msg),
						 imp->get_machine_name()->get_name()->get_value()) ;
		  clause_location_error(imp->get_uses(), "USES") ;
		}

	  cur_item = (T_used_machine *)cur_item->get_next() ;
	}
}

// Verifier qu'aucune machine effectuant un USES n'est
// referencee dans IMPORTS
// CTRL Ref : PROJ 1-6
static void check_imports_uses(T_machine *root)
{
  check_XXX_uses(root, root->get_imports(), E_CAN_NOT_IMPORT_A_MACHINE_THAT_USES) ;
}

// Verifier qu'aucune machine effectuant un USES n'est
// referencee dans EXTENDS d'une implémentation
// CTRL Ref : PROJ 1-7
static void check_extends_uses(T_machine *root)
{
  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  check_XXX_uses(root,
					 root->get_extends(),
					 E_CAN_NOT_EXTEND_A_MACHINE_THAT_USES) ;
	}
}

// Verifier qu'aucune machine effectuant un USES n'est
// referencee dans SEES
// CTRL Ref : PROJ 1-8
static void check_sees_uses(T_machine *root)
{
  check_XXX_uses(root, root->get_sees(), E_CAN_NOT_SEE_A_MACHINE_THAT_USES) ;
}

// Verifier que la liste ordonnee des parametres formels
// est identique a celle de l'abstraction
// CTRL Ref : PROJ 2-2
static void check_params(T_machine *root, T_machine *abstract)
{
  TRACE4("check_params(root = %x:%s, abstract = %x:%s)",
		 root,
		 root->get_machine_name()->get_name()->get_value(),
		 abstract,
		 abstract->get_machine_name()->get_name()->get_value()) ;

  if (abstract->get_ref_abstraction() != NULL)
	{
	  check_params(abstract, abstract->get_ref_abstraction()) ;
	}

  T_ident *cur_param = root->get_params() ;
  T_ident *abstract_param = abstract->get_params() ;
  size_t count = 0 ; // compteur du numero de parametre

  for (;;)
	{
	  count ++ ;
	  TRACE5("count %d : cur %x %s, abstract %x %s",
			 count,
			 cur_param,
			 (cur_param == NULL) ? "NULL" : cur_param->get_name()->get_value(),
			 abstract_param,
			 (abstract_param == NULL)
			 ? "NULL" : abstract_param->get_name()->get_value()) ;
	  if ( (cur_param == NULL)  && (abstract_param == NULL) )
		{
		  // Fin du test !
		  return ;
		}

	  if (cur_param == NULL) // donc abstract_param != NULL
		{
		  // Le parametre count de l'abstraction n'est pas implemente
		  semantic_error(root,
						 CAN_CONTINUE,
						 get_error_msg(E_PARAM_N_IS_NOT_REFINED),
						 count,
						 abstract_param->get_name()->get_value()) ;
		  semantic_error_details(abstract_param,
								 get_error_msg(E_LOCATION_OF_PARAM_N_IN_ABSTRACTION),
								 count) ;
		}
	  else if (abstract_param == NULL) // donc cur_param != NULL
		{
		  // Le parametre count n'existe pas dans l'abstraction
		  semantic_error(cur_param,
						 CAN_CONTINUE,
						 get_error_msg(E_PARAM_N_IS_NOT_SPECIFIED),
						 count,
						 cur_param->get_name()->get_value()) ;
		  semantic_error_details(abstract,
								 get_error_msg(E_LOCATION_OF_ABSTRACT_PARAMS)) ;
		}
	  else
		{
		  if (cur_param->get_name()->compare(abstract_param->get_name()) == FALSE)
			{
			  semantic_error(cur_param,
							 CAN_CONTINUE,
							 get_error_msg(E_PARAM_N_DIFFERS),
							 count,
							 cur_param->get_name()->get_value(),
							 count,
							 abstract_param->get_name()->get_value()) ;
			  semantic_error_details(abstract_param,
									 get_error_msg(
												   E_LOCATION_OF_PARAM_N_IN_ABSTRACTION),
									 count) ;
			}
		  else
			{
			  // C'est bon, on peut mettre le def a jour
			  TRACE2("ici mise a jour du def de %x avec %x", cur_param, abstract_param) ;
			  cur_param->set_def(abstract_param) ;
			  cur_param->set_homonym_in_abstraction(abstract_param);
			}
		}

	  // On avance !
	  if (abstract_param != NULL)
		{
		  abstract_param = (T_ident *)abstract_param->get_next() ;
		}
	  if (cur_param != NULL)
		{
		  cur_param = (T_ident *)cur_param->get_next() ;
		}
	}
}

// Verifier que l'abstraction ne fait pas de uses
// CTRL Ref : PROJ 1-9
static void check_abstract_has_no_uses(T_machine *root,
												T_machine *abstract)
{
  TRACE1("check_abstract_has_no_uses(%x)", root) ;

  if (abstract->get_uses() != NULL)
	{
	  semantic_error(root->get_abstraction_name(),
					 CAN_CONTINUE,
					 get_error_msg(E_CAN_NOT_REFINE_A_MACHINE_THAT_USES),
					 abstract->get_machine_name()->get_name()->get_value()) ;
	  clause_location_error(abstract->get_uses(), "USES") ;
	}
}

// Verifications de type projet locales a un composant
static T_betree *local_analysis(T_betree *betree)
{
  TRACE1("DEBUT local_analysis(%x)", betree) ;
  ENTER_TRACE ;

  T_machine *root = betree->get_root() ;
  T_machine *abstract = root->get_ref_abstraction() ;

  if (abstract != NULL)
	{
	  // Verifier que toutes les machines vues par l'abstraction
	  // sont vues localement
	  // Ref : PROJ-XX-YY
	  check_sees(root, abstract) ;
	}

  // Verifier qu'aucune machine effectuant un USES n'est
  // referencee dans IMPORTS
  // Ref : PROJ-XX-YY
  check_imports_uses(root) ;

  // Verifier qu'aucune machine effectuant un USES n'est
  // referencee dans EXTENDS d'une implémentation
  // Ref : PROJ-XX-YY
  check_extends_uses(root) ;

  // Verifier qu'aucune machine effectuant un USES n'est
  // referencee dans SEES
  // Ref : PROJ-XX-YY
  check_sees_uses(root) ;

  //GP 14/1/99
  // Verifier que si M1 INCLUDE M2 USES M3
  //alors M1 INCLUDE M3
  //Ref : PROJ 2-8
  check_includes_uses(root) ;

  EXIT_TRACE ;
  TRACE1("FIN   local_analysis(%x)", betree) ;
  return betree ;
}

// Fonction auxiliaire :
// Verifications globales a un developpement vertical
static T_betree *vertical_analysis(T_betree *betree)
{
  TRACE1("DEBUT vertical_analysis(%x)", betree) ;
  ENTER_TRACE ;

  T_machine *root = betree->get_root() ;
  T_machine *abstract = root->get_ref_abstraction() ;

  if (abstract != NULL)
	{
	  // Verifier que la liste ordonnee des parametres formels
	  // est identique a celle de l'abstraction
	  // Ref : PROJ-XX-YY
	  check_params(root, abstract) ;

	  // Verifier que l'abstraction ne fait pas de uses
	  // Ref : PROJ-XX-YY
	  check_abstract_has_no_uses(root, abstract) ;
	}

  EXIT_TRACE ;
  TRACE1("FIN   vertical_analysis(%x)", betree) ;
  return betree ;
}

// Fonction auxiliaire :
// Verifications globales a un developpement vertical
static void global_project_analysis(void)
{
  TRACE0("DEBUT global_project_analysis(%x)") ;

#if 0 // SL 21/09/98 non en plus pose des problemes pour les
  // chargement de plusieurs Betree non liberes (LC)
  // On verifie qu'une instance de machine n'est pas importee
  // plusieurs fois dans le projet
  //
  //	A FAIRE :: etudier
  //
  // comme on ne peut importer que depuis une implementation, ce
  // test n'a aucune chance d'aboutir ?
  //
  T_betree *cur = get_betree_manager()->get_betrees() ;

  while (cur != NULL)
	{
	  T_betree_list *cur_list = cur->get_imported_by() ;
	  if ( 	(cur_list->get_links() != NULL)					// au moins 1 elem
			&& (cur_list->get_links()->get_next() != NULL) )	// au moins 2 elem
		{
		  const char *name = cur->get_file_name()->get_value() ;
		  toplevel_error(CAN_CONTINUE,
						 get_error_msg(E_MULTIPLE_IMPORTATION_OF_COMPONENT),
						 name) ;

		  T_list_link *pb = cur_list->get_links() ;

		  while (pb != NULL)
			{
			  semantic_error_details(pb,
									 get_error_msg(E_LOCATION_OF_COMPONENT_IMPORT),
									 name) ;

			  pb = (T_list_link *)pb->get_next() ;
			}
		}
	  cur = cur->get_next_betree() ;
	}

  TRACE0("FIN   global_project_analysis(%x)") ;
#endif
}

// Analyse semantique : controles au niveau projet
T_betree *project_analysis(T_betree *betree)
{
  if (betree->get_root()->get_project_checked() == TRUE)
	{
	  TRACE1("Projet déjà analysé (%x)",betree) ;
	  return betree ;
	}
  else
	{
	  betree->get_root()->set_project_checked(TRUE) ;
	  TRACE1("DEBUT project_analysis(%x)", betree) ;
	  ENTER_TRACE ;

	  // Verifications locales a un composant
	  betree = local_analysis(betree) ;

	  // Verifications globales a un developpement vertical
	  betree = vertical_analysis(betree) ;

	  // Verifications sur les operations
	  // Effet de bord : utilise le champ tmp2 et get_timestamp()
	  // pour detecter les bouclages
	  // Pour cela, on prend un timestamp propre ...
	  next_timestamp() ;
	  // ... et on lance la recherche
	  op_analysis(betree->get_root()) ;

	  // Verifications globales au projet
	  global_project_analysis() ;

	  //mise à jours des liens de chainage entre opérations raffinéees
	  solve_homonym_in_abstraction_op(betree->get_root());

	  EXIT_TRACE ;
	  TRACE1("FIN   project_analysis(%x)", betree) ;

	  return betree ;
	}
}

//
// Fin du fichier
//


