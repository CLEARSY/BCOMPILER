/******************************* CLEARSY **************************************
* Fichier : $Id: p_string.h,v 2.0 2007/07/27 13:23:53 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_string
* 				Modelisation d'une chaine de caracteres.
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
#ifndef _P_STRING_H_
#define _P_STRING_H_

#include <cstdio>

// Definition des Classes
// _________________

class T_resource_string
{
  // taille de la chaine
  size_t length_i ;

  // pointe sur la chaine
  char *string_cp ;

protected :

  // Initialise les menbres de la classes
  // Factorisation du traitement des constructeurs
  void init_string(const char *new_string_acp,
                            size_t new_length_ai) ;
public:

  // Constructeur avec calcul de taille
  T_resource_string(const char *new_string_acp) ;

  // Constructeur sans calcul de taille
  T_resource_string(const char *new_string_acp, size_t new_length_ai) ;

  // Destructeur
  ~T_resource_string(void) ;

  // retourne la taille de la chaine
  size_t get_length(void)
	{
	  return length_i ;
	}

  // retourne le poiteur sur la chaine
  const char * const get_string(void)
	{
	  return string_cp ;
	}

  // Compare deux chaines et retourne
  // TRUE si elles sont egales, FALSE sinon.
  int compare(const T_resource_string *string_aop) ;

  //impression de la chaine dans un fichier
  void print(FILE *fd) ;

} ;


// Declaration des Fonctions
// _____________________

// Copie une chaine source en une nouvelle chaine
// Retoune l'adresse de la nouvelle chaine
extern T_resource_string *
string_copy(T_resource_string *src_string_aop) ;

// Concatene une premiere chaine et une seconde en une nouvelle
// Retoune l'adresse de la nouvelle chaine
/*
extern T_resource_string *
string_concat(T_resource_string *first_string_aop,
			  T_resource_string *second_string_aop) ;
*/

#endif // _P_STRING_H_
