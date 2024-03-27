/******************************* CLEARSY **************************************
* Fichier : $Id: p_mana.h,v 2.0 1999/12/01 17:41:11 clb Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_mana
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
#ifndef _P_MANA_H_
#define _P_MANA_H_


// Definition de Classes
// ______________________

class T_resource_manager : public T_resource_item
{
  // Fichier systeme
  T_resource_file *system_file_op ;

  // Fichier utilisateur
  T_resource_file *user_file_op ;

  // Fichier explicit
  T_resource_file *explicit_file_op ;

  // Premier outil de la liste
  T_resource_item *first_tool_op ;

  // Dernier outil de la liste
  T_resource_item *last_tool_op ;

public :

  // Constructeur
  T_resource_manager(T_resource_item **adr_first_aopp,
					  T_resource_item **adr_last_aopp,
					  T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_manager(void) ;

  // Recherche un outil
  T_resource_tool *get_tool(const char *tool_name_acp) ;

  // Methodes d'acces
  T_resource_item **get_adr_first_tool(void)
	{
	  return &first_tool_op ;
	}

  T_resource_item **get_adr_last_tool(void)
	{
	  return &last_tool_op ;
	}

  // Cree un nouvel outil
  T_resource_tool *set_tool(const char *tool_name_acp) ;

  // Detruit un outil de la list
  void delete_tool(const char *tool_name_acp) ;

  // Detruit tous les outils de la list
  void delete_all_tools(void) ;

  // Recherche un fichier
  T_resource_file *get_file(T_resource_source source_ae) ;

  // Cree un nouveau fichier
  T_resource_file *set_file(const char *file_name_acp,
							T_resource_source source_ae) ;

  // Detruit un fichier
  void delete_file(T_resource_source source_ae) ;

  //Imprime les ressources dans un fichier
  void print_resources(FILE *stream_file) ;

} ;


#endif // _P_MANA_H_
