/******************************* CLEARSY **************************************
* Fichier : $Id: c_parexp.cpp,v 2.0 2000-11-03 12:52:18 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions "parenthesees"
*
* Compilation :		-DDEBUG_PAREXPR pour avoir des traces
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
RCS_ID("$Id: c_parexp.cpp,v 1.31 2000-11-03 12:52:18 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_expr_with_parenthesis
//
T_expr_with_parenthesis::T_expr_with_parenthesis(T_expr *new_expr,
														  T_item **adr_first,
														  T_item **adr_last,
														  T_item *father,
														  T_betree *betree,
														  T_lexem *ref_lexem)
  :T_expr(NODE_EXPR_WITH_PARENTHESIS,
		  adr_first,
		  adr_last,
		  father,
		  betree,
		  ref_lexem)
{
#ifdef DEBUG_PAREXPR
  TRACE7(">>T_expr_with_paren(%x)::T_expr_with_paren(%x,%x,%d,%x,%x,%x,%x,%x)",
		 this,
  		 new_expr,
		 adr_first,
		 adr_last,
		 father,
		 betree,
		 ref_lexem) ;
#endif // DEBUG_PAREXPR

  expr = new_expr ;

  // On met a jour les liens "father"
  expr->set_father(this) ;

#ifdef DEBUG_PAREXPR
  TRACE7("<<T_expr_with_paren(%x)::T_expr_with_paren(%x,%x,%d,%x,%x,%x,%x,%x)",
		 this,
  		 new_expr,
		 adr_first,
		 adr_last,
		 father,
		 betree,
		 ref_lexem) ;
#endif // DEBUG_PAREXPR
}

T_item *T_expr_with_parenthesis::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1(">> T_expr_with_parenthesis(%x)::check_tree()", this) ;
  ENTER_TRACE ;
  TRACE1("appel recursif pour expr=%x", expr) ;
#endif

  if (expr != NULL)
	{
	  expr->check_tree((T_item **)&expr) ;
	}

#ifdef DEBUG_CHECK
  EXIT_TRACE ;
  TRACE1("<< T_expr_with_parenthesis(%x)::check_tree()", this) ;
#endif

  return this ;
  // Pour le warning
  ref_this = ref_this ;
}

//
//	Destructeur
//
T_expr_with_parenthesis::~T_expr_with_parenthesis(void)
{
  TRACE1("T_expr_with_parenthesis::~T_expr_with_parenthesis(%x)", this) ;
}

//
//	}{	Constructeur de la classe T_comprehensive_set
//
T_comprehensive_set::T_comprehensive_set(T_expr *expr,
												  T_item **adr_first,
												  T_item **adr_last,
												  T_item *father,
												  T_betree *betree,
												  T_lexem *ref_lexem)
  :T_expr(NODE_COMPREHENSIVE_SET, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PAREXPR
  TRACE6("T_comprehensive_set::T_comprehensive_set(%x,%x,%d,%x,%x,%x,%x,%x)",
		 expr, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  /*
  // 		Par construction, expr est de la forme suivante pour :
  //		l'ensemble { xx, yy | xx : NAT & yy : NAT & xx + yy = 10}
  //  	expr est par construction un binop de type BOP_PIPE (verifie par appelant)
  //
  //                      |
  //          /---------------------\
  //         ,                       &
  //        / \                  /--------\
  //       xx yy                /          \
  //                           &            =
  //                         /    \        / \
  //                        :      :      +  10
  //                       / \    / \    / \
  //                     xx  NAT yy NAT xx yy
  //
  */
  ASSERT(expr->get_node_type() == NODE_BINARY_OP) ;
  T_binary_op *bop = (T_binary_op *)expr ;
  ASSERT(bop->get_operator() == BOP_PIPE) ;

  // Les identificateurs
  first_ident = last_ident = NULL ;
  bop->get_op1()->extract_params((int)BOP_COMMA,
								 this,
								 (T_item **)&first_ident,
								 (T_item **)&last_ident) ;

  // On verifie que ce sont des idents. Au passage, on met a jour le champ
  // father
  T_ident *cur_ident = first_ident ;

  while (cur_ident != NULL)
	{
	  if (cur_ident->is_an_ident() == FALSE)
		{
		  syntax_ident_expected_error(cur_ident, CAN_CONTINUE) ;
		}
	  else
		{
		  cur_ident->set_father(this) ;

		  // On en profite pour dire que sont sont des quantificateurs locaux
		  cur_ident->set_ident_type(ITYPE_LOCAL_QUANTIFIER) ;
		}

	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}


  // Le predicat
  T_item *new_pred = bop->get_op2() ;
  if (new_pred->is_a_predicate() == FALSE)
	{
	  TRACE2("erreur : op2 = %x <%s> pas un predicat",
			 new_pred, new_pred->get_class_name()) ;
	  syntax_predicate_expected_error(new_pred, CAN_CONTINUE) ;
	}
  predicate = (T_predicate *)new_pred ;
  predicate->set_father(this) ;

#ifdef DEBUG_PAREXPR
  TRACE0("fin de T_comprehensive_set::T_comprehensive_set") ;
#endif
}

//
//	Destructeur
//
T_comprehensive_set::~T_comprehensive_set(void)
{
  TRACE1("T_comprehensive_set::~T_comprehensive_set(%x)", this) ;
}

// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_comprehensive_set::check_tree(T_item **ref_this)
{
  // On fait ensuite des check_tree maintenant qu'on sait
  // qu'il n'y a que des expressions
#ifdef DEBUG_CHECK
  TRACE1("T_comprehensive_set(%x)::check_tree des elements a l'interieur", this) ;
#endif

  if (predicate != NULL)
	{
	  predicate->check_tree((T_item **)&predicate) ;
	}
  return this ;
  // Pour le waring
  ref_this = ref_this ;
}

//
//	}{	Constructeur de la classe T_image
//
T_image::T_image(T_expr *new_expr,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *father,
						  T_betree *betree,
						  T_lexem *ref_lexem)
  :T_expr(NODE_IMAGE, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PAREXPR
  TRACE6("T_image::T_image(%x,%x,%d,%x,%x,%x,%x,%x)",
		 expr, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  expr = new_expr ;
  relation = NULL ;

  // On met a jour les liens "father"
  expr->set_father(this) ;

#ifdef DEBUG_PAREXPR
  TRACE0("fin de T_image::T_image") ;
#endif

}

//
//	}{	"fin" du constructeur : on met en place la relation
//
void T_image::end_constructor(T_item *new_relation)
{
#ifdef DEBUG_PAREXPR
  TRACE2("T_image(%x)::end_constructor(%x)", this, new_relation) ;
#endif

  if (new_relation->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(new_relation, FATAL_ERROR) ;
	}
  else
	{
	  new_relation->make_expr()->post_check() ;
	}

  relation = new_relation->make_expr() ;
  relation->set_father(this) ;
  set_ref_lexem(relation->get_ref_lexem()) ;
}

//
//	Destructeur
//
T_image::~T_image(void)
{
}


//
//	}{	Phase de correction
// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_image::check_tree(T_item **ref_this)
{
#ifdef DEBUG_PAREXPR
  TRACE1("T_image(%x::%x)::check_tree", this) ;
#endif

  if (expr == NULL)
	{
	  return NULL ;
	}

  expr->check_tree((T_item **)&expr) ;
  if (relation != NULL)
	{
	  relation->check_tree((T_item **)&relation) ;
	  return this ;
	}

  TRACE0("relation == NULL : on se change en T_extensive_seq") ;
  T_extensive_seq *eseq = new T_extensive_seq(this,
											  NULL,
											  NULL,
											  get_father(),
											  get_betree(),
											  get_ref_lexem()) ;

  // Cast valide par construction
  eseq = (T_extensive_seq *)eseq->check_tree(NULL) ;
  ASSERT(eseq->get_node_type() == NODE_EXTENSIVE_SEQ) ;

  if (ref_this != NULL)
	{
#ifdef DEBUG_PAREXPR
	  TRACE2("mise a jour de ref_this(%x) avec %x", ref_this, eseq) ;
#endif // DEBUG_PAREXPR
	  *ref_this = eseq ;
	}

  // Tres important : on met expr a NULL pour ne pas le liberer dans le
  // destructeur. En effet, la responsabilite a ete transferee au
  // T_extensive_seq. Voir le commentaire dans T_extensive_seq::T_extensive_seq
  expr = NULL ;

  return eseq ;
}

//
//	}{	Constructeur de la classe T_extensive_set
//
T_extensive_set::T_extensive_set(T_expr *new_expr,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *father,
										  T_betree *betree,
										  T_lexem *ref_lexem)
  :T_expr(NODE_EXTENSIVE_SET, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PAREXPR
  TRACE6("T_extensive_set::T_extensive_set(%x,%x,%d,%x,%x,%x,%x,%x)",
		 new_expr, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  first_item = last_item = NULL ;
  T_expr *save_expr = new_expr ;

#ifdef MATRA
  // Correction anomalie 2163 : ceci est un reste de
  // compatibilite pour METEOR
  new_expr = new_expr->strip_parenthesis() ;
#endif // MATRA

  if (save_expr != new_expr)
	{
	  syntax_warning(save_expr->get_ref_lexem(),
					 EXTENDED_WARNING,
					 get_warning_msg(W_EXTRA_PAREN_IN_EXPR_IGNORED)) ;
	}

  new_expr->extract_params((int)BOP_COMMA,
						   this,
						   (T_item **)&first_item,
						   (T_item **)&last_item) ;

  T_expr *cur_item = first_item ;
  while (cur_item != NULL)
	{
	  if (cur_item->is_an_expr() == FALSE)
		{
		  syntax_expr_expected_error(cur_item, CAN_CONTINUE) ;
		}
	  else
		{
		  T_expr *cur_expr = cur_item->make_expr() ;
		  if (cur_expr != cur_item) // cas si cur_item est un T_list_link
			{
			  // Il faut les echanger dans la liste
			  cur_item->exchange_in_list(cur_expr,
										 (T_item **)&first_item,
										 (T_item **)&last_item) ;
			}

		  cur_expr->post_check() ;
		}
	  cur_item = (T_expr *)cur_item->get_next() ;
	}

#ifdef DEBUG_PAREXPR
  TRACE0("fin de T_extensive_set::T_extensive_set") ;
#endif
}

// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_extensive_set::check_tree(T_item **ref_this)
{
  // On fait ensuite des check_tree maintenant qu'on sait
  // qu'il n'y a que des expressions
#ifdef DEBUG_CHECK
  TRACE1("T_exensive_set(%x)::check_tree des elements a l'interieur", this) ;
#endif

  T_expr *cur_item = first_item ;

  if (first_item == NULL)
	{
	  return NULL ;
	}

  T_item *tmp_first = NULL ;
  T_item *tmp_last = NULL ;

  while (cur_item != NULL)
	{
	  // Cast justifie car on fait un check d'une expression, qui ne
	  // peut que rendre une expression. Le ASSERT ci-apres le
	  // verifie a posteriori

	  // On memorise le next avant de casser le chainage
	  T_expr *next = (T_expr *)cur_item->get_next() ;

	  //GP l'instruction ci-dessous provoque une seg-fault
	  T_item *res_item = cur_item->check_tree(NULL) ;
	  T_item *res = res_item->make_expr() ;
	  ASSERT(res->is_an_expr() == TRUE) ;

	  res->tail_insert(&tmp_first, &tmp_last) ;
	  cur_item = next ;
	}

  first_item = tmp_first->make_expr() ;
  last_item = tmp_last->make_expr() ;

  return this ;
  assert(ref_this == ref_this) ; // pour le warning
}


//
//	Destructeur
//
T_extensive_set::~T_extensive_set(void)
{
  TRACE1("T_extensive_set::~T_extensive_set(%x)", this) ;
}

//
//	}{	Constructeur de la classe T_extensive_seq
//
T_extensive_seq::T_extensive_seq(T_image *ref_image,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *father,
										  T_betree *betree,
										  T_lexem *ref_lexem)
  :T_expr(NODE_EXTENSIVE_SEQ, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_PAREXPR
  TRACE6("T_extensive_seq::T_extensive_seq(%x,%x,%d,%x,%x,%x,%x,%x)",
		 ref_image, adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  first_item = last_item = NULL ;

  ref_image->get_expr()->extract_params((int)BOP_COMMA,
										this,
										(T_item **)&first_item,
										(T_item **)&last_item) ;

  // On verifie que l'on a des expressions
  // NON : Au passage, on pose les liens (car le T_image source va se suicider)
  // NON car le extract_params et la mise a null de expr dans le ref_image
  // invalide cette liberation.
  // on a ici un transfert de responsabilite
  // On ne peut pas juste faire link ici et unlink dans le T_image car il
  // faut liberer toute la structure autour des values i.e. les T_binary_op
  T_item *cur = first_item ;
  int error = FALSE ;

  while (cur != NULL)
	{
	  if (cur->is_an_expr() == FALSE)
		{
		  syntax_expr_expected_error(cur, CAN_CONTINUE) ;
		  error = TRUE ;
		}
	  else
		{
		  cur->make_expr()->post_check() ;
		}
	  //	  cur->link() ;  NON CF CI-DESSUS
	  cur = cur->get_next() ;
	  TRACE1("cur = %x", cur) ;
	}

  // object_unlink(ref_image->get_expr()) ; NON fait automatiquement
  // par extract_params. La mise a NULL de ref_image->get_expr() est
  // faite par l'appelant
  if (error == TRUE)
	{
	  TRACE0("fin prematuree de T_extensive_seq::T_extensive_seq") ;
	  return ;
	}



}

// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_extensive_seq::check_tree(T_item **ref_this)
{
  // On fait ensuite des check_tree maintenant qu'on sait
  // qu'il n'y a que des expressions
  //TRACE1("T_exensive_seq(%x)::check_tree des elements a l'interieur", this) ;

  if (first_item == NULL)
	{
	  return NULL ;
	}

  T_expr *cur_item = first_item ;
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
	  cur_item = next ;
	}

  first_item = tmp_first->make_expr() ;
  last_item = tmp_last->make_expr() ;

  return this ;
  assert(ref_this == ref_this) ; // pour le warning
}

//
//	Destructeur
//
T_extensive_seq::~T_extensive_seq(void)
{
}

//
//	}{	Constructeur de la classe T_generalised_union
//
T_generalised_union::T_generalised_union(T_op_result *ref_op_res,
												  T_item **adr_first,
												  T_item **adr_last,
												  T_item *father,
												  T_betree *betree,
												  T_lexem *ref_lexem)
  :T_expr(NODE_GENERALISED_UNION, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE6("T_generalised_union::T_generalised_union(%x,%x,%d,%x,%x,%x,%x,%x)",
		 ref_op_res, adr_first, adr_last, father, betree, ref_lexem) ;

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  first_expr = last_expr = NULL ;

  // On recupere les parametres
  T_item *cur_param = ref_op_res->get_in_params() ;

  while (cur_param != NULL)
	{
	  T_item *next = cur_param->get_next() ;

#ifdef DEBUG_PAREXPR
	  TRACE2("cur_param %s %s",
			 cur_param->get_class_name(),
			 cur_param->get_ref_lexem()->get_lex_ascii()) ;
#endif

	  cur_param->tail_insert((T_item **)&first_expr,
							 (T_item **)&last_expr) ;
	  cur_param->link() ;

	  // Les parametres doivent etre des expressions
	  if (cur_param->is_an_expr() == FALSE)
		{
		  syntax_expr_expected_error(cur_param, CAN_CONTINUE) ;
		}
	  else
		{
		  cur_param->make_expr()->post_check() ;
		}
	  cur_param = next ;
	}
  TRACE0("fin de T_generalised_union::T_generalised_union") ;

}

//
//	Destructeur
//
T_generalised_union::~T_generalised_union(void)
{
  TRACE1("T_generalised_union::~T_generalised_union(%x)", this) ; 
}

//
//	}{	Correction de l'arbre
//
T_item *T_generalised_union::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_generalised_union(%x)::check_tree", this) ;
#endif

  if (first_expr == NULL)
	{
	  return NULL ;
	}

  T_expr *cur_item = first_expr ;
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
	  cur_item = next ;
	}

  first_expr = tmp_first->make_expr() ;
  last_expr = tmp_last->make_expr() ;

  return this ;
  assert(ref_this == ref_this) ; // pour le warning
}

//
//	}{	Constructeur de la classe T_generalised_intersection
//
T_generalised_intersection::T_generalised_intersection(T_op_result *ref_op_res,
													   T_item **adr_first,
													   T_item **adr_last,
													   T_item *father,
													   T_betree *betree,
													   T_lexem *ref_lexem)
  :T_expr(NODE_GENERALISED_INTERSECTION,
		  adr_first,
		  adr_last,
		  father,
		  betree,
		  ref_lexem)
{
  TRACE6("T_generalised_intersection::T_gener_inter(%x,%x,%d,%x,%x,%x,%x,%x)",
		 ref_op_res, adr_first, adr_last, father, betree, ref_lexem) ;

  // Recherche de la machine et de l'operation de definition
  T_machine *mach_def ;
  T_op *op_def ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  first_expr = last_expr = NULL ;

  // On recupere les parametres
  T_item *cur_param = ref_op_res->get_in_params() ;

  while (cur_param != NULL)
	{
	  T_item *next = cur_param->get_next() ;

#ifdef DEBUG_PAREXPR
	  TRACE2("cur_param %s %s",
			 cur_param->get_class_name(),
			 cur_param->get_ref_lexem()->get_lex_ascii()) ;
#endif

	  cur_param->tail_insert((T_item **)&first_expr,
							 (T_item **)&last_expr) ;
	  cur_param->link() ;

	  // Les parametres doivent etre des expressions
	  if (cur_param->is_an_expr() == FALSE)
		{
		  syntax_expr_expected_error(cur_param, CAN_CONTINUE) ;
		}
	  else
		{
		  cur_param->make_expr()->post_check() ;
		}
	  cur_param = next ;
	}
  TRACE0("fin de T_generalised_intersection::T_generalised_intersection") ;

}

//
//	Destructeur
//
T_generalised_intersection::~T_generalised_intersection(void)
{
  TRACE1("T_generalised_intersection::~T_generalised_intersection(%x)", this) ;  
}

//
//	}{	Correction de l'arbre
//
T_item *T_generalised_intersection::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_generalised_intersection(%x)::check_tree", this) ;
#endif

  if (first_expr == NULL)
	{
	  return NULL ;
	}

  T_expr *cur_item = first_expr ;
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
	  cur_item = next ;
	}

  first_expr = tmp_first->make_expr() ;
  last_expr = tmp_last->make_expr() ;

  return this ;
  assert(ref_this == ref_this) ; // pour le warning
}

//
//	}{	Fin du fichier
//
