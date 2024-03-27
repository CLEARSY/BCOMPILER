/******************************* CLEARSY **************************************
* Fichier : $Id: c_bltin.cpp,v 2.0 2005-04-25 10:43:55 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Symboles builtin
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
RCS_ID("$Id: c_bltin.cpp,v 1.33 2005-04-25 10:43:55 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Taille de la table des symboles
static const int TSYMB_SIZE=211 ;		// taille de la table des symboles

// Forward : Typage des mots cles

static void type_keywords(void) ;

//
//	}{	Variables statiques de memorisation des mots cles
//
static T_keyword *machine_sop = NULL ;
static T_keyword *system_sop = NULL ;
#ifdef BALBULETTE
static T_keyword *model_sop = NULL ;
#endif
static T_keyword *refinement_sop = NULL ;
static T_keyword *implementation_sop = NULL ;
static T_keyword *begin_sop = NULL ;
static T_keyword *end_sop = NULL ;
static T_keyword *sees_sop = NULL ;
static T_keyword *includes_sop = NULL ;
static T_keyword *promotes_sop = NULL ;
static T_keyword *imports_sop = NULL ;
static T_keyword *refines_sop = NULL ;
static T_keyword *extends_sop = NULL ;
static T_keyword *uses_sop = NULL ;
static T_keyword *constraints_sop = NULL ;
static T_keyword *definitions_sop = NULL ;
static T_keyword *invariant_sop = NULL ;
static T_keyword *assertions_sop = NULL ;
#ifdef BALBULETTE
static T_keyword *theorems_sop = NULL ;
#endif
static T_keyword *values_sop = NULL ;
static T_keyword *sets_sop = NULL ;
static T_keyword *properties_sop = NULL ;
static T_keyword *abstract_variables_sop = NULL ;
static T_keyword *abstract_variables2_sop = NULL ;
static T_keyword *abstract_variables3_sop = NULL ;
static T_keyword *concrete_variables_sop = NULL ;
static T_keyword *concrete_variables2_sop = NULL ;
static T_keyword *abstract_constants_sop = NULL ;
static T_keyword *abstract_constants2_sop = NULL ;
static T_keyword *concrete_constants_sop = NULL ;
static T_keyword *concrete_constants2_sop = NULL ;
static T_keyword *concrete_constants3_sop = NULL ;
static T_keyword *initialisation_sop = NULL ;
static T_keyword *operations_sop = NULL ;
static T_keyword *events_sop = NULL ;
static T_keyword *local_operations_sop = NULL ;
static T_keyword *var_sop = NULL ;
static T_keyword *in_sop = NULL ;
static T_keyword *if_sop = NULL ;
static T_keyword *then_sop = NULL ;
static T_keyword *elsif_sop = NULL ;
static T_keyword *else_sop = NULL ;
static T_keyword *case_sop = NULL ;
static T_keyword *either_sop = NULL ;
static T_keyword *or_sop = NULL ;
static T_keyword *skip_sop = NULL ;
static T_keyword *pre_sop = NULL ;
static T_keyword *choice_sop = NULL ;
static T_keyword *select_sop = NULL ;
static T_keyword *when_sop = NULL ;
static T_keyword *while_sop = NULL ;
static T_keyword *variant_sop = NULL ;
static T_keyword *let_sop = NULL ;
static T_keyword *be_sop = NULL ;
static T_keyword *assert_sop = NULL ;
static T_keyword *witness_sop = NULL ;
static T_keyword *of_sop = NULL ;
static T_keyword *do_sop = NULL ;
static T_keyword *any_sop = NULL ;
static T_keyword *where_sop = NULL ;
static T_keyword *rec_sop = NULL ;
static T_keyword *struct_sop = NULL ;
static T_ident *bool_sop = NULL ;
static T_ident *mod_sop = NULL ;
static T_ident *succ_sop = NULL ;
static T_ident *pred_sop = NULL ;
static T_ident *max_sop = NULL ;
static T_ident *min_sop = NULL ;
static T_ident *real_sop = NULL ;
static T_ident *floor_sop = NULL ;
static T_ident *ceiling_sop = NULL ;
static T_ident *card_sop = NULL ;
static T_ident *sigma_sop = NULL ;
static T_ident *pi_sop = NULL ;
static T_ident *gunion_sop = NULL ;
static T_ident *ginter_sop = NULL ;
static T_ident *qunion_sop = NULL ;
static T_ident *qinter_sop = NULL ;
static T_ident *id_sop = NULL ;
static T_ident *prj1_sop = NULL ;
static T_ident *prj2_sop = NULL ;
static T_ident *dom_sop = NULL ;
static T_ident *ran_sop = NULL ;
static T_ident *size_sop = NULL ;
static T_ident *first_sop = NULL ;
static T_ident *last_sop = NULL ;
static T_ident *front_sop = NULL ;
static T_ident *tail_sop = NULL ;
static T_ident *rev_sop = NULL ;
static T_ident *l_or_sop = NULL ;
static T_ident *l_not_sop = NULL ;
static T_ident *nat_sop = NULL ;
static T_ident *nat1_sop = NULL ;
static T_ident *int_sop = NULL ;
static T_ident *true_sop = NULL ;
static T_ident *false_sop = NULL ;
static T_ident *maxint_sop = NULL ;
static T_ident *minint_sop = NULL ;
static T_ident *integer_sop = NULL ;
static T_ident *natural_sop = NULL ;
static T_ident *natural1_sop = NULL ;
static T_ident *bool_type_sop = NULL ;
static T_ident *string_type_sop = NULL ;
static T_ident *real_type_sop = NULL ;
static T_ident *float_type_sop = NULL ;
static T_ident *pow_sop = NULL ;
static T_ident *pow1_sop = NULL ;
static T_ident *fin_sop = NULL ;
static T_ident *fin1_sop = NULL ;
static T_ident *trans_func_sop = NULL ;
static T_ident *trans_rel_sop = NULL ;
static T_ident *sequence_sop = NULL ;
static T_ident *non_empty_sequence_sop = NULL ;
static T_ident *injective_seq_sop = NULL ;
static T_ident *non_empty_injective_seq_sop = NULL ;
static T_ident *permutation_sop = NULL ;
static T_ident *generalised_concat_sop = NULL ;
static T_ident *iterate_sop = NULL ;
static T_ident *closure_sop = NULL ;
static T_ident *closure1_sop = NULL ;

static T_ident *tree_sop = NULL ;
static T_ident *btree_sop = NULL ;
static T_ident *const_sop = NULL ;
static T_ident *top_sop = NULL ;
static T_ident *sons_sop = NULL ;
static T_ident *prefix_sop = NULL ;
static T_ident *postfix_sop = NULL ;
static T_ident *sizet_sop = NULL ;
static T_ident *mirror_sop = NULL ;
static T_ident *rank_sop = NULL ;
static T_ident *father_sop = NULL ;
static T_ident *son_sop = NULL ;
static T_ident *subtree_sop = NULL ;
static T_ident *arity_sop = NULL ;
static T_ident *bin_sop = NULL ;
static T_ident *left_sop = NULL ;
static T_ident *right_sop = NULL ;
static T_ident *infix_sop = NULL ;
static T_keyword *jumpif_sop = NULL ;
static T_keyword *label_sop = NULL ;
static T_keyword *to_sop = NULL ;
static T_ident *ref_sop = NULL ;

static T_integer *minint_value_sop = NULL ;
static T_integer *maxint_value_sop = NULL ;

//
//	}{	Methodes d'acces
//

// Une petite macro pour aider
static T_list_link *make_link(T_lexem *ref_lexem,
									   T_betree *betree,
									   T_ident *ref_ident)
{
  TRACE1("make_link ref_ident %x", ref_ident) ;
  TRACE5("make_link(%x, %x, %x:%x)",
		 ref_lexem,
		 betree,
		 ref_ident,
		 ref_ident->get_name(),
		 (ref_ident->get_name() == NULL) ? NULL : ref_ident->get_name()->get_value()) ;
  TRACE4("make_link(%x, %x, %x:%s)",
		 ref_lexem,
		 betree,
		 ref_ident,
		 ref_ident->get_name()->get_value()) ;
  //  ref_ident->link() ;
  T_ident *clone = new T_ident(ref_ident, NULL, NULL, NULL) ;
  clone->set_ref_lexem(ref_lexem) ;

  if (clone->get_B_type() != NULL)
	{
	  clone->get_B_type()->set_ref_lexem(ref_lexem) ;
	}
  clone->set_betree(betree) ;

  T_list_link *res = new T_list_link(clone,
									 LINK_BUILTIN,
									 NULL,
									 NULL,
									 NULL,
									 NULL,
									 ref_lexem) ;
  res->set_do_free() ;
  return res ;
}


T_list_link *link_builtin_bool(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, bool_sop) ;
}

T_list_link *link_builtin_mod(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, mod_sop) ;
}

T_list_link *link_builtin_succ(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, succ_sop) ;
}

T_list_link *link_builtin_pred(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, pred_sop) ;
}

T_list_link *link_builtin_max(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, max_sop) ;
}

T_list_link *link_builtin_min(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, min_sop) ;
}

T_list_link *link_builtin_real(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, real_sop) ;
}

T_list_link *link_builtin_floor(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, floor_sop) ;
}

T_list_link *link_builtin_ceiling(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, ceiling_sop) ;
}

T_list_link *link_builtin_card(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, card_sop) ;
}

T_list_link *link_builtin_sigma(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, sigma_sop) ;
}

T_list_link *link_builtin_pi(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, pi_sop) ;
}

T_list_link *link_builtin_gunion(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, gunion_sop) ;
}

T_list_link *link_builtin_ginter(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, ginter_sop) ;
}

T_list_link *link_builtin_qunion(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, qunion_sop) ;
}

T_list_link *link_builtin_qinter(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, qinter_sop) ;
}

T_list_link *link_builtin_id(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, id_sop) ;
}

T_list_link *link_builtin_prj1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, prj1_sop) ;
}

T_list_link *link_builtin_prj2(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, prj2_sop) ;
}

T_list_link *link_builtin_dom(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, dom_sop) ;
}

T_list_link *link_builtin_ran(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, ran_sop) ;
}

T_list_link *link_builtin_size(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, size_sop) ;
}

T_list_link *link_builtin_first(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, first_sop) ;
}

T_list_link *link_builtin_last(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, last_sop) ;
}

T_list_link *link_builtin_front(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, front_sop) ;
}

T_list_link *link_builtin_tail(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, tail_sop) ;
}

T_list_link *link_builtin_rev(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, rev_sop) ;
}

T_list_link *link_builtin_l_or(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, l_or_sop) ;
}

T_list_link *link_builtin_l_not(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, l_not_sop) ;
}

T_list_link *link_builtin_nat(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, nat_sop) ;
}

T_list_link *link_builtin_nat1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, nat1_sop) ;
}

T_list_link *link_builtin_int(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, int_sop) ;
}

T_list_link *link_builtin_true(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, true_sop) ;
}

T_list_link *link_builtin_false(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, false_sop) ;
}

T_list_link *link_builtin_maxint(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, maxint_sop) ;
}

T_list_link *link_builtin_minint(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, minint_sop) ;
}

T_list_link *link_builtin_integer(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, integer_sop) ;
}

T_list_link *link_builtin_natural(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, natural_sop) ;
}

T_list_link *link_builtin_natural1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, natural1_sop) ;
}

T_list_link *link_builtin_bool_type(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, bool_type_sop) ;
}

T_list_link *link_builtin_string_type(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, string_type_sop) ;
}

T_list_link *link_builtin_real_type(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, real_type_sop) ;
}

T_list_link *link_builtin_float_type(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, float_type_sop) ;
}

T_list_link *link_builtin_pow(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, pow_sop) ;
}

T_list_link *link_builtin_pow1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, pow1_sop) ;
}

T_list_link *link_builtin_fin(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, fin_sop) ;
}

T_list_link *link_builtin_fin1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, fin1_sop) ;
}

T_list_link *link_builtin_trans_func(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, trans_func_sop) ;
}

T_list_link *link_builtin_trans_rel(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, trans_rel_sop) ;
}

T_list_link *link_builtin_sequence(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, sequence_sop) ;
}

T_list_link *link_builtin_non_empty_sequence(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, non_empty_sequence_sop) ;
}

T_list_link *link_builtin_injective_seq(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, injective_seq_sop) ;
}

T_list_link *link_builtin_non_empty_injective_seq(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, non_empty_injective_seq_sop) ;
}

T_list_link *link_builtin_permutation(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, permutation_sop) ;
}

T_list_link *link_builtin_generalised_concat(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, generalised_concat_sop) ;
}

T_list_link *link_builtin_iterate(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, iterate_sop) ;
}

T_list_link *link_builtin_closure(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, closure_sop) ;
}

T_list_link *link_builtin_closure1(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, closure1_sop) ;
}

// GP 8/02/99 Pour les arbres
T_list_link *link_builtin_tree(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, tree_sop) ;
}

T_list_link *link_builtin_btree(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, btree_sop) ;
}

T_list_link *link_builtin_const(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, const_sop) ;
}

T_list_link *link_builtin_top(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, top_sop) ;
}

T_list_link *link_builtin_sons(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, sons_sop) ;
}

T_list_link *link_builtin_prefix(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, prefix_sop) ;
}

T_list_link *link_builtin_postfix(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, postfix_sop) ;
}

T_list_link *link_builtin_sizet(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, sizet_sop) ;
}

T_list_link *link_builtin_mirror(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, mirror_sop) ;
}

T_list_link *link_builtin_rank(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, rank_sop) ;
}

T_list_link *link_builtin_father(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, father_sop) ;
}

T_list_link *link_builtin_son(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, son_sop) ;
}

T_list_link *link_builtin_subtree(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, subtree_sop) ;
}

T_list_link *link_builtin_arity(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, arity_sop) ;
}

T_list_link *link_builtin_bin(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, bin_sop) ;
}

T_list_link *link_builtin_left(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, left_sop) ;
}

T_list_link *link_builtin_right(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, right_sop) ;
}

T_list_link *link_builtin_infix(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, infix_sop) ;
}


T_list_link * link_builtin_ref(T_lexem *ref_lexem, T_betree *betree)
{
  return make_link(ref_lexem, betree, ref_sop) ;
}

//
//	}{	T_keyword
//
T_keyword::T_keyword(const char *new_value,
					 size_t new_len,
					 int h,
					 T_symbol *cur_symbol,
					 T_symbol *prev_symbol,
					 T_lex_type new_lex_type)
  : T_symbol(new_value, new_len, h, cur_symbol, prev_symbol)
{
  //  TRACE2("T_keyword(%x)::T_keyword(%s)",
  //	 this, (new_value == NULL) ? "" : new_value) ;
  lex_type = new_lex_type ;
  // Il faut corriger le node_type mis par defaut a NODE_SYMBOL
  set_node_type(NODE_KEYWORD) ;
}

//
//	Destructeur
//
T_keyword::~T_keyword()
{
}

//
//	}{	Initialisation de la table des mots cles
//
void add_keywords(void)
{
  machine_sop = insert_builtin(L_MACHINE, 			   	"MACHINE") ;
  system_sop = insert_builtin(L_SYSTEM, 			   	"SYSTEM") ;
#ifdef BALBULETTE
  model_sop = insert_builtin(L_MODEL,	 			   	"MODEL") ;
#endif
  refinement_sop = insert_builtin(L_REFINEMENT, 		   	"REFINEMENT") ;
  implementation_sop = insert_builtin(L_IMPLEMENTATION,  	"IMPLEMENTATION") ;
  begin_sop = insert_builtin(L_BEGIN,						"BEGIN") ;
  end_sop = insert_builtin(L_END,							"END") ;
  sees_sop = insert_builtin(L_SEES,						"SEES") ;
  includes_sop = insert_builtin(L_INCLUDES,			   	"INCLUDES") ;
  promotes_sop = insert_builtin(L_PROMOTES,			   	"PROMOTES") ;
  imports_sop = insert_builtin(L_IMPORTS,					"IMPORTS") ;
  refines_sop = insert_builtin(L_REFINES,					"REFINES") ;
  extends_sop = insert_builtin(L_EXTENDS,					"EXTENDS") ;
  uses_sop = insert_builtin(L_USES,						"USES") ;
  constraints_sop = insert_builtin(L_CONSTRAINTS,	   		"CONSTRAINTS") ;
  definitions_sop = insert_builtin(L_DEFINITIONS,	   		"DEFINITIONS") ;
  invariant_sop = insert_builtin(L_INVARIANT,		   		"INVARIANT") ;
  assertions_sop = insert_builtin(L_ASSERTIONS,	   		"ASSERTIONS") ;
#ifdef BALBULETTE
  theorems_sop = insert_builtin(L_THEOREMS,				   	"THEOREMS") ;
#endif
  values_sop = insert_builtin(L_VALUES,					"VALUES") ;
  sets_sop = insert_builtin(L_SETS,						"SETS") ;
  properties_sop = insert_builtin(L_PROPERTIES,  			"PROPERTIES") ;
  //
  //	attention !
  //
  // pour les constantes, CONSTANTS = VISIBLE_CONSTANTS = CONCRETE_CONSTANTS
  // pour les variables,  variables = HIDDEN_VARIABLES
  //
  concrete_variables_sop = insert_builtin(L_CONCRETE_VARIABLES, "VISIBLE_VARIABLES") ;
  concrete_variables2_sop = insert_builtin(L_CONCRETE_VARIABLES,"CONCRETE_VARIABLES");

  abstract_variables_sop = insert_builtin(L_ABSTRACT_VARIABLES, "VARIABLES") ;
  abstract_variables2_sop = insert_builtin(L_ABSTRACT_VARIABLES,"HIDDEN_VARIABLES") ;
  abstract_variables3_sop = insert_builtin(L_ABSTRACT_VARIABLES,"ABSTRACT_VARIABLES");

  concrete_constants_sop = insert_builtin(L_CONCRETE_CONSTANTS, "CONSTANTS") ;
  concrete_constants2_sop = insert_builtin(L_CONCRETE_CONSTANTS,"VISIBLE_CONSTANTS") ;
  concrete_constants3_sop = insert_builtin(L_CONCRETE_CONSTANTS,"CONCRETE_CONSTANTS");

  abstract_constants_sop = insert_builtin(L_ABSTRACT_CONSTANTS, "HIDDEN_CONSTANTS") ;
  abstract_constants2_sop = insert_builtin(L_ABSTRACT_CONSTANTS,"ABSTRACT_CONSTANTS");

  initialisation_sop = insert_builtin(L_INITIALISATION,  	"INITIALISATION") ;
  operations_sop = insert_builtin(L_OPERATIONS,		   	"OPERATIONS") ;
  events_sop = insert_builtin(L_EVENTS,				   	"EVENTS") ;
  local_operations_sop = insert_builtin(L_LOCAL_OPERATIONS,
										"LOCAL_OPERATIONS") ;
  var_sop = insert_builtin(L_VAR,							"VAR") ;
  in_sop = insert_builtin(L_IN,							"IN") ;
  if_sop = insert_builtin(L_IF,							"IF") ;
  then_sop = insert_builtin(L_THEN,						"THEN") ;
  elsif_sop = insert_builtin(L_ELSIF,						"ELSIF") ;
  else_sop = insert_builtin(L_ELSE,						"ELSE") ;
  case_sop = insert_builtin(L_CASE,						"CASE") ;
  either_sop = insert_builtin(L_EITHER,					"EITHER") ;
  or_sop = insert_builtin(L_OR,							"OR") ;
  skip_sop = insert_builtin(L_SKIP,						"skip") ;
  pre_sop = insert_builtin(L_PRE,							"PRE") ;
  choice_sop = insert_builtin(L_CHOICE,					"CHOICE") ;
  select_sop = insert_builtin(L_SELECT,					"SELECT") ;
  when_sop = insert_builtin(L_WHEN,						"WHEN") ;
  while_sop = insert_builtin(L_WHILE,						"WHILE") ;
  variant_sop = insert_builtin(L_VARIANT,					"VARIANT") ;
  let_sop = insert_builtin(L_LET,							"LET") ;
  be_sop = insert_builtin(L_BE,							"BE") ;
  assert_sop = insert_builtin(L_ASSERT,					"ASSERT") ;
  witness_sop = insert_builtin(L_WITNESS,					"WITNESS") ;
  of_sop = insert_builtin(L_OF,							"OF") ;
  do_sop = insert_builtin(L_DO,							"DO") ;
  any_sop = insert_builtin(L_ANY,							"ANY") ;
  where_sop = insert_builtin(L_WHERE,   					"WHERE") ;
  rec_sop = insert_builtin(L_REC, "rec") ;
  struct_sop = insert_builtin(L_STRUCT, "struct") ;
  bool_sop 		= new T_ident(insert_builtin(L_BOOL,	   	   	"bool")) ;
  mod_sop 		= new T_ident(insert_builtin(L_MOD,	   			"mod")) ;

  succ_sop 		= new T_ident(insert_builtin(L_SUCC,	   	   	"succ")) ;
  pred_sop 		= new T_ident(insert_builtin(L_PRED,  		   	"pred")) ;
  max_sop 		= new T_ident(insert_builtin(L_MAX,   		   	"max")) ;
  min_sop 		= new T_ident(insert_builtin(L_MIN,		   		"min")) ;
  card_sop 		= new T_ident(insert_builtin(L_CARD,		    "card")) ;
  real_sop 		= new T_ident(insert_builtin(L_REALOP,		    "real")) ;
  floor_sop 	= new T_ident(insert_builtin(L_FLOOR,		    "floor")) ;
  ceiling_sop 	= new T_ident(insert_builtin(L_CEILING,		    "ceiling")) ;
  sigma_sop		= new T_ident(insert_builtin(L_SIGMA,		   	"SIGMA")) ;
  pi_sop 			= new T_ident(insert_builtin(L_PI,			   	"PI")) ;
  gunion_sop 		= new T_ident(insert_builtin(L_GUNION,		   	"union")) ;
  ginter_sop 		= new T_ident(insert_builtin(L_GINTER,		   	"inter")) ;
  qunion_sop 		= new T_ident(insert_builtin(L_QUNION,		    "UNION")) ;
  qinter_sop 		= new T_ident(insert_builtin(L_QINTER,		   	"INTER")) ;
  id_sop 			= new T_ident(insert_builtin(L_ID,			  	"id")) ;
  prj1_sop 		= new T_ident(insert_builtin(L_PRJ1,	      	"prj1")) ;
  prj2_sop 		= new T_ident(insert_builtin(L_PRJ2,		   	"prj2")) ;
  dom_sop 		= new T_ident(insert_builtin(L_DOM,	   			"dom")) ;
  ran_sop 		= new T_ident(insert_builtin(L_RAN,	   			"ran")) ;
  size_sop 		= new T_ident(insert_builtin(L_SIZE,	   	  	"size")) ;
  first_sop 		= new T_ident(insert_builtin(L_FIRST,	   	   	"first")) ;
  last_sop 		= new T_ident(insert_builtin(L_LAST,	   	   	"last")) ;
  front_sop 		= new T_ident(insert_builtin(L_FRONT,		   	"front")) ;
  tail_sop 		= new T_ident(insert_builtin(L_TAIL,		   	"tail")) ;
  rev_sop 		= new T_ident(insert_builtin(L_REV,			   	"rev")) ;
  l_or_sop 		= new T_ident(insert_builtin(L_L_OR,		   	"or")) ;
  l_not_sop 	= new T_ident(insert_builtin(L_L_NOT,	       	"not")) ;
  nat_sop 		= new T_ident(insert_builtin(L_NAT,	   			"NAT")) ;
  nat1_sop 		= new T_ident(insert_builtin(L_NAT1,	  	  	"NAT1")) ;
  int_sop 		= new T_ident(insert_builtin(L_INT,	   			"INT")) ;
  true_sop 		= new T_ident(insert_builtin(L_TRUE,	   	   	"TRUE")) ;
  false_sop 	= new T_ident(insert_builtin(L_FALSE,      		"FALSE")) ;
  maxint_sop 	= new T_ident(insert_builtin(L_MAXINT,	   		"MAXINT")) ;
  minint_sop 	= new T_ident(insert_builtin(L_MININT,     		"MININT")) ;
  integer_sop 	= new T_ident(insert_builtin(L_INTEGER,	      	"INTEGER")) ;
  natural_sop 	= new T_ident(insert_builtin(L_NATURAL,	  	   	"NATURAL")) ;
  natural1_sop 	= new T_ident(insert_builtin(L_NATURAL1,	   	"NATURAL1")) ;
  bool_type_sop 	= new T_ident(insert_builtin(L_BOOL_TYPE,      	"BOOL")) ;
  string_type_sop = new T_ident(insert_builtin(L_STRING_TYPE,    	"STRING")) ;
  real_type_sop = new T_ident(insert_builtin(L_REAL_TYPE,    	"REAL")) ;
  float_type_sop = new T_ident(insert_builtin(L_FLOAT_TYPE,    	"FLOAT")) ;
  pow_sop 		= new T_ident(insert_builtin(L_POW,		   		"POW")) ;
  pow1_sop 		= new T_ident(insert_builtin(L_POW1,	   	  	"POW1")) ;
  fin_sop 		= new T_ident(insert_builtin(L_FIN,	   			"FIN")) ;
  fin1_sop 		= new T_ident(insert_builtin(L_FIN1,	   	  	"FIN1")) ;
  trans_func_sop 	= new T_ident(insert_builtin(L_TRANS_FUNC,	   	"fnc")) ;
  trans_rel_sop 	= new T_ident(insert_builtin(L_TRANS_REL,	   	"rel")) ;
  sequence_sop 	= new T_ident(insert_builtin(L_SEQUENCE,	   	"seq")) ;
  non_empty_sequence_sop 	= new T_ident(insert_builtin(L_NON_EMPTY_SEQUENCE,
														 "seq1")) ;
  injective_seq_sop =	new T_ident(insert_builtin(L_INJECTIVE_SEQ, "iseq")) ;
  non_empty_injective_seq_sop = new T_ident(insert_builtin(L_NON_EMPTY_INJECTIVE_SEQ,
														   "iseq1")) ;
  permutation_sop = new T_ident(insert_builtin(L_PERMUTATION,	   	"perm")) ;
  generalised_concat_sop	= new T_ident(insert_builtin(L_GENERALISED_CONCAT,
														 "conc")) ;
  iterate_sop 	= new T_ident(insert_builtin(L_ITERATE,   	   	"iterate")) ;
  closure_sop 	= new T_ident(insert_builtin(L_CLOSURE,   	   	"closure")) ;
  closure1_sop 	= new T_ident(insert_builtin(L_CLOSURE1, 	   	"closure1")) ;

  tree_sop      = new T_ident(insert_builtin(L_TREE,            "tree")) ;
  btree_sop     = new T_ident(insert_builtin(L_BTREE,           "btree")) ;
  const_sop     = new T_ident(insert_builtin(L_CONST,           "const")) ;
  top_sop       = new T_ident(insert_builtin(L_TOP,             "top")) ;
  sons_sop      = new T_ident(insert_builtin(L_SONS,            "sons")) ;
  prefix_sop    = new T_ident(insert_builtin(L_PREFIX,          "prefix")) ;
  postfix_sop   = new T_ident(insert_builtin(L_POSTFIX,         "postfix")) ;
  sizet_sop     = new T_ident(insert_builtin(L_SIZET,           "sizet")) ;
  mirror_sop    = new T_ident(insert_builtin(L_MIRROR,          "mirror")) ;
  rank_sop      = new T_ident(insert_builtin(L_RANK,            "rank")) ;
  father_sop    = new T_ident(insert_builtin(L_FATHER,          "father")) ;
  son_sop       = new T_ident(insert_builtin(L_SON,             "son")) ;
  subtree_sop   = new T_ident(insert_builtin(L_SUBTREE,         "subtree")) ;
  arity_sop     = new T_ident(insert_builtin(L_ARITY,           "arity")) ;
  bin_sop       = new T_ident(insert_builtin(L_BIN,             "bin")) ;
  left_sop      = new T_ident(insert_builtin(L_LEFT,            "left")) ;
  right_sop     = new T_ident(insert_builtin(L_RIGHT,           "right")) ;
  infix_sop     = new T_ident(insert_builtin(L_INFIX,           "infix")) ;
  jumpif_sop = insert_builtin(L_JUMPIF,						"JUMPIF") ;
  label_sop = insert_builtin(L_LABEL,					"LABEL") ;
  to_sop = insert_builtin(L_TO,						"TO") ;
  ref_sop 	= new T_ident(insert_builtin(L_REF,		    "ref")) ;

  // Creation de MININT/MAXINT parametrables : pas maintenant mais sur demande
  // Car : peut necessiter de connaitre le fichier de ressources, qui n'est pas
  // connu au moment de l'initialisation
  // Donc on n'ecrit surtout pas ici :
  // minint_value_sop = new T_integer(get_minint_value()) ;
  // maxint_value_sop = new T_integer(get_maxint_value()) ;

  if (get_dep_mode() == TRUE)
	{
	  // On s'arrete la en mode dependances
	  return ;
	}

  type_keywords() ;
}

// Suppression des builtins de la table des symboles
void delete_keywords(void)
{
  TRACE0("debut delete_keywords") ;
  ENTER_TRACE ;
  object_unlink(machine_sop) ;
  object_unlink(system_sop) ;
#ifdef BALBULETTE
  object_unlink(model_sop) ;
#endif
  object_unlink(refinement_sop) ;
  object_unlink(implementation_sop) ;
  object_unlink(begin_sop) ;
  object_unlink(end_sop) ;
  object_unlink(sees_sop) ;
  object_unlink(includes_sop) ;
  object_unlink(promotes_sop) ;
  object_unlink(imports_sop) ;
  object_unlink(refines_sop) ;
  object_unlink(extends_sop) ;
  object_unlink(uses_sop) ;
  object_unlink(constraints_sop) ;
  object_unlink(definitions_sop) ;
  object_unlink(invariant_sop) ;
  object_unlink(assertions_sop) ;
#ifdef BALBULETTE
  object_unlink(theorems_sop) ;
#endif
  object_unlink(values_sop) ;
  object_unlink(sets_sop) ;
  object_unlink(properties_sop) ;
  object_unlink(concrete_variables_sop) ;
  object_unlink(concrete_variables2_sop) ;
  object_unlink(abstract_variables_sop) ;
  object_unlink(abstract_variables2_sop) ;
  object_unlink(abstract_variables3_sop) ;
  object_unlink(concrete_constants_sop) ;
  object_unlink(concrete_constants2_sop) ;
  object_unlink(concrete_constants3_sop) ;
  object_unlink(abstract_constants_sop) ;
  object_unlink(abstract_constants2_sop) ;
  object_unlink(initialisation_sop) ;
  object_unlink(operations_sop) ;
  object_unlink(events_sop) ;
  object_unlink(local_operations_sop) ;
  object_unlink(var_sop) ;
  object_unlink(in_sop) ;
  object_unlink(if_sop) ;
  object_unlink(then_sop) ;
  object_unlink(elsif_sop) ;
  object_unlink(else_sop) ;
  object_unlink(case_sop) ;
  object_unlink(either_sop) ;
  object_unlink(or_sop) ;
  object_unlink(skip_sop) ;
  object_unlink(pre_sop) ;
  object_unlink(choice_sop) ;
  object_unlink(select_sop) ;
  object_unlink(when_sop) ;
  object_unlink(while_sop) ;
  object_unlink(variant_sop) ;
  object_unlink(let_sop) ;
  object_unlink(be_sop) ;
  object_unlink(assert_sop) ;
  object_unlink(witness_sop) ;
  object_unlink(of_sop) ;
  object_unlink(do_sop) ;
  object_unlink(any_sop) ;
  object_unlink(where_sop) ;
  object_unlink(bool_sop) ;
  object_unlink(mod_sop) ;
  object_unlink(succ_sop) ;
  object_unlink(pred_sop) ;
  object_unlink(max_sop) ;
  object_unlink(min_sop) ;
  object_unlink(card_sop) ;
  object_unlink(real_sop) ;
  object_unlink(floor_sop) ;
  object_unlink(ceiling_sop) ;
  object_unlink(sigma_sop) ;
  object_unlink(pi_sop) ;
  object_unlink(gunion_sop) ;
  object_unlink(ginter_sop) ;
  object_unlink(qunion_sop) ;
  object_unlink(qinter_sop) ;
  object_unlink(id_sop) ;
  object_unlink(prj1_sop) ;
  object_unlink(prj2_sop) ;
  object_unlink(dom_sop) ;
  object_unlink(ran_sop) ;
  object_unlink(size_sop) ;
  object_unlink(first_sop) ;
  object_unlink(last_sop) ;
  object_unlink(front_sop) ;
  object_unlink(tail_sop) ;
  object_unlink(rev_sop) ;
  object_unlink(l_or_sop) ;
  object_unlink(l_not_sop) ;
  object_unlink(nat_sop) ;
  object_unlink(nat1_sop) ;
  object_unlink(int_sop) ;
  object_unlink(true_sop) ;
  object_unlink(false_sop) ;
  object_unlink(maxint_sop) ;
  object_unlink(minint_sop) ;
  object_unlink(integer_sop) ;
  object_unlink(natural_sop) ;
  object_unlink(natural1_sop) ;
  object_unlink(bool_type_sop) ;
  object_unlink(string_type_sop) ;
  object_unlink(real_type_sop) ;
  object_unlink(float_type_sop) ;
  object_unlink(pow_sop) ;
  object_unlink(pow1_sop) ;
  object_unlink(fin_sop) ;
  object_unlink(fin1_sop) ;
  object_unlink(trans_func_sop) ;
  object_unlink(trans_rel_sop) ;
  object_unlink(sequence_sop) ;
  object_unlink(non_empty_sequence_sop) ;
  object_unlink(injective_seq_sop) ;
  object_unlink(non_empty_injective_seq_sop) ;
  object_unlink(permutation_sop) ;
  object_unlink(generalised_concat_sop) ;
  object_unlink(iterate_sop) ;
  object_unlink(closure_sop) ;
  object_unlink(closure1_sop) ;

  object_unlink(tree_sop) ;
  object_unlink(btree_sop) ;
  object_unlink(const_sop) ;
  object_unlink(top_sop) ;
  object_unlink(sons_sop) ;
  object_unlink(prefix_sop) ;
  object_unlink(postfix_sop) ;
  object_unlink(sizet_sop) ;
  object_unlink(mirror_sop) ;
  object_unlink(rank_sop) ;
  object_unlink(father_sop) ;
  object_unlink(son_sop) ;
  object_unlink(subtree_sop) ;
  object_unlink(arity_sop) ;
  object_unlink(bin_sop) ;
  object_unlink(left_sop) ;
  object_unlink(right_sop) ;
  object_unlink(infix_sop) ;
  object_unlink(jumpif_sop) ;
  object_unlink(label_sop) ;
  object_unlink(to_sop) ;
  object_unlink(ref_sop) ;

  EXIT_TRACE ;
  TRACE0("fin delete_keywords") ;
}

// Typage des mots cles
static void type_keywords(void)
{
  TRACE0("Debut type_keywords") ;
  ENTER_TRACE ;
#define TROIS_NULL NULL, NULL, NULL
#define CINQ_NULL NULL, NULL, NULL, NULL, NULL

#define SET_PREDEFINED_TYPE(x, y) \
  x->set_B_type(new T_predefined_type(y, CINQ_NULL), NULL) ;

#define SET_SETOF_TYPE(x, y, z, t) \
	x->set_B_type(new T_setof_type( \
								   new T_predefined_type(y, z, t, TROIS_NULL), TROIS_NULL),\
                  NULL) ;

  SET_PREDEFINED_TYPE(minint_sop, BTYPE_INTEGER) ;
  SET_PREDEFINED_TYPE(maxint_sop, BTYPE_INTEGER) ;
  SET_PREDEFINED_TYPE(false_sop, BTYPE_BOOL) ;
  SET_PREDEFINED_TYPE(true_sop, BTYPE_BOOL) ;

  // Les types entiers
  T_literal_integer *zero ;
  T_literal_integer *one ;
  zero = new T_literal_integer(0, NULL, NULL, NULL, NULL, NULL) ;
  one = new T_literal_integer(1, NULL, NULL, NULL, NULL, NULL) ;

  SET_SETOF_TYPE(nat_sop, 		BTYPE_INTEGER, zero,		maxint_sop) ;
  SET_SETOF_TYPE(nat1_sop, 		BTYPE_INTEGER, one,			maxint_sop) ;
  SET_SETOF_TYPE(natural_sop, 	BTYPE_INTEGER, NULL, 		NULL) ;
  SET_SETOF_TYPE(natural1_sop,	BTYPE_INTEGER, NULL, 		NULL) ;
  SET_SETOF_TYPE(integer_sop,		BTYPE_INTEGER, NULL, 		NULL) ;
  SET_SETOF_TYPE(int_sop, 		BTYPE_INTEGER, minint_sop,	maxint_sop) ;

  // Le type booleen
  SET_SETOF_TYPE(bool_type_sop,	BTYPE_BOOL,    zero,	 	one) ;

  // Le type chaine
  SET_SETOF_TYPE(string_type_sop, BTYPE_STRING,  NULL,		NULL) ;

  // Le type réel
  SET_SETOF_TYPE(real_type_sop, BTYPE_REAL,  NULL,		NULL) ;

  // Le type float
  SET_SETOF_TYPE(float_type_sop, BTYPE_FLOAT,  NULL,		NULL) ;

  // Les fonctions de base succ, pred
  T_type *tint_1 = new T_predefined_type(BTYPE_INTEGER, CINQ_NULL) ;
  T_type *tint_2 = new T_predefined_type(BTYPE_INTEGER, CINQ_NULL) ;
  T_product_type *prod = new T_product_type(tint_1, tint_2, NULL, NULL, NULL) ;
  tint_1->unlink() ;
  tint_2->unlink() ;
  T_type *set = new T_setof_type(prod, NULL, NULL, NULL) ;
  prod->unlink() ;
  succ_sop->set_B_type(set, NULL) ;

  T_type *set2 = set->clone_type(NULL, NULL, NULL) ;
  pred_sop->set_B_type(set2, NULL) ;

  TRACE0("Fin type_keywords") ;
  EXIT_TRACE ;
}

T_ident *get_builtin_pred(void)
{
  return pred_sop ;
}

T_ident *get_builtin_succ(void)
{
  return succ_sop ;
}

T_ident *get_builtin_INT(void)
{
  return int_sop ;
}

T_ident *get_builtin_NAT(void)
{
  return nat_sop ;
}

T_ident *get_builtin_NAT1(void)
{
  return nat1_sop ;
}

T_ident *get_builtin_BOOL(void)
{
  return bool_type_sop ;
}

T_ident *get_builtin_REAL(void)
{
  return real_type_sop ;
}

T_ident *get_builtin_FLOAT(void)
{
  return float_type_sop ;
}

T_ident *get_builtin_STRING(void)
{
  return string_type_sop ;
}

T_ident *get_builtin_MAXINT(void)
{
  return maxint_sop ;
}

T_ident *get_builtin_MININT(void)
{
  return minint_sop ;
}

T_ident *get_builtin_TRUE(void)
{
  return true_sop ;
}

T_integer *get_builtin_MAXINT_value(void)
{
  // Creation de MININT/MAXINT parametrables : pas maintenant mais sur demande
  // Car : peut necessiter de connaitre le fichier de ressources, qui n'est pas
  // connu au moment de l'initialisation
  if (maxint_value_sop == NULL)
	{
	  maxint_value_sop = new T_integer(get_maxint_value()) ;
	}
  return maxint_value_sop ;
}

T_integer *get_builtin_MININT_value(void)
{
  // Creation de MININT/MAXINT parametrables : pas maintenant mais sur demande
  // Car : peut necessiter de connaitre le fichier de ressources, qui n'est pas
  // connu au moment de l'initialisation
  if (minint_value_sop == NULL)
	{
	  minint_value_sop = new T_integer(get_minint_value()) ;
	}
  return minint_value_sop ;
}

#ifdef B0C

T_ident *get_builtin_INTEGER(void)
{
  return integer_sop ;
}

T_ident *get_builtin_NATURAL(void)
{
  return natural_sop ;
}

T_ident *get_builtin_NATURAL1(void)
{
  return natural1_sop ;
}

T_ident *get_builtin_bool(void)
{
  return bool_sop ;
}


#endif // B0C
//
//	}{	Fin du fichier
//
