/******************************* CLEARSY **************************************
* Fichier : $Id: m_comsta.cpp,v 2.0 2000-10-19 08:27:59 cc Exp $
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
#include "c_port.h"
RCS_ID("$Id: m_comsta.cpp,v 1.10 2000-10-19 08:27:59 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_comsta.h"

//tableau static des symboles des actions
static T_symbol *tab_name_action_sot[] =
{
	/* 00 */
	NULL,
	/* 01 */
	NULL,
	/* 02 */
	NULL,
	/* 03 */
	NULL,
	/* 04 */
   NULL,
	/* 05 */
   NULL,
	/* 06 */
   NULL,
	/* 07 */
   NULL,
	/* 08 */
   NULL,
	/* 09 */
   NULL,
	/* 10 */
   NULL,
	/* 11 */
   NULL,
	/* 12 */
   NULL,
	/* 13 */
   NULL,
	/* 14 */
   NULL,
	/* 15 */
   NULL,
	/* 16 */
   NULL
} ;

static int cpt_tab_name_action_si = 0 ;

static void init_one_action_symbol(char *name)
{
	tab_name_action_sot[cpt_tab_name_action_si] = lookup_symbol(name) ;
	cpt_tab_name_action_si++ ;
}
//initialisation de la table des symbols des actions
extern void init_action_symbols(void)
{
	TRACE0(">> init_action_symbols") ;
	init_one_action_symbol("_bpp") ;
	init_one_action_symbol("_ComponentCheck") ;
	init_one_action_symbol("_POGenerate") ;
	init_one_action_symbol("_Prove") ;
	init_one_action_symbol("_ADATrans") ;
	init_one_action_symbol("_CTrans") ;
	init_one_action_symbol("_PscTransDvg") ;
	init_one_action_symbol("_PscTransCvg") ;
	init_one_action_symbol("_CPPTrans") ;
	init_one_action_symbol("_NewPscTransConv") ;
	init_one_action_symbol("_NewPscTrans") ;
	init_one_action_symbol("_Latex") ;
	init_one_action_symbol("_Interleaf") ;
	init_one_action_symbol("_HIATrans") ;
	init_one_action_symbol("_ALATrans") ;
	init_one_action_symbol("_ALBTrans") ;
	init_one_action_symbol("_Noaction") ;
}

//fonction static d'ecriture du type d'action dans le fichier d'etat
static void write_type_action(FILE *fd, int action)
{
	TRACE1(">> write_type_action(%d)", action) ;

	if ( (action < 0) || (action >= get_nb_actions()))
		{
			TRACE1("Le type d'action no %d n'existe pas",action) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg
						   (E_MS_TYPE_OF_ACTION_INEXISTANT),
						   action) ;
			assert(FALSE) ;
		}
	else
		{
			TRACE1("fichier :%x", fd) ;
			TRACE1("action :%x", tab_name_action_sot[action]) ;
			fputs( (tab_name_action_sot[action])->get_value(), fd ) ;
		}

	TRACE0("<< write_type_action") ;
}

//fonction static d'ecriture d'une { dans le fichier d'etat
static void write_open_braces(FILE *fd)
{
	TRACE0(">> write_open_braces") ;
	fputc('{', fd) ;
}

//fonction static d'ecriture d'une } dans le fichier d'etat
static void write_close_braces(FILE *fd)
{
	TRACE0(">> write_close_braces") ;
	fputc('}', fd) ;
}



/************************************************************
 * Description : Initialisation de la classe T_component_state
 ************************************************************/
extern void	init_T_component_state(void)
{
  TRACE0(">> init_T_component_state") ;
  ENTER_TRACE ;

  //crée la description de métaclasse pour T_component_state
  // (NODE_USER_DEFINED2)
  T_metaclass *meta_component_state
	  = new T_metaclass("T_component_state",
						sizeof(T_component_state),
						get_metaclass(NODE_ITEM));

  //ajout de la description des champs de T_component_state
  //(state_file_name, tab_state)
  meta_component_state->add_field("state_file_name", FIELD_OBJECT, REF_SHARED);
  meta_component_state->add_field("tab_state", FIELD_OBJECT, REF_SHARED);

  //affectation de la métaclasse à la classe
  set_metaclass(NODE_USER_DEFINED2, meta_component_state);

  EXIT_TRACE ;
  TRACE0("<< init_T_component_state");
}


//
//classe définissant l'état d'un composant
//

//constructeur a partir d'un fichier d'etat
T_component_state::T_component_state(T_symbol *file_name)
: T_object(NODE_USER_DEFINED2)
{
	TRACE1(">> constructeur T_component_state(%s)", file_name->get_value()) ;

	int nb_actions = get_nb_actions() ;
	size_t array_size = sizeof(T_action_state *) * nb_actions ;
	tab_state = (T_action_state **)s_malloc(array_size) ;
	memset(tab_state, 0, array_size) ;

	for ( int cur_action = 0;
		  cur_action < get_nb_actions();
		  cur_action++ )
		{
			tab_state[cur_action] = NULL ;
		}

	state_file_name = lookup_symbol(file_name->get_value()) ;

	set_block_state(this, MINFO_PERSISTENT) ;

	TRACE1("<< constructeur T_component_state(%s)",
		   file_name->get_value()) ;
}


//destructeur
T_component_state::~T_component_state(void)
{
	TRACE0(">> ~T_component_state") ;

	/*	for(int  ind=0; ind<NB_ACTIONS; ind++)
	  {
		delete[] tab_state[ind] ;
	  }
	*/
	TRACE0("<< ~T_component_state") ;
}

//rajoute l'état d'une action dans l'état d'un composant
//l'etat du composant ne doit pas avoir deja un etat pour cette action
void T_component_state::add_action_state(T_action_state *action_state,
												  T_state_type_action
												  type_action)
{
	TRACE2(">> T_component_state::add_action_state(%x,%d)",
		   action_state,
		   type_action) ;

	if ( tab_state[type_action] == NULL )
		{
			tab_state[type_action] = action_state ;
		}
	else
		{
			TRACE2("L'etat de l'action %d existe deja dans le tableau %x",
				   type_action,
				   action_state) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg
						   (E_MS_STATE_OF_ACTION_HAS_EVER_BEEN_MEMORIZED),
						   type_action,
						   action_state) ;
			assert(FALSE) ;
		}

	TRACE0("<< T_component_state::add_action_state") ;
}

//redefinition de la methode virtuelle de T_object get_class_name
//pour la nouvelle classe T_component_state
const char* T_component_state::get_class_name(void)
{
	return "T_component_state" ;
}

//mise a jour de l'etat du composant apres l'execution d'une action
void T_component_state::update(T_signature *current_sign,
										T_state_type_action action,
										T_action_result result)
{
	TRACE4(">> T_component_state::update(%x,%d,%d,%x)",
		   current_sign,
		   action,
		   result,
           tab_state[action]) ;

	//mise a jour de l'action
	//si l'etat de l'action est inexistant :
	if ( tab_state[action] == NULL )
		{
			tab_state[action] = new T_action_state(current_sign, result) ;
		}
	else
		{
			(tab_state[action])->update(current_sign, result) ;
		}
	//mise a jour des autres actions
	T_state_type_action cur_action = action ;
	while ( exist_action_toupdate(cur_action, action) )
		{
			cur_action = get_action_toupdate(cur_action, action) ;
				//si l'etat de l'action est inexistant :
			if ( tab_state[cur_action] == NULL )
				{
					tab_state[cur_action] =
						new T_action_state(current_sign, result) ;
				}
			else
				{
					(tab_state[cur_action])->update(current_sign, result) ;
				}
		}


	TRACE0("<< T_component_state::update" ) ;

}

//ecriture du fichier d'etat a partir de l'etat du composant
void T_component_state::write_state_file(void)
{
	TRACE1(">> T_component_state::write_state_file (%x)",
		   state_file_name) ;
	//ouvre le fichier d'état
	FILE *fd = fopen(state_file_name->get_value(), "wb+") ;

	TRACE2("fichier ouvert(%x,%s)", fd, state_file_name->get_value()) ;
	if (fd != NULL)
		{
		  TRACE0("fd non nul") ;
			//ecrit dans le fichier l'etat de chaque action
			//n'ecrit rien si l'action n'a jamais ete realisee
			for ( int cur_action = 0;
				  cur_action < get_nb_actions();
				  cur_action++ )
				{
				  TRACE1("pour l'action (%d)", cur_action) ;
					if ( tab_state[cur_action] != NULL )
						{
						  TRACE0("action etat non nul") ;
							//ecriture du type d'action
							write_type_action(fd, cur_action) ;
							write_state_rc(fd) ;
							write_open_braces(fd) ;
							write_state_rc(fd) ;
							//ecriture de l'etat de l'action
							(tab_state[cur_action])->
								write_state_file(fd) ;
							write_close_braces(fd) ;
							write_state_rc(fd) ;
						}
				}
		}
	else
		{
			TRACE1("impossible d'ouvrir le fichier d'etat %s",
				   state_file_name->get_value()) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg
						   (E_MS_CAN_NOT_OPEN_STATE_FILE),
						   state_file_name->get_value()) ;
			assert(FALSE) ;
		}

	//fermeture du fichier
	int res = fclose(fd) ;

	TRACE1("<< T_component_state::write_state_file(%d)", res ) ;

}
//rend l'etat d'une action pour ce composant (=si il faut la refaire)
//rend TRUE si l'action est a jour et son resultat etait ok
//rend FALSE si l'action n'est pas a jour ou si son resultat etait ko
int T_component_state::is_update_action(T_signature *current_sign,
												  T_state_type_action action)
{
	TRACE2(">> T_component_state::is_update_action(%x,%d)",
		   current_sign,
		   action ) ;

	int res = FALSE ;

	if ( tab_state[action] == NULL )
		{
			TRACE0("l'etat de l'action n'est pas stocke") ;
			res = FALSE ;
		}
	else
		{
			res = tab_state[action]->is_update(current_sign) ;
		}

	TRACE2("<< T_component_state::is_update_action(%d),FALSE(%d)", res, FALSE) ;

	return res ;
}

//dit si l'action a deja ete faite sur le composant
//rend TRUE si l'action a deja ete faite quelque soit son resultat
//rend FALSE sinon
int T_component_state::has_been_done_action(T_state_type_action
													  action)
{
	TRACE1(">> T_component_state::has_been_done_action(%d)", action) ;

	int res = ( tab_state[action] != NULL ) ;

	TRACE1("<< T_component_state::has_been_done_action(%d)", res) ;

	return res ;
}

//rend la signature de l'etat d'un composant pour une action
//rend NULL si l'etat pour cette action n'est pas stocke
T_signature *
T_component_state::get_signature(T_state_type_action action)
{
	TRACE2("T_component_state::get_signature(%d,%x)",
		   action,
		   tab_state[action]) ;
	if (tab_state[action] == NULL)
		{
			TRACE1("l'etat n'est pas stocke pour l'action (%d)", action) ;
			return NULL ;
		}

	return (tab_state[action])->get_signature() ;
}

#endif /* ACTION_NG */
