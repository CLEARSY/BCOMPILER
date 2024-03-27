/******************************* CLEARSY **************************************
* Fichier : $Id: c_expr.cpp,v 2.0 2008-01-18 13:49:24 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions
*
* Compilation :		-DDEBUG_EXPR pour avoir des traces
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
RCS_ID("$Id: c_expr.cpp,v 1.80 2008-01-18 13:49:24 arequet Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

/* Includes Composant Local */
#include "c_api.h"

int T_expr::warn_ident = TRUE;

//
//	Destructeur
//
T_expr::~T_expr(void)
{
  TRACE1("T_expr::~T_expr(%x)", this) ;
}

void T_expr::check_field_offsets()
{
    T_metaclass *meta = get_metaclass(NODE_EXPR);

    check_field_offset(meta, "B_type", offsetof(T_expr, B_type));
    check_field_offset(meta, "type_inferred", offsetof(T_expr, type_inferred));
}


void T_expr::set_warn_ident(int w)
{
    warn_ident = w;
}


int T_expr::get_warn_ident()
{
    return warn_ident;
}


int T_expr::is_an_expr(void)
{
  return TRUE ;
}

//
//	}{	Fonction qui fabrique un item a partir d'une item parenthesee
//
T_item *T_expr::new_paren_item(T_betree *betree,
										T_lexem *opn,
										T_lexem *clo)
{
  TRACE1("T_expr(%x)::new_paren_item", this) ;

  switch(opn->get_lex_type())
	{
	case L_OPEN_PAREN :
	  {
		// A priori on a (Expr) -> T_expr_with_parenthesis
		// Plusieurs cas particuliers pour lesquels il faut
		// construire un T_parenth_binary_operator
		// ( Expr ; Expr) 	-> BOP_COMPOSITION
		// ( Expr >< Expr)	-> BOP_DIRECT_PRODUCT
		// ( Expr || Expr)	-> BOP_PARALLEL_PRODUCT
		// Pour ces cas, on a expr de type NODE_BINARY_OP, avec oper
		// valant BOP_COMPOSE, BOP_DIRECT_PRODUCT ou BOP_PARALLEL
#ifdef DEBUG_EXPR
		TRACE0("on construit l'expr parenthesee") ;
#endif // DEBUG_EXPR
		return new T_expr_with_parenthesis(this, NULL, NULL, NULL, betree, clo) ;
	  }
	case L_OPEN_BRACKET :
	  {
		/*
		// A priori on est dans le cas Expr1 [ Expr2 ], construit comme ci-apres:
		//				EVL
		//             /   \
		//          Expr1  [ ]      <---- etat courant de l'analyse
		//                  |
		//				  Expr2     <---- variable locale expr
		// Il faut construire un T_image en retournant a EVL pour trouver Expr1
		*/
#ifdef DEBUG_EXPR
		TRACE0("on construit un squelette de T_image qu'on mettra a jour + tard");
#endif // DEBUG_EXPR
		return new T_image(this, NULL, NULL, NULL, betree, opn) ;
	  }

	case L_OPEN_BRACES :
	  {
		// { xxx }
		// Plusieurs cas possibles :
		// 1) Expr_tableau :              '{' terme '|->' terme '|->'terme +, '}'
		//                             ou Plage+* '*' '{' Terme '}' +\/
		// 2) Plage        			:    '{' terme +, '}'
		// 3) Typage appartenance   :    '{' terme +, '}'
		//
		// Pour l'instant on ne gere que le deuxieme cas du 1) -> on rend le terme
		// Les autres cas sont determines au cours de l'analyse semantique
#ifdef DEBUG_EXPR
		TRACE1("type expr = %s", get_class_name()) ;
#endif // DEBUG_EXPR

		return new T_extensive_set(this, NULL, NULL, NULL, betree, opn) ;
		assert(FALSE) ;
	  }

	default :
	  {
		// Impossible car on est sur un opn
		assert(FALSE) ;
	  }
	}

  TRACE0("-> return NULL") ;
  return NULL ;
}

//
//	}{	Fonction qui fabrique un atome
//
T_item *new_atom(T_betree *betree, T_lexem *cur_lexem)
{
#ifdef DEBUG_EXPR
  TRACE2("new_atom(%x, %s)", betree,
		 (cur_lexem == NULL) ? "NULL" : cur_lexem->get_lex_name()) ;
#endif
  ASSERT(cur_lexem) ;

  T_lex_type lex_type = cur_lexem->get_lex_type() ;

  switch (lex_type)
	{
	case L_NUMBER :
	  {
		return new T_literal_integer(NULL, NULL, NULL, betree, cur_lexem) ;
	  }

        case L_REAL :
          {
                return new T_literal_real(NULL, NULL, NULL, betree, cur_lexem) ;
          }

        case L_IDENT :
	  {
		return new T_ident(cur_lexem->get_value(),
						   ITYPE_UNKNOWN,
						   NULL,
						   NULL,
						   NULL,
						   betree,
						   cur_lexem) ;
	  }

	case L_RENAMED_IDENT :
	  {
		return new T_renamed_ident(cur_lexem->get_value(),
								   ITYPE_UNKNOWN,
								   NULL,
								   NULL,
								   NULL,
								   betree,
								   cur_lexem) ;
	  }

	case L_STRING :
	  {
		return new T_literal_string(NULL, NULL, NULL, betree, cur_lexem) ;
	  }

	case L_TRUE :
	case L_FALSE :
	  {
		return new T_literal_boolean(NULL, NULL, NULL, betree, cur_lexem) ;
	  }

	case L_SUCC : 			{ return link_builtin_succ(cur_lexem, betree) ; }
	case L_PRED :    		{ return link_builtin_pred(cur_lexem, betree) ; }
	case L_NAT : 			{ return link_builtin_nat(cur_lexem, betree) ; }
	case L_NAT1 :    		{ return link_builtin_nat1(cur_lexem, betree) ; }
	case L_INT : 			{ return link_builtin_int(cur_lexem, betree) ; }
	case L_MAXINT : 		{ return link_builtin_maxint(cur_lexem, betree) ; }
	case L_MININT : 		{ return link_builtin_minint(cur_lexem, betree) ; }
	case L_INTEGER : 		{ return link_builtin_integer(cur_lexem, betree) ; }
	case L_NATURAL : 		{ return link_builtin_natural(cur_lexem, betree) ; }
	case L_NATURAL1 : 		{ return link_builtin_natural1(cur_lexem, betree) ; }
	case L_BOOL_TYPE : 		{ return link_builtin_bool_type(cur_lexem, betree) ; }
	case L_STRING_TYPE :	{ return link_builtin_string_type(cur_lexem, betree) ; }
	case L_REAL_TYPE :	{ return link_builtin_real_type(cur_lexem, betree) ; }
	case L_FLOAT_TYPE :	{ return link_builtin_float_type(cur_lexem, betree) ; }


	case L_EMPTY_SET :
	  { return new T_empty_set(NULL, NULL, NULL, betree, cur_lexem) ; }
	case L_EMPTY_SEQ :
	  { return new T_empty_seq(NULL, NULL, NULL, betree, cur_lexem) ; }

	default :
	  {
		// On ne doit pas passer ici, mais on prevoit une sortie
		// propre pour le debug
		TRACE1("type %d pas implemente", lex_type) ;
		internal_error(cur_lexem,
					   __FILE__,
					   __LINE__,
					   get_error_msg(E_ATOM_TYPE_NOT_YET_IMPLEMENTED),
					   cur_lexem->get_lex_name(),
					   cur_lexem->get_lex_ascii()) ;
		assert(FALSE) ;
	  }
	}

  return NULL ;
}

//
//	}{	Fonction qui fabrique un pst
//
T_expr *new_pst(T_expr *operand,
						 T_lexem *pst,
						 T_betree *betree,
						 T_lexem *cur_lexem)
{
  TRACE4("new_pst(%x, %x, %x, %x, %x)", operand,  pst, betree, cur_lexem) ;
  ASSERT(cur_lexem) ;

  T_lex_type lex_type = pst->get_lex_type() ;

  if (lex_type == L_CONVERSE)
	{
	  return new T_converse(operand,
							NULL,
							NULL,
							NULL,
							betree,
							cur_lexem) ;
	}

  // Par construction de l'analyseur LR on ne doit pas passer ici
  assert(FALSE) ;
  return NULL ;
}

//
//	}{	Constructeur de la classe T_converse
//
T_converse::T_converse(T_expr *new_expr,
								T_item **adr_first,
								T_item **adr_last,
								T_item *father,
								T_betree *betree,
								T_lexem *ref_lexem)
  :T_expr(NODE_CONVERSE, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_converse(%x)::T_converse(%x,%x,%d,%x,%x,%x,%x,%x)",
		 this, expr, adr_first, adr_last, father, betree, ref_lexem) ;

  expr = new_expr ;

  // On met a jour les liens "father"
  expr->set_father(this) ;

  TRACE0("fin de T_converse::T_converse") ;

}

//
//	}{	Correction de l'arbre
//
T_item *T_converse::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_converse(%x)::check_tree", this) ;
#endif

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}
  return this ;
  assert(ref_this) ; // pour le warning
}

//
//	Destructeur
//
T_converse::~T_converse(void)
{
  TRACE1("T_converse::~T_converse(%x)", this) ;
}

//
//	}{	Constructeur de la classe T_op_result
//
T_op_result::T_op_result(T_item *new_function,
								  T_item *params,
								  T_ident *ref_builtin_op,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *father,
								  T_betree *betree,
								  T_lexem *ref_lexem)
  :T_expr(NODE_OP_RESULT, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef TRACE
  TRACE8("T_op_result(%x)::T_op_result(%x,%x,%x,%d,%x,%x,%x)",
		 this,
		 new_function, params, adr_first, adr_last, father, betree, ref_lexem) ;
  TRACE1("ref_builtin_op = %x", ref_builtin_op) ;
#endif

  ref_array_item = NULL ;
  ref_builtin = NULL ;

  if (ref_builtin_op == NULL)
	{
	  // Nom de fonction
	  // > Attention au cas particulier suivant :
	  // > Cas d'un builtin :: new_function est un T_list_link de type LINK_BUILTIN
	  // > Il faut alors suivre l'indirection

	  // Cast valide par construction des builtin
	  new_function = (T_item *)new_function->follow_indirection(LINK_BUILTIN) ;

	  T_node_type ftype = new_function->get_node_type() ;

	  TRACE3("new_function : %x (%d : %s)",
			 new_function, ftype, new_function->get_class_name()) ;

	  switch(ftype)
		{
		case NODE_IDENT :
		case NODE_RENAMED_IDENT :
		  {
			// Appel du type 'f(x)'
#ifdef DEBUG_EXPR
			TRACE0("appel de fonction simple") ;
#endif
			op_name = new_function ;

			break ;
		  }

		default :
		  {
			// Appel de type <item>(x) : new_function est l'expression
#ifdef DEBUG_EXPR
			TRACE0("appel de fonction donnee par un item") ;
#endif
			op_name = new_function ;
		  }
		}

	  op_name->link() ;

	  ref_builtin = NULL ;

	  // 2 cas particuliers a rattraper ici : succ et pred, qui sont des atom
	  // et pas des unr
	  if (op_name->is_an_ident() == TRUE)
		{
		  T_symbol *name = (op_name->make_ident())->get_name() ;

		  if (name->compare(get_builtin_pred()->get_name()) == TRUE)
			{
			  ref_builtin = (T_keyword *) get_builtin_pred()->get_name() ;
			}
		  else if (name->compare(get_builtin_succ()->get_name()) == TRUE)
			{
			  ref_builtin = (T_keyword *) get_builtin_succ()->get_name() ;
			}
		}

	}
  else
	{
	  // Operation predefinie
	  op_name = ref_builtin_op ;
	  op_name->link() ; // car libere dans le destructeur
	  ref_builtin = (T_keyword *) ref_builtin_op->get_name() ;
	}

  ASSERT(ref_builtin == NULL || ref_builtin->get_node_type() == NODE_KEYWORD);

  // Parametres : on les expanse dans first_in_param, last_in_param
  first_in_param = last_in_param = NULL ;

  // Le cast suivant est valide par construction du parseur LR
  // On lance l'extraction des parametres
  if (params->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  // Appel ; fonction ( parametres )
  	  T_expr *root = ((T_expr_with_parenthesis *)params)->get_expr() ;

	  // On extrait les parametres
  	  root->extract_params((int)BOP_COMMA,
						   this,
						   (T_item **)&first_in_param,
						   (T_item **)&last_in_param) ;

	  // On les libere
	  root->link() ;
	  params->unlink() ;
  	}
  else if (params->get_node_type() == NODE_PREDICATE_WITH_PARENTHESIS)
	{
	  // Cas :: bool(predicat)
	  // Appel ; fonction ( parametres )
	  T_predicate *root = ((T_predicate_with_parenthesis *)params)->get_predicate();

	  // On extrait les parametres
	  root->extract_params((int)BOP_COMMA,
						   this,
						   (T_item **)&first_in_param,
						   (T_item **)&last_in_param) ;

	  // On libere
	  root->link() ;
	  params->unlink() ;


	}
  else
	{
	  // Appel : fonction parametres
	  // Valide ssi la fonction est un Unr (ex : card) et pas un atom (ex : succ)
	  // Dans ce cas, il ne peut y avoir qu'un seul parametre
	  //
	  //	Suite reunion 14/11/96 avec JRA, cette syntaxe est interdite
	  //	On signale un message d'erreur non bloquante
	  //
	  TRACE1("Erreur : la fonction %s est appelle sans parentheses",
			 ptr_ref(op_name)) ;
	  TRACE3("params %x %d %s",
			 params,
			 params->get_node_type(),
			 params->get_class_name()) ;
	  syntax_error(get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_PARENTH_COMPULSORY_IN_FUNCTION_CALL)) ;
	  first_in_param = last_in_param = params ;
	  first_in_param->link() ;
	}

  // Intialisation des autres champs
  ref_op = NULL ;

  // Calcul du nombre de parametres
  nb_in_params = 0 ;
  T_item *cur = first_in_param ;
  while (cur != NULL)
	{
	  nb_in_params ++ ;
	  cur = cur->get_next() ;
	}

  // On met a jour les liens "father"
#ifdef DEBUG_EXPR
  TRACE2("op_result(%x)->op_name = %x", this, op_name) ;
#endif
  op_name->set_father(this) ;

  // On se chaine dans l'object manager
  get_object_manager()->add_op_result(this) ;

#ifdef DEBUG_EXPR
  TRACE0("fin de T_op_result::T_op_result") ;
#endif

}

//
//	Destructeur
//
T_op_result::~T_op_result(void)
{
  TRACE1("T_op_result::~T_op_result(%x)", this) ;

  get_object_manager()->remove_op_result(this);
}

//
//	}{	Phase de correction
// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_op_result::check_tree(T_item **ref_this)
{
  TRACE1("T_op_result(%x::%x)::check_tree", this) ;

  T_item *res_object = NULL ;

  T_lexem *lexem = get_ref_lexem() ;
  ASSERT(lexem != NULL) ;
  T_lex_type lex_type = lexem->get_lex_type() ;
  if (lex_type == L_GUNION)
	{
	  TRACE0("C'est union :: il faut faire un T_generalised_union") ;
	  res_object = new T_generalised_union(this,
										   NULL,
										   NULL,
										   get_father(),
										   get_betree(),
										   get_ref_lexem()) ;
	}
  else if (lex_type == L_GINTER)
	{
	  TRACE0("C'est inter::il faut faire un T_generalised_intersection") ;
	  res_object = new T_generalised_intersection(this,
												  NULL,
												  NULL,
												  get_father(),
												  get_betree(),
												  get_ref_lexem()) ;
	}

  // On appelle check_tree sur op_name
  op_name = op_name->check_tree(NULL);

  // Parametres d'appel
  TRACE1("correction des parametres d'appel first_in_param=%x", first_in_param) ;
  T_item *cur_param = first_in_param ;
  // Chainage temporaire
  T_item *tmp_first = NULL ;
  T_item *tmp_last = NULL ;
  while (cur_param != NULL)
	{
		//	  TRACE2("cur_param = %x %s", cur_param, cur_param->get_class_name()) ;
	  T_item *next = cur_param->get_next() ;
	  T_item *res_param = cur_param->check_tree(NULL) ;
	  res_param->tail_insert(&tmp_first, &tmp_last) ;
	  cur_param = next ;
	}

  // Mise a jour du nouveau chainage
  first_in_param = tmp_first ;
  last_in_param = tmp_last ;

  if (res_object == NULL)
	{
	  TRACE0("fin de T_op_result::check_tree() : on n'a pas change l'operation") ;
	  return this ;
	}

  if (ref_this != NULL)
	{
	  TRACE2("mise a jour de ref_this(%x) avec %x", ref_this, res_object) ;
	  *ref_this = res_object ;
	}

  TRACE1("fin de T_op_result::check_tree(): on a cree un %s",
		 res_object->get_class_name()) ;
  return res_object ;
}

// Fonction de verfication (est-ce un T_op_result ou un T_array_item ?)
// Corrige l'arbre le cas echeant
void T_op_result::check_is_an_op_result(void)
{
  TRACE2("T_op_result(%x)::check_is_an_op_result op_name %x", this, op_name) ;

  // Un T_op_result ne peut etre valide que si ref_builtin != NULL
  // Sinon, c'est en fait un acces a un tableau, donc un T_array_item
  // Il faut donc verifier dans ce cas que op_name ne pointe pas
  // vers un nom d'operation (ITYPE_OP_NAME) car la syntaxe
  // x := f(y) ou f est une operation est illegale
  if (ref_builtin != NULL)
	{
	  TRACE0("Ok, c'est un builtin") ;
	  return ;
	}

  // Autre cas particulier : appel "tel quel" dans une substituion

  if (op_name->is_an_ident() == TRUE)
	{
	  // Appel de l'operation :   [out_params] <-- op_name
	  // <-- : lexeme avant le lexeme de op_name
	  if (op_name->get_ref_lexem() == NULL)
		{
		  // C'est un builtin, donc c'est bon (c'est le seul cas tolere)
		  TRACE2("ici op_name(%x)->get_ref_lexem %x, mais c'est OK car builtin",
				 op_name,
				 op_name->get_ref_lexem()) ;
		  return ;
		}
	  T_lexem *ref_lexem = op_name->get_ref_lexem()->get_prev_lexem() ;

	  if (ref_lexem->get_lex_type() == L_AFFECT)
		{
		  // Syntaxe illegale pour un appel d'operation !
		  T_ident *name = (T_ident *)op_name ; // A FAIRE : justifier le cast ...
		  // A FAIRE : pourquoi a-t-on utilis� un T_item ?
		  while (name->get_def() != NULL)
			{
			  TRACE1("name = %x", name) ;
			  TRACE1("name->get_class_name() = %s", name->get_class_name()) ;
			  TRACE1("name->get_def = %x", name->get_def()) ;
			  name = name->get_def() ;
			}

		  if (name->get_ident_type() == ITYPE_OP_NAME)
			{
			  // Syntaxe illegale pour un appel d'operation !
			  TRACE1("erreur : operation %s appelee avec :=",
					 name->get_name()->get_value()) ;
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_INVALID_OP_CALL),
							 name->get_name()->get_value()) ;
			  semantic_error_details(name,
									 get_error_msg(E_LOCATION_OF_ITEM_DEF),
									 name->get_name()->get_value()) ;
			}
		}
	}
  else
	{
	  // A FAIRE !!
#if 0
	  fprintf(stdout,
			  "%s:%d T_op_call : verif supplementaires a faire\n",
			  __FILE__,
			  __LINE__) ;
#endif
	  TRACE2("panic op_name %x %s", op_name, op_name->get_class_name()) ;
	}

  // Ici on peut fabriquer un T_array_item
  TRACE0("ON DOIT FABRIQUER UN T_array_item") ;

  T_array_item *array_item = new T_array_item(this,
											  NULL,
											  NULL,
											  get_father(),
											  get_betree(),
											  get_ref_lexem()) ;

  TRACE0("ON CHANGE DANS LA LISTE") ;

  // Copie du chainage avec le pr�d�cesseur et le successeur.
  array_item->set_prev(get_prev()) ;
  if (get_prev() != NULL)
	{
	  get_prev()->set_next(array_item) ;
	}
  array_item->set_next(get_next()) ;
  if (get_next() != NULL)
	{
	  get_next()->set_prev(array_item) ;
	}

  // Si on est reference par le pere, il faut changer la valeur
  if (get_father() != NULL)
	{
	  T_field *cur_field ;
  TRACE1("get_father %x", get_father()) ;
  TRACE1("get_father->get_node_type %d", get_father()->get_node_type()) ;
	  cur_field = get_metaclass(get_father()->get_node_type())->get_fields() ;
	  size_t base_adr = (size_t)get_father() ;
	  TRACE2("ici pere %x %s", get_father(), get_father()->get_class_name()) ;
	  while (cur_field != NULL)
		{
		  if (cur_field->get_field_type() == FIELD_OBJECT)
			   //			   && (cur_field->get_ref_type() != REF_PRIVATE_LIST) )
			{
			  size_t adr = base_adr + cur_field->get_field_offset() ;
			  if (*((T_item **)adr) == this)
				{
				  TRACE2("On change la valeur du champ %s (offset %d)",
						 cur_field->get_field_name(),
						 cur_field->get_field_offset()) ;
				  *((T_item **)adr) = array_item ;
				}
			}
		  cur_field = cur_field->get_next() ;
		}
	}

  // Mise a jour de ref_array_item
  ref_array_item = array_item ;

  TRACE0("fin de la transformation T_op_result -> T_array_item") ;
}

//
//	}{	Classe T_empty_seq
//
T_empty_seq::T_empty_seq(T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *new_ref_lexem)
  : T_expr(NODE_EMPTY_SEQ,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
  TRACE0("T_empty_seq::T_empty_seq") ;

}

T_empty_seq::~T_empty_seq(void)
{
  TRACE1("T_empty_seq::~T_empty_seq(%x)", this) ;
}

//
//	}{	Classe T_empty_set
//
T_empty_set::T_empty_set(T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *new_ref_lexem)
  : T_expr(NODE_EMPTY_SET,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
  TRACE0("T_empty_set::T_empty_set") ;

}

T_empty_set::~T_empty_set(void)
{
  TRACE1("T_empty_set::~T_empty_set(%x)", this) ;
}

// Est-ce une relation ? oui !
int T_empty_set::is_a_relation(void)
{
  TRACE1("T_empty_set(%x)::is_a_relation", this) ;

  // L'ensemble vide est une relation ! Et si son type est encore
  // Setof(?), il faut le "remplacer" par Setof(?*?)

  T_type *type = get_B_type() ;
  ASSERT(type != NULL) ; // Par construction

  if (get_B_type()->get_node_type() == NODE_SETOF_TYPE)
	{
	  T_type *base_type = ((T_setof_type *)get_B_type())->get_base_type() ;

	  if (base_type->get_node_type() == NODE_GENERIC_TYPE)
		{
		  // Il faut transformer Setof(?) en Setof(?*?)
		  T_type *joker1 = new T_generic_type(this, get_betree(), get_ref_lexem()) ;
		  T_type *joker2 = new T_generic_type(this, get_betree(), get_ref_lexem()) ;

		  T_type *product = new T_product_type(joker1,
											   joker2,
											   this,
											   get_betree(),
											   get_ref_lexem()) ;

		  T_setof_type *type = new T_setof_type(product,
												this,
												get_betree(),
												get_ref_lexem()) ;

		  reset_B_type() ; // pour debrancher les verifications de compatibilite
		  set_B_type(type, get_ref_lexem()) ;
		}
	}

  return TRUE ;
}


// Affectation du type B

void T_expr::set_B_type(T_type *new_B_type, T_lexem *)
{
  if (new_B_type == NULL)
	{
	  return ;
	}
  /*
  fprintf(stderr,
          "T_expr::set_B_type\n"
          "\tthis: %p\n"
          "\tB_type: %p\n"
          "\tnew_B_type: %p\n"
          , this, B_type, new_B_type
          );
  this->dump_html();
  if(B_type != NULL) B_type->dump_html();
  new_B_type->dump_html();
  */
#ifdef DUMP_TYPES
    TRACE3("T_expr(%x:%s)::set_B_type(%x)", this, get_class_name(), new_B_type) ;
#endif


  new_B_type->set_father(this) ;

  // Si l'expression etait deja typee, alors on verifie
  // la compatibilite du nouveau type
  if ( (is_an_ident() == FALSE) && (B_type != NULL) )
	{
	  TRACE4("TEST COMPATIBILITE de B_type=%s(%s) et new_B_type=%s(%s)",
			 make_type_name(B_type)->get_value(),
			 (B_type == NULL) ? "null" : B_type->get_class_name(),
			 make_type_name(new_B_type)->get_value(),
			 (new_B_type == NULL) ? "null" : new_B_type->get_class_name()) ;
	  if (B_type->is_compatible_with(new_B_type) == FALSE)
		{
		  TRACE0("types incompatibles -> on ne type pas") ;
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
						 B_type->make_type_name()->get_value(),
						 new_B_type->make_type_name()->get_value()) ;
		  return ;
		}

	  // On libere le type precedent
	  B_type->unlink() ;
	}

  // B_type = new_B_type;
  // new_B_type->link();
  set_B_type_rec(new_B_type);
  // Attention ! le type string n'est autorise que pour typer les parametres
  // d'entree des operations
  // On ne le verifie pas maintenant car on a besoin des father pour
  // le faire, et ils ne sont pas tous positionnes dans le cas
  // des Literal_String. On fait donc ce travail en fin de Type Check
}

// Reset du type B
void T_expr::reset_B_type(void)
{
  TRACE1("T_expr(%x)::reset_B_type()", this) ;
  B_type = NULL ;
}

T_type *T_expr::get_B_type(void)
{
  // Attention, dans le cas d'un ensemble abstrait deja value, il faut
  // rendre le type apres valuation
  if ( (B_type != NULL) && (B_type->get_node_type() == NODE_ABSTRACT_TYPE))
	{
	  T_abstract_type *atype = (T_abstract_type *)B_type ;
	  if (atype->get_after_valuation_type() != NULL)
		{
#ifdef DUMP_TYPES
  TRACE6("T_expr(%x:%s)::get_B_type()->%x,%s value avec %x,%s <-- ce qu'on renvoie",
		 this,
		 get_class_name(),
		 B_type,
		 B_type->get_class_name(),
		 atype->get_after_valuation_type(),
		 atype->get_after_valuation_type()->make_type_name()->get_value()) ;
  TRACE2("atype = %x -> get_after_valuation_type() %x",
		 atype,
		 atype->get_after_valuation_type()) ;
#endif

		  return atype->get_after_valuation_type() ;
		}
	}

#ifdef DUMP_TYPES
  TRACE3("T_expr(%x:%s)::get_B_type()->%x",
		 this,
		 get_class_name(),
		 B_type) ;
  TRACE1("B_type = %s",
		 (B_type == NULL) ? "NULL" : B_type->make_type_name()->get_value()) ;
#endif

  return B_type ;
}

T_type *T_expr::get_original_B_type(void)
{
#ifdef DUMP_TYPES
  TRACE3("T_expr(%x:%s)::get_original_B_type -> %x",
		 this,
		 get_class_name(),
		 B_type) ;
#endif // DUMP_TYPES
  return B_type ;
}

// Est-ce un ensemble ?
int T_expr::is_a_set(void)
{
  TRACE3("T_expr(%x::%s)::is_a_set -> %s",
		 this,
		 get_class_name(),
		 (get_B_type() == NULL) ? "PANIC, TYPE = NULL" :
	     (get_B_type()->get_node_type() == NODE_SETOF_TYPE) ? "TRUE" : "FALSE") ;

  if (get_B_type() == NULL)
	{
	  // C'est possible en cas d'erreur. On prend un comportement
	  // timide, on repond NON
	  return FALSE ;
	}

  return get_B_type()->is_a_set() ;
}

// Est-ce un ensemble ?
int T_expr::is_an_integer_set(void)
{
  TRACE2("T_expr(%x::%s)::is_an_integer_set", this, get_class_name()) ;


  if (get_B_type() == NULL)
	{
	  // C'est possible en cas d'erreur. On prend un comportement
	  // timide, on repond NON
	  TRACE0("type = NULL ->FALSE") ;
	  return FALSE ;
	}

  if (get_B_type()->get_node_type() != NODE_SETOF_TYPE)
	{
	  TRACE1("type %s ->FALSE", get_B_type()->get_class_name()) ;
	  return FALSE ;
	}

  T_type *base = ((T_setof_type *)get_B_type())->get_base_type() ;
  TRACE1("base = %x", base) ;
  TRACE2("base->get_node_type = %d (PREDEFINED_TYPE : %s)",
		 base->get_node_type(),
		 (base->get_node_type() == NODE_PREDEFINED_TYPE) ? "TRUE" : "FALSE") ;
  TRACE2("base->get_node_type->type = %d (BTYPE_INTEGER : %s)",
		 ((T_predefined_type *)base)->get_type(),
		 (((T_predefined_type *)base)->get_type() == BTYPE_INTEGER)
		 ? "TRUE" : "FALSE") ;

  if ( 	(base != NULL)
		&& (base->get_node_type() == NODE_PREDEFINED_TYPE)
                && ((((T_predefined_type *)base)->get_type() == BTYPE_INTEGER)))
	{
	  TRACE0("->TRUE") ;
	  return TRUE ;
	}

  TRACE0("->FALSE") ;
  return FALSE ;
}

// Est-ce un ensemble ?
int T_expr::is_a_real_set(void)
{
  TRACE2("T_expr(%x::%s)::is_a_real_set", this, get_class_name()) ;

  if (get_B_type() == NULL)
        {
          // C'est possible en cas d'erreur. On prend un comportement
          // timide, on repond NON
          TRACE0("type = NULL ->FALSE") ;
          return FALSE ;
        }

  if (get_B_type()->get_node_type() != NODE_SETOF_TYPE)
        {
          TRACE1("type %s ->FALSE", get_B_type()->get_class_name()) ;
          return FALSE ;
        }

  T_type *base = ((T_setof_type *)get_B_type())->get_base_type() ;
  TRACE1("base = %x", base) ;
  TRACE2("base->get_node_type = %d (PREDEFINED_TYPE : %s)",
                 base->get_node_type(),
                 (base->get_node_type() == NODE_PREDEFINED_TYPE) ? "TRUE" : "FALSE") ;
  TRACE2("base->get_node_type->type = %d (BTYPE_REAL : %s)",
                 ((T_predefined_type *)base)->get_type(),
                 (((T_predefined_type *)base)->get_type() == BTYPE_REAL)
                 ? "TRUE" : "FALSE") ;

  if ( 	(base != NULL)
                && (base->get_node_type() == NODE_PREDEFINED_TYPE)
                && (((T_predefined_type *)base)->get_type() == BTYPE_REAL))
        {
          TRACE0("->TRUE") ;
          return TRUE ;
        }

  TRACE0("->FALSE") ;
  return FALSE ;
}

// Est-ce un ensemble ?
int T_expr::is_a_seq(void)
{
  TRACE3("T_expr(%x::%s)::is_a_seq(B_type = %s)",
		 this,
		 get_class_name(),
		 (get_B_type() == NULL)
		 ? "(null)"
		 : get_B_type()->make_type_name()->get_value()) ;

  if (get_B_type() == NULL)
	{
	  // C'est possible en cas d'erreur. On prend un comportement
	  // timide, on repond NON
	  return FALSE ;
	}

  return (get_B_type()->is_a_seq()) ;
}

// Est-ce un ensemble ?
int T_expr::is_a_non_empty_seq(void)
{
  TRACE3("T_expr(%x::%s)::is_a_non_empty_seq(B_type = %s)",
		 this,
		 get_class_name(),
		 (get_B_type() == NULL)
		 ? "(null)" : get_B_type()->make_type_name()->get_value()) ;

  // On verifie que l'on n'est pas [] ...
  if ( (get_ref_lexem() != NULL)
	   && (get_ref_lexem()->get_lex_type() == L_EMPTY_SEQ) )
	{
	  TRACE0("sequence vide -> FALSE") ;
	  return FALSE ;
	}

  // ... et on peut appeler l'algo general
  return is_a_seq() ;

}

// Est-ce un ensemble ?
int T_expr::is_a_relation(void)
{
#ifdef TRACE
  if (get_ref_lexem() == NULL)
	{
	  TRACE2("T_expr(%x::%s, ref_lexem == NULL)::is_a_relation",
			 this,
			 get_class_name()) ;
	}
  else
	{
	  TRACE5("T_expr(%x::%s, %s:%d:%d)::is_a_relation",
			 this,
			 get_class_name(),
			 get_ref_lexem()->get_file_name()->get_value(),
			 get_ref_lexem()->get_file_line(),
			 get_ref_lexem()->get_file_column()) ;
	}
#endif // TRACE


  if (get_B_type() == NULL)
	{
	  // C'est possible en cas d'erreur. On prend un comportement
	  // timide, on repond NON
	  TRACE0("type nul -> pas relation") ;
	  return FALSE ;
	}

  TRACE2("type %x:%s", get_B_type(), get_B_type()->make_type_name()->get_value()) ;

  // Une relation est de type : Setof(A * B)
  if (get_B_type()->get_node_type() != NODE_SETOF_TYPE)
	{
	  TRACE0("pas setof -> pas relation") ;
	  return FALSE ;
	}

  T_setof_type *setof = (T_setof_type *)get_B_type() ;
  T_type *base = setof->get_base_type() ;

  if (base->get_node_type() != NODE_PRODUCT_TYPE)
	{
	  TRACE0("pas setof(prod) -> pas relation") ;
	  return FALSE ;
	}

  TRACE0("ok c'est une relation") ;
  return TRUE ;
}

// Fonction de typage et de verification de type
// Pour une expression, type_check = make_type (cf ci-dessous)
void T_expr::type_check(void)
{
  TRACE2("T_expr(%x:%s)::type_check() -> on fait make_type",
		 this, get_class_name()) ;

  make_type() ;
}

// Est-ce une constante/variable ?
// Par defaut, NON
// OUI ssi classe T_ident avec le bon ident_type
int T_expr::is_a_constant(int ask_def)
{
  return FALSE ;
  ask_def = ask_def ;
}
int T_expr::is_a_variable(int ask_def)
{
  return FALSE ;
  ask_def = ask_def ;
}

//
//	}{	Classe T_array_item
//
//	Constructeur a partir d'un T_op_result
T_array_item::T_array_item(T_op_result *ref_op_result,
									T_item **adr_first,
									T_item **adr_last,
									T_item *father,
									T_betree *betree,
									T_lexem *ref_lexem)
  :T_expr(NODE_ARRAY_ITEM, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE6("T_array_item::T_array_item(%x,%x,%d,%x,%x,%x,%x,%x)",
		 ref_op_result, adr_first, adr_last, father, betree, ref_lexem) ;

  array_name = ref_op_result->op_name ;
  array_name->set_father(this) ;
  array_name->link() ;

  separator = BOP_COMMA;

  // Si les indexes sont separes par des maplets et pas des virgules,
  // on ne le voit pas ! Donc on transforme les maplets en virgules
  first_index = NULL ;
  last_index = NULL ;
  T_item *cur_idx = ref_op_result->first_in_param ;
  while (cur_idx != NULL)
	{
      if (cur_idx->get_node_type() == NODE_BINARY_OP)
        separator = ((T_binary_op *) cur_idx)->get_operator();
	  T_item *next = cur_idx->get_next() ;

	  // Si l'index est une expression, il faut sauter les parentheses
	  // pour detecter les maplets
	  T_item *real_idx = (cur_idx->is_an_expr() == TRUE) ?
		cur_idx->make_expr()->strip_parenthesis() : cur_idx ;

	  real_idx->extract_params((int)BOP_MAPLET,
							   this,
							   (T_item **)&first_index,
							   (T_item **)&last_index) ;

	  if (last_index == real_idx)
		{
		  // extract_params a rendu real_idx : il faut poser un lien
		  // dessus pour etre coherent avec le cas ou extract_params
		  // sort les parametres des maplets, et donc dans ce cas
		  // pose des liens sur les indexes
		  real_idx->link() ;
		}

	  cur_idx = next ;
	}

  // Correction des champs father des indexes
  T_item *cur = first_index ;
  while (cur != NULL)
	{
	  cur->set_father(this) ;
	  cur = cur->get_next() ;
	}
  // On n'a pas encore fait les types donc on ne peut pas encore faire
  // compute_nb_indexes

  TRACE2("fin de T_array_item(%x)::T_array_item -> nb_types = %d",
		 this,
		 nb_indexes) ;
}

// Destructeur
T_array_item::~T_array_item(void)
{
}


// Calcul du nombre d'indexes
void T_array_item::compute_nb_indexes(void)
{
  TRACE1("T_array_item(%x)::compute_nb_indexes()", this) ;

  // On compte le nombre d'indexes
  nb_indexes = 0 ;

  T_item *cur = first_index ;

  while (cur != NULL)
	{
	  TRACE2("cur=%x:%s", cur, cur->get_class_name()) ;
	  T_type *cur_type = cur->get_B_type() ;

	  if (cur_type == NULL)
		{
		  TRACE5("!! %x %s pas type (%s:%d:%d)",
				 cur,
				 cur->get_class_name(),
				 cur->get_ref_lexem()->get_file_name()->get_value(),
				 cur->get_ref_lexem()->get_file_line(),
				 cur->get_ref_lexem()->get_file_column()) ;
		  assert(FALSE) ;
		}

	  if (cur_type->is_a_set() == TRUE)
		{
		  // Cast justifie par le test ci-dessus
		  T_type *base = ((T_setof_type *)cur_type)->get_base_type() ;

		  if (base->get_node_type() == NODE_PRODUCT_TYPE)
			{
			  T_product_type *prod = (T_product_type *)base ;
			  nb_indexes += prod->get_nb_types() ;
			  TRACE3("prod cartesien %x:%s -> nb_indexes += %x",
					 prod,
					 prod->make_type_name()->get_value(),
					 prod->get_nb_types()) ;
			}
		  else
			{
			  nb_indexes ++ ;
			  TRACE0("nb_indexes ++") ;
			}
		}
	  else
		{
		  TRACE0("nb_indexes ++") ;
		  nb_indexes ++ ;
		}

	  cur = cur->get_next() ;
	}

  TRACE2("T_array_item(%x)::compute_nb_indexes() -> %d", this, nb_indexes) ;
}

//
//	}{	Correction de l'arbre
//
T_item *T_array_item::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_array_item(%x)::check_tree", this) ;
#endif

#ifdef DEBUG_CHECK
  TRACE1("-> check nom %x", array_name) ;
  ENTER_TRACE ;
#endif

  if (array_name == NULL)
	{
	  return NULL ;
	}

  array_name->check_tree((T_item **)&array_name) ;

#ifdef DEBUG_CHECK
  EXIT_TRACE ;
  TRACE2("-> check indexes de %x a %x", first_index, last_index) ;
  ENTER_TRACE ;
#endif

  T_expr *cur_item = first_index ;
  T_item *tmp_first = NULL ;
  T_item *tmp_last = NULL ;

  while (cur_item != NULL)
	{
#ifdef DEBUG_CHECK
	  TRACE1("-> check cur_index de %x", cur_item) ;
#endif
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
	  cur_item = next ;
	}

  if (tmp_first == NULL)
	{
	  return NULL ;
	}

  first_index = tmp_first->make_expr() ;
  last_index = tmp_last->make_expr() ;

#ifdef DEBUG_CHECK
  EXIT_TRACE ;
#endif

  return this ;
  assert(ref_this == ref_this) ; // pour le warning
}

//
T_access_authorisation T_array_item::get_auth_request(T_item *ref)
{
  if (ref ==  this->array_name )
	{
	  // l'appel vient du fils gauche :
	  // c'est le p�re qui connait le contexte.
	  return get_father()->get_auth_request(this) ;
	}
  else
	{
	  // l'appel vient d'un fils droit :
	  // c'est juste un acc�s en lecture d'un indice
	  return AUTH_READ ;
	}
}

// Fonction auxiliaire qui extrait l'identificateur passe en
// parametre si c'est un ident, un op_result ou un array_item, i.e.
// x, f(...) ou tab(...)
// Renvoie NULL en cas d'erreur
T_ident *fetch_ident(T_expr *expr)
{
  T_expr *cur_expr = expr ;
  while (cur_expr->is_an_ident() == FALSE)
	{
	  TRACE2("ici cur_expr %x:%s", cur_expr, cur_expr->get_class_name()) ;
	  switch (cur_expr->get_node_type())
		{
		case NODE_OP_RESULT :
		  {
			// Cast valide par construction et verifie a posteriori
			cur_expr = (T_expr *)((T_op_result *)cur_expr)->get_op_name() ;
			ASSERT(cur_expr->is_an_expr() == TRUE) ;
			break ;
		  }

		case NODE_ARRAY_ITEM :
		  {
			// Cast valide par construction et verifie a posteriori
			cur_expr = (T_expr *)((T_array_item *)cur_expr)->get_array_name() ;
			ASSERT(cur_expr->is_an_expr() == TRUE) ;
			break ;
		  }

		case NODE_CONVERSE :
		  {
			// Appel recursif
			return fetch_ident(((T_converse *)cur_expr)->get_expr()) ;
		  }

		default :
		  {
			// Pb !
			TRACE0("pb fetch_ident") ;
			return NULL ;
		  }
		}
	}

  // Cast justifie par le while
  return (T_ident *)cur_expr ;
}

  // Fonction qui rend l'expression sans les parentheses qui l'entourent
T_expr *T_expr::strip_parenthesis(void)
{
  TRACE2("T_expr(%x)::strip_parenthesis() -> return %x", this, this) ;
  return this ;
}

T_expr *T_expr_with_parenthesis::strip_parenthesis(void)
{
  TRACE2("T_expr_with_paren(%x)::strip_parenthesis() -> appel pour %x",
		 this, expr) ;
  T_expr *res = expr->strip_parenthesis() ;

  return res ;
}

// Renvoie l'expression "contenu" dans this
// i.e. this pour la classe T_expr et sous-classes
//      this->object pour T_list_link qui contient une expr, ...
// prerequis : is_an_expr == TRUE
T_expr *T_expr::make_expr(void)
{
  return this ;
}

void T_op_result::set_next_op_result(T_op_result *new_next_op_result)
{
	/*
  TRACE3("T_op_result(%x)::set_next_op_result(%x), was (%x)",
		 this,
		 new_next_op_result,
		 next_op_result) ;
  TRACE6("T_op_result(%x:%s)::set_next_op_result(%x:%d), was (%x:%d)",
		 this,
		 get_class_name(),
		 new_next_op_result,
		 (new_next_op_result == NULL) ? -1 : new_next_op_result->get_node_type(),
		 next_op_result,
		 (next_op_result == NULL) ? -1 :
		 (object_test(next_op_result) == FALSE) ? -2 : next_op_result->get_node_type()) ;
	*/

    ASSERT(this != new_next_op_result);
  next_op_result = new_next_op_result ;
}


//
// }{ METHODES DE VERIFICATION A POSTERIORI DE BONNE ECRITURE DES EXPRESSIOSN
// (par exemple pas de binop a = b)

// Tout d'abord les indirections par T_list_link
void T_item::post_check(void)
{
	/*
  TRACE2("T_item(%x:%s)::post_check -> ne doit pas arriver",
		 this,
		 get_class_name()) ;
	*/
  // Ici on peut tomber ici en cas de reprise sur erreur
  // auquel cas on ne dit rien
  if (get_error_count() == 0)
	{
	  internal_error(get_ref_lexem(),
					 __FILE__,
					 __LINE__,
				 get_error_msg(E_UNEXPECTED_NODE_TYPE),
					 get_node_type(),
					 make_class_name(this)) ;
	}
}

void T_list_link::post_check(void)
{
	//  TRACE2("T_list_link(%x:%s)::post_check -> on transmet a %x", this, object) ;
  T_item *item = (T_item *)object ;
  ASSERT(item->is_an_item() == TRUE) ;
  item->post_check() ;
}


// Puis le cas general
void T_expr::post_check(void)
{
	/*
  TRACE2("T_expr(%x:%s)::post_check -> on ne fait rien",
		 this,
		 get_class_name()) ;
	*/
	/*
  if (is_an_ident() == TRUE)
	{
	  T_ident *ident = (T_ident *)this ;
	  //TRACE1("ident name <%s>", ident->get_name()->get_value()) ;
	}
	*/
}

void T_op_result::post_check(void)
{
	/*
  TRACE1("T_op_result(%x)::post_check", this) ;
  TRACE2("op_name %x:%s", op_name, op_name->get_class_name()) ;
	*/
  if (ref_builtin != NULL && ref_builtin->get_lex_type() == L_BOOL)
	{
	  // Cas particulier de bool
	  // ATTENTION DANS CE CAS les parametres sont un T_predicate ...
	  TRACE0("cas particulier bool") ;
	  if (nb_in_params != 1)
		{
		  syntax_error(this->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND),
					   get_lex_type_ascii(ref_builtin->get_lex_type()),
					   nb_in_params) ;
		}
	  else if (first_in_param->is_a_predicate() == FALSE)
		{
		  syntax_predicate_expected_error(first_in_param, CAN_CONTINUE) ;
		}
	  else
		{
		  TRACE0("ok") ;
		}
	}

  if (op_name->is_an_expr() == TRUE)
	{
	  op_name->make_expr()->post_check() ;
	}

  T_item *cur = first_in_param ;
  while (cur != NULL)
	{
	  if (cur->is_an_expr() == TRUE)
		{
		  cur->make_expr()->post_check() ;
		}
	  cur = cur->get_next() ;
	}


}

void T_expr_with_parenthesis::post_check(void)
{
  expr->post_check() ;
}

void T_image::post_check(void)
{
  if (relation != NULL)
	{
	  // relation peut etre null pour une suite en extension,
	  // modelisee temporairement en T_image
	  relation->post_check() ;
	}
  expr->post_check() ;
}

void T_sigma::post_check(void)
{
  expr->post_check() ;
}

void T_extensive_seq::post_check(void)
{
  T_expr *cur = first_item ;
  while (cur != NULL)
	{
	  cur->post_check() ;
	  cur = (T_expr *)cur->get_next() ;
	}
}

void T_lambda_expr::post_check(void)
{
  expr->post_check() ;
}

void T_unary_op::post_check(void)
{
  operand->post_check() ;
}

void T_binary_op::post_check(void)
{
	//  TRACE2("T_binary_op(%x, oper %d)::post_check()", this, oper) ;

  // Verification recursive
  if (op1->is_an_expr() == TRUE)
	{
	  op1->make_expr()->post_check() ;
	}

  if (op2->is_an_expr() == TRUE)
	{
	  op2->make_expr()->post_check() ;
	}
}

void T_array_item::post_check(void)
{
  if (array_name->is_an_expr() == TRUE)
	{
	  array_name->make_expr()->post_check() ;
	}

  T_expr *cur = first_index ;
  while (cur != NULL)
	{
	  cur->post_check() ;
	  cur = (T_expr *)cur->get_next() ;
	}
}

void T_pi::post_check(void)
{
  expr->post_check() ;
}

void T_extensive_set::post_check(void)
{
	/*
  TRACE2(">> T_extensive_set(%x:%s)::post_check",
		 this, get_class_name()) ;
  ENTER_TRACE ;
	*/
  T_expr *cur = first_item ;
  while (cur != NULL)
	{
		//TRACE2("cur %x (%s)", cur, cur->get_class_name()) ;
	  cur->post_check() ;
	  cur = (T_expr *)cur->get_next() ;
	}
  /*
  TRACE2(">> T_extensive_set(%x:%s)::post_check",
		 this, get_class_name()) ;
  EXIT_TRACE ;
  */
}

void T_converse::post_check(void)
{
	/*
  TRACE3(">> T_converse(%x)::post_check(expr = %x, %s)",
		 this,
		 expr,
		 make_class_name(expr)) ;
		 ENTER_TRACE ;*/

  expr->post_check() ;

  /*

  EXIT_TRACE ;
  TRACE1("<< T_converse(%x)::post_check", this) ;
  */
}

void T_generalised_intersection::post_check(void)
{
  T_expr *cur = first_expr ;
  while (cur != NULL)
	{
	  cur->post_check() ;
	  cur = (T_expr *)cur->get_next() ;
	}
}

void T_quantified_intersection::post_check(void)
{
  expr->post_check() ;
}

void T_generalised_union::post_check(void)
{
  T_expr *cur = first_expr ;
  while (cur != NULL)
	{
	  cur->post_check() ;
	  cur = (T_expr *)cur->get_next() ;
	}
}

void T_quantified_union::post_check(void)
{
  expr->post_check() ;
}


#ifdef COMPAT
// Methode qui permet de dire que les lexemes '(' et ')' delimitant un
// T_expr_with_parenthesis doivent etre ignore par le
// decompilateur. Si le lexeme provient de l'expansion d'une
// definition, on propage la correction a la definition elle-meme
void T_expr_with_parenthesis::mark_as_void(void)
{
  TRACE1("T_expr_with_parenthesis(%x)::mark_as_void", this) ;

  // Il faut supprimer le '(' et le ')' du flux de lexemes

  // 1) Par construction, get_ref_lexem() pointe sur le ')'
  get_ref_lexem()->mark_as_void() ;

  // 2) On cherche le '('
  T_lexem *cur_lexem = get_ref_lexem() ;
  int cpt_paren = 0 ;
  int encore = TRUE ;

  while (encore == TRUE)
	{
	  cur_lexem = cur_lexem->get_prev_lexem() ;

	  if (cur_lexem == NULL)
		{
		  encore = FALSE ;
		}
	  else if (cur_lexem->get_lex_type() == L_CLOSE_PAREN)
		{
		  cpt_paren++ ;
		}
	  else if (cur_lexem->get_lex_type() == L_OPEN_PAREN)
		{
		  if (cpt_paren == 0)
			{
			  // Trouve !
			  cur_lexem->mark_as_void() ;
			  encore = FALSE ;
			}
		  else
			{
			  // Un de moins ...
			  cpt_paren -- ;
			}
		}
	}
}
#endif

// Verification que les entiers litteraux ne sont pas superieur a MAXINT
// ou inferieur a MININT
void T_literal_integer::post_check(void)
{
  //On transforme automatiquement les -0 en 0
  if ((value->get_is_positive() == FALSE) &&
	  (value->get_value() == lookup_symbol("0")))
	{
	  value->set_opposite();
	}
}

//
// }{ METHODES D'AJOUT
//
void T_extensive_set::add_item(T_expr *new_item)
{
  new_item->set_father(this);
  new_item->tail_insert((T_item **) &first_item, (T_item **) &last_item);
}

static bool maybe_generic(T_type* type)
{
    return type == NULL || type->has_wildcards(TRUE);
}

//
// Cree un type POW(t) dont le parent est p,
// et l'associe à l'expression e.
// Le constructeur de T_setof_type change le parent du
// premier paramètre. Pour ne pas modifier le chainage,
// on doit cloner avant de passer en parametre a ce
// constructeur.
//
static void
set_B_type_setof
(T_type* t, T_expr* e, T_expr* p)
{
    if(maybe_generic(t))
        return;
    T_betree* bt = p->get_betree();
    T_lexem* l = p->get_ref_lexem();
    T_type* nt = t->clone_type(NULL, bt, l);
    T_setof_type *st = new T_setof_type(nt, NULL, bt, l) ;
    e->set_B_type_rec(st);
    nt->unlink();
    st->unlink();
}

//
// Cree un type POW(ta * tb) dont le parent est p,
// et l'associe à l'expression e.
static void
set_B_type_setof_pairs
(T_type* ta, T_type* tb, T_expr* e, T_expr* p)
{
    if(maybe_generic(ta) || maybe_generic(tb))
        return;
    T_betree* bt = p->get_betree();
    T_lexem* l = p->get_ref_lexem();
    T_type* nta = ta->clone_type(NULL, bt, l);
    T_type* ntb = tb->clone_type(NULL, bt, l);
    T_type *pt = new T_product_type(nta, ntb, p, bt, l) ;
    T_setof_type *spt = new T_setof_type(pt, p, bt, l) ;
    e->set_B_type_rec(spt);
    nta->unlink();
    ntb->unlink();
    pt->unlink();
    spt->unlink();
}

//
// }{ METHODES AUXILIAIRES DE TYPAGE
//
// Tres inefficace - car on peut avoir une expression avec un type
// instancie et, en meme temps, une sous-expression dont le type n'est
// pas completement instancie. On ne dispose donc pas de moyen de
// determiner quand arreter la recursion.
//
// Pour eviter un cout trop eleve, des conditions d'arret abusives
// ont ete placees (appels à la fonction maybe_generic) alors que
// des sous-expressions pourraient ne toujours pas etre completement
// typees.
//
// A FAIRE : concevoir un algorithme  d'inference et de verification
// de type inspire des algorithmes J et W POUR TOUTE LA PARTIE DE
// TYPAGE du B_COMPILER
//
void T_expr::set_B_type_rec(T_type* type)
{
    if (maybe_generic(B_type)) {
        if(B_type != NULL)
            B_type->unlink();
        B_type = type;
        type->link();
    }
}

void T_binary_op::set_B_type_rec(T_type * type)
{
    T_expr::set_B_type_rec(type);

    // reprises sur erreur
    if (!op1->is_an_expr())
        return;
    if (!op2->is_an_expr())
        return;
    T_expr* e1 = (T_expr*) op1;
    T_type* te1 = e1->get_B_type();
    T_expr* e2 = (T_expr*) op2;
    T_type* te2 = e2->get_B_type();

    switch(oper) {
        // rien a faire dans ces cas
    case BOP_PLUS:
    case BOP_DIVIDES:
    case BOP_MOD:
    case BOP_POWER:
        return;
        // ici, traiter le cas de la soustraction ensembliste
    case BOP_MINUS: {
        // rien a faire dans ces cas
        if(type->is_an_integer() || type->is_a_real() || type->is_a_float())
            return;
        if(!type->is_a_set())
            return; // reprises sur erreur
        e1->set_B_type_rec(type);
        e2->set_B_type_rec(type);

    }
        return;
        // ici, traiter le cas du produit cartesien
    case BOP_TIMES : {
        if (type->is_an_integer() || type->is_a_real() || type->is_a_float())
            return;
        // inference de type pour e1 * ... * en
        // this : e1 * ... * en, TYPE(e1 * ... * en) == type
        // donc
        // type == POW(t1 * t2), TYPE(e1) == POW(t1), TYPE(e2) == POW(t2)
        // t0 := t1 * t2
        // pt1 := POW(t1)
        // pt2 := POW(t2)
        if(!type->is_a_set())
            return; // reprise sur erreur
        T_type* t0 = ((T_setof_type*) type)->get_base_type();
        if (t0->get_nb_types() < 2)
            return;

        if(maybe_generic(te1)) {
            T_type* t1 = ((T_product_type*) t0)->get_type1();
            set_B_type_setof(t1, e1, this);
        }
        if(maybe_generic(te2)) {
            T_type* t2 = ((T_product_type*) t0)->get_type2();
            set_B_type_setof(t2, e2, this);
        }
        return;
    }
    case BOP_RESTRICT:
    case BOP_ANTIRESTRICT: {
        // inference de type pour e1 <| e2 (idem pour e1 <<| e2)
        // this : e1 <| e2, TYPE(e1 <| e2) == type
        // donc
        // type == POW(t1 * t2), TYPE(e1) == POW(t1),
        // TYPE(e2) == POW(t1 * t2)
        // pt1 := POW(t1)
        if(!type->is_a_set())
            return; // reprise sur erreur
        if(maybe_generic(te1)) {
            T_type* btype = ((T_setof_type*) type)->get_base_type();
            if (btype->get_nb_types() < 2)
                return; // reprise sur erreur
            T_type* t1 = ((T_product_type*) btype)->get_type1();
            set_B_type_setof(t1, e1, this);
        }
        if(te2 == NULL || te2->has_wildcards(TRUE)) {
            e2->set_B_type_rec(type);
        }
    }
        return;
    case BOP_CORESTRICT:
    case BOP_ANTICORESTRICT: {
        // inference de type pour e1 |> e2 (idem pour e1 |>> e2)
        // this : e1 |> e2, TYPE(e1 |> e2) == type
        // donc
        // type == POW(t1 * t2), TYPE(e1) == POW(t1 * t2),
        // TYPE(e2) == POW(t2)
        // pt2 := POW(t2)
        if (!type->is_a_set())
            return; // reprise sur erreur
        if(maybe_generic(te1)) {
            e1->set_B_type_rec(type);
        }
        if(maybe_generic(te2)) {
            T_type* btype = ((T_setof_type*) type)->get_base_type();
            if (btype->get_nb_types() < 2)
                return; // reprise sur erreur
            T_type* t2 = ((T_product_type*) btype)->get_type2();
            set_B_type_setof(t2, e2, this);
        }
    }
        return;
    case BOP_LEFT_OVERLOAD :
    case BOP_CONCAT:
    case BOP_INTERSECTION:
    case BOP_UNION: {
        // this : e1 <+ e2, TYPE(e1 <+ e2) = type
        // donc TYPE(e1) == type, TYPE(e2) == type
        e1->set_B_type_rec(type);
        e2->set_B_type_rec(type);
    }
        return;
        // A FAIRE Comprendre pourquoi le typage est déjà réalisé
        // dans ce cas
    case BOP_HEAD_INSERT:
        return;
        // A FAIRE Comprendre pourquoi le typage est déjà réalisé
        // dans ce cas
    case BOP_TAIL_INSERT:
        return;
        // A FAIRE
    case BOP_HEAD_RESTRICT:
    case BOP_TAIL_RESTRICT: {
        // this : e1 /|\ e2, TYPE(e1 /|\ e2) = type
        // donc TYPE(e1) == type
        if(maybe_generic(te1)) {
            e1->set_B_type_rec(type);
        }
    }
        return;
    case BOP_INTERVAL: {
      if (!type->is_a_set())
	return; // reprise sur err
      if(te1 != nullptr && te2 != nullptr)
	return; // rien a faire dans ce cas
      T_type* btype = ((T_setof_type*) type)->get_base_type();
      if(te1 == nullptr)
	e1->set_B_type_rec(btype);
      if(te2 == nullptr)
	e2->set_B_type_rec(btype);
      return;
    }
    case BOP_MAPLET :
        // A FAIRE : tester ce second cas
    case BOP_COMMA : {
        // this : e1 |-> e2
        // TYPE(e1 |-> e2) = t1 * t2
        // donc type == t1 * t2, TYPE(e1) == t1, TYPE(e2) == t2

        if (type->get_nb_types() != 2)
            return;

        if(maybe_generic(te1)) {
            e1->set_B_type_rec(((T_product_type*) type)->get_type1());
        }
        if(maybe_generic(te2)) {
            e2->set_B_type_rec(((T_product_type*) type)->get_type2());
        }
        return;
    }
    case BOP_SET_RELATION:
    case BOP_SURJ_RELATION:
    case BOP_TOTAL_RELATION:
    case BOP_TOTAL_SURJ_RELATION:{
        // this : e1 <<->> e2
        // TYPE(e1 <<->> e2) = POW(POW(t1 * t2)) == type
        // donc TYPE(e1) == POW(t1), TYPE(e2) == POW(t2)
        if (!type->is_a_set())
            return; // reprise sur erreur
        T_type* btype = ((T_setof_type*) type)->get_base_type();
        if (!btype->is_a_set())
            return; // reprise sur erreur
        T_type* bbtype = ((T_setof_type*) btype)->get_base_type();
        if (bbtype->get_nb_types() < 2)
            return; // reprise sur erreur
        if(maybe_generic(te1)) {
            T_type* t1 = ((T_product_type*) bbtype)->get_type1();
            set_B_type_setof(t1, e1, this);
        }
        if(maybe_generic(te2)) {
            T_type* t2 = ((T_product_type*) bbtype)->get_type2();
            set_B_type_setof(t2, e2, this);
        }
    }
        return;
        // A FAIRE Voir à quoi cela correspond ?
        // Il semblerait que BOP_TIMES occupe le role prevu pour
        // cet operateur.
    case BOP_CONSTANT_FUNCTION:
        return;
        // A FAIRE
    case BOP_COMPOSITION: {
        // this = (e1 ; e2), type = POW(t1 * t2)
        // TYPE(e1) = POW(t1 * tx), TYPE(e2) = POW(tx * t2)
        // Il faut trouver tx, que l'on trouve dans te1 ou dans te2
        if(!type->is_a_set())
            return; // reprise sur erreur:
        T_type* btype = ((T_setof_type*)type)->get_base_type();
        if(btype->get_nb_types() < 2)
            return; // reprise sur erreur
        if(maybe_generic(te1)) {
            if(te2 == NULL || !te2->is_a_set())
        return;
            T_type* bte = ((T_setof_type*)te2)->get_base_type();
            if(bte->get_nb_types() < 2)
        return;
            T_type* nt1 = ((T_product_type*) btype)->get_type1();
            T_type* nt2 = ((T_product_type*) bte)->get_type1();
            if(!maybe_generic(nt1) && !maybe_generic(nt2)) {
                set_B_type_setof_pairs(nt1, nt2, e1, this);
            }
        }
        if(maybe_generic(te2)) {
            if(te1 == NULL || !te1->is_a_set())
                return;
            T_type* bte = ((T_setof_type*)te1)->get_base_type();
            if(bte->get_nb_types() < 2)
                return;
            T_type* nt1 = ((T_product_type*) bte)->get_type2();
            T_type* nt2 = ((T_product_type*) btype)->get_type2();
            if(!maybe_generic(nt1) && !maybe_generic(nt2)) {
                set_B_type_setof_pairs(nt1, nt2, e2, this);
            }
        }
    }
        return;
    case BOP_DIRECT_PRODUCT: {
        // this = e1 >< e2, type = POW(t1 * (t2 * t3))
        // TYPE(e1) = POW(t1 * t2), TYPE(e2) = POW(t1 * t3)
        if(!type->is_a_set())
            return;
        T_type* btype = ((T_setof_type*)type)->get_base_type();
        if(btype->get_nb_types() < 2)
            return;
        T_type* t1 = ((T_product_type*) btype)->get_type1();
        T_type* pt = ((T_product_type*) btype)->get_type2();
        if(pt->get_nb_types() < 2)
            return;
        T_type* t2 = ((T_product_type*) pt)->get_type1();
        T_type* t3 = ((T_product_type*) pt)->get_type2();
        if(maybe_generic(te1)) {
            set_B_type_setof_pairs(t1, t2, e1, this);
        }
        if(maybe_generic(te2)) {
            set_B_type_setof_pairs(t1, t3, e2, this);
        }
    }
        return;
    case BOP_PARALLEL_PRODUCT: {
        // this = e1 || e2, type = POW((t1 * t2) * (t3 * t4))
        // TYPE(e1) = POW(t1 * t3), TYPE(e2) = POW(t2 * t4)
        if(!type->is_a_set())
            return;
        T_type* btype = ((T_setof_type*)type)->get_base_type();
        if(btype->get_nb_types() < 2)
            return;
        T_type* tl = ((T_product_type*) btype)->get_type1();
        T_type* tr = ((T_product_type*) btype)->get_type2();
        // tl: t1 * t2, tr: t3 * t4
        if(tl->get_nb_types() < 2)
            return;
        if(tr->get_nb_types() < 2)
            return;
        T_type* t1 = ((T_product_type*) tl)->get_type1();
        T_type* t2 = ((T_product_type*) tl)->get_type2();
        T_type* t3 = ((T_product_type*) tr)->get_type1();
        T_type* t4 = ((T_product_type*) tr)->get_type2();
        if(maybe_generic(te1)) {
            set_B_type_setof_pairs(t1, t3, e1, this);
        }
        if(maybe_generic(te2)) {
            set_B_type_setof_pairs(t2, t4, e2, this);
        }
    }
        return;
        // A FAIRE reporter erreur
    case BOP_PIPE:
        return;
    }
    // A FAIRE verifier s'il ne faut pas faire de traitements pour d'autres opérateurs
}

void T_extensive_set::set_B_type_rec(T_type* type)
{
    if (!type->is_a_set())
        return; // reprise sur erreur

    T_expr::set_B_type_rec(type);

    T_setof_type* setof_type = (T_setof_type*) type;
    T_type* base_type = setof_type->get_base_type();

    T_expr *cur = first_item ;
    while (cur != NULL)	{
        cur->set_B_type_rec(base_type);
        cur = (T_expr *)cur->get_next() ;
    }
}

void T_extensive_seq::set_B_type_rec(T_type* type)
{
    if (!type->is_a_seq())
        return; // reprise sur erreur

    T_expr::set_B_type_rec(type);

    T_type* base_type = ((T_setof_type*) type)->get_seq_base_type();

    T_expr *cur = first_item ;
    while (cur != NULL)	{
        cur->set_B_type_rec(base_type);
        cur = (T_expr *)cur->get_next() ;
    }
}

void T_lambda_expr::set_B_type_rec(T_type * type)
{
    if (!type->is_a_set())
        return; // reprise sur erreur

    T_expr::set_B_type_rec(type);

    // this : %(v)(p1 | e2),
    // p1 est un prédicat typant tel que TYPE(v) = POW(t1)
    // type == POW(t1 * t2), TYPE(e2) == POW(t2)

    T_type* base_type = ((T_setof_type*) type)->get_base_type();

    // t0 := t1 * t2
    // pt1 := POW(t1)
    // pt2 := POW(t2)
    if (base_type->get_nb_types() < 2)
        return; // reprise sur erreur

    T_type* texpr = expr->get_B_type();
    if(texpr == NULL || texpr->has_wildcards(TRUE)) {
        // Le constructeur de T_setof_type change le parent du
        // premier paramètre. Comme il faut conserver le chainage
        // entre les descendants de t0, on doit les cloner avant
        // de les passer en parametre a ce constructeur.
        T_type* t2 =
            ((T_product_type*) base_type)
            ->get_type2()->clone_type(NULL, get_betree(), get_ref_lexem());
        expr->set_B_type_rec(t2);
        t2->unlink();
    }
}

void T_array_item::set_B_type_rec(T_type* type)
{
    // inference de type pour f(x1 ... xn)
    // => on va ici inferer le type de x1 ... xn a partir du type de f

    // pour f(a), on a TYPE(f) == POW( ta * type )
    // pour f(a,b), on a TYPE(f) == POW( ( ta*tb ) * type )
    // pour f(a,b,c), on a TYPE(f) == POW( ( (ta*tb)*tc ) * type )
    // donc seul le type du dernier argument est accessible directement.
    // on fait donc un parcours inverse de la liste des arguments

    if(array_name->get_B_type() == NULL || !array_name->get_B_type()->is_a_set())
        return; // reprise sur erreur

    if(last_index == NULL || !last_index->is_an_item())
        return; // reprise sur erreur

    T_expr::set_B_type_rec(type);

    T_setof_type * setof_type = (T_setof_type*) array_name->get_B_type();
    T_type* base_type = setof_type->get_base_type();
    if(base_type->get_nb_types() < 2)
        return; // reprise sur erreur
    T_product_type* pt = (T_product_type*) base_type;
    T_item* item = (T_item*) last_index; // parcours des index
    T_type* dt = pt->get_type1();        // parcours du type du domaine
    while (item != first_index) {
        if(dt->get_nb_types() < 2)
            return; // reprise sur erreur
        if(!item->is_an_expr())
            return; // reprise sur erreur
        T_item* prev = item->get_prev();
        ((T_expr*)item)->set_B_type_rec(((T_product_type*) dt)->get_type2());
        dt = ((T_product_type*) dt)->get_type1();
        item = prev;
    }
    if(!item->is_an_expr())
        return; // reprise sur erreur
    T_type* titem = item->get_B_type();
    ((T_expr*)item)->set_B_type_rec(dt);

    // calcul de ft, le type de f (c.a.d. array_name)
    T_betree* bt = get_betree();
    T_lexem* l = get_ref_lexem();
    item = first_index;
    T_type* ft = item->get_B_type();
    T_type* nft = NULL;
    while(item != last_index) {
        item = item->get_next();
        T_type* it = item->get_B_type();
        nft = ft->clone_type(NULL, bt, l);
        T_type* nit = it->clone_type(NULL, bt, l);
        ft = new T_product_type(nft, nit, this, bt, l);
        nft->unlink();
        nit->unlink();
    }
    nft = ft->clone_type(NULL, bt, l);
    T_type* ntype = type->clone_type(NULL, bt, l);
    ft = new T_product_type(nft, ntype, this, bt, l);
    nft->unlink();
    ntype->unlink();
    T_type* pft = new T_setof_type(ft, NULL, bt, l);
    ft->unlink();
    ((T_expr*)array_name)->set_B_type_rec(pft);
    pft->unlink();
}

void T_expr_with_parenthesis::set_B_type_rec(T_type* type)
{
    /*
  fprintf(stderr,
          "T_expr_with_parenthesis::set_B_type_rec\n"
          "\tthis: %p\n"
          "\ttype: %p\n"
          , this, type
          );
  this->dump_html();
  type->dump_html();
    */
  if(expr == NULL)
      return; // reprise sur erreur

  T_expr::set_B_type_rec(type);
  expr->set_B_type_rec(type);
}

// A FAIRE verifier s'il ne faut pas surcharger set_B_type_rec
// sur d'autres classes.

//
//	}{	Fin du fichier
//
