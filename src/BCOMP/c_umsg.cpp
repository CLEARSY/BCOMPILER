/******************************* CLEARSY **************************************
* Fichier : $Id: c_umsg.cpp,v 2.0 1999-06-09 12:38:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Gestionnaire de messages utilisateur
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

/* Includes Composant Local */
#include "c_api.h"

// Ajout d'un message d'erreur utilisateur
// Renvoie l'identifiant associe
static size_t cpt_user_err = 0 ;
static char **user_err_tbl = NULL ;
static size_t user_err_tbl_allocated = 0 ;
static size_t user_err_free = 0 ;
static const size_t user_err_realloc_step = 128 ; /* pas de reallocation */

void reset_user_error_msg(void)
{
  cpt_user_err = 0 ;
  user_err_tbl = NULL ;
  user_err_tbl_allocated = 0 ;
  user_err_free = 0 ;
}

T_user_error_code add_user_error_msg(const char *new_message)
{
#ifdef FULL_TRACE
  TRACE1("add_user_error_message(\"%s\")", new_message) ;
#endif

  if (user_err_free == 0)
	{
#ifdef FULL_TRACE
	  TRACE1("on realloue de la place pour %d messages", user_err_realloc_step) ;
#endif

	  if (user_err_tbl_allocated != 0)
		{
		  char **new_user_err_tbl = new
			char *[user_err_tbl_allocated + user_err_realloc_step * sizeof(char *)];
#ifdef STATS_ALLOC
		  add_alloc(*user_err_tbl,
					user_err_tbl_allocated + user_err_realloc_step * sizeof(char *),
					__FILE__,
					__LINE__);
#endif
		  memcpy(new_user_err_tbl, user_err_tbl, user_err_tbl_allocated) ;
		  delete [] user_err_tbl ;
		  user_err_tbl = new_user_err_tbl ;
		  user_err_tbl_allocated += user_err_realloc_step ;
		  user_err_free = user_err_realloc_step ;
		}
	  else
		{
		  user_err_tbl_allocated = user_err_realloc_step ;
		  user_err_tbl = new char *[user_err_tbl_allocated * sizeof(char *)] ;
#ifdef STATS_ALLOC
		  add_alloc(*user_err_tbl,
					user_err_tbl_allocated * sizeof(char *),
					__FILE__,
					__LINE__) ;
#endif
		  user_err_free = user_err_realloc_step ;
		}
	}

  // On peut caster en char * car on sait que le message ne sera pas lu
  user_err_tbl[cpt_user_err] = (char *)new_message ;

  // Les messages ne sont pas liberes par defaut
  // Mais on ne les met pas persistants car ils proviennent de chaines
  // statiques, ex: add_user_error_msg("bla bla") ;
  // Si l'utilisateur appelle avec une chaine allouee, il doit la rendre
  // persistante (A DOCUMENTER)
  // set_block_state((char *)new_message, MINFO_ALWAYS_PERSISTENT) ;

  user_err_free --  ;
  return cpt_user_err++ ;
}

// Ajout d'un message d'avertissement utilisateur
// Renvoie l'identifiant associe
static size_t cpt_user_warn = 0 ;
static char **user_warn_tbl = NULL ;
static size_t user_warn_tbl_allocated = 0 ;
static size_t user_warn_free = 0 ;
static const size_t user_warn_realloc_step = 128 ; /* pas de reallocation */

void reset_user_warning_msg(void)
{
  cpt_user_warn = 0 ;
  user_warn_tbl = NULL ;
  user_warn_tbl_allocated = 0 ;
  user_warn_free = 0 ;
}



T_user_warning_code add_user_warning_msg(const char *new_message)
{
#ifdef FULL_TRACE
  TRACE1("add_user_warnor_message(\"%s\")", new_message) ;
#endif

  if (user_warn_free == 0)
	{
#ifdef FULL_TRACE
	  TRACE1("on realloue de la place pour %d messages", user_warn_realloc_step) ;
#endif
	  if (user_warn_tbl_allocated != 0)
		{
		  char **new_user_warn_tbl = new
			char *[user_warn_tbl_allocated + user_warn_realloc_step*sizeof(char *)];
#ifdef STATS_ALLOC
		  add_alloc(*user_warn_tbl,
					user_warn_tbl_allocated + user_warn_realloc_step*sizeof(char *),
					__FILE__,
					__LINE__);
#endif
		  memcpy(new_user_warn_tbl, user_warn_tbl, user_warn_tbl_allocated) ;
		  delete [] user_warn_tbl ;
		  user_warn_tbl = new_user_warn_tbl ;
		  user_warn_tbl_allocated += user_warn_realloc_step ;
		  user_warn_free = user_warn_realloc_step ;
		}
	  else
		{
		  user_warn_tbl_allocated = user_warn_realloc_step ;
		  user_warn_tbl = new char *[user_warn_tbl_allocated * sizeof(char *)] ;
#ifdef STATS_ALLOC
		  add_alloc(*user_warn_tbl,
					user_warn_tbl_allocated * sizeof(char *),
					__FILE__,
					__LINE__) ;
#endif
		  user_warn_free = user_warn_realloc_step ;
		}
	}

  // On peut caster en char * car on sait que le message ne sera pas lu
  user_warn_tbl[cpt_user_warn] = (char *)new_message ;

  // Les messages ne sont pas liberes par defaut
  // On peut caster en char * car on sait que l'operation ne modifie pas le char *
  // Mais on ne les met pas persistants car ils proviennent de chaines
  // statiques, ex: add_user_error_msg("bla bla") ;
  // Si l'utilisateur appelle avec une chaine allouee, il doit la rendre
  // persistante (A DOCUMENTER)
  //set_block_state((char *)new_message, MINFO_ALWAYS_PERSISTENT) ;

  user_warn_free -- ;
  return cpt_user_warn++ ;
}

#ifdef TRACE
const char *_get_user_error_msg(T_user_error_code error_code,
										 const char *file,
										 int line)

#else // !TRACE
  const char *get_user_error_msg(T_user_error_code error_code)
#endif // TRACE
{
  ASSERT(error_code <= cpt_user_err) ;
  TRACE2("get_user_error_msg appele depuis %s:%d", file, line) ;
  return user_err_tbl[error_code] ;
}

const char *get_user_warning_msg(T_user_warning_code warning_code)
{
  ASSERT(warning_code <= cpt_user_warn) ;
  return user_warn_tbl[warning_code] ;
}

//
//	}{	Fin du fichier
//

