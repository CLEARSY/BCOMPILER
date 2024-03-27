/******************************* CLEARSY **************************************
* Date : $Id: c_cleanup.h,v 2.0 1999-07-21 07:42:55 sl Exp $
*
* (C) 2008 CLEARSY
* Description :		Garbage collector du compilateur
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
#ifndef _C_CLEANUP_H
#define _C_CLEANUP_H

// Certaines fonctions ont une interface C pour pouvoir etre appelles depuis le MS

// Lancer le garbage collector
extern void memory_cleanup(void) ;
// Compatibilite avec l'ancienne API
#define s_memory_cleanup memory_cleanup

// Etats d'un bloc
typedef enum
{
  //
  // 1) Blocs dont l'etat peut etre change par set_block_state
  //

  // Bloc persistant
  MINFO_PERSISTENT,
  // Bloc volatile
  MINFO_VOLATILE,

  //
  // Blocs dont l'etat peut ne peut pas etre change par set_block_state
  //

  // Bloc persistant
  MINFO_ALWAYS_PERSISTENT,
  // Bloc volatile
  MINFO_ALWAYS_VOLATILE
} T_memory_state ;

#endif // _C_CLEANUP_H


