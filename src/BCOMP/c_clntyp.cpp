/***************************** CLEARSY **************************************
* Fichier : $Id: c_clntyp.cpp,v 2.0 1999-10-04 16:26:00 sl Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_TYPE pour les traces generales
*
* Description :	Clonage des types B
*				Provient de c_type.cpp 1.11
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_clntyp.cpp,v 1.19 1999-10-04 16:26:00 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	FONCTIONS DE CLONAGE DE TYPE
//

// Clonage
T_type *T_setof_type::clone_type(T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE1("T_setof_type(%x)::clone_type", this) ;
#endif

  ENTER_TRACE ;

  T_type *new_base_type = base_type->clone_type(NULL,
												new_betree,
												new_ref_lexem) ;

#ifdef DEBUG_TYPE
  TRACE0("AVANT CREATION") ;
#endif

  T_setof_type *res = new T_setof_type(new_base_type,
									   new_father,
									   new_betree,
									   new_ref_lexem) ;
  new_base_type->unlink() ;
#ifdef DEBUG_TYPE
  TRACE0("APRES CREATION") ;

#endif

  new_base_type->set_father(res) ;
  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE1("FIN T_setof_type(%x)::clone_type", this) ;
#endif

  TRACE1("copie du tid %x", get_typing_ident()) ;
  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

// Clonage
T_type *T_product_type::clone_type(T_item *new_father,
											T_betree *new_betree,
											T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE1("T_product_type(%x)::clone_type", this) ;
#endif

  ENTER_TRACE ;

  T_type *clone1 = type1->clone_type(NULL, new_betree, new_ref_lexem) ;
  T_type *clone2 = type2->clone_type(NULL, new_betree, new_ref_lexem) ;

  T_product_type *res = new T_product_type(clone1,
										   clone2,
										   new_father,
										   get_betree(),
										   new_ref_lexem) ;

  clone1->unlink() ;
  clone2->unlink() ;


  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE1("FIN T_product_type(%x)::clone_type", this) ;
#endif

  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

// Clonage
T_type *T_predefined_type::clone_type(T_item *new_father,
											   T_betree *new_betree,
											   T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE3("T_predefined_type(%x)::clone_type (ref_lexem %x, new_father=%x)",
		 this, get_ref_lexem(), new_father) ;
#endif

  ENTER_TRACE ;

  T_expr *new_upper_bound = get_upper_bound() ;
#if 0 // NON, fait dans le constructeur
  if (new_upper_bound != NULL)
	{
	  new_upper_bound->link() ;
	}
#endif
  T_expr *new_lower_bound = get_lower_bound() ;
#if 0 // NON, fait dans le constructeur
  if (new_lower_bound != NULL)
	{
	  new_lower_bound->link() ;
	}
#endif
  T_predefined_type *res = new T_predefined_type(type,
												 new_lower_bound,
												 new_upper_bound,
												 new_father,
												 new_betree,
												 new_ref_lexem) ;

#ifdef DEBUG_TYPE
  TRACE2("type clone src = %x, dst = %x", this, res) ;
#endif

#ifdef DEBUG_TYPE
  TRACE1("copie de ref_interval = %x", ref_interval) ;
#endif
  res->ref_interval = ref_interval ;
  if (ref_interval != NULL)
	{
	  ref_interval->link() ;
	}

#ifdef DEBUG_TYPE
  TRACE1("copie de name = %x", get_name()) ;
#endif
  res->set_name(get_name()) ;

  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE2("FIN T_predefined_type(%x)::clone_type -> res", this, res) ;
#endif


  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

// Clonage
T_type *T_abstract_type::clone_type(T_item *new_father,
											 T_betree *new_betree,
											 T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE1("T_abstract_type(%x)::clone_type", this) ;
#endif

  ASSERT(get_after_valuation_type() != this) ;
  ENTER_TRACE ;

  // On ne clone pas les bornes
  T_abstract_type *res = new T_abstract_type(set,
											 get_lower_bound(),
											 get_upper_bound(),
											 new_father,
											 new_betree,
											 new_ref_lexem) ;

  if (after_valuation_type != NULL)
	{
	  res->after_valuation_type = after_valuation_type->clone_type(res,
																   new_betree,
																   new_ref_lexem) ;
	}
  else
	{
	  res->after_valuation_type = NULL ;
	}

  res->valuation_ident = valuation_ident ;
  res->set_name(get_name()) ;
  //  res->set_original_type(get_original_type()) ;
  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE1("FIN T_abstract_type(%x)::clone_type", this) ;
#endif


  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

// Clonage
T_type *T_enumerated_type::clone_type(T_item *new_father,
											   T_betree *new_betree,
											   T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE1("T_enumerated_type(%x)::clone_type", this) ;
#endif

  ENTER_TRACE ;

  // On ne clone pas les bornes
  T_enumerated_type *res = new T_enumerated_type(type_definition,
												 get_lower_bound(),
												 get_upper_bound(),
												 new_father,
												 new_betree,
												 new_ref_lexem) ;

  //  res->set_original_type(get_original_type()) ;
  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE1("FIN T_enumerated_type(%x)::clone_type", this) ;
#endif


  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

// Clonage
T_type *T_generic_type::clone_type(T_item *new_father,
											T_betree *new_betree,
											T_lexem *new_ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE1("T_generic_type(%x)::clone_type", this) ;
#endif

  ENTER_TRACE ;
  T_generic_type *res = new T_generic_type(new_father, new_betree, new_ref_lexem) ;
  res->type = type ;

  EXIT_TRACE ;
#ifdef DEBUG_TYPE
  TRACE1("FIN T_generic_type(%x)::clone_type", this) ;
#endif


  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

T_type *T_label_type::clone_type(T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *new_ref_lexem)
{
  if (get_unique_record_type_mode() == TRUE)
	{
	  // Mode pour le GNF ou on garde toujours le meme type pour tout renseigner
	  return this ;
	}

#ifdef DEBUG_TYPE
  TRACE1("T_label_type(%x)::clone_type", this) ;
  ENTER_TRACE ;
#endif

  T_label_type *res = new T_label_type(name,
									   NULL,
									   type,
									   NULL,
									   NULL,
									   new_father,
									   new_betree,
									   new_ref_lexem) ;

#ifdef DEBUG_TYPE
  EXIT_TRACE ;
  TRACE1("FIN T_label_type(%x)::clone_type", this) ;
#endif

  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

T_type *T_record_type::clone_type(T_item *new_father,
										   T_betree *new_betree,
										   T_lexem *new_ref_lexem)
{
  if (get_unique_record_type_mode() == TRUE)
	{
	  // Mode pour le GNF ou on garde toujours le meme type pour tout renseigner
	  return this ;
	}

#ifdef DEBUG_TYPE
  TRACE1("T_record_type(%x)::clone_type", this) ;
  ENTER_TRACE ;
#endif

  T_record_type *res = new T_record_type(new_father,
										 new_betree,
										 new_ref_lexem) ;

  T_item **adr_first = (T_item **)res->get_adr_first_label() ;
  T_item **adr_last = (T_item **)res->get_adr_last_label() ;
  T_label_type *cur = first_label ;
  while (cur != NULL)
	{
	  T_type *clone = cur->clone_type(res, new_betree, new_ref_lexem) ;
	  clone->tail_insert(adr_first, adr_last) ;
	  cur = (T_label_type *)cur->get_next() ;
	}

#ifdef DEBUG_TYPE
  EXIT_TRACE ;
  TRACE1("FIN T_record_type(%x)::clone_type", this) ;
#endif

  res->set_typing_ident(get_typing_ident()) ;
  return res ;
}

//
//	}{ Fin du fichier
//
