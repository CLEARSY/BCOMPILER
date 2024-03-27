/******************************* CLEARSY **************************************
* Fichier : $Id: p_resour.cpp,v 2.0 2000/07/11 16:10:38 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_resour
*				Modelisation d'une ressource.
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
RCS_ID("$Id: p_resour.cpp,v 1.7 2000/07/11 16:10:38 fl Exp $") ;

// includes head
#include "p_head.h"


// Definition de Methodes
// _______________________

// Constructeur
T_resource_ident::T_resource_ident(T_resource_string *new_name_aop,
								   T_resource_string *new_value_aop,
								   T_resource_line *new_line_aop,
								   T_resource_item **adr_first_aopp,
								   T_resource_item **adr_last_aopp,
								   T_resource_item *new_father_aop)
  : T_resource_item(adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE4("-> New RESOURCE %x name \"%s\" valeur \"%s\" line %x",
		 this,
		 (new_name_aop != NULL)? new_name_aop->get_string():"VIDE",
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE",
		 new_line_aop) ;
#endif

  // Positionne le type
  set_type(RESOURCE_IDENT) ;

  // Positionne le nom
  if(new_name_aop != NULL)
	{
	  name_op = string_copy(new_name_aop) ;
	}

  // Positionne la valeur
  if(new_value_aop != NULL)
	{
	  value_op = string_copy(new_value_aop) ;
	}

  // Positionne la ligne
  set_line(new_line_aop) ;

  // Positionne la liste de commentaires
  set_first_comment(NULL) ;
  set_last_comment(NULL) ;

#ifdef DEBUG_RESOURCES
  TRACE4("<- New RESOURCE %x name \"%s\" valeur \"%s\" line %x",
		 this,
		 (new_name_aop != NULL)? new_name_aop->get_string():"VIDE",
		 (new_value_aop != NULL)? new_value_aop->get_string():"VIDE",
		 new_line_aop) ;
  EXIT_TRACE ;
#endif
}

// Destructeur
T_resource_ident::~T_resource_ident(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("Del RESOURCE %x name \"%s\"",
		 this,
		 name_op->get_string()) ;
#endif

  // Libere la chaîne nom
  if(name_op != NULL)
	{
	  delete name_op ;
	}

  // Libere la chaîne value
  if(value_op != NULL)
	{
	  delete value_op ;
	}

  // Libere la liste des commentaires
  delete_list(first_comment_op, last_comment_op) ;

  // Libere les lignes d'origines
  while( (line_op != NULL) && (line_op->get_item() == this) )
	{
	  T_resource_item **first_line_lop =
		((T_resource_file*)line_op->get_father())->get_adr_first_line() ;

	  T_resource_item **last_line_lop =
		((T_resource_file*)line_op->get_father())->get_adr_last_line() ;

	  // Pointe la ligne courante
	  T_resource_line *cur_line_lop = line_op ;

	  // Passe a la precedante
	  line_op = (T_resource_line*)line_op->get_prev() ;

	  // Evite un interblocage des destructeurs
	  cur_line_lop->set_item(NULL) ;

	  // Libere la ligne courante
	  delete_item(cur_line_lop, *first_line_lop, *last_line_lop) ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}

// Affectation d'une valeur
void T_resource_ident::set_value(const char *new_value_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("Set RESOURCE %x name \"%s\" value \"%s\"",
		 this,
		 (name_op != NULL)? name_op->get_string():"VIDE",
		 new_value_acp) ;
#endif

  // Libere l'ancienne valeur
  if(new_value_acp != NULL)
	{
	  delete value_op ;
	}

  // Mise a jour de la valeur
  value_op = new T_resource_string(new_value_acp) ;
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Fin de fichier
// _______________
