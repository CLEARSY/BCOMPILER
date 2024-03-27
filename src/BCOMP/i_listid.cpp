/******************************* CLEARSY **************************************
* Fichier : $Id: i_listid.cpp,v 2.0 1999-01-06 14:13:55 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		corps de la classe T_list_ident
*
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
RCS_ID("$Id: i_listid.cpp,v 1.2 1999-01-06 14:13:55 sl Exp $") ;

// Includes systeme
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Includes Composant Local */
#include "c_api.h"

#ifdef B0C
#include "i_listid.h"

T_list_ident::T_list_ident(void)
{
  // Initialisations
  first = NULL;
  last = NULL;
}

T_list_ident::T_list_ident(T_list_ident *list_ident)
{
  // Initialisations
  first = NULL;
  last = NULL;

  ASSERT(list_ident != NULL);

  T_list_link *cur_list_link = list_ident->get_list_ident();

  while (cur_list_link != NULL)
	{
	  // on insere l'ident courant dans la liste
	  T_list_link *link ;
	  link = new T_list_link((T_ident *)cur_list_link->get_object(),
							 LINK_OTHER,
							 &first,
							 &last,
							 NULL,
							 NULL,
							 NULL);

	  // Prise de responsabilite sur list_ident
	  ((T_ident *)cur_list_link->get_object())->link() ;
	  // On previent le T_list_link qu'il faudra liberer le
	  // list_ident
	  link->set_do_free() ;

	  cur_list_link = (T_list_link *)cur_list_link->get_next() ;
	}

  // Au clonage, le marqueur vaut last
}

T_list_ident::~T_list_ident(void)
{
  list_unlink(first) ;
}

void T_list_ident::add_identifier(T_ident *ident)
{
  T_list_link *link = new T_list_link(ident,
									  LINK_OTHER,
									  &first,
									  &last,
									  NULL,
									  NULL,
									  NULL);

  // Prise de responsabilite sur ident
  ident->link() ;
  // On previent le T_list_link qu'il faudra liberer l'ident
  link->set_do_free() ;
}

int T_list_ident::find_identifier(T_ident *ident)
{
  T_ident *ident_definition = ident->get_definition();

  T_list_link *cur_list_link = (T_list_link *) first;

  while (cur_list_link != NULL)
	{
	  // on compare la definition de l'ident courant avec ident_definition
	  if (((T_ident *) cur_list_link->get_object())->get_definition()
		  == ident_definition)
		{
		  return TRUE;
		}
	  cur_list_link = (T_list_link *)cur_list_link->get_next() ;
	}
  return FALSE;
}


// Realise l'intersection avec une autre T_list_ident
void T_list_ident::make_intersection(T_list_ident *list_ident)
{
  T_list_ident *cur_list_ident = list_ident;
  T_list_link *cur_list_link = (T_list_link *) first;

  while (cur_list_link != NULL)
	{
	  // Si l'identifier courant n'est pas dans la liste en parametre,
	  // on le supprime
	  if (cur_list_ident->find_identifier((T_ident *)cur_list_link->get_object() ) == FALSE )
		{
		  T_list_link *inter_list_link = (T_list_link *)cur_list_link->get_next();

		  cur_list_link->remove_from_list(&first,&last);

		  cur_list_link = inter_list_link;

		  continue;
		}

	  cur_list_link = (T_list_link *)cur_list_link->get_next() ;
	}
}


#endif // B0C
