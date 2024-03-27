/******************************* CLEARSY **************************************
* Fichier : $Id: p_api.h,v 2.0 1999/12/01 17:41:10 clb Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface de la bibliotheque
*				du Parametrage de Resources
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
#ifndef _P_API_H_
#define _P_API_H_

#ifndef __RESOURCE__
// Pour les clients, definitions des macros EXTERN, ...
#include <p_port.h>
#endif // !__RESOURCE__

// Declaration de Fonctions
// _________________________

// Initialisation du Gestionnaire de Resources
EXTERN void init_resource_manager(void) ;

// Lecture de la valeur d'une ressource
EXTERN const char *lookup_resource(const char *tool_name_acp,
								   const char *resource_name_acp) ;

// Prise en compte d'un fichier de resources
EXTERN void load_resources(const char *system_file_name_acp,
						   const char *user_file_name_acp,
						   const char *explicit_file_name_acp) ;

// Archivage des resources dans un fichier
EXTERN void save_resources(void) ;

// Ecriture directe de resources
EXTERN void set_resource(const char *tool_name_acp,
						 const char *resource_name_acp,
						 const char *resource_value_acp) ;

// Liberation d'une ressource
EXTERN void delete_resource(const char *tool_name_acp,
							const char *resource_name_acp) ;

// Liberation de toutes les resources d'un outil
EXTERN void delete_tool_resources(const char *tool_name_acp) ;

// Liberation de toutes les resources
EXTERN void delete_all_resources(void) ;

// Liberation de toutes les resources definies explicitement
EXTERN void delete_explicit_resources(void) ;

// Liberation de toutes les resources definies par l'environnement urilisateur
EXTERN void  delete_user_resources(void) ;

// Liberation de toutes les resources definies par l'Atelier B
EXTERN void delete_atelierb_resources(void) ;

// Informations
EXTERN void resource_info(void) ;

//Impression de toutes les ressources dans le flux stream_file
EXTERN void print_resources(FILE *stream_file) ;

#endif // _P_API_H_
