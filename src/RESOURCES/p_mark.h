/******************************* CLEARSY **************************************
* Fichier : $Id: p_mark.h,v 2.0 1998/12/01 16:20:48 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_mark
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
#ifndef _P_MARK_H_
#define _P_MARK_H_


// Declaration de Fonctions
// _________________________

// Acces au marqueur tilde
extern const char get_RESOURCE_MARK_TILDA(void) ;

// Acces au marqueur de fin de ligne
extern const char get_RESOURCE_MARK_ENDLINE(void) ;

// Acces au marqueur de retour chariot
extern const char get_RESOURCE_MARK_RETURN(void) ;

// Acces au marqueur de tabulation
extern const char get_RESOURCE_MARK_TAB(void) ;

// Acces au marqueur d'espace
extern const char get_RESOURCE_MARK_SPACE(void) ;

// Acces au marqueur de saut a la ligne
extern const char get_RESOURCE_MARK_NEXTLINE(void) ;

// Acces au marqueur de debut de commentaire
extern const char get_RESOURCE_MARK_COMMENT(void) ;

// Acces au marqueur de separation etoile
extern const char get_RESOURCE_MARK_STAR(void) ;

// Acces au marqueur de separation point
extern const char get_RESOURCE_MARK_POINT(void) ;

// Acces au marqueur de separation double point
extern const char get_RESOURCE_MARK_COLON(void) ;

// Acces au marqueur de debut de resource
extern const char *get_RESOURCE_MARK_RESOURCE(void) ;


#endif // _P_MARK_H_
