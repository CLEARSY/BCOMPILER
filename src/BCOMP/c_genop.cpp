/******************************* CLEARSY **************************************
* Fichier : $Id: c_genop.cpp,v 2.0 2002-09-24 09:32:37 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Operations generiques
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
RCS_ID("$Id: c_genop.cpp,v 1.4 2002-09-24 09:32:37 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Constructeur de la classe T_op
T_generic_op::T_generic_op(T_node_type new_node_type,
									T_item **adr_first,
									T_item **adr_last,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem *cur_lexem)
: T_item(new_node_type,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 cur_lexem)
{
}

//
//	Destructeur
//
T_generic_op::~T_generic_op(void)
{
TRACE1("T_generic_op::~T_generic_op(%x)", this) ;
}

// Pour savoir si un item  est une operation
int T_generic_op::is_an_operation(void)
{
return TRUE ;
}


//
//	{	Fin du fichier
//
