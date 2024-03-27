/******************************* CLEARSY **************************************
* Date : $Id: c_memory.h,v 2.0 1999-04-20 08:14:08 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Gestion de memoire securisee
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
#ifndef C_MEMORY_H
#define C_MEMORY_H

#include "c_port.h"

// Les fonctions ont des prototypes C pour l'interfacage Betree -> B0tree

// Gestion de memoire securisee, API C

//
// A) Allocations
//
// Si on ne compile pas avec MEMORY_CLEANUP, les deux types
// d'allocations sont strictement identiques
// 1) Bloc persistant (non libere par cleanup)
// Nouvelle appelation
EXTERN void *s_persistent_malloc(size_t size) ;
// Apellation obsolete pour compatibilite ascendante
#define s_malloc(size) s_persistent_malloc(size)
// 2) Bloc volatile (libere par cleanup)
// A utiliser de preference dans le compilateur
EXTERN void *s_volatile_malloc(size_t size) ;

//
// B) Liberation
//
EXTERN void s_free(void *adr) ;

//
// C) Reallocation
//
// Si on ne compile pas avec MEMORY_CLEANUP, les deux types
// de reallocations sont strictement identiques
// Nouvelle appelation
EXTERN void *s_persistent_realloc(void *adr, size_t size) ;
// Apellation obsolete pour compatibilite ascendante
#define s_realloc(adr, size) s_persistent_realloc(adr, size)
// 2) Bloc volatile (libere par cleanup)
// A utiliser de preference dans le compilateur
EXTERN void *s_volatile_realloc(void *adr, size_t size) ;

// Informations
EXTERN void secure_info(void) ;

#endif /* C_MEMORY_H */


