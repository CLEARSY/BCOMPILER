/******************************* CLEARSY **************************************
* Fichier : $Id: c_path.h,v 2.0 1998-01-28 11:13:47 sl Exp $
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
#ifndef _C_PATH_H_
#define _C_PATH_H_

#ifdef __BCOMP__
// Modelisation d'un chemin de recherche
class T_path
{
  // Le chemin de recherche
  char	*path_name ;
  size_t path_len ;

  // Le T_path suivant
  T_path		*next ;

public :
  // Constructeur
  T_path(const char *new_path, T_path **adr_first_path, T_path **adr_last_path) ;

  // Destructeur
  virtual ~T_path() ;

  // Methodes de lecture
  const char *get_path_name(void) { return path_name ; } ;
  size_t get_path_len(void) { return path_len ; } ;
  T_path *get_next(void) 	  { return next ; } ;
} ;

// Obtention de la liste des paths de recherche
extern T_path *get_source_paths(void) ;
#endif

// Ajout d'un path de recherche de fichiers sources
// (seule fonction visible de l'exterieur)
extern void add_source_path(const char *new_source_path) ;

// Reset de la liste des paths des fichiers sources
extern void reset_source_paths(void) ;

#endif /* _C_PATH_H_ */

