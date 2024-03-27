/******************************* CLEARSY **************************************
* Fichier : $Id: p_mana.cpp,v 2.0 2000/07/11 16:09:41 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_mana
*				Modelisation du manager de resources.
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
RCS_ID("$Id: p_mana.cpp,v 1.8 2000/07/11 16:09:41 fl Exp $") ;

// includes head
#include "p_head.h"


// Definition de Methodes
// _______________________

// Constructeur
T_resource_manager::T_resource_manager(T_resource_item **adr_first_aopp,
									   T_resource_item **adr_last_aopp,
									   T_resource_item *new_father_aop)
  : T_resource_item (adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> New MANAGER %x", this) ;
#endif

  // Positionne le type
  set_type(RESOURCE_MANAGER) ;

  // Positionne la liste d'outils
  first_tool_op		= NULL ;
  last_tool_op		= NULL ;

  // Positionne les fichiers
  system_file_op	= NULL ;
  user_file_op		= NULL ;
  explicit_file_op	= NULL ;

#ifdef DEBUG_RESOURCES

  TRACE1("<- New MANAGER %x", this) ;
  EXIT_TRACE ;
#endif
}

// Destructeur
T_resource_manager::~T_resource_manager(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> Del MANAGER %x", this) ;
#endif

  // Libere la liste d'outils
  delete_all_tools() ;

  // Libere les fichiers
  if(system_file_op != NULL)
	{
	  delete system_file_op ;
	}

  if(user_file_op != NULL)
	{
	  delete user_file_op ;
	}

  if(explicit_file_op != NULL)
	{
	  delete explicit_file_op ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- Del MANAGER %x", this) ;
  EXIT_TRACE ;
#endif
}

// Recherche un outil
T_resource_tool *
T_resource_manager::get_tool(const char *tool_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("T_resource_manager(%x):get_tool(\"%s\")", this, tool_name_acp) ;
#endif

  T_resource_tool *tool_lop = (T_resource_tool*)first_tool_op ;

  if(tool_name_acp != NULL)
	{
	  // Cree un objet string de tool_name_acp
	  T_resource_string *string_lop = new T_resource_string(tool_name_acp) ;

	  // Recherche de l'outil
	  while( (tool_lop != NULL) &&
			 (0 == string_lop->compare(tool_lop->get_name())) )
		{
		  tool_lop = (T_resource_tool*)tool_lop->get_next() ;
		}
	  // Libere string_lop
	  delete string_lop ;
	  string_lop = NULL ;
	}

#ifdef DEBUG_RESOURCES
  TRACE2("<- Get MANAGER TOOL %x name \"%s\"",
		 tool_lop,
		 (tool_lop != NULL)? tool_lop->get_name()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif

  // Retourne l'outil recherche ou NULL
  return tool_lop ;
}

// Cree un nouvel outil
T_resource_tool *
T_resource_manager::set_tool(const char *tool_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Set MANAGER TOOL %x name \"%s\"",
		 this,
		 tool_name_acp) ;
#endif

  if(tool_name_acp != NULL)
	{
#ifdef DEBUG_RESOURCES
	  TRACE2("<+ Set MANAGER TOOL %x name \"%s\"",
			 this,
			 tool_name_acp) ;
	  EXIT_TRACE ;
#endif

	  return new T_resource_tool(tool_name_acp,
								  (T_resource_item**)&first_tool_op,
								  (T_resource_item**)&last_tool_op,
								  this) ;
	}
  else
	{
#ifdef DEBUG_RESOURCES
	  TRACE2("<- Set MANAGER TOOL %x name \"%s\"",
			 this,
			 tool_name_acp) ;
	  EXIT_TRACE ;
#endif

	  return NULL ;
	}
}

// Detruit un outil
void T_resource_manager::delete_tool(const char *tool_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Del MANAGER TOOL %x name \"%s\"",
		 this,
		 tool_name_acp) ;
#endif

  // Recherche l'outil
  T_resource_tool *tool_lop = get_tool(tool_name_acp) ;

  // Detruit l'outil, s'il existe
  if(tool_lop != NULL)
	{
	  delete_item(tool_lop, first_tool_op, last_tool_op) ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- Del MANAGER TOOL %x",
		 tool_lop) ;
  EXIT_TRACE ;
#endif
}

// Detruit tous les outils de la list
void T_resource_manager::delete_all_tools(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> Del ALL MANAGER TOOL %x", this) ;
#endif

  // Libere la liste d'outils
  delete_list(first_tool_op, last_tool_op) ;

#ifdef DEBUG_RESOURCES
  TRACE1("<- Del ALL MANAGER TOOL %x", this) ;
  EXIT_TRACE ;
#endif
}

// Recherche un fichier
T_resource_file *
T_resource_manager::get_file(T_resource_source source_ae)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Get MANAGER FILE %x source %d",
		 this,
		 source_ae) ;
  EXIT_TRACE ;
#endif

  switch (source_ae)
	{
	case RESOURCE_ATELIERB :
	  {
		return system_file_op ;
	  }
	case RESOURCE_USER :
	  {
		return user_file_op ;
	  }
	case RESOURCE_EXPLICIT :
	  {
		return explicit_file_op ;
	  }
	}
  return NULL ;
}

// Cree un nouveau fichier
T_resource_file *T_resource_manager::set_file(const char *file_name_acp,
													   T_resource_source source_ae)
{
  T_resource_file *file_lop = get_file(source_ae) ;
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Set File %x name \"%s\"",
		 file_lop,
		 (file_lop != NULL)? file_lop->get_name()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif

  // Detruit le fichier existant, excepte Explicit
  if( (file_lop  != NULL) &&
	  (source_ae != RESOURCE_EXPLICIT) )
	{
	  delete_file(source_ae) ;
	}

  switch (source_ae)
	{
	case RESOURCE_ATELIERB :
	  {
		system_file_op = new T_resource_file(file_name_acp,
											  source_ae,
											  NULL,
											  NULL,
											  this) ;
		return system_file_op ;
		break ;
	  }
	case RESOURCE_USER :
	  {
		user_file_op = new T_resource_file(file_name_acp,
											source_ae,
											NULL,
											NULL,
											this) ;
		return user_file_op ;
		break ;
	  }
	case RESOURCE_EXPLICIT :
	  {
		// Si le fichier explicit existe, modifie son nom
		if(explicit_file_op != NULL)
		  {
			explicit_file_op->set_name(file_name_acp) ;
		  }
		// Sinon, cree un nouveau fichier
		else
		  {
			explicit_file_op = new T_resource_file(file_name_acp,
													source_ae,
													NULL,
													NULL,
													this) ;
		  }

		return explicit_file_op ;
		break ;
	  }
	}
  return NULL ; // Echec
}

// Detruit un fichier
void T_resource_manager::delete_file(T_resource_source source_ae)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("Del MANAGER FILE %x source %d",
		 this,
		 source_ae) ;
#endif

  switch (source_ae)
	{
	case RESOURCE_ATELIERB :
	  {
		if(system_file_op != NULL)
		  {
			delete system_file_op ;
			system_file_op = NULL ;
		  }
		break ;
	  }
	case RESOURCE_USER :
	  {
		if(user_file_op != NULL)
		  {
			delete user_file_op ;
			user_file_op = NULL ;
		  }
		break ;
	  }
	case RESOURCE_EXPLICIT :
	  {
		if(explicit_file_op != NULL)
		  {
			delete explicit_file_op ;
			explicit_file_op = NULL ;
		  }
		break ;
	  }
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}

//Imprime les ressources dans un fichier
void T_resource_manager::print_resources(FILE *stream_file)
{
#ifdef DEBUG_RESOURCES
  TRACE1("-> T_resource_manager::print_resources(%x)", stream_file) ;
#endif

  //impression des ressources de chacun des outils
  T_resource_tool *tool_lop = (T_resource_tool*)first_tool_op ;
  while(tool_lop != NULL)
	{
	  tool_lop->print_resources(stream_file) ;
	  tool_lop = (T_resource_tool*)tool_lop->get_next() ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- T_resource_manager::print_resources(%x)", stream_file) ;
#endif
}
// Fin de fichier
// _______________
