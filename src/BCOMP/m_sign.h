/******************************* CLEARSY **************************************
* Fichier : $Id: m_sign.h,v 2.0 1999-11-08 15:28:32 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : gestionnaire des actions
*
This file is part of B_COMPILER
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    B_COMPILER is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    B_COMPILER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with B_COMPILER; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#ifdef  ACTION_NG

#ifndef _M_SIGN_H_
#define _M_SIGN_H_

#include "m_gcheck.h"

//initialisation de la classe T_signature
extern void init_T_signature(void) ;

//classe definissant la signature d'un composant
class T_signature : public T_object
{

	//attributs

	//checksum du composant
	T_global_component_checksum *main_checksum ;

	//liste des checksums des composants dependants
	T_global_component_checksum *first_list_checksum ;

	//fin de la liste des checksums dependants
	T_global_component_checksum *last_list_checksum ;

  //checksum courant : sert a get_next_checksum ;
  T_global_component_checksum *cur_checksum ;

public :

	//construit une signature entiere
	T_signature(T_global_component_checksum *main,
						 T_global_component_checksum *adr_first,
						 T_global_component_checksum *adr_last) ;

	//construit une signature avec uniquement le checksum de la racine
	T_signature(T_global_component_checksum *main) ;

	//destructeur
	virtual ~T_signature(void) ;

	//redefinition de la methode virtuelle de T_object get_class_name
	//pour la classe T_signature
	const char* get_class_name(void) ;

	//ajout d'un checksum d'un composant dependant a une signature
	void add_checksum(T_global_component_checksum
							   *dependant_checksum) ;

	//compare a une autre signature
	//rend TRUE si elles sont egales, FALSE sinon
	int compare(T_signature *other_sign) ;

	//compare le checksum principal a check
	//rend TRUE si ils sont egaux, FALSE sinon
	int compare_main_checksum(T_global_component_checksum *check) ;

  //ecriture de la signature dans le fichier d'etat fd
	void write_state_file(FILE *fd) ;

	//methodes de consultation

	//recuperation du checksum du composant principal
	T_global_component_checksum *get_main_checksum(void) ;

	//recuperation du premier element de la liste des checksums dependants
	T_global_component_checksum *get_first_checksum(void) ;

	//recuperation du checksum suivant
	T_global_component_checksum *get_next_checksum(void) ;


};

#endif /* _M_SIGN_H_ */

#endif /* ACTION_NG */
