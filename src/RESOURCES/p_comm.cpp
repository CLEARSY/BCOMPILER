/******************************* CLEARSY **************************************
* Fichier : $Id: p_comm.cpp,v 2.0 2000/07/11 16:08:04 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_comm
*				Modelisation des commentaires.
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
RCS_ID("$Id: p_comm.cpp,v 1.6 2000/07/11 16:08:04 fl Exp $") ;

// includes head
#include "p_head.h"


// Definition de Methodes
// _______________________

// Constructeur
T_resource_comment::T_resource_comment(T_resource_line *new_line_aop,
										 T_resource_item **adr_first_aopp,
										 T_resource_item **adr_last_aopp,
										 T_resource_item *new_father_aop)
  : T_resource_item( adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> New COMMENT %x line %x",
		 this,
		 new_line_aop) ;
#endif

  // Positionne le type
  set_type(RESOURCE_COMMENT) ;

  // Positionne la ligne
  set_line(new_line_aop) ;

#ifdef DEBUG_RESOURCES
  TRACE2("<- New COMMENT %x line %x",
		 this,
		 new_line_aop) ;
  EXIT_TRACE ;
#endif
}

// Destructeur
T_resource_comment::~T_resource_comment(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("Del COMMENT %x line %d \"%s\"",
		 this,
		 (line_op != NULL)? line_op->get_number():0,
		 (line_op != NULL)? line_op->get_value()->get_string():"VIDE") ;
#endif

  if(line_op != NULL)
	{
	  T_resource_file *father_lop = (T_resource_file *)line_op->get_father() ;

	  T_resource_item **first_line_lopp = father_lop->get_adr_first_line() ;
	  T_resource_item **last_line_lopp = father_lop->get_adr_last_line() ;

	  // Evite le bouclage des destructeurs
	  line_op->set_item(NULL) ;

	  // Detruit la ligne
	  delete_item(line_op, *first_line_lopp, *last_line_lopp) ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Fin de fichier
// _______________
