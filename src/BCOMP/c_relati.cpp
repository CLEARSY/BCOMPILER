/******************************* CLEARSY **************************************
* Fichier : $Id: c_relati.cpp,v 2.0 1999-06-09 12:38:28 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Relations
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
RCS_ID("$Id: c_relati.cpp,v 1.15 1999-06-09 12:38:28 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_relation
//
T_relation::T_relation(T_item *new_src_set,
								T_item *new_dst_set,
								T_relation_type new_relation_typeator,
								T_item **adr_first,
								T_item **adr_last,
								T_item *father,
								T_betree *betree,
								T_lexem *ref_lexem)
  : T_expr(NODE_RELATION, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE8("T_relation::T_relation(%x, %x, %d, %x, %x, %x, %x, %x)",
		 new_src_set, new_dst_set, new_relation_typeator,
		 adr_first, adr_last, father, betree, ref_lexem) ;

  int error = FALSE ;
  if (new_src_set->is_an_expr() == FALSE)
	{
	  error = TRUE ;
	  syntax_expr_expected_error(new_src_set, CAN_CONTINUE) ;
	}
  else
	{
	  src_set = new_src_set->make_expr() ;
	  src_set->post_check() ;
	}

  if (new_dst_set->is_an_expr() == FALSE)
	{
	  error = TRUE ;
	  syntax_expr_expected_error(new_dst_set, CAN_CONTINUE) ;
	}
  else
	{
	  dst_set = new_dst_set->make_expr() ;
	  dst_set->post_check() ;
	}

  relation_type = new_relation_typeator ;

  if (error == TRUE)
	{
	  // Reprise sur erreur
	  return ;
	}
  // On met a jour les liens "father"
  src_set->set_father(this) ;
  dst_set->set_father(this) ;

  // Champs first_set, last_set
  first_set = last_set = NULL ;
  fetch_sets() ;

  // Calcul du nombre de sets
  nb_sets = 0 ;
  T_item *cur = first_set ;
  while (cur != NULL)
	{
	  nb_sets ++ ;
	  cur = cur->get_next() ;
	}

}

// Destructeur
T_relation::~T_relation(void)
{
}
// Ajout des sets propres et celles des fils
void T_relation::fetch_sets(void)
{
  TRACE1("T_relation(%x)::fetch_sets", this) ;

  T_relation *rsrc = (T_relation *)src_set ; // cast justifie quand necessaire apres
  T_relation *rdst = (T_relation *)dst_set ; // cast justifie quand necessaire apres

  if ( 	(src_set->get_node_type() == NODE_RELATION)
		&& (rsrc->relation_type == relation_type) )
	{
	  // On recupere les sets de first_set
#ifdef FULL_TRACE
	  TRACE0("DEBUT recuperation des sets de first_set") ;
#endif
	  ENTER_TRACE ;
	  T_list_link *cur = rsrc->first_set ;
	  while (cur != NULL)
		{
		  (void)new T_list_link(cur->get_object(),
								LINK_RELATION_SRC_SET_LIST,
								(T_item **)&first_set,
								(T_item **)&last_set,
								this,
								get_betree(),
								((T_item *)cur->get_object())->get_ref_lexem()) ;
		  cur = (T_list_link *)cur->get_next() ;
		}
	  EXIT_TRACE ;
#ifdef FULL_TRACE
	  TRACE0("FIN   recuperation des sets de first_set") ;
#endif
	}
  else
	{
	  // On ajoute src_set dans la liste des sets
#ifdef FULL_TRACE
	  TRACE0("AJOUT DE SRC_SET") ;
#endif
	  (void)new T_list_link(src_set,
							LINK_RELATION_SRC_SET_LIST,
							(T_item **)&first_set,
							(T_item **)&last_set,
							this,
							get_betree(),
							src_set->get_ref_lexem()) ;
	}

  if ( 	(dst_set->get_node_type() == NODE_RELATION)
		&& (rdst->relation_type == relation_type) )
	{
	  // On recupere les sets de dst_set
#ifdef FULL_TRACE
	  TRACE0("DEBUT recuperation des sets de dst_set") ;
#endif
	  ENTER_TRACE ;
	  T_list_link *cur = rdst->first_set ;
	  while (cur != NULL)
		{
		  (void)new T_list_link(cur->get_object(),
								LINK_RELATION_DST_SET_LIST,
								(T_item **)&first_set,
								(T_item **)&last_set,
								this,
								get_betree(),
								((T_item *)cur->get_object())->get_ref_lexem()) ;
		  cur = (T_list_link *)cur->get_next() ;
		}
	  EXIT_TRACE ;
#ifdef FULL_TRACE
	  TRACE0("FIN   recuperation des sets de dst_set") ;
#endif
	}
  else
	{
	  // On ajoute dst_set dans la liste des sets
#ifdef FULL_TRACE
	  TRACE0("AJOUT DE DST_SET") ;
#endif
	  (void)new T_list_link(dst_set,
							LINK_RELATION_DST_SET_LIST,
							(T_item **)&first_set,
							(T_item **)&last_set,
							this,
							get_betree(),
							dst_set->get_ref_lexem()) ;
	}

}

//
//	}{	Correction de l'arbre
//
T_item *T_relation::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_relation(%x)::check_tree", this) ;
#endif

  if (src_set != NULL)
	{
	  src_set->check_tree((T_item **)&src_set) ;
	}

  if (dst_set != NULL)
	{
	  dst_set->check_tree((T_item **)&dst_set) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

//
//	}{	Fin du fichier
//
