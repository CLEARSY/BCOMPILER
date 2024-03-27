/******************************* CLEARSY **************************************
* Fichier : $Id: p_item.h,v 2.0 1998/11/02 14:16:26 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_item
*				Modelisation des elements d'une liste.
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
#ifndef _P_ITEM_H_
#define _P_ITEM_H_


// Definition de Types
// ____________________

// Types de ressources
typedef enum
{
  /* 00 */
  RESOURCE_ITEM,
  /* 01 */
  RESOURCE_LINE,
  /* 02 */
  RESOURCE_FILE,
  /* 03 */
  RESOURCE_IDENT,
  /* 04 */
  RESOURCE_COMMENT,
  /* 05 */
  RESOURCE_TOOL,
  /* 06 */
  RESOURCE_MANAGER
} T_resource_type ;


// Definition de Classes
// ______________________

class T_resource_item
{
  // Type de l'objet
  T_resource_type type_e ;

  // Pere de l'element
  T_resource_item *father_op ;

  // Element precedant
  T_resource_item *prev_op ;

  // Element suivant
  T_resource_item *next_op ;

public:

  // Constructeur
  T_resource_item(T_resource_item **adr_first_aopp,
				  T_resource_item **adr_last_aopp,
				  T_resource_item *new_father_aop) ;

  // Destructeur
  virtual ~T_resource_item(void) ;

  // Acces aux attributs
  T_resource_type get_type(void)
	{
	  return type_e ;
	}

  T_resource_item *get_father(void)
	{
	  return father_op ;
	}

  T_resource_item *get_prev(void)
	{
	  return prev_op ;
	}

  T_resource_item *get_next(void)
	{
	  return next_op ;
	}

  // Affecte les attributs
  void set_type(T_resource_type new_type_ae)
	{
	  type_e = new_type_ae ;
	}

  void set_father(T_resource_item *new_father_aop)
	{
	  father_op = new_father_aop ;
	}

  void set_prev(T_resource_item *new_prev_aop)
	{
	  prev_op = new_prev_aop ;
	}

  void set_next(T_resource_item *new_next_aop)
	{
	  next_op = new_next_aop ;
	}

} ;


// Declaration de Fonctions
// _________________________

// Detruit une liste d'objets
extern void delete_list(T_resource_item *&first_item_aop,
								 T_resource_item *&last_item_aop) ;

// Detruit un element d'une liste d'objets
extern void delete_item(T_resource_item *item_aop,
								 T_resource_item *&first_item_aop,
								 T_resource_item *&last_item_aop) ;


#endif // _P_ITEM_H_
