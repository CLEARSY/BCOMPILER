/******************************* CLEARSY **************************************
* Fichier : $Id: p_mark.cpp,v 2.0 1998/12/01 16:20:48 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_mark
*				Marqueurs des fichiers de resources.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_mark.cpp,v 1.5 1998/12/01 16:20:48 rp Exp $") ;


// Constantes statiques
// _____________________

// Marqueur de tilde
static const char RESOURCE_MARK_TILDA_sc		= '~' ;

// Marqueur de fin de ligne
static const char RESOURCE_MARK_ENDLINE_sc		= '\0' ;

// Marqueur de retour chariot
static const char RESOURCE_MARK_RETURN_sc		= '\n' ;

// Marqueur de tabulation
static const char RESOURCE_MARK_TAB_sc			= '\t' ;

// Marqueur d'espace
static const char RESOURCE_MARK_SPACE_sc		= ' ' ;

// Marqueur de saut a la ligne
static const char RESOURCE_MARK_NEXTLINE_sc		= '\\' ;

// Marqueur de debut de commentaire
static const char RESOURCE_MARK_COMMENT_sc		= '!' ;

// Marqueur de separation etoile
static const char RESOURCE_MARK_STAR_sc			= '*' ;

// Marqueur de separation point
static const char RESOURCE_MARK_POINT_sc		= '.' ;

// Marqueur de separation double point
static const char RESOURCE_MARK_COLON_sc		= ':' ;

// Marqueur de debut de resource
static const char *RESOURCE_MARK_RESOURCE_scp	= "ATB" ;


// Definition de Fonctions
// ________________________

// Acces au marqueur tilde
const char get_RESOURCE_MARK_TILDA()
{
  return RESOURCE_MARK_TILDA_sc ;
}

// Acces au marqueur de fin de ligne
const char get_RESOURCE_MARK_ENDLINE()
{
  return RESOURCE_MARK_ENDLINE_sc ;
}

// Acces au marqueur de retour chariot
const char get_RESOURCE_MARK_RETURN()
{
  return RESOURCE_MARK_RETURN_sc ;
}

// Acces au marqueur de tabulation
const char get_RESOURCE_MARK_TAB()
{
  return RESOURCE_MARK_TAB_sc ;
}

// Acces au marqueur d'espace
const char get_RESOURCE_MARK_SPACE()
{
  return RESOURCE_MARK_SPACE_sc ;
}

// Acces au marqueur de saut a la ligne
const char get_RESOURCE_MARK_NEXTLINE()
{
  return RESOURCE_MARK_NEXTLINE_sc ;
}

// Acces au marqueur de debut de commentaire
const char get_RESOURCE_MARK_COMMENT()
{
  return RESOURCE_MARK_COMMENT_sc ;
}

// Acces au marqueur de separation etoile
const char get_RESOURCE_MARK_STAR()
{
  return RESOURCE_MARK_STAR_sc ;
}

// Acces au marqueur de separation point
const char get_RESOURCE_MARK_POINT()
{
  return RESOURCE_MARK_POINT_sc ;
}

// Acces au marqueur de separation double point
const char get_RESOURCE_MARK_COLON()
{
  return RESOURCE_MARK_COLON_sc ;
}

// Acces au marqueur de debut de resource
const char *get_RESOURCE_MARK_RESOURCE()
{
  return RESOURCE_MARK_RESOURCE_scp ;
}


// Fin de fichier
// _______________
