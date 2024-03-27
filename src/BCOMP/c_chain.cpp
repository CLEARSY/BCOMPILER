/******************************* CLEARSY **************************************
* Fichier : $Id: c_chain.cpp,v 2.0 1999-04-20 08:02:18 sl Exp $
* (C) 2008 CLEARSY
*
* Compilations :
*	   -DSTATS_CHAIN	pour avoir des statistiques sur les chainages
*	   -DPORTABLE_CHAIN	pour avoir une version plus lente mais machine
*						independante
*		-DDEBUG_CHAIN	pour avoir des traces
*
* Description :		Chainage des pages HTML deja crees
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
RCS_ID("$Id: c_chain.cpp,v 1.7 1999-04-20 08:02:18 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>

/* Includes Composants externes */
#include "c_api.h"

// Taille du H-CODE de chain
static const int CHAIN_HASH_SIZE = 10 ;
static T_chain *first_chain[CHAIN_HASH_SIZE] ;
static T_chain *last_chain[CHAIN_HASH_SIZE] ;

// Ligne courante pour chain_get
static int cur_line_si = 0 ;

#ifdef STATS_CHAIN
static int nb_chain_calls_si = 0 ;
static int nb_chain_chained_si = 0 ;
static int nb_chain_already_si = 0 ;
static int total_chain_depth = 0 ;
#endif

void chain_reset(T_betree *new_betree)
{
  TRACE0("chain reset") ;
  int i ;

  for (i = 0 ; i < CHAIN_HASH_SIZE ; i++)
	{
	  first_chain[i] = NULL ;
	  last_chain[i] = NULL ;
	}

#ifdef STATS_CHAIN
  nb_chain_calls_si = 0 ;
  nb_chain_chained_si = 0 ;
  nb_chain_already_si = 0 ;
  total_chain_depth = 0 ;
#endif

  cur_line_si = 0 ;

  // Amorcage avec le Betree
  T_chain *chain = new T_chain(new_betree
#ifndef PORTABLE_CHAIN
							   , NULL, NULL,
#endif
							   ((size_t)new_betree) % 10) ;
  chain->set_generated(TRUE) ;
}

/* fonction qui ajoute une adresse */
int chain_add(T_object *adr)
{
  ASSERT(adr != NULL) ;
#ifdef STATS_CHAIN
  nb_chain_calls_si++ ;
#endif

  int line = (size_t)adr % CHAIN_HASH_SIZE ;
  T_chain *cur = first_chain[line] ;
  T_chain *prev = NULL ;

#ifdef DEBUG_CHAIN
  TRACE1("chain_add(%x)", adr) ;
#endif

  for (;;)
	{
#ifdef STATS_CHAIN
	  total_chain_depth ++ ;
#endif
#ifdef PORTABLE_CHAIN
	  // Version degradee permettant d'avoir des fichiers independants de
	  // la plateforme utilisee
	  if (cur == NULL)
#else
		if ( (cur == NULL) || (cur->get_adr() > adr) )
#endif
		  {
			/* Pas trouve ! */
#ifdef DEBUG_CHAIN
			TRACE1("pas trouve -> ajout dans la ligne %d", line) ;
#endif
#ifdef STATS_CHAIN
			nb_chain_chained_si ++ ;
#endif
			(void)new T_chain(adr
#ifndef PORTABLE_CHAIN
						, cur, prev,
#endif
						line) ;
			return FALSE ;
		  }

	  if (cur->get_adr() == adr)
		{
		  /* Trouve ! */
#ifdef DEBUG_CHAIN
		  TRACE0("trouve") ;
#endif
		  int res = cur->get_generated() ;
#ifdef STATS_CHAIN
		  nb_chain_already_si ++ ;
#endif
		  return res ;
		}

	  prev = cur ;
	  cur = cur->get_next() ;
	}
}


#ifndef PORTABLE_CHAIN
//
//	Version optimisee
//
T_chain::T_chain(T_object *new_adr,
						  T_chain *new_next,
						  T_chain *new_prev,
						  int line)
{
#ifdef DEBUG_CHAIN
  TRACE5("T_chain(%x)::T_chain(%x, %x, %x, %d)",
		 this, new_adr, new_next, new_prev, line) ;
#endif
  adr = new_adr ;
  next = new_next ;
  prev = new_prev ;
  generated = FALSE ;

  if (next == NULL)
	{
	  last_chain[line] = this ;
	}
  else
	{
	  next->prev = this ;
	}

  if (prev == NULL)
	{
	  first_chain[line] = this ;
	}
  else
	{
	  prev->next = this ;
	}
}
#else /* PORTABLE_CHAIN */
// Version lente, machine-independante
// CHAINAGE EN QUEUE POUR L'INSTANCE
T_chain::T_chain(T_object *new_adr, int line)
{
  adr = new_adr ;
  generated = FALSE ;

  if (first_chain[line] == NULL)
	{
	  first_chain[line] = this ;
	}
  else
	{
	  last_chain[line]->next = this ;
	}

  next = NULL ;
  prev = last_chain[line] ;
  last_chain[line] = this ;
}
#endif /* ! PORTABLE_CHAIN */


/* Nettoyage */
T_chain::~T_chain(void)
{
#if 0 // A FAIRE
  T_chain *cur = firsT_chain ;
  T_chain *next ;

  while (cur != NULL)
	{
	  next = cur->next ;
	  s_free(cur) ;
	  cur = next ;
	}
#endif
}

/* Fonction qui rend une adresse non generee */
T_object *chain_get(void)
{
  T_chain *cur = first_chain[cur_line_si] ;

  int first_line = cur_line_si ;

#ifdef DEBUG_CHAIN
  TRACE1("chain_get(cur_line_si = %d) ...", cur_line_si) ;
#endif
  for (;;)
	{
	  if (cur == NULL)
		{
#ifdef DEBUG_CHAIN
		  TRACE1("ligne courante (%d) -> NULL", cur_line_si) ;
#endif
		  // On passe a la ligne suivante
		  cur_line_si = (cur_line_si + 1) % CHAIN_HASH_SIZE ;

		  if (cur_line_si == first_line)
			{
			  // On a fait tout le tour pour rien !
			  TRACE0("on sort car on a fait tout le tour pour rien") ;
			  return NULL ;
			}

		  // On continue ...
		  cur = first_chain[cur_line_si] ;
		  continue ;
		}

	  if (cur->get_generated() == FALSE)
		{
#ifdef DEBUG_CHAIN
		  TRACE1("chain_get -> %x", cur->get_adr()) ;
#endif
		  cur->set_generated(TRUE) ;
		  return cur->get_adr() ;
		}

	  cur = cur->get_next() ;
	}
}

#ifdef STATS_CHAIN
void get_chain_stats(void)
{
  TRACE0("--- DEBUT STATISTIQUES DE CHAIN ---") ;
  TRACE1("nombre d'appels a chain_add                 : %d", nb_chain_calls_si) ;
  TRACE1("nombre d'adresses effectivement chainees    : %d", nb_chain_chained_si) ;
  TRACE1("nombre d'adresses trouvees dans la liste    : %d", nb_chain_already_si) ;
  TRACE1("nombre total de cellules visitees :         : %d", total_chain_depth) ;
  if (nb_chain_calls_si > 0)
	{
	  TRACE1("nombre moyen de cellules visitees par appel : %d",
			 (total_chain_depth / nb_chain_calls_si)) ;
	}
  TRACE0("---- FIN STATISTIQUES DE CHAIN ----") ;
}
#endif

