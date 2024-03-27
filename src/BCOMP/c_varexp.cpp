/******************************* CLEARSY **************************************
* Fichier : $Id: c_varexp.cpp,v 2.0 2000-11-06 10:33:32 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions avec des variables quantifiees
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
RCS_ID("$Id: c_varexp.cpp,v 1.19 2000-11-06 10:33:32 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_lambda_expr
//
T_lambda_expr::T_lambda_expr(T_item *new_variable,
									  T_item *new_expr,
									  T_item **adr_first,
									  T_item **adr_last,
									  T_item *father,
									  T_betree *betree,
									  T_lexem *ref_lexem)
  :T_expr(NODE_LAMBDA_EXPR, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_lambda_expr::T_lambda_expr(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
	 //
	 // On a, pour %x.(Pred | Expr) :
	 //       '.'
	 //		 / \
	 //      / '()'
	 //    '%'  |     (!!! le '%' n'est pas dans l'arbre)
	 //     |  '|'
	 //    'x' / \
	 //      Pred Expr
	 //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // La variable doit etre un identificateur, ou une liste d'identificateurs,
  // eventuellement parenthese
  if (new_variable->is_an_expr() == TRUE)
	{
	  // On enleve les parentheses superflues
	  new_variable = new_variable->make_expr()->strip_parenthesis() ;
	}

  first_variable = last_variable = NULL ;
  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // On verifie que les variables sont des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable,
								  CAN_CONTINUE,
								  get_catalog(C_VAR_OF_LAMBDA_EXPR)) ;
		}
	  else
		{
		  // On en profite pour mettre a jour le type d'identificateur
		  T_ident *cur_ident = (T_ident *)cur_variable ;
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		  cur_ident->set_ref_op(op_def) ;
		  cur_ident->set_ref_machine(mach_def) ;
		}

	  cur_variable = cur_variable->get_next() ;
	}

  // L'expression en partie droite doit etre parenthesee
  if (new_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PAREXPR)) ;
	}
  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)new_expr ;
  T_expr *arg = paren->strip_parenthesis() ;

  // La lambda-expression doit etre une expression binaire ...
  if (arg->get_node_type() != NODE_BINARY_OP)
	{
	  syntax_unexpected_error(arg,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}
  T_binary_op *bop = (T_binary_op *)arg ;

  // ... avec un PIPE pour separer les predicats des expressions
  if (bop->get_operator() != BOP_PIPE)
	{
	  syntax_unexpected_error(bop,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}

  // On extrait les predicats
  T_item *pred = bop->get_op1() ;
  if (pred->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(bop->get_op1(), CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)pred ;

  // On colle l'expression
  if (bop->get_op2()->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(bop->get_op2(), FATAL_ERROR) ;
	}
  expr = bop->get_op2()->make_expr() ;
  expr->post_check() ;

  // On libere le binop
  bop->reset_operands() ;
  bop->unlink() ;

  // On recolle les father
  expr->set_father(this) ;
  predicate->set_father(this) ;

  TRACE0("fin de T_lambda_expr::T_lambda_expr") ;

}

//
//	}{	Correction de l'arbre
//
T_item *T_lambda_expr::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_lambda_expr(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	Destructeur
//
T_lambda_expr::~T_lambda_expr(void)
{
  TRACE1("T_lambda_expr::~T_lambda_expr(%x)", this) ;
}

//
//	}{	Constructeur de la classe T_sigma
//
T_sigma::T_sigma(      T_node_type new_node_type,
                                T_item *new_variable,
						  T_item *new_expr,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *father,
						  T_betree *betree,
						  T_lexem *ref_lexem)
  :T_expr(new_node_type, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_sigma::T_sigma(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
	 //
	 // On a, pour %x.(Pred | Expr) :
	 //          '.'
	 //		 /     \
	 //      /     '()'
	 //    'sigma'  |     (!!! le 'sigma' n'est pas dans l'arbre)
	 //     |      '|'
	 //    'x'     / \
	 //          Pred Expr
	 //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // La variable doit etre un identificateur, ou une liste d'identificateurs,
  // eventuellement parenthese
  if (new_variable->is_an_expr() == TRUE)
	{
	  // On enleve les parentheses superflues
	  new_variable = new_variable->make_expr()->strip_parenthesis() ;
	}

  first_variable = last_variable = NULL ;
  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // On verifie que les variables sont des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable,
								  CAN_CONTINUE,
								  get_catalog(C_VAR_OF_SIGMA)) ;
		}
	  else
		{
		  // On en profite pour mettre a jour le type d'identificateur
		  T_ident *cur_ident = (T_ident *)cur_variable ;
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		  cur_ident->set_ref_op(op_def) ;
		  cur_ident->set_ref_machine(mach_def) ;
		}

	  cur_variable = cur_variable->get_next() ;
	}

  // L'expression en partie droite doit etre parenthesee
  if (new_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PAREXPR)) ;
	}
  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)new_expr ;
  T_expr *arg = paren->strip_parenthesis() ;

  // La lambda-expression doit etre une expression binaire ...
  if (arg->get_node_type() != NODE_BINARY_OP)
	{
	  syntax_unexpected_error(arg,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}
  T_binary_op *bop = (T_binary_op *)arg ;

  // ... avec un PIPE pour separer les predicats des expressions
  if (bop->get_operator() != BOP_PIPE)
	{
	  syntax_unexpected_error(bop,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}

  // On extrait les predicats
  if (bop->get_op1()->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(bop->get_op1(), CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)bop->get_op1() ;

  // On colle l'expression
  if (bop->get_op2()->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(bop->get_op2(), FATAL_ERROR) ;
	}

  expr = bop->get_op2()->make_expr() ;
  expr->post_check() ;

  // On recolle les father
  expr->set_father(this) ;
  predicate->set_father(this) ;

  TRACE0("fin de T_sigma::T_sigma") ;

}

//
//	Destructeur
//
T_sigma::~T_sigma(void)
{
  TRACE1("T_sigma::~T_sigma(%x)", this) ;  
}

//
//	}{	Correction de l'arbre
//
T_item *T_sigma::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_sigma(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	}{	Constructeur de la classe T_pi
//
T_pi::T_pi(T_node_type new_node_type,
                                        T_item *new_variable,
					T_item *new_expr,
					T_item **adr_first,
					T_item **adr_last,
					T_item *father,
					T_betree *betree,
					T_lexem *ref_lexem)
  :T_expr(new_node_type, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_pi::T_pi(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
	 //
	 // On a, pour %x.(Pred | Expr) :
	 //          '.'
	 //		 /     \
	 //      /     '()'
	 //    'pi'  |     (!!! le 'pi' n'est pas dans l'arbre)
	 //     |      '|'
	 //    'x'     / \
	 //          Pred Expr
	 //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // La variable doit etre un identificateur, ou une liste d'identificateurs,
  // eventuellement parenthese
  if (new_variable->is_an_expr() == TRUE)
	{
	  // On enleve les parentheses superflues
	  new_variable = new_variable->make_expr()->strip_parenthesis() ;
	}

  first_variable = last_variable = NULL ;
  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // On verifie que les variables sont des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable,
								  CAN_CONTINUE,
								  get_catalog(C_VAR_OF_PI)) ;
		}
	  else
		{
		  // On en profite pour mettre a jour le type d'identificateur
		  T_ident *cur_ident = (T_ident *)cur_variable ;
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		  cur_ident->set_ref_op(op_def) ;
		  cur_ident->set_ref_machine(mach_def) ;
		}

	  cur_variable = cur_variable->get_next() ;
	}

  // L'expression en partie droite doit etre parenthesee
  if (new_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PAREXPR)) ;
	}
  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)new_expr ;
  T_expr *arg = paren->strip_parenthesis() ;

  // La lambda-expression doit etre une expression binaire ...
  if (arg->get_node_type() != NODE_BINARY_OP)
	{
	  syntax_unexpected_error(arg,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}
  T_binary_op *bop = (T_binary_op *)arg ;

  // ... avec un PIPE pour separer les predicats des expressions
  if (bop->get_operator() != BOP_PIPE)
	{
	  syntax_unexpected_error(bop,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}

  // On extrait les predicats
  if (bop->get_op1()->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(bop->get_op1(), CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)bop->get_op1() ;

  // On colle l'expression
  if (bop->get_op2()->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(bop->get_op2(), FATAL_ERROR) ;
	}

  expr = bop->get_op2()->make_expr() ;
  expr->post_check() ;

  // On recolle les father
  expr->set_father(this) ;
  predicate->set_father(this) ;

  TRACE0("fin de T_pi::T_pi") ;

}

//
//	}{	Correction de l'arbre
//
T_item *T_pi::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_pi(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	Destructeur
//
T_pi::~T_pi(void)
{
  TRACE1("T_pi::~T_pi(%x)", this) ;   
}

//
//	}{	Constructeur de la classe T_quantified_union
//
T_quantified_union::T_quantified_union(T_item *new_variable,
												T_item *new_expr,
												T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  :T_expr(NODE_QUANTIFIED_UNION, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_quantified_union::T_quantified_union(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
	 //
	 // On a, pour %x.(Pred | Expr) :
	 //          '.'
	 //		 /     \
	 //      /     '()'
	 //    'quantified_union'  |     (!!! le 'quantified_union' n'est pas dans l'arbre)
	 //     |      '|'
	 //    'x'     / \
	 //          Pred Expr
	 //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // La variable doit etre un identificateur, ou une liste d'identificateurs,
  // eventuellement parenthese
  if (new_variable->is_an_expr() == TRUE)
	{
	  // On enleve les parentheses superflues
	  new_variable = new_variable->make_expr()->strip_parenthesis() ;
	}

  first_variable = last_variable = NULL ;
  TRACE1("ici new_variable %x", new_variable) ;
  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // On verifie que les variables sont des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable,
								  CAN_CONTINUE,
								  get_catalog(C_VAR_OF_QUNION)) ;
		}
	  else
		{
		  // On en profite pour mettre a jour le type d'identificateur
		  T_ident *cur_ident = (T_ident *)cur_variable ;
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		  cur_ident->set_ref_op(op_def) ;
		  cur_ident->set_ref_machine(mach_def) ;
		}

	  cur_variable = cur_variable->get_next() ;
	}

  // L'expression en partie droite doit etre parenthesee
  if (new_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PAREXPR)) ;
	}
  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)new_expr ;
  T_expr *arg = paren->strip_parenthesis() ;

  // La lambda-expression doit etre une expression binaire ...
  if (arg->get_node_type() != NODE_BINARY_OP)
	{
	  syntax_unexpected_error(arg,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}
  T_binary_op *bop = (T_binary_op *)arg ;

  // ... avec un PIPE pour separer les predicats des expressions
  if (bop->get_operator() != BOP_PIPE)
	{
	  syntax_unexpected_error(bop,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}

  // On extrait les predicats
  if (bop->get_op1()->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(bop->get_op1(), CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)bop->get_op1() ;

  // On colle l'expression
  if (bop->get_op2()->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(bop->get_op2(), FATAL_ERROR) ;
	}

  expr = bop->get_op2()->make_expr() ;
  expr->post_check() ;

  // On re-verifie que expr est une expression (pour "de vrai")

  // On recolle les father
  expr->set_father(this) ;
  predicate->set_father(this) ;

  TRACE0("fin de T_quantified_union::T_quantified_union") ;

}

//
//	Destructeur
//
T_quantified_union::~T_quantified_union(void)
{
  TRACE1("T_quantified_union::~T_quantified_union(%x)", this) ;
}

//
//	}{	Correction de l'arbre
//
T_item *T_quantified_union::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_quantified_union(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	}{	Constructeur de la classe T_quantified_intersection
//
T_quantified_intersection::T_quantified_intersection(T_item *new_variable,
															  T_item *new_expr,
															  T_item **adr_first,
															  T_item **adr_last,
															  T_item *father,
															  T_betree *betree,
															  T_lexem *ref_lexem)
  :T_expr(NODE_QUANTIFIED_INTERSECTION, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_quantified_intersection::T_quantified_intersection(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
	 //
	 // On a, pour %x.(Pred | Expr) :
	 //          '.'
	 //		 /     \
	 //      /     '()'
	 //    'quantified_intersection'  |     (!!! le 'quantified_intersection' n'est pas dans l'arbre)
	 //     |      '|'
	 //    'x'     / \
	 //          Pred Expr
	 //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // La variable doit etre un identificateur, ou une liste d'identificateurs,
  // eventuellement parenthese
  if (new_variable->is_an_expr() == TRUE)
	{
	  // On enleve les parentheses superflues
	  new_variable = new_variable->make_expr()->strip_parenthesis() ;
	}

  first_variable = last_variable = NULL ;
  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // On verifie que les variables sont des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable,
								  CAN_CONTINUE,
								  get_catalog(C_VAR_OF_QINTER)) ;
		}
	  else
		{
		  // On en profite pour mettre a jour le type d'identificateur
		  T_ident *cur_ident = (T_ident *)cur_variable ;
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		  cur_ident->set_ref_op(op_def) ;
		  cur_ident->set_ref_machine(mach_def) ;
		}

	  cur_variable = cur_variable->get_next() ;
	}

  // L'expression en partie droite doit etre parenthesee
  if (new_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_unexpected_error(new_expr, FATAL_ERROR, get_catalog(C_PAREXPR)) ;
	}
  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)new_expr ;
  T_expr *arg = paren->strip_parenthesis() ;

  // La lambda-expression doit etre une expression binaire ...
  if (arg->get_node_type() != NODE_BINARY_OP)
	{
	  syntax_unexpected_error(arg, FATAL_ERROR, get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}
  T_binary_op *bop = (T_binary_op *)arg ;

  // ... avec un PIPE pour separer les predicats des expressions
  if (bop->get_operator() != BOP_PIPE)
	{
	  syntax_unexpected_error(bop,
							  FATAL_ERROR,
							  get_catalog(C_PREDICATE_PIPE_EXPR)) ;
	}

  // On extrait les predicats
  if (bop->get_op1()->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(bop->get_op1(), CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)bop->get_op1() ;

  // On colle l'expression
  if (bop->get_op2()->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(bop->get_op2(), FATAL_ERROR) ;
	}

  expr = bop->get_op2()->make_expr() ;
  expr->post_check() ;

  // On recolle les father
  expr->set_father(this) ;
  predicate->set_father(this) ;

  TRACE0("fin de T_quantified_intersection::T_quantified_intersection") ;

}

//
//	Destructeur
//
T_quantified_intersection::~T_quantified_intersection(void)
{
  TRACE1("T_quantified_intersection::~T_quantified_intersection(%x)", this) ;   
}

//
//	}{	Correction de l'arbre
//
T_item *T_quantified_intersection::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_quantified_intersection(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	}{	Fin du fichier
//
