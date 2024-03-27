/******************************* CLEARSY **************************************
* Fichier : $Id: c_char.cpp,v 2.0 1999-04-20 08:02:18 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface d'obtentition de chaines temporaires
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
RCS_ID("$Id: c_char.cpp,v 1.3 1999-04-20 08:02:18 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// La chaine temporaire
static char *local_string_sct = NULL ;

// Pas de reallocation
static const size_t string_alloc_size = 128 ;

// Taille actuellement alloue pour la chaine
static size_t string_size = 0 ;

// Obtention d'une chaine temporaire de taille "au moins size"
// La chaine est valide jusqu'au prochain appel de la fonction
char *get_tmp_string(size_t size)
{
ASSERT(size > 0) ;
if (size > string_size)
	{
	TRACE2("size %d, string_size %d -> il faut reallouer", size, string_size) ;
	// Il faut reallouer
	if (local_string_sct != NULL)
		{
		delete [] local_string_sct ;
		}
	while (string_size <= size)
	  {
		string_size = size + string_alloc_size ;
	  }
	local_string_sct = new char[string_size] ;
#ifdef STATS_ALLOC
	add_alloc(local_string_sct, string_size, __FILE__, __LINE__) ;
#endif
	}

return local_string_sct ;
}

// Obtention d'une chaine de 1Ko
static const int STATIC_STRING_SIZE = 1024 ;
static const int nb_strings = 64 ;
static int cur_string = 0 ;
static char strings[nb_strings][STATIC_STRING_SIZE] ;

char *get_1Ko_string(void)
{
char *res = strings[cur_string ++] ;
if (cur_string == nb_strings)
	{
	cur_string = 0 ;
	}
return res ;
}




