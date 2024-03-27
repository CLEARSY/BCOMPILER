/******************************* CLEARSY **************************************
* Fichier : $Id: c_projdb.cpp,v 2.0 1998-01-29 11:24:07 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Interfacage avec la base de donnees projet du
*					Moniteur de Session
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
RCS_ID("$Id: c_projdb.cpp,v 1.2 1998-01-29 11:24:07 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Prise en compte de la base de donnees projet
void load_project_database(const char *database_file_name)
{
  TRACE1("load_project_database(\"%s\")", database_file_name) ;

  // Initialisation du gestionnaire de projets, si ce n'est deja fait
  init_project_manager() ;

  // Chargement du projet
    int exit_status = get_project_manager()->load_current_project(database_file_name) ;
    if (exit_status)
    {
      // A FAIRE : ameliorer ce message
      toplevel_error(FATAL_ERROR,
                     get_error_msg(E_NO_SUCH_FILE),
                     database_file_name,
                     strerror(errno)) ;
    }
}

/**
 * @brief load_project_library loads a library into the project
 * @param library_file_name the fully qualified pat and name of the library
 */
void load_project_library(const char *library_file_name)
{
  TRACE1("load_project_library(\"%s\")", library_file_name) ;


  // try to get the pm
  T_project_manager *pPm = get_project_manager();
  if (  pPm == NULL )
  {
    // we don't have one yet, so we init it
    init_project_manager();
    // see if the init has worked
    pPm = get_project_manager();
  }


  if (  pPm != NULL )
  {
    T_project * lib = pPm->load_library(library_file_name);
    if (lib == NULL)
    {
      // A FAIRE : ameliorer ce message
      toplevel_error(FATAL_ERROR,
                     get_error_msg(E_NO_SUCH_FILE),
                     library_file_name,
                     strerror(errno)) ;
    }
    else
    {
        // all went well, the lib has been loaded!
    }
  }
  else
  {
      // no project manager
  }
}


