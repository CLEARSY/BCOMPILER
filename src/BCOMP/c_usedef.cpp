/******************************* CLEARSY **************************************
* Fichier : $Id: c_usedef.cpp,v 2.0 2001-07-19 16:24:44 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Gestion des fonctions utilisateurs
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
RCS_ID("$Id: c_usedef.cpp,v 1.24 2001-07-19 16:24:44 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

static user_defined_1 *switch_f1 = NULL ;
static user_defined_2 *switch_f2 = NULL ;
static user_defined_3 *switch_f3 = NULL ;
static user_defined_4 *switch_f4 = NULL ;
static user_defined_5 *switch_f5 = NULL ;

extern size_t msg_get_number_of_nodes(void) ;

// Debut macro init
#define INIT(X,Y) \
if (X != NULL) { delete [] X ; } ; \
X = new Y[msg_get_number_of_nodes()] ; \
memset(X, 0, msg_get_number_of_nodes() * sizeof(Y)) ;
// Fin Macro init

// Initilisation individuelle de chaque aiguillage
void reset_user_defined_f1_switch(void)
{
INIT(switch_f1, user_defined_1) ;
}

void reset_user_defined_f2_switch(void)
{
INIT(switch_f2, user_defined_2) ;
}

void reset_user_defined_f3_switch(void)
{
INIT(switch_f3, user_defined_3) ;
}

void reset_user_defined_f4_switch(void)
{
INIT(switch_f4, user_defined_4) ;
}

void reset_user_defined_f5_switch(void)
{
INIT(switch_f5, user_defined_5) ;
}

// Initialisation des "aiguillages"
void reset_user_defined_switches(void)
{
TRACE0("reset_user_defined_switches") ;

reset_user_defined_f1_switch() ;
reset_user_defined_f2_switch() ;
reset_user_defined_f3_switch() ;
reset_user_defined_f4_switch() ;
reset_user_defined_f5_switch() ;
}

// Liberation des "aiguillages"
void delete_user_defined_switches(void)
{
  TRACE0("delete_user_defined_switches") ;

  if (switch_f1 != NULL)
	{
	  delete [] switch_f1 ;
	  switch_f1 = NULL ;
	}
  if (switch_f2 != NULL)
	{
	  delete [] switch_f2 ;
	  switch_f2 = NULL ;
	}
  if (switch_f3 != NULL)
	{
	  delete [] switch_f3 ;
	  switch_f3 = NULL ;
	}
  if (switch_f4 != NULL)
	{
	  delete [] switch_f4 ;
	  switch_f4 = NULL ;
	}
  if (switch_f5 != NULL)
	{
	  delete [] switch_f5 ;
	  switch_f5 = NULL ;
	}
}

int T_lexem::user_defined_f1(void)
{
  return ((switch_f1[NODE_LEXEM] != NULL) ?
		  ((switch_f1[NODE_LEXEM])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_lexem::user_defined_f2(void)
{
  return ((switch_f2[NODE_LEXEM] != NULL) ?
		  ((switch_f2[NODE_LEXEM])(this)) : T_object::user_defined_f2()) ;
}

int T_object::user_defined_f1(void)
{
  assert (switch_f1[NODE_OBJECT] != NULL) ;
  return switch_f1[NODE_OBJECT](this) ;
}

T_object *T_object::user_defined_f2(void)
{
  assert (switch_f2[NODE_OBJECT] != NULL) ;
  return switch_f2[NODE_OBJECT](this) ;
}

int T_betree::user_defined_f1(void)
{
  return ((switch_f1[NODE_BETREE] != NULL) ?
		  ((switch_f1[NODE_BETREE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_betree::user_defined_f2(void)
{
  return ((switch_f2[NODE_BETREE] != NULL) ?
		  ((switch_f2[NODE_BETREE])(this)) : T_item::user_defined_f2()) ;
}

int T_machine::user_defined_f1(void)
{
  return ((switch_f1[NODE_MACHINE] != NULL) ?
		  ((switch_f1[NODE_MACHINE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_machine::user_defined_f2(void)
{
  return (switch_f2[NODE_MACHINE] != NULL) ?
	(switch_f2[NODE_MACHINE])(this) : T_item::user_defined_f2() ;
}

int T_item::user_defined_f1(void)
{
  return ((switch_f1[NODE_ITEM] != NULL) ?
		  ((switch_f1[NODE_ITEM])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_item::user_defined_f2(void)
{
  return ((switch_f2[NODE_ITEM] != NULL) ?
		  ((switch_f2[NODE_ITEM])(this)) : T_object::user_defined_f2()) ;
}

int T_symbol::user_defined_f1(void)
{
  return ((switch_f1[NODE_SYMBOL] != NULL) ?
		  ((switch_f1[NODE_SYMBOL])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_symbol::user_defined_f2(void)
{
  return ((switch_f2[NODE_SYMBOL] != NULL) ?
		  ((switch_f2[NODE_SYMBOL])(this)) : T_object::user_defined_f2()) ;
}

int T_op::user_defined_f1(void)
{
  return ((switch_f1[NODE_OPERATION] != NULL) ?
		  ((switch_f1[NODE_OPERATION])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_op::user_defined_f2(void)
{
  return ((switch_f2[NODE_OPERATION] != NULL) ?
		  ((switch_f2[NODE_OPERATION])(this)) : T_item::user_defined_f2()) ;
}

int T_substitution::user_defined_f1(void)
{
  return ((switch_f1[NODE_SUBSTITUTION] != NULL) ?
		  ((switch_f1[NODE_SUBSTITUTION])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_substitution::user_defined_f2(void)
{
  return ((switch_f2[NODE_SUBSTITUTION] != NULL) ?
		  ((switch_f2[NODE_SUBSTITUTION])(this)) : T_item::user_defined_f2()) ;
}

int T_typing_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_TYPING_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_TYPING_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_typing_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_TYPING_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_TYPING_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_existential_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXISTENTIAL_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_EXISTENTIAL_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_existential_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXISTENTIAL_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_EXISTENTIAL_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_universal_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_UNIVERSAL_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_UNIVERSAL_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_universal_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_UNIVERSAL_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_UNIVERSAL_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_expr_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXPR_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_EXPR_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_expr_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXPR_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_EXPR_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_binary_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_BINARY_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_BINARY_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_binary_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_BINARY_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_BINARY_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_not_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_NOT_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_NOT_PREDICATE])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_not_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_NOT_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_NOT_PREDICATE])(this)) : T_predicate::user_defined_f2()) ;
}

int T_predicate_with_parenthesis::user_defined_f1(void)
{
  return ((switch_f1[NODE_PREDICATE_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f1[NODE_PREDICATE_WITH_PARENTHESIS])(this)) : T_predicate::user_defined_f1()) ;
}

T_object *T_predicate_with_parenthesis::user_defined_f2(void)
{
  return ((switch_f2[NODE_PREDICATE_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f2[NODE_PREDICATE_WITH_PARENTHESIS])(this)) : T_predicate::user_defined_f2()) ;
}

int T_predicate::user_defined_f1(void)
{
  return ((switch_f1[NODE_PREDICATE] != NULL) ?
		  ((switch_f1[NODE_PREDICATE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_predicate::user_defined_f2(void)
{
  return ((switch_f2[NODE_PREDICATE] != NULL) ?
		  ((switch_f2[NODE_PREDICATE])(this)) : T_item::user_defined_f2()) ;
}

int T_converse::user_defined_f1(void)
{
  return ((switch_f1[NODE_CONVERSE] != NULL) ?
		  ((switch_f1[NODE_CONVERSE])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_converse::user_defined_f2(void)
{
  return ((switch_f2[NODE_CONVERSE] != NULL) ?
		  ((switch_f2[NODE_CONVERSE])(this)) : T_expr::user_defined_f2()) ;
}

int T_empty_set::user_defined_f1(void)
{
  return ((switch_f1[NODE_EMPTY_SET] != NULL) ?
		  ((switch_f1[NODE_EMPTY_SET])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_empty_set::user_defined_f2(void)
{
  return ((switch_f2[NODE_EMPTY_SET] != NULL) ?
		  ((switch_f2[NODE_EMPTY_SET])(this)) : T_expr::user_defined_f2()) ;
}

int T_empty_seq::user_defined_f1(void)
{
  return ((switch_f1[NODE_EMPTY_SEQ] != NULL) ?
		  ((switch_f1[NODE_EMPTY_SEQ])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_empty_seq::user_defined_f2(void)
{
  return ((switch_f2[NODE_EMPTY_SEQ] != NULL) ?
		  ((switch_f2[NODE_EMPTY_SEQ])(this)) : T_expr::user_defined_f2()) ;
}

int T_array_item::user_defined_f1(void)
{
  return ((switch_f1[NODE_ARRAY_ITEM] != NULL) ?
		  ((switch_f1[NODE_ARRAY_ITEM])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_array_item::user_defined_f2(void)
{
  return ((switch_f2[NODE_ARRAY_ITEM] != NULL) ?
		  ((switch_f2[NODE_ARRAY_ITEM])(this)) : T_expr::user_defined_f2()) ;
}

int T_expr::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXPR] != NULL) ?
		  ((switch_f1[NODE_EXPR])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_expr::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXPR] != NULL) ?
		  ((switch_f2[NODE_EXPR])(this)) : T_item::user_defined_f2()) ;
}

int T_list_link::user_defined_f1(void)
{
  return ((switch_f1[NODE_LIST_LINK] != NULL) ?
		  ((switch_f1[NODE_LIST_LINK])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_list_link::user_defined_f2(void)
{
  return ((switch_f2[NODE_LIST_LINK] != NULL) ?
		  ((switch_f2[NODE_LIST_LINK])(this)) : T_item::user_defined_f2()) ;
}

int T_while::user_defined_f1(void)
{
  return ((switch_f1[NODE_WHILE] != NULL) ?
		  ((switch_f1[NODE_WHILE])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_while::user_defined_f2(void)
{
  return ((switch_f2[NODE_WHILE] != NULL) ?
		  ((switch_f2[NODE_WHILE])(this)) : T_substitution::user_defined_f2()) ;
}

int T_case::user_defined_f1(void)
{
  return ((switch_f1[NODE_CASE] != NULL) ?
		  ((switch_f1[NODE_CASE])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_case::user_defined_f2(void)
{
  return ((switch_f2[NODE_CASE] != NULL) ?
		  ((switch_f2[NODE_CASE])(this)) : T_substitution::user_defined_f2()) ;
}

int T_case_item::user_defined_f1(void)
{
  return ((switch_f1[NODE_CASE_ITEM] != NULL) ?
		  ((switch_f1[NODE_CASE_ITEM])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_case_item::user_defined_f2(void)
{
  return ((switch_f2[NODE_CASE_ITEM] != NULL) ?
		  ((switch_f2[NODE_CASE_ITEM])(this)) : T_item::user_defined_f2()) ;
}

int T_var::user_defined_f1(void)
{
  return ((switch_f1[NODE_VAR] != NULL) ?
		  ((switch_f1[NODE_VAR])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_var::user_defined_f2(void)
{
  return ((switch_f2[NODE_VAR] != NULL) ?
		  ((switch_f2[NODE_VAR])(this)) : T_substitution::user_defined_f2()) ;
}

int T_let::user_defined_f1(void)
{
  return ((switch_f1[NODE_LET] != NULL) ?
		  ((switch_f1[NODE_LET])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_let::user_defined_f2(void)
{
  return ((switch_f2[NODE_LET] != NULL) ?
		  ((switch_f2[NODE_LET])(this)) : T_substitution::user_defined_f2()) ;
}

int T_any::user_defined_f1(void)
{
  return ((switch_f1[NODE_ANY] != NULL) ?
		  ((switch_f1[NODE_ANY])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_any::user_defined_f2(void)
{
  return ((switch_f2[NODE_ANY] != NULL) ?
		  ((switch_f2[NODE_ANY])(this)) : T_substitution::user_defined_f2()) ;
}

int T_select::user_defined_f1(void)
{
  return ((switch_f1[NODE_SELECT] != NULL) ?
		  ((switch_f1[NODE_SELECT])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_select::user_defined_f2(void)
{
  return ((switch_f2[NODE_SELECT] != NULL) ?
		  ((switch_f2[NODE_SELECT])(this)) : T_substitution::user_defined_f2()) ;
}

int T_when::user_defined_f1(void)
{
  return ((switch_f1[NODE_WHEN] != NULL) ?
		  ((switch_f1[NODE_WHEN])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_when::user_defined_f2(void)
{
  return ((switch_f2[NODE_WHEN] != NULL) ?
		  ((switch_f2[NODE_WHEN])(this)) : T_item::user_defined_f2()) ;
}

int T_if::user_defined_f1(void)
{
  return ((switch_f1[NODE_IF] != NULL) ?
		  ((switch_f1[NODE_IF])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_if::user_defined_f2(void)
{
  return ((switch_f2[NODE_IF] != NULL) ?
		  ((switch_f2[NODE_IF])(this)) : T_substitution::user_defined_f2()) ;
}

int T_else::user_defined_f1(void)
{
  return ((switch_f1[NODE_ELSE] != NULL) ?
		  ((switch_f1[NODE_ELSE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_else::user_defined_f2(void)
{
  return ((switch_f2[NODE_ELSE] != NULL) ?
		  ((switch_f2[NODE_ELSE])(this)) : T_item::user_defined_f2()) ;
}

int T_choice::user_defined_f1(void)
{
  return ((switch_f1[NODE_CHOICE] != NULL) ?
		  ((switch_f1[NODE_CHOICE])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_choice::user_defined_f2(void)
{
  return ((switch_f2[NODE_CHOICE] != NULL) ?
		  ((switch_f2[NODE_CHOICE])(this)) : T_substitution::user_defined_f2()) ;
}

int T_or::user_defined_f1(void)
{
  return ((switch_f1[NODE_OR] != NULL) ?
		  ((switch_f1[NODE_OR])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_or::user_defined_f2(void)
{
  return ((switch_f2[NODE_OR] != NULL) ?
		  ((switch_f2[NODE_OR])(this)) : T_item::user_defined_f2()) ;
}

int T_assert::user_defined_f1(void)
{
  return ((switch_f1[NODE_ASSERT] != NULL) ?
		  ((switch_f1[NODE_ASSERT])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_assert::user_defined_f2(void)
{
  return ((switch_f2[NODE_ASSERT] != NULL) ?
		  ((switch_f2[NODE_ASSERT])(this)) : T_substitution::user_defined_f2()) ;
}

int T_label::user_defined_f1(void)
{
  return ((switch_f1[NODE_LABEL] != NULL) ?
                  ((switch_f1[NODE_LABEL])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_label::user_defined_f2(void)
{
  return ((switch_f2[NODE_LABEL] != NULL) ?
                  ((switch_f2[NODE_LABEL])(this)) : T_substitution::user_defined_f2()) ;
}

int T_jumpif::user_defined_f1(void)
{
  return ((switch_f1[NODE_JUMPIF] != NULL) ?
                  ((switch_f1[NODE_JUMPIF])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_jumpif::user_defined_f2(void)
{
  return ((switch_f2[NODE_JUMPIF] != NULL) ?
                  ((switch_f2[NODE_JUMPIF])(this)) : T_substitution::user_defined_f2()) ;
}

int T_witness::user_defined_f1(void)
{
  return ((switch_f1[NODE_WITNESS] != NULL) ?
		  ((switch_f1[NODE_WITNESS])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_witness::user_defined_f2(void)
{
  return ((switch_f2[NODE_WITNESS] != NULL) ?
		  ((switch_f2[NODE_WITNESS])(this)) : T_substitution::user_defined_f2()) ;
}

int T_precond::user_defined_f1(void)
{
  return ((switch_f1[NODE_PRECOND] != NULL) ?
		  ((switch_f1[NODE_PRECOND])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_precond::user_defined_f2(void)
{
  return ((switch_f2[NODE_PRECOND] != NULL) ?
		  ((switch_f2[NODE_PRECOND])(this)) : T_substitution::user_defined_f2()) ;
}

int T_skip::user_defined_f1(void)
{
  return ((switch_f1[NODE_SKIP] != NULL) ?
		  ((switch_f1[NODE_SKIP])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_skip::user_defined_f2(void)
{
  return ((switch_f2[NODE_SKIP] != NULL) ?
		  ((switch_f2[NODE_SKIP])(this)) : T_substitution::user_defined_f2()) ;
}

int T_begin::user_defined_f1(void)
{
  return ((switch_f1[NODE_BEGIN] != NULL) ?
		  ((switch_f1[NODE_BEGIN])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_begin::user_defined_f2(void)
{
  return ((switch_f2[NODE_BEGIN] != NULL) ?
		  ((switch_f2[NODE_BEGIN])(this)) : T_substitution::user_defined_f2()) ;
}

int T_op_call::user_defined_f1(void)
{
  return ((switch_f1[NODE_OP_CALL] != NULL) ?
		  ((switch_f1[NODE_OP_CALL])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_op_call::user_defined_f2(void)
{
  return ((switch_f2[NODE_OP_CALL] != NULL) ?
		  ((switch_f2[NODE_OP_CALL])(this)) : T_substitution::user_defined_f2()) ;
}

int T_becomes_such_that::user_defined_f1(void)
{
  return ((switch_f1[NODE_BECOMES_SUCH_THAT] != NULL) ?
		  ((switch_f1[NODE_BECOMES_SUCH_THAT])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_becomes_such_that::user_defined_f2(void)
{
  return ((switch_f2[NODE_BECOMES_SUCH_THAT] != NULL) ?
		  ((switch_f2[NODE_BECOMES_SUCH_THAT])(this)) : T_substitution::user_defined_f2()) ;
}

int T_becomes_member_of::user_defined_f1(void)
{
  return ((switch_f1[NODE_BECOMES_MEMBER_OF] != NULL) ?
		  ((switch_f1[NODE_BECOMES_MEMBER_OF])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_becomes_member_of::user_defined_f2(void)
{
  return ((switch_f2[NODE_BECOMES_MEMBER_OF] != NULL) ?
		  ((switch_f2[NODE_BECOMES_MEMBER_OF])(this)) : T_substitution::user_defined_f2()) ;
}

int T_affect::user_defined_f1(void)
{
  return ((switch_f1[NODE_AFFECT] != NULL) ?
		  ((switch_f1[NODE_AFFECT])(this)) : T_substitution::user_defined_f1()) ;
}

T_object *T_affect::user_defined_f2(void)
{
  return ((switch_f2[NODE_AFFECT] != NULL) ?
		  ((switch_f2[NODE_AFFECT])(this)) : T_substitution::user_defined_f2()) ;
}

int T_comment::user_defined_f1(void)
{
  return ((switch_f1[NODE_COMMENT] != NULL) ?
		  ((switch_f1[NODE_COMMENT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_comment::user_defined_f2(void)
{
  return ((switch_f2[NODE_COMMENT] != NULL) ?
		  ((switch_f2[NODE_COMMENT])(this)) : T_item::user_defined_f2()) ;
}

int T_flag::user_defined_f1(void)
{
  return ((switch_f1[NODE_FLAG] != NULL) ?
		  ((switch_f1[NODE_FLAG])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_flag::user_defined_f2(void)
{
  return ((switch_f2[NODE_FLAG] != NULL) ?
		  ((switch_f2[NODE_FLAG])(this)) : T_item::user_defined_f2()) ;
}

int T_definition::user_defined_f1(void)
{
  return ((switch_f1[NODE_DEFINITION] != NULL) ?
		  ((switch_f1[NODE_DEFINITION])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_definition::user_defined_f2(void)
{
  return ((switch_f2[NODE_DEFINITION] != NULL) ?
		  ((switch_f2[NODE_DEFINITION])(this)) : T_item::user_defined_f2()) ;
}

int T_used_machine::user_defined_f1(void)
{
  return ((switch_f1[NODE_USED_MACHINE] != NULL) ?
		  ((switch_f1[NODE_USED_MACHINE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_used_machine::user_defined_f2(void)
{
  return ((switch_f2[NODE_USED_MACHINE] != NULL) ?
		  ((switch_f2[NODE_USED_MACHINE])(this)) : T_item::user_defined_f2()) ;
}

int T_literal_enumerated_value::user_defined_f1(void)
{
  return ((switch_f1[NODE_LITERAL_ENUMERATED_VALUE] != NULL) ?
		  ((switch_f1[NODE_LITERAL_ENUMERATED_VALUE])(this)) : T_ident::user_defined_f1()) ;
}

T_object *T_literal_enumerated_value::user_defined_f2(void)
{
  return ((switch_f2[NODE_LITERAL_ENUMERATED_VALUE] != NULL) ?
		  ((switch_f2[NODE_LITERAL_ENUMERATED_VALUE])(this)) : T_ident::user_defined_f2()) ;
}

int T_renamed_ident::user_defined_f1(void)
{
  return ((switch_f1[NODE_RENAMED_IDENT] != NULL) ?
		  ((switch_f1[NODE_RENAMED_IDENT])(this)) : T_ident::user_defined_f1()) ;
}

T_object *T_renamed_ident::user_defined_f2(void)
{
  return ((switch_f2[NODE_RENAMED_IDENT] != NULL) ?
		  ((switch_f2[NODE_RENAMED_IDENT])(this)) : T_ident::user_defined_f2()) ;
}

int T_ident::user_defined_f1(void)
{
  return ((switch_f1[NODE_IDENT] != NULL) ?
		  ((switch_f1[NODE_IDENT])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_ident::user_defined_f2(void)
{
  return ((switch_f2[NODE_IDENT] != NULL) ?
		  ((switch_f2[NODE_IDENT])(this)) : T_expr::user_defined_f2()) ;
}

int T_keyword::user_defined_f1(void)
{
  return ((switch_f1[NODE_KEYWORD] != NULL) ?
		  ((switch_f1[NODE_KEYWORD])(this)) : T_symbol::user_defined_f1()) ;
}

T_object *T_keyword::user_defined_f2(void)
{
  return ((switch_f2[NODE_KEYWORD] != NULL) ?
		  ((switch_f2[NODE_KEYWORD])(this)) : T_symbol::user_defined_f2()) ;
}

int T_generalised_union::user_defined_f1(void)
{
  return ((switch_f1[NODE_GENERALISED_UNION] != NULL) ?
		  ((switch_f1[NODE_GENERALISED_UNION])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_generalised_union::user_defined_f2(void)
{
  return ((switch_f2[NODE_GENERALISED_UNION] != NULL) ?
		  ((switch_f2[NODE_GENERALISED_UNION])(this)) : T_expr::user_defined_f2()) ;
}

int T_generalised_intersection::user_defined_f1(void)
{
  return ((switch_f1[NODE_GENERALISED_INTERSECTION] != NULL) ?
		  ((switch_f1[NODE_GENERALISED_INTERSECTION])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_generalised_intersection::user_defined_f2(void)
{
  return ((switch_f2[NODE_GENERALISED_INTERSECTION] != NULL) ?
		  ((switch_f2[NODE_GENERALISED_INTERSECTION])(this)) : T_expr::user_defined_f2()) ;
}

int T_extensive_set::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXTENSIVE_SET] != NULL) ?
		  ((switch_f1[NODE_EXTENSIVE_SET])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_extensive_set::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXTENSIVE_SET] != NULL) ?
		  ((switch_f2[NODE_EXTENSIVE_SET])(this)) : T_expr::user_defined_f2()) ;
}

int T_comprehensive_set::user_defined_f1(void)
{
  return ((switch_f1[NODE_COMPREHENSIVE_SET] != NULL) ?
		  ((switch_f1[NODE_COMPREHENSIVE_SET])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_comprehensive_set::user_defined_f2(void)
{
  return ((switch_f2[NODE_COMPREHENSIVE_SET] != NULL) ?
		  ((switch_f2[NODE_COMPREHENSIVE_SET])(this)) : T_expr::user_defined_f2()) ;
}

int T_extensive_seq::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXTENSIVE_SEQ] != NULL) ?
		  ((switch_f1[NODE_EXTENSIVE_SEQ])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_extensive_seq::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXTENSIVE_SEQ] != NULL) ?
		  ((switch_f2[NODE_EXTENSIVE_SEQ])(this)) : T_expr::user_defined_f2()) ;
}

int T_image::user_defined_f1(void)
{
  return ((switch_f1[NODE_IMAGE] != NULL) ?
		  ((switch_f1[NODE_IMAGE])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_image::user_defined_f2(void)
{
  return ((switch_f2[NODE_IMAGE] != NULL) ?
		  ((switch_f2[NODE_IMAGE])(this)) : T_expr::user_defined_f2()) ;
}

int T_expr_with_parenthesis::user_defined_f1(void)
{
  return ((switch_f1[NODE_EXPR_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f1[NODE_EXPR_WITH_PARENTHESIS])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_expr_with_parenthesis::user_defined_f2(void)
{
  return ((switch_f2[NODE_EXPR_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f2[NODE_EXPR_WITH_PARENTHESIS])(this)) : T_expr::user_defined_f2()) ;
}

int T_literal_integer::user_defined_f1(void)
{
  return ((switch_f1[NODE_LITERAL_INTEGER] != NULL) ?
		  ((switch_f1[NODE_LITERAL_INTEGER])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_literal_integer::user_defined_f2(void)
{
  return ((switch_f2[NODE_LITERAL_INTEGER] != NULL) ?
		  ((switch_f2[NODE_LITERAL_INTEGER])(this)) : T_expr::user_defined_f2()) ;
}

int T_literal_boolean::user_defined_f1(void)
{
  return ((switch_f1[NODE_LITERAL_BOOLEAN] != NULL) ?
		  ((switch_f1[NODE_LITERAL_BOOLEAN])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_literal_boolean::user_defined_f2(void)
{
  return ((switch_f2[NODE_LITERAL_BOOLEAN] != NULL) ?
		  ((switch_f2[NODE_LITERAL_BOOLEAN])(this)) : T_expr::user_defined_f2()) ;
}

int T_literal_string::user_defined_f1(void)
{
  return ((switch_f1[NODE_LITERAL_STRING] != NULL) ?
		  ((switch_f1[NODE_LITERAL_STRING])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_literal_string::user_defined_f2(void)
{
  return ((switch_f2[NODE_LITERAL_STRING] != NULL) ?
		  ((switch_f2[NODE_LITERAL_STRING])(this)) : T_expr::user_defined_f2()) ;
}

int T_literal_real::user_defined_f1(void)
{
  return ((switch_f1[NODE_LITERAL_REAL] != NULL) ?
                  ((switch_f1[NODE_LITERAL_REAL])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_literal_real::user_defined_f2(void)
{
  return ((switch_f2[NODE_LITERAL_REAL] != NULL) ?
                  ((switch_f2[NODE_LITERAL_REAL])(this)) : T_expr::user_defined_f2()) ;
}

int T_quantified_union::user_defined_f1(void)
{
  return ((switch_f1[NODE_QUANTIFIED_UNION] != NULL) ?
		  ((switch_f1[NODE_QUANTIFIED_UNION])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_quantified_union::user_defined_f2(void)
{
  return ((switch_f2[NODE_QUANTIFIED_UNION] != NULL) ?
		  ((switch_f2[NODE_QUANTIFIED_UNION])(this)) : T_expr::user_defined_f2()) ;
}

int T_quantified_intersection::user_defined_f1(void)
{
  return ((switch_f1[NODE_QUANTIFIED_INTERSECTION] != NULL) ?
		  ((switch_f1[NODE_QUANTIFIED_INTERSECTION])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_quantified_intersection::user_defined_f2(void)
{
  return ((switch_f2[NODE_QUANTIFIED_INTERSECTION] != NULL) ?
		  ((switch_f2[NODE_QUANTIFIED_INTERSECTION])(this)) : T_expr::user_defined_f2()) ;
}

int T_pi::user_defined_f1(void)
{
  return ((switch_f1[NODE_PI] != NULL) ?
		  ((switch_f1[NODE_PI])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_pi::user_defined_f2(void)
{
  return ((switch_f2[NODE_PI] != NULL) ?
		  ((switch_f2[NODE_PI])(this)) : T_expr::user_defined_f2()) ;
}

int T_lambda_expr::user_defined_f1(void)
{
  return ((switch_f1[NODE_LAMBDA_EXPR] != NULL) ?
		  ((switch_f1[NODE_LAMBDA_EXPR])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_lambda_expr::user_defined_f2(void)
{
  return ((switch_f2[NODE_LAMBDA_EXPR] != NULL) ?
		  ((switch_f2[NODE_LAMBDA_EXPR])(this)) : T_expr::user_defined_f2()) ;
}

int T_sigma::user_defined_f1(void)
{
  return ((switch_f1[NODE_SIGMA] != NULL) ?
		  ((switch_f1[NODE_SIGMA])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_sigma::user_defined_f2(void)
{
  return ((switch_f2[NODE_SIGMA] != NULL) ?
		  ((switch_f2[NODE_SIGMA])(this)) : T_expr::user_defined_f2()) ;
}

int T_binary_op::user_defined_f1(void)
{
  return ((switch_f1[NODE_BINARY_OP] != NULL) ?
		  ((switch_f1[NODE_BINARY_OP])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_binary_op::user_defined_f2(void)
{
  return ((switch_f2[NODE_BINARY_OP] != NULL) ?
		  ((switch_f2[NODE_BINARY_OP])(this)) : T_expr::user_defined_f2()) ;
}

int T_unary_op::user_defined_f1(void)
{
  return ((switch_f1[NODE_UNARY_OP] != NULL) ?
		  ((switch_f1[NODE_UNARY_OP])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_unary_op::user_defined_f2(void)
{
  return ((switch_f2[NODE_UNARY_OP] != NULL) ?
		  ((switch_f2[NODE_UNARY_OP])(this)) : T_expr::user_defined_f2()) ;
}

int T_valuation::user_defined_f1(void)
{
  return ((switch_f1[NODE_VALUATION] != NULL) ?
		  ((switch_f1[NODE_VALUATION])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_valuation::user_defined_f2(void)
{
  return ((switch_f2[NODE_VALUATION] != NULL) ?
		  ((switch_f2[NODE_VALUATION])(this)) : T_item::user_defined_f2()) ;
}

int T_relation::user_defined_f1(void)
{
  return ((switch_f1[NODE_RELATION] != NULL) ?
		  ((switch_f1[NODE_RELATION])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_relation::user_defined_f2(void)
{
  return ((switch_f2[NODE_RELATION] != NULL) ?
		  ((switch_f2[NODE_RELATION])(this)) : T_expr::user_defined_f2()) ;
}

// Methodes de mise a jour des "aiguillages"
void set_user_defined_f1(T_node_type node_type,
								  user_defined_1 new_function)
{
  switch_f1[node_type] = new_function ;
}


void set_user_defined_f2(T_node_type node_type,
								  user_defined_2 new_function)
{
  switch_f2[node_type] = new_function ;
}

int T_lexem::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LEXEM] != NULL) ?
		  ((switch_f3[NODE_LEXEM])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_lexem::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LEXEM] != NULL) ?
		  ((switch_f4[NODE_LEXEM])(this, arg)) : T_object::user_defined_f4(arg)) ;
}

int T_lexem::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LEXEM] != NULL) ?
		  ((switch_f5[NODE_LEXEM])(this, arg)) : T_object::user_defined_f5(arg)) ;
}

int T_object::user_defined_f3(int arg)
{
  assert (switch_f3[NODE_OBJECT] != NULL) ;
  return switch_f3[NODE_OBJECT](this, arg) ;
}

T_object *T_object::user_defined_f4(T_object *arg)
{
  assert (switch_f4[NODE_OBJECT] != NULL) ;
  return switch_f4[NODE_OBJECT](this, arg) ;
}

int T_object::user_defined_f5(T_object *arg)
{
  assert (switch_f5[NODE_OBJECT] != NULL) ;
  return switch_f5[NODE_OBJECT](this, arg) ;
}

int T_betree::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BETREE] != NULL) ?
		  ((switch_f3[NODE_BETREE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_betree::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BETREE] != NULL) ?
		  ((switch_f4[NODE_BETREE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_betree::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BETREE] != NULL) ?
		  ((switch_f5[NODE_BETREE])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_machine::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_MACHINE] != NULL) ?
		  ((switch_f3[NODE_MACHINE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_machine::user_defined_f4(T_object *arg)
{
  return (switch_f4[NODE_MACHINE] != NULL) ?
	(switch_f4[NODE_MACHINE])(this, arg) : T_item::user_defined_f4(arg) ;
}

int T_machine::user_defined_f5(T_object *arg)
{
  return (switch_f5[NODE_MACHINE] != NULL) ?
	(switch_f5[NODE_MACHINE])(this, arg) : T_item::user_defined_f5(arg) ;
}

int T_item::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ITEM] != NULL) ?
		  ((switch_f3[NODE_ITEM])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_item::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ITEM] != NULL) ?
		  ((switch_f4[NODE_ITEM])(this, arg)) : T_object::user_defined_f4(arg)) ;
}

int T_item::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ITEM] != NULL) ?
		  ((switch_f5[NODE_ITEM])(this, arg)) : T_object::user_defined_f5(arg)) ;
}

int T_symbol::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SYMBOL] != NULL) ?
		  ((switch_f3[NODE_SYMBOL])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_symbol::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SYMBOL] != NULL) ?
		  ((switch_f4[NODE_SYMBOL])(this, arg)) : T_object::user_defined_f4(arg)) ;
}

int T_symbol::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SYMBOL] != NULL) ?
		  ((switch_f5[NODE_SYMBOL])(this, arg)) : T_object::user_defined_f5(arg)) ;
}

int T_op::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_OPERATION] != NULL) ?
		  ((switch_f3[NODE_OPERATION])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_op::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_OPERATION] != NULL) ?
		  ((switch_f4[NODE_OPERATION])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_op::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_OPERATION] != NULL) ?
		  ((switch_f5[NODE_OPERATION])(this, arg)) : T_item::user_defined_f5(arg)) ;
}


int T_substitution::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SUBSTITUTION] != NULL) ?
		  ((switch_f3[NODE_SUBSTITUTION])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_substitution::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SUBSTITUTION] != NULL) ?
		  ((switch_f4[NODE_SUBSTITUTION])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_substitution::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SUBSTITUTION] != NULL) ?
		  ((switch_f5[NODE_SUBSTITUTION])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_typing_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_TYPING_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_TYPING_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_typing_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_TYPING_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_TYPING_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_typing_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_TYPING_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_TYPING_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_existential_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXISTENTIAL_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_EXISTENTIAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_existential_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXISTENTIAL_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_EXISTENTIAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_existential_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXISTENTIAL_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_EXISTENTIAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_universal_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_UNIVERSAL_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_UNIVERSAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_universal_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_UNIVERSAL_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_UNIVERSAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_universal_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_UNIVERSAL_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_UNIVERSAL_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_expr_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXPR_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_EXPR_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_expr_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXPR_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_EXPR_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_expr_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXPR_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_EXPR_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_binary_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BINARY_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_BINARY_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_binary_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BINARY_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_BINARY_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_binary_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BINARY_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_BINARY_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_not_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_NOT_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_NOT_PREDICATE])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_not_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_NOT_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_NOT_PREDICATE])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_not_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_NOT_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_NOT_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_predicate_with_parenthesis::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PREDICATE_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f3[NODE_PREDICATE_WITH_PARENTHESIS])(this, arg)) : T_predicate::user_defined_f3(arg)) ;
}

T_object *T_predicate_with_parenthesis::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PREDICATE_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f4[NODE_PREDICATE_WITH_PARENTHESIS])(this, arg)) : T_predicate::user_defined_f4(arg)) ;
}

int T_predicate_with_parenthesis::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PREDICATE_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f5[NODE_PREDICATE_WITH_PARENTHESIS])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_predicate::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PREDICATE] != NULL) ?
		  ((switch_f3[NODE_PREDICATE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_predicate::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PREDICATE] != NULL) ?
		  ((switch_f4[NODE_PREDICATE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_predicate::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PREDICATE] != NULL) ?
		  ((switch_f5[NODE_PREDICATE])(this, arg)) : T_predicate::user_defined_f5(arg)) ;
}

int T_converse::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_CONVERSE] != NULL) ?
		  ((switch_f3[NODE_CONVERSE])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_converse::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_CONVERSE] != NULL) ?
		  ((switch_f4[NODE_CONVERSE])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_converse::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_CONVERSE] != NULL) ?
		  ((switch_f5[NODE_CONVERSE])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_empty_set::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EMPTY_SET] != NULL) ?
		  ((switch_f3[NODE_EMPTY_SET])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_empty_set::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EMPTY_SET] != NULL) ?
		  ((switch_f4[NODE_EMPTY_SET])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_empty_set::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EMPTY_SET] != NULL) ?
		  ((switch_f5[NODE_EMPTY_SET])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_empty_seq::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EMPTY_SEQ] != NULL) ?
		  ((switch_f3[NODE_EMPTY_SEQ])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_empty_seq::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EMPTY_SEQ] != NULL) ?
		  ((switch_f4[NODE_EMPTY_SEQ])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_empty_seq::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EMPTY_SEQ] != NULL) ?
		  ((switch_f5[NODE_EMPTY_SEQ])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_array_item::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ARRAY_ITEM] != NULL) ?
		  ((switch_f3[NODE_ARRAY_ITEM])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_array_item::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ARRAY_ITEM] != NULL) ?
		  ((switch_f4[NODE_ARRAY_ITEM])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_array_item::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ARRAY_ITEM] != NULL) ?
		  ((switch_f5[NODE_ARRAY_ITEM])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_expr::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXPR] != NULL) ?
		  ((switch_f3[NODE_EXPR])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_expr::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXPR] != NULL) ?
		  ((switch_f4[NODE_EXPR])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_expr::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXPR] != NULL) ?
		  ((switch_f5[NODE_EXPR])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_list_link::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LIST_LINK] != NULL) ?
		  ((switch_f3[NODE_LIST_LINK])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_list_link::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LIST_LINK] != NULL) ?
		  ((switch_f4[NODE_LIST_LINK])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_list_link::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LIST_LINK] != NULL) ?
		  ((switch_f5[NODE_LIST_LINK])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_while::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_WHILE] != NULL) ?
		  ((switch_f3[NODE_WHILE])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_while::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_WHILE] != NULL) ?
		  ((switch_f4[NODE_WHILE])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_while::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_WHILE] != NULL) ?
		  ((switch_f5[NODE_WHILE])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_case::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_CASE] != NULL) ?
		  ((switch_f3[NODE_CASE])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_case::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_CASE] != NULL) ?
		  ((switch_f4[NODE_CASE])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_case::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_CASE] != NULL) ?
		  ((switch_f5[NODE_CASE])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_case_item::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_CASE_ITEM] != NULL) ?
		  ((switch_f3[NODE_CASE_ITEM])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_case_item::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_CASE_ITEM] != NULL) ?
		  ((switch_f4[NODE_CASE_ITEM])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_case_item::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_CASE_ITEM] != NULL) ?
		  ((switch_f5[NODE_CASE_ITEM])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_var::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_VAR] != NULL) ?
		  ((switch_f3[NODE_VAR])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_var::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_VAR] != NULL) ?
		  ((switch_f4[NODE_VAR])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_var::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_VAR] != NULL) ?
		  ((switch_f5[NODE_VAR])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_let::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LET] != NULL) ?
		  ((switch_f3[NODE_LET])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_let::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LET] != NULL) ?
		  ((switch_f4[NODE_LET])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_let::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LET] != NULL) ?
		  ((switch_f5[NODE_LET])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_any::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ANY] != NULL) ?
		  ((switch_f3[NODE_ANY])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_any::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ANY] != NULL) ?
		  ((switch_f4[NODE_ANY])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_any::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ANY] != NULL) ?
		  ((switch_f5[NODE_ANY])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_select::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SELECT] != NULL) ?
		  ((switch_f3[NODE_SELECT])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_select::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SELECT] != NULL) ?
		  ((switch_f4[NODE_SELECT])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_select::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SELECT] != NULL) ?
		  ((switch_f5[NODE_SELECT])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_when::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_WHEN] != NULL) ?
		  ((switch_f3[NODE_WHEN])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_when::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_WHEN] != NULL) ?
		  ((switch_f4[NODE_WHEN])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_when::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_WHEN] != NULL) ?
		  ((switch_f5[NODE_WHEN])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_if::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_IF] != NULL) ?
		  ((switch_f3[NODE_IF])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_if::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_IF] != NULL) ?
		  ((switch_f4[NODE_IF])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_if::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_IF] != NULL) ?
		  ((switch_f5[NODE_IF])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_else::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ELSE] != NULL) ?
		  ((switch_f3[NODE_ELSE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_else::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ELSE] != NULL) ?
		  ((switch_f4[NODE_ELSE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_else::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ELSE] != NULL) ?
		  ((switch_f5[NODE_ELSE])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_choice::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_CHOICE] != NULL) ?
		  ((switch_f3[NODE_CHOICE])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_choice::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_CHOICE] != NULL) ?
		  ((switch_f4[NODE_CHOICE])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_choice::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_CHOICE] != NULL) ?
		  ((switch_f5[NODE_CHOICE])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_or::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_OR] != NULL) ?
		  ((switch_f3[NODE_OR])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_or::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_OR] != NULL) ?
		  ((switch_f4[NODE_OR])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_or::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_OR] != NULL) ?
		  ((switch_f5[NODE_OR])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_assert::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ASSERT] != NULL) ?
		  ((switch_f3[NODE_ASSERT])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_assert::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ASSERT] != NULL) ?
		  ((switch_f4[NODE_ASSERT])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_assert::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ASSERT] != NULL) ?
		  ((switch_f5[NODE_ASSERT])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}
int T_label::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LABEL] != NULL) ?
                  ((switch_f3[NODE_LABEL])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_label::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LABEL] != NULL) ?
                  ((switch_f4[NODE_LABEL])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_label::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LABEL] != NULL) ?
                  ((switch_f5[NODE_LABEL])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}
int T_jumpif::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_JUMPIF] != NULL) ?
                  ((switch_f3[NODE_JUMPIF])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_jumpif::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_JUMPIF] != NULL) ?
                  ((switch_f4[NODE_JUMPIF])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_jumpif::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_JUMPIF] != NULL) ?
                  ((switch_f5[NODE_JUMPIF])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_witness::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_WITNESS] != NULL) ?
		  ((switch_f3[NODE_WITNESS])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_witness::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_WITNESS] != NULL) ?
		  ((switch_f4[NODE_WITNESS])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_witness::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_WITNESS] != NULL) ?
		  ((switch_f5[NODE_WITNESS])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_precond::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PRECOND] != NULL) ?
		  ((switch_f3[NODE_PRECOND])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_precond::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PRECOND] != NULL) ?
		  ((switch_f4[NODE_PRECOND])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_precond::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PRECOND] != NULL) ?
		  ((switch_f5[NODE_PRECOND])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_skip::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SKIP] != NULL) ?
		  ((switch_f3[NODE_SKIP])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_skip::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SKIP] != NULL) ?
		  ((switch_f4[NODE_SKIP])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_skip::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SKIP] != NULL) ?
		  ((switch_f5[NODE_SKIP])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_begin::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BEGIN] != NULL) ?
		  ((switch_f3[NODE_BEGIN])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_begin::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BEGIN] != NULL) ?
		  ((switch_f4[NODE_BEGIN])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_begin::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BEGIN] != NULL) ?
		  ((switch_f5[NODE_BEGIN])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_op_call::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_OP_CALL] != NULL) ?
		  ((switch_f3[NODE_OP_CALL])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_op_call::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_OP_CALL] != NULL) ?
		  ((switch_f4[NODE_OP_CALL])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_op_call::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_OP_CALL] != NULL) ?
		  ((switch_f5[NODE_OP_CALL])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_becomes_such_that::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BECOMES_SUCH_THAT] != NULL) ?
		  ((switch_f3[NODE_BECOMES_SUCH_THAT])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_becomes_such_that::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BECOMES_SUCH_THAT] != NULL) ?
		  ((switch_f4[NODE_BECOMES_SUCH_THAT])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_becomes_such_that::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BECOMES_SUCH_THAT] != NULL) ?
		  ((switch_f5[NODE_BECOMES_SUCH_THAT])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_becomes_member_of::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BECOMES_MEMBER_OF] != NULL) ?
		  ((switch_f3[NODE_BECOMES_MEMBER_OF])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_becomes_member_of::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BECOMES_MEMBER_OF] != NULL) ?
		  ((switch_f4[NODE_BECOMES_MEMBER_OF])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_becomes_member_of::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BECOMES_MEMBER_OF] != NULL) ?
		  ((switch_f5[NODE_BECOMES_MEMBER_OF])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_affect::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_AFFECT] != NULL) ?
		  ((switch_f3[NODE_AFFECT])(this, arg)) : T_substitution::user_defined_f3(arg)) ;
}

T_object *T_affect::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_AFFECT] != NULL) ?
		  ((switch_f4[NODE_AFFECT])(this, arg)) : T_substitution::user_defined_f4(arg)) ;
}

int T_affect::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_AFFECT] != NULL) ?
		  ((switch_f5[NODE_AFFECT])(this, arg)) : T_substitution::user_defined_f5(arg)) ;
}

int T_comment::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_COMMENT] != NULL) ?
		  ((switch_f3[NODE_COMMENT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_comment::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_COMMENT] != NULL) ?
		  ((switch_f4[NODE_COMMENT])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_comment::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_COMMENT] != NULL) ?
		  ((switch_f5[NODE_COMMENT])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_flag::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_FLAG] != NULL) ?
		  ((switch_f3[NODE_FLAG])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_flag::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_FLAG] != NULL) ?
		  ((switch_f4[NODE_FLAG])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_flag::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_FLAG] != NULL) ?
		  ((switch_f5[NODE_FLAG])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_definition::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_DEFINITION] != NULL) ?
		  ((switch_f3[NODE_DEFINITION])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_definition::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_DEFINITION] != NULL) ?
		  ((switch_f4[NODE_DEFINITION])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_definition::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_DEFINITION] != NULL) ?
		  ((switch_f5[NODE_DEFINITION])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_used_machine::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_USED_MACHINE] != NULL) ?
		  ((switch_f3[NODE_USED_MACHINE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_used_machine::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_USED_MACHINE] != NULL) ?
		  ((switch_f4[NODE_USED_MACHINE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_used_machine::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_USED_MACHINE] != NULL) ?
		  ((switch_f5[NODE_USED_MACHINE])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_literal_enumerated_value::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LITERAL_ENUMERATED_VALUE] != NULL) ?
		  ((switch_f3[NODE_LITERAL_ENUMERATED_VALUE])(this, arg)) : T_ident::user_defined_f3(arg)) ;
}

T_object *T_literal_enumerated_value::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LITERAL_ENUMERATED_VALUE] != NULL) ?
		  ((switch_f4[NODE_LITERAL_ENUMERATED_VALUE])(this, arg)) : T_ident::user_defined_f4(arg)) ;
}

int T_literal_enumerated_value::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LITERAL_ENUMERATED_VALUE] != NULL) ?
		  ((switch_f5[NODE_LITERAL_ENUMERATED_VALUE])(this, arg)) : T_ident::user_defined_f5(arg)) ;
}

int T_renamed_ident::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RENAMED_IDENT] != NULL) ?
		  ((switch_f3[NODE_RENAMED_IDENT])(this, arg)) : T_ident::user_defined_f3(arg)) ;
}

T_object *T_renamed_ident::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RENAMED_IDENT] != NULL) ?
		  ((switch_f4[NODE_RENAMED_IDENT])(this, arg)) : T_ident::user_defined_f4(arg)) ;
}

int T_renamed_ident::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RENAMED_IDENT] != NULL) ?
		  ((switch_f5[NODE_RENAMED_IDENT])(this, arg)) : T_ident::user_defined_f5(arg)) ;
}

int T_ident::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_IDENT] != NULL) ?
		  ((switch_f3[NODE_IDENT])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_ident::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_IDENT] != NULL) ?
		  ((switch_f4[NODE_IDENT])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_ident::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_IDENT] != NULL) ?
		  ((switch_f5[NODE_IDENT])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_keyword::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_KEYWORD] != NULL) ?
		  ((switch_f3[NODE_KEYWORD])(this, arg)) : T_symbol::user_defined_f3(arg)) ;
}

T_object *T_keyword::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_KEYWORD] != NULL) ?
		  ((switch_f4[NODE_KEYWORD])(this, arg)) : T_symbol::user_defined_f4(arg)) ;
}

int T_keyword::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_KEYWORD] != NULL) ?
		  ((switch_f5[NODE_KEYWORD])(this, arg)) : T_symbol::user_defined_f5(arg)) ;
}

int T_generalised_union::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_GENERALISED_UNION] != NULL) ?
		  ((switch_f3[NODE_GENERALISED_UNION])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_generalised_union::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_GENERALISED_UNION] != NULL) ?
		  ((switch_f4[NODE_GENERALISED_UNION])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_generalised_union::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_GENERALISED_UNION] != NULL) ?
		  ((switch_f5[NODE_GENERALISED_UNION])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_generalised_intersection::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_GENERALISED_INTERSECTION] != NULL) ?
		  ((switch_f3[NODE_GENERALISED_INTERSECTION])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_generalised_intersection::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_GENERALISED_INTERSECTION] != NULL) ?
		  ((switch_f4[NODE_GENERALISED_INTERSECTION])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_generalised_intersection::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_GENERALISED_INTERSECTION] != NULL) ?
		  ((switch_f5[NODE_GENERALISED_INTERSECTION])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_extensive_set::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXTENSIVE_SET] != NULL) ?
		  ((switch_f3[NODE_EXTENSIVE_SET])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_extensive_set::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXTENSIVE_SET] != NULL) ?
		  ((switch_f4[NODE_EXTENSIVE_SET])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_extensive_set::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXTENSIVE_SET] != NULL) ?
		  ((switch_f5[NODE_EXTENSIVE_SET])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_comprehensive_set::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_COMPREHENSIVE_SET] != NULL) ?
		  ((switch_f3[NODE_COMPREHENSIVE_SET])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_comprehensive_set::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_COMPREHENSIVE_SET] != NULL) ?
		  ((switch_f4[NODE_COMPREHENSIVE_SET])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_comprehensive_set::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_COMPREHENSIVE_SET] != NULL) ?
		  ((switch_f5[NODE_COMPREHENSIVE_SET])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_extensive_seq::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXTENSIVE_SEQ] != NULL) ?
		  ((switch_f3[NODE_EXTENSIVE_SEQ])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_extensive_seq::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXTENSIVE_SEQ] != NULL) ?
		  ((switch_f4[NODE_EXTENSIVE_SEQ])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_extensive_seq::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXTENSIVE_SEQ] != NULL) ?
		  ((switch_f5[NODE_EXTENSIVE_SEQ])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_image::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_IMAGE] != NULL) ?
		  ((switch_f3[NODE_IMAGE])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_image::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_IMAGE] != NULL) ?
		  ((switch_f4[NODE_IMAGE])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_image::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_IMAGE] != NULL) ?
		  ((switch_f5[NODE_IMAGE])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_expr_with_parenthesis::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_EXPR_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f3[NODE_EXPR_WITH_PARENTHESIS])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_expr_with_parenthesis::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_EXPR_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f4[NODE_EXPR_WITH_PARENTHESIS])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_expr_with_parenthesis::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_EXPR_WITH_PARENTHESIS] != NULL) ?
		  ((switch_f5[NODE_EXPR_WITH_PARENTHESIS])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_literal_integer::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LITERAL_INTEGER] != NULL) ?
		  ((switch_f3[NODE_LITERAL_INTEGER])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_literal_integer::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LITERAL_INTEGER] != NULL) ?
		  ((switch_f4[NODE_LITERAL_INTEGER])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_literal_integer::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LITERAL_INTEGER] != NULL) ?
		  ((switch_f5[NODE_LITERAL_INTEGER])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_literal_boolean::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LITERAL_BOOLEAN] != NULL) ?
		  ((switch_f3[NODE_LITERAL_BOOLEAN])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_literal_boolean::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LITERAL_BOOLEAN] != NULL) ?
		  ((switch_f4[NODE_LITERAL_BOOLEAN])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_literal_boolean::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LITERAL_BOOLEAN] != NULL) ?
		  ((switch_f5[NODE_LITERAL_BOOLEAN])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_literal_string::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LITERAL_STRING] != NULL) ?
                  ((switch_f3[NODE_LITERAL_STRING])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_literal_string::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LITERAL_STRING] != NULL) ?
                  ((switch_f4[NODE_LITERAL_STRING])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_literal_string::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LITERAL_STRING] != NULL) ?
                  ((switch_f5[NODE_LITERAL_STRING])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_literal_real::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LITERAL_REAL] != NULL) ?
                  ((switch_f3[NODE_LITERAL_REAL])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_literal_real::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LITERAL_REAL] != NULL) ?
                  ((switch_f4[NODE_LITERAL_REAL])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_literal_real::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LITERAL_REAL] != NULL) ?
                  ((switch_f5[NODE_LITERAL_REAL])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_quantified_union::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_QUANTIFIED_UNION] != NULL) ?
		  ((switch_f3[NODE_QUANTIFIED_UNION])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_quantified_union::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_QUANTIFIED_UNION] != NULL) ?
		  ((switch_f4[NODE_QUANTIFIED_UNION])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_quantified_union::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_QUANTIFIED_UNION] != NULL) ?
		  ((switch_f5[NODE_QUANTIFIED_UNION])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_quantified_intersection::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_QUANTIFIED_INTERSECTION] != NULL) ?
		  ((switch_f3[NODE_QUANTIFIED_INTERSECTION])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_quantified_intersection::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_QUANTIFIED_INTERSECTION] != NULL) ?
		  ((switch_f4[NODE_QUANTIFIED_INTERSECTION])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_quantified_intersection::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_QUANTIFIED_INTERSECTION] != NULL) ?
		  ((switch_f5[NODE_QUANTIFIED_INTERSECTION])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_pi::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PI] != NULL) ?
		  ((switch_f3[NODE_PI])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_pi::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PI] != NULL) ?
		  ((switch_f4[NODE_PI])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_pi::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PI] != NULL) ?
		  ((switch_f5[NODE_PI])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_lambda_expr::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LAMBDA_EXPR] != NULL) ?
		  ((switch_f3[NODE_LAMBDA_EXPR])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_lambda_expr::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_LAMBDA_EXPR] != NULL) ?
		  ((switch_f4[NODE_LAMBDA_EXPR])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_lambda_expr::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_LAMBDA_EXPR] != NULL) ?
		  ((switch_f5[NODE_LAMBDA_EXPR])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_sigma::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SIGMA] != NULL) ?
		  ((switch_f3[NODE_SIGMA])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_sigma::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SIGMA] != NULL) ?
		  ((switch_f4[NODE_SIGMA])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_sigma::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SIGMA] != NULL) ?
		  ((switch_f5[NODE_SIGMA])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_binary_op::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BINARY_OP] != NULL) ?
		  ((switch_f3[NODE_BINARY_OP])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_binary_op::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BINARY_OP] != NULL) ?
		  ((switch_f4[NODE_BINARY_OP])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_binary_op::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BINARY_OP] != NULL) ?
		  ((switch_f5[NODE_BINARY_OP])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_unary_op::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_UNARY_OP] != NULL) ?
		  ((switch_f3[NODE_UNARY_OP])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_unary_op::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_UNARY_OP] != NULL) ?
		  ((switch_f4[NODE_UNARY_OP])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_unary_op::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_UNARY_OP] != NULL) ?
		  ((switch_f5[NODE_UNARY_OP])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

int T_valuation::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_VALUATION] != NULL) ?
		  ((switch_f3[NODE_VALUATION])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_valuation::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_VALUATION] != NULL) ?
		  ((switch_f4[NODE_VALUATION])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_valuation::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_VALUATION] != NULL) ?
		  ((switch_f5[NODE_VALUATION])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_relation::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RELATION] != NULL) ?
		  ((switch_f3[NODE_RELATION])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_relation::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RELATION] != NULL) ?
		  ((switch_f4[NODE_RELATION])(this, arg)) : T_expr::user_defined_f4(arg)) ;
}

int T_relation::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RELATION] != NULL) ?
		  ((switch_f5[NODE_RELATION])(this, arg)) : T_expr::user_defined_f5(arg)) ;
}

// Methodes de mise a jour des "aiguillages"
void set_user_defined_f3(T_node_type node_type,
								  user_defined_3 new_function)
{
  switch_f3[node_type] = new_function ;
}

void set_user_defined_f4(T_node_type node_type,
								  user_defined_4 new_function)
{
  switch_f4[node_type] = new_function ;
}

void set_user_defined_f5(T_node_type node_type,
								  user_defined_5 new_function)
{
  switch_f5[node_type] = new_function ;
}

int T_used_op::user_defined_f1(void)
{
  return ((switch_f1[NODE_USED_OP] != NULL) ?
		  ((switch_f1[NODE_USED_OP])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_used_op::user_defined_f2(void)
{
  return ((switch_f2[NODE_USED_OP] != NULL) ?
		  ((switch_f2[NODE_USED_OP])(this)) : T_item::user_defined_f2()) ;
}

int T_used_op::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_USED_OP] != NULL) ?
		  ((switch_f3[NODE_USED_OP])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_used_op::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_USED_OP] != NULL) ?
		  ((switch_f4[NODE_USED_OP])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_used_op::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_USED_OP] != NULL) ?
		  ((switch_f5[NODE_USED_OP])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_betree_manager::user_defined_f1(void)
{
  return ((switch_f1[NODE_BETREE_MANAGER] != NULL) ?
		  ((switch_f1[NODE_BETREE_MANAGER])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_betree_manager::user_defined_f2(void)
{
  return ((switch_f2[NODE_BETREE_MANAGER] != NULL) ?
		  ((switch_f2[NODE_BETREE_MANAGER])(this)) : T_object::user_defined_f2()) ;
}

int T_betree_manager::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BETREE_MANAGER] != NULL) ?
		  ((switch_f3[NODE_BETREE_MANAGER])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_betree_manager::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f4[NODE_BETREE_LIST])(this, arg)) : T_object::user_defined_f4(arg)) ;
}

int T_betree_manager::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f5[NODE_BETREE_LIST])(this, arg)) : T_object::user_defined_f5(arg)) ;
}

int T_betree_list::user_defined_f1(void)
{
  return ((switch_f1[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f1[NODE_BETREE_LIST])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_betree_list::user_defined_f2(void)
{
  return ((switch_f2[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f2[NODE_BETREE_LIST])(this)) : T_object::user_defined_f2()) ;
}

int T_betree_list::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f3[NODE_BETREE_LIST])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_betree_list::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f4[NODE_BETREE_LIST])(this, arg)) : T_object::user_defined_f4(arg)) ;
}

int T_betree_list::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BETREE_LIST] != NULL) ?
		  ((switch_f5[NODE_BETREE_LIST])(this, arg)) : T_object::user_defined_f5(arg)) ;
}

int T_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_TYPE] != NULL) ?
		  ((switch_f1[NODE_TYPE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_TYPE] != NULL) ?
		  ((switch_f2[NODE_TYPE])(this)) : T_item::user_defined_f2()) ;
}

int T_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_TYPE] != NULL) ?
		  ((switch_f3[NODE_TYPE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_type::user_defined_f4(T_object *arg)
{
  TRACE0("T_label_type::user_defined_f4") ;
  return ((switch_f4[NODE_TYPE] != NULL) ?
		  ((switch_f4[NODE_TYPE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_type::user_defined_f5(T_object *arg)
{
  TRACE0("T_label_type::user_defined_f5") ;
  return ((switch_f5[NODE_TYPE] != NULL) ?
		  ((switch_f5[NODE_TYPE])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_product_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_PRODUCT_TYPE] != NULL) ?
		  ((switch_f1[NODE_PRODUCT_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_product_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_PRODUCT_TYPE] != NULL) ?
		  ((switch_f2[NODE_PRODUCT_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_product_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PRODUCT_TYPE] != NULL) ?
		  ((switch_f3[NODE_PRODUCT_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_product_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PRODUCT_TYPE] != NULL) ?
		  ((switch_f4[NODE_PRODUCT_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_product_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PRODUCT_TYPE] != NULL) ?
		  ((switch_f5[NODE_PRODUCT_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_setof_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_SETOF_TYPE] != NULL) ?
		  ((switch_f1[NODE_SETOF_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_setof_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_SETOF_TYPE] != NULL) ?
		  ((switch_f2[NODE_SETOF_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_setof_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_SETOF_TYPE] != NULL) ?
		  ((switch_f3[NODE_SETOF_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_setof_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_SETOF_TYPE] != NULL) ?
		  ((switch_f4[NODE_SETOF_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_setof_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_SETOF_TYPE] != NULL) ?
		  ((switch_f5[NODE_SETOF_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_abstract_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f1[NODE_ABSTRACT_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_abstract_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f2[NODE_ABSTRACT_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_abstract_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f3[NODE_ABSTRACT_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_abstract_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f4[NODE_ABSTRACT_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_abstract_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f5[NODE_ABSTRACT_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_enumerated_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f1[NODE_ENUMERATED_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_enumerated_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f2[NODE_ENUMERATED_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_enumerated_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f3[NODE_ENUMERATED_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_enumerated_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f4[NODE_ENUMERATED_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_enumerated_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f5[NODE_ENUMERATED_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_base_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_BASE_TYPE] != NULL) ?
		  ((switch_f1[NODE_BASE_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_base_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_BASE_TYPE] != NULL) ?
		  ((switch_f2[NODE_BASE_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_base_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BASE_TYPE] != NULL) ?
		  ((switch_f3[NODE_BASE_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_base_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BASE_TYPE] != NULL) ?
		  ((switch_f4[NODE_BASE_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_base_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BASE_TYPE] != NULL) ?
		  ((switch_f5[NODE_BASE_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_predefined_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_PREDEFINED_TYPE] != NULL) ?
		  ((switch_f1[NODE_PREDEFINED_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_predefined_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_PREDEFINED_TYPE] != NULL) ?
		  ((switch_f2[NODE_PREDEFINED_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_predefined_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PREDEFINED_TYPE] != NULL) ?
		  ((switch_f3[NODE_PREDEFINED_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_predefined_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PREDEFINED_TYPE] != NULL) ?
		  ((switch_f4[NODE_PREDEFINED_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_predefined_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PREDEFINED_TYPE] != NULL) ?
		  ((switch_f5[NODE_PREDEFINED_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_constructor_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_CONSTRUCTOR_TYPE] != NULL) ?
		  ((switch_f1[NODE_CONSTRUCTOR_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_constructor_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_CONSTRUCTOR_TYPE] != NULL) ?
		  ((switch_f2[NODE_CONSTRUCTOR_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_constructor_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_CONSTRUCTOR_TYPE] != NULL) ?
		  ((switch_f3[NODE_CONSTRUCTOR_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_constructor_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_CONSTRUCTOR_TYPE] != NULL) ?
		  ((switch_f4[NODE_CONSTRUCTOR_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_constructor_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_CONSTRUCTOR_TYPE] != NULL) ?
		  ((switch_f5[NODE_CONSTRUCTOR_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_generic_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f1[NODE_GENERIC_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_generic_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f2[NODE_GENERIC_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_generic_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f3[NODE_GENERIC_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_generic_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f4[NODE_GENERIC_TYPE])(this, arg)) : T_type::user_defined_f4(arg)) ;
}

int T_generic_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f5[NODE_GENERIC_TYPE])(this, arg)) : T_type::user_defined_f5(arg)) ;
}

int T_generic_op::user_defined_f1(void)
{
  return ((switch_f1[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f1[NODE_GENERIC_TYPE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_generic_op::user_defined_f2(void)
{
  return ((switch_f2[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f2[NODE_GENERIC_TYPE])(this)) : T_item::user_defined_f2()) ;
}

int T_generic_op::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f3[NODE_GENERIC_TYPE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_generic_op::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f4[NODE_GENERIC_TYPE])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_generic_op::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_GENERIC_TYPE] != NULL) ?
		  ((switch_f5[NODE_GENERIC_TYPE])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_bound::user_defined_f1(void)
{
  return ((switch_f1[NODE_BOUND] != NULL) ?
		  ((switch_f1[NODE_BOUND])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_bound::user_defined_f2(void)
{
  return ((switch_f2[NODE_BOUND] != NULL) ?
		  ((switch_f2[NODE_BOUND])(this)) : T_item::user_defined_f2()) ;
}

int T_bound::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_BOUND] != NULL) ?
		  ((switch_f3[NODE_BOUND])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_bound::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_BOUND] != NULL) ?
		  ((switch_f4[NODE_BOUND])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_bound::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_BOUND] != NULL) ?
		  ((switch_f5[NODE_BOUND])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_op_result::user_defined_f1(void)
{
  return ((switch_f1[NODE_OP_RESULT] != NULL) ?
		  ((switch_f1[NODE_OP_RESULT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_op_result::user_defined_f2(void)
{
  return ((switch_f2[NODE_OP_RESULT] != NULL) ?
		  ((switch_f2[NODE_OP_RESULT])(this)) : T_item::user_defined_f2()) ;
}

int T_op_result::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_OP_RESULT] != NULL) ?
		  ((switch_f3[NODE_OP_RESULT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_op_result::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_OP_RESULT] != NULL) ?
		  ((switch_f4[NODE_OP_RESULT])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_op_result::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_OP_RESULT] != NULL) ?
		  ((switch_f5[NODE_OP_RESULT])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_pragma::user_defined_f1(void)
{
  return ((switch_f1[NODE_PRAGMA] != NULL) ?
		  ((switch_f1[NODE_PRAGMA])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_pragma::user_defined_f2(void)
{
  return ((switch_f2[NODE_PRAGMA] != NULL) ?
		  ((switch_f2[NODE_PRAGMA])(this)) : T_item::user_defined_f2()) ;
}

int T_pragma::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PRAGMA] != NULL) ?
		  ((switch_f3[NODE_PRAGMA])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_pragma::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PRAGMA] != NULL) ?
		  ((switch_f4[NODE_PRAGMA])(this, arg)) : T_item::user_defined_f4(arg)) ;
}

int T_pragma::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PRAGMA] != NULL) ?
		  ((switch_f5[NODE_PRAGMA])(this, arg)) : T_item::user_defined_f5(arg)) ;
}

int T_pragma_lexem::user_defined_f1(void)
{
  return ((switch_f1[NODE_PRAGMA_LEXEM] != NULL) ?
		  ((switch_f1[NODE_PRAGMA_LEXEM])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_pragma_lexem::user_defined_f2(void)
{
  return ((switch_f2[NODE_PRAGMA_LEXEM] != NULL) ?
		  ((switch_f2[NODE_PRAGMA_LEXEM])(this)) : T_object::user_defined_f2()) ;
}

int T_pragma_lexem::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PRAGMA_LEXEM] != NULL) ?
		  ((switch_f3[NODE_PRAGMA_LEXEM])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_pragma_lexem::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PRAGMA_LEXEM] != NULL) ?
		  ((switch_f4[NODE_PRAGMA_LEXEM])(this, arg)): T_object::user_defined_f4(arg)) ;
}

int T_pragma_lexem::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PRAGMA_LEXEM] != NULL) ?
		  ((switch_f5[NODE_PRAGMA_LEXEM])(this, arg)): T_object::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_type
//
int T_B0_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_TYPE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_B0_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_TYPE])(this)) : T_item::user_defined_f2()) ;
}

int T_B0_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_TYPE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_B0_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_TYPE])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_B0_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_TYPE])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_base_type
//
int T_B0_base_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_BASE_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_BASE_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_base_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_BASE_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_BASE_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_base_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_BASE_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_BASE_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_base_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_BASE_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_BASE_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_base_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_BASE_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_BASE_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_abstract_type
//
int T_B0_abstract_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_ABSTRACT_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_abstract_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_ABSTRACT_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_abstract_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_ABSTRACT_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_abstract_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_ABSTRACT_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_abstract_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_ABSTRACT_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_ABSTRACT_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_enumerated_type
//
int T_B0_enumerated_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_ENUMERATED_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_enumerated_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_ENUMERATED_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_enumerated_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_ENUMERATED_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_enumerated_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_ENUMERATED_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_enumerated_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_ENUMERATED_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_ENUMERATED_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_interval_type
//
int T_B0_interval_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_INTERVAL_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_INTERVAL_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_interval_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_INTERVAL_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_INTERVAL_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_interval_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_INTERVAL_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_INTERVAL_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_interval_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_INTERVAL_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_INTERVAL_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_interval_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_INTERVAL_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_INTERVAL_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_array_type
//
int T_B0_array_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_ARRAY_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_ARRAY_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_array_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_ARRAY_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_ARRAY_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_array_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_ARRAY_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_ARRAY_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_array_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_ARRAY_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_ARRAY_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_array_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_ARRAY_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_ARRAY_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_record
//
int T_record::user_defined_f1(void)
{
  return ((switch_f1[NODE_RECORD] != NULL) ?
		  ((switch_f1[NODE_RECORD])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_record::user_defined_f2(void)
{
  return ((switch_f2[NODE_RECORD] != NULL) ?
		  ((switch_f2[NODE_RECORD])(this)) : T_expr::user_defined_f2()) ;
}

int T_record::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RECORD] != NULL) ?
		  ((switch_f3[NODE_RECORD])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_record::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RECORD] != NULL) ?
		  ((switch_f4[NODE_RECORD])(this, arg)): T_expr::user_defined_f4(arg)) ;
}

int T_record::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RECORD] != NULL) ?
		  ((switch_f5[NODE_RECORD])(this, arg)): T_expr::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_record_item
//
int T_record_item::user_defined_f1(void)
{
  return ((switch_f1[NODE_RECORD_ITEM] != NULL) ?
		  ((switch_f1[NODE_RECORD_ITEM])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_record_item::user_defined_f2(void)
{
  return ((switch_f2[NODE_RECORD_ITEM] != NULL) ?
		  ((switch_f2[NODE_RECORD_ITEM])(this)) : T_expr::user_defined_f2()) ;
}

int T_record_item::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RECORD_ITEM] != NULL) ?
		  ((switch_f3[NODE_RECORD_ITEM])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_record_item::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RECORD_ITEM] != NULL) ?
		  ((switch_f4[NODE_RECORD_ITEM])(this, arg)): T_expr::user_defined_f4(arg)) ;
}

int T_record_item::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RECORD_ITEM] != NULL) ?
		  ((switch_f5[NODE_RECORD_ITEM])(this, arg)): T_expr::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_struct
//
int T_struct::user_defined_f1(void)
{
  return ((switch_f1[NODE_STRUCT] != NULL) ?
		  ((switch_f1[NODE_STRUCT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_struct::user_defined_f2(void)
{
  return ((switch_f2[NODE_STRUCT] != NULL) ?
		  ((switch_f2[NODE_STRUCT])(this)) : T_item::user_defined_f2()) ;
}

int T_struct::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_STRUCT] != NULL) ?
		  ((switch_f3[NODE_STRUCT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_struct::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_STRUCT] != NULL) ?
		  ((switch_f4[NODE_STRUCT])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_struct::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_STRUCT] != NULL) ?
		  ((switch_f5[NODE_STRUCT])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_record_access
//
int T_record_access::user_defined_f1(void)
{
  return ((switch_f1[NODE_RECORD_ACCESS] != NULL) ?
		  ((switch_f1[NODE_RECORD_ACCESS])(this)) : T_expr::user_defined_f1()) ;
}

T_object *T_record_access::user_defined_f2(void)
{
  return ((switch_f2[NODE_RECORD_ACCESS] != NULL) ?
		  ((switch_f2[NODE_RECORD_ACCESS])(this)) : T_expr::user_defined_f2()) ;
}

int T_record_access::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RECORD_ACCESS] != NULL) ?
		  ((switch_f3[NODE_RECORD_ACCESS])(this, arg)) : T_expr::user_defined_f3(arg)) ;
}

T_object *T_record_access::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RECORD_ACCESS] != NULL) ?
		  ((switch_f4[NODE_RECORD_ACCESS])(this, arg)): T_expr::user_defined_f4(arg)) ;
}

int T_record_access::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RECORD_ACCESS] != NULL) ?
		  ((switch_f5[NODE_RECORD_ACCESS])(this, arg)): T_expr::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_record_type
//
int T_record_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_RECORD_TYPE] != NULL) ?
		  ((switch_f1[NODE_RECORD_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_record_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_RECORD_TYPE] != NULL) ?
		  ((switch_f2[NODE_RECORD_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_record_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_RECORD_TYPE] != NULL) ?
		  ((switch_f3[NODE_RECORD_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_record_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_RECORD_TYPE] != NULL) ?
		  ((switch_f4[NODE_RECORD_TYPE])(this, arg)): T_type::user_defined_f4(arg)) ;
}

int T_record_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_RECORD_TYPE] != NULL) ?
		  ((switch_f5[NODE_RECORD_TYPE])(this, arg)): T_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_label_type
//
int T_label_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_LABEL_TYPE] != NULL) ?
		  ((switch_f1[NODE_LABEL_TYPE])(this)) : T_type::user_defined_f1()) ;
}

T_object *T_label_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_LABEL_TYPE] != NULL) ?
		  ((switch_f2[NODE_LABEL_TYPE])(this)) : T_type::user_defined_f2()) ;
}

int T_label_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_LABEL_TYPE] != NULL) ?
		  ((switch_f3[NODE_LABEL_TYPE])(this, arg)) : T_type::user_defined_f3(arg)) ;
}

T_object *T_label_type::user_defined_f4(T_object *arg)
{
  TRACE0("T_label_type::user_defined_f4") ;
  return ((switch_f4[NODE_LABEL_TYPE] != NULL) ?
		  ((switch_f4[NODE_LABEL_TYPE])(this, arg)): T_type::user_defined_f4(arg)) ;
}

int T_label_type::user_defined_f5(T_object *arg)
{
  TRACE0("T_label_type::user_defined_f5") ;
  return ((switch_f5[NODE_LABEL_TYPE] != NULL) ?
		  ((switch_f5[NODE_LABEL_TYPE])(this, arg)): T_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_record_type
//
int T_B0_record_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_RECORD_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_RECORD_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_record_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_RECORD_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_RECORD_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_record_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_RECORD_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_RECORD_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_record_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_RECORD_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_RECORD_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_record_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_RECORD_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_RECORD_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_B0_label_type
//
int T_B0_label_type::user_defined_f1(void)
{
  return ((switch_f1[NODE_B0_LABEL_TYPE] != NULL) ?
		  ((switch_f1[NODE_B0_LABEL_TYPE])(this)) : T_B0_type::user_defined_f1()) ;
}

T_object *T_B0_label_type::user_defined_f2(void)
{
  return ((switch_f2[NODE_B0_LABEL_TYPE] != NULL) ?
		  ((switch_f2[NODE_B0_LABEL_TYPE])(this)) : T_B0_type::user_defined_f2()) ;
}

int T_B0_label_type::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_B0_LABEL_TYPE] != NULL) ?
		  ((switch_f3[NODE_B0_LABEL_TYPE])(this, arg)) : T_B0_type::user_defined_f3(arg)) ;
}

T_object *T_B0_label_type::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_B0_LABEL_TYPE] != NULL) ?
		  ((switch_f4[NODE_B0_LABEL_TYPE])(this, arg)): T_B0_type::user_defined_f4(arg)) ;
}

int T_B0_label_type::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_B0_LABEL_TYPE] != NULL) ?
		  ((switch_f5[NODE_B0_LABEL_TYPE])(this, arg)): T_B0_type::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_msg_line
//
int T_msg_line::user_defined_f1(void)
{
  return ((switch_f1[NODE_MSG_LINE] != NULL) ?
		  ((switch_f1[NODE_MSG_LINE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_msg_line::user_defined_f2(void)
{
  return ((switch_f2[NODE_MSG_LINE] != NULL) ?
		  ((switch_f2[NODE_MSG_LINE])(this)) : T_item::user_defined_f2()) ;
}

int T_msg_line::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_MSG_LINE] != NULL) ?
		  ((switch_f3[NODE_MSG_LINE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_msg_line::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_MSG_LINE] != NULL) ?
		  ((switch_f4[NODE_MSG_LINE])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_msg_line::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_MSG_LINE] != NULL) ?
		  ((switch_f5[NODE_MSG_LINE])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_msg_line_manager
//
int T_msg_line_manager::user_defined_f1(void)
{
  return ((switch_f1[NODE_MSG_LINE_MANAGER] != NULL) ?
		  ((switch_f1[NODE_MSG_LINE_MANAGER])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_msg_line_manager::user_defined_f2(void)
{
  return ((switch_f2[NODE_MSG_LINE_MANAGER] != NULL) ?
		  ((switch_f2[NODE_MSG_LINE_MANAGER])(this)) : T_item::user_defined_f2()) ;
}

int T_msg_line_manager::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_MSG_LINE_MANAGER] != NULL) ?
		  ((switch_f3[NODE_MSG_LINE_MANAGER])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_msg_line_manager::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_MSG_LINE_MANAGER] != NULL) ?
		  ((switch_f4[NODE_MSG_LINE_MANAGER])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_msg_line_manager::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_MSG_LINE_MANAGER] != NULL) ?
		  ((switch_f5[NODE_MSG_LINE_MANAGER])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_component
//
int T_component::user_defined_f1(void)
{
  return ((switch_f1[NODE_COMPONENT] != NULL) ?
		  ((switch_f1[NODE_COMPONENT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_component::user_defined_f2(void)
{
  return ((switch_f2[NODE_COMPONENT] != NULL) ?
		  ((switch_f2[NODE_COMPONENT])(this)) : T_item::user_defined_f2()) ;
}

int T_component::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_COMPONENT] != NULL) ?
		  ((switch_f3[NODE_COMPONENT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_component::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_COMPONENT] != NULL) ?
		  ((switch_f4[NODE_COMPONENT])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_component::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_COMPONENT] != NULL) ?
		  ((switch_f5[NODE_COMPONENT])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_file_component
//
int T_file_component::user_defined_f1(void)
{
  return ((switch_f1[NODE_FILE_COMPONENT] != NULL) ?
		  ((switch_f1[NODE_FILE_COMPONENT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_file_component::user_defined_f2(void)
{
  return ((switch_f2[NODE_FILE_COMPONENT] != NULL) ?
		  ((switch_f2[NODE_FILE_COMPONENT])(this)) : T_item::user_defined_f2()) ;
}

int T_file_component::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_FILE_COMPONENT] != NULL) ?
		  ((switch_f3[NODE_FILE_COMPONENT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_file_component::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_FILE_COMPONENT] != NULL) ?
		  ((switch_f4[NODE_FILE_COMPONENT])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_file_component::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_FILE_COMPONENT] != NULL) ?
		  ((switch_f5[NODE_FILE_COMPONENT])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_project
//
int T_project::user_defined_f1(void)
{
  return ((switch_f1[NODE_PROJECT] != NULL) ?
		  ((switch_f1[NODE_PROJECT])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_project::user_defined_f2(void)
{
  return ((switch_f2[NODE_PROJECT] != NULL) ?
		  ((switch_f2[NODE_PROJECT])(this)) : T_item::user_defined_f2()) ;
}

int T_project::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PROJECT] != NULL) ?
		  ((switch_f3[NODE_PROJECT])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_project::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PROJECT] != NULL) ?
		  ((switch_f4[NODE_PROJECT])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_project::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PROJECT] != NULL) ?
		  ((switch_f5[NODE_PROJECT])(this, arg)): T_item::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_project_manager
//
int T_project_manager::user_defined_f1(void)
{
  return ((switch_f1[NODE_PROJECT_MANAGER] != NULL) ?
		  ((switch_f1[NODE_PROJECT_MANAGER])(this)) : T_object::user_defined_f1()) ;
}

T_object *T_project_manager::user_defined_f2(void)
{
  return ((switch_f2[NODE_PROJECT_MANAGER] != NULL) ?
		  ((switch_f2[NODE_PROJECT_MANAGER])(this)) : T_object::user_defined_f2()) ;
}

int T_project_manager::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_PROJECT_MANAGER] != NULL) ?
		  ((switch_f3[NODE_PROJECT_MANAGER])(this, arg)) : T_object::user_defined_f3(arg)) ;
}

T_object *T_project_manager::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_PROJECT_MANAGER] != NULL) ?
		  ((switch_f4[NODE_PROJECT_MANAGER])(this, arg)): T_object::user_defined_f4(arg)) ;
}

int T_project_manager::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_PROJECT_MANAGER] != NULL) ?
		  ((switch_f5[NODE_PROJECT_MANAGER])(this, arg)): T_object::user_defined_f5(arg)) ;
}

//
//	}{	Classe T_file_definition
//
int T_file_definition::user_defined_f1(void)
{
  return ((switch_f1[NODE_FILE_DEFINITION] != NULL) ?
		  ((switch_f1[NODE_FILE_DEFINITION])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_file_definition::user_defined_f2(void)
{
  return ((switch_f2[NODE_FILE_DEFINITION] != NULL) ?
		  ((switch_f2[NODE_FILE_DEFINITION])(this)) : T_item::user_defined_f2()) ;
}

int T_file_definition::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_FILE_DEFINITION] != NULL) ?
		  ((switch_f3[NODE_FILE_DEFINITION])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_file_definition::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_FILE_DEFINITION] != NULL) ?
		  ((switch_f4[NODE_FILE_DEFINITION])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_file_definition::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_FILE_DEFINITION] != NULL) ?
		  ((switch_f5[NODE_FILE_DEFINITION])(this, arg)): T_item::user_defined_f5(arg)) ;
}


//
//	}{	Classe T_op_call_tree
//
int T_op_call_tree::user_defined_f1(void)
{
  return ((switch_f1[NODE_OP_CALL_TREE] != NULL) ?
		  ((switch_f1[NODE_OP_CALL_TREE])(this)) : T_item::user_defined_f1()) ;
}

T_object *T_op_call_tree::user_defined_f2(void)
{
  return ((switch_f2[NODE_OP_CALL_TREE] != NULL) ?
		  ((switch_f2[NODE_OP_CALL_TREE])(this)) : T_item::user_defined_f2()) ;
}

int T_op_call_tree::user_defined_f3(int arg)
{
  return ((switch_f3[NODE_OP_CALL_TREE] != NULL) ?
		  ((switch_f3[NODE_OP_CALL_TREE])(this, arg)) : T_item::user_defined_f3(arg)) ;
}

T_object *T_op_call_tree::user_defined_f4(T_object *arg)
{
  return ((switch_f4[NODE_OP_CALL_TREE] != NULL) ?
		  ((switch_f4[NODE_OP_CALL_TREE])(this, arg)): T_item::user_defined_f4(arg)) ;
}

int T_op_call_tree::user_defined_f5(T_object *arg)
{
  return ((switch_f5[NODE_OP_CALL_TREE] != NULL) ?
		  ((switch_f5[NODE_OP_CALL_TREE])(this, arg)): T_item::user_defined_f5(arg)) ;
}



//
// Fin du fichier
//

