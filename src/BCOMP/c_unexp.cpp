/******************************* CLEARSY **************************************
* Fichier : $Id: c_unexp.cpp,v 2.0 1999-04-20 08:02:32 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions unaires
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
RCS_ID("$Id: c_unexp.cpp,v 1.8 1999-04-20 08:02:32 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_unary_op
//
T_unary_op::T_unary_op(T_item *new_operand,
								T_unary_operator new_operator,
								T_item **adr_first,
								T_item **adr_last,
								T_item *father,
								T_betree *betree,
								T_lexem *ref_lexem)
: T_expr(NODE_UNARY_OP, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_unary_op::T_unary_op(%x, %d, %x, %x, %x, %x, %x)",
		 new_operand, new_operator,
		 adr_first, adr_last, father, betree, ref_lexem) ;

  if (new_operand->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(new_operand, CAN_CONTINUE) ;
	}
  else
	{
	  operand = new_operand->make_expr() ;
	  operand->post_check() ;
	  oper = new_operator ;
	}

  // On met a jour les liens "father"
  operand->set_father(this) ;
}

#if ( defined(__INTERFACE__) || defined(__BCOMP__) )
  // Constructeur pour l'interface B0Tree->Betree
T_unary_op::T_unary_op(T_predicate *new_operand,
								T_unary_operator new_operator,
								T_item **adr_first,
								T_item **adr_last,
								T_item *father,
								T_betree *betree,
								T_lexem *ref_lexem)
: T_expr(NODE_UNARY_OP, adr_first, adr_last, father, betree, ref_lexem)
{
TRACE7("T_unary_op::T_unary_op(%x, %d, %x, %x, %x, %x, %x)",
	   new_operand, new_operator,
	   adr_first, adr_last, father, betree, ref_lexem) ;

 operand = new_operand->make_expr() ; // Charge a l'interface de se debrouiller
 oper = new_operator ;

 // On met a jour les liens "father"
 operand->set_father(this) ;
}
#endif

//
//	}{	Correction de l'arbre
//
T_item *T_unary_op::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_unary_op(%x)::check_tree", this) ;
#endif

  if (operand != NULL)
	{
	  operand->check_tree((T_item **)&operand) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

//
//	Destructeur
//
T_unary_op::~T_unary_op(void)
{
TRACE1("T_unary_op::~T_unary_op(%x)", this) ;
}

//
//	}{	Fin du fichier
//
