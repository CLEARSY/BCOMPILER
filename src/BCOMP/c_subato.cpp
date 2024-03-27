/******************************* CLEARSY **************************************
* Fichier : $Id: c_subato.cpp,v 2.0 2002-03-15 13:58:48 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Substitutions "atomiques"
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
RCS_ID("$Id: c_subato.cpp,v 1.29 2002-03-15 13:58:48 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_affect
//
// Constructeur de la classe T_affect
T_affect::T_affect(T_item *lhs,
							T_item *rhs,
							T_item **adr_first,
							T_item **adr_last,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *cur_lexem)
  : T_substitution(NODE_AFFECT,
				   adr_first,
				   adr_last,
				   new_father,
				   new_betree,
				   cur_lexem)
{
  TRACE6("T_affect::T_affect(%x, %x, %x, %x, %x, %x)",
		 lhs,
		 rhs,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree) ;
  ENTER_TRACE ;

  /*
  // On a trois configurations possibles :
  // Premier cas :     :=				  	i.e.  Ident := Expression
  //				   /    \
  //              Ident  Expr
  // Deuxieme cas :     :=					i.e.  Ident, Ident := Expression
  //				   /    \
  //                ,  Expr
  //               /\
  //			  Ident Ident
  // Troisieme cas :   :=	   				i.e.  Ident ( Expression ) := Expression
  //				   /    \
  //               Evl  Expr		   	L'arbre de gauche est deguise en T_op_reuslt
  //			   /    \
  //          Ident  Expr
  */

	// Identificateur
	T_node_type lhs_type = lhs->get_node_type() ;
	TRACE1("lhs_type %s", lhs->get_class_name()) ;

	switch(lhs_type)
	  {
	  case NODE_IDENT :
	  case NODE_RENAMED_IDENT :
	  case NODE_OP_RESULT :
	  case NODE_BINARY_OP :
		{
		  TRACE0("ok lhs binary op") ;
		  first_name = last_name = NULL ;
		  lhs->extract_params((int)BOP_COMMA,
							  this,
							  (T_item **)&first_name,
							  (T_item **)&last_name) ;

		  T_item *cur = first_name ;

		  while (cur != NULL)
			{
#if 0
			  // WAS : ancien code, avant correction anomalie 2144
			  if (	(cur->get_node_type() != NODE_IDENT)
					&& (cur->get_node_type() != NODE_RENAMED_IDENT)
					&& (cur->get_node_type() != NODE_OP_RESULT) )
				{
				  TRACE0("ici, on crie") ;
				  syntax_unexpected_error(cur,
										  CAN_CONTINUE,
										  get_catalog(C_IDENT_OR_FCALL)) ;
				}
#endif // 0

			  // Correction Anomalie 2144 : dans le cas d'un T_op_result, il faut
			  // en plus verifier que rhs respecte la regle :
			  //
			  // Ident_Ren "(" Expression +, ")".
			  //
			  // En particulier, la construction "f~(x)" ne respecte
			  // pas la BNF car "f~" n'est pas un Ident_Ren.
			  //
			  // Par contre, il faut noter que "g(f~(x))" est un lhs valide
			  // On se contente donc de faire la verification au premier niveau
			  // C'est facile car :
			  // f~(x) est un T_op_result de name f~ i.e. de type T_converse
			  //
			  // Suite correction anomalie 2144 : s'il y a plus d'un
			  // paramètre en partie droite, alors seuls des ident_ren
			  // sont acceptes
			  //
			  // On fait ce controle en premier pour affiner les messages d'erreur
			  if (    (first_name != last_name)
				   && (cur->is_an_ident() == FALSE) )
				{
				  TRACE0("ici, on crie") ;
				  syntax_unexpected_error(cur,
										  CAN_CONTINUE,
										  get_catalog(C_IDENT_IN_MULTIPLE_AFFECT)) ;
				}
			  else if (cur->get_node_type() == NODE_OP_RESULT)
				{
				  T_op_result *op_result = (T_op_result *)cur ;
				  if (op_result->get_op_name()->get_node_type() == NODE_CONVERSE)
					{
					  TRACE0("ici, on crie") ;
					  syntax_unexpected_error(op_result->get_op_name(),
											  CAN_CONTINUE,
											  get_catalog(C_IDENT_OR_FCALL)) ;
					}
				}

			  // Fin correction anomalie 2144

			  cur->set_father(this) ;
			  cur = cur->get_next() ;
			}

		  break ;
		}

	  case NODE_RECORD_ACCESS :
		{
		  TRACE0("ok lhs record access") ;
		  // Cast valide car un record_access est une expression
		  first_name = last_name = lhs->make_expr() ;
		  lhs->set_father(this) ;
		  break ;
		}

	  default :
		{
		  syntax_unexpected_error(lhs,
								  FATAL_ERROR,
								  get_catalog(C_IDENT_OR_FCALL)) ;
		}
	  }

	// Valeur (partie droite)
	TRACE0("-- VALEUR  --") ;


	// rhs doit etre une expression
	// Cas particulier : un builtin
	// rhs est alors un T_list_link de type LINK_BUILTIN,
	// et il faut suivre l'indirection
	// Cast valide par construction des builtin
	T_item *save_rhs = rhs ;
	rhs = (T_item *)rhs->follow_indirection(LINK_BUILTIN) ;

	if (save_rhs != rhs)
	  {
		// On libere le T_list_link. on sait que la liberation
		// ne se propagera pas a rhs (on va s'en assurer !!!)
		TRACE2("rhs %x save_rhs %x : on libere save_rhs", rhs, save_rhs) ;

		if (save_rhs->get_node_type() == NODE_LIST_LINK)
		  {
			((T_list_link *)save_rhs)->unset_do_free() ;
		  }
		object_unlink(save_rhs) ;
	  }


	first_value = last_value = NULL ;
	TRACE1("ici rhs = %x", rhs) ;
	rhs->extract_params((int)BOP_COMMA,
						this,
						(T_item **)&first_value,
						(T_item **)&last_value) ;

	T_item *cur_value = first_value ;

	while (cur_value != NULL)
	  {
		if (cur_value->is_an_expr() == FALSE)
		  {
			syntax_expr_expected_error(cur_value,
									   CAN_CONTINUE) ;
		  }
		else
		  {
			cur_value->make_expr()->post_check() ;
		  }

		cur_value->set_father(this) ;
		cur_value = cur_value->get_next() ;
	  }


	EXIT_TRACE ;
	TRACE0("fin de T_affect::T_affect") ;
}

//
//	}{	Correction de l'arbre
T_item *T_affect::check_tree(T_item **ref_this)
{
  TRACE2("T_affect::check_tree(%x) %X", this, ref_this) ;

  T_item *value = first_value ;
  while (value != NULL)
	{
	  T_item *next = value->get_next() ;
	  T_item *res = value->check_tree(NULL) ;
	  if (res != value)
		{
		  TRACE1("%x a ete modifiee ! on change la liste", value) ;
		  value->exchange_in_list(res,
								  (T_item **)&first_value,
								  (T_item **)&last_value) ;
		}

	  value = next ;
	}

  TRACE2("fin de T_affect::check_tree(%x) %X", this, ref_this) ;
  return this ;
}

// Demande de l'acces necessaire pour le fils ref
// Renvoie TRUE si ref est en partie gauche
T_access_authorisation T_affect::get_auth_request(T_item *ref)
{
  TRACE2("T_affect::get_auth_request(%x, %s) -> AUTH_READ",
		 ref,
		 ref->get_class_name()) ;

  T_expr *cur = first_name ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return AUTH_READ ;
		}

	  if (cur->is_an_ident()
		  && cur == ref)
		{
		  return AUTH_READ_WRITE ;
		}

	  if ( 	(cur->get_node_type() == NODE_OP_RESULT)
			&& (((T_op_result *)cur)->get_op_name() == ref) )
		{
		  return AUTH_READ_WRITE ;
		}

	  cur = (T_expr *)cur->get_next() ;
	}

}



//
//	Destructeur
//
T_affect::~T_affect(void)
{
  TRACE1("T_affect::~T_affect(%x)", this) ;
}

//
//	}{	Classe T_becomes_member_of
//
// Constructeur de la classe T_becomes_member_of
T_becomes_member_of::T_becomes_member_of(T_item *op1,
												  T_item *op2,
												  T_item **adr_first,
												  T_item **adr_last,
												  T_item *new_father,
												  T_betree *new_betree,
												  T_lexem *ref_lexem)
  : T_substitution(NODE_BECOMES_MEMBER_OF,
				   adr_first,
				   adr_last,
				   new_father,
				   new_betree,
				   ref_lexem)
{
  TRACE7("T_becomes_member_of::T_becomes_member_of(%x, %x, %x, %x, %x, %x, %x)",
		 op1,
		 op2,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 ref_lexem) ;
  ENTER_TRACE ;

  // Extraction des identifiants
  first_ident = last_ident = NULL  ;

  op1->extract_params((int)BOP_COMMA,
					  this,
					  (T_item **)&first_ident,
					  (T_item **)&last_ident) ;

  T_item *cur_item = first_ident ;

  while (cur_item != NULL)
	{
	  if (cur_item->is_an_ident() == FALSE)
		{
		  syntax_ident_expected_error(cur_item,
									  FATAL_ERROR) ;
		}

	  cur_item = cur_item->get_next() ;
	}

  // op2 doit etre une expression
  // Cas particulier : un builtin
  // op2 est alors un T_list_link de type LINK_BUILTIN,
  // et il faut suivre l'indirection
  // Cast valide par construction des builtin
  op2 = (T_item *)op2->follow_indirection(LINK_BUILTIN) ;

  if (op2->is_an_expr() == FALSE)
	{
	  TRACE1("op2 : %s", op2->get_class_name()) ;
	  syntax_expr_expected_error(op2, FATAL_ERROR) ;
	}

  op2->make_expr()->post_check() ;
  set = op2->make_expr() ;
  set->set_father(this) ;

  EXIT_TRACE ;
  TRACE0("fin de T_becomes_member_of::T_becomes_member_of") ;
}

//
//	}{	Correction de l'arbre
T_item *T_becomes_member_of::check_tree(T_item **ref_this)
{
  TRACE2("T_becomes_member_of::check_tree(%x) %X", this, ref_this) ;

  if (set != NULL)
	{
	  set->check_tree((T_item **)&set) ;
	}

  TRACE2("fin de T_becomes_member_of::check_tree(%x) %X", this, ref_this) ;
  return this ;
}

//
//	Destructeur
//
T_becomes_member_of::~T_becomes_member_of(void)
{
  TRACE1("T_becomes_member_of::~T_becomes_member_of(%x)", this) ;
}

//GP 18/01/99
//retourne AUTH_READ_WRITE, si l'item se trouve en partie gauche
//du T_become_such_taht
T_access_authorisation T_becomes_member_of::get_auth_request(T_item *ref)
{
  T_ident* cur_ident = first_ident ;

  while(cur_ident != NULL)
	{
	  if(ref == cur_ident)
		{
		  return AUTH_READ_WRITE ;
		}
	  cur_ident=(T_ident*)cur_ident->get_next() ;
	}

  return AUTH_READ ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_becomes_member_of::is_read_only(void)
{
  // il faut verifier dans les sous-expression du bloc
  TRACE0("T_becomes_member_of::is_read_only");
  T_ident *cur_ident = get_lvalues();
  while (cur_ident != NULL)
	{
	  if ( (cur_ident->get_ref_machine()->get_specification()->
			find_variable(cur_ident->get_name()))
		   != NULL)
		{
		  return FALSE;
		}
	  cur_ident = (T_ident*)cur_ident->get_next();
	}

  return TRUE ;
}

//
//	}{	Classe T_becomes_such_that
//
// Constructeur de la classe T_becomes_such_that a partir
// d'un typing predicate
T_becomes_such_that::T_becomes_such_that(T_typing_predicate *tpred,
												  T_item *new_father,
												  T_betree *new_betree)
  : T_substitution(NODE_BECOMES_SUCH_THAT,
				   NULL,
				   NULL,
				   new_father,
				   new_betree,
				   tpred->get_ref_lexem())
{
  TRACE3("T_becomes_such_that::T_becomes_such_that(%x, %x, %x, %x, %x)",
		 tpred,
		 new_father,
		 new_betree) ;

  // Identificateurs

  first_ident = (T_ident *)tpred->first_ident ;
  last_ident = (T_ident *)tpred->last_ident ;

#ifdef ENABLE_PAREN_IDENT
  // Si les idents sont en fait un () alors c'est un () avec dedans
  // la liste des operandes. Il faut faire un peu de travail pour
  // reparer ca.
  if (    (first_ident == last_ident)
		  || (first_ident->get_node_type() == NODE_EXPR_WITH_PARENTHESIS) )
	{
	  TRACE0("..correction des idents parentheses ...") ;
	  T_expr *inside = first_ident->strip_parenthesis() ;

#ifdef COMPAT
	  TRACE2("ici inside %x first_ident %x", inside, first_ident) ;
	  if (inside != first_ident)
		{
		  T_expr *cur = first_ident ;
		  while (    (cur != NULL)
				  && (cur->get_node_type() == NODE_EXPR_WITH_PARENTHESIS) )
			{
			  T_expr_with_parenthesis *paren =
				(T_expr_with_parenthesis *)cur ;
			  paren->mark_as_void() ;
			  cur = paren->get_expr() ;
			}
		}
#endif

	  T_ident *new_first = NULL ;
	  T_ident *new_last = NULL ;
	  inside->extract_params(BOP_COMMA,
							 this,
							 (T_item **)&new_first,
							 (T_item **)&new_last) ;

	  first_ident = new_first ;
	  last_ident = new_last ;
	}
#endif

  T_ident *cur_ident = first_ident ;
  while (cur_ident != NULL)
	{
	  if (cur_ident->is_an_ident() == FALSE)
		{
		  syntax_ident_expected_error(cur_ident, CAN_CONTINUE) ;
		}
	  cur_ident->link() ;
	  cur_ident->set_father(this) ;
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}

  T_item *type = tpred->type ;

  TRACE3(":-) type %x %s is_a_predicate = %s",
		 type,
		 type->get_class_name(),
		 (type->is_a_predicate() == TRUE) ? "TRUE" : "FALSE") ;

  if (type->is_a_predicate() == FALSE)
	{
	  syntax_unexpected_error(type, FATAL_ERROR, get_catalog(C_PRED)) ;
	}
  pred = (T_predicate *)type ;
  pred->set_father(this) ;

  TRACE0("fin de T_becomes_such_that::T_becomes_such_that") ;
}

//
//	Destructeur
//
T_becomes_such_that::~T_becomes_such_that(void)
{
}

//GP 18/01/99
//retourne AUTH_READ_WRITE, si l'item se trouve en partie gauche
//du T_become_such_taht
T_access_authorisation T_becomes_such_that::get_auth_request(T_item *ref)
{
  T_ident* cur_ident = first_ident ;

  while(cur_ident != NULL)
	{
	  if(ref == cur_ident)
		{
		  return AUTH_READ_WRITE ;
		}
	  cur_ident=(T_ident*)cur_ident->get_next() ;
	}

  return AUTH_READ ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_becomes_such_that::is_read_only(void)
{
  //il faut verifier la partie gauche....
  TRACE0("T_becomes_such_that::is_read_only");
  T_ident *cur_ident = get_lvalues();
  while (cur_ident != NULL)
	{
	  if ( (cur_ident->get_ref_machine()->get_specification()->
			find_variable(cur_ident->get_name()))
		   != NULL)
		{
		  return FALSE;
		}
	  cur_ident = (T_ident*)cur_ident->get_next();
	}

  return TRUE ;
}

//
//	}{	Classe T_op_call
//
//
// }{	Fonction utilisee par les constructeurs pour recuperer
// les champs d'un T_op_result
void T_op_call::fetch_params(T_op_result *op_res)
{
  op_name = op_res->op_name ;

  op_name->set_father(this) ;
  op_name->link() ;

  first_in_param = op_res->first_in_param ;
  last_in_param = op_res->last_in_param ;

  T_item *cur_in_param = first_in_param ;
  while (cur_in_param != NULL)
	{
	  cur_in_param->link() ;
	  cur_in_param->set_father(this) ;
	  cur_in_param = cur_in_param->get_next() ;
	}

  ref_op = (T_op *)op_res->ref_op ;
  if (ref_op != NULL)
	{
	  ref_op->link() ;
	}

  // LV, le 20/10/1999 et le 22/11/2000: Il faut supprimer le
  // T_op_result de la liste des op_result de l'object manager.  Si on
  // ne le fait pas, il se fera ensuite transformer en T_array_item,
  // et ça cassera tout l'arbre.
  TRACE0("on previent l'object manager de la fin du T_op_result") ;
  get_object_manager()->remove_op_result(op_res) ;
}

//
//	}{	Constructeur de la classe T_op_call
//		Constructeur par copie d'un T_op_res existant
T_op_call::T_op_call(T_op_result *op_res,
							  T_item *new_father,
							  T_betree *new_betree)
  : T_substitution(NODE_OP_CALL,
				   NULL,
				   NULL,
				   new_father,
				   new_betree,
				   op_res->get_ref_lexem())
{
  TRACE1("T_op_call::T_op_call(op_res = %x)", op_res) ;

  fetch_params(op_res) ;

  // out_params : NULL dans cette configuration
  first_out_param = NULL ;
  last_out_param = NULL ;

  TRACE0("fin de T_op_call::T_op_call") ;
}

//
//	}{	Constructeur de la classe T_op_call
//		Constructeur par copie d'un T_ident existant
T_op_call::T_op_call(T_ident *ident,
							  T_item *new_father,
							  T_betree *new_betree)
  : T_substitution(NODE_OP_CALL,
				   NULL,
				   NULL,
				   new_father,
				   new_betree,
				   ident->get_ref_lexem())
{
  TRACE1("T_op_call::T_op_call(ident = %x)", ident) ;

  // La seule chose a recuperer, c'est le nom de l'operation
  op_name = ident ;
  op_name->link() ;

  // in_params : NULL dans cette configuration
  first_in_param = NULL ;
  last_in_param = NULL ;

  // out_params : NULL dans cette configuration
  first_out_param = NULL ;
  last_out_param = NULL ;

  // op_def, machine_def : NULL
  ref_op = NULL ;

  TRACE0("fin de T_op_call::T_op_call") ;
}


#ifdef ENABLE_PAREN_IDENT
// Fonction auxiliaire de parsing plus large que la BNF
// pour accepter des syntaxes comme
// (x, y) <-- op
// (x, y), z <-- op
// etc etc
static void fix_for_compatibility(T_item **adr_expr)
{
  T_item *cur_expr = *adr_expr ;

  for (;;)
	{
	  // On autorise (x, y) <-- ...
	  if (cur_expr->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
		{
		  T_expr_with_parenthesis *paren = (T_expr_with_parenthesis *)cur_expr ;
#ifdef COMPAT
		  paren->mark_as_void() ;
#endif // COMPAT
		  cur_expr = paren->get_expr() ;
		}
	  else if (cur_expr->get_node_type() == NODE_BINARY_OP)
		{
		  T_binary_op *binop = (T_binary_op *)cur_expr ;
		  T_item **adr_op1 = binop->get_adr_op1() ;
		  T_item **adr_op2 = binop->get_adr_op2() ;
		  fix_for_compatibility(adr_op1) ;
		  fix_for_compatibility(adr_op2) ;
		  *adr_expr = cur_expr ;
		  return ;
		}
	  else
		{
		  *adr_expr = cur_expr ;
		  return ;
		}
	}
}
#endif // ENABLE_PAREN_IDENT

//
//	}{	Constructeur a partir du parseur LR
//
T_op_call::T_op_call(T_item *op1,
					 T_item *op2,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *new_father,
					 T_betree *new_betree,
					 T_lexem *new_ref_lexem)
  : T_substitution(NODE_OP_CALL,
				   adr_first,
				   adr_last,
				   new_father,
				   new_betree,
				   new_ref_lexem)
{
  TRACE2("T_op_call::T_op_call(op1, op2)", op1, op2) ;

  TRACE2("op2 = %x %s", op2, op2->get_class_name()) ;

  // Ici deux cas possibles :
  //
  // Premier cas : appel d'une operation avec parametres : op2 est un NODE_OP_RESULT
  // Deuxieme cas : appel d'une operation sans parametre : op2 est un NODE_IDENT
  switch(op2->get_node_type())
	{
	case NODE_OP_RESULT :
	  {
		// On recupere les parametres
		TRACE0("on recupere les parametres") ;
		fetch_params((T_op_result *)op2) ;
		break ;
	  }

	case NODE_IDENT :
	case NODE_RENAMED_IDENT :
	  {
		TRACE1("%s est une operation sans parametre d'entree",
			   ((T_ident *)op2)->get_name()->get_value()) ;
		first_in_param = last_in_param = NULL ;
		op_name = ((T_ident *)op2) ;
		op_name->link() ;
		break ;
	  }

	default :
	  {
		TRACE2("ici erreur node_type=%x %s",
			   op2->get_node_type(),
			   op2->get_class_name()) ;
		syntax_unexpected_error(op2,
								FATAL_ERROR,
								get_catalog(C_OP_NAME)) ;
	  }
	}

  op_name->set_father(this) ;

  // out_params :: recuperes dans le fils gauche

#ifdef ENABLE_PAREN_IDENT
  // Prise en compte des syntaxes non-bnf style (x, y), z <-- op
  fix_for_compatibility(&op1) ;
#endif // ENABLE_PAREN_IDENT

  first_out_param = NULL ;
  last_out_param = NULL ;

  op1->extract_params((int)BOP_COMMA,
					  this,
					  (T_item **)&first_out_param,
					  (T_item **)&last_out_param) ;

  // On verifie que ces parametres sont bien des identificateurs
  T_item *cur_param = first_out_param ;

  while (cur_param != NULL)
	{
	  if (cur_param->is_an_ident() == FALSE)
		{
		  TRACE1("erreur : cur_param %s", cur_param->get_class_name()) ;
		  syntax_ident_expected_error(cur_param, CAN_CONTINUE) ;
		}
	  cur_param = cur_param->get_next() ;
	}

  TRACE0("fin de T_op_call::T_op_call") ;
}

// Demande de l'acces necessaire pour le fils ref
// Renvoie TRUE si ref est en partie gauche
T_access_authorisation T_op_call::get_auth_request(T_item *ref)
{
  TRACE2("T_op_call::get_auth_request(%x, %s) -> AUTH_READ",
		 ref,
		 ref->get_class_name()) ;

  T_item *cur = first_out_param ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return AUTH_READ ;
		}

	  if (cur->is_an_ident() == TRUE
		  && cur == ref )
		{
		  return AUTH_READ_WRITE ;
		}

	  if ( 	(cur->get_node_type() == NODE_OP_RESULT)
			&& (((T_op_result *)cur)->get_op_name() == ref) )
		{
		  return AUTH_READ_WRITE ;
		}

	  cur = (T_expr *)cur->get_next() ;
	}
}




//
//	Destructeur
//
T_op_call::~T_op_call(void)
{
  TRACE1("--> T_op_call::~T_op_call(%x)", this) ;
}

//
//	}{	Fin du fichier
//





