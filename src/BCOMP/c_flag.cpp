/******************************* CLEARSY **************************************
* Fichier : $Id: c_flag.cpp,v 2.0 1999-04-20 08:02:20 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Predicats
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
RCS_ID("$Id: c_flag.cpp,v 1.4 1999-04-20 08:02:20 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_flag
//
T_flag::T_flag(T_item *new_father,
						T_betree *new_betree,
						T_lexem *new_ref_lexem)
: T_item(NODE_FLAG, NULL, NULL, new_father, new_betree, new_ref_lexem)
{
TRACE3("T_flag::T_flag(%x, %x, %x)", new_father, new_betree, new_ref_lexem) ;

TRACE1("DEBUG : first_comment = %x", get_comments()) ;
// Corps vide
}


//
//	}{	Fin du fichier
//
