/******************************* CLEARSY **************************************
* Fichier : $Id: m_actsta.h,v 2.0 1999-10-27 07:48:57 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : classe T_action_state
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

#ifndef _M_ACTSTA_H_
#define _M_ACTSTA_H_

#include "m_sign.h"
#include "m_action.h"

//initialisation de la classe T_action_state
extern void init_T_action_state(void) ;

//classe définissant l'état d'un composant pour une action
class T_action_state : public T_object
{

	//attributs

	//signature du composant pour cette action
	T_signature *signature ;

	//résultat de l'action
	T_action_result result ;

public :

	//constructeur
	T_action_state(T_signature *sign, T_action_result result) ;

	//destructeur
	virtual ~T_action_state(void) ;

	//redefinition de la methode virtuelle de T_object get_class_name
	//pour la classe T_action_state
	const char* get_class_name(void) ;

	//mise a jour pres l'execution d'une action
	void update(T_signature *new_sign, T_action_result new_result) ;

	//ecriture de l'etat d'une action dans le fichier d'etat
	//ce fichier est deja ouvert en ecriture (w+)
	void write_state_file(FILE *fd) ;

	//methodes de consultation

	//rend l'etat d'une action (=si il faut la refaire)
	//rend TRUE  si : la signature est la meme que celle passee en parametre
	// et le resultat est ok
	//rend FALSE si la signature est differente ou si le resultat est ko
	int is_update(T_signature *current_sign) ;

	//rend la signature
	T_signature *get_signature(void) ;

};

#endif /* _M_ACTSTA_H_ */

#endif /* ACTION_NG */
