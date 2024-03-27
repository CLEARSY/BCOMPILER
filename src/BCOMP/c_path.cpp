/******************************* CLEARSY **************************************
* Fichier : $Id: c_path.cpp,v 2.0 1999-06-09 12:38:27 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des chemins de recherche
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
RCS_ID("$Id: c_path.cpp,v 1.8 1999-06-09 12:38:27 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_trace.h"
#include "c_path.h"
#include "c_cleanup.h"
#include "c_lex.h"

//
//	}{ Classe T_path
//

static T_path *first_source_path_sop = NULL ;
static T_path *last_source_path_sop = NULL ;

// Constructeur
T_path::T_path(const char *new_path_name,
						T_path **adr_first_path,
						T_path **adr_last_path)
{
  TRACE1("T_path::T_path(%s)", new_path_name) ;

  path_len = clone(&path_name, new_path_name) ;

  // Chainage en queue
  next = NULL ;

  if (*adr_first_path == NULL)
	{
	  *adr_first_path = this ;
	}
  else
	{
	  (*adr_last_path)->next = this ;
	}

  (*adr_last_path) = this ;
}

// Destructeur
T_path::~T_path(void)
{
  free(path_name) ;
}

//
//	}{	API
//

// Ajout d'un path
void add_source_path(const char *new_path)
{
  TRACE1("add_source_path(\"%s\")", new_path) ;
  // On cree l'objet associe
  (void)new T_path(new_path, &first_source_path_sop, &last_source_path_sop) ;
}

// Obtention de la liste des paths de recherche
T_path *get_source_paths(void)
{
  return first_source_path_sop ;
}


// Reset de la liste des paths des fichiers sources
void reset_source_paths(void)
{
  TRACE0("reset_source_paths") ;

  T_path *cur_path = first_source_path_sop ;

  while (cur_path != NULL)
	{
	  T_path *next = cur_path->get_next() ;
	  delete cur_path ;
	  cur_path = next ;
	}


  first_source_path_sop = NULL ;
  last_source_path_sop = NULL ;
}

