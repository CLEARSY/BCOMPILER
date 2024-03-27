/******************************* CLEARSY **************************************
* Fichier : $Id: p_line.cpp,v 2.0 2000/07/11 16:09:24 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_line
*				Modelisation des lignes d'un fichier.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_line.cpp,v 1.8 2000/07/11 16:09:24 fl Exp $") ;

// includes head
#include "p_head.h"


//Definition des Methodes
// ___________________

// Constructeur
T_resource_line::T_resource_line(int new_number_ai,
										  T_resource_string *new_value_aop,
										  T_resource_item **adr_first_aopp,
										  T_resource_item **adr_last_aopp,
										  T_resource_item *new_father_aop)
  : T_resource_item(adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> New LINE %x number %d \"%s\"",
		 this,
		 new_number_ai,
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE") ;
#endif

  // Positionne le type
  set_type(RESOURCE_LINE) ;

  // Initialisations
  set_number(new_number_ai) ;
  value_op = string_copy(new_value_aop) ;
  item_op = NULL ;

#ifdef DEBUG_RESOURCES
  TRACE3("<- New LINE  %x number %d \"%s\"",
		 this,
		 new_number_ai,
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif
}


// Destructeur
T_resource_line::~T_resource_line(void)
{
#ifdef DEBUG_RESOURCES
  TRACE3(">> T_resource_line(%x)::~T_resource_line()",
		 this,
		 number_i,
		 value_op->get_string()) ;
  ENTER_TRACE ;
#endif

  // Libere la chaîne value
  if (value_op != NULL)
	{
	  delete value_op ;
	  value_op = NULL ;
	}

  // Libere l'objet pointe par item
  if (item_op != NULL)
	{
	  T_resource_item **first_item_lop = NULL ;
	  T_resource_item **last_item_lop  = NULL ;

	  // Traite une resource
#ifdef DEBUG_RESOURCES
	  TRACE0("ici avant acces item_op") ;
	  TRACE1("item_op = %x", item_op) ;
	  TRACE0("ici apres acces item_op") ;
#endif

	  if (item_op->get_type() == RESOURCE_IDENT)
		{
		  T_resource_ident *resource_lop = (T_resource_ident*)item_op ;
		  T_resource_tool  *tool_lop =
			(T_resource_tool*)(resource_lop->get_father()) ;

#ifdef DEBUG_RESOURCES
		  TRACE1("Father RESOURCE %x", tool_lop) ;
#endif

		  // Evite le bouclage des destructeurs
		  resource_lop->set_line(NULL) ;

		  first_item_lop = tool_lop->get_adr_first_resource() ;
		  last_item_lop  = tool_lop->get_adr_last_resource() ;

		  // Libere l'objet
		  delete_item(item_op, *first_item_lop, *last_item_lop) ;
		}
	  // Traite un commentaire
	  else
		{
		  T_resource_comment *comment_lop = (T_resource_comment*)item_op ;
		  T_resource_ident  *resource_lop =
			(T_resource_ident*)(comment_lop->get_father()) ;
#ifdef DEBUG_RESOURCES
		  TRACE1("Father COMMENT %x", resource_lop) ;
#endif

		  // Evite le bouclage des destructeurs
		  comment_lop->set_line(NULL) ;

		  first_item_lop = resource_lop->get_adr_first_comment() ;
		  last_item_lop  = resource_lop->get_adr_last_comment() ;

		  // L'appel du destructeur se fera par la resource pere
		}

	  // Positionne le pointeur
	  item_op = NULL ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
  TRACE1("<< T_resource_line(%x)::~T_resource_line()", this) ;
#endif
}

// Affectation de l'attribut value
void T_resource_line::set_value(T_resource_string *new_value_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> Set LINE %x number %d value \"%s\"",
		 this,
		 get_number(),
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE") ;
#endif

  // Libere l'ancienne chaîne
  if (value_op != NULL)
	{
	  delete value_op ;
	}

  // Copie la nouvelle chaîne
  value_op = string_copy(new_value_aop) ;
#ifdef DEBUG_RESOURCES
  TRACE3("<- Set LINE %x number %d value \"%s\"",
		 this,
		 get_number(),
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif
}


// Fin de fichier
//
