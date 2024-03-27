/******************************* CLEARSY **************************************
* Fichier : $Id: p_line.h,v 2.0 1998/11/02 14:16:27 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_line
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
#ifndef _P_LINE_H_
#define _P_LINE_H_


// Definition de Classes
// ________________

class T_resource_line : public T_resource_item
{
  // Numero de la ligne
  int number_i ;

  // Contenu de la ligne
  T_resource_string *value_op ;

  // Element d'une ligne (resource, comment ou  NULL)
  T_resource_item *item_op ;

public:

  // Constructeur
  T_resource_line(int new_number_ai,
				   T_resource_string *new_value_aop,
				   T_resource_item **adr_first_aopp,
				   T_resource_item **adr_last_aopp,
				   T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_line(void) ;

  // Acces aux attributs
  int get_number(void)
	{
	  return number_i ;
	}

  T_resource_string *get_value(void)
	{
	  return value_op ;
	}

  T_resource_item *get_item(void)
	{
	  return item_op ;
	}

  // Affectation des attributs
  void set_number(int new_number_ai)
	{
	  number_i = new_number_ai ;
	}

  void set_value(T_resource_string *new_value_aop) ;

  void set_item(T_resource_item *new_item_aop)
	{
	  item_op = new_item_aop ;
	}

} ;


#endif // _P_LINE_H_
