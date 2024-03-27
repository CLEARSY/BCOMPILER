/******************************* CLEARSY **************************************
* Fichier : $Id: p_file.h,v 2.0 1998/11/02 14:16:24 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_file
*				Modelisation d'un fichier.
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
#ifndef _P_FILE_H_
#define _P_FILE_H_


// Definition de Types
// ________________

typedef enum
{
  RESOURCE_ATELIERB,
  RESOURCE_USER,
  RESOURCE_EXPLICIT
} T_resource_source ;


// Definition de Classes
// _______________________

class T_resource_file : public T_resource_item
{
  // Nom et chemin du fichier
  T_resource_string *name_op ;

  // Type du fichier source
  T_resource_source source_e ;

  // Premiere ligne du fichier
  T_resource_item *first_line_op ;

  // Derniere ligne du fichier
  T_resource_item *last_line_op ;

public :

  // Constructeur
  T_resource_file(const char *new_name_acp,
				   T_resource_source new_source_ae,
				   T_resource_item **adr_first_aopp,
				   T_resource_item **adr_last_aopp,
				   T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_file(void) ;

  // Methodes d'acces aux attributs
  T_resource_string *get_name(void)
	{
	  return name_op ;
	}

  T_resource_source get_source(void)
	{
	  return source_e ;
	}

  T_resource_item **get_adr_first_line(void)
	{
	  return &first_line_op ;
	}

  T_resource_item **get_adr_last_line(void)
	{
	  return &last_line_op ;
	}

  // Recherche d'une ligne
  T_resource_line *get_line(int new_number_ai) ;

  // Affectation des attributs
  void set_name(const char *new_string_acp) ;

  void set_source(T_resource_source new_source_ae)
	{
	  source_e = new_source_ae ;
	}

  void set_line(int new_number_ai, const char *new_string_acp) ;

  // Destruction de la ligne
  void delete_line(int new_number_ai) ;

  // Chargement du contenu d'un fichier
  T_resource_line *load(void) ;

  // Archivage des resources d'un fichier
  const char *save(void) ;

} ;


#endif // _P_FILE_H_
