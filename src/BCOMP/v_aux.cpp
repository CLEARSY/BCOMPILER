/******************************* CLEARSY **************************************
* Fichier : $Id: v_aux.cpp,v 2.0 1998-01-13 10:29:36 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique
*
*					Fonctions auxiliaires
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
RCS_ID("$Id: v_aux.cpp,v 1.2 1998-01-13 10:29:36 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_toplvl.h"
#include "v_aux.h"

// Recherche d'une machine dans une liste de T_used_machine
extern T_used_machine *find_used_machine(T_symbol *machine_name,
										 T_used_machine *first)
{
T_used_machine *cur = first ;
for (;;)
	{
	if (cur == NULL)
		{
		return NULL ;
		}

	if (machine_name->compare(cur->get_name()->get_name()) == TRUE)
		{
		return cur ;
		}

	cur = (T_used_machine *)cur->get_next() ;
	}
}



