/******************************* CLEARSY **************************************
* Fichier : $Id: c_chain2.cpp,v 2.0 1999-10-06 16:39:39 sl Exp $
*
* Compilations :
*
* Description :		Chainage de type "LIFO" pour sauvegarde/restauration
*					Le comportement est necessaire pour que les E/S de Betree
*					se passent bien. A faire : tenter une optimisation !
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
RCS_ID("$Id: c_chain2.cpp,v 1.13 1999-10-06 16:39:39 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>

/* Includes Composants externes */
#include "c_api.h"
#include "c_chain2.h"

static T_chain2 *first_chain2 = NULL ;
static T_chain2 *last_chain2 = NULL ;

void reset_chain2(T_betree *new_betree)
{
  TRACE0("chain2 reset") ;
  for (T_chain2 *chain2 = first_chain2; chain2;)
    {
      T_chain2 *chain2_to_delete = chain2 ;
      chain2 = chain2->get_next() ;
      delete chain2_to_delete ;
    }
  first_chain2 = NULL ;
  last_chain2 = NULL ;

  T_chain2 *chain2 = new T_chain2(new_betree, NULL, NULL) ;
  chain2->set_generated(TRUE) ;
}

/* fonction qui ajoute une adresse */
int chain2_add(T_object *adr)
{
  T_chain2 *cur = first_chain2 ;
  T_chain2 *prev = NULL ;

  //  TRACE1("chain2_add(%x)", adr) ;

  for (;;)
	{
#ifndef OPTIM_PERSIST
	  if (cur == NULL)
#else // OPTIM_PERSIST
		// Liste trie par ordre croissant
		if ( (cur == NULL) || (cur->get_adr() > adr) )
#endif // !OPTIM_PERSIST
		  {
			/* Pas trouve ! */
			(void)new T_chain2(adr, cur, prev) ;
			return FALSE ;
		  }

	  if (cur->get_adr() == adr)
		{
		  /* Trouve ! */
		  int res = cur->get_generated() ;
		  //TRACE0("trouve") ;
		  return res ;
		}

	  prev = cur ;
	  cur = cur->get_next() ;
	}
}


T_chain2::T_chain2(T_object *new_adr,
							T_chain2 *new_next,
							T_chain2 *new_prev)
{
	//  TRACE4("T_chain2(%x)::T_chain2(%x, %x, %x)", this, new_adr, new_next, new_prev) ;

  adr = new_adr ;
  next = new_next ;
  prev = new_prev ;
  generated = FALSE ;

  if (next == NULL)
	{
	  last_chain2 = this ;
	}
  else
	{
	  next->prev = this ;
	}

  if (prev == NULL)
	{
	  first_chain2 = this ;
	}
  else
	{
	  prev->next = this ;
	}
}


/* Nettoyage */
T_chain2::~T_chain2(void)
{
	//  TRACE1("T_chain2(%x)::~T_chain2()", this) ;
  // On ne libere rien (objet est une reference)
}

// Fonction qui rend une adresse non generee
#ifndef OPTIM_PERSIST
// Code non optimise
T_object *chain2_get(void)
{
T_chain2 *cur = first_chain2 ;

//TRACE0("chain2_get ...") ;
for (;;)
	{
	if (cur == NULL)
		{
			//		TRACE0("chain2_get -> NULL") ;
		return NULL ;
		}

	if (cur->get_generated() == FALSE)
		{
			//		TRACE1("chain2_get -> %x", cur->get_adr()) ;
		cur->set_generated(TRUE) ;
		return cur->get_adr() ;
		}

	cur = cur->get_next() ;
	}
}
#else // OPTIM_PERSIST
// Code optimise
T_object *chain2_get(void)
{
  T_chain2 *cur = first_chain2 ;

  //  TRACE0("chain2_get ...") ;
  for (;;)
	{
	  if (cur == NULL)
		{
			//		  TRACE0("chain2_get -> NULL") ;
		  return NULL ;
		}

	  if (cur->get_generated() == FALSE)
		{
		  // Ici il faut dechainer cur
		  T_chain2 *prev = cur->get_prev() ;
		  T_chain2 *next = cur->get_next() ;

		  if (prev != NULL)
			{
			  prev->set_next(next) ;
			}
		  else
			{
			  first_chain2 = next ;
			}

		  if (next != NULL)
			{
			  next->set_prev(prev) ;
			}
		  else
			{
			  last_chain2 = prev ;
			}

		  T_object *res = cur->get_adr() ;
		  //		  TRACE2("chain2_get->%x (delete %x)", res, cur) ;
		  return res ;
		}

	  cur = cur->get_next() ;
	}
}
#endif // !OPTIM_PERSIST
