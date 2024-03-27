/******************************* CLEARSY **************************************
* Fichier : $Id: s_lrstck.h,v 2.0 1998-11-16 10:43:45 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des piles de l'analyseur LR
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
#ifndef _S_LRSTCK_H_
#define _S_LRSTCK_H_

// Type de pile
typedef enum
{
  STYPE_LIFO,
  STYPE_FIFO
} T_stack_type ;

//
//	}{	Template T_lr_stack : pile LIFO ou FIFO generique
//
template <class T>
class T_lr_stack
{
  // Type de la file
  T_stack_type stack_type ;

  // Les elements de la pile
  T			*items ;

  // Taille actuellement allouee pour la pile
  size_t 		allocated_size ;

  // Element actuellement utilise dans la pile (haut)
  size_t 		index ;

  // Bas la pile (bas)
  size_t 		bottom_index ;

public :
  T_lr_stack(T_stack_type new_stack_type = STYPE_LIFO) ;
  ~T_lr_stack(void) ;

	// Empiler un element
  void push(T new_item) ;

  // Depiler nb_items elements
  T pop(size_t nb_items) ;

  // Depiler un element
  T pop(void) { return pop(1) ; } ;

  // Reset de la pile
  void reset_stack(void) ;

  // Obtenir l'element a la profondeur depth de la pile SANS LE DEPILER
  // Par convention depth = 0 represente le haut de pile
  T get_depth(size_t depth) ;

  // Obtenir l'element en haut de piler SANS LE DEPILER
  T get_top(void) { return get_depth(0) ; } ;

  // Obtenir la taille de la pile
  int get_nb_items(void) { return index ; } ;

  // Pour savoir si la pile est vide
  int is_empty(void) ;
} ;

#endif /* _S_LRSTCK_H_ */

