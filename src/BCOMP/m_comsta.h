/******************************* CLEARSY ************************************
* Fichier : $Id: m_comsta.h,v 2.0 1999-10-27 07:49:01 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : classe T_component_state
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

#ifndef _M_COMSTA_H_
#define _M_COMSTA_H_

#include "m_actsta.h"

//initialisation de la classe T_component_state
extern void init_T_component_state(void) ;



//initialisation des symbols des actions
extern void init_action_symbols(void) ;

//classe définissant l'état d'un composant
class T_component_state : public T_object
{

	//attributs

	//nom du fichier d'etat du composant
	T_symbol *state_file_name ;

	//tableau d'etat du composant
	T_action_state **tab_state ;

public :

	//constructeur a partir d'un fichier d'etat
	T_component_state(T_symbol *file_name) ;

	//destructeur
	virtual ~T_component_state(void) ;

	//redefinition de la methode virtuelle de T_object get_class_name
	//pour la classe T_component_state
	const char* get_class_name(void) ;

	//modifications de l'etat

	//rajoute l'état d'une action dans l'état d'un composant
	void add_action_state(T_action_state *action_state,
								   T_state_type_action type_action) ;

	//mise a jour de l'etat du composant apres l'execution d'une action
	void update(T_signature *current_sign,
						 T_state_type_action action,
						 T_action_result result) ;

	//ecriture du fichier d'etat a partir de l'etat du composant
	void write_state_file(void) ;

	//consultation de l'etat

	//rend l'etat d'une action pour ce composant
	//rend TRUE si l'action est a jour et son resultat etait ok
	//rend FALSE si l'action n'est pas a jour ou si son resultat etait ko
	int is_update_action(T_signature *current_sign,
								  T_state_type_action action) ;

	//dit si l'action a deja ete faite sur le composant
	//rend TRUE si l'action a deja ete faite quelque soit son resultat
	//rend FALSE sinon
	int has_been_done_action(T_state_type_action action) ;

	//rend la signature de l'etat d'un composant pour une action
	T_signature *get_signature(T_state_type_action action) ;

} ;




#endif /* _M_COMSTA_H_ */

#endif /* ACTION_NG */
