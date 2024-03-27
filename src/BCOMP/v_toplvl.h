/******************************* CLEARSY **************************************
* Fichier : $Id: v_toplvl.h,v 2.0 2002-02-13 15:56:28 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de haut niveau de l'analyseur semantique
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
#ifndef _V_TOPLVL_H_
#define _V_TOPLVL_H_

#include "v_vistbl.h"
#include "v_visi.h"

// Analyse semantique : controles au niveau projet
extern T_betree *project_analysis(T_betree *betree) ;

// Analyse semantique : type check
extern T_betree *type_check(T_betree *betree) ;

// Analyse semantique : verification de l'existence et de la
// localisation du typage
extern T_betree *check_typing_location(T_betree *betree) ;

// Analyse semantique : verification de l'existence et de la
// localisation des initialisations
extern T_betree *check_init_location(T_betree *betree) ;

// Analyse semantique : controles au niveau composant sur les operations
// Effet de bord : utilise le champ tmp2 et get_timestamp()
// pour detecter les bouclages
extern void op_analysis(T_machine *root) ;

// la fonction parcours la liste des opérations  et cherche un homonyme
// dans l'abstraction, dans le cas d'un succé elle crée les liens
// d'homonymie sur l'abstraction.
extern void solve_homonym_in_abstraction_op(T_machine *root);

#endif /* _V_TOPLVL_H_ */


