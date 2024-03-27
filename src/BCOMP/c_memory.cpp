/******************************* CLEARSY **************************************
* Fichier : $Id: c_memory.cpp,v 2.0 2007-12-13 11:12:41 arequet Exp $
* (C) 2008 CLEARSY
*
* Compilations :	(i) MEMORY_CHECK pour verifier allocations/liberations
*					+ bilan
*					(ii) TRACE_ALLOC pour voir les allocations/liberations
*					(iii) PATCH_CHECK pour verifier les debordements
*					(ii) et (iii) necessitent (i)
*					-DSTOP_AT_ALLOC + stop_alloc_at_number() permettent un arret
*					a une allocation donnee
*					-DSTOP_AT_FREE + stop_free_at_number() permettent un arret
*					a une liberation donnee
*
*					-DDEBUG_MEMORY pour debugger ce module
*
* Description :		Gestion du memoire avec detection d'erreurs
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
RCS_ID("$Id: c_memory.cpp,v 1.14 2007-12-13 11:12:41 arequet Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Includes locaux
#include "c_api.h"

//
// API C
//

// Allocation persistante
EXTERN void *s_persistent_malloc(size_t size)
{
  void *res = malloc(size) ;
  TRACE2("s_persistent_malloc(%d) -> %p", size, res);
  return res ;
}

// Allocation volatile
EXTERN void *s_volatile_malloc(size_t size)
{
  void *res = malloc(size) ;
  TRACE2("s_volatile_malloc(%d) --> %p", size, res) ;
  return res ;
}

// Reallocation volatile
EXTERN void *s_volatile_realloc(void *adr, size_t size)
{
  void *res = realloc(adr, size) ;
  TRACE3("s_volatile_realloc(%x, %d) --> %p", adr, size, res) ;
  return res ;
}

// Reallocation persistante
EXTERN void *s_persistent_realloc(void *adr, size_t size)
{
  void *res = realloc(adr, size) ;
  TRACE3("s_persistent_realloc(%x, %d) --> %p", adr, size, res) ;
  return res ;
}

// Liberation
EXTERN void s_free(void *adr_ap)
{
  free(adr_ap) ;
}

//
// Fin du fichier
//
