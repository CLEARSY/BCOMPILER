/******************************* CLEARSY **************************************
* Fichier : $Id: c_usedef.h,v 2.0 1999-05-21 07:55:29 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe
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
#ifndef _C_USEDEF_H_
#define _C_USEDEF_H_

#include "c_object.h"

// Type pointeur sur fonction 1
// i.e. fonction qui prend en parametre un object, et qui retourne un int
typedef int (*user_defined_1)(T_object *object) ;

// Type pointeur sur fonction 2
// i.e. fonction qui prend en parametre un object, et qui retourne un (T_object*)
typedef T_object * (*user_defined_2)(T_object *object) ;

// Type pointeur sur fonction 3
// i.e. fonction qui prend en parametre un object, et un argument,
// et qui retourne un int
typedef int (*user_defined_3)(T_object *object, int arg) ;

// Type pointeur sur fonction 4
// i.e. fonction qui prend en parametre un object et un object,
// et qui retourne un (T_object*)
typedef T_object * (*user_defined_4)(T_object *object, T_object *arg) ;

// Type pointeur sur fonction 5
// i.e. fonction qui prend en parametre un object et un object,
// et qui retourne un int
typedef int (*user_defined_5)(T_object *object, T_object *arg) ;

// Initialisation des "aiguillages" (fait une liberation au prealable
// le cas echeant)
extern void reset_user_defined_switches(void) ;

// Initilisation individuelle de chaque aiguillage
extern void reset_user_defined_f1_switch(void) ;
extern void reset_user_defined_f2_switch(void) ;
extern void reset_user_defined_f3_switch(void) ;
extern void reset_user_defined_f4_switch(void) ;
extern void reset_user_defined_f5_switch(void) ;

// Liberation des "aiguillages"
extern void delete_user_defined_switches(void) ;

// Methodes de mise a jour des "aiguillages"
extern void set_user_defined_f1(T_node_type node_type,
										 user_defined_1 new_function) ;

extern void set_user_defined_f2(T_node_type node_type,
										 user_defined_2 new_function) ;

extern void set_user_defined_f3(T_node_type node_type,
										 user_defined_3 new_function) ;

extern void set_user_defined_f4(T_node_type node_type,
										 user_defined_4 new_function) ;

extern void set_user_defined_f5(T_node_type node_type,
										 user_defined_5 new_function) ;

#endif /* _C_USEDEF_H_ */
