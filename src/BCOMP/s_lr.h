/******************************* CLEARSY **************************************
* Fichier : $Id: s_lr.h,v 2.0 1999-06-09 12:38:41 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface du moteur de l'analseur LR
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
#ifndef _S_LR_H_
#define _S_LR_H_

//
//	}{	Fonction qui appelle l'analyseur LR
//
extern T_item *syntax_call_LR_parser(T_item **adr_first,
											  T_item **adr_last,
											  T_item *father,
											  T_betree *betree,
											  T_lexem **adr_cur_lexem) ;

//
// }{ Etats de l'automate LR
//
typedef enum
	{
	LR_STATE_E0 = 0,// Etat E0 :
	LR_STATE_E1,   	// Etat E1 :
	LR_STATE_E2,   	// Etat E2 :
	LR_STATE_E3,   	// Etat E3 :
	LR_STATE_E4,   	// Etat E4 : dans la pile on a Exp/E0 ie on pourrait terminer
	LR_STATE_E5,   	// Etat E5 :
	LR_STATE_E6,   	// Etat E6 :
	LR_STATE_E7,   	// Etat E7 :
	LR_STATE_E8,   	// Etat E8 :
	LR_STATE_E9,   	// Etat E9 :
	LR_STATE_E10   	// Etat E10:
	} T_lr_state ;

extern void reset_LR_stacks(void) ;

#endif /* _S_LR_H_ */
