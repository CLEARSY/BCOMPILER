/******************************* CLEARSY **************************************
* Fichier : $Id: i_pred.cpp,v 2.0 2002-07-16 09:37:16 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Fonction de vérification des prédicat B0 OM
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
RCS_ID("$Id: i_pred.cpp,v 1.1 2002-07-16 09:37:16 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"
#include "i_type.h"


//
// méthode par défaut de control B0 : erreur
//

void T_predicate::B0_OM_check(void)
{

  B0_semantic_error (this,
					 CAN_CONTINUE,
					 get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
					 make_class_name(this));
}


//
// Fonction de vérification des predicats statiques
//
int T_predicate::is_static(void)
{
  return FALSE;
}


// test de référencement: return NULL par defaut ie c'est une erreur
// mais signalée précédemment inutil de signalé une erreur d'aliasing
T_ident *T_predicate::B0_OM_refer_to (T_ident *ident)
{
  return NULL;
}

void T_predicate_with_parenthesis::B0_OM_check(void)
{
  predicate->B0_OM_check();
}

int T_predicate_with_parenthesis::is_static(void)
{
  return predicate->is_static();
}

T_ident *T_predicate_with_parenthesis::B0_OM_refer_to(T_ident *ident)
{
  return predicate->B0_OM_refer_to(ident);
}
void T_not_predicate::B0_OM_check(void)
{
  predicate->B0_OM_check();
}

T_ident *T_not_predicate::B0_OM_refer_to(T_ident *ident)
{
  return predicate->B0_OM_refer_to(ident);
}

int T_not_predicate::is_static(void)
{
  return predicate->is_static();
}

void T_typing_predicate::B0_OM_check(void)
{
  TRACE0("T_typing_predicate::B0_OM_check");
  if (typing_predicate_type == TPRED_EQUAL)
	{
	  ASSERT(first_ident == last_ident
			 &&
			 first_ident->is_an_expr() == TRUE);

	  T_ident *lhs_type = first_ident->make_expr()
		->B0_check_expr(NULL,
						first_ident->get_ref_lexem(),
						first_ident->get_ref_lexem(),
						B0_CTX_GENERAL);

	  ASSERT(type->is_an_expr() == TRUE);
	  T_ident *rhs_type = type->make_expr()->
		B0_check_expr(NULL,
					  type->get_ref_lexem(),
					  type->get_ref_lexem(),
					  B0_CTX_GENERAL);

	  if (rhs_type != NULL
		  &&
		  lhs_type != NULL
		  &&
		  ident_type_semantic_equal(rhs_type, lhs_type) == FALSE)
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
							lhs_type->get_name()->get_value(),
							rhs_type->get_name()->get_value());
		}
	}
  else
	{
	  T_predicate::B0_OM_check();
	}
}

int T_typing_predicate::is_static(void)
{
  if (typing_predicate_type == TPRED_EQUAL)
	{
	  ASSERT(first_ident == last_ident
			 &&
			 first_ident->is_an_expr() == TRUE
			 &&
			 type->is_an_expr());

	  T_expr *lhs_expr = first_ident->make_expr();
	  T_expr *rhs_expr = type->make_expr();
	  if (lhs_expr->is_static() == TRUE
		  &&
		  rhs_expr->is_static() == TRUE)
		{
		  return TRUE;
		}
	}
  return FALSE;
}


// Test de referencement dans les predicats B0 de l'opérateur bool
T_ident *T_typing_predicate::B0_OM_refer_to(T_ident *ident)
{
  T_ident *result = NULL;
  if (typing_predicate_type == TPRED_EQUAL)
	{
	  ASSERT(first_ident == last_ident
			 &&
			 first_ident->is_an_expr() == TRUE
			 &&
			 type->is_an_expr());

	  T_expr *lhs_expr = first_ident->make_expr();
	  T_expr *rhs_expr = type->make_expr();
	  result = lhs_expr->B0_OM_refer_to(ident);
	  if (result == NULL)
	    {
	      result = rhs_expr->B0_OM_refer_to(ident);
	    }
	}
  else
	{
	  // prédicat non B0
	}
  return result;
}

void T_expr_predicate::B0_OM_check(void)
{
  if (predicate_type == EPRED_DIFFERENT
	  ||
	  predicate_type == EPRED_LESS_THAN
	  ||
	  predicate_type == EPRED_GREATER_THAN
	  ||
	  predicate_type == EPRED_LESS_THAN_OR_EQUAL
	  ||
          predicate_type == EPRED_GREATER_THAN_OR_EQUAL)
	{

	  T_ident *lhs_type = expr1->B0_check_expr(NULL,
											   expr1->get_ref_lexem(),
											   expr1->get_ref_lexem(),
											   B0_CTX_GENERAL);

	  T_ident *rhs_type = expr2->B0_check_expr(NULL,
											   expr2->get_ref_lexem(),
											   expr2->get_ref_lexem(),
											   B0_CTX_GENERAL);

	  if (rhs_type != NULL
		  &&
		  lhs_type != NULL
		  &&
		  ident_type_semantic_equal(rhs_type, lhs_type) == FALSE)
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
							lhs_type->get_name()->get_value(),
							rhs_type->get_name()->get_value());
		}
	}
  else
	{
	  T_predicate::B0_OM_check();
	}
}

int T_expr_predicate::is_static(void)
{
  if (expr1->is_static() == TRUE
	  &&
	  expr2->is_static() == TRUE)
	{
	  return TRUE;
	}
  return FALSE;
}

T_ident *T_expr_predicate::B0_OM_refer_to(T_ident *ident)
{
  T_ident *result = expr1->B0_OM_refer_to(ident);
  if (result == NULL)
	{
	  result = expr1->B0_OM_refer_to(ident);
	}
  return result;
}

void T_binary_predicate::B0_OM_check(void)
{

  if (predicate_type == BPRED_CONJONCTION
	  ||
	  predicate_type == BPRED_OR)
	{
	  pred1->B0_OM_check();
	  pred2->B0_OM_check();
	}
  else
	{
	  T_predicate::B0_OM_check();
	}

}
T_ident *T_binary_predicate::B0_OM_refer_to(T_ident *ident)
{
  T_ident *result = pred1->B0_OM_refer_to(ident);
  if (result == NULL)
	{
	  result = pred2->B0_OM_refer_to(ident);
	}
  return result;
}


int T_binary_predicate::is_static(void)
{
  if (pred1->is_static() == TRUE
	  &&
	  pred2->is_static() == TRUE)
	{
	  return TRUE;
	}
  return FALSE;
}





