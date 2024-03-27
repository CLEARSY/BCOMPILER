/******************************* CLEARSY **************************************
* Fichier : $Id: c_usecre.h,v 1.2 1997-10-23 15:31:18 sl Exp $
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
#ifndef _C_USECRE_H_
#define _C_USECRE_H_

#include "c_object.h"

// Type pointeur sur fonction de creation utilisateur
// Non du type : T_user_create_method
// Parametre de sortie : T_object *
// Parametres d'entree : T_node_type, int &
typedef T_object *(*T_user_create_method)(T_node_type node_type, int &incr) ;

// Initialisation/Liberation du module de methodes utilisateur
extern void init_user_create_methods(void) ;
extern void unlink_user_create_methods(void) ;

// Prise en compte d'une methode de creation
extern void set_user_create_method(T_node_type node_type,
											T_user_create_method method) ;

// Obtention d'une methode de creation
extern T_user_create_method get_user_create_method(T_node_type node_type) ;

#endif /* _C_USECRE_H_ */
