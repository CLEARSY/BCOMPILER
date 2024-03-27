/******************************* CLEARSY **************************************
* Fichier : $Id: m_action.h,v 2.0 2000-10-19 08:27:12 cc Exp $
*
* (C) 2008 CLEARSY
*
* Description : gestion des listes d'actions
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

#ifndef _M_ACTION_H_
#define _M_ACTION_H_


//type définissant le type d'action
//!!! pour la fonction remake, il prend les actions dans l'ordre
//de STA_BPP jusqu'a STA_PSCTRANSCVG
//-> ne pas changer l'ordre des actions
typedef enum
{
	/* 00 */
	STA_BPP = 0,
	/* 01 */
	STA_COMPONENTCHECK,
	/* 02 */
	STA_POGENERATE,
	/* 03 */
	STA_PROVE,
	/* 04 */
	STA_ADATRANS,
	/* 05 */
	STA_CTRANS,
	/* 06 */
	STA_PSCTRANSDVG,
	/* 07 */
	STA_PSCTRANSCVG,
	/* 08 */
	STA_CPPTRANS,
	/* 09 */
	STA_NEWPSCTRANSCONV,
	/* 10 */
	STA_NEWPSCTRANS,
	/* 11 */
	STA_LATEX,
	/* 12 */
	STA_INTERLEAF,
	/* 13 */
	STA_HIATRANS,
	/* 14 */
	STA_ALATRANS,
	/* 15 */
	STA_ALBTRANS,
	/* 16 */
	STA_NOACTION //DOIT ETRE LA DERNIERE VALEUR
	             //car elle est utilisee pour avoir le nombre des actions
} T_state_type_action ;


//type définissant le résultat d'une action
typedef  enum
{
	/* 00 */
	AS_OK = 0, // l'action s'est bien passee et a eu un résultat positif
	/* 01 */
	AS_KO //l'action s'est mal passée ou a eu un résultat négatif
} T_action_result ;

//fonctions externes

//dit si il existe une action avant "action"
//dans la liste commencant par first_action"
//rend TRUE si il existe une action avant, FLASE sinon
extern int exist_action_todo_before(T_state_type_action action,
											 T_state_type_action
											 first_action) ;

//rend l'action precedente a "action"
//dans la liste commencant par first_action"
extern T_state_type_action get_action_todo_before(T_state_type_action
														   action,
														   T_state_type_action
														   first_action) ;


//dit si il existe une action a mettre a jour apres l'action action
//apres l'execution de l'action action_executed
//rend TRUE si il existe une action a mettre a jour, FALSE sinon
extern int  exist_action_toupdate(T_state_type_action action,
										  T_state_type_action
										  action_executed) ;

//rend l'action a mettre a jour avant l'action action
//apres l'execution de l'action action_executed
extern T_state_type_action  get_action_toupdate(T_state_type_action
														  action,
														  T_state_type_action
														  action_executed) ;

//pour msremake : les actions doivent etre effectuees dans un certain ordre
//rend la 1ere action a faire lors d'un msremake
extern T_state_type_action  get_first_action(void) ;

//rend l'action suivante a faire lors d'un msremake
extern T_state_type_action  get_next_action(void) ;

//rend le nombre d'actions differentes
extern int  get_nb_actions(void) ;


#endif /* _M_ACTION_H_ */

#endif /* ACTION_NG */
