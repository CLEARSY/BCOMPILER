/******************************* CLEARSY **************************************
* Fichier : $Id: p_resour.h,v 2.0 1998/11/02 14:16:33 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_resour
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
#ifndef _P_RESOUR_H_
#define _P_RESOUR_H_


// Definition de Classes
// _______________________

class T_resource_ident : public T_resource_item
{
  // Nom de la resource
  T_resource_string *name_op ;

  // Valeur de la resource
  T_resource_string *value_op ;

  // Ligne rattachee
  T_resource_line *line_op ;

  // Premier de la liste des commentaires
  T_resource_item *first_comment_op ;

  // Dernier de la liste des commentaires
  T_resource_item *last_comment_op ;

public :

  // Constricteur
  T_resource_ident(T_resource_string *new_name_aop,
					T_resource_string *new_value_aop,
					T_resource_line *new_line_aop,
					T_resource_item **adr_first_aopp,
					T_resource_item **adr_last_aopp,
					T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_ident(void) ;

  // Methodes d'acces aux attributs
  T_resource_string *get_name(void) {return name_op ;}
  T_resource_string *get_value(void) {return value_op ;}
  T_resource_line *get_line(void) {return line_op ;}
  T_resource_item **get_adr_first_comment(void) {return &first_comment_op ;}
  T_resource_item **get_adr_last_comment(void) {return &last_comment_op ;}

  // Affectation d'une valeur
  void set_value(const char *new_value_acp) ;

  void set_line(T_resource_line *new_line_aop) {line_op = new_line_aop ;}

  void set_first_comment(T_resource_item *new_first_aop)
	{
	  first_comment_op = new_first_aop ;
	}

  void set_last_comment(T_resource_item *new_last_aop)
	{
	  last_comment_op = new_last_aop ;
	}

} ;


#endif // _P_RESOUR_H_
