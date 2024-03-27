/******************************* CLEARSY **************************************
* Fichier : $Id: s_pstack.h,v 2.0 1997-11-03 08:53:55 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Pile de memorisation des positions des lexemes
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
#ifndef _S_PSTACK_H_
#define _S_PSTACK_H_

class T_keep_pos
	{
	int		line ;
	int		column ;
	public :
	int get_line() { return line ; } ;
	int get_column() { return column ; } ;
	void set_pos(int new_line, int new_column)
		{ line = new_line ; column = new_column ; } ;
	} ;

class T_pstack				/* LIFO des positions */
	{
	T_keep_pos	*stack ;
	int			cur_push ;
	int			size ;

	public :
	T_pstack(int new_size) ;
	virtual ~T_pstack() ;
	void push(int new_line, int new_column) ;
	void pop(int &line, int &column) ;
	} ;

#endif /* _S_PSTACK_H_ */




