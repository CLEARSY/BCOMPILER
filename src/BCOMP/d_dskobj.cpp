/***************************** CLEARSY **************************************
* Fichier : $Id: d_dskobj.cpp,v 2.0 2007-07-25 09:58:51 arequet Exp $
* (C) 2008-2025 CLEARSY
*
* Description :	Objets persistants
*
* Compilation : -DPERSIST_TRACE pour avoir des traces
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
RCS_ID("$Id: d_dskobj.cpp,v 1.22 2007-07-25 09:58:51 arequet Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "d_dskman.h"
#include "d_dskobj.h"
#include "d_dskstr.h"
#include "c_chain2.h"

#ifdef OPTIM_PERSIST
#ifdef USE_CACHE
// Declarations forward pour le cache
// Initialisation du cache
static void init_index_cache(void) ;
#endif // !USE_CACHE
#endif // OPTIM_PERSIST



static T_disk_manager *disk_manager_sop ;
static T_disk_object *first_disk_object_sop ;
static T_disk_object *last_disk_object_sop ;
static size_t cur_index_si = 1 ;
static size_t cur_rank_si = 1 ;
static void *read_start_sop = NULL ;

static T_disk_object **rank_to_object_sot = NULL ;
static size_t rank_to_object_size_si = 0 ;

void init_rank_to_object_table(size_t max_rank)
{
  TRACE1("init_rank_to_object_table(%d)", max_rank) ;
  size_t size = (max_rank + 1) * sizeof(T_disk_object *) ;
  rank_to_object_size_si = max_rank + 1 ;
  rank_to_object_sot = (T_disk_object **)s_volatile_malloc(size) ;
  memset(rank_to_object_sot, 0, size) ;
}

void unlink_rank_to_object_table(void)
{
  TRACE0("unlink_rank_to_object_table") ;
  s_free(rank_to_object_sot) ;
}


void T_disk_object::set_rank(void)
{
#ifdef PERSIST_TRACE
  TRACE3("T_disk_object::set_rank() object %x (%s) is at rank %d",
		 get_address(),
		 ((T_object *)get_address())->get_class_name(),
		 cur_rank_si) ;
#endif
  rank = cur_rank_si++ ;
}

T_disk_manager*init_disk_object_write(T_betree *new_betree,
											   const char *file_name,
											   T_betree_file_type file_type)
{
#ifdef PERSIST_TRACE
  TRACE0("init_disk_object_write") ;
#endif

#ifdef OPTIM_PERSIST
#ifdef USE_CACHE
  // Initialisation du cache
  init_index_cache() ;
#endif // USE_CACHE
#endif // OPTIM_PERSIST

  first_disk_object_sop = NULL ;
  last_disk_object_sop = NULL ;
  cur_index_si = 1 ;
  cur_rank_si = 0 ;

  FILE *fd = fopen(file_name, "wb") ;

  if (fd == NULL)
	{
	  TRACE0("retour d'erreur dans init_disk_object_write") ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  if (new_betree != NULL)
	{
#ifndef OPTIM_PERSIST
	  (void)new T_disk_object(new_betree) ;
#else // OPTIM_PERSIST
	  (void)new T_disk_object(new_betree, NULL) ;
#endif // !OPTIM_PERSIST
	}

  disk_manager_sop = new T_disk_manager(fd,
										file_name,
										file_type,
										FILE_WRITE_BETREE) ;
  return disk_manager_sop ;
}

void init_disk_object_read(void *new_read_start_sop)
{
#ifdef PERSIST_TRACE
  TRACE0("init_disk_object_read") ;
#endif

#ifdef OPTIM_PERSIST
#ifdef USE_CACHE
  // Initialisation du cache
  init_index_cache() ;
#endif // USE_CACHE
#endif // OPTIM_PERSIST

  first_disk_object_sop = NULL ;
  last_disk_object_sop = NULL ;
  read_start_sop = new_read_start_sop ;
}

void exit_disk_object()
{
#ifdef PERSIST_TRACE
  TRACE0("exit_disk_object") ;
#endif

  // Liberation du manager
  delete disk_manager_sop ;
  disk_manager_sop = NULL ;
}

#ifndef OPTIM_PERSIST
// Vieux code
T_disk_object::T_disk_object(T_object *new_object)
{
#ifdef PERSIST_TRACE
  TRACE2("T_disk_object::T_disk_object(adr = %x) -> index = %d",
		 new_object,
		 cur_index_si) ;
#endif

  address = new_object ;
  //  TRACE2("mise a jour du tmp de %x avec %x", new_object, this) ;
  new_object->set_tmp(this) ;
  index = cur_index_si ++ ;
  offset = 0 ;
  solved = FALSE ;
  rank = 0 ;
  next = NULL ;

  if (last_disk_object_sop == NULL)
	{
	  first_disk_object_sop = this ;
	}
  else
	{
	  last_disk_object_sop->next = this ;
	}

  last_disk_object_sop = this ;
}
#else // OPTIM_PERSIST
// Code optimise
// Constructeur qui chaine apres after_this_object
// Si after_this_object == NULL, chainage en tete
T_disk_object::T_disk_object(T_object *new_object,
									  T_disk_object *after_this_object)
{
#ifdef PERSIST_TRACE
  TRACE2("T_disk_object::T_disk_object(adr = %x) OPTIMISE -> index = %d",
		 new_object,
		 cur_index_si) ;
#endif

  address = new_object ;
  TRACE2("mise a jour du tmp de %x avec %x", new_object, this) ;
  new_object->set_tmp(this) ;
  index = cur_index_si ++ ;
  offset = 0 ;
  solved = FALSE ;
  rank = 0 ;

  // Chainage apres after_this_object
  if (after_this_object != NULL)
	{
	  // Chainage dans une liste non vide
	  T_disk_object *after_next = after_this_object->next ;
	  after_this_object->next = this ;
	  next = after_next ;

	  if (after_next == NULL)
		{
		  // On est le nouveau dernier
		  last_disk_object_sop = this ;
		}
	}
  else
	{
	  // Premier element de la liste
	  first_disk_object_sop = this ;
	  next = NULL ;
	}
}
#endif // !OPTIM_PERSIST

T_disk_object::T_disk_object(size_t new_index,
									  size_t new_offset,
									  size_t new_rank)
{
#ifdef PERSIST_TRACE
  TRACE2("T_disk_object::T_disk_object(index = %d, offset = %d)",
		 new_index,
		 new_offset) ;
#endif

  // fprintf(stdout, "index %08d, rang %08d\n", new_index, new_rank) ;
  // fflush(stdout) ;

  address = NULL ;
  index = new_index ;
  offset = new_offset ;
  rank = new_rank ;
  assert(0 <= rank && rank < rank_to_object_size_si) ;
  rank_to_object_sot[rank] = this ;
  solved = FALSE ;

  next = NULL ;

  if (last_disk_object_sop == NULL)
	{
	  first_disk_object_sop = this ;
	}
  else
	{
	  last_disk_object_sop->next = this ;
	}

  last_disk_object_sop = this ;
}

void T_disk_object::dump_disk(void)
{
#ifdef PERSIST_TRACE
  TRACE1("T_disk_object(%x)::dump_disk", this) ;
#endif
  disk_manager_sop->write(offset) ;
  disk_manager_sop->write(rank) ;
#ifdef PERSIST_TRACE
  TRACE4("address = 0x%08x (%s) rank = %d offset = %d",
		 address,
		 ((T_object *)address)->get_class_name(),
		 rank,
		 offset) ;
#endif
}


#ifdef OPTIM_PERSIST
#ifdef USE_CACHE
// Recherche d'un index dans un cache
// Renvoie TRUE si l'objet est trouve (et renseigne *adr_index), FALSE sinon
struct S_index_cache
{
  // Adresse
  T_object *adr ;

  // Index
  size_t index ;

  // Timestamp (Date de dernier acces)
  size_t timestamp ;
} ;
typedef struct S_index_cache T_index_cache ;

// Le cache : taille, tableau, dernier index inutilise, heure actuelle
#define CACHE_SIZE (sizeof(T_item) * 2)
static T_index_cache index_cache_sot[CACHE_SIZE] ;
static size_t last_unused_index_cache_entry_si ;
static size_t current_index_cache_timestamp_si = CACHE_SIZE ;

// Initialisation du cache
static void init_index_cache(void)
{
  for (size_t i = 0 ; i < CACHE_SIZE ; i++)
	{
	  index_cache_sot[i].adr = NULL ;
	  index_cache_sot[i].index = 0 ;
	  index_cache_sot[i].timestamp = 0 ;
	}

  last_unused_index_cache_entry_si = 0 ;
  current_index_cache_timestamp_si = CACHE_SIZE + 1 ;
}

// Recherche dans le cache
static int object_index_cache_lookup(T_object *adr, size_t *adr_index)
{
  for (size_t i = 0 ; i < CACHE_SIZE ; i++)
	{
	  if (index_cache_sot[i].adr == adr)
		{
		  index_cache_sot[i].timestamp = current_index_cache_timestamp_si++ ;
		  *adr_index = index_cache_sot[i].index ;
		  return TRUE ;
		}
	}

  return FALSE ;
}

// Ajout dans le cache
static void object_index_cache_add(T_object *adr, size_t index)
{
  T_index_cache *cache_entry = NULL ;

  if (last_unused_index_cache_entry_si < CACHE_SIZE)
	{
	  // On utilise cette entree
	  cache_entry = &(index_cache_sot[last_unused_index_cache_entry_si]) ;
	  last_unused_index_cache_entry_si ++ ;
	}
  else
	{
	  // Il faut trouver celui accede il y a le plus longtemps
	  int min_cache_index = 0 ;
	  size_t min_timestamp = current_index_cache_timestamp_si ;

	  for (size_t i = 0 ; i < CACHE_SIZE ; i++)
		{
		  size_t timestamp = index_cache_sot[i].timestamp ;
		  if (timestamp < min_timestamp)
			{
			  min_timestamp = timestamp ;
			  min_cache_index = i ;
			}
		}

	  cache_entry = &(index_cache_sot[min_cache_index]) ;
	}

  // Mise a jour
  cache_entry->timestamp = current_index_cache_timestamp_si++ ;
  cache_entry->adr = adr ;
  cache_entry->index = index ;
}
#endif // USE_CACHE
#endif // OPTIM_PERSIST


#ifndef OPTIM_PERSIST
// Obtention de l'index associe a un objet
// Si l'objet est deja dans la table, l'index associe est retourne
// Sinon, on cree une entree dans la table et on insere l'adresse
// dans la liste des adresses a sauvegarder
size_t disk_object_get_object_index(T_object *adr)
{
#ifdef PERSIST_TRACE
  TRACE1("disk_object_get_object_index(%x)", adr) ;
#endif
  ASSERT(object_test(adr) == TRUE) ;

  T_disk_object *cur = first_disk_object_sop ;

#ifdef PERSIST_TRACE
  TRACE1("first = %x", cur) ;
#endif

  while ( (cur != NULL) && (cur->get_address() != adr) )
	{
#ifdef PERSIST_TRACE
	  /*	  TRACE3("cur %x cur->get_address %x adr %x",
			 cur,
			 cur->get_address(),
			 adr) ;*/
#endif
	  cur = cur->get_next() ;
	}

  if (cur != NULL)
	{
#ifdef PERSIST_TRACE
	  TRACE3("Trouve dans la table ! %x, adr = %x, index = %d",
			 cur,
			 cur->get_address(),
			 cur->get_index()) ;
#endif
	  return cur->get_index() ;
	}

  // Il faut creer un index dans la table
#ifdef PERSIST_TRACE
  TRACE1("on cree un index dans la table pour l'adresse %x", adr) ;
#endif
  cur = new T_disk_object(adr) ;

  // Chainage
#ifdef PERSIST_TRACE
  TRACE0("on chaine l'adresse") ;
#endif
  chain2_add(adr) ;

  return cur->get_index() ;
}
#else // OPTIM_PERSIST
// Obtention de l'index associe a un objet
// Si l'objet est deja dans la table, l'index associe est retourne
// Sinon, on cree une entree dans la table et on insere l'adresse
// dans la liste des adresses a sauvegarder
//
// Version optimisee qui utilise la propriete suivante : la liste
// est triee par ordre croissant des adresses
size_t disk_object_get_object_index(T_object *adr)
{
#ifdef PERSIST_TRACE
  TRACE1("disk_object_get_object_index(%x) OPTIMISE", adr) ;
#endif

#ifdef USE_CACHE
  // On essaye tout d'abord dans le cache
  size_t index ;
  int found = object_index_cache_lookup(adr, &index) ;

  if (found == TRUE)
	{
	  // Trouve
	  return index ;
	}
#endif // USE_CACHE

  // Pas dans le cache ...
  T_disk_object *cur = first_disk_object_sop ;
  T_disk_object *prev = NULL ;

#ifdef PERSIST_TRACE
  TRACE1("first = %x", cur) ;
#endif

  while ( (cur != NULL) && ((size_t)cur->get_address() < (size_t)adr) )
	{
#ifdef PERSIST_TRACE
	  TRACE3("cur %x cur->get_address %x adr %x",
			 cur,
			 cur->get_address(),
			 adr) ;
#endif
	  prev = cur ;
	  cur = cur->get_next() ;
	}

  if ( (cur != NULL) && (cur->get_address() == adr) )
	{
#ifdef PERSIST_TRACE
	  TRACE3("Trouve dans la table ! %x, adr = %x, index = %d",
			 cur,
			 cur->get_address(),
			 cur->get_index()) ;
#endif

#ifdef USE_CACHE
	  // Mise a jour du cache
	  object_index_cache_add(adr, cur->get_index()) ;
#endif // USE_CACHE

	  return cur->get_index() ;
	}

  // Il faut creer un index dans la table
#ifdef PERSIST_TRACE
  TRACE0("on cree un index dans la table") ;
#endif
  cur = new T_disk_object(adr, prev) ;

  // Chainage
#ifdef PERSIST_TRACE
  TRACE0("on chaine l'adresse") ;
#endif
  chain2_add(adr) ;

#ifdef USE_CACHE
  // Mise a jour du cache
  object_index_cache_add(adr, cur->get_index()) ;
#endif // USE_CACHE

  return cur->get_index() ;
}
#endif // !OPTIM_PERSIST

size_t dump_disk_object(void)
{
  TRACE0("dump_disk_object DEBUT") ; ENTER_TRACE ;

  // On memorise l'offset de la table
  size_t cur_offset = disk_manager_sop->get_offset() ;

  // On sauvegarde la table dans l'ordre des indexes
  // A FAIRE :: optimiser !!!
  size_t index = 1 ;

  while (index < cur_rank_si)
	{
	  //	  TRACE1("sauvegarde index %d", index) ;
	  T_disk_object *cur = first_disk_object_sop ;

	  while (cur->get_index() != index)
		{
		  cur = cur->get_next() ;
		  if (cur == NULL)
			{
			  return cur_offset ;
			}
		  assert(cur != NULL) ;
		}

	  cur->dump_disk() ;
	  index++ ;
	}

#if 0
  int rank = 1 ;

  while (rank < cur_rank_si)
	{
	  TRACE1("sauvegarde rank %d", rank) ;
	  T_disk_object *cur = first_disk_object_sop ;

	  while (cur->get_rank() != rank)
		{
		  cur = cur->get_next() ;
		  assert(cur != NULL) ;
		}

	  cur->dump_disk() ;
	  rank++ ;
	}
#endif
  EXIT_TRACE ; TRACE0("dump_disk_object FIN") ;

  // On finit le fichier en ecrivant l'offset de cette table
  return cur_offset ;
}

// Lecture de la table depuis le disque
extern size_t load_disk_object(void *start, size_t len)
{
  size_t nb_elem = len / (2 * sizeof(int)) ;

  TRACE3("load_disk_object start = 0x%x, len = %d -> %d elems",
		 start,
		 len,
		 nb_elem) ;
  ENTER_TRACE ;

  size_t cur_index = 1 ; // on commence a 1 car 0 = pointeur NULL

  first_disk_object_sop = NULL ;
  last_disk_object_sop = NULL ;

  while (cur_index <= nb_elem)
	{
	  size_t offset = *(int *)start ;
	  start = (void *)((size_t)start + sizeof(int)) ;
	  size_t rank = *(int *)start ;
	  (void)new T_disk_object(cur_index, ntohl(offset), ntohl(rank)) ;
	  cur_index++ ;
	  start = (void *)((size_t)start + sizeof(int)) ;
	}

  EXIT_TRACE ;
  return first_disk_object_sop->get_rank() ;
}

// Fonction auxiliaire qui recherche l'objet a un rang donne
// Mise a jour de l'adresse pour un rang
static T_disk_object *find_object_at_rank(size_t rank)
{
#ifdef PERSIST_TRACE
  TRACE2("find_object_at_rank(%d) -> %x", rank, rank_to_object_sot[rank]) ;
#endif
  assert(0 <= rank && rank < rank_to_object_size_si) ;
  return rank_to_object_sot[rank] ;
}

// Mise a jour de l'adresse pour un rang
T_disk_object *disk_object_set_address(size_t rank,
												T_object *new_address)
{
#ifdef PERSIST_TRACE
  TRACE2("disk_object_set_address(%d, %x)", rank, new_address) ;
#endif

  T_disk_object *cur = find_object_at_rank(rank) ;

  if (cur != NULL)
	{
	  cur->set_address(new_address) ;
	  return cur ;
	}

#ifdef PERSIST_TRACE
  TRACE1("pb :  on n'a pas trouve d'objet au rang %d", rank) ;
#endif
  toplevel_error(FATAL_ERROR,
				 get_error_msg(E_UNABLE_TO_FIND_OBJECT_AT_RANK),
				 rank) ;
  return NULL ;
}

#ifdef PERSIST_TRACE
void *_disk_object_find_address(const char *file, int line, size_t index)
#else
  void *disk_object_find_address(size_t index)
#endif
{
  if (index == 0)
	{
#ifdef PERSIST_TRACE
	  TRACE0("disk_object_find_address(0) -> NULL") ;
#endif
	  return NULL ;
	}

#ifdef PERSIST_TRACE
  TRACE3("disk_object_find_address(%d) appele depuis %s::%d", index, file, line) ;
#endif

  T_disk_object *cur = find_object_at_rank(index) ;

  if (cur != NULL)
	{
	  if (cur->get_address() == NULL)
		{
#ifdef PERSIST_TRACE
		  TRACE1("ALARM :: index %d is at address NULL", index) ;
#endif
		}

	  else
		{
#ifdef PERSIST_TRACE
		  TRACE4("index %d is at address 0x%x, offset %d (class %s)",
				 index,
				 cur->get_address(),
				 cur->get_offset(),
				 (object_test(cur->get_address()) == TRUE) ?
				 (((T_object *)cur->get_address())->get_class_name()) :
				 "<not an object>") ;
#endif

		  if ( 	(cur->get_solved() == FALSE)
				&& (object_test(cur->get_address()) == TRUE))
			{
			  cur->set_solved(TRUE) ;
			  T_object *object = (T_object *)cur->get_address() ;
#ifdef PERSIST_TRACE
			  TRACE1("DEBUT APPEL RECURSIF POUR Ox%x", object) ;
			  TRACE2("type %d, (class %s)",
					 ((T_item *)object)->get_node_type(),
					 object->get_class_name()) ;
			  ENTER_TRACE ; ENTER_TRACE ;
#endif
			  object->solve_objects() ;
#ifdef PERSIST_TRACE
			  EXIT_TRACE ; EXIT_TRACE ;
			  TRACE1("FIN   APPEL RECURSIF POUR Ox%x", object) ;
			  TRACE2("type %d, (class %s)",
					 ((T_item *)object)->get_node_type(),
					 object->get_class_name()) ;
#endif
			}
		}

	  return cur->get_address() ;
	}

  toplevel_error(FATAL_ERROR,
				 get_error_msg(E_UNABLE_TO_FIND_OBJECT_AT_RANK),
				 index) ;
  return NULL ;
}


