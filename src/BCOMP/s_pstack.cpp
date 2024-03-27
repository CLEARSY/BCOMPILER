/******************************* CLEARSY **************************************
* Fichier : $Id: s_pstack.cpp,v 2.0 1998-11-16 10:43:46 sl Exp $
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
#include "c_port.h"
RCS_ID("$Id: s_pstack.cpp,v 1.4 1998-11-16 10:43:46 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

/* Includes Composant Local */
#ifdef STACK_TRACE
#include "c_trace.h"
#endif
#include "s_pstack.h"

T_pstack::T_pstack(int new_size)
{
size = new_size ;
stack = new T_keep_pos[size] ;
cur_push = 0 ;
}

T_pstack::~T_pstack()
{
delete [] stack ;
}

void T_pstack::push(int new_line, int new_column)
{
#ifdef STACK_TRACE
TRACE2("T_pstack::push(L%03dC%03d)", new_line, new_column) ;
#endif
assert(cur_push < size) ;
stack[cur_push].set_pos(new_line, new_column) ;
if (++cur_push == size)
	{
	cur_push = 0 ;
	}
}

void T_pstack::pop(int &line, int &column)
{
if (--cur_push == -1)
	{
	cur_push = size - 1 ;
	}
line = stack[cur_push].get_line() ;
column = stack[cur_push].get_column() ;

#ifdef STACK_TRACE
TRACE2("T_pstack::pop(L%03dC%03d)", line, column) ;
#endif

stack[cur_push].set_pos(-1, -1) ;
}
