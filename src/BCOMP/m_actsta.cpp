/******************************* CLEARSY **************************************
* Fichier : $Id: m_actsta.cpp,v 2.0 1999-10-27 07:48:56 clb Exp $
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
#include "c_port.h"
RCS_ID("$Id: m_actsta.cpp,v 1.3 1999-10-27 07:48:56 clb Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_actsta.h"


//fonction static d'ecriture du resultat d'une action dans le fichier d'etat
static void write_result(FILE* fd, T_action_result result)
{
	TRACE1(">> write_result(%d)", result) ;
	if ( result ==  AS_OK )
		{
			fputs("_ok", fd) ;
		}
	else
		{
			fputs("_ko", fd) ;
		}
}

//initialisation de la classe T_action_state
void init_T_action_state(void)
{
	TRACE0(">> init_T_action_state") ;
	ENTER_TRACE ;

	//crée la description de métaclasse pour T_action_state
	// (NODE_USER_DEFINED3)
	T_metaclass *meta_action_state =
		new T_metaclass("T_action_state",
						sizeof(T_action_state),
						get_metaclass(NODE_ITEM)) ;

	//ajout de la description des champs de T_action_state :
	//signature du composant et resultat de l'action
	meta_action_state->add_field("signature", FIELD_OBJECT, REF_SHARED) ;
	meta_action_state->add_field("result", FIELD_INTEGER) ;

	//affectation de la métaclasse à la classe
	set_metaclass(NODE_USER_DEFINED3 , meta_action_state) ;

	EXIT_TRACE ;
	TRACE0("<< init_T_action_state") ;
}

//
//classe T_action_state définissant l'état d'un composant pour une action
//

//constructeur
T_action_state::T_action_state(T_signature *sign,
							   T_action_result result_action)
	: T_object(NODE_USER_DEFINED3)
{
	TRACE2(">> constructeur T_action_state(%x,%d)", sign, result_action) ;

	signature = sign ;
	result = result_action ;

	set_block_state(this, MINFO_PERSISTENT) ;

	TRACE2("<< constructeur T_action_state(%x,%d)", signature, result) ;
}

//destructeur
T_action_state::~T_action_state(void)
{
	TRACE0(">> ~T_action_state") ;

	//	delete[] signature ;

	TRACE0("<< ~T_action_state") ;
}

//redefinition de la methode virtuelle de T_object get_class_name
//pour la nouvelle classe T_component_state
const char* T_action_state::get_class_name(void)
{
	return "T_action_state" ;
}

//mise a jour de l'etat d'une action
void T_action_state::update(T_signature *new_sign,
									 T_action_result new_result)
{
  TRACE2(">> T_action_state::update(%x,%d)", new_sign, new_result) ;

  result = new_result ;
  //delete[] signature ;
  signature = new_sign ;

  TRACE2("<< T_action_state::update(%x,%d)", new_sign, new_result) ;
}



//ecriture de l'etat d'une action dans le fichier d'etat fd
//ce fichier est deja ouvert en ecriture (w+)
void T_action_state::write_state_file(FILE *fd)
{
	TRACE2(">> T_action_state::write_state_file(%x,%x)", this, fd) ;
	TRACE1("result(%d)", result) ;

	write_result(fd, result) ;
	write_state_rc(fd) ;
	signature->write_state_file(fd) ;

	TRACE0("<< T_action_state::write_state_file") ;
}

//methodes de consultation


//rend l'etat d'une action (=si il faut la refaire)
//rend TRUE  si : la signature est la meme que celle passee en parametre
// et le resultat est ok
//rend FALSE si la signature est differente ou si le resultat est ko
int T_action_state::is_update(T_signature *current_sign)
{
	TRACE1(">> T_action_state::is_update(%x)", current_sign) ;

	int res = FALSE ;

	if ( current_sign == NULL )
		{
			TRACE1("probleme de signature nulle", this) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg
						   (E_MS_UPDATE_ACTION_WITH_NULL_SIGNATURE),
						   this) ;
			assert(FALSE) ;
		}
	else
		{
			if ( result == AS_KO )
				{
					res = FALSE ;
				}
			else
				{
					res = (signature->compare(current_sign)) ;
				}
		}

	TRACE1("<< T_action_state::is_update(%d)", res) ;

	return res ;
}

//rend la signature
T_signature *T_action_state::get_signature(void)
{
	TRACE1(" T_action_state::get_signature(%x)", signature) ;
	return signature ;
}

#endif /* ACTION_NG */
