/******************************* CLEARSY **************************************
* Fichier : $Id: i_decl.cpp,v 2.0 2002-09-24 09:45:01 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Fonctions et méthodes de controle B0 des déclarations
*
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
RCS_ID("$Id: i_decl.cpp,v 1.2 2002-09-24 09:45:01 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"
#include "i_type.h"

//
// Fonction de vérification que les identificzateurs n'ont pas '__'
// dans leurs nom
//

void B0_OM_name_check(T_ident *name)
{
  TRACE1("B0_OM_name_check(%s)",
		 name->get_name()->get_value());
  const char *occur = strstr(name->get_name()->get_value(), "__");
  if (occur != NULL)
	{
	  B0_semantic_error(name,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_IDENT),
						name->get_name()->get_value());
	}
  TRACE0("<-B0_OM_name_check");
}

//
// test si l'item passé en paramètre en un type utilisateur valide
// pour les donnnées B0-OM
// (En dehors de STRING)
//
static int is_valid_B0_data_type(T_item *item_type)
{
  TRACE1("is_valid_B0_data_type(%p)",item_type);
  int result = FALSE;
  if (item_type->is_an_ident() == TRUE)
	{
	  T_ident *ident_BOM_type = item_type->make_ident();
	  if (ident_BOM_type->get_ident_type() == ITYPE_UNKNOWN)
		{
		  // On remonte sur la définition
		  ASSERT(ident_BOM_type->get_def() != NULL);
		  result = is_valid_B0_data_type(ident_BOM_type->get_def());
		}
	  else
		{
		  // Le type est BOOL
		  if (ident_BOM_type->get_name() == get_builtin_BOOL()->get_name())
			{
			  result = TRUE;
			}
		  // Le type est une constante ou un ensemble
		  if (ident_BOM_type->get_ident_type() == ITYPE_CONCRETE_CONSTANT
			  ||
			  ident_BOM_type->get_ident_type() == ITYPE_ABSTRACT_SET
			  ||
			  ident_BOM_type->get_ident_type() == ITYPE_ENUMERATED_SET)
			{
			  result = TRUE;
			}
		}

	}
  return result;
}

//
// test si l'item passé en paramètre en un type utilisateur valide
// pour les indices de tableaux B0-OM
//
static int is_valid_B0_rang_type(T_item *item_type)
{
  TRACE1("is_valid_B0_data_type(%p)",item_type);
  int src_OK = FALSE;
  if (item_type->is_an_ident() == TRUE)
	{
	  T_ident *ident_BOM_type = item_type->make_ident();
	  if (ident_BOM_type->get_ident_type() == ITYPE_UNKNOWN)
		{
		  // On remonte sur la définition
		  ASSERT(ident_BOM_type->get_def() != NULL);
		  src_OK = is_valid_B0_rang_type(ident_BOM_type->get_def());
		}
	  else
		{
		  // Le type est BOOL
		  if (ident_BOM_type->get_name() == get_builtin_BOOL()->get_name())
			{
			  src_OK = TRUE;
			}
		  // Le type est un ensemble enuméré
		  if (ident_BOM_type->get_ident_type() == ITYPE_ENUMERATED_SET)
			{
			  src_OK = TRUE;
			}
		}

	}
  else if (item_type->is_an_expr())
	{
	  T_expr *src = item_type->make_expr();

	  if (src->get_node_type() == NODE_BINARY_OP)
		{
		  T_binary_op *bin_op = (T_binary_op *)src;
		  if (bin_op->get_operator() == BOP_INTERVAL)
			{
			  // la borne min doit etre 0
			  T_expr *bmin = bin_op->get_op1()->make_expr();
			  int bmin_OK = FALSE;
			  if (bmin->get_node_type() == NODE_LITERAL_INTEGER)
				{
				  T_literal_integer *lit_bmin = (T_literal_integer *)bmin;
				  int v_bmin = lit_bmin->get_value()->get_int_value();
				  bmin_OK = (v_bmin == 0);
				}

			  TRACE1("bmin_OK = %s", bmin_OK == TRUE ? "true" : "false");
			  T_expr *bmax = bin_op->get_op2()->make_expr();

			  // On vérifie que l'expression est valide pour la borne
			  // max
			  int bmax_OK;
			  if (bmax->get_node_type() != NODE_LITERAL_INTEGER)
				{
				  T_ident *type = bmax->B0_check_expr(NULL, // pas de type attendu
													  NULL,
													  NULL,
													  B0_CTX_GENERAL);
				  bmax_OK = (type!=NULL);

				  // Si le type est non null (pas d'erreur ) on vérifie
				  // que l'expression est static
				  if (type != NULL
					  &&
					  bmax->is_static() == FALSE)
					{
					  B0_semantic_error(bmax,
										CAN_CONTINUE,
										get_error_msg(E_B0_EXPR_STATIC));
					  bmax_OK = FALSE;
					}
				}
			  else
				{
				  // dans le cas d'un entier literal on considère que
				  // la borne max est bonne. On ne lance pas une
				  // vérification de l'expression sur un entier
				  // literal sinon on a éventuellement une erreur du
				  // au fait que la machine de base définissant le
				  // type des entiers literal n'est pas accéssible
				  bmax_OK = TRUE;
				}
			  TRACE1("bmax_OK = %s", bmax_OK == TRUE ? "true" : "false");
			  src_OK =(bmin_OK == TRUE
					   &&
					   bmax_OK == TRUE);
			}
		  else
			{
			  TRACE2("bin_op = %p; type = %d",
					 bin_op,
					 bin_op->get_operator());
			}
		}
	}
  return src_OK;
}

static int is_valid_B0_user_array(T_item *item_type)
{
  int result = FALSE;
  ASSERT(item_type->is_an_expr());
  T_expr *expr_type = item_type->make_expr();
  // fonction partielle
  if (expr_type->get_node_type() == NODE_RELATION)
	{
	  T_relation *arr_type = (T_relation *)expr_type;
	  if (arr_type->get_relation_type() == RTYPE_TOTAL_FUNCTION)
		{
		  T_expr *src = arr_type->get_src_set();
		  int src_OK = is_valid_B0_rang_type(src);

		  TRACE1("src_OK = %s", src_OK == TRUE ? "true" : "false");

		  // Vérification de l'ensemble d'arrivé
		  T_expr *dst = arr_type->get_dst_set();
		  int dst_OK = dst->is_an_ident();
		  TRACE1("dst_OK = %s", dst_OK == TRUE ? "true" : "false");

		  result = (src_OK == TRUE
					&&
					dst_OK == TRUE);

		}
	}
  TRACE1("%s<-user_array", result == TRUE ? "true" : "false");
  return result;
}

//
// Test si l'item est un opérateur valide
//
static int is_valid_B0_basic_array_type(T_item *item_type,
												 T_ident *ident)
{
  TRACE2("is_valid_B0_basic_array_type(%p,%p)",
		 item_type,
		 ident);

  int result = FALSE;

  T_machine *root = ident->get_ref_machine();
  if (root->is_basic_translator_machine() == TRUE)
	{
	  ASSERT(item_type->is_an_expr());
	  T_expr *expr_type = item_type->make_expr();

	  // fonction partielle ou totale
	  if (expr_type->get_node_type() == NODE_RELATION)
		{
		  T_relation *arr_type = (T_relation *)expr_type;
		  if (arr_type->get_relation_type() == RTYPE_TOTAL_FUNCTION
			  ||
			  arr_type->get_relation_type() == RTYPE_PARTIAL_FUNCTION)
			{
			  // vérification de l'ensemble d'indice
			  int src_OK = FALSE;
			  T_expr *src = arr_type->get_src_set();
			  if (src->is_an_ident() == TRUE)
				{
				  src_OK = is_valid_B0_data_type(src);
				}
			  else if (src->get_node_type() == NODE_BINARY_OP)
				{
				  T_binary_op *bin_op = (T_binary_op *)src;
				  if (bin_op->get_operator() == BOP_TIMES)
					{
					  TRACE1("produit cartésien en ensemble source %p",
							 bin_op);

					  ASSERT(bin_op->get_op1()->is_an_expr() == TRUE);
					  ASSERT(bin_op->get_op2()->is_an_expr() == TRUE);
					  T_expr *f_set = bin_op->get_op1()->make_expr();
					  T_expr *l_set = bin_op->get_op2()->make_expr();

					  src_OK = (is_valid_B0_data_type(f_set) == TRUE
								&&
								is_valid_B0_data_type(l_set) == TRUE);
					}
				}

			  TRACE1("src_OK == %s", src_OK == TRUE ? "true" : "false");

			  int dst_OK = is_valid_B0_data_type(arr_type->get_dst_set());
			  TRACE1("dst_OK == %s", dst_OK == TRUE ? "true" : "false");

			  // La vérification est OK ssi la source est la
			  // destination sont OK
			  result = (src_OK == TRUE && dst_OK == TRUE);

			}
		}

	}
  return result;
}

//
// Fonction qui test si l'expression peut définir un type B0 de base
// Un seul cas l'intervalle donné par deux littéraux
//
static int is_valid_B0_basic_intervalle_type(T_item *item_type,
													  T_ident *ident)
{
  ASSERT(ident != NULL);
  TRACE2("is_valid_B0_basic_intervalle_type(item_type %p,%s",
		 item_type,
		 ident->get_name()->get_value());
  int result = FALSE;
  ASSERT(item_type->is_an_expr());
  T_machine *root = ident->get_ref_machine();
  if (root->is_basic_translator_machine() == TRUE)
	{
	  ASSERT(item_type->is_an_expr());
	  T_expr *expr_type = item_type->make_expr();

	  if (expr_type->get_node_type() ==  NODE_BINARY_OP)
		{
		  T_binary_op *bin_op = (T_binary_op *)expr_type;
		  if (bin_op->get_operator() == BOP_INTERVAL)
			{
			  // vérification de la forme litéral..litéral
			  T_expr *bmin = bin_op->get_op1()->make_expr()->strip_parenthesis();
			  int bmin_OK = (bmin->get_node_type() == NODE_LITERAL_INTEGER);

			  T_expr *bmax = bin_op->get_op2()->make_expr()->strip_parenthesis();
			  int bmax_OK = (bmax->get_node_type() == NODE_LITERAL_INTEGER);

			  result = (bmin_OK == TRUE
						&&
						bmax_OK == TRUE);
			}
		}
	}
  else
	{
	  TRACE0("intervalle non dans une machine de base");
	}
  TRACE1("%s<--is_valid_B0_basic_intervalle_type()",
		 result == TRUE ? "true":"false");
  return result;
}
//
// Vérification des données HORS MACHINE DE BASE
//
static void B0_check_decl_data(T_ident *ident)
{
  TRACE1("B0_check_decl_data(%s)",
		 ident->get_name()->get_value());
  T_error_code error_code = (T_error_code) 0;
  T_error_code error_expected = (T_error_code)0;
  int error = FALSE;
  switch (ident->get_ident_type())
	{

	case ITYPE_CONCRETE_CONSTANT:
	  if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE
		  &&
		  is_valid_B0_basic_array_type(ident->get_BOM_type(), ident) == FALSE)
		{
		  error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT;
		  error_expected = E_B0_TYPE_DATA_EXPECTED;
		  error = TRUE;
		}
	  break;
	case ITYPE_CONCRETE_VARIABLE:
	  if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE)
		{
		  error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE;
		  error_expected = E_B0_TYPE_DATA_EXPECTED;
		  error = TRUE;
		}
	  break;
	case ITYPE_LOCAL_VARIABLE:
	  if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE)
		{
		  error_code = E_B0_INVALID_TYPING_EXPR_FOR_LOCAL_VARIABLE;
		  error_expected = E_B0_TYPE_DATA_EXPECTED;
		  error = TRUE;
		}
	  else
		{
		  T_ident *ident_BOM_type = ident->get_BOM_type()->make_ident();
		  // Verification de l'initialisation éventuelle
		  if (ident->get_BOM_init_value() == NULL)
			{
			  if (ident_BOM_type->is_a_BOM_complete_type() == FALSE)
				{
				  B0_semantic_error(ident,
									CAN_CONTINUE,
									get_error_msg(E_B0_LOC_VAR_MUST_INITIALISED),
									ident->get_name()->get_value());
				}
			}
		  else
			{
			  // verification de cohérence de type
			  T_expr *init_expr =  ident->get_BOM_init_value()->make_expr();
			  T_ident *type;
			  type = init_expr->B0_check_expr(ident_BOM_type->make_ident(),
											  ident->get_BOM_lexem_type(),
											  init_expr->get_ref_lexem(),
											  B0_CTX_INIT_VAR_LOC);
			  if (type != NULL
				  &&
				  ident_BOM_type->make_ident()->is_an_array_type() == TRUE)
				{
				  // On vérifie que la valeur d'initialisation est
				  // static
				  int is_sta = init_expr->is_static();
				  if (is_sta == FALSE)
					{
					  B0_semantic_error(init_expr,
										CAN_CONTINUE,
										get_error_msg(E_B0_EXPR_STATIC));
					}
				}
			}
		}
	  break;
	case ITYPE_OP_OUT_PARAM:
	  if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE)
		{
		  error_code = E_B0_INVALID_TYPING_EXPR_FOR_OP_OUT_PARAM;
		  error_expected = E_B0_TYPE_DATA_EXPECTED;
		  error = TRUE;
		}
	  break;
	case ITYPE_OP_IN_PARAM:
	  if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE
		  &&
		  (ident->get_BOM_type()->is_an_ident() == FALSE
		   ||
		   ident->get_BOM_type()->make_ident()->get_name()
		      != get_builtin_STRING()->get_name()))
		{
		  error_code = E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER;
		  error_expected = E_B0_TYPE_INPUT_PARAM_EXPECTED;
		  error = TRUE;
		}
	  break;
	default :
	  ASSERT(0);
	}


  if (error ==TRUE)
	{
	  B0_semantic_error(ident,
						MULTI_LINE,
						get_error_msg(error_code),
						make_class_name(ident),
						ident->get_name()->get_value());
	  B0_semantic_error(ident->get_BOM_type(),
						CAN_CONTINUE,
						get_error_msg(error_expected));
	}
  TRACE0("<-B0_check_decl_data");
}


static void B0_check_decl_type(T_ident *ident)
{
  TRACE1("B0_check_decl_type(%s)", ident->get_name()->get_value());
  // On ne fait que des vérifs pour les constantes concrètes et les
  // ensembles éumérés, autres cas n'ont pas besoins de vérification
  // (SET, BOOL)
  switch (ident->get_ident_type())
	{
	case ITYPE_CONCRETE_CONSTANT :
	  {

		//
		// Cas valide :
		//     renommage de type
		//     un type intervalle dans la machine de base
		//     un type tableau utilisateur
		if (is_valid_B0_data_type(ident->get_BOM_type()) == FALSE
			&&
			is_valid_B0_basic_intervalle_type(ident->get_BOM_type(),
											  ident) == FALSE
			&&
			is_valid_B0_user_array(ident->get_BOM_type()) == FALSE)
		  {
			B0_semantic_error(ident,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_TYPE_DECL),
							  ident->get_name()->get_value());
		  }
	  }
	case ITYPE_ENUMERATED_SET :
	  {
		T_ident *cur_enum_value = (T_ident *)ident->get_first_value();
		while (cur_enum_value != NULL)
		  {
			B0_OM_name_check ( cur_enum_value );
			cur_enum_value = (T_ident *)cur_enum_value->get_next();
		  }
	  }
	default :{}
	}
}

//
// Fonction de vérification des déclarations B0 d'un identificateur
//

void T_ident::B0_check_decl()
{
  TRACE2("T_ident(%s,%p)::B0_check_decl",
		 name->get_value(),
		 this);
  // La fonction n'a de sens que pour les déclarations:
  //   de variables globales et locales
  //   de constantes concrètes
  //   de paramètres d'opération
  //   d'ensembles diféré
  //   d'ensembles éumérés

  ASSERT(ident_type == ITYPE_CONCRETE_VARIABLE
		 ||
		 ident_type == ITYPE_LOCAL_VARIABLE
		 ||
		 ident_type == ITYPE_CONCRETE_CONSTANT
		 ||
		 ident_type == ITYPE_OP_IN_PARAM
		 ||
		 ident_type == ITYPE_OP_OUT_PARAM
		 ||
		 ident_type == ITYPE_ABSTRACT_SET
		 ||
		 ident_type == ITYPE_ENUMERATED_SET);

  // Deux cas suivant que l'ident est explicitement déclaré ou s'il
  // est importé
  if (explicitly_declared == TRUE)
	{
	  B0_OM_name_check(this);
	  // On vérifie la déclaration
	  if (is_a_BOM_type() == TRUE)
		{
		  B0_check_decl_type(this);
		}
	  else
		{
		  B0_check_decl_data(this);
		}
	}
  else
	{
	  // l'ident est importé, on vérifie la cohérence de la double
	  // importation éventuelle des données concrètes typées
	  // explicitement c'est à dire les variables et les constantes
	  // concrètes.
	  if (homonym_in_abstraction != NULL
		  &&
		  homonym_in_required_mach != NULL
		  &&
		  homonym_in_abstraction->get_BOM_type() != NULL
		  &&
		  homonym_in_required_mach->get_BOM_type() != NULL)

		{
		  T_expr *abs_type = homonym_in_abstraction->
			get_BOM_type()->make_expr();
		  T_expr *req_type = homonym_in_required_mach->
			get_BOM_type()->make_expr();

		  if (expr_type_semantic_equal(abs_type,
									   req_type)
			  == FALSE)
			{
			  B0_semantic_error(BOM_type,
								MULTI_LINE,
								get_error_msg(E_B0_INCOMPATIBLE_DECLARATIONS),
								name->get_value());
			  B0_semantic_error(abs_type,
								CAN_CONTINUE,
								get_error_msg(E_B0_LOCATION_OF_FIRST_DECLARATION));
			}
		}
	}
  TRACE0("<--T_ident::B0_check_decl");
}

