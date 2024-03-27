/******************************* CLEARSY **************************************
* Fichier : $Id: c_subst.cpp,v 2.0 1999-04-20 08:02:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Substitutions
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
RCS_ID("$Id: c_subst.cpp,v 1.4 1999-04-20 08:02:31 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"


T_substitution::~T_substitution(void)
{
TRACE0("T_substitution::T_substitution") ;
}

// Vrai nom de l'operation
T_symbol *T_op_call::get_real_op_name(void)
{
// Nom de l'operation
// 2 cas possibles :
// - cas simple : op_name est un symbole (ex : 'f' dans 'f(x)')
// - cas complique : op_name est un T_op_result (ex : 'f(x)' dans 'f(x)(y)')
if (op_name->is_an_ident() == TRUE)
	{
	return ((T_ident  *)op_name)->get_name() ;
	}
else if (op_name->get_node_type() == NODE_OP_RESULT)
	{
	assert(FALSE) ; // A FAIRE !
	}

TRACE2("aie : op_name %x type %s", op_name, op_name->get_class_name()) ;
ASSERT(FALSE) ;
return NULL ;
}


//
//	}{	Fin du fichier
//
