/***************************** CLEARSY **************************************
* Fichier : $Id: d_dskstr.cpp,v 2.0 1999-04-20 08:02:36 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Chaines persistantes
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
RCS_ID("$Id: d_dskstr.cpp,v 1.6 1999-04-20 08:02:36 sl Exp $") ;

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

static T_disk_string *first_disk_string_sop = NULL ;
static T_disk_string *last_disk_string_sop = NULL ;
static T_disk_manager *disk_manager_sop = NULL ;
static int cur_index_si = 1 ;

//
//	}{	Init ecritures chaines
//
void init_disk_string_write(T_disk_manager *disk_manager)
{
#ifdef FULL_TRACE
  TRACE0("init_disk_string_write") ;
#endif

  first_disk_string_sop = NULL ;
  last_disk_string_sop = NULL ;
  cur_index_si = 1 ;
  disk_manager_sop = disk_manager ;
}

//
//	}{	Init lectures chaines
//
void init_disk_string_read(void)
{
#ifdef FULL_TRACE
  TRACE0("init_disk_string") ;
#endif
  first_disk_string_sop = NULL ;
  last_disk_string_sop = NULL ;
  cur_index_si = 1 ;
}

//
//	}{	Sortie
//
void exit_disk_string()
{
#ifdef FULL_TRACE
  TRACE0("exit_disk_string") ;
#endif
  disk_manager_sop = NULL ;
}

//
//	}{	Creation d'un disk string a partir de son nom
//
T_disk_string::T_disk_string(char *new_string)
{
#ifdef FULL_TRACE
  TRACE1("T_disk_string::T_disk_string(%s)", new_string) ;
#endif
  address = new_string ;
  index = cur_index_si ++ ;

  if (last_disk_string_sop == NULL)
	{
	  first_disk_string_sop = this ;
	}
  else
	{
	  last_disk_string_sop->next = this ;
	}

  last_disk_string_sop = this ;
  next = NULL ;
#ifdef FULL_TRACE
  TRACE0("fin de T_disk_string::T_disk_string") ;
#endif
}

//
//	}{	Creation d'un disk string a partir de son nom et de sa lgr
//
T_disk_string::T_disk_string(char *new_string, size_t new_len)
{
#ifdef FULL_TRACE
  TRACE1("T_disk_string::T_disk_string(%x)", new_string) ;
#endif

  address = new char[new_len + 1] ;
#ifdef STATS_ALLOC
  add_alloc(address, new_len + 1, __FILE__, __LINE__) ;
#endif
  char *p = address ;
  size_t cpt = 0 ;

  while (cpt++ < new_len)
	{
	  *(p++) = *(new_string++) ;
	}

  *p = '\0' ;

#ifdef FULL_TRACE
  TRACE1("new_string(\"%s\")", address) ;
  TRACE2("string = %x, value = \"%s\"", address, address) ;
#endif

  index = cur_index_si ++ ;

  if (last_disk_string_sop == NULL)
	{
	  first_disk_string_sop = this ;
	}
  else
	{
	  last_disk_string_sop->next = this ;
	}

  last_disk_string_sop = this ;
}

void T_disk_string::dump_disk(void)
{
#ifdef FULL_TRACE
  TRACE1("T_disk_string(%x)::dump_disk", this) ;
#endif
  disk_manager_sop->write_string(address, strlen(address)) ;
}

//
//	]{	Recherche d'une chaine
//	A FAIRE :: optimiser
T_disk_string *find_string(const char *str)
{
#ifdef FULL_TRACE
  TRACE1("find_string(\"%s\")", str) ;
#endif

  T_disk_string *cur = first_disk_string_sop ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return NULL ;
		}

	  if (strcmp(cur->address, str) == 0)
		{
		  return cur ;
		}

	  cur = cur->next ;
	}
}

//
// 	}{	Obtention de l'index associe a un objet
// Si l'objet est deja dans la table, l'index associe est retourne
// Sinon, on cree une entree dans la table et on insere l'adresse
// dans la liste des adresses a sauvegarder
//
size_t disk_string_get_string_index(const char *str)
{
#ifdef FULL_TRACE
  TRACE1("disk_string_get_string_index(%s)", (str == NULL) ? "<NULL>" : str) ;
#endif

  T_disk_string *new_string = find_string(str) ;

  if (new_string == NULL)
	{
#ifdef FULL_TRACE
	  TRACE0("on cree l'entree...") ;
#endif
	  // On caste str en (char *) et pas (const char *)
	  // Un petit mensonge bien utile car il n'est pas evident de transformer
	  // le prototype de T_disk_string::T_disk_string(char *) en
	  // T_disk_string::T_disk_string(const char *), bien que la valeur ne
	  // soit jamais modifiee dans les faits
	  new_string = new T_disk_string((char *)str) ;
	}

  return new_string->get_index() ;
}

//
//	}{	Ecriture des chaines
//
size_t dump_disk_string(void)
{
  TRACE1("dump_disk_string DEBUT disk_manager = %x", disk_manager_sop) ;
  ENTER_TRACE ;

  // On memorise l'offset de la table
  size_t cur_offset = disk_manager_sop->get_offset() ;

  TRACE1("offset debut table = 0x%x", cur_offset) ;

  // On sauvegarde la table dans l'ordre des indexes
  // A FAIRE :: optimiser !!!
  T_disk_string *cur = first_disk_string_sop ;

  while (cur != NULL)
	{
	  cur->dump_disk() ;
	  cur = cur->get_next() ;
	}

  // ON ALIGNE SUR sizeof(void *) (4)
  TRACE0("alignement sur sizeof(void *)") ;


  int base = disk_manager_sop->get_offset() / sizeof(void *) ;
  size_t multiple = (base * sizeof(void *)) ;
  if (multiple < disk_manager_sop->get_offset())
	{
	  multiple += sizeof(void *) ;
	}

  int align = multiple - disk_manager_sop->get_offset() ;

  TRACE1("offset courant = %d", disk_manager_sop->get_offset()) ;
  TRACE1("caracteres de bourrage = %d", align) ;

  if (align != 0)
	{
	  TRACE2("ajout de %d carac align i.e. une string de %d carac",
			 align, align-1) ;
	  // !!! ON UTILISE (align - 1) car write_string ecrit un 0 de fin
	  disk_manager_sop->write_string("                  ", align - 1) ;
	}

  EXIT_TRACE ; TRACE0("dump_disk_string FIN") ;

  return cur_offset ;
}

//
//	}{	Lecture de la table depuis le disque
//
extern void load_disk_string(void *start, size_t len)
{
  TRACE2("load_disk_string start = 0x%x, len = %d", start, len) ;
  ENTER_TRACE ;

  first_disk_string_sop = NULL ;
  last_disk_string_sop = NULL ;

  char *cur = (char *)start ;
  char *end = (char *)((size_t) start + len) ;

  while (cur < end)
	{
	  char *deb = (char *)cur ;

	  // On lit la chaine
	  while (*(cur) != '\0') cur++ ;

	  // On cree l'entree associee
	  (void)new T_disk_string(deb, cur - deb) ;

	  // On saute le '\0'
	  cur ++ ;
	}

  EXIT_TRACE ;
  TRACE0("fin de load_disk_string") ;
}

//
//	}{	Obtention d'une chaine a un index donne
//
char *disk_string_find_string(size_t index)
{
  TRACE1("disk_string_find_string(%d)", index) ;
  if (index == 0)
	{
	  TRACE0("disk_string_find_string(0) -> NULL") ;
	  return NULL ;
	}

#ifdef FULL_TRACE
  TRACE1("disk_string_find_string(%d)", index) ;
#endif

  T_disk_string *cur = first_disk_string_sop ;

  while ( (cur != NULL) && (cur->get_index() != index) )
	{
#ifdef FULL_TRACE
	  TRACE3("cur = %x index = %d ref_index = %x", cur, cur->get_index(), index) ;
#endif
	  cur = cur->get_next() ;
	}

  if (cur != NULL)
	{
#ifdef FULL_TRACE
	  TRACE1("trouve : \"%s\"", cur->get_address()) ;
#endif
	  return (char *)cur->get_address() ;
	}

  assert(FALSE) ;
  return NULL ;
}

//
//	}{	Fin du fichier
//

