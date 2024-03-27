/***************************** CLEARSY **************************************
* Fichier : $Id: c_bound.cpp,v 2.0 1999-06-09 12:38:12 sl Exp $
* (C) 2008 CLEARSY
*
* Description :	Bornes des types
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_bound.cpp,v 1.4 1999-06-09 12:38:12 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_bound
//
// Constructeur
T_bound::T_bound(T_ident *new_ref_ident,
						  int new_is_upper_bound,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *father,
						  T_betree *betree,
						  T_lexem *ref_lexem)
  : T_expr(NODE_BOUND, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE8("T_bound(%x)::T_bound(%x, %d, %x, %x, %x, %x, %x)",
		 this,
		 new_ref_ident,
		 new_is_upper_bound,
		 adr_first,
		 adr_last,
		 father,
		 betree,
		 ref_lexem) ;

  ref_ident = new_ref_ident ;
  //  ref_ident->link() ; NON REF CONSULTATIVE SINON BOUCLE
  is_upper_bound = new_is_upper_bound ;
}

// Destructeur
T_bound::~T_bound(void)
{
  TRACE1("T_bound(%x)::~T_bound()", this) ;
}
