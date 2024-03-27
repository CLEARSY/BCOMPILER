/******************************* CLEARSY **************************************
* Fichier : $Id: c_valu.cpp,v 2.0 1999-06-09 12:38:33 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Valuations
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
RCS_ID("$Id: c_valu.cpp,v 1.11 1999-06-09 12:38:33 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_valuation
//
// Constructeur de la classe T_valuation
// version classique avec parsing
T_valuation::T_valuation(T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem **adr_ref_lexem)
  : T_item(NODE_VALUATION,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   *adr_ref_lexem)
{
  TRACE5("T_valuation::T_valuation(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
// }{	Constructeur a partir d'un binary op avec
// typing_predicate_type = EQUAL deja fait
//
T_valuation::T_valuation(T_typing_predicate *ref_tpred,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree)
  : T_item(NODE_VALUATION,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   ref_tpred->get_ref_lexem())
{
  TRACE5("T_valuation::T_valuation(%x, %x, %x, %x, %x)",
		 ref_tpred,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree) ;

  // Verifie par l'appelant
  ASSERT(ref_tpred->get_typing_predicate_type() == TPRED_EQUAL) ;

  ident = (T_ident *)ref_tpred->get_identifiers() ;

  if (ident->is_an_ident() == FALSE)
	{
	  syntax_ident_expected_error(ident, CAN_CONTINUE) ;
	}

  TRACE2("mise a jour du pere de %x avec %x", ident, this) ;
  ident->set_father(this) ;

  T_item *ref_type = ref_tpred->get_type() ;

  TRACE2("mise a jour du pere de %x avec %x", ref_type, this) ;
  ref_type->set_father(this) ;

  if (ref_type->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(ref_type, CAN_CONTINUE) ;
	}
  else
	{
	  value = ref_type->make_expr() ;
	  value->post_check() ;
	}

  TRACE0("fin de T_valuation::T_valuation") ;
}

// Destructeur
T_valuation::~T_valuation(void)
{
}

// Demande de l'acces necessaire pour le fils ref
// Renvoie TRUE si ref est en partie gauche
T_access_authorisation T_valuation::get_auth_request(T_item *ref)
{
  TRACE5("T_valuation(ident %x:%s)::get_auth_request(%x, %s) -> %s",
		 ident,
		 ident->get_class_name(),
		 ref,
		 ref->get_class_name(),
		 (ref == ident) ? "AUTH_WRITE" : "AUTH_READ") ;

  return (ref == ident) ? AUTH_WRITE : AUTH_READ ;
}

// Bascule de fonctionnement pour T_valuation::type_check
// Si new_mode == TRUE : le TC de la clause values ne traite que les SETS
// Si new_mode == FALSE : le TC de la clause values ne traite que les CST
static int values_mode_sets_si = FALSE ;
void set_values_mode_sets(int new_mode)
{
  values_mode_sets_si = new_mode ;
}

// Lecture de la bascule
int get_values_mode_sets(void)
{
  return values_mode_sets_si ;
}

//
//	}{	Fin du fichier
//
