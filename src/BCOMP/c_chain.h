/******************************* CLEARSY **************************************
* Fichier : $Id: c_chain.h,v 2.0 1999-03-02 16:43:58 sl Exp $
* (C) 2008 CLEARSY
*
* Description : Chainage des adresses pour HTML
*				Module interne au compilateur B
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
#ifndef C_HCHAIN_H
#define C_HCHAIN_H

class T_chain
{
  T_object   	*adr ;
  int		  	generated ;
  T_chain   	*prev ;
  T_chain   	*next ;

public :
#ifdef PORTABLE_CHAIN
  T_chain(T_object *new_adr, int line) ;	// Version portable (pour debug)
#else /* ! PORTABLE_CHAIN */
  T_chain(T_object *new_adr,		// Version optimisee
		  T_chain *new_next,
		  T_chain *new_prev,
		  int line) ;
#endif /* PORTABLE_CHAIN */
  ~T_chain() ;

  T_object *get_adr(void)	   				{ return adr ; } ;
  T_chain *get_next(void)   				{ return next ; } ;
  int get_generated(void)					{ return generated ; } ;
  void set_generated(int new_generated)	{ generated = new_generated ; } ;
} ;

//
//	}{	Methodes exportees
//
extern void chain_reset(T_betree *new_betree) ;
extern int chain_add(T_object *adr) ;
extern T_object *chain_get(void) ;

#ifdef STATS_CHAIN
extern void get_chain_stats(void) ;
#endif


#endif /* C_HCHAIN_H */


