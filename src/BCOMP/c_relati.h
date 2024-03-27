/******************************* CLEARSY **************************************
* Fichier : $Id: c_relati.h,v 2.0 1999-01-06 13:41:23 ab Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interfaces des relations
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
#ifndef _C_RELATION_H_
#define _C_RELATION_H_

#include "c_expr.h"

//
//	}{	Type de relation
//
typedef enum
{
  /* 00 '-->' */	RTYPE_TOTAL_FUNCTION = 0,
					/* 01 '>->' */	RTYPE_TOTAL_INJECTION,
					/* 02 '-->>'*/	RTYPE_TOTAL_SURJECTION,
					/* 03 '+->' */	RTYPE_PARTIAL_FUNCTION,
					/* 04 '>+>' */	RTYPE_PARTIAL_INJECTION,
					/* 05 '+->>'*/	RTYPE_PARTIAL_SURJECTION,
					/* 06 '>->>'*/	RTYPE_BIJECTION
} T_relation_type ;

class T_relation : public T_expr
{
  // Type de relation
  T_relation_type		relation_type ;

	// Ensembles de depart
  T_expr				*src_set ;

  // Ensemble d'arrivee
  T_expr				*dst_set ;

  // Operandes n-aires
  T_list_link			*first_set ;
  T_list_link	   		*last_set ;

	// Nombre d'operandes
  size_t				nb_sets ;

  //
  //	Methodes privees
  //
  // Mise en place des ensembles n-aires
  void fetch_sets(void) ;

public :
  T_relation() : T_expr(NODE_RELATION) {} ;
  T_relation(T_item *new_src_set,
			 T_item *new_dst_set,
			 T_relation_type new_relation_type,
			 T_item **adr_first,
			 T_item **adr_last,
			 T_item *new_father,
			 T_betree *new_betree,
			 T_lexem *new_ref_lexem) ;
  virtual ~T_relation(void) ;

	// Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_relation" ; }  ;
  T_relation_type get_relation_type(void)
	{ return relation_type ; } ;
  T_expr *get_src_set(void)			{ return src_set ; } ;
  T_expr *get_dst_set(void)			{ return dst_set ; } ;

	  // Phase de correction
	  // ref_this contient l'adresse du champ du pere a mettre a jour
  virtual T_item *check_tree(T_item **ref_this) ;

  // Fonction generique de calcul de type
  virtual void internal_make_type(void) ;

#ifdef B0C
  // Est-ce un ensemble de fonctions totales ?
  virtual int syntax_check_is_a_total_function_set(void);

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_RELATION_H_ */

