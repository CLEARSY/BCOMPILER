/******************************* CLEARSY **************************************
* Fichier : $Id: s_expr.cpp,v 2.0 2002-05-21 08:45:47 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions
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
RCS_ID("$Id: s_expr.cpp,v 1.13 2002-05-21 08:45:47 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

//
//	}{	Fonction qui parse les exprs
//
T_expr *syntax_get_expr(T_item **adr_first,
								 T_item **adr_last,
								 T_item *father,
								 T_betree *betree,
								 T_lexem **adr_cur_lexem)
{
  TRACE0("debut syntax_get_expr") ;

  TRACE0("Appel parseur LR") ;
  T_item *res = syntax_call_LR_parser(adr_first,
									  adr_last,
									  father,
									  betree,
									  adr_cur_lexem) ;

  if (res->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(res, CAN_CONTINUE) ;
	  return NULL ;
	}

  TRACE0(".. make_expr") ;
  T_expr *expr = res->make_expr() ;

  // Verifications de bonne construction a posteriori
  TRACE0(".. post_check") ;
  expr->post_check() ;

  TRACE0("fin syntax_get_expr") ;
  return (T_expr *)res ; // et pas expr en cas d'un T_list_link
}


//
//	}{	Fonction qui parse les instanciations
//
T_expr *syntax_get_instanciation(T_item **adr_first,
										  T_item **adr_last,
										  T_item *father,
										  T_betree *betree,
										  T_lexem **adr_cur_lexem)
{
  TRACE0("debut syntax_get_instanciation") ;
  ENTER_TRACE ;

  TRACE0("Appel parseur LR") ;
  T_item *res = syntax_call_LR_parser(adr_first,
									  adr_last,
									  father,
									  betree,
									  adr_cur_lexem) ;

  EXIT_TRACE ;
  TRACE0("fin syntax_get_instanciation") ;
  return (T_expr *)res ;
}

//
//	}{	Fonction qui parse les exprs
//
T_expr *syntax_get_simple_term(T_item **adr_first,
										T_item **adr_last,
										T_item *father,
										T_betree *betree,
										T_lexem **adr_cur_lexem)
{
  TRACE0("debut syntax_get_simple_term") ;
  ENTER_TRACE ;

  TRACE0("Appel parseur LR") ;
  T_item *res = syntax_call_LR_parser(adr_first,
									  adr_last,
									  father,
									  betree,
									  adr_cur_lexem) ;

  EXIT_TRACE ;
  TRACE0("fin syntax_get_simple_term") ;
  return (T_expr *)res ;
}

//
//	}{	Fonction qui parse les identificateurs
//
T_ident *syntax_get_ident(T_ident_type ident_type,
								   T_item **adr_first,
								   T_item **adr_last,
								   T_item *father,
								   T_betree *betree,
								   T_lexem **adr_cur_lexem)
{
#ifdef FULL_TRACE
  TRACE0("debut syntax_get_ident") ;
#endif
  ENTER_TRACE ;

  T_lex_type lex_type = (*adr_cur_lexem)->get_lex_type() ;

#ifndef NO_CHECKS
  // Verifie par l'appelant en principe
  assert ((lex_type == L_IDENT) || (lex_type == L_RENAMED_IDENT) ) ;
#endif

  T_lexem *cur_lexem = *adr_cur_lexem ;


  T_ident *res = NULL ;

  if (lex_type == L_IDENT)
	{
	  res = new T_ident(cur_lexem->get_value(),
						ident_type,
						adr_first,
						adr_last,
						father,
						betree,
						cur_lexem) ;
	}
  else
	{
	  res = new T_renamed_ident(cur_lexem->get_value(),
								ITYPE_UNKNOWN,
								adr_first,
								adr_last,
								father,
								betree,
								cur_lexem) ;
	}

  *adr_cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef FULL_TRACE
  TRACE0("fin syntax_get_ident") ;
#endif
  EXIT_TRACE ;
  return res ;
}

//
//	}{	Parse la valeur d'un ensemble en extension
// Prend la main sur le '{'
// Rend la main apres le '}'
void T_ident::parse_set_values(T_lexem **adr_cur_lexem)
{
  TRACE1("T_ident(%s)::parse_set_values", name->get_value()) ;

  T_lexem *cur_lexem = *adr_cur_lexem ;
  T_lexem *start_of_def = cur_lexem ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_OPEN_BRACES) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_OPEN_BRACES)) ;
	}

  set_ident_type(ITYPE_ENUMERATED_SET) ;
  int encore = TRUE ;
  first_value = last_value = NULL ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  T_machine *cur_mach ;
  T_op *cur_op ;

  find_machine_and_op_def(&cur_mach, &cur_op) ;
  set_ref_machine(cur_mach) ;
  set_ref_op(cur_op) ;

  while (encore == TRUE)
	{
	  // Parse un element
	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
		{
		  syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_SET_ITEM)) ;
		}

	  T_literal_enumerated_value *value ;
	  value = new T_literal_enumerated_value(cur_lexem->get_value(),
											  (T_item **)&first_value,
											  (T_item **)&last_value,
											  this,
											  get_betree(),
											  cur_lexem) ;

	  value->set_ref_machine(cur_mach) ;

	  // Parse un element
	  cur_lexem = cur_lexem->get_next_lexem() ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_CLOSE_BRACES) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_CLOSE_BRACES)) ;
	}

  *adr_cur_lexem = cur_lexem->get_next_lexem() ;


  T_literal_integer *lower = new T_literal_integer(0,
													 NULL,
													 NULL,
													 NULL,
													 get_betree(),
													 get_ref_lexem()) ;
  T_literal_enumerated_value *eval = (T_literal_enumerated_value *)last_value ;
  T_literal_integer *upper = new T_literal_integer(eval->get_value(),
													 NULL,
													 NULL,
													 NULL,
													 get_betree(),
													 get_ref_lexem()) ;
  T_enumerated_type *etype = new T_enumerated_type(this,
												   lower,
												   upper,
												   NULL,
												   NULL,
												   get_ref_lexem()) ;
  lower->unlink() ;
  upper->unlink() ;

  T_setof_type *real_type = new T_setof_type(etype,
											 this,
											 get_betree(),
											 get_ref_lexem()) ;
  etype->unlink() ;

  TRACE5("real = %x : Setof(etype(%x) : <ident %x, lower %x, upper %x>",
		 real_type,
		 etype,
		 eval,
		 lower,
		 upper) ;

  set_B_type(real_type, start_of_def) ;
  real_type->unlink() ;

  T_literal_enumerated_value *cur_val = (T_literal_enumerated_value *)first_value ;
  while (cur_val != NULL)
	{
	  cur_val->set_B_type(etype, cur_val->get_ref_lexem()) ;
	  cur_val = (T_literal_enumerated_value *)cur_val->get_next() ;
	}
}

//
//	}{	Fin du fichier
//
