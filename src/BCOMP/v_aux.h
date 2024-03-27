/******************************* CLEARSY **************************************
* Fichier : $Id: v_aux.h,v 2.0 1997-07-08 07:45:02 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique, fonctions auxiliares
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
#ifndef _V_AUX_H_
#define _V_AUX_H_

// Recherche d'une machine dans une liste de T_used_machine
extern T_used_machine *find_used_machine(T_symbol *machine_name,
										 T_used_machine *first) ;

#endif /* _V_AUX_H_ */


