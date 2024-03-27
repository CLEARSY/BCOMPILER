/******************************* CLEARSY **************************************
* Fichier : $Id: s_lrstck.cpp,v 2.0 1999-10-08 10:54:00 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Piles de l'analyseur LR
*
* Compilation :		-DDEBUG_STACK pour avoir des traces
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
#ifdef DEFINE_TEMPLATES
// Le module des templates n'est pas tout a fait un module comme les
// autres car on fait #include de ce fichier source pour les instanciations
// de templates
// Il n'a donc pas une structure usuelle car il est inclus par d'autres
// fichier sources. Le #ifdef en debut de fichier evite une compilation
// automatique intempestive par Visual-c++. Corrollaire : il faut ecrire
// #define DEFINE_TEMPLATES puis #include "s_lrstck.cpp" lors de
// l'instanciation explicite des templates

#include "s_lrstck.h"

//
//	}{	Template T_lr_stack : pile LIFO generique
//

// Pas de reallocation pour la pile
static const size_t stack_realloc_size_si = 1024 ;

//
//	}{	Constructeur
//
template <class T> T_lr_stack<T>::T_lr_stack(T_stack_type new_stack_type)
{
#ifdef DEBUG_STACK
TRACE1("debut T_lr_stack(%x)::T_lr_stack", this) ;
ENTER_TRACE ;
#endif

stack_type = new_stack_type ;

items = new T[stack_realloc_size_si] ;
allocated_size = stack_realloc_size_si ;

#ifdef DEBUG_STACK
TRACE2("STACK allocated %d realloc %d", allocated_size, stack_realloc_size_si) ;
#endif // DEBUG_STACK

index = 0 ;
bottom_index = 0 ;

#ifdef DEBUG_STACK
EXIT_TRACE ;
TRACE0("fin T_lr_stack::T_lr_stack") ;
#endif

/*
  Non : ils sont liberes par cleanup et remis a zero par reset_compiler
 set_block_state(this, MINFO_ALWAYS_PERSISTENT) ;
 set_block_state(items, MINFO_ALWAYS_PERSISTENT) ;
*/

}

//
//	}{	Destructeur
//
template <class T> T_lr_stack<T>::~T_lr_stack(void)
{
    delete[] items ;
}


//
//	}{	Reset
//
template <class T> void T_lr_stack<T>::reset_stack(void)
{
#ifdef DEBUG_STACK
TRACE1("T_lr_stack(%x)::reset_stack()", this) ;
ENTER_TRACE ;
#endif

 index = 0 ;
}

//
//	}{	Empiler
//
template <class T> void T_lr_stack<T>::push(T new_item)
{
#ifdef DEBUG_STACK
TRACE5("T_lr_stack(%x)::push(%x) (index %d, allocated_size %d, remain %d)",
	   this,
	   (size_t)new_item,
	   index,
	   allocated_size,
	   allocated_size - index) ;
#endif

if (allocated_size == index)
	{
	// La pile est pleine !!! on realloue
	TRACE2("on realloue la pile, la taille passe de %d a %d",
		   allocated_size,
		   allocated_size * 2) ;
	allocated_size *= 2 ;
	T *new_items = new T[allocated_size] ;
	memcpy(new_items, items, allocated_size * sizeof(T)) ;
	delete [] items ;
	items = new_items ;
	}

items[index++] = new_item ; // A FAIRE : etudier le link
}

//
//	}{	Depiler n elements
//
template <class T> T T_lr_stack<T>::pop(size_t nb_items)
{
#ifdef DEBUG_STACK
	TRACE3("T_lr_stack(%x)::pop(%d) -> (index %d)",
		   this,
		   nb_items,
		   index) ;
	TRACE6("T_lr_stack(%x)::pop(%d) -> %x (index %d,allocated_size %d,remain %d)",
		   this,
		   nb_items,
		   (index - nb_items < 0) ? 1000*(index-nb_items) : (size_t)(items[index - nb_items]),
		   index,
		   allocated_size,
		   allocated_size - index) ;
#endif
ASSERT(index >= nb_items) ;

// En fifo, on ne sait gerer que pop(1)
ASSERT( (stack_type == STYPE_LIFO) || (nb_items == 1) ) ;

if (stack_type == STYPE_LIFO)
  {
#ifdef DEBUG_STACK
	TRACE6("T_lr_stack(%x)::pop(%d) -> %x (index %d,allocated_size %d,remain %d)",
		   this,
		   nb_items,
		   (size_t)(items[index - nb_items]),
		   index,
		   allocated_size,
		   allocated_size - index) ;
#endif

	index -= nb_items ;
#ifdef DEBUG_STACK
	T res = items[index] ;
	TRACE1("res = %x", (size_t) res) ;
	return res ;
#else
	return items[index] ;
#endif
  }

// Type FIFO
// On ne recupere pas la place "recuperee"
#ifdef DEBUG_STACK
	TRACE7("T_lr_stack(%x)::pop(%d) -> %x (index %d, bottom_index %d, allocated_size %d,remain %d)",
		   this,
		   nb_items,
		   (size_t)(items[index - nb_items]),
		   index,
		   bottom_index,
		   allocated_size,
		   allocated_size - index) ;
	T res = items[bottom_index++] ;
	TRACE1("res = %x", (size_t) res) ;
	return res ;
#else // !DEBUG_STACK
return items[bottom_index++] ;
#endif // DEBUG_STACK
}

//
//	}{	Obtenir l'objet a la profondeur depth la pile SANS DEPILER
//
template <class T> T T_lr_stack<T>::get_depth(size_t depth)
{
#ifdef DEBUG_STACK
TRACE6("T_lr_stack(%x)::get_depth(%d)->%x(index %d, allocated_size %d, remain %d)",
	   this,
	   depth,
	   (size_t)(items[index - (depth + 1)]),
	   index - (depth + 1),
	   allocated_size,
	   allocated_size - index) ;
#endif

return items[index - (depth + 1)] ;
}

// Pour savoir si la pile est vide
template <class T> int T_lr_stack<T>::is_empty(void)
{
  if (stack_type == STYPE_LIFO)
	{
	  return (index == 0) ? TRUE : FALSE ;
	}

  // stack_type == STYPE_FIFO
  return (bottom_index == index) ? TRUE : FALSE ;
}

//
//	}{	Fin du fichier
//
#endif // DEFINE_TEMPLATES
