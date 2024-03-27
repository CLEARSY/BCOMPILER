/******************************* CLEARSY **************************************
* Fichier : $Id: t_misc.cpp,v 2.0 2007-05-09 08:13:54 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type check, classes de base
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
RCS_ID("$Id: t_misc.cpp,v 1.50 2007-05-09 08:13:54 jburlando Exp $") ;

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
#include "t_misc.h"

/* Include module de passe B0 pour le pragma literal_integer_type */
#include "i_toplvl.h"

// Analyse semantique : type check
T_betree *type_check(T_betree *betree)
{
  TRACE2("DEBUT type_check(%x::%s)", betree, betree->get_file_name()->get_value()) ;
  ENTER_TRACE ;

  betree->get_root()->type_check() ;

  EXIT_TRACE ;
  TRACE2("FIN   type_check(%x::%s)", betree, betree->get_file_name()->get_value()) ;
  return betree ;
}

//
//	Fonctions de type-check
//

// Type check generique : on ne fait rien
void T_item::type_check(void)
{
  TRACE2("T_item(%x:%s)::type_check() ON NE FAIT RIEN", this, get_class_name()) ;
}

// B0 check generique : on ne fait rien
void T_item::B0_check(void)
{
  TRACE2("T_item(%x:%s)::B0_check() ON NE FAIT RIEN", this, get_class_name()) ;
}

//GP 5/01/99 Il faut verifier que le type de l'ident soit
//compatible avec celui de l'abastraction et celui du
//collage
//Dans un raffinement, le champs "def" des variables et des constantes
//qui raffinent des objets abastraits, point sur l'abstraction
//qu'elle raffine (meme dans le cas d'un collage)(voir v_colli.cpp).
//Quand elles raffinent des objets concrets, dans le cas d'un collage
//le champs "def" pointe sur le collage. On est donc
//Dans le cas general on est oblige de verifier la compatiblite du type
//entre la variable dans l'abstraction et eventuellement celle
//du collage
static void internal_check_type_compatibility(T_machine* root,
													   T_ident* ident)
{
  T_machine* abstraction = root->get_ref_abstraction() ;
  TRACE1("Abstraction:%x", abstraction) ;

  if(abstraction != NULL) //i.e on est dans un raffinement ou une imp
	{
	  //On recherche parmi les constantes de l'abstraction
	  T_ident* spec_ident = abstraction->find_constant(ident->get_name()) ;
	  if(spec_ident == NULL)
		{
		  //On recherche parmi les variables de l'abstraction
		  spec_ident = abstraction->find_variable(ident->get_name()) ;
		}

	  //Quand l'ident est colle, on va chercher le collage
	  if(ident->get_ref_glued() != NULL)
		{
		  ident=ident->get_ref_glued() ;
		}

	  TRACE1("Ident defini dans la spec: %x", spec_ident) ;
	  if(spec_ident != NULL) //L'ident est defini dans l'abstraction
		{
		  T_type * spec_B_type = spec_ident->get_B_type() ;
		  TRACE2("B type ident: %s, B type spec :%s",
				 ident->get_B_type()->make_type_name()->get_value(),
				 spec_B_type->make_type_name()->get_value()) ;

		  if (ident->get_B_type()->is_compatible_with(spec_B_type) == FALSE)
			{

			  semantic_error(ident,
							 CAN_CONTINUE,
							 get_error_msg(E_INCOMPATIBLE_TYPES),
							 ident->get_B_type()->make_type_name()->get_value(),
							 spec_B_type->make_type_name()->get_value()) ;

			  semantic_error_details(spec_ident,
									 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
									 spec_B_type->make_type_name()->get_value(),
									 ident->get_ident_type_name(),
									 ident->get_name()->get_value()) ;
			}
		}
	}
}


static T_ident *analyse_lit_int_type_pragma(T_machine *mach,
													 T_pragma *pragma)
{
  T_ident *res = NULL;

  T_expr *param = pragma->get_params();
  if (param == NULL || param->get_next() != NULL)
	{
	  semantic_error(pragma,
					 CAN_CONTINUE,
					 get_error_msg(E_LIT_INT_TYPE_TAKES_ONE_PARAM));
	}
  else if (param->is_an_ident() == FALSE)
	{
	  // Le param��tre du pragma n'est pas un identificateur
	  semantic_error(pragma,
					 CAN_CONTINUE,
					 get_error_msg(E_B0_LIT_PRAGMA_PARAM_CONCRETE_CONSTANT));
	}
  else
	{
	  res = mach->find_concrete_constant(param->make_ident()->get_name());
	  if (res == NULL)
		{
		  // Le param��tre du pragma n'est pas une constante concr��te
		  semantic_error(pragma,
						 CAN_CONTINUE,
						 get_error_msg(E_B0_LIT_PRAGMA_PARAM_CONCRETE_CONSTANT));
		}
	  else
		{
		  T_type *type = res->get_B_type();
		  if (type == NULL)
			{
			  // Reprise sur erreur
			  res = NULL;
			}
		  else if (type->get_node_type() != NODE_SETOF_TYPE)
			{
			  // Erreur pas un type ensemble
			  semantic_error(pragma,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_LIT_PRAGMA_PARAM_INTEGER_TYPE));
			  res = NULL;
			}
		  else
			{
			  T_type *btype = ((T_setof_type *) type)->get_base_type();
			  if (btype == NULL)
				{
				  // Reprise sur erreur
				  res = NULL;
				}
			  else if (btype->is_a_concrete_integer() == FALSE)
				{
				  // Erreur pas un type entier
				  semantic_error(pragma,
								 CAN_CONTINUE,
								 get_error_msg(E_B0_LIT_PRAGMA_PARAM_INTEGER_TYPE));
				  res = NULL;
				}
			  else
				{
				  // Ok, c'est bien une constante concr��te qui d��note
				  // un type entier.
				}
			}
		}
	}
  return res;
} // analyse_lit_int_type_pragma


// Analyse semantique : type check
void T_machine::type_check(void)
{
  TRACE3("DEBUT T_machine::type_check(%x::%s) type_checked=%s",
		 this, machine_name->get_name()->get_value(),
		 (type_checked == TRUE) ? "TRUE" : "FALSE") ;
  ENTER_TRACE ;

  if (type_checked == TRUE)
	{
	  TRACE0("deja type checke") ;
	  return ;
	}

  TRACE1("mise a jour du champ type_checked de %x a TRUE", this) ;

  type_checked = TRUE ;

#define TYPE_CHECK(x) \
  if (x != NULL) \
	{\
	   T_item *y = x ;\
	while (y != NULL) \
		 {\
			y->type_check() ;\
		 y = y->get_next() ;\
		 } ;\
	}

  // Type check des machines requises
  TRACE1("---- DEBUT TYPE CHECK DES MACHINES REQUISES DU COMPOSANT %s ----",
		 machine_name->get_name()->get_value()) ;
  TYPE_CHECK(first_sees) ;
  TYPE_CHECK(first_includes) ;
  TYPE_CHECK(first_imports) ;
  TYPE_CHECK(first_extends) ;
  TYPE_CHECK(first_uses) ;
  TYPE_CHECK(ref_abstraction) ;
  TRACE1("---------- DEBUT TYPE CHECK COMPOSANT %s -------",
		 machine_name->get_name()->get_value()) ;

  // Parametres formels "ensemblistes"
  // i.e. dont le nom ne comporte pas de minuscule
  T_ident *cur_param = first_param ;
  while (cur_param != NULL)
	{
	  if (cur_param->get_ident_type() == ITYPE_MACHINE_SET_FORMAL_PARAM)
		{
		  T_type *base_type = new T_abstract_type(cur_param,
												  NULL,
												  NULL,
												  NULL,
												  get_betree(),
												  cur_param->get_ref_lexem()) ;
		  T_setof_type *setof = new T_setof_type(base_type,
												 NULL,
												 get_betree(),
												 cur_param->get_ref_lexem()) ;
		  base_type->unlink() ;
		  cur_param->set_B_type(setof, cur_param->get_ref_lexem()) ;
		  setof->unlink() ;
		}
	  else if (get_Use_B0_Declaration() == TRUE)
		{
		  // En BOM type check des param�tres scalaires de machines
		  first_param->type_check();
		}


	  cur_param = (T_ident *)cur_param->get_next() ;
	}

  // SETS de la machine qui ne sont pas des ensembles enumeres
  // -> type Setof(atype(xx, NULL, NULL))
  // Attention s'ils sont deja types (par ex. set provenant d'une machine
  // incluse, on ne les retype pas. Ex: BB SETS ENS, AA INCLUDES ins.BB
  // alors ins.SETS est deja type avec atype(ENS). On ne le retype pas
  // car on ne veut pas lui donner le type atype(ins.ENS)
  T_ident *cur_set = first_set ;

  while (cur_set != NULL)
	{
	  if ( (cur_set->get_B_type() == NULL)
		   && (cur_set->get_ident_type() != ITYPE_ENUMERATED_SET) )
		{
		  TRACE1("affectation du type atype(..) au SET %s",
				 cur_set->get_name()->get_value()) ;
		  T_bound *lower_bound = new T_bound(cur_set,
											 FALSE,
											 NULL,
											 NULL,
											 NULL,
											 cur_set->get_betree(),
											 cur_set->get_ref_lexem()) ;
		  T_bound *upper_bound = new T_bound(cur_set,
											 TRUE,
											 NULL,
											 NULL,
											 NULL,
											 cur_set->get_betree(),
											 cur_set->get_ref_lexem()) ;
		  T_type *atype = new T_abstract_type(cur_set,
											  lower_bound,
											  upper_bound,
											  cur_set,
											  cur_set->get_betree(),
											  cur_set->get_ref_lexem()) ;
		  TRACE3("atype %x, lower_bound %x, upper_bound %x",
				 atype,
				 lower_bound,
				 upper_bound) ;
		  lower_bound->set_father(atype) ;
		  upper_bound->set_father(atype) ;
		  lower_bound->unlink() ;
		  upper_bound->unlink() ;
		  T_setof_type *type = new T_setof_type(atype,
												cur_set,
												cur_set->get_betree(),
												cur_set->get_ref_lexem()) ;
		  atype->unlink() ;
		  cur_set->set_B_type(type, cur_set->get_ref_lexem()) ;
		  TRACE1("setof %x", type) ;
		  type->unlink() ;
		}

	  cur_set = (T_ident *)cur_set->get_next() ;
	}


  // Type check de la machine locale
  TYPE_CHECK(constraints) ; 					// parametres formels "scalaires"
  TYPE_CHECK(first_set) ;						// ensembles



  if (first_value != NULL)
	{
	  ENTER_TRACE ; TRACE0("-- DEBUT TC VALUES (SETS) --") ;
	  // On passe en mode "values"
	  set_inside_values(TRUE) ;
	  set_values_mode_sets(TRUE) ;
	  TYPE_CHECK(first_value) ;					// Clause VALUES
	  set_inside_values(FALSE) ;

	  TRACE0("-- FIN TC VALUES (SETS) --") ; EXIT_TRACE ;
	}

  // En mode BOM uiquement:
  if (get_Use_B0_Declaration() == TRUE)
	{
	  TYPE_CHECK(first_concrete_constant);
	}

  TYPE_CHECK(properties) ;					// constantes

  /* SL 26/01/99 deplace apres l'instanciation des machines requises
	 Necessaire si les variables sont typees avec un parametre formel
	 d'une machine requise */
  /* was TYPE_CHECK(first_promotes) ;*/
  // FIN SL 26/01/1999

  if (first_value != NULL)
	{
	  ENTER_TRACE ; TRACE0("-- DEBUT TC VALUES (CST) --") ;
	  // On passe en mode "values"
	  set_inside_values(TRUE) ;
	  set_values_mode_sets(FALSE) ;
	  TYPE_CHECK(first_value) ;					// Clause VALUES
	  set_inside_values(FALSE) ;

	  TRACE0("-- FIN TC VALUES (CST) --") ; EXIT_TRACE ;
	}

  /* SL 22/01/99 deplace apres l'instanciation des machines requises
	 Necessaire si les variables sont typees avec un parametre formel
	 d'une machine requise

    TYPE_CHECK(invariant) ;						// variables
	  TYPE_CHECK(first_assertion) ;
  Fin SL 22/01/99 */

  // SL 12/12/97 controle deplace
  // Pour toutes les machines requises, verification
  // des instanciations
  // On passe en mode VALUES pour l'eventuelle valuation
  // des parametres formels ensemblistes
  TRACE1("-- DEBUT VERIF INSTANCIATION DES MACHINES REQUISES DU COMPOSANT %s --",
		 machine_name->get_name()->get_value()) ;
  set_inside_values(TRUE) ;
  check_required_machines_instanciation() ;
  set_inside_values(FALSE) ;

  // DEBUT SL 26/01/1999
  TRACE0("-- TC PROMOTES") ;
  TYPE_CHECK(first_promotes) ;
  // FIN SL 26/01/1999

  // DEBUT SL 21/01/1999
  TRACE0("-- TC INVARIANT") ;
  if (get_Use_B0_Declaration() == TRUE)
	{
	  TYPE_CHECK(first_concrete_variable);
	}
  TYPE_CHECK(invariant) ;						// variables

  TRACE0("-- TC VARIANT") ;
  TYPE_CHECK(variant) ;


  TRACE0("-- TC ASSERTIONS") ;
  TYPE_CHECK(first_assertion) ;
  // DEBUT SL 21/01/1999


  // Clause initialisation : il faut appeler l'operation dediee
  // pour les verifications supplementaires si || est utilise
  type_check_substitution_list(first_initialisation) ;


  // La fin

  // Les operations locales
  TYPE_CHECK(first_local_operation) ;

  // Les operations
  T_list_link *cur_op_link = first_op_list;
  while (cur_op_link != NULL)
	{
	  T_generic_op *op = (T_generic_op *)cur_op_link->get_object();
	  ASSERT(op->is_an_operation());
	  op->type_check();
	  cur_op_link = (T_list_link *)cur_op_link->get_next();
	}

  TRACE1("----------  FIN  TYPE CHECK COMPOSANT -------",
		 machine_name->get_name()->get_value()) ;


  // Verification :: on verifie que toutes les concrete_variables sont typees
  // CTRL Ref : TYPE 1-4
  T_ident *cur = first_concrete_variable ;
  int i ;
  for(i = 0 ; i<2 ; i++) //2 passes: variables concrete puis les abstraites
	{
	  while (cur != NULL)
		{
		  if (cur->get_B_type() == NULL)
			{
			  semantic_error(cur,
							 CAN_CONTINUE,
							 get_error_msg(E_NO_TYPE_FOR_IDENT),
							 cur->get_ident_type_name(),
							 cur->get_name()->get_value()) ;
		}
		  else
			{
			  internal_check_type_compatibility(this, cur) ;
			}

		  cur = (T_ident *)cur->get_next() ;
		}
	  cur = first_abstract_variable ;
	}

  cur = first_concrete_constant ;
  for(i = 0 ; i<2 ; i++) //2 passes: constantes concrete puis les abstraites
	{
	  while (cur != NULL)
		{
		  if (cur->get_B_type() == NULL)
			{
			  semantic_error(cur,
							 CAN_CONTINUE,
							 get_error_msg(E_NO_TYPE_FOR_IDENT),
							 cur->get_ident_type_name(),
							 cur->get_name()->get_value()) ;
			}
		  else
			{
			  internal_check_type_compatibility(this, cur) ;
			}

		  cur = (T_ident *)cur->get_next() ;
		}
	  cur = first_abstract_constant ;
	}

  // chargement du type B0 des entiers litt�raux
  if (get_Use_B0_Declaration() == TRUE)
	{
	  // recherche des pragmas literal_integer_type
	  T_pragma *pragma = lookup_pragma(lookup_symbol("literal_integer_type"),
									   NULL);
	  while (pragma != NULL)
		{
		  T_ident *new_lit = analyse_lit_int_type_pragma(this, pragma);
		  if (new_lit != NULL)
			{
			  if (literal_integer_type == NULL)
				{
				  // Premi��re occurrence du pragma.
				  literal_integer_type = new_lit;
				}
			  else if (literal_integer_type->get_name() != new_lit->get_name())
				{
				  // Erreur s��mantique, deux pragmas contradictoires.
				  semantic_error(pragma,
								 CAN_CONTINUE,
								 get_error_msg(E_REDEF_PRAG_LIT_INT_TYPE));
				}
			  else
				{
				  // OK, deux pragmas avec le m��me nom de constante.
				}
			}
		  else
			{
			  // erreur d��j�� signal��e, ne rien faire
			}
		  pragma = lookup_pragma(lookup_symbol("literal_integer_type"),
								 pragma);
		} // while (pragma != NULL)
	} // if (get_Use_B0_Declaration() == TRUE)

  EXIT_TRACE ;
  TRACE2("FIN   type_check(%x::%s)",
		 this, machine_name->get_name()->get_value()) ;
}


// Fonction qui permet de savoir si c'est une machine qui d�finie
// des types de base
int T_machine::is_basic_translator_machine(void)
{
  T_pragma *basic_tr_mach;
  basic_tr_mach = lookup_pragma (lookup_symbol("basic_translator_machine"),
								 NULL);
  if (basic_tr_mach == NULL
	  ||
	  basic_tr_mach->get_params() != NULL)
	{
	  return FALSE;
	}
  else
	{
	  return TRUE;
	}
}
// Verification des instanciations des machines requises
// Effet de bord : utilise le champ tmp2
void T_machine::check_required_machines_instanciation(void)
{
  TRACE1(">> T_machine(%x)::check_required_machines_instanciation()", this) ;

  // Il faut verifier les instanciations pour les clauses
  // IMPORTS EXTENDS INCLUDES

  T_used_machine *cur = first_imports ;
  while (cur != NULL)
	{
	  check_single_machine_instanciation(cur) ;
	  cur = (T_used_machine *)cur->get_next() ;
	}

  cur = first_extends ;
  while (cur != NULL)
	{
	  check_single_machine_instanciation(cur) ;
	  cur = (T_used_machine *)cur->get_next() ;
	}

  cur = first_includes ;
  while (cur != NULL)
	{
	  check_single_machine_instanciation(cur) ;
	  cur = (T_used_machine *)cur->get_next() ;
	}

  TRACE1("<< T_machine(%x)::check_required_machines_instanciation()", this) ;
}

// Verification des instanciations des machines requises
void T_machine::check_single_machine_instanciation(T_used_machine *umach)
{
  TRACE4("--> T_machine(%x:%s)::check_single_machines_instanciation(%x:%s)",
		 this,
		 get_machine_name()->get_name()->get_value(),
		 umach,
		 umach->get_name()->get_name()->get_value()) ;
  ENTER_TRACE ;

  int nb_effective_params = 0 ;
  T_item *cur_param_item = umach->get_use_params() ;

  while (cur_param_item != NULL)
	{
	  T_expr *cur_param = cur_param_item->make_expr() ;
	  nb_effective_params ++ ;
	  cur_param->make_type() ;
	  cur_param_item = cur_param_item->get_next() ;
	}

  int nb_formal_params = 0 ;
  cur_param_item = umach->get_ref_machine()->get_params() ;

  while (cur_param_item != NULL)
	{
	  nb_formal_params ++ ;
	  cur_param_item = cur_param_item->get_next() ;
	}

  TRACE2("%d param effectifs fournis, %d attendus",
		 nb_effective_params,
		 nb_formal_params) ;

  if (nb_formal_params != nb_effective_params)
	{
	  semantic_error(umach,
					 CAN_CONTINUE,
					 get_error_msg(E_WRONG_NUMBER_OF_EFFECTIVE_MACHINE_PARAMS),
					 umach->get_name()->get_name()->get_value(),
					 nb_effective_params,
					 nb_formal_params) ;
	  semantic_error_details(umach->get_ref_machine(),
							 get_error_msg(E_LOCATION_OF_ITEM_DEF),
							 umach->get_ref_machine()->
							 	get_machine_name()->get_name()->get_value()) ;

	  return ;
	}

  // Il faut verifier les types de tous les parametres !
  // Ici on sait qu'il y a autant de parametres effectifs que
  // de parametres formels
  T_item *cur_effective_item = umach->get_use_params() ;
  T_ident *cur_formal = umach->get_ref_machine()->get_params() ;

  TRACE2("umach %x ->get_ref_machine %x", umach, umach->get_ref_machine()) ;
  while (cur_effective_item != NULL)
	{
	  T_expr *cur_effective = cur_effective_item->make_expr() ;
	  ASSERT(cur_formal != NULL) ;

	  T_type *etype = cur_effective->get_B_type() ;

	  if (etype == NULL)
		{
		  semantic_error(cur_effective,
						 CAN_CONTINUE,
						 get_error_msg(E_NO_TYPE_GIVEN_FOR_EFFECTIVE_PARAMETER)) ;
		}
	  else /* etype != NULL */
		{
		  // On verifie la compatibilite des types des parametres
		  // formels et effectifs
		  TRACE2("cur_formal %x %s" ,
				 cur_formal,
				 cur_formal->get_name()->get_value()) ;
		  T_machine *ref_machine = NULL ;
		  cur_formal->find_machine_and_op_def(&ref_machine, NULL) ;
		  TRACE1("cur_formal -> context = %s", ref_machine->get_context()->get_name()->get_name()->get_value()) ;
		  TRACE1("etype %x", etype) ;

		  T_type *ftype = cur_formal->get_B_type() ;

		  if (    (ftype != NULL) // si NULL, reprise sur erreur
		       && (ftype->is_compatible_with(etype) == FALSE)
			   && (ftype->can_be_valuated_by(etype, cur_formal, NULL) == FALSE) )
			{
			  semantic_error(cur_effective,
							 CAN_CONTINUE,
							 get_error_msg(E_INCOMPATIBLE_TYPES_FOR_MACHINE_PARAMS),
							 etype->make_type_name()->get_value(),
							 ftype->make_type_name()->get_value(),
							 cur_formal->get_name()->get_value()) ;
			  semantic_error(cur_formal->get_typing_location(),
							 MULTI_LINE,
							 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
							 ftype->make_type_name()->get_value(),
							 cur_formal->get_ident_type_name(),
							 cur_formal->get_name()->get_value()) ;
			}
		  else
			{
			  // On provoque une mise a jour du type pour provoquer la
			  // valuation du type abstrait de cur_formal
			  cur_formal->set_B_type(etype, etype->get_ref_lexem()) ;
			}
		}

	  cur_effective_item = cur_effective_item->get_next() ;
	  cur_formal = (T_ident *)cur_formal->get_next() ;
	}

  EXIT_TRACE ;
  TRACE4("<-- T_machine(%x:%s)::check_required_machines_instanciation(%x:%s)",
		 this,
		 get_machine_name()->get_name()->get_value(),
		 umach,
		 umach->get_name()->get_name()->get_value()) ;
}

// Analyse semantique : type check
void T_used_machine::type_check(void)
{
  TRACE2("DEBUT T_used_machine::type_check(%x::%s)",
		 this, name->get_name()->get_value()) ;
  ENTER_TRACE ;

  ASSERT(ref_machine != NULL) ;

#if 0 //GP 25/01/99 C'est du B0
  // CTRL Ref : LVAL 3
  //L'instanciation d'un parametre ensemble d'une instance de machine importee,
  //ne peut pas etre un intervalle vide.
  if ( (use_type == USE_IMPORTS) || (use_type == USE_EXTENDS) )
	{
	  T_expr* cur_param = first_use_param ;
	  while (cur_param != NULL)
		{
		  if (    (cur_param->get_node_type() == NODE_EMPTY_SEQ)
			   || (cur_param->get_node_type() == NODE_EMPTY_SET) )
			{
			  semantic_error(cur_param,
							 CAN_CONTINUE,
							 get_error_msg(E_SET_FORMAL_PARAMETERS_CAN_NOT_BE_AN_EMPTY_INTERVAL)) ;
			}
		  cur_param = (T_expr*)cur_param->get_next() ;
		}
	}
#endif

  ref_machine->type_check() ;

  T_item *cur_param_item = first_use_param ;
  while (cur_param_item != NULL)
	{
	  T_expr* cur_param = cur_param_item->make_expr() ;
	  if(cur_param->get_B_type()==NULL)
		{
		  TRACE1("REPRISE SUR ERREUR: T_used_machine(%x)::type_check", this);
		  //reprise sur erreur
		}
	  else
		{
//		  if(cur_param->get_B_type()->is_based_on_string() == TRUE)
//			{
//			  semantic_error(cur_param,
//							 CAN_CONTINUE,
//							 get_error_msg(E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM)) ;
//			}
		}
	   cur_param_item = cur_param_item->get_next() ;
	}

  EXIT_TRACE ;
  TRACE2("FIN   type_check(%x::%s)", this, name->get_name()->get_value()) ;
}

// Classe T_valuation : on type !
void T_valuation::type_check(void)
{
  TRACE2("T_valuation(%x:%s)::type_check", this, ident->get_name()->get_value()) ;
  T_ident *def = ident ;
  T_ident *set_name = NULL ;

  while (def->get_def() != NULL)
	{
	  def = def->get_def() ;
	}

  TRACE2("def %s %s", def->get_name()->get_value(), def->get_ident_type_name()) ;

  if (get_values_mode_sets() == TRUE)
	{
	  // Mode TC des sets
	  if (def->get_ident_type() != ITYPE_ABSTRACT_SET)
		{
		  // Ce n'est pas un sets -> on le traitera plus tard
		  TRACE1("%s n'est pas un sets->deja fait",
				 ident->get_name()->get_value()) ;
		  return ;
		}
	  TRACE1("%s est un sets", ident->get_name()->get_value()) ;
	}
  else
	{
	  // Mode TC des constantes
	  if (def->get_ident_type() == ITYPE_ABSTRACT_SET)
		{
		  // Ce n'est pas une cte -> on l'a deja traitee
		  TRACE1("%s n'est pas une cst->deja fait",
				 ident->get_name()->get_value()) ;
		  return ;
		}
	  TRACE1("%s est une cst", ident->get_name()->get_value()) ;
	}

  ENTER_TRACE ;
  TRACE1("--- DEBUT TC VALUES %s ---", ident->get_name()->get_value()) ;
  TRACE2("T_valuation(%x:%s)::type_check", this, ident->get_name()->get_value()) ;

  ident->make_type() ;
  value->make_type() ;

  if (value->get_B_type() == NULL)
	{
	  semantic_error(value,
					 CAN_CONTINUE,
					 get_error_msg(E_RHS_OF_VALUATION_HAS_NO_TYPE)) ;

	  TRACE0("PANIC ! value->get_B_type() == NULL") ;
	  return ;
	}

  // Verification de la validite de la valuation pour les ABSTRACT SETS
  if (ident->get_def()->get_ident_type() == ITYPE_ABSTRACT_SET)
	{
	  // Les trois types de valuations possibles sont :
	  // 1) valuation avec un ensemble abstrait d'une machine vue ou importee
	  // 2) valuation avec collage implicite par un ensemble homonyme d'une
	  //    machine vue ou importee -- Pas traite ici, car ce n'est pas
	  //    une valuation
	  // 3) valuation par un intervalle d'entiers, fini et non vide
	  int valid = FALSE ;

	  TRACE3("ici value %x %s is_an_ident %s",
			 value,
			 value->get_class_name(),
			 (value->is_an_ident() == TRUE) ? "TRUE" : "FALSE") ;
	  if (value->is_an_ident() == TRUE)
		{
		  T_ident *ival = (T_ident *)value->follow_indirection() ;

		  TRACE1("ival %x", ival) ;
		  TRACE1("ival %s", ival->get_name()->get_value()) ;

		  while (ival->get_def() != NULL)
			{
			  ival = ival->get_def() ;
			}

		  TRACE1("ival %x", ival) ;
		  TRACE2("ival %s ident_type %s",
				 ival->get_name()->get_value(),
				 ival->get_ident_type_name()) ;

		  switch (ival->get_ident_type())
			{
			case ITYPE_ABSTRACT_SET :
			  {
				// Les regles de visibilite nous garantissent que l'identificateur
				// vient d'une machine importee ou vue
			  	valid = TRUE;
			  	set_name = ival ; //GUB3242
			  	break;
			  }
			case ITYPE_ABSTRACT_CONSTANT :
			case ITYPE_CONCRETE_CONSTANT :
			  {
				valid = FALSE ;
				set_name = ival ;
				break ;
			  }

			case ITYPE_BUILTIN :
			  {
				// Il faut toujours faire la verification, donc
				// on ne touche pas a valid. Par contre, on met
				// a jour abs pour referencer le nom de l'ensemble
				set_name = ival ;
				break ;
			  }

			default :
			  {
				// Par defaut, on ne fait rien
			  }

			}


		  TRACE2("ici set_name = %x:%s",
				 set_name,
				 (set_name == NULL) ? "" : set_name->get_name()->get_value()) ;
		}

	  if ( (valid == FALSE) && (value->is_a_set() == TRUE) )
		{
		  // Il faut que le type soit : intervalle d'entiers, fini
		  // et non vide
		  T_setof_type *set = (T_setof_type *)(value->get_B_type()) ;
		  TRACE2("ici set->get_base_type %s is_an_integer %s",
				 set->get_base_type()->make_type_name()->get_value(),
				 (set->get_base_type()->is_an_integer() == TRUE) ? "TRUE":"FALSE") ;
		  if (set->get_base_type()->is_an_integer() == TRUE)
			{
			  T_predefined_type *ptype = (T_predefined_type *) set->get_base_type();

			  if (ptype->is_bounded() == TRUE)
				{
				  valid = TRUE ;
				}
			}

		}

	  if (valid == FALSE)
		{
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_INVALID_VALUATION_OF_ABSTRACT_SET),
						 ident->get_name()->get_value()) ;
		  return ;
		}

	}


  // Temporairement : on met le type sans reflechir
  T_type *before = ident->get_B_type() ;

  TRACE1("ici set_name=%x", set_name) ;
  if (set_name != NULL)
	{
	  T_setof_type *set = (T_setof_type *)before ;
	  T_abstract_type *atype = (T_abstract_type *)set->get_spec_base_type() ;
	  atype->set_valuation_ident(set_name) ;
	  TRACE2("mise a jour du valuation ident de %x avec %x", atype, set_name) ;
	}

  // Modification importante 17/08/97
  // Copie privee du type pour l'inference sinon on risque de
  // patcher les types predefinis INTEGER, ...
  T_type *copy = value->get_B_type()->clone_type(ident,
												 ident->get_betree(),
												 ident->get_ref_lexem()) ;
  ident->set_B_type(copy, get_ref_lexem()) ;

  // report de la modif [2537] dans le cadre des valuations
  (void)value->get_B_type()->is_compatible_with(copy) ;

  // Le unlink est fait apres

  if (ident->get_B_type() == before)
	{
#if 0
	  TRACE0("reset du type et mise a jour avec les bonnes bornes") ;
	  ident->reset_B_type() ;
	  ident->set_B_type(value->get_B_type(), get_ref_lexem()) ;
#else
	  // Le type n'a pas change
	  // S'il y a des bornes, on les met
	  // Partie gauche : Setof(XX) avec XX borne avec des T_bound
	  // Partie droite : Setof(YY) avec YY borne, sans T_bound
	  TRACE0("on regarde s'il y a des bornes valuees a recuperer ...") ;

	  T_type *ltype = ident->get_B_type() ;
	  T_type *rtype = copy ;


	  if ( (ltype != NULL) && (rtype != NULL) )
		{
		  TRACE2("ltype %x rtype %x", ltype, rtype) ;
		  TRACE2("ltype %s rtype %s",
				 ltype->get_class_name(),
				 rtype->get_class_name()) ;

		  TRACE1("ltype->get_node_type() %x", ltype->get_node_type()) ;
		  TRACE1("rtype->get_node_type() %x", rtype->get_node_type()) ;
		  if ( (ltype->get_node_type() == NODE_SETOF_TYPE)
			   && (rtype->get_node_type() == NODE_SETOF_TYPE) )
			{
			  T_setof_type *lset = (T_setof_type *)ltype ;
			  T_setof_type *rset = (T_setof_type *)rtype ;

			  TRACE2("lset->is_a_base_type() %s rset->is_a_base_type() %s",
					 (lset->get_base_type()->is_a_base_type() == TRUE) ? "TRUE" : "FALSE",
					 (rset->get_base_type()->is_a_base_type() == TRUE) ? "TRUE" : "FALSE") ;

			  if ( (lset->get_base_type()->is_a_base_type() == TRUE)
				   && (rset->get_base_type()->is_a_base_type() == TRUE) )
				{
				  T_base_type *lbase = (T_base_type *)(lset->get_base_type()) ;
				  T_base_type *rbase = (T_base_type *)(rset->get_base_type()) ;

				  TRACE2("lbase(%x)->is_bounded() = %s",
						 lbase,
						 (lbase->is_bounded() == TRUE) ? "TRUE" : "FALSE") ;
				  TRACE2("rbase(%x)->is_bounded() = %s",
						 rbase,
						 (rbase->is_bounded() == TRUE) ? "TRUE" : "FALSE") ;

#ifdef TRACE
				  TRACE2("lbase(%x)->are_bounds_set() = %s",
						 lbase, (lbase->are_bounds_set() == TRUE) ? "TRUE" : "FALSE") ;
				  TRACE2("rbase(%x)->is_bounded() = %s",
						 rbase, (rbase->is_bounded() == TRUE) ? "TRUE" : "FALSE") ;
				  if (rbase->is_bounded() == TRUE)
					{
					  TRACE1("rbase->get_lower_bound %s",
							 rbase->get_lower_bound()->get_class_name()) ;
					}
#endif

				  TRACE3("mise a jour des bornes de %x avec [%x, %x]",
						 lbase,
						 rbase->get_lower_bound(),
						 rbase->get_upper_bound()) ;


				  T_base_type *orig_lba =
					(T_base_type *)lset->get_spec_base_type() ;
				  ASSERT(orig_lba->is_a_base_type() == TRUE) ;
				  T_base_type *new_lbase =
					(T_base_type *)orig_lba->clone_type(ltype,
														ltype->get_betree(),
														ltype->get_ref_lexem());


				  TRACE2("ici new_lbase %x:%s",
						 new_lbase,
						 new_lbase->get_class_name()) ;
				  ASSERT(new_lbase->is_a_base_type() == TRUE) ;
				  new_lbase->set_bounds(rbase->get_lower_bound(),
										rbase->get_upper_bound()) ;

				  lset->set_base_type(new_lbase) ;
				}
#ifdef TRACE
			  else
				{
				  TRACE0("pas de bornes a recuperer en partie droite") ;
				}
#endif

			}
		}
	}

#endif

  copy->unlink() ;

  //GP 28/01/99
  ident->check_string_uses(FALSE) ;


  EXIT_TRACE ;
  TRACE1("--- FIN TC VALUES %s ---", ident->get_name()->get_value()) ;
}

//
//	}{	Classe T_used_op
//

//GP 30/12/98
//Verifier que les parametres sont compatibles
void T_used_op::check_op_signature(T_generic_op* spec_op)
{
  TRACE0("T_used_op");
  TRACE4("T_used_op(%p:%s)::check_op_signature(%p:%s)",
		 this,
		 real_op_name->get_value(),
		 spec_op,
		 spec_op->get_name()->get_name()->get_value());

  T_ident* params_spec = spec_op->get_in_params() ;
  T_ident* params_prom = get_in_params() ;

  //verification des parametres d'entree et de sortie, en deux passes (i=0..1)
  for(int i=0 ; i<2 ; i++)
	{

	  while((params_spec != NULL) && (params_prom != NULL))
		{

		  if(!params_spec->get_name()->compare(params_prom->get_name()))
			{
			  //Les paramatres n'ont pas le meme nom
			  semantic_error(params_prom,
							 CAN_CONTINUE,
							 get_error_msg(E_OP_PARAM_NOT_SPECIFIED),
							 params_prom->get_ident_type_name(),
							 params_prom->get_name()->get_value()) ;
			  semantic_error_details(spec_op,
									 get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
			}
		  else
			{
			  T_type* param_spec_type = params_spec->get_B_type() ;
			  T_type* param_prom_type = params_prom->get_B_type() ;
			  ASSERT(param_spec_type != NULL) ;
			  ASSERT(param_prom_type != NULL) ;

			  if(!param_spec_type->is_compatible_with(param_prom_type))
				 {
				   //Les parametres n'ont pas le meme type
				   semantic_error(params_prom,
								  CAN_CONTINUE,
								  get_error_msg(E_INCOMPATIBLE_TYPES),
								  param_prom_type->make_type_name()->get_value(),
								  param_prom_type->make_type_name()->get_value()) ;

				   semantic_error_details(params_spec,
								 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
								 param_spec_type->make_type_name()->get_value(),
								 params_spec->get_ident_type_name(),
								 params_spec->get_name()->get_value()) ;
				 }
			}

		  params_spec = (T_ident*)params_spec->get_next() ;
		  params_prom = (T_ident*)params_prom->get_next() ;
		}

	  while(params_spec != NULL)
		{
		  //Le parametre n'est pas raffine
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_OP_PARAM_NOT_REFINED),
						 params_spec->get_ident_type_name(),
						 params_spec->get_name()->get_value()) ;
			  semantic_error_details(spec_op,
									 get_error_msg(E_LOCATION_OF_OP_SPEC)) ;

			  params_spec = (T_ident*)params_spec->get_next() ;
		}

	  while(params_prom != NULL)
		{
		  //Le parametre n'est pas specifie
		  semantic_error(params_prom,
						 CAN_CONTINUE,
						 get_error_msg(E_OP_PARAM_NOT_SPECIFIED),
						 params_prom->get_ident_type_name(),
						 params_prom->get_name()->get_value()) ;
		  semantic_error_details(spec_op,
								 get_error_msg(E_LOCATION_OF_OP_SPEC)) ;

		  params_prom = (T_ident*)params_prom->get_next() ;
		}

	  //Pour la deuxieme passe, on prends les parametres de sortie
	  params_spec = spec_op->get_out_params() ;
	  params_prom = get_out_params() ;
	}

  TRACE4("<-T_used_op(%p:%s)::check_op_signature(%p:%s)",
		 this,
		 real_op_name->get_value(),
		 spec_op,
		 spec_op->get_name()->get_name()->get_value());
}


void T_used_op::type_check(void)
{
  TRACE2("T_used_op(%x:%s)::type_check", this, real_op_name->get_value()) ;

  if (type_checked == TRUE)
	{
	  TRACE0("deja fait") ;
	  return ;
	}
  type_checked = TRUE ;

  T_machine *root = get_betree()->get_root() ;

  T_item *res = find_op_in_list(root->get_includes(), this) ;

  // Recherche de l'operation dans les machines requises
  if (res == NULL)
	{
	  res = find_op_in_list(root->get_extends(), this) ;
	  if (res == NULL)
		{
		  res = find_op_in_list(root->get_imports(), this) ;
		}
	}

  if (res == NULL)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_UNKNOWN_OPERATION_CAN_NOT_BE_PROMOTED),
					 real_op_name->get_value()) ;
	}
  else
	{
	  ref_op = (T_generic_op *)res ;

	  if(root->get_machine_type() != MTYPE_SPECIFICATION &&
			  root->get_machine_type() != MTYPE_SYSTEM)
		{
		  T_machine* spec = root->get_specification() ;
		  TRACE0("avant");
		  T_generic_op* spec_op = spec->find_op(get_op_name()) ;
		  TRACE0("apres");
		  ASSERT(spec_op != NULL) ; //Garanti par Proj 2-4

		  // On v�rifie que les signature sont homog�ne entre
		  // op�rations promues
		  if (get_Use_B0_Declaration() == TRUE
			  &&
			  spec_op->get_status() != get_status())
			{
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_INVALID_OP_REFINEMENT),
							 name->get_name()->get_value());
			}
		  //GP 31/12/98
		  //CTRL Ref: PROJ 2-5
		  check_op_signature(spec_op) ;
		}
	}
  TRACE2("T_used_op(%x:%s)::type_check", this, real_op_name->get_value()) ;
}

//GP 15/01/99
// V�rifie que tous les identificateurs de list_idents ont �t� typ�s.

extern void
check_type_ident_list(T_ident* list_idents)
{
  T_ident *cur_ident = list_idents ;
  while (cur_ident != NULL)
	{
	  if (cur_ident->get_B_type() == NULL)
		{
		  semantic_error(cur_ident,
						 CAN_CONTINUE,
						 get_error_msg(E_NO_TYPE_FOR_IDENT),
						 cur_ident->get_ident_type_name(),
						 cur_ident->get_name()->get_value()) ;
		}
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}
}


extern void
check_type_ident_list(T_ident* list_idents,
					  const char *loc1,
					  const char *loc2)
{
  T_ident *cur = list_idents ;
  while (cur != NULL)
	{
	  if (cur->get_B_type() == NULL)
		{
		  semantic_error(cur,
						 CAN_CONTINUE,
						 get_error_msg(E_NO_TYPE_FOR_IDENT_LOC_EXPECTED),
						 cur->get_ident_type_name(),
						 cur->get_name()->get_value(),
						 cur->get_name()->get_value(),
						 loc1,
						 loc2) ;
		}

	  cur = (T_ident *)cur->get_next() ;
	}
}
