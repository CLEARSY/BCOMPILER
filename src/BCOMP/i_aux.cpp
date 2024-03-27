/******************************* CLEARSY **************************************
* Fichier : $Id: i_aux.cpp,v 2.0 2001-12-04 11:20:36 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		B0 Checker, Fonctions auxiliaires
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
RCS_ID("$Id: i_aux.cpp,v 1.13 2001-12-04 11:20:36 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#ifdef B0C
#include "i_ctx.h"
#endif //B0C
#include "c_api.h"

#ifdef B0C
#include "i_globdat.h"
#include "i_listid.h"
#endif //B0C

#include "i_aux.h"

#ifdef B0C


// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est une expression B0
// de maplet
// Elle renvoie TRUE dans ce cas.

int syntax_check_list_is_a_maplet(T_item *list_items,
										   T_list_ident **list_ident,
										   T_B0_context context)
{
  T_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  // Appel a la fonction virtuelle recursive
	  if (cur_list_items->syntax_check_is_a_maplet(list_ident,
												   context) == FALSE)
		{
		  // erreur B0 ...;

		  result = FALSE;
		}
	  cur_list_items = (T_item *)cur_list_items->get_next() ;
	}


  return result;
}

// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est un terme
// Elle renvoie TRUE dans ce cas.
int syntax_check_list_is_a_term(T_item *list_items,
										 T_list_ident **list_ident)
{
  T_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  // Appel a la fonction virtuelle recursive et verification que l'item
	  // est un terme
	  if (cur_list_items->syntax_check_is_a_term(list_ident) == FALSE)
		{
		  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			{
			  // erreur B0 ...;
			  B0_syntax_error(cur_list_items,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_TERM)) ;
			}
		   result = FALSE;
		}
	  cur_list_items = (T_item *)cur_list_items->get_next() ;
	}
  return result;
}




// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est une expression B0
// de parametre effectif d'entree d'operation (terme, tableau,
// chaine literale)
// Elle renvoie TRUE dans ce cas.

int syntax_check_list_is_an_op_in_param(T_item *list_items,
												 T_list_ident **list_ident)
{
  T_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  // Appel a la fonction virtuelle recursive
	  if ( (cur_list_items->syntax_check_is_a_term(list_ident) == FALSE) &&
		   (cur_list_items->syntax_check_is_an_array(list_ident) == FALSE) &&
		   (cur_list_items->get_node_type() != NODE_LITERAL_STRING) )
		{
		  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			{
			  // erreur B0 ...;
			  B0_syntax_error(cur_list_items,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_EFFECTIVE_PARAMETER));
			}
		  result = FALSE;
		}
	  cur_list_items = (T_item *)cur_list_items->get_next() ;
	}
  return result;
}


// Fonction prenant en argument une liste de T_record_item
// Elle parcourt la liste, et vérifie que chaque champ est un terme ou une
// expression de tableau.
// Elle renvoie TRUE dans ce cas.

int syntax_check_list_is_a_record_item(T_record_item *list_items,
												T_list_ident **list_ident)
{
  T_record_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  T_expr *cur_value = cur_list_items->get_value();
	  // Appel a la fonction virtuelle recursive
	  if ( (cur_value->syntax_check_is_a_term(list_ident) == FALSE) &&
		   (cur_value->syntax_check_is_an_array(list_ident) == FALSE) )
		{
		  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			{
			  // erreur B0 ...;
			  B0_syntax_error(cur_list_items,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_RECORD_ITEM)) ;
			}
		  result = FALSE;
		}
	  cur_list_items =
		(T_record_item *)cur_list_items->get_next() ;
	}
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
// Fonctions auxiliaires pour la verification des types des donnees concretes
//
/////////////////////////////////////////////////////////////////////////////

// Fonction prenant en argument une liste de T_label_type
// Elle parcourt la liste, et vérifie que chaque element est un type conforme
// au B0
// Elle renvoie TRUE dans ce cas.
int type_check_list_is_a_label_type(T_label_type *list_labels)
{
  T_label_type * cur_list_labels = list_labels;
  int result = TRUE;
  while (cur_list_labels != NULL)
	{
	  T_type *type = cur_list_labels->get_type();
	  // Appel aux fonctions virtuelles recursives

	  if ( (type->is_an_integer() == FALSE) &&
		   (type->is_a_boolean() == FALSE) &&
		   (type->is_an_array() == FALSE) &&
		   (type->is_a_record() == FALSE) &&
		   (type->is_an_abstract_or_enumerated_set_element() == FALSE) )
		{
		  // erreur B0 : type de label non conforme au B0
		  B0_semantic_error(cur_list_labels,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_TYPE_FOR_RECORD_LABEL));

		  result = FALSE ;
		}

	  cur_list_labels =
		(T_label_type *)cur_list_labels->get_next() ;
	}
  return result;
}



// Fonction prenant en argument une liste de T_ident
// Elle parcourt la liste, et vérifie si chaque type B est valide pour
// un type de variable locale
void local_variable_type_check_list(T_ident *list_ident)
{
  T_ident * cur_list_ident = list_ident;
  while (cur_list_ident != NULL)
	{
	  if ( (cur_list_ident->get_definition()->get_original_B_type() != NULL) &&
		   (cur_list_ident->get_definition()->get_original_B_type()
			->is_local_variable_type() == FALSE) )
		{
		  // Erreur B0
		  B0_semantic_error(cur_list_ident,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_TYPE),
							cur_list_ident->get_definition()->get_ident_type_name(),
							cur_list_ident->get_definition()->get_name()->get_value()) ;
		}

	  cur_list_ident =
		(T_ident *)cur_list_ident->get_next() ;
	}
}


// Fonction prenant en argument une liste de T_record_item
// Elle parcourt la liste, et vérifie que chaque champ est une expression de
// typage par appartenance d'une constante concrete
// Elle renvoie TRUE dans ce cas.
int syntax_check_list_is_a_record_item_type(T_record_item *list_items)
{
  T_record_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  T_expr *cur_value = cur_list_items->get_value();
	  // Appel a la fonction virtuelle recursive

	  if (cur_value->is_belong_concrete_data_expr() == FALSE)
		{
		  // erreur B0 ...;
		  B0_semantic_error(cur_value,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_TYPING_EXPR_FOR_RECORD_ITEM),
							make_class_name(cur_value)) ;
		  result = FALSE;
		}

	  cur_list_items =
		(T_record_item *)cur_list_items->get_next() ;
	}
  return result;
}


// Fonction prenant en argument une liste d'items
// Elle parcourt la liste, et vérifie que chaque item est un terme simple
// (verification syntaxique)
// Elle renvoie TRUE dans ce cas.
int syntax_check_list_is_a_simple_term(T_item *list_items,
												T_B0_context context)
{
  T_item * cur_list_items = list_items;
  int result = TRUE;
  while (cur_list_items != NULL)
	{
	  if (cur_list_items->syntax_check_is_a_simple_term(NULL) == FALSE)
		{
		  // erreur B0 ...;
		  B0_syntax_error(cur_list_items,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_SIMPLE_TERM)) ;
		  result = FALSE;
		}
	  if ( (context == 1) &&
		   (cur_list_items->get_B_type() != NULL) &&
		   (cur_list_items->get_B_type()->is_a_base_type() == FALSE) )
		{
		  // ERREUR : on attend un scalaire...
		  B0_semantic_error(cur_list_items,
							CAN_CONTINUE,
							get_error_msg(E_B0_SCALAR_EXPECTED),
							make_class_name(cur_list_items)) ;
		}

	  cur_list_items = (T_item *)cur_list_items->get_next() ;
	}
  return result;
}


// Fonction prenant en argument une liste d'instances de T_used_machine
// Elle parcourt la liste, et effectue pour chaque element les verifications
// sur les instanciations B0
void instanciation_check_list(T_used_machine *used_machine)
{
  T_used_machine * cur_used_machine = used_machine;

  while (cur_used_machine != NULL)
	{
	  cur_used_machine->instanciation_check();

	  cur_used_machine = (T_used_machine *) cur_used_machine->get_next();
	}
}


// Fonction prenant en argument une liste d'instances de T_item
// Elle parcourt la liste, et verifie pour chaque element si ce dernier est
// valide pour une expression d'instanciation B0
void instanciation_check_list(T_item *item)
{
  T_item * cur_item = item;
  while (cur_item != NULL)
	{
	  if ( (cur_item->syntax_check_is_a_simple_term(NULL,
													B0CTX_INSTANCIATION_CONTEXT)
			== FALSE)
		   &&
		   (cur_item->syntax_check_is_a_bool_expr(NULL) == FALSE)
		   &&
		   (cur_item->syntax_check_is_an_arith_expr(NULL,
													B0CTX_INSTANCIATION_CONTEXT)
			== FALSE)
		   &&
		   (cur_item->syntax_check_is_a_range(NULL,B0CTX_INSTANCIATION_CONTEXT)
			== FALSE))
		{
		  // ERREUR B0 : l'instanciation n'est pas valide pour le B0
		  B0_semantic_error(cur_item,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_EXPR_FOR_INSTANCIATION),
							make_class_name(cur_item));

		}

	  // Verification supplementaire, sur demande (i.e. par exemple
	  // traducteur HIA) : verifier que les instanciations ne font pas
	  // intervenir de parametres formels
	  if (get_B0_enable_restricted_formal_param_instanciation() == TRUE)
		{
		  int is_valid = FALSE ;

		  T_expr *instanciation = cur_item->make_expr() ;

		  switch (instanciation->get_node_type())
			{
			case NODE_LITERAL_INTEGER :
			  {
				is_valid = TRUE ;
				break ;
			  }
			case NODE_LITERAL_BOOLEAN :
			  {
				is_valid = TRUE ;
				break ;
			  }
			case NODE_BINARY_OP :
			  {
				T_binary_op *binop = (T_binary_op *)instanciation ;
				T_node_type type1 = binop->get_op1()->get_node_type() ;
				T_node_type type2 = binop->get_op2()->get_node_type() ;

				if (    (    (type1 == NODE_LITERAL_BOOLEAN)
			    	      || (type1 == NODE_LITERAL_INTEGER)
					    )
					 &&
					    (    (type2 == NODE_LITERAL_BOOLEAN)
					      || (type2 == NODE_LITERAL_INTEGER)
					    )
				   )
				  {
					is_valid = TRUE ;
				  }
				break ;
			  }

			default :
			  {
				// On ne fait rien
				;
			  }
			}

		  if (is_valid == FALSE)
			{
			  B0_semantic_error(cur_item,
								CAN_CONTINUE,
								get_error_msg(E_B0_TOOL_RESTRICTS_INSTANCIATION_TO_LITERALS),
								get_tool_name()->get_value()) ;


			}
		}

	  cur_item = (T_item *) cur_item->get_next();
	}
}


//
// Prend en parametre une liste d'item sur laquelle on effectue le controle
// RINIT 1-1 (pas d'identificateur non initialise en partie droite d'une
// affectation)
//
void initialisation_check_list(T_item *item,
										T_list_ident **list_ident)
{
  T_item *cur_item = item;
  while (cur_item != NULL)
	{
	  cur_item->initialisation_check(list_ident);

	  cur_item = (T_item *)cur_item->get_next();
	}
}

//
// Prend en parametre une liste d'item sur laquelle on effectue le controle
// RINIT 1-1 (pas d'identificateur non initialise en partie droite d'une
// affectation)
// Mais dans ce cas, la T_list_ident retournee est l'intersection des
// differentes T_list_ident
//
void initialisation_get_intersection(T_item *item,
											  T_list_ident **inter)
{
  // On se protege...
  if (item == NULL)
	{
	  return;
	}

  // On clone la list_ident en parametre
  T_list_ident *clone = new T_list_ident(*inter);

  T_item *cur_item = item;
  cur_item->initialisation_check(inter);

  cur_item = (T_item *)cur_item->get_next();
  while (cur_item != NULL)
	{
	  T_list_ident *cur_list = new T_list_ident(clone);
	  cur_item->initialisation_check(&cur_list);

	  // On fait l'intersection avec la liste *inter
	  (*inter)->make_intersection(cur_list);

	  // On detruit cur_list
	  delete cur_list;

	  cur_item = (T_item *)cur_item->get_next();
	}
}

#endif //B0C

int T_ident::is_a_concrete_ident(void) const
{
  int result = FALSE;

  switch (ident_type)
    {
    case ITYPE_UNKNOWN:
      // Il faut aller voir sur le père.
      ASSERT(def != NULL);
      result = def->is_a_concrete_ident();
      break;
    case ITYPE_CONCRETE_CONSTANT:
      result = TRUE;
      break;
    case ITYPE_CONCRETE_VARIABLE:
      result = TRUE;
      break;
    case ITYPE_BUILTIN:
      result =

	// Types B infinis acceptés ?
	(get_allow_unbounded_builtin_types() == TRUE &&
	  ((name == get_builtin_INTEGER()->get_name()) ||
	   (name == get_builtin_NATURAL()->get_name()) ||
	   (name == get_builtin_NATURAL1()->get_name()))) ||

	// Types B0 prédéfinis
	(name == get_builtin_NAT()->get_name()) ||
	(name == get_builtin_NAT1()->get_name()) ||
	(name == get_builtin_INT()->get_name()) ||
	(name == get_builtin_BOOL()->get_name()) ||
	(name == get_builtin_STRING()->get_name()) ||

	// Constantes B0 prédéfinies
	(name == get_builtin_MAXINT()->get_name()) ||
	(name == get_builtin_MININT()->get_name()) ||

	// Opérateurs B0
	(name == get_builtin_succ()->get_name()) ||
	(name == get_builtin_pred()->get_name()) ||
	(name == get_builtin_bool()->get_name());
      break;
    case ITYPE_MACHINE_NAME:
      break;
    case ITYPE_ABSTRACTION_NAME:
      break;
    case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
      result = TRUE;
      break;
    case ITYPE_ABSTRACT_SET:
      result = TRUE;
      break;
    case ITYPE_OP_NAME:
      result = TRUE;
      break;
    case ITYPE_LOCAL_OP_NAME:
      result = TRUE;
      break;
    case ITYPE_MACHINE_SET_FORMAL_PARAM:
      result = TRUE;
      break;
    case ITYPE_USED_MACHINE_NAME:
      break;
    case ITYPE_ENUMERATED_VALUE:
      result = TRUE;
      break;
    case ITYPE_DEFINITION_PARAM:
      break;
    case ITYPE_OP_IN_PARAM:
      result = TRUE;
      break;
    case ITYPE_OP_OUT_PARAM:
      result = TRUE;
      break;
    case ITYPE_LOCAL_VARIABLE:
      result = TRUE;
      break;
    case ITYPE_ANY_QUANTIFIER:
    case ITYPE_LOCAL_QUANTIFIER:
      break;
    case ITYPE_ABSTRACT_VARIABLE:
      break;
    case ITYPE_ABSTRACT_CONSTANT:
      break;
    case ITYPE_ENUMERATED_SET:
      result = TRUE;
      break;
    case ITYPE_USED_OP_NAME:
      break;
    case ITYPE_RECORD_LABEL:
      // Dépend du type article père.  On dit pas B0 par défaut.
      break;
    case ITYPE_PRAGMA_PARAMETER:
      break;
      // pas de default pour être prévenu en cas d'oubli d'un cas.
    }
  return result;
}

