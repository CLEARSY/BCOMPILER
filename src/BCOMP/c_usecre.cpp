/******************************* CLEARSY **************************************
* Fichier : $Id: c_usecre.cpp,v 2.0 1999-06-09 12:38:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Methodes utilisateur de creation de classes
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
RCS_ID("$Id: c_usecre.cpp,v 1.7 1999-06-09 12:38:31 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Initialisation du module de methodes utilisateur
static T_user_create_method *user_create_methods_sfp = NULL ;

static int init_user_create_methods_done_si = FALSE ;

void init_user_create_methods(void)
{
  TRACE0("init_user_create_methods") ;

  if (init_user_create_methods_done_si == TRUE)
	{
	  TRACE0("deja fait") ;
	  return ;
	}

  init_user_create_methods_done_si = TRUE ;

#if 0
  // Decommenter pour debugger les controles de securite
  TRACE0("DEBUT DEBUG") ;

  char *toto = new char[10] ;
  //  sprintf(toto, "1234567890") ;
  set_block_state(toto, MINFO_PERSISTENT) ;
  recursive_set_block_state(toto, MINFO_VOLATILE) ;
   delete [] toto  ;
  //delete [] (toto +1) ;

  TRACE0("FIN DEBUG") ;
  exit(1) ;
#endif

  user_create_methods_sfp = new T_user_create_method[NODE_USER_DEFINED20 + 1] ;
}

// Prise en compte d'une methode de creation
void set_user_create_method(T_node_type node_type,
									 T_user_create_method method)
{
  TRACE2("set_user_create_method(%d, %x)", node_type, method) ;
  ASSERT(node_type < NODE_USER_DEFINED20) ;
  user_create_methods_sfp[node_type] = method ;
}

// Obtention d'une methode de creation
T_user_create_method get_user_create_method(T_node_type node_type)
{
  ASSERT(node_type < NODE_USER_DEFINED20) ;
  TRACE2("get_user_create_method(%d)-> %x)",
		 node_type,
		 user_create_methods_sfp[node_type]) ;

  return user_create_methods_sfp[node_type] ;
}

//
// Fin du fichier
//
