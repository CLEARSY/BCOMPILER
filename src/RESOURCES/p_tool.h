/******************************* CLEARSY **************************************
* Fichier : $Id: p_tool.h,v 2.0 1999/12/01 17:41:12 clb Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_tool
*				Modelisation d'un outil.
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
#ifndef _P_TOOL_H_
#define _P_TOOL_H_


// Definition de Classes
// ______________________

class T_resource_tool : public T_resource_item
{
  // Nom de l'outil
  T_resource_string *name_op ;

  // Premiere resource de la liste
  T_resource_item *first_resource_op ;

  // Derniere resource de la liste
  T_resource_item *last_resource_op ;

public :

  // Constructeur
  T_resource_tool(const char *new_name_acp,
				   T_resource_item **adr_first_aopp,
				   T_resource_item **adr_last_aopp,
				   T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_tool(void) ;

  // Methodes d'acces
  T_resource_string *get_name(void) {return name_op ;}
  T_resource_item **get_adr_first_resource(void) {return &first_resource_op ;}
  T_resource_item **get_adr_last_resource(void) {return &last_resource_op ;}

  // Recherche une resource de la liste
  T_resource_ident *get_resource(const char *resource_name_acp) ;

  // Affecte une resource de la liste
  T_resource_ident *set_resource(const char *resource_name_acp,
								   const char *resource_value_acp,
								   T_resource_line *line_aop) ;

  // Libere une resource de la liste
  void delete_resource(const char *resource_name_acp) ;

  //Imprime les ressources dans un fichier
  void print_resources(FILE *fd) ;

} ;


// Declaration de Fonctions
// _________________________

// Retourne le nom du fichier utilisateur par defaut
extern const char *get_default_user_file() ;


#endif // _P_TOOL_H_
