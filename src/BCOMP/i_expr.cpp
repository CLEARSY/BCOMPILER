/******************************* CLEARSY **************************************
* Fichier : $Id: i_expr.cpp,v 2.0 2002-07-16 09:37:16 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Définition des méthode de vérification de type B0
*                   verification de B0
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
RCS_ID("$Id: i_expr.cpp,v 1.1 2002-07-16 09:37:16 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"
#include "i_toplvl.h"
#include "i_type.h"

//
// Fonction de vérification de type pour une expression
//
T_ident *T_expr::B0_check_expr(T_ident *expected_type,
										T_lexem *err_pos,
										T_lexem *invalid_err_pos,
										T_B0_type_context ctx)
{
  if (invalid_err_pos != NULL)
	{
	  B0_semantic_error(invalid_err_pos,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_EXPR));
	}
  else
	{
	  B0_semantic_error(this,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_EXPR));
	}
  TRACE1("T_expr::B0_check_expr type(this) = %s",
		 make_class_name(this));
  return NULL;
}

// test de référecement: retourne la première occurence de l'ident
// dans l'expression
T_ident *T_expr::B0_OM_refer_to (T_ident *ident)
{
  // L'expression est invalide on retourne donc NULL pour évité de
  // mettre u message d'erreur d'anti aliasing et pour évité d'écrire
  // pour toute les expression la méthode
  return NULL;
}

//
// Test d'expression static par defaut : TRUE pour évité en cas
// d'erreur de donner un erreur sur une expression invalide
//
int T_expr::is_static(void)
{
  return TRUE;
  TRACE1("T_expr::is_static type(this) = %s",
		 make_class_name(this));
}

//
// Fonction auxiliaire qui test si deux types scalaires sont
// compatibles. Utilisée par les méthodes B0_check_expr. Dans le cas
// contraire message d'erreur et retour à null
//
//
static void B0_check_types(T_ident *effectiv_type,
									T_ident *expected_type,
									T_lexem *err_pos)
{
  if (expected_type != NULL
	  &&
	  ident_type_semantic_equal(effectiv_type, expected_type) == FALSE
	  &&
	  err_pos != NULL)

	{
	  // On donne comme type le type renommé le plus profond
	  T_ident *renammed_type = NULL;
	  T_ident *next_homo = expected_type;
	  do
		{
		  renammed_type = next_homo;
		  next_homo = get_next_homonym_type(renammed_type);
		}
	  while(next_homo != NULL);

	  TRACE0("avant erreur");
	  B0_semantic_error(err_pos,
						CAN_CONTINUE,
						get_error_msg(E_INCOMPATIBLE_TYPES_IN_EXPR),
						effectiv_type->get_name()->get_value(),
						renammed_type->get_name()->get_value());
	}
}

// test de référecement: retourne vrai ssi l'identificateur ident
// est référencé dans l'expression
T_ident *T_ident::B0_OM_refer_to (T_ident *ident)
{
  TRACE2("T_ident(%s)::B0_OM_refer_to(%s)",
		 name->get_value(),
		 ident->get_name()->get_value());
  T_ident *result = NULL;
  if (name->compare(ident->get_name()) == TRUE)
	{
	  result = this;
	}
  return result;
}

//
// Fonction de vérification de type
//
T_ident *T_ident::B0_check_expr(T_ident *expected_type,
										 T_lexem *err_pos,
										 T_lexem *invalid_err_pos,
										 T_B0_type_context ctx)
{
  TRACE3("T_ident(%s,%p)::B0_check_expr(%s)",
		 name->get_value(),
		 this,
		 expected_type != NULL ?
		     expected_type->get_name()->get_value() : "null");
  ENTER_TRACE;
  T_ident *result = NULL;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->B0_check_expr(expected_type,
								  err_pos,
								  invalid_err_pos,
								  ctx);
	}
  else if (ident_type == ITYPE_ENUMERATED_VALUE
		   ||
		   (BOM_type != NULL
			&&
			is_a_BOM_type() == FALSE
			&&
			BOM_type->is_an_ident() == TRUE))
	{
	  TRACE0("avant make ident");
	  T_ident *ident_BOM_type;
	  if (ident_type == ITYPE_ENUMERATED_VALUE)
		{
		  ASSERT(get_father()->is_an_ident());
		  ident_BOM_type = get_father()->make_ident();
		}
	  else
		{
		  ident_BOM_type= BOM_type->make_ident();
		}
	  B0_check_types(ident_BOM_type,expected_type,err_pos);
	  result = ident_BOM_type;
  	}
  else if (BOM_type == NULL)
	{
	  //reprise sur erreur
	}
  else
	{
	  TRACE1("%s n'est pas une donnée", name->get_value());
	  // Ce n'est pas une donnée on appelle sur le père pour avoir le
	  // message d'erreur par defaut
	  T_expr::B0_check_expr(expected_type,
							err_pos,
							invalid_err_pos,
							ctx);
	}
  EXIT_TRACE;
  TRACE0("<--B0_check_expr");
  return result;
}

//
// Fonction de vérification des expressions statiques
//
int T_ident::is_static (void)
{
  if (ident_type == ITYPE_UNKNOWN)
	{
	  return def->is_static();
	}
  if (ident_type == ITYPE_ENUMERATED_VALUE
	  ||
	  ident_type == ITYPE_CONCRETE_CONSTANT)
	{
	  return TRUE;
	}
  return FALSE;
}

// test de référecement: retourne vrai ssi l'identificateur ident
// est référencé dans l'expression
T_ident *T_literal_integer::B0_OM_refer_to (T_ident *ident)
{
  return NULL;
}
//
// Vérification de type pour les littéraux entiers.
//
// Le type à été résolu lors du type check des machines.
// S'il n'est pas présent on ne crie qu'une fois
//

T_ident *T_literal_integer::B0_check_expr(T_ident *expected_type,
												   T_lexem *err_pos,
												   T_lexem *invalid_err_pos,
												   T_B0_type_context ctx)
{
  TRACE0("T_literal_integer::B0_check_expr");
  T_ident *result = NULL;
  if (literal_integer_type_checked == FALSE)
	{
	  TRACE0("type check de literal_integer_type");
	  literal_integer_type_checked = TRUE;
	  // On vérifie son existence
	  if (literal_integer_type == NULL)
		{
		  // On positionne le message sur la machine
		  T_item *father = get_father();
		  while (father->get_node_type() != NODE_MACHINE)
			{
			  father = father->get_father();
			}
		  T_ident *mach_name = ((T_machine *)father)->get_machine_name();
		  B0_semantic_error(mach_name,
							CAN_CONTINUE,
							get_error_msg(E_B0_NO_LITERAL_INTEGER_TYPE),
							mach_name->get_name()->get_value());

		}

	}
  if (literal_integer_type != NULL)
	{
	  B0_check_types(literal_integer_type, expected_type,err_pos);
	  result = literal_integer_type;
	}
  TRACE1("%s<--T_literal_integer::B0_check_expr",
		 result != NULL ? result->get_name()->get_value() :"null");
  return result;
}

//
// Fonction de vérification des expressions statiques
//
int T_literal_integer::is_static(void)
{
  return TRUE;
}

T_ident *T_literal_string::B0_OM_refer_to (T_ident *ident)
{
  return NULL;
}
T_ident *T_literal_string::B0_check_expr(T_ident *expected_type,
												  T_lexem *err_pos,
												  T_lexem *invalid_err_pos,
												  T_B0_type_context ctx)
{
  B0_check_types(get_builtin_STRING(), expected_type, err_pos);
  return get_builtin_STRING();
}

T_ident *T_literal_real::B0_OM_refer_to (T_ident *ident)
{
  return NULL;
}
T_ident *T_literal_real::B0_check_expr(T_ident *expected_type,
                                                                                                  T_lexem *err_pos,
                                                                                                  T_lexem *invalid_err_pos,
                                                                                                  T_B0_type_context ctx)
{
  B0_check_types(get_builtin_REAL(), expected_type, err_pos);
  return get_builtin_REAL();
}




// test de référecement: retourne vrai ssi l'identificateur ident
// est référencé dans l'expression
T_ident *T_literal_boolean::B0_OM_refer_to (T_ident *ident)
{
  return NULL;
}

T_ident *T_literal_boolean::B0_check_expr(T_ident *expected_type,
												   T_lexem *err_pos,
												   T_lexem *invalid_err_pos,
												   T_B0_type_context ctx)
{
  B0_check_types(get_builtin_BOOL(), expected_type, err_pos);
  return get_builtin_BOOL();
}

//
// Fonction de vérification des expressions statiques
//
int T_literal_boolean::is_static(void)
{
  return TRUE;
}

//
// Fonction de controle des agregats tableau
//
static void B0_check_indice(T_expr *indice,
						   int num)
{
  int error = FALSE;
  if (indice->get_node_type() ==  NODE_LITERAL_INTEGER)
	{
	  int val = ((T_literal_integer *)indice)->get_value()
		->get_int_value();
	  if (val != num)
		{
		  error = TRUE;
		}
	}
  else if (indice->get_node_type() == NODE_LITERAL_BOOLEAN)
	{
	  int val = ((T_literal_boolean *)indice)->get_value();
	  TRACE1("valeur de l'indice de l'elément courant %d",
			 val);
	  if (val != num)
		{
		  error = TRUE;
		}
	}
  else if(indice->is_an_ident() == TRUE)
	{
	  T_ident *enum_value = indice->make_ident();
	  while (enum_value->get_ident_type() == ITYPE_UNKNOWN)
		{
		  enum_value = enum_value->get_def();
		}

	  if (enum_value->get_ident_type() == ITYPE_ENUMERATED_VALUE)
		{
		  // calcule de l'indice de l'élément
		  int cur_indice = 0;
		  ASSERT(enum_value->get_father()->is_an_ident());
		  T_ident *enum_set = enum_value->get_father()->make_ident();
		  ASSERT(enum_set->get_ident_type() == ITYPE_ENUMERATED_SET);

		  T_ident *cur_enum_value = enum_set->get_values()->make_ident();
		  while (cur_enum_value != enum_value)
			{
			  cur_enum_value = ((T_expr *)cur_enum_value->get_next())
				->make_ident();
			  cur_indice ++;
			}
		  if (cur_indice != num)
			{
			  error = TRUE;
			}
		}
	  else
		{
		  B0_semantic_error(indice,
							CAN_CONTINUE,
							get_error_msg(E_B0_INDEX_MUST_BE_A_TERM));
		}
	}
  else
	{
	  B0_semantic_error(indice,
						CAN_CONTINUE,
						get_error_msg(E_B0_INDEX_MUST_BE_A_TERM));
	}

  if (error == TRUE)
	{
	  B0_semantic_error(indice,
						CAN_CONTINUE,
						get_error_msg(E_B0_INDICES_ORDERED));
	}
}

static void B0_check_elt(T_expr *elt,
								  T_ident *expected_type,
								  T_B0_type_context ctx,
								  int num)
{
  if (elt->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  B0_check_elt(((T_expr_with_parenthesis *)elt)->get_expr(),
				   expected_type,
				   ctx,
				   num);
	}
  else if (elt->get_node_type() == NODE_BINARY_OP)
	{
	  T_binary_op *bin_op = (T_binary_op *) elt;
	  if (bin_op->get_operator() == BOP_MAPLET
		  ||
		  bin_op->get_operator() == BOP_COMMA)
		{
		  // Vérification que l'indice correspond au i ème element
		  ASSERT(bin_op->get_op1()->is_an_expr() == TRUE);
		  B0_check_indice((T_expr *)bin_op->get_op1(), num);

		  ASSERT(bin_op->get_op2()->is_an_expr() == TRUE);
		  T_expr *expr = ((T_expr *)bin_op->get_op2());
		  expr->B0_check_expr(expected_type,
							  NULL,
							  expr->get_ref_lexem(),
							  ctx);
		}
	  else
		{
		  B0_semantic_error(elt,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE),
							make_class_name(elt));
		}
	}
  else
	{
	  B0_semantic_error(elt,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE),
						make_class_name(elt));
	}

}

static int get_card_indices (T_ident *array_type)
{
  TRACE1("get_card_indices (%s)",
		 array_type->get_name()->get_value());

  ASSERT(array_type->is_an_array_type() == TRUE);
  int result = 0;
  if (array_type->get_ident_type() == ITYPE_UNKNOWN)
	{
	  result = get_card_indices(array_type->get_def());
	}
  else
	{
	  ASSERT(array_type->get_BOM_type() != NULL);
	  if (array_type->get_BOM_type()->is_an_ident() == TRUE)
		{
		  result = get_card_indices(array_type->get_BOM_type()->make_ident());
		}
	  else
		{
		  TRACE0("on est sur la def du type tableau");
		  ASSERT(array_type->get_BOM_type()->get_node_type() == NODE_RELATION);
		  T_relation *rela = (T_relation *)array_type->get_BOM_type();

		  T_expr *src_set = rela->get_src_set();
		  TRACE1("src_set: %p", src_set);
		  if (src_set->is_an_ident() == TRUE)
			{

			  T_ident *src_ident = src_set->make_ident();
			  TRACE1("src = %s", src_ident->get_name()->get_value());
			  if (src_ident->get_ident_type() == ITYPE_UNKNOWN)
				{
				  src_ident = src_ident->get_def();
				}
			  if (src_ident->get_name() == get_builtin_BOOL()->get_name())
				{
				  TRACE0("c'est BOOL");
				  result = 2;
				}
			  else if (src_ident->get_ident_type() == ITYPE_ENUMERATED_SET)
				{
				  TRACE0("type énuméré");
				  int card = 0;
				  T_ident *cur_enum_val  = (T_ident *)src_ident->get_values();
				  while (cur_enum_val != NULL)
					{
					  cur_enum_val = (T_ident *)cur_enum_val->get_next();
					  card ++;
					}
				  TRACE1("énuméré card = %d", card);
				  result = card;
				}
			}

		  else if (src_set->is_an_expr() == TRUE
				   &&
				   src_set->get_node_type() == NODE_BINARY_OP)
			{
			  T_binary_op *bin_op = (T_binary_op *)src_set;
			  if (bin_op->get_operator() == BOP_INTERVAL)
				{
				  T_expr *bmax = bin_op->get_op2()->make_expr();
				  if (bmax->get_node_type() != NODE_LITERAL_INTEGER)
					{
					  B0_semantic_error(bmax,
										CAN_CONTINUE,
										get_error_msg(E_B0_INTERVAL_BOUND_MUST_BE_LITERAL_INTEGER),
										make_class_name(bmax));
					}
				  else
					{
					  T_literal_integer *lit_bmax = (T_literal_integer *)bmax;
					  result = lit_bmax->get_value()->get_int_value() + 1;
					  TRACE0("intervalle");
					}
				}
			}
		}
	}

  TRACE1("%d <-- get_card_indices", result);
  return result;
}


// test de référecement: retourne vrai ssi l'identificateur ident
// est référencé dans l'expression
T_ident *T_extensive_set::B0_OM_refer_to (T_ident *ident)
{
  // les indices sont des terminaux on recherche donc uniquement sur
  // la destination
  T_expr *cur_elt = first_item;
  T_ident *result = NULL;
  while (cur_elt != NULL && result == NULL)
	{
	  result = cur_elt->B0_OM_refer_to(ident);
	  cur_elt = (T_expr *)cur_elt->get_next();
	}
  return result;
}


T_ident *T_extensive_set::B0_check_expr(T_ident *expected_type,
												 T_lexem *err_pos,
												 T_lexem *invalid_err_pos,
												 T_B0_type_context ctx)

{
  TRACE1("T_extensive_set(%p)::B0_check_expr",this);
  if (ctx != B0_CTX_VALUATION
	  &&
	  ctx != B0_CTX_INIT_VAR_LOC)
	{
	  B0_semantic_error(this,
						CAN_CONTINUE,
						get_error_msg(E_B0_AGREGATE_ONLY_IN_VAR_VALUES));
	  return expected_type;
	}

  // On calcul le type de l'ensemble de destination
  if (expected_type->is_an_array_type() == TRUE)
	{
	  T_ident *dst_type = expected_type->get_dst_type();
	  // !! Attention cur_indice est utilisé pour avoir le cardinal de
	  // l'ensemble!!
	  int cur_indice = 0;
	  TRACE1("indice courant : %d", cur_indice);
	  T_expr *cur_elt = first_item;
	  while (cur_elt != NULL)
		{
		  B0_check_elt(cur_elt,
					   dst_type,
					   ctx,
					   cur_indice);
		  cur_indice ++;
		  cur_elt = (T_expr *)cur_elt->get_next();
		}
	  int nb_elem = get_card_indices(expected_type);

	  // S'il n'y a pas eu d'erreur sur l'évaluation du cardinal de
	  // l'ensemble on vérifie que l'ensemble des indices du tableaux
	  // en extension est 'complet'
	  if (nb_elem != 0
		  &&
		  nb_elem != cur_indice)
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_OM_INVALID_RANGE_FOR_ARRAY_AGGREGATE));
		}
	}
  else
	{
	  //reprise sur erreur
	}
  // Le type est par defaut le type attendu
  return expected_type;
}

int T_extensive_set::is_static(void)
{
  T_expr *cur_elt = first_item;
  int result = TRUE;

  while (cur_elt != NULL && result == TRUE)
	{
	  result = cur_elt->is_static();
	  cur_elt = (T_expr *)cur_elt->get_next();
	}
  return result;
}

//
// Classe T_expr_with_parenthesis
//

T_ident *T_expr_with_parenthesis::B0_OM_refer_to (T_ident *ident)
{
  return expr->B0_OM_refer_to(ident);
}

T_ident *T_expr_with_parenthesis::B0_check_expr(T_ident *expected_type,
														 T_lexem *err_pos,
														 T_lexem *invalid_err_pos,
														 T_B0_type_context ctx)
{
  return expr->B0_check_expr(expected_type,
							 err_pos,
							 expr->get_ref_lexem(),
							 ctx);
}
//
// Fonction de vérification des expressions statiques
//
int T_expr_with_parenthesis::is_static(void)
{
  return expr->is_static();
}
//
// Classe T_array_item
//

T_ident *T_array_item::B0_OM_refer_to (T_ident *ident)
{
  TRACE2("T_array_item(%p)::B0_OM_refer_to(%s)",
		 this,
		 ident->get_name()->get_value());
  ASSERT(array_name->is_an_expr());
  T_ident *result = NULL;

  result = array_name->make_expr()->B0_OM_refer_to(ident);
  if (result == NULL)
	{
	  T_expr *cur_index = first_index;
	  while (cur_index != NULL  && result == NULL)
		{
		  result = cur_index->B0_OM_refer_to(ident);
		  cur_index = (T_expr *)cur_index->get_next();
		}
	}
  return result;
}

//
// Controle de type sur un array item
// Deux cas: un tableau ou une opération
//
T_ident *T_array_item::B0_check_expr(T_ident *expected_type,
											  T_lexem *err_pos,
											  T_lexem *invalid_err_pos,
											  T_B0_type_context ctx)
{
  TRACE0("T_array_item::B0_check_expr");
  T_ident *result = NULL;
  // premier cas une opération
  if (array_name->is_an_ident() == TRUE
	  &&
	  array_name->make_ident()->is_an_operator() == TRUE)
	{
	  T_ident *op_name = (T_ident *)array_name;

	  T_ident *dst_type = op_name->get_dst_type();

	  // On controle la cohérence avec le type attendu
	  B0_check_types(dst_type, expected_type, err_pos);
	  // Le type de l'expression est le type de la destination
	  result = dst_type;

	  // On controle les types des indices
	  TRACE0("Vérification du premier indice indices");
	  T_ident *src1_type = op_name->get_src1_type();

	  T_expr *cur_indice = first_index;
	  cur_indice->B0_check_expr(src1_type,
								cur_indice->get_ref_lexem(),
								cur_indice->get_ref_lexem(),
								B0_CTX_GENERAL);

	  TRACE0("Vérification du deuxième indice");
	  cur_indice = (T_expr *)cur_indice->get_next();
	  if(cur_indice != NULL)
		{
		  T_ident *src2_type = op_name->get_src2_type();
		  ASSERT(src2_type != NULL);
		  cur_indice->B0_check_expr(src2_type,
									cur_indice->get_ref_lexem(),
									cur_indice->get_ref_lexem(),
									B0_CTX_GENERAL);
		}


	}
  else
	{

	  // Dans le cas d'un tableau on controle que l'indice est une
	  // expression autorisée (pas de + ...) mais le controle de type
	  // est limité au controle de type de la passe sémantique

	  T_expr *cur_indice = first_index;
	  while (cur_indice != NULL)
		{
		  cur_indice->B0_check_expr(NULL,
									NULL,
									cur_indice->get_ref_lexem(),
									B0_CTX_GENERAL);
		  cur_indice = (T_expr *)cur_indice->get_next();
		}

	  if (array_name->is_an_ident() == TRUE)
		{
		  TRACE0("tableau simple");
		  T_ident *name = array_name->make_ident();
		  if (name->get_def()->get_BOM_type() != NULL
			  &&
			  name->get_def()->get_BOM_type()->is_an_ident() == TRUE)
			{
			  T_ident *array_type = name->get_def()->get_BOM_type()->make_ident();

			  // On est sur un nom de tableau on récupère son type
			  result = array_type->get_dst_type();

			  ASSERT(result != NULL);
			  // On controle la cohérence avec le type attendu
			  B0_check_types(result, expected_type, err_pos);
			}
		  else
			{
			  // reprise sur erreur
			}
		}
	  else
		{
		  TRACE0("tableau de tableau");
		  // c'est un tableau de tableau donc on rappelle la
		  // fonction. On ne peut pas inférer sont types donc on
		  // appelle sans type attedu et sans position d'erreur
		  ASSERT(array_name->is_an_expr() == TRUE);
		  T_expr *expr =  array_name->make_expr();
		  T_ident *array_type = expr->B0_check_expr(NULL,
													NULL,
													expr->get_ref_lexem(),
													B0_CTX_GENERAL);
		  if (array_type != NULL)
			{
			  ASSERT(array_type->is_an_array_type());
			  result = array_type->get_dst_type();

			  // On controle la cohérence avec le type attendu
			  B0_check_types(result, expected_type, err_pos);
			}
		  else
			{
			  // reprise sur erreur
			}
		}
	}
  return result;
}


//
// Fonction de vérification des expressions statiques
//
int T_array_item::is_static(void)
{
  if (array_name->is_an_ident() == TRUE)
	{
	  return array_name->make_ident()->is_an_operator();
	}
  else
	{
	  ASSERT(array_name->is_an_expr() == TRUE);
	  return array_name->make_expr()->is_static();
	}
}

T_ident *T_op_result::B0_check_expr(T_ident *expected_type,
											 T_lexem *err_pos,
											 T_lexem *invalid_err_pos,
											 T_B0_type_context ctx)
{
  T_ident *result = NULL;

  if ((ref_builtin != NULL) &&
	  (ref_builtin->get_lex_type() == L_BOOL) &&
	  (nb_in_params == 1))
	{
	  result = get_builtin_BOOL();
	  B0_check_types(result,
					 expected_type,
					 err_pos);
	  ASSERT(first_in_param->is_a_predicate() == TRUE);
	  ((T_predicate *)first_in_param)->B0_OM_check();

	}
  else
	{
	  // message d'erreur par defaut
	  T_expr::B0_check_expr(expected_type, err_pos, invalid_err_pos, ctx);
	}
  return result;
}

// Test de referencement dans les expressions d'un identificateur
T_ident *T_op_result::B0_OM_refer_to(T_ident *ident)
{
  T_ident *result = NULL;
  if (ref_builtin != NULL
	  &&
	  ref_builtin->get_lex_type() == L_BOOL
	  &&
	  nb_in_params == 1)
	{
	  ASSERT(first_in_param->is_a_predicate() == TRUE);

	  result = ((T_predicate *)first_in_param)->B0_OM_refer_to(ident);
	}
  else
	{
	  // on est en erreur: op_result autre que bool en expression
	  // normalement signalée avant
	}
  return result;
}

//
// Fonction de vérification des expressions statiques
//
int T_op_result::is_static(void)
{
  if ((ref_builtin != NULL) &&
	  (ref_builtin->get_lex_type() == L_BOOL) &&
	  (nb_in_params == 1))
	{
	  ASSERT(first_in_param->is_a_predicate() == TRUE);
	  return ((T_predicate *)first_in_param)->is_static();
	}
   else
	 {
	   // on est en erreur: op_result autre que bool en expression
	   // normalement signalée avant
	 }
  return FALSE;
}



int T_binary_op::is_static(void)
{
  int result = TRUE;
  if (op1->is_an_expr() == TRUE
	  &&
	  op2->is_an_expr() == TRUE)
	{
	  result = op1->make_expr()->is_static();
	  if (result == TRUE)
		{
		  result = op2->make_expr()->is_static();
		}
	}
  return result;
}

