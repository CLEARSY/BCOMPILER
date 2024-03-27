/******************************* CLEARSY **************************************
* Fichier : $Id: p_synt.h,v 2.0 1998/11/02 14:16:36 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du module p_synth
*				Analyse syntaxique.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#ifndef _P_SYNTH_H_
#define _P_SYNTH_H_


// Declaration de Fonctions
// ________________________

// Adresse du Gestionnaire de Resources (source dans p_api.cpp)
extern T_resource_manager *get_resource_manager(void) ;

// Analayse syntaxique
extern void syntax_analysis(T_resource_line *first_line_aop) ;


#endif // _P_SYNTH_H_
