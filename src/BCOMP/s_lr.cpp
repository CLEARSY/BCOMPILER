/******************************* CLEARSY **************************************
* Fichier : $Id: s_lr.cpp,v 2.0 2001-07-19 16:31:33 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Moteur de l'analyseur LR
*
* Compilations :	-DLR_TRACE pour avoir des traces de l'analyse LR
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
RCS_ID("$Id: s_lr.cpp,v 1.16 2001-07-19 16:31:33 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_lr.h"
#include "s_lrtab.h"

//
//	}{	Instanciation explicite des templates
//
#define DEFINE_TEMPLATES
#include "s_lrstck.cpp"

template class T_lr_stack<T_lr_state> ;
template class T_lr_stack<T_object *> ;
template class T_lr_stack<T_lexem *> ;

//
//	Type : que faire dans le flux de lexemes ?
//
typedef enum
	{
	LSTREAM_NEXT,		// Passer au lexeme suivant
	LSTREAM_PREV,		// Passer au lexeme precedent
	LSTREAM_STAY		// Ne pas changer de lexeme
	} T_lexem_stream_action ;

//
//	}{	Forward :: Analyse LR, warning :: opn pas ferme
//
static void warning_open(T_lr_stack<T_lexem *> *open_stack)
{
while (open_stack->is_empty() == FALSE)
	{
	T_lexem *opn = open_stack->pop() ;
	syntax_warning(opn,
				   BASE_WARNING,
				   get_warning_msg(W_OPEN_LEXEM_NOT_CLOSED)) ;
	}
}

//
// }{ Forward :: Analyse LR, version "interne"
//
static T_item *LR_analysis(T_item **adr_first,
									T_item **adr_last,
									T_betree *betree,
									T_lexem **adr_cur_lexem) ;

//
//	}{	Fonction qui appelle l'analyseur LR
//
T_item *syntax_call_LR_parser(T_item **adr_first,
									   T_item **adr_last,
									   T_item *father,
									   T_betree *betree,
									   T_lexem **adr_cur_lexem)
{
#ifdef LR_TRACE
  TRACE0(">> syntax_call_LR_PARSER --> appel de l'analyseur LR") ;
  ENTER_TRACE ;
#endif

T_item *res = NULL ;

res = LR_analysis(adr_first, adr_last, betree, adr_cur_lexem) ;

// Mise a jour du champ father
res->set_father(father) ;

#ifdef LR_TRACE
  TRACE1("<< syntax_call_LR_PARSER --> %x", res) ;
  ENTER_TRACE ;
#endif

  return res ;
}


//
//	}{	Fonction de shift - renvoie TRUE
//
static T_lexem_stream_action LR_shift(T_lexem *cur_lexem,
											   T_lr_stack<T_lr_state> *state_stack,
											   T_lr_stack<T_object *> *symbol_stack,
											   T_lr_state new_state)
{
#ifdef LR_TRACE
TRACE4("LR_SHIFT(%x, %x, %x, %d)", cur_lexem, state_stack, symbol_stack, new_state);
#endif
symbol_stack->push(cur_lexem) ;
state_stack->push(new_state) ;
return LSTREAM_NEXT ;
}


//
//	}{	Fonction de Reduce 1 - renvoie FALSE
//
// Forward
static T_lr_state LR_get_next_state(T_lr_state cur_state) ;
// Fonction de reduce 1
static T_lexem_stream_action LR_reduce_1(T_betree *betree,
								T_lr_stack<T_lr_state> *state_stack,
								T_lr_stack<T_object *> *symbol_stack)
{
#ifdef LR_TRACE
TRACE0("ON A RECONNU EXPR->EXPR BIN EXPR :: on fait REDUCE 1") ;
ENTER_TRACE ;

TRACE0("On depile 3 symboles par pile") ;
#endif

state_stack->pop(3) ;

// Dans la pile des symboles on a de haut en bas op2, oper et op1
// par construction du parseur LR
T_expr *op2 = (T_expr *)symbol_stack->pop() ;
T_lexem *oper = (T_lexem *)symbol_stack->pop() ;
T_expr *op1 = (T_expr *)symbol_stack->pop() ;

#ifdef LR_TRACE
TRACE5("on fabrique le binop op1=%x:%s op2=%x:%s oper=%s",
	   op1,
	   op1->get_class_name(),
	   op2,
	   op2->get_class_name(),
	   (oper == NULL) ? "NULL" : oper->get_lex_name()) ;
#endif

T_item *binop = new_binary_item(op1, op2, oper, betree) ;

#ifdef LR_TRACE
TRACE0("On empile le binop") ;
#endif

symbol_stack->push(binop) ;
T_lr_state next_state = LR_get_next_state(state_stack->get_top()) ;

#ifdef LR_TRACE
TRACE1("On empile Etat suivant -> %d", next_state) ;
#endif

state_stack->push(next_state) ;

#ifdef LR_TRACE
EXIT_TRACE ;
#endif

return LSTREAM_STAY ;
}

//
//	}{	Fonction de Reduce 2 - renvoie FALSE
//
static T_lexem_stream_action LR_reduce_2(T_betree *betree,
								T_lr_stack<T_lr_state> *state_stack,
								T_lr_stack<T_object *> *symbol_stack)
{
#ifdef LR_TRACE
TRACE0("ON A RECONNU EXPR->UNR EXPR :: on fait REDUCE 2") ;
ENTER_TRACE ;

TRACE0("on commence par construire le T_unary_op") ;
#endif

// Dans la pile on a de haut en bas Expr puis UNR
T_item *operand = (T_expr *)symbol_stack->pop() ;
T_lexem *oper = (T_lexem *)symbol_stack->pop() ;
#ifdef LR_TRACE
TRACE2("oper = %x %s", oper, oper->get_class_name()) ;
#endif
ASSERT(oper->get_node_type() == NODE_LEXEM) ;

T_item *unop = new_unary_item(operand,
							  oper,
							  betree) ;

state_stack->pop(2) ;

#ifdef LR_TRACE
TRACE0("On empile le noeud ") ;
#endif
symbol_stack->push(unop) ;

T_lr_state next_state = LR_get_next_state(state_stack->get_top()) ;
#ifdef LR_TRACE
TRACE1("On empile Etat suivant -> %d", next_state) ;
#endif
state_stack->push(next_state) ;

// C'est fini. On n'avance pas apres un reduce
#ifdef LR_TRACE
EXIT_TRACE ;
#endif
return LSTREAM_STAY ;
}

//
//	}{	Fonction de Reduce 3 - renvoie FALSE
//
static T_lexem_stream_action LR_reduce_3(T_betree *betree,
								T_lr_stack<T_lr_state> *state_stack,
								T_lr_stack<T_object *> *symbol_stack)
{
#ifdef LR_TRACE
TRACE0("ON A RECONNU EXPR->EXPR PST :: on fait REDUCE 3") ;
ENTER_TRACE ;
#endif

// Dans la pile on a de haut en bas PST puis EXPR
T_lexem *pst = (T_lexem *)symbol_stack->pop() ;
#ifdef LR_TRACE
TRACE2("pst = %x %s", pst, pst->get_class_name()) ;
#endif
ASSERT(pst->get_node_type() == NODE_LEXEM) ;

T_item *operand_tmp = (T_expr *)symbol_stack->pop() ;

if (operand_tmp->is_an_expr() == FALSE)
  {
	syntax_expr_expected_error(operand_tmp, FATAL_ERROR) ;
  }

T_expr *operand = operand_tmp->make_expr() ;

T_expr *pst_node = new_pst(operand, pst, betree, operand->get_ref_lexem()) ;

state_stack->pop(2) ;

#ifdef LR_TRACE
TRACE0("On empile le noeud ") ;
#endif
symbol_stack->push(pst_node) ;

T_lr_state next_state = LR_get_next_state(state_stack->get_top()) ;
#ifdef LR_TRACE
TRACE1("On empile Etat suivant -> %d", next_state) ;
#endif
state_stack->push(next_state) ;

// C'est fini. On n'avance pas apres un reduce
#ifdef LR_TRACE
EXIT_TRACE ;
#endif
return LSTREAM_STAY ;
}

//
//	}{	Fonction de Reduce 4 - renvoie FALSE
//
static T_lexem_stream_action LR_reduce_4(T_betree *betree,
								T_lr_stack<T_lr_state> *state_stack,
								T_lr_stack<T_object *> *symbol_stack)
{
#ifdef LR_TRACE
TRACE0("ON A RECONNU EXPR->ATOM :: on fait REDUCE 4") ;
ENTER_TRACE ;

TRACE0("on commence par construire l'atome") ;
#endif

// Le cast est valide par construction de l'automate LR
T_item *atom = new_atom(betree, (T_lexem *)(symbol_stack->get_top())) ;

#ifdef LR_TRACE
TRACE0("On depile 1 symbole par pile") ;
#endif
state_stack->pop() ;
symbol_stack->pop() ;

#ifdef LR_TRACE
TRACE2("On empile le noeud ATOM %x (%s)", atom, atom->get_class_name()) ;
#endif
symbol_stack->push(atom) ;

T_lr_state next_state = LR_get_next_state(state_stack->get_top()) ;
#ifdef LR_TRACE
TRACE1("On empile Etat suivant -> %d", next_state) ;
#endif
state_stack->push(next_state) ;

// C'est fini. On n'avance pas apres un reduce
#ifdef LR_TRACE
EXIT_TRACE ;
#endif
return LSTREAM_STAY ;
}

//
//	}{	Fonction de Reduce 5 - renvoie FALSE
//
static T_lexem_stream_action LR_reduce_5(T_betree *betree,
								T_lr_stack<T_lr_state> *state_stack,
								T_lr_stack<T_object *> *symbol_stack)
{
#ifdef LR_TRACE
TRACE0("ON A RECONNU EXPR->OPN BIN CLO :: on fait REDUCE 5") ;
ENTER_TRACE ;

TRACE0("On depile 3 symboles par pile") ;
#endif
state_stack->pop(3) ;

// Dans la pile des symboles on a de haut en bas CLO expr OPN
// Au milieu, on a une expression ou un predicat
T_lexem *clo = (T_lexem *)symbol_stack->pop() ;
T_item *expr_or_pred = (T_item *)symbol_stack->pop() ;
T_lexem *opn = (T_lexem *)symbol_stack->pop() ;

#ifdef LR_TRACE
TRACE3("clo = %x expr_or_pred = %x opn = %x", clo, expr_or_pred, opn) ;
#endif
if (clo->get_node_type() != NODE_LEXEM)
  {
	TRACE2("panic clo %x node_type %d", clo, clo->get_node_type()) ;
	TRACE1("symbol_stack = %x", symbol_stack) ;
  }
else
  {
	TRACE2("ok clo %x node_type %d", clo, clo->get_node_type()) ;
  }
ASSERT(clo->get_node_type() == NODE_LEXEM) ;		// garanti par Ana LR
#ifdef LR_TRACE
TRACE1("opn = %s", opn->get_lex_name()) ;
#endif
ASSERT(opn->get_node_type() == NODE_LEXEM) ;		// garanti par Ana LR

T_item *paren = expr_or_pred->new_paren_item(betree, opn, clo)  ;

#ifdef LR_TRACE
TRACE0("On empile l'expr construite") ;
#endif
symbol_stack->push(paren) ;

T_lr_state next_state = LR_get_next_state(state_stack->get_top()) ;
#ifdef LR_TRACE
TRACE1("On empile Etat suivant -> %d", next_state) ;
#endif
state_stack->push(next_state) ;

#ifdef LR_TRACE
EXIT_TRACE ;
#endif
return LSTREAM_STAY ;
}

//
//	}{	Fonction qui donne l'etat suivant
//
static T_lr_state LR_get_next_state(T_lr_state cur_state)
{
switch(cur_state)
	{
	case LR_STATE_E0 : { return LR_STATE_E4 ; } ;
	case LR_STATE_E1 : { return LR_STATE_E5 ; } ;
	case LR_STATE_E3 : { return LR_STATE_E6 ; } ;
	case LR_STATE_E8 : { return LR_STATE_E10 ; } ;

	default :
		{
		// Impossible, par construction du parseur LR
		internal_error(NULL,
					   __FILE__,
					   __LINE__,
					   get_error_msg(E_LR_STATE_DOES_NOT_HAVE_A_SUCCESSOR),
					   cur_state) ;
		}
	}

// Pour le warning
return LR_STATE_E0 ;
}

//
//	}{	Fonctions d'action
//
// Type generique : pointeur sur fonction action
// i.e. fonction qui prend en argument ... et qui retournent int
typedef T_lexem_stream_action (*T_LR_action)(T_betree *betree,
						   T_lexem *cur_lexem,
						   T_lr_stack<T_lr_state> *state_stack,
						   T_lr_stack<T_object *> *symbol_stack,
						   T_lr_stack<T_lexem *> *open_stack,
						   int *adr_stop) ;

//
//	}{ Action LR a partir de l'etat F0
//
static T_lexem_stream_action LR_action_F0(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F0") ;
#endif

switch(get_lexem_family(cur_lexem))
	{
	case FAMILY_ATOM :
		{
		// SHIFT E2
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E2)  ;
		}

	case FAMILY_UNR :
		{
		// SHIFT E3
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E3)  ;
		}

	case FAMILY_BIN :
		{
		if (cur_lexem->get_lex_type() == L_MINUS)
			{
			// On change le '-' binaire en '-' unaire et on recommence
#ifdef LR_TRACE
			TRACE0("Le - est un - binaire : on recommence") ;
#endif
			cur_lexem->set_lex_type(L_UMINUS) ;
			return LSTREAM_STAY ;
            }
        else
            {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_UNEXPECTED_BINOP),
						 cur_lexem->get_lex_name()) ;
			warning_open(open_stack) ;
			stop() ;
			}
		}

	case FAMILY_OPN :
		{
		// SHIFT E1
		open_stack->push(cur_lexem) ;
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E1)  ;
		}

	default :
		{
		syntax_unexpected_error(cur_lexem,
								CAN_CONTINUE,
								get_catalog(C_ATOM_UN_BINOP_OPN)) ;
		warning_open(open_stack) ;
		stop() ;
		}
	}

// Pour les warnings
return LSTREAM_NEXT ;

ASSERT(betree) ;
ASSERT(adr_stop) ;
}


//
//	}{ Action LR a partir de l'etat F1
//
static T_lexem_stream_action LR_action_F1(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F1") ;
#endif

switch(get_lexem_family(cur_lexem))
	{
	case FAMILY_ATOM :
		{
		// SHIFT E2
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E2)  ;
		}

	case FAMILY_UNR :
		{
		// SHIFT E3
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E3)  ;
		}

	case FAMILY_OPN :
		{
		// SHIFT E1
		open_stack->push(cur_lexem) ;
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E1)  ;
		}

    case FAMILY_BIN :
        {
        if (cur_lexem->get_lex_type() == L_MINUS)
            {
			// On change le '-' binaire en '-' unaire et on recommence
			// Si le bin dans la pile est un +,-,*,/, mod on met un
			// warning
			T_lexem *binop = (T_lexem *)symbol_stack->get_top() ;
#ifdef LR_TRACE
			TRACE1("binop %s", binop->get_class_name()) ;
#endif
			ASSERT(binop->get_node_type() == NODE_LEXEM) ;
			switch(binop->get_lex_type())
				{
				case L_PLUS :
				case L_MINUS :
				case L_TIMES :
				case L_DIVIDES :
                case L_MOD :
					{
					// Warning
					// Lexeme, sans les '' autour
					const char *tmp = binop->get_lex_ascii() ;
					syntax_warning(cur_lexem,
								   BASE_WARNING,
								   get_warning_msg(W_AMBIGOUS_MINUS)) ;
					syntax_warning(binop,
								   BASE_WARNING,
								   get_warning_msg(W_AMBIGOUS_MINUS_HINT),
								   tmp,
								   tmp) ;
					}

				default :
					{
					// On ne fait rien par defaut (pas de warning) !
					}
				}
#ifdef LR_TRACE
			TRACE0("Le - est un - binaire : on recommence") ;
#endif
			cur_lexem->set_lex_type(L_UMINUS) ;
			return LSTREAM_STAY ;
			}
        else
            {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_UNEXPECTED_BINOP),
						 cur_lexem->get_lex_name()) ;
			warning_open(open_stack) ;
			stop() ;
			}
		}

	default :
		{
		syntax_unexpected_error(cur_lexem,
								FATAL_ERROR,
								get_catalog(C_ATOM_UNOP_OPN)) ;
		}
	}

// Pour les warnings
return LSTREAM_STAY ;
ASSERT(betree) ;
ASSERT(adr_stop) ;
}


//
//	}{ Action LR a partir de l'etat F2
//
static T_lexem_stream_action LR_action_F2(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE3("LR_action_F2 cur_lexem %x:%s family %d",
	   cur_lexem ,
	   cur_lexem->get_lex_name(),
	   get_lexem_family(cur_lexem)) ;
#endif

switch(get_lexem_family(cur_lexem))
	{
	case FAMILY_PST :
		{
		// SHIFT E7
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E7)  ;
		}

	case FAMILY_BIN :
		{
		// SHIFT E8
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E8)  ;
		}

	case FAMILY_OPN :
		{
		T_lex_type lex_type = cur_lexem->get_lex_type() ;

		// Si on a '(' ou '[' on insere evl et on recommence
		if ( (lex_type == L_OPEN_PAREN) || (lex_type == L_OPEN_BRACKET) )
			{
#ifdef LR_TRACE
			TRACE0("on insere EVL et on recommence") ;
#endif
			(void)new T_lexem(cur_lexem, L_EVL) ;
			return LSTREAM_PREV ;
			}

		// Sinon il y a erreur
		open_stack->push(cur_lexem) ;
		syntax_unexpected_error(cur_lexem,
								CAN_CONTINUE,
								get_catalog(C_OPN_PAREN_OR_BRACKET)) ;
		warning_open(open_stack) ;
		stop() ;
		}

	case FAMILY_ATOM :
		{
		// Erreur !
		syntax_unexpected_error(cur_lexem,
								FATAL_ERROR,
								get_catalog(C_BINOP_OR_OPN_OR_TILDA)) ;
		}

	default :
		{
#ifdef LR_TRACE
		TRACE0("ACCEPT") ;
#endif
		*adr_stop = TRUE ;
		return LSTREAM_STAY ;
		}
	}

// Pour les warnings
return LSTREAM_STAY ;
ASSERT(betree) ;
ASSERT(adr_stop) ;
}


//
//	}{ Action LR a partir de l'etat F3
//
static T_lexem_stream_action LR_action_F3(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F3") ;
#endif

switch(get_lexem_family(cur_lexem))
	{
	case FAMILY_PST :
		{
		// SHIFT E7
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E7)  ;
		}

	case FAMILY_BIN :
		{
		// SHIFT E8
		return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E8)  ;
		}

	case FAMILY_OPN :
		{
		T_lex_type lex_type = cur_lexem->get_lex_type() ;

		// Si on a '(' ou '[' on insere evl et on recommence
		if ( (lex_type == L_OPEN_PAREN) || (lex_type == L_OPEN_BRACKET) )
			{
#ifdef LR_TRACE
			TRACE0("on insere EVL et on recommence") ;
#endif
			(void)new T_lexem(cur_lexem, L_EVL) ;
			return LSTREAM_PREV ;
			}

		// Sinon il y a erreur
		open_stack->push(cur_lexem) ;
		syntax_unexpected_error(cur_lexem,
									  CAN_CONTINUE,
									  get_catalog(C_OPN_PAREN_OR_BRACKET)) ;
		warning_open(open_stack) ;
		stop() ;
		}

	case FAMILY_CLO :
		{
		// Il faut verifier que l'ouvrant dans la pile correspond a ce CLO
		// Cet OPN est a la profondeur 1 dans la pile (pile : OPN EXPR)
#ifdef LR_TRACE
		TRACE0("VERIF OPN MATCHE CLO") ;
#endif
		T_lexem *opn = (T_lexem *)symbol_stack->get_depth(1) ;
		ASSERT(opn->get_node_type() == NODE_LEXEM) ;
#ifdef LR_TRACE
		TRACE2("opn = %s clo = %s",
			   opn->get_lex_name(), cur_lexem->get_lex_name()) ;
#endif

		T_lex_type t_opn = opn->get_lex_type() ;
		T_lex_type t_clo = cur_lexem->get_lex_type() ;

		int match = (	 ( (t_opn == L_OPEN_PAREN)  && (t_clo == L_CLOSE_PAREN) )
					  || ( (t_opn == L_OPEN_BRACES) && (t_clo == L_CLOSE_BRACES) )
					  || ( (t_opn == L_OPEN_BRACKET)&&(t_clo == L_CLOSE_BRACKET))) ;

		if (match)
			{
#ifdef LR_TRACE
			TRACE0("Ok : clo matche opn") ;
#endif
			open_stack->pop() ;
			// SHIFT E9
			return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E9)  ;
			}
		else
			{
			/// Erreur !
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_CLO_DOES_NOT_MATCH_OPN),
						 cur_lexem->get_lex_name()) ;
			syntax_error(opn,
						 CAN_CONTINUE,
						 get_error_msg(E_OPN_LOCALISATION),
						 opn->get_lex_name()) ;
			warning_open(open_stack) ;
			stop() ;
			}
		}

	default :
		{
		syntax_unexpected_error(cur_lexem,
									  CAN_CONTINUE,
									  get_catalog(C_ATOM_UN_BINOP_OPN_CLO)) ;
		warning_open(open_stack) ;
		stop() ;
		}
	}

// Pour les warnings
return LSTREAM_STAY ;
ASSERT(betree) ;
ASSERT(adr_stop) ;
}


//
//	}{ Action LR a partir de l'etat F_3
//
static T_lexem_stream_action LR_action_F_3(T_betree *betree,
										   T_lexem *cur_lexem,
										   T_lr_stack<T_lr_state> *state_stack,
										   T_lr_stack<T_object *> *symbol_stack,
										   T_lr_stack<T_lexem *> *open_stack,
										   int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F3 -> REDUCE 3") ;
#endif
// REDUCE 4
return LR_reduce_3(betree, state_stack, symbol_stack) ;
ASSERT(cur_lexem) ;
ASSERT(adr_stop) ;
ASSERT(open_stack) ;
}

//
//	}{ Action LR a partir de l'etat F_4
//
static T_lexem_stream_action LR_action_F_4(T_betree *betree,
										   T_lexem *cur_lexem,
										   T_lr_stack<T_lr_state> *state_stack,
										   T_lr_stack<T_object *> *symbol_stack,
										   T_lr_stack<T_lexem *> *open_stack,
										   int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F_4 -> REDUCE 4") ;
#endif
// REDUCE 4
return LR_reduce_4(betree, state_stack, symbol_stack) ;
ASSERT(cur_lexem) ;
ASSERT(adr_stop) ;
ASSERT(open_stack) ;
}

//
//	}{ Action LR a partir de l'etat F4
//
static T_lexem_stream_action LR_action_F4(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F4") ;
#endif

T_lexem_family family = get_lexem_family(cur_lexem) ;

switch(family)
	{
	case FAMILY_PST :
	case FAMILY_BIN :
		{
		// Lexem courant = PST
		// Il faut verifier la priorite du Unr dans la pile
		// Dans la pile on a de haut en base EXPR UNR
		T_lexem *unr = (T_lexem *)symbol_stack->get_depth(1) ;
#ifdef LR_TRACE
		TRACE1("unr = %x", unr) ;
#endif
		ASSERT(get_lexem_family(unr) == FAMILY_UNR) ; // garanti par ana LR

		int prio = get_lexem_prio(cur_lexem) ;
		int unr_prio = get_lexem_prio(unr) ;
#ifdef LR_TRACE
		TRACE2("unr_in_stack prio %d prio %d", unr_prio, prio) ;
#endif
		if (unr_prio > prio)
			{
			// Reduce 2
			return LR_reduce_2(betree, state_stack, symbol_stack) ;
			}
		else
			{
#ifdef LR_TRACE
			TRACE0("on fait shift E7 si PST ou E8 si BIN") ;
#endif
			if (family == FAMILY_PST)
				{
				// Shift E7
				return LR_shift(cur_lexem,state_stack,symbol_stack,LR_STATE_E7)  ;
				}
			else // family == BIN, garanti par le case
				{
				// SHIFT E8
				return LR_shift(cur_lexem,state_stack,symbol_stack,LR_STATE_E8)  ;
				}
			}
		}

	case FAMILY_OPN :
		{
		T_lex_type lex_type = cur_lexem->get_lex_type() ;

		// Si on a '(' ou '[' on insere evl et on recommence
		if ( (lex_type == L_OPEN_PAREN) || (lex_type == L_OPEN_BRACKET) )
			{
#ifdef LR_TRACE
			TRACE0("on insere EVL et on recommence") ;
#endif
			(void)new T_lexem(cur_lexem, L_EVL) ;
			return LSTREAM_PREV ;
			}
		else
			{
			// Sinon Reduce 2
			open_stack->push(cur_lexem) ;
			return LR_reduce_2(betree, state_stack, symbol_stack) ;
			}
		}

	default :
		{
		// RED 2
#ifdef LR_TRACE
		TRACE0("ON A RECONNU EXPR->UNR EXPR :: on fait REDUCE 2") ;
#endif
		return LR_reduce_2(betree, state_stack, symbol_stack) ;
		}
	}

// Pour les warnings
return LSTREAM_STAY ;
ASSERT(adr_stop) ;
}


//
//	}{ Action LR a partir de l'etat F_5
//
static T_lexem_stream_action LR_action_F_5(T_betree *betree,
										   T_lexem *cur_lexem,
										   T_lr_stack<T_lr_state> *state_stack,
										   T_lr_stack<T_object *> *symbol_stack,
										   T_lr_stack<T_lexem *> *open_stack,
										   int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F_5") ;
#endif
return LR_reduce_5(betree, state_stack, symbol_stack) ;
ASSERT(cur_lexem) ;
ASSERT(adr_stop) ;
ASSERT(open_stack) ;
}

//
//	}{ Action LR a partir de l'etat F5
//
static T_lexem_stream_action LR_action_F5(T_betree *betree,
										  T_lexem *cur_lexem,
										  T_lr_stack<T_lr_state> *state_stack,
										  T_lr_stack<T_object *> *symbol_stack,
										  T_lr_stack<T_lexem *> *open_stack,
										  int *adr_stop)
{
#ifdef LR_TRACE
TRACE0("LR_action_F5") ;
#endif

switch(get_lexem_family(cur_lexem))
	{
	case FAMILY_PST :
		{
		  // Il faut comparer la priorite du lexeme courant "Pst" avec
		  // le Bin dans la pile (pile de haut en bas : EXPR BIN)
		  // SHIFT E7
		  T_lexem *bin_in_stack = (T_lexem *)symbol_stack->get_depth(1) ;
		  ASSERT(bin_in_stack->get_node_type() == NODE_LEXEM) ;
		  ASSERT(get_lexem_family(bin_in_stack) == FAMILY_BIN) ;
		  int cur_prio = get_lexem_prio(cur_lexem) ;
#ifdef LR_TRACE
		  TRACE1("cur_prio = %d", cur_prio) ;
#endif
		  int stack_prio = get_lexem_prio(bin_in_stack) ;
#ifdef LR_TRACE
		  TRACE1("stack_prio = %d", stack_prio) ;
#endif
		  if (stack_prio > cur_prio)
			{
			  // Reduce 1
#ifdef LR_TRACE
			  TRACE0("prio bin > prio pst danc r1") ;
#endif
			  return LR_reduce_1(betree, state_stack, symbol_stack) ;
			}
		  else
			{
			  // Shift E7
#ifdef LR_TRACE
			  TRACE0("prio pst >= prio bin donc Shift E7") ;
#endif
			  return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E7)  ;
			}
		}

	case FAMILY_BIN :
		{
		// Il faut comparer la priorite du lexeme courant "Bin" avec
		// le Bin dans la pile (pile de haut en bas : EXPR BIN)
		T_lexem *bin_in_stack = (T_lexem *)symbol_stack->get_depth(1) ;
		ASSERT(bin_in_stack->get_node_type() == NODE_LEXEM) ;
		ASSERT(get_lexem_family(bin_in_stack) == FAMILY_BIN) ;
		int cur_prio = get_lexem_prio(cur_lexem) ;
#ifdef LR_TRACE
		TRACE1("cur_prio = %d", cur_prio) ;
#endif
		int stack_prio = get_lexem_prio(bin_in_stack) ;
#ifdef LR_TRACE
		TRACE1("stack_prio = %d", stack_prio) ;
#endif

		if (stack_prio == cur_prio)
			{
			// Il faut regarder l'associativite de l'operateur binaire
			T_lexem_associativity asso = get_lexem_assoc(cur_lexem) ;
			switch(asso)
				{
				case ASSOS_LEFT :
					{
					// Reduce 1
#ifdef LR_TRACE
					TRACE0("operateur associatif a gauche -> reduce 1") ;
#endif
					return LR_reduce_1(betree, state_stack, symbol_stack) ;
					}
				case ASSOS_RIGHT :
					{
					// Shift E8
#ifdef LR_TRACE
					TRACE0("operateur associatif a droite -> shift E8") ;
#endif
					return LR_shift(cur_lexem,state_stack,symbol_stack,LR_STATE_E8);
					}
				default :
					{
					// On ne doit pas arriver ici, si les tables sont bien
					// formees ...
#ifdef LR_TRACE
					TRACE1("le lexeme %s n'a pas d'associativite",
						   cur_lexem->get_lex_name()) ;
#endif
					internal_error(cur_lexem,
										 __FILE__,
										 __LINE__,
										 get_error_msg(E_LEXEM_WITHOUT_ASSOC)) ;
					}
				}

			}
		else if (stack_prio > cur_prio)
			{
			// Reduce 1
#ifdef LR_TRACE
			TRACE0("Cas > : on fait reduce 1") ;
#endif
			return LR_reduce_1(betree, state_stack, symbol_stack) ;
			}
		else
			{
			// Shift E8
#ifdef LR_TRACE
			TRACE0("cas else : on fait shift E8") ;
#endif
			return LR_shift(cur_lexem, state_stack, symbol_stack, LR_STATE_E8)  ;
			}
		}

	case FAMILY_OPN :
		{
		T_lex_type lex_type = cur_lexem->get_lex_type() ;

		// Si on a '(' ou '[' on insere evl et on recommence
		if ( (lex_type == L_OPEN_PAREN) || (lex_type == L_OPEN_BRACKET) )
			{
#ifdef LR_TRACE
			TRACE0("on insere EVL et on recommence") ;
#endif
			(void)new T_lexem(cur_lexem, L_EVL) ;
			return LSTREAM_PREV ;
			}
		else
			{
			// Sinon Reduce 1
			open_stack->push(cur_lexem) ;
			return LR_reduce_1(betree, state_stack, symbol_stack) ;
			}
		}

	default :
	  {
	  // REDUCE 1
	  return LR_reduce_1(betree, state_stack, symbol_stack) ;
	  }
	}

// Pour les warnings
return LSTREAM_STAY ;
ASSERT(adr_stop) ;
}

//
//	}{	Tableau statique des actions
//
static T_LR_action LR_actions_sst[] =
	{
	/* LR_STATE_E0 */	(T_LR_action)LR_action_F0,		// F0 	= { E0, E1, E3 }
	/* LR_STATE_E1 */	(T_LR_action)LR_action_F0,		// F0 	= { E0, E1, E3 }
	/* LR_STATE_E2 */	(T_LR_action)LR_action_F_4,		// F_4 	= { E2 }
	/* LR_STATE_E3 */	(T_LR_action)LR_action_F0,		// F0 	= { E0, E1, E3 }
	/* LR_STATE_E4 */	(T_LR_action)LR_action_F2,		// F2 	= { E4 }
	/* LR_STATE_E5 */	(T_LR_action)LR_action_F3,		// F3 	= { E5 }
	/* LR_STATE_E6 */	(T_LR_action)LR_action_F4,		// F4 	= { E6 }
	/* LR_STATE_E7 */	(T_LR_action)LR_action_F_3,		// F_3 	= { E7 }
	/* LR_STATE_E8 */	(T_LR_action)LR_action_F1,		// F1 	= { E8 }
	/* LR_STATE_E9 */	(T_LR_action)LR_action_F_5,		// F_5	= { E9 }
	/* LR_STATE_E10 */	(T_LR_action)LR_action_F5		// F6 	= { E10 }
	} ;

// Fonction d'obtention des piles
// Allocation des piles d'etat et de symboles
static T_lr_stack<T_lr_state> *state_stack_si = NULL ;
static T_lr_stack<T_object *> *symbol_stack_si = NULL ;
// Allocation de la pile qui memorise les ouvrants
static T_lr_stack<T_lexem *> *open_stack_si = NULL ;

static void get_stacks(void)
{
  if (state_stack_si != NULL)
	{
	  state_stack_si->reset_stack() ;
	  symbol_stack_si->reset_stack() ;
	  open_stack_si->reset_stack() ;
	}
  else
	{
	  // Allocation des piles d'etat et de symboles
	  state_stack_si = new T_lr_stack<T_lr_state> ;
	  symbol_stack_si = new T_lr_stack<T_object *> ;
	  open_stack_si = new T_lr_stack<T_lexem *> ;
	}
}

void reset_LR_stacks(void)
{
  TRACE0("reset_LR_stacks") ;
  delete state_stack_si ;
  delete symbol_stack_si ;
  delete open_stack_si ;
  state_stack_si = NULL ;
  symbol_stack_si = NULL ;
  open_stack_si = NULL ;
}


  //
//	}{	Analyse LR
//
T_item *LR_analysis(T_item **adr_first,
							 T_item **adr_last,
							 T_betree *betree,
							 T_lexem **adr_cur_lexem)
{
#ifdef LR_TRACE
TRACE4("LR_analysis(%x,%x,%x,%x",
	   adr_first, adr_last, betree, adr_cur_lexem) ;
#endif

get_stacks() ;

T_lr_stack<T_lr_state> *state_stack = state_stack_si ;
T_lr_stack<T_object *> *symbol_stack = symbol_stack_si ;
T_lr_stack<T_lexem *> *open_stack = open_stack_si ;

// Initialisation de la pile d'etat
state_stack->push(LR_STATE_E0) ;

// Intialisation du lexeme courant
T_lexem *cur_lexem = *adr_cur_lexem ;

// Initialisation de la variable globale qui permet de savoir
// s'il faut relancer l'analyse LR, et de la variable globale
// qui permet de compter le nombre d'ouvrants

#ifdef LR_TRACE
// Compteur d'iterations
int iter = 0 ;
#endif

// Algorithme LR
for (;;)
	{
#ifdef LR_TRACE
	TRACE3("}{ ITERATION LR (%d) lexeme courant %x:%s",
		   ++iter,
		   cur_lexem,
		   cur_lexem->get_lex_name()) ;
#endif

	// Obtention de l'etat courant
	T_lr_state cur_state = state_stack->get_top() ;
#ifdef LR_TRACE
	TRACE1("etat courant : %d", cur_state) ;
#endif

	// Pour savoir s'il faut continuer
	int stop ;

	// Si la pile des ouvrants n'est pas vide alors :
	// Si le lexeme courant est un ';' il faut le transformer en '\;'
	// Si le lexeme courant est un '||' il faut le transformer en '\||'
	if (open_stack->is_empty() == FALSE)
		{
		if (cur_lexem->get_lex_type() == L_SCOL)
			{
			TRACE0("on patche ';' en '\\;'") ;
 			cur_lexem->set_lex_type(L_LR_SCOL) ;
			}
		else if (cur_lexem->get_lex_type() == L_PARALLEL)
			{
			TRACE0("on patche '||' en '\\||'") ;
 			cur_lexem->set_lex_type(L_LR_PARALLEL) ;
			}
		}

	// Pour savoir s'il faut avancer dans le flux de lexemes
	T_lexem_stream_action goto_next_lexem ;

	// On appelle le traitement associe
	stop = FALSE ;

#ifdef LR_TRACE
	TRACE1("cur_state = %d", cur_state) ;
#endif
	goto_next_lexem = (LR_actions_sst[cur_state])(betree,
												  cur_lexem,
												  state_stack,
												  symbol_stack,
												  open_stack,
												  &stop) ;

	if (stop == TRUE)
		{
#ifdef LR_TRACE
		TRACE0("Fin de l'analyse LR ...") ;
#endif
		*adr_cur_lexem = cur_lexem ;

		T_item *res = (T_item *)symbol_stack->get_top() ;

		// Phase de correction
#ifdef LR_TRACE
		TRACE1("debut phase de correction (res = %x)", res) ;
		ENTER_TRACE ;
#endif
		res->check_tree(NULL) ;
#ifdef LR_TRACE
		EXIT_TRACE ;
		TRACE1("fin   phase de correction (res = %x)", res) ;
#endif

		// On chaine en queue
		res->tail_insert(adr_first, adr_last) ;
#ifdef LR_TRACE
		TRACE1("analyse LR terminee : on rend le haut de la pile (res = %x)", res) ;
#endif

		// C'est tout
		return res ;
		}

	switch(goto_next_lexem)
		{
		case LSTREAM_NEXT :
			{
#ifdef LR_TRACE
			TRACE2("on passe au lexeme suivant %x:%s",
				   cur_lexem->get_next_lexem(),
				   (cur_lexem->get_next_lexem() == NULL) ? "(null)" :
				   cur_lexem->get_next_lexem()->get_lex_name()) ;
#endif
			cur_lexem = cur_lexem->get_next_lexem() ;
			break ;
			}

		case LSTREAM_PREV :
			{
#ifdef LR_TRACE
			TRACE0("on passe au lexeme precedent") ;
#endif
			cur_lexem = cur_lexem->get_prev_lexem() ;
			break ;
			}

		case LSTREAM_STAY :
			{
#ifdef LR_TRACE
			TRACE0("on reste sur le meme lexeme") ;
#endif
			break ;
			}
		}

	}
}

//
//	}{	Fin du fichier
//
