/******************************* CLEARSY **************************************
* Fichier : $Id: c_blist.cpp,v 2.0 1999-06-09 12:38:11 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Liste de Betree
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
RCS_ID("$Id: c_blist.cpp,v 1.7 1999-06-09 12:38:11 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Constructeur
T_betree_list::T_betree_list(void)
: T_object(NODE_BETREE_LIST)
{
  TRACE1("T_betree_list::T_betree_list(%x)", this) ;
  first_link = NULL ;
  last_link = NULL ;
}

// Destructeur
T_betree_list::~T_betree_list(void)
{
  TRACE1("T_betree_list::~T_betree_list(%x)", this) ;
}

// Ajout d'un betree dans la liste, en verifiant qu'il ne s'y trouvait pas
// Renvoie NULL si l'operation s'est bien passee, et l'occurence precedente
// en cas de redefinition
T_list_link *T_betree_list::add_betree(T_betree *new_betree,
												T_lexem *new_ref_lexem)
{
TRACE2("T_betree_list(%x)::add_betree(%x)", this, new_betree);

// On regarde d'abord si le betree n'est pas dans la liste !!!
T_list_link *cur = first_link ;

for (;;)
	{
	if (cur == NULL)
		{
		TRACE0("on ajoute le betree dans la liste") ;
		(void)new T_list_link(new_betree,
							  LINK_BETREE_LIST,
							  (T_item **)&first_link,
							  (T_item **)&last_link,
							  (T_item *)this, /* un petit mensonge bien pratique !*/
							  NULL,
							  new_ref_lexem) ;
		return NULL ;
		}

	if ((T_betree *)(cur->get_object()) == new_betree)
		{
		TRACE2("le betree est deja dans la liste cur=%x,object=%x",
			   cur,
			   cur->get_object()) ;
		return cur ;
		}

	cur = (T_list_link *)cur->get_next() ;
	}
}

// Recherche si un betree est dans la liste. Renvoie la definition si oui,
// et NULL sinon
T_list_link *T_betree_list::search_betree(T_betree *new_betree)
{
  //TRACE2("T_betree_list(%x)::search_betree(%x)", this, new_betree);

T_list_link *cur = first_link ;

for (;;)
	{
	if (cur == NULL)
		{
		  //TRACE0("pas trouve !") ;
		return NULL ;
		}

	if ((T_betree *)(cur->get_object()) == new_betree)
		{
		  /*TRACE2("le betree est dans la liste cur=%x,object=%x",
			   cur,
			   cur->get_object()) ;*/
		return cur ;
		}

	cur = (T_list_link *)cur->get_next() ;
	}
}

