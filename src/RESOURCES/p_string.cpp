/******************************* CLEARSY **************************************
* Fichier : $Id: p_string.cpp,v 2.0 2000/07/11 16:10:26 fl Exp $
* (C) 2008 ClearSy
*
* Description :	Corps du module p_string
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
*
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_string.cpp,v 1.7 2000/07/11 16:10:26 fl Exp $") ;

// includes head
#include "p_head.h"


//Definition des Methodes
// _______________________

// Constructeur avec calcul de taille
T_resource_string::T_resource_string(const char *new_string_acp)
{
  init_string(new_string_acp, strlen(new_string_acp)) ;
}


// Constructeur sans calcul de taille
T_resource_string::T_resource_string(const char *new_string_acp,
												size_t new_length_ai)
{
  init_string(new_string_acp, new_length_ai) ;
}


// Destructeur
T_resource_string::~T_resource_string(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Del STRING %x \"%s\"", this, string_cp) ;
#endif

  delete [] string_cp ;

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Initialise les menbres de la classes
// Factorisation du traitement des constructeurs
void T_resource_string::init_string(const char *new_string_acp,
											  size_t new_length_ai)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> New STRING %x de taille %d \"%s\"",
		 this,
		 new_length_ai,
		 new_string_acp) ;
#endif

  // calcul de la taille de la chaine
  length_i = new_length_ai ;

  // allocation du tableau de char que pointe string
  string_cp = new char [length_i + 1] ;

  // recopie de la chaine
  strcpy(string_cp, new_string_acp) ;
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Compare deux chaines et retourne
// TRUE si elles sont egales, FALSE sinon.
int
T_resource_string::compare(const T_resource_string *string_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE6("-> Compare STRING %x:%d \"%s\" & %x:%d \"%s\"",
		 this,
		 get_length(),
		 get_string(),
		 string_aop,
		 (string_aop != NULL)?
		 ((T_resource_string *)string_aop)->get_length():0,
		 (string_aop != NULL)?
		 ((T_resource_string *)string_aop)->get_string():"VIDE") ;
#endif

  // Compare les deux chaines seulement si elles ont la meme longeur
  if(    (length_i == string_aop->length_i)
	  && (strcmp(string_cp, string_aop->string_cp) == 0) )
	{
#ifdef DEBUG_RESOURCES
	  TRACE0("<- Compare OK") ;
	  EXIT_TRACE ;
#endif

	  return TRUE ;
	}

#ifdef DEBUG_RESOURCES
  TRACE0("<- Compare NONE") ;
  EXIT_TRACE ;
#endif

  return FALSE ;
}


//impression de la chaine dans un fichier
void T_resource_string::print(FILE *fd)
{
#ifdef DEBUG_RESOURCES
  TRACE1("-> T_resource_string::print(%x)", fd) ;
#endif
  fputs(string_cp ,fd) ;
#ifdef DEBUG_RESOURCES
  TRACE1("<- T_resource_string::print(%x)", fd) ;
#endif
}


// Definition des Fonctions
// ___________________

// Copie une chaine source en une nouvelle chaine
// Retoune l'adresse de la nouvelle chaine
T_resource_string *string_copy(T_resource_string *src_string_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> Copy STRING %x:%d \"%s\"",
		 src_string_aop,
		 (src_string_aop != NULL)? src_string_aop->get_length():0,
		 (src_string_aop != NULL)? src_string_aop->get_string():"VIDE") ;
#endif

  // Creation de la nouvelle chaine
  T_resource_string *dst_string_lop =
	new T_resource_string(src_string_aop->get_string()) ;
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  // Retourne la nouvelle chaine
  return dst_string_lop ;
}

// Concatene une premiere chaine et une seconde en une nouvelle
// Retoune l'adresse de la nouvelle chaine
/*
T_resource_string *
string_concat(T_resource_string *first_string_aop,
			  T_resource_string *second_string_aop)
{
  ENTER_TRACE ;
  TRACE6("Concat STRING %x:%d \"%s\" & %x:%d \"%s\"",
		 first_string_aop,
		 (first_string_aop != NULL)? first_string_aop->get_length():0,
		 (first_string_aop != NULL)? first_string_aop->get_string():"VIDE",
		 second_string_aop,
		 (second_string_aop != NULL)? second_string_aop->get_length():0,
		 (second_string_aop != NULL)? second_string_aop->get_string():"VIDE") ;

  // Calcul la taille cumulle des deux chaines
  int new_length_li =
	first_string_aop->get_length() + second_string_aop->get_length() ;

  // Cree un tableau tampon
  char *new_string_lcp = new char [new_length_li + 1] ;

  // Concatenation des chaines
  sprintf(new_string_lcp,
		  "%s%s",
		  first_string_aop->get_string(),
		  second_string_aop->get_string()) ;

  // Creation de la nouvelle chaine
  T_resource_string *dst_string_lop =
	new T_resource_string(new_string_lcp, new_length_li) ;

  // Destruction le tableau
  delete [] new_string_lcp ;
  new_string_lcp = NULL ;

  EXIT_TRACE ;

  // Retourne la nouvelle chaine
  return dst_string_lop ;
}
*/

// Fin de fichier
// _______________
