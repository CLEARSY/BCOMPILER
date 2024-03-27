/******************************* CLEARSY **************************************
* Fichier : $Id: p_comm.h,v 2.0 1998/11/02 14:16:22 rp Exp $
* (C) 2008 CLEARSY
* Description :	Interface du module p_comm
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
#ifndef _P_COMM_H_
#define _P_COMM_H_


// Definition de Classes
// _______________________
class T_resource_ident ;

class T_resource_comment : public T_resource_item
{
  // Ligne rattachee
  T_resource_line *line_op ;

public :

  // Constructeur
  T_resource_comment(T_resource_line *new_line_aop,
					  T_resource_item **adr_first_aopp,
					  T_resource_item **adr_last_aopp,
					  T_resource_item *new_father_aop) ;

  // Destructeur
  ~T_resource_comment(void) ;

  //Methode d'acces aux attributs
  T_resource_line *get_line(void)
	{
	  return line_op ;
	}

  // Methode d'affectation des attributs
  void set_line(T_resource_line *new_line_aop)
	{
	  line_op = new_line_aop ;
	}

} ;


#endif // _P_COMM_H_
