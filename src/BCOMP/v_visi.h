/******************************* CLEARSY **************************************
* Fichier : $Id: v_visi.h,v 2.0 1997-07-17 09:22:43 ct Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des tables de visibilite
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
#ifndef _V_VISI_H_
#define _V_VISI_H_

// Initialisation des tables de visibilite
extern void init_visibility_tables(void) ;

// Lecture des table de visibilite
extern T_access_authorisation get_auth(T_access_entity_type type,
												T_access_entity_location location,
												T_access_from_context context) ;

// Lecture des table de visibilite, avec en retour une chaine qui
// decrit l'autorisation
// Attention cette chaine ne peut etre que lue ...
extern
	T_access_authorisation get_auth_with_details(T_access_entity_type type,
												 T_access_entity_location location,
												 T_access_from_context context,
												 char ** adr_string) ;

// Dump des tables au format LaTeX 2e
extern void latex2e_dump_visibility_tables(const char *file_name,
													const char *output_path) ;

// Dump des tables au format HTML
extern void html_dump_visibility_tables(const char *file_name,
												 const char *output_path) ;

// Calcul de T_access_entity_location a partir des machines
extern T_access_entity_location make_entity_location(T_machine *from,
															  T_machine *to) ;

//recherche dans une liste de machines utilisees, la machine
//passee en parametre.
extern T_access_entity_location find_entity_location_in_used_mch(T_used_machine *from_used_mch,
																		  T_machine *to,
																		  T_access_entity_location previous_res, 																		      T_access_entity_location can_be);
#endif /* _V_VISI_H_ */
