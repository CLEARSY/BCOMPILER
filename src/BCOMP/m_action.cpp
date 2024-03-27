/******************************* CLEARSY **************************************
* Fichier : $Id: m_action.cpp,v 2.0 2000-10-19 08:27:11 cc Exp $
*
* (C) 2008 CLEARSY
*
* Description : gestion des listes d'actions :
*               liste d'action a realiser lors de l'execution d'une action
*               liste d'action a mettre ajour apres l'execution d'une action
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
RCS_ID("$Id: m_action.cpp,v 1.7 2000-10-19 08:27:11 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_action.h"

//nombre d'actions differentes
static const int nb_actions_si = STA_NOACTION + 1 ;

//nombre maximum d'actions dans la liste des actions a realiser
static const int nb_act_todo_max_si = 3 ;

//nombre maximum d'actions dans la liste des actions a mettre a jour
static const int nb_act_toupdate_max_si= 4 ;

//tableau des actions a realiser pour chaque action
static T_state_type_action
         tab_actions_todo_set[nb_actions_si][nb_act_todo_max_si] =
{
	/* 00 = STA_BPP */
	{STA_BPP, STA_NOACTION, STA_NOACTION},
	/* 01 = STA_COMPONENTCHECK */
	{STA_BPP, STA_COMPONENTCHECK, STA_NOACTION},
	/* 02 = STA_POGENERATE */
	{STA_BPP, STA_POGENERATE, STA_NOACTION},
	/* 03 = STA_PROVE */
	{STA_BPP, STA_POGENERATE, STA_PROVE},
	/* 04 = STA_ADATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ADATRANS},
	/* 05 = STA_CTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_CTRANS},
	/* 06 = STA_PSCTRANSDVG */
	{STA_COMPONENTCHECK, STA_PSCTRANSDVG, STA_NOACTION},
	/* 07 = STA_PSCTRANSCVG */
	{STA_COMPONENTCHECK, STA_PSCTRANSCVG, STA_NOACTION},
	/* 08 = STA_CPPTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_CPPTRANS},
	/* 09 = STA_NEWPSCTRANSCONV */
	{STA_BPP, STA_COMPONENTCHECK, STA_NEWPSCTRANSCONV},
	/* 10 = STA_NEWPSCTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_NEWPSCTRANS},
	/* 11 = STA_LATEX */
	{STA_BPP, STA_LATEX, STA_NOACTION},
	/* 12 = STA_INTERLEAF */
	{STA_BPP, STA_INTERLEAF, STA_NOACTION},
	/* 13 = STA_HIATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_HIATRANS},
	/* 14 = STA_ALATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ALATRANS},
	/* 15 = STA_ALBTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ALBTRANS},
	/* 16 = STA_NOACTION */
	{STA_NOACTION, STA_NOACTION, STA_NOACTION}, //DOIT ETRE LA DERNIERE ACTION
} ;

//tableau des actions a mettre a jour pour chaque action
static T_state_type_action
    tab_actions_toupdate_set[nb_actions_si][nb_act_toupdate_max_si]=
{
 	/* 00 = STA_BPP */
	{STA_BPP, STA_NOACTION, STA_NOACTION, STA_NOACTION},
	/* 01 = STA_COMPONENTCHECK */
	{STA_BPP, STA_COMPONENTCHECK, STA_NOACTION, STA_NOACTION},
	/* 02 = STA_POGENERATE */
	{STA_BPP, STA_COMPONENTCHECK, STA_POGENERATE, STA_NOACTION},
	/* 03 = STA_PROVE */
	{STA_BPP, STA_COMPONENTCHECK, STA_POGENERATE, STA_PROVE},
	/* 04 = STA_ADATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ADATRANS, STA_NOACTION},
	/* 05 = STA_CTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_CTRANS, STA_NOACTION},
	/* 06 = STA_PSCTRANSDVG */
	{STA_BPP, STA_COMPONENTCHECK, STA_PSCTRANSDVG, STA_NOACTION},
	/* 07 = STA_PSCTRANSCVG */
	{STA_BPP, STA_COMPONENTCHECK, STA_PSCTRANSCVG, STA_NOACTION},
	/* 08 = STA_CPPTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_CPPTRANS, STA_NOACTION},
	/* 09 = STA_NEWPSCTRANSCONV */
	{STA_BPP, STA_COMPONENTCHECK, STA_NEWPSCTRANSCONV, STA_NOACTION},
	/* 10 = STA_NEWPSCTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_NEWPSCTRANS, STA_NOACTION},
	/* 11 = STA_LATEX */
	{STA_BPP, STA_COMPONENTCHECK, STA_LATEX, STA_NOACTION},
	/* 12 = STA_INTERLEAF */
	{STA_BPP, STA_COMPONENTCHECK, STA_INTERLEAF, STA_NOACTION},
	/* 13 = STA_HIATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_HIATRANS, STA_NOACTION},
	/* 14 = STA_ALATRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ALATRANS, STA_NOACTION},
	/* 15 = STA_ALBTRANS */
	{STA_BPP, STA_COMPONENTCHECK, STA_ALBTRANS, STA_NOACTION},
	/* 16 = STA_NOACTION */
	{STA_BPP, STA_NOACTION, STA_NOACTION, STA_NOACTION},//DOIT ETRE LA DERNIERE ACTION
} ;


//rend le rang de l'action action dans la liste d'action a executer
//d'indice first_action, si l'action est dans cette liste
//et rend NB_ACT_TODO_MAX sinon
static int get_action_todo_rank( T_state_type_action action,
							T_state_type_action first_action)
{
	TRACE2(">> get_action_todo_rank(%d,%d)", action, first_action) ;

	int trouve = FALSE;
	int rank = 0 ;

	while ( ( trouve == FALSE ) && ( rank < nb_act_todo_max_si ) )
		{
			if ( tab_actions_todo_set[first_action][rank] == action )
				{
					trouve = TRUE ;
				}
			else
				{
					rank++ ;
				}
		}

	TRACE1("<<  get_action_todo_rank(%d)",  rank) ;
	return rank ;
}

//rend le rang de l'action action dans la liste d'action a mettre a jour
//d'indice action_executed, si l'action est dans cette liste
//et rend NB_ACT_TOUPDATE_MAX sinon
static int get_action_toupdate_rank( T_state_type_action action,
									 T_state_type_action action_executed)
{
	TRACE2(">> get_action_toupdate_rank(%d,%d)", action, action_executed) ;

	int trouve = FALSE;
	int rank = 0 ;

	while ( ( trouve == FALSE ) && ( rank < nb_act_toupdate_max_si ) )
		{
			if ( tab_actions_toupdate_set[action_executed][rank] == action )
				{
					trouve = TRUE ;
				}
			else
				{
					rank++ ;
				}
		}

	TRACE1("<<  get_action_toupdate_rank(%d)",  rank) ;
	return rank ;
}


//dit si il esiste une action a realiser avant l'action action dans la
//liste d'indice first_action
int exist_action_todo_before(T_state_type_action action,
								 T_state_type_action first_action)
{
	TRACE2(">> exist_action_todo_before(%d,%d)", action, first_action) ;

	int rank = get_action_todo_rank(action, first_action) ;
	int res = ( (rank < nb_act_todo_max_si) & (rank>0)  ) ;

	TRACE1("<< exist_action_todo_before(%d)", res) ;

	return res ;

}

//rend l'action a realiser avant l'action action dans la liste
//d'indice first_action
T_state_type_action get_action_todo_before(T_state_type_action action,
											   T_state_type_action
											   first_action)
{
	TRACE2(">> get_action_todo_before(%d,%d)", action, first_action) ;

	//rang de l'action action dans la liste d'indice first_action
	//dans le tableau
	int rank = get_action_todo_rank(action, first_action) ;
	//si l'action n'est pas dans la liste : on est en erreur
	if ( (rank >= nb_act_todo_max_si ) || ( rank <= 0 ) )
		{
			TRACE2("Action a faire inexistante dans tab_actions_todo(%d,%d)",
				   first_action,
				   rank) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg(E_MS_ACTION_TODO_INEXISTANT),
						   first_action,
						   rank) ;
			assert(FALSE) ;
		}

	T_state_type_action res = tab_actions_todo_set[first_action][rank-1] ;

	TRACE1("<< get_action_todo_before(%d)", res) ;

	return res ;
}

//dit si il existe une action a mettre a jour apres l'action action
//apres l'execution de l'action action_executed
//rend TRUE si il existe une action a mettre a jour, FALSE sinon
extern int exist_action_toupdate(T_state_type_action action,
										  T_state_type_action
										  action_executed)
{
	TRACE2(">> exist_action_toupdate(%d,%d)", action, action_executed) ;

	int rank = get_action_toupdate_rank(action, action_executed) ;
	int res = ( (rank < nb_act_toupdate_max_si) & (rank>0)  ) ;

	TRACE1("<< exist_action_toupdate(%d)", res) ;

	return res ;

}


//rend l'action a mettre a jour avant l'action action
//apres l'execution de l'action action_executed
extern T_state_type_action get_action_toupdate(T_state_type_action
														  action,
														  T_state_type_action
														  action_executed)
{
	TRACE2(">> get_action_toupdate(%d,%d)", action, action_executed) ;

	//rang de l'action action dans la liste d'indice action_executed
	//dans le tableau des actions a mettre a jour
	int rank = get_action_toupdate_rank(action, action_executed) ;
	//si l'action n'est pas dans le tableau : on est en erreur
	if ( (rank >= nb_act_toupdate_max_si ) || ( rank < 0 ) )
		{
			TRACE2("action inexistante dans tab_actions_toupdate(%d,%d)",
				   action_executed,
				   rank) ;
			toplevel_error(FATAL_ERROR,
						   get_error_msg(E_MS_ACTION_TOUPDATE_INEXISTANT),
						   action_executed,
						   rank) ;
			assert(FALSE) ;
		}

	T_state_type_action res =
		tab_actions_toupdate_set[action_executed][rank-1] ;

	TRACE1("<< get_action_toupdate(%d)", res) ;

	return res ;
}

extern int get_nb_actions(void)
{
	return nb_actions_si ;
}

#endif /* ACTION_NG */
