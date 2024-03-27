/******************************* CLEARSY **************************************
* Fichier : $Id: v_compo.cpp,v 2.0 2002-11-22 14:37:33 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique : controle du niveau "composant"
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
RCS_ID("$Id: v_compo.cpp,v 1.8 2002-11-22 14:37:33 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Fonction auxiliaire qui verifie que cl2 existe si cl1 est non nul,
// et qui emet un message d'erreur le cas echeant
static inline void check_cl2_if_cl1(T_flag *cl1,
											 T_flag *cl2,
											 T_lex_type cl2_type)
{
if ( (cl1 != NULL) && (cl2 == NULL) )
	{
	cl1_implies_cl2_error(cl1, cl2_type) ;
	}
}

// Fonction qui fait l'analyse de niveau "composant" syntaxique
// d'un Betree syntaxique
// Le Betree enrichi est retourne en cas de succes. En cas d'erreur,
// la fonction renvoie NULL
// Pour des raisons de confort utilisateur, cette fonction est
// appelee en fin d'analyse syntaxique, et pas en debut
// d'analyse semantique
T_betree *component_syntax_analysis(T_betree *betree)
{
TRACE1("component_syntax_analysis(%x)", betree) ;
ENTER_TRACE ;

T_machine *root = betree->get_root() ;

ASSERT(root) ;
ASSERT(root->get_node_type() == NODE_MACHINE) ;

// Si le composant a une clause REFINES, alors il ne peut
// pas se rafiner lui-meme
// CTRL Ref : PROJ 2-1
T_ident *name = root->get_abstraction_name() ;

if ( 	(name != NULL)
	 && (name->get_name()->compare(root->get_machine_name()->get_name()) == TRUE) )
	{
	semantic_error(root->get_abstraction_name(),
				   CAN_CONTINUE,
				   get_error_msg(E_COMPONENT_CAN_NOT_REFINE_ITSELF)) ;
	}


#if 0 // Non, on ne peut pas savoir (collage)
check_cl2_if_cl1(root->get_abstract_constants_clause(),
				 root->get_properties_clause(),
				 L_PROPERTIES) ;
#endif

//
// Si le composant a une clause CONCRETE_VARIABLES ou
// ABSTRACT_VARIABLES, alors il doit avoir une clause
// INITIALISATION
// CTRL Ref : CSYN 4-1
//
if (root->get_concrete_variables_clause() != NULL)
	{
	TRACE1("concrete variables clause : %s",
		   root->get_concrete_variables_clause()->get_class_name()) ;
	}
if (root->get_invariant_clause() != NULL)
	{
	TRACE1("invariant_clause : %s",
		   root->get_invariant_clause()->get_class_name()) ;
	}

#if 0 // Non, on ne peut pas savoir (collage)
check_cl2_if_cl1(root->get_concrete_variables_clause(),
				 root->get_invariant_clause(),
				 L_INVARIANT) ;
#endif

check_cl2_if_cl1(root->get_concrete_variables_clause(),
				 root->get_initialisation_clause(),
				 L_INITIALISATION) ;

#if 0 // NON, on ne peut pas savoir (collage)
check_cl2_if_cl1(root->get_abstract_variables_clause(),
				 root->get_invariant_clause(),
				 L_INVARIANT) ;
#endif

check_cl2_if_cl1(root->get_abstract_variables_clause(),
				 root->get_initialisation_clause(),
				 L_INITIALISATION) ;


if (root->get_machine_type() == MTYPE_SPECIFICATION ||
		root->get_machine_type() == MTYPE_SYSTEM)
	{
	  // Le composant est une specification
	  // S'il a des parametres formels scalaires non typ�s, alors il
	  // doit avoir une clause CONSTRAINTS.
	  // R�ciproquement, s'il n'a aucun param�tre formel, il ne doit
	  // pas avoir de clause CONSTRAINTS.
	  T_ident *cur_param = root->get_params() ;
	  int has_formal_params = (cur_param != NULL) ;

	  int has_untyped_formal_scalar_params = FALSE ;
	  T_ident *first_untyped_scal_para = NULL ;
	  while (cur_param != NULL)
		{
		  if (cur_param->get_ident_type() == ITYPE_MACHINE_SCALAR_FORMAL_PARAM
			  && cur_param->get_BOM_type() == NULL)
			{
			  has_untyped_formal_scalar_params = TRUE ;
			  if (first_untyped_scal_para == NULL)
				first_untyped_scal_para = cur_param ;
			}
		  cur_param = (T_ident *)cur_param->get_next() ;
		}

	  if (has_untyped_formal_scalar_params == TRUE)
		{
		  if (root->get_constraints_clause() == NULL)
			{
			  // La clause CONSTRAINTS est obligatoire
			  // CTRL Ref : CSYN 4-3
			  semantic_error(first_untyped_scal_para,
							 CAN_CONTINUE,
							 get_error_msg(E_SPEC_SHOULD_HAVE_A_CONSTRAINTS_CL)) ;
			}
		}
	  if (has_formal_params == FALSE)
		{
		  if (root->get_constraints_clause() != NULL)
			{
			  // La clause CONSTRAINTS est interdite
			  // CTRL Ref : CSYN 5-1
			  semantic_error(root->get_constraints_clause(),
							 CAN_CONTINUE,
							 get_error_msg(E_SPEC_SHOULD_NOT_HAVE_A_CONSTRAINTS_CL)) ;
			}
		}
	}
else
	{
	// Le composant n'est pas une specification
	// Il doit avoir une clause REFINES
	// CTRL Ref : CSYN 4-2
	if (root->get_refines_clause() == NULL)
		{
		semantic_error(root,
					   CAN_CONTINUE,
					   get_error_msg(E_COMPONENT_HAS_NO_REFINES_CLAUSE)) ;
		}
	}
EXIT_TRACE ;
TRACE1("fin de component_syntax_analysis(%x)", betree) ;

return betree ;
}

// Fonction qui fait l'analyse de niveau "composant" semantique
// d'un Betree syntaxique
// Le Betree enrichi est retourne en cas de succes. En cas d'erreur,
// la fonction renvoie NULL
T_betree *component_seman_analysis(T_betree *betree)
{
  TRACE1("component_seman_analysis(%x)", betree) ;
  ENTER_TRACE ;

 EXIT_TRACE ;
 TRACE1("fin de component_seman_analysis(%x)", betree) ;
 return betree ;
}
