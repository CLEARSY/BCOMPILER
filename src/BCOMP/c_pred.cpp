/******************************* CLEARSY **************************************
* Fichier : $Id: c_pred.cpp,v 2.0 2004-01-08 09:33:01 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Predicats
*
* Compilation :		-DDEBUG_PRED pour avoir des traces
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
RCS_ID("$Id: c_pred.cpp,v 1.46 2004-01-08 09:33:01 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{ Classe T_predicate
//
T_predicate::~T_predicate(void)
{
}

// Pour savoir si un item  est un predicate
int T_predicate::is_a_predicate(void)
{
#ifdef DEBUG_PRED
  TRACE2("T_predicate(%x:%s)::is_a_predicate() -> TRUE", this, get_class_name()) ;
#endif // DEBUG_PRED
  return TRUE ;
}

//
//	}{	Fonction qui fabrique un item a partir d'une item parenthesee
//
T_item *T_predicate::new_paren_item(T_betree *betree,
											 T_lexem *opn,
											 T_lexem *clo)
{
#ifdef DEBUG_PRED
  TRACE0("T_predicate::new_paren_item") ;
#endif
  // On fabrique un T_predicate_with_parenthesis
  return new T_predicate_with_parenthesis(this, NULL, NULL, NULL, betree, clo) ;
  // Pour le warning
  assert(opn) ;
  return NULL ;
}

//
//	}{	Constructeur de la classe T_binary_pred
//
T_binary_predicate::T_binary_predicate(T_item *new_pred1,
												T_item *new_pred2,
												T_binary_predicate_type new_type,
												T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  : T_predicate(NODE_BINARY_PREDICATE, adr_first,adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE8("T_binary_predicate::T_binary_predicate(%x, %x, %d, %x, %x, %x, %x, %x)",
		 new_pred1, new_pred2, new_type,
		 adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  if (new_pred1->is_a_predicate() == FALSE)
	{
	  TRACE0("ici on crie p1 pas predicat") ;
	  syntax_error(new_pred1->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_BPRED_PRED1_NOT_A_PRED),
				   make_class_name(new_pred1)) ;
	}
  pred1 = (T_predicate *)new_pred1 ;

  if (new_pred2->is_a_predicate() == FALSE)
	{
	  TRACE0("ici on crie p2 pas predicat") ;
	  syntax_error(new_pred2->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_BPRED_PRED2_NOT_A_PRED),
				   make_class_name(new_pred2)) ;
	}
  pred2 = (T_predicate *)new_pred2 ;

  predicate_type = new_type ;

  // On met a jour les liens "father"
  pred1->set_father(this) ;
  pred2->set_father(this) ;

}

// Destructeur
T_binary_predicate::~T_binary_predicate(void)
{
  TRACE1("T_binary_predicate(%x)::~T_binary_predicate", this) ;
}

//
//	}{	Fonction auxiliaire qui extrait une liste de parametres
//
void T_binary_predicate::extract_params(int separator,
												 T_item *father,
												 T_item **adr_first,
												 T_item **adr_last)
{
  TRACE4(">> T_binary_predicate(%x)::extract_params(%d, %x, %x)",
		 this, separator, adr_first, adr_last) ;
  ENTER_TRACE ;

  if (predicate_type == separator)
	{
	  // On extrait les params des fils gauches et droits
	  TRACE0("extract_params fils gauche") ;
	  pred1->extract_params(separator, father, adr_first, adr_last) ;
	  TRACE0("extract_params fils droit") ;
	  pred2->extract_params(separator, father, adr_first, adr_last) ;
	}
  else
	{
	  T_item::extract_params(separator, father, adr_first, adr_last) ;
	}

  EXIT_TRACE ;
  TRACE4("<< T_binary_predicate(%x)::extract_params(%d, %x, %x)",
		 this, separator, adr_first, adr_last) ;
}


//
//	}{	Constructeur de la classe T_expr_predicate
//
//
T_expr_predicate::T_expr_predicate(T_item *new_expr1,
								   T_item *new_expr2,
								   T_expr_predicate_type new_predicate_type,
								   T_item **adr_first,
								   T_item **adr_last,
								   T_item *father,
								   T_betree *betree,
								   T_lexem *ref_lexem)
  : T_predicate(NODE_EXPR_PREDICATE, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE7("T_expr_predicate::T_expr_predicate(%x, %x, %d, %x, %x, %x, %x, %x)",
		 new_expr1, new_expr2, new_predicate_type,
		 adr_first, adr_last, father, betree) ;
#endif

  TRACE2("new_expr1 %x:%s", new_expr1, new_expr1->get_class_name()) ;
  if (new_expr1->is_an_expr() == FALSE)
	{
	  TRACE0("on crie car expr1 n'est pas une expression") ;
	  syntax_expr_expected_error(new_expr1, CAN_CONTINUE) ;
	}
  else
	{
	  expr1 = new_expr1->make_expr() ;
	  expr1->post_check() ;

	  expr1->set_father(this) ;
	}

  TRACE2("new_expr2 %x:%s", new_expr2, new_expr2->get_class_name()) ;
  if (new_expr2->is_an_expr() == FALSE)
	{
	  TRACE0("on crie car expr2 n'est pas une expression") ;
	  syntax_expr_expected_error(new_expr2, CAN_CONTINUE) ;
	}
  else
	{
	  expr2 = new_expr2->make_expr() ;
	  expr2->set_father(this) ;
	  expr2->post_check() ;
	}

  predicate_type = new_predicate_type ;


}

// Destructeur
T_expr_predicate::~T_expr_predicate(void)
{
  TRACE1("T_expr_predicate(%x)::~T_expr_predicate", this) ;
}

//
//	}{	Constructeur de la classe T_typing_predicate
//
T_typing_predicate::T_typing_predicate(T_item *new_pred1,
												T_item *new_pred2,
												T_typing_predicate_type new_type,
												T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  : T_predicate(NODE_TYPING_PREDICATE, adr_first,adr_last, father, betree, ref_lexem)
{
  TRACE9("T_typing_predicate(%x)::T_typing_predicate(%x,%x,%d,%x,%x,%x,%x,%x)",
		 this,
		 new_pred1, new_pred2, new_type,
		 adr_first, adr_last, father, betree, ref_lexem) ;

  first_ident = last_ident = NULL ;

#ifdef MATRA
  //
  // 02/03/1999 : Analyse complete de ce code : cf fiche 2067 de listebug
  //

  // On enleve les eventuelles parentheses pour commencer
  // tout en sachant qu'elles sont non conformes a la BNF
  if (new_pred1->is_an_expr() == TRUE)
	{
	  T_item *save_pred1 = new_pred1 ;
	  //	  TRACE2("ici new_pred1 %x:%s", new_pred1, new_pred1->get_class_name()) ;
	  T_expr *expr = new_pred1->make_expr() ;
	  new_pred1 = expr->strip_parenthesis() ;

	  if (save_pred1 != new_pred1)
		{
		  syntax_warning(save_pred1->get_ref_lexem(),
						 EXTENDED_WARNING,
						 get_warning_msg(W_EXTRA_PAREN_IN_TPRED_IGNORED)) ;

#ifdef COMPAT
		  while (save_pred1 != new_pred1)
			{
			  if (save_pred1->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
				{
				  T_expr_with_parenthesis *ignored =
					(T_expr_with_parenthesis *)save_pred1 ;
				  ignored->mark_as_void() ;
				  save_pred1 = ignored->get_expr() ;
				}
			  else
				{
				  // ???? on sort en catastrophe
				  TRACE0("oups") ;
				  save_pred1 = new_pred1 ;
				}
			}
#endif // COMPAT
		}
	}
#endif // MATRA

  // Et on extrait les paramètres, separes par des virgules
  TRACE1("this %x", this) ;
  TRACE1("&first %x", &first_ident) ;
  TRACE1("&last %x", &last_ident) ;
  TRACE1("new_pred1 %x", new_pred1) ;
  TRACE1("new_pred1 %s", new_pred1->get_class_name()) ;
  separator = BOP_COMMA;
  new_pred1->extract_params((int)BOP_COMMA,
							this,
							(T_item **)&first_ident,
							(T_item **)&last_ident) ;

  //#ifdef MATRA // NON C'EST LE CAS GENERAL, '|->' et ',' sont synonymes
  // Dans certains codes de meteor, on type non pas par "a,b : E * F"
  // mais par "a |-> b : E * F"
  // Apres discussion SL/FBa 03/06/98 on n'est pas 100% que c'est le cas
  // general, en fait ce n'est pas conforme a la BNF des predicats de typage
  // qui impose l'utilisation du ','
  // Mais bon on fait marcher ce cas la car il ne coute pas cher et n'est
  // pas dangereux. Le problème reste toutefois en suspend
  // A FAIRE : mettre un warning ???
  if ( (first_ident == last_ident) && (first_ident->is_an_ident() == FALSE) )
	{
      first_ident = last_ident = NULL ;
      separator = BOP_MAPLET;

	  new_pred1->extract_params((int)BOP_MAPLET,
								this,
								(T_item **)&first_ident,
								(T_item **)&last_ident) ;
	}
  //#endif

  // On verifie que les ident sont des expressions
  T_item *cur = first_ident ;
  while (cur != NULL)
	{
	  if (cur->is_an_expr() == FALSE)
		{
		  TRACE2("ici on crie car %x:%s n'est pas une expression",
				 cur, cur->get_class_name()) ;
		  syntax_expr_expected_error(cur, CAN_CONTINUE) ;
		}
	  else
		{
		  cur->make_expr()->post_check() ;
		}
	  cur->set_father(this) ;
	  cur = cur->get_next() ;
	}

  type = new_pred2 ;
  if (type->is_an_expr() == TRUE)
	{
	  type->post_check() ;
	}

  typing_predicate_type = new_type ;

  // On met a jour les liens "father"
  type->set_father(this) ;

  // On verifie les parametres
  check_params() ;


}

// Destructeur
T_typing_predicate::~T_typing_predicate(void)
{
  TRACE1("T_typing_predicate(%x)::~T_typing_predicate", this) ;
}

// Verification d'integrite syntaxique
void T_typing_predicate::check_params(void)
{
  // Si le type est "=" alors obligatoirement type est une
  // expression, et first_ident = last_ident = une expression
  if (typing_predicate_type == TPRED_EQUAL)
	{
	  if (type->is_an_expr() == FALSE)
		{
		  syntax_error(type->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_NOT_AN_EXPRESSION),
					   make_class_name(type)) ;

		}

	  /*
	  if (first_ident != last_ident)
		{
		  // Le test suffit pour verifier que la liste a un element
		  // le cas first_ident = last_ident = NULL est impossible grace
		  // au parseur LR
		  T_expr *cur = (T_expr *)first_ident->get_next() ;
		  while (cur != NULL)
			{
			  syntax_error(cur->get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_UNEXPECTED_ARGUMENT_IN_EQPRED)) ;
			  cur = (T_expr *)cur->get_next() ;
			}
		}
	  */
	}
}

//
// Ajout d'un parametre en tete de liste
//
void T_typing_predicate::insert_param(T_ident *new_param)
{
  TRACE2("T_typing_predicate(%x)::insert_param(%x)", this, new_param) ;
  new_param->head_insert((T_item **)&first_ident, (T_item **)&last_ident) ;
}

//
//	}{	Constructeur de la classe T_predicate_with_parenthesis
//		Constructeur classique
T_predicate_with_parenthesis::
T_predicate_with_parenthesis(T_predicate *new_predicate,
							 T_item **adr_first,
							 T_item **adr_last,
							 T_item *father,
							 T_betree *betree,
							 T_lexem *ref_lexem)
  :T_predicate(NODE_PREDICATE_WITH_PARENTHESIS, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE6("T_pred_with_paren::T_pred_with_paren(%x,%x,%d,%x,%x,%x,%x,%x)",
		 predicate, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  predicate = new_predicate ;

  // On met a jour les liens "father"
  predicate->set_father(this) ;

#ifdef DEBUG_PRED
  TRACE0("fin de T_predicate_with_parenthesis::T_predicate_with_parenthesis") ;
#endif
}

//
//	}{	Constructeur de la classe T_predicate_with_parenthesis
//		Constructeur a partir d'un T_expr_with_parenthesis deja fait
//		(et contenant un predicat ...)
T_predicate_with_parenthesis::
T_predicate_with_parenthesis(T_expr_with_parenthesis *ref_parexpr,
							 T_item **adr_first,
							 T_item **adr_last,
							 T_item *father,
							 T_betree *betree,
							 T_lexem *ref_lexem)
  :T_predicate(NODE_PREDICATE_WITH_PARENTHESIS,
			   adr_first,
			   adr_last,
			   father,
			   betree,
			   ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE6("T_pred_with_paren::T_pred_with_paren(%x,%x,%d,%x,%x,%x,%x,%x)",
		 predicate, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  predicate = (T_predicate *)ref_parexpr->get_expr() ;
  ASSERT(predicate->is_a_predicate() == TRUE) ; // Verifie par l'appelant

  // On met a jour les liens "father"
  predicate->set_father(this) ;

  // On dit qu'il est "a nous"
  //  predicate->link() ;

  TRACE0("fin de T_predicate_with_parenthesis::T_predicate_with_parenthesis") ;
}

// Destructeur
T_predicate_with_parenthesis::~T_predicate_with_parenthesis(void)
{
}

//
//	}{	Constructeur de la classe T_not_predicate
//
T_not_predicate:: T_not_predicate(T_predicate *new_predicate,
										   T_item **adr_first,
										   T_item **adr_last,
										   T_item *father,
										   T_betree *betree,
										   T_lexem *ref_lexem)
  :T_predicate(NODE_NOT_PREDICATE, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE6("T_not_predicate::T_not_predicate(%x,%x,%d,%x,%x,%x,%x,%x)",
		 predicate, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  predicate = (T_predicate *)new_predicate ;

  // On met a jour les liens "father"
  predicate->set_father(this) ;

  TRACE0("fin de T_not_predicate::T_not_predicate") ;
}

// Destructeur
T_not_predicate::~T_not_predicate(void)
{
  TRACE1("T_not_predicate(%x)::~T_not_predicate", this) ;
}

//
//	}{	Constructeur de la classe T_universal_predicate
//
T_universal_predicate::T_universal_predicate(T_item *new_variable,
													  T_item *new_expr,
													  T_item **adr_first,
													  T_item **adr_last,
													  T_item *father,
													  T_betree *betree,
													  T_lexem *ref_lexem)
  :T_predicate(NODE_UNIVERSAL_PREDICATE,
			   adr_first,
			   adr_last,
			   father,
			   betree,
			   ref_lexem)
{
#ifdef DEBUG_PRED
  TRACE7("T_universal_predicate::T_universal_predicate(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
#endif
  /*
    On a, pour %x.(Pred | Expr) :
           '.'
   		   / \
          / '()'
        '!'  |     (!!! le '!' n'est pas dans l'arbre)
         |  '|'
        'x' / \    (x ou binop  '(x,y)')
          Pred Expr
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // Variables
  first_variable = last_variable = NULL ;

  if (new_variable->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  // On s'interesse au contenu !!
	  TRACE0("on lit le contenu de new_variable") ;
	  new_variable =
		((T_expr_with_parenthesis *)new_variable)->strip_parenthesis() ;
	}

  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // Verification : les variables doivent etre des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  TRACE3("cur_variable %x type %s lex %s",
			 cur_variable,
			 cur_variable->get_class_name(),
			 cur_variable->get_ref_lexem()->get_lex_ascii()) ;
	  if (cur_variable->is_an_ident() == FALSE)
		{
		  syntax_unexpected_error(cur_variable->get_ref_lexem(),
								  CAN_CONTINUE,
								  get_catalog(C_QUANT_VAR)) ;
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
  if (new_expr->get_node_type() != NODE_PREDICATE_WITH_PARENTHESIS)
	{
	  TRACE1("on crie car l'expression en partie droite (%s) n'est pas ()",
			 new_expr->get_class_name()) ;
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PARPRED)) ;
	}

  predicate = (T_predicate_with_parenthesis *)new_expr ;
  // Pour STS, on supprime les parenthèses en trop (Gub 3091)
  {
	T_predicate *child =
	  ((T_predicate_with_parenthesis *)predicate)->get_predicate() ;
	while (child->get_node_type() == NODE_PREDICATE_WITH_PARENTHESIS)
	  {
		predicate = child;
		child = ((T_predicate_with_parenthesis *)child)->get_predicate() ;
	  }
  }
  predicate->set_father(this) ;

  // L'expression en partie droite doit etre du type P1 => P2,
  // c'est à dire un T_binary_predicate de type BPRED_IMPLIES
  T_predicate *inside ;
  // Cast valide par construction cf 3 lignes au dessus
  inside = ((T_predicate_with_parenthesis *)predicate)->get_predicate() ;
  // Cast justife ci-apres le cas echeant
  T_binary_predicate *binside = (T_binary_predicate *)inside ;

  if (    (inside->get_node_type() != NODE_BINARY_PREDICATE)
	   || (binside->get_predicate_type() != BPRED_IMPLIES) )
	{
	  syntax_error(inside->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_INVALID_UNIVERSAL_PREDICATE)) ;
	}

  TRACE0("fin de T_universal_predicate::T_universal_predicate") ;


}

// Destructeur
T_universal_predicate::~T_universal_predicate(void)
{
  TRACE1("T_universal_predicate(%x)::~T_universal_predicate", this) ;
}

//
//	}{	Constructeur de la classe T_existantial_predicate
//
T_existential_predicate::T_existential_predicate(T_item *new_variable,
														  T_item *new_expr,
														  T_item **adr_first,
														  T_item **adr_last,
														  T_item *father,
														  T_betree *betree,
														  T_lexem *ref_lexem)
  :T_predicate(NODE_EXISTENTIAL_PREDICATE,
			   adr_first,
			   adr_last,
			   father,
			   betree,
			   ref_lexem)
{
  TRACE7("T_existential_predicate::T_existential_prede(%x,%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_variable, new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
  /*
  //
  // On a, pour %x.(Pred | Expr) :
  //       '.'
  //		 / \
  //      / '()'
  //    '!'  |     (!!! le '!' n'est pas dans l'arbre)
  //     |  '|'
  //    'x' / \    (x ou binop  '(x,y)')
  //      Pred Expr
  //
  */

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // Variables
  first_variable = last_variable = NULL ;

  if (new_variable->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  // On s'interesse au contenu !!
	  TRACE0("on lit le contenu de new_variable") ;
	  new_variable =
		((T_expr_with_parenthesis *)new_variable)->strip_parenthesis() ;
	}

  new_variable->extract_params((int)BOP_COMMA,
							   this,
							   (T_item **)&first_variable,
							   (T_item **)&last_variable) ;

  // Verification : les variables doivent etre des identificateurs
  T_item *cur_variable = first_variable ;

  while (cur_variable != NULL)
	{
	  TRACE3("cur_variable %x type %s lex %s",
			 cur_variable,
			 cur_variable->get_class_name(),
			 cur_variable->get_ref_lexem()->get_lex_ascii()) ;
	  if (cur_variable->get_node_type() != NODE_IDENT)
		{
		  syntax_unexpected_error(cur_variable->get_ref_lexem(),
								  CAN_CONTINUE,
								  get_catalog(C_QUANT_VAR)) ;
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
  if (new_expr->get_node_type() != NODE_PREDICATE_WITH_PARENTHESIS)
	{
	  TRACE1("on crie car l'expression en partie droite (%s) n'est pas un pred ()",
			 new_expr->get_class_name()) ;
	  syntax_unexpected_error(new_expr,
							  FATAL_ERROR,
							  get_catalog(C_PARPRED)) ;
	}

  predicate = (T_predicate_with_parenthesis *)new_expr ;
  predicate->set_father(this) ;

  TRACE0("fin de T_existential_predicate::T_existential_predicate") ;

}

// Destructeur
T_existential_predicate::~T_existential_predicate(void)
{
  TRACE1("T_existential_predicate(%x)::~T_existential_predicate", this) ;
}

//
//	}{	Correction de l'arbre
//
T_item *T_typing_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_typing_predicate(%x)::check_tree", this) ;
#endif

  if (first_ident == NULL)
	{
	  return NULL ;
	}

  T_expr *cur_item = first_ident ;
  T_item *tmp_first = NULL ;
  T_item *tmp_last = NULL ;

  while (cur_item != NULL)
	{
	  // Cast justifie car on fait un check d'une expression, qui ne
	  // peut que rendre une expression. Le ASSERT ci-apres le
	  // verifie a posteriori

	  // On memorise le next avant de casser le chainage
	  T_expr *next = (T_expr *)cur_item->get_next() ;
	  T_expr *res = cur_item->check_tree(NULL)->make_expr() ;

	  if (res->is_an_expr() == FALSE)
		{
		  // Reprise sur erreur
		  return this ;
		}

	  res->tail_insert(&tmp_first, &tmp_last) ;

#ifdef DEBUG_CHECK
	  TRACE2("res (dst) %x, cur_item (src) %x", res, cur_item) ;
#endif // DEBUG_CHECK
	  cur_item = next ;
	}

  first_ident = tmp_first->make_expr() ;
  last_ident = tmp_last->make_expr() ;

  type->check_tree((T_item **)&type) ;

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_predicate_with_parenthesis::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_predicate_with_parenthesis(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_not_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_not_predicate(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_binary_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1(">> T_binary_predicate(%x)::check_tree", this) ;
  ENTER_TRACE ; ENTER_TRACE ;
#endif

#ifdef DEBUG_CHECK
  TRACE1("--> check fils gauche %x", pred1) ;
  ENTER_TRACE ;
#endif // DEBUG_CHECK

  if (pred1 != NULL)
	{
	  pred1->check_tree((T_item **)&pred1) ;
	}

#ifdef DEBUG_CHECK
  TRACE1("--> check fils droit %x", pred2) ;
  ENTER_TRACE ;
#endif // DEBUG_CHECK

  if (pred2 != NULL)
	{
	  pred2->check_tree((T_item **)&pred2) ;
	}

#ifdef DEBUG_CHECK
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE2("<< T_binary_predicate(%x)::check_tree() -> %x", this, this) ;
#endif // DEBUG_CHECK

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_expr_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_expr_predicate(%x)::check_tree", this) ;
#endif

  if (expr1 != NULL)
	{
	  expr1->check_tree((T_item **)&expr1) ;
	}

  if (expr2 != NULL)
	{
	  expr2->check_tree((T_item **)&expr2) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_universal_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_universal_predicate(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

T_item *T_existential_predicate::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_existential_predicate(%x)::check_tree", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

//
//	}{	Fin du fichier
//

