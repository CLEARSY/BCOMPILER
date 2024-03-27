/******************************* CLEARSY **************************************
* Fichier : $Id: c_bltin.h,v 2.0 1999-08-09 15:33:48 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des builtins
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
#ifndef _C_BLTIN_H_
#define _C_BLTIN_H_

class T_list_link ;

//
//	}{	Modelisation d'un mot cle : c'est un symbole, avec un lex_type
//		associe
//
class T_keyword : public T_symbol
	{
	T_lex_type 						lex_type ;
	public :
	T_keyword() : T_symbol(NODE_KEYWORD) {} ;
	T_keyword(const char *new_value,
					  size_t new_len,
					  int h,
					  T_symbol *cur_symbol,
					  T_symbol *prev_symbol,
					  T_lex_type new_lex_type) ;
	virtual ~T_keyword() ;

	// Methode de dump HTML
	virtual FILE *dump_html(void) ;

	// Obtention du nom de la classe
	virtual const char *get_class_name(void) { return "T_keyword" ; } ;
	T_lex_type get_lex_type(void)				{ return lex_type ; } ;

	// Fonctions utilisateur
	virtual int user_defined_f1(void) ;
	virtual T_object *user_defined_f2(void) ;
	virtual int user_defined_f3(int arg) ;
	virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
	} ;

//
//	}{	Ajout des mots cles dans la table des symboles
//
extern void add_keywords(void) ;

//
//	}{	Retrait des mots cles de la table des symboles
//
extern void delete_keywords(void) ;

// Ajout d'un mot-cle
extern T_keyword *insert_builtin(T_lex_type new_lew_type,
										  const char *new_value) ;

//
//	}{	Obtention de references sur les mots-cles et les builtin
//
extern T_list_link *link_builtin_bool(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_mod(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_succ(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_pred(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_max(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_min(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_rmax(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_rmin(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_real(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_floor(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_ceiling(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_card(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_sigma(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_pi(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_gunion(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_ginter(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_qunion(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_qinter(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_id(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_prj1(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_prj2(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_dom(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_ran(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_size(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_first(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_last(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_front(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_tail(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_rev(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_l_or(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_l_not(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_nat(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_nat1(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_int(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_true(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_false(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_maxint(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_minint(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_integer(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_natural(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_natural1(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_bool_type(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_string_type(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_real_type(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_float_type(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_pow(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_pow1(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_fin(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_fin1(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_trans_func(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_trans_rel(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_sequence(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_non_empty_sequence(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_injective_seq(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *
	link_builtin_non_empty_injective_seq(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_permutation(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_generalised_concat(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_iterate(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_closure(T_lexem *ref_lexem, T_betree *betree) ;
extern T_list_link *link_builtin_closure1(T_lexem *ref_lexem, T_betree *betree) ;

extern T_list_link * link_builtin_tree    (T_lexem *ref_lexem,
											       T_betree *betree) ;
extern T_list_link * link_builtin_btree   (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_const   (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_top     (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_sons    (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_prefix  (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link *link_builtin_postfix (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_sizet   (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_mirror  (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_rank    (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_father  (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_son     (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link *link_builtin_subtree (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_arity   (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_bin     (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_left    (T_lexem *ref_lexem,
												   T_betree *betree) ;
extern T_list_link * link_builtin_right   (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_infix   (T_lexem *ref_lexem,
												   T_betree *betree);
extern T_list_link * link_builtin_ref(T_lexem *ref_lexem, T_betree *betree) ;

extern T_ident *get_builtin_bool(void) ;
extern T_ident *get_builtin_pred(void) ;
extern T_ident *get_builtin_succ(void) ;
extern T_ident *get_builtin_INT(void) ;
extern T_ident *get_builtin_NAT(void) ;
extern T_ident *get_builtin_NAT1(void) ;
extern T_ident *get_builtin_BOOL(void) ;
extern T_ident *get_builtin_STRING(void) ;
extern T_ident *get_builtin_REAL(void) ;
extern T_ident *get_builtin_FLOAT(void) ;
extern T_ident *get_builtin_MAXINT(void) ;
extern T_ident *get_builtin_MININT(void) ;
extern T_ident *get_builtin_max(void) ;
extern T_ident *get_builtin_min(void) ;
extern T_ident *get_builtin_card(void) ;
extern T_ident *get_builtin_gunion(void) ;
extern T_ident *get_builtin_ginter(void) ;
extern T_ident *get_builtin_id(void) ;
extern T_ident *get_builtin_prj1(void) ;
extern T_ident *get_builtin_prj2(void) ;
extern T_ident *get_builtin_dom(void) ;
extern T_ident *get_builtin_ran(void) ;
extern T_ident *get_builtin_TRUE(void) ;


#ifdef B0C
extern T_ident *get_builtin_INTEGER(void) ;
extern T_ident *get_builtin_NATURAL(void) ;
extern T_ident *get_builtin_NATURAL1(void) ;
#endif // B0C

// Valeurs de MININT/MAXINT
extern T_integer *get_builtin_MAXINT_value(void) ;
extern T_integer *get_builtin_MININT_value(void) ;

//
//	}{	Fin du fichier
//
#endif /* _C_BLTIN_H_ */




