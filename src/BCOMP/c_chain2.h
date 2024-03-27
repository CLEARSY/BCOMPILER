/******************************* CLEARSY **************************************
* Fichier : $Id: c_chain2.h,v 2.0 1999-06-09 12:38:13 sl Exp $
* (C) 2008 CLEARSY
*
* Description : Chainage des adresses pour HTML
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
#ifndef C_HCHAIN2_H
#define C_HCHAIN2_H

class T_chain2
{
  // Adresse
  T_object   	*adr ;

  // Deja genere ?
  int		  	generated ;

  // Chainage
  T_chain2   	*prev ;
  T_chain2   	*next ;

public :
  T_chain2(T_object *new_adr, T_chain2 *new_next, T_chain2 *new_prev) ;
  ~T_chain2(void) ;

  // Methodes de lecture
  T_object *get_adr(void) 		{ return adr ; } ;
  T_chain2 *get_next(void)   	{ return next ; } ;
  T_chain2 *get_prev(void)   	{ return prev ; } ;
  int get_generated(void)	   	{ return generated ; } ;

  // Methodes d'ecriture
  void set_next(T_chain2 *new_next) { next = new_next ; } ;
  void set_prev(T_chain2 *new_prev) { prev = new_prev ; } ;
  void set_generated(int new_generated) { generated = new_generated ; } ;
} ;

// Init/Liberation
extern void reset_chain2(T_betree *new_betree) ;

// Ajout
extern int chain2_add(T_object *adr) ;

// Lecture
extern T_object *chain2_get(void) ;

#endif /* C_HCHAIN2_H */


