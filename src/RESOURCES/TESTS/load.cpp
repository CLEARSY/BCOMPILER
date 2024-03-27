/******************************* CLEARSY **************************************
* Fichier : $Id: load.cpp,v 2.0 1999/12/07 15:40:08 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : test du gestionnaire de ressource
*               fonctions de lecture des paramètres
*               et chargement des ressources
This file is part of RESOURCES
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESOURCES is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESOURCES is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESOURCES; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/#include "c_port.h"
RCS_ID("$Id: load.cpp,v 1.1 1999/12/07 15:40:08 clb Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Local Components */
#include <c_api.h>
#include <p_api.h>

//chargement des ressources de l'atelier : resources systeme et utilisateur
void load_atelier_resources(void)
{
	char system_file_name[1024] ; //fichier de ressource systeme
	char user_file_name[1024] ; //fichier de ressource utilisateur
	char *arg = NULL ; //argument de la commande

	TRACE0(">> load_atelier_resources") ;

	//récupération du fichier de ressource systeme
	arg = strtok(NULL, " \t\n") ;
	if( arg != NULL && strlen(arg)< 1024 )
		{
			strcpy(system_file_name, arg) ;
			TRACE1("system_file_name = %s", system_file_name) ;
		}
	else
		{
			// Usage
			printf("test_res -car system_resource_file_name user_resource_file_name \n -cxr explicit_resource_file_name\n -d \n -p \n -q \n") ;

			return ;
		}

	//récupération du fichier de ressource utilisateur
	if( arg != NULL && strlen(arg)< 1024 )
		{
			arg = strtok(NULL, " \t\n") ;
			strcpy(user_file_name, arg) ;
			TRACE1("user_file_name = %s", user_file_name) ;
		}
	else
		{
			// Usage
			printf("test_res -car system_resource_file_name user_resource_file_name \n -cxr explicit_resource_file_name\n -d \n -p \n -q \n") ;

			return ;
		}

	//chargement des ressources
	load_resources(system_file_name,
				   user_file_name,
				   NULL) ;
	TRACE0("<< load_atelier_resources") ;
}

//chargement des ressources explicites
void load_explicit_resources(void)
{
	char explicit_file_name[1024] ;//fichier de ressources explicite
	const char *arg = NULL ; //argument de la commande

	TRACE0(">> load_explicit_resources") ;

	//récupération du fichier de ressource explicite
	arg = strtok(NULL, " \t\n") ;
	TRACE1("arg = %s", arg) ;
	if( arg != NULL && strlen(arg)< 1024 )
		{
			strcpy(explicit_file_name, arg) ;
			TRACE1("explicit_file_name = %s", explicit_file_name) ;
		}
	else
		{
			// Usage
			printf("test_res -car system_resource_file_name user_resource_file_name \n -cxr explicit_resource_file_name\n -d \n -p \n -q \n") ;

			return ;
		}


	//chargement des ressources
	load_resources(NULL,
				   NULL,
				   explicit_file_name) ;

	TRACE0("<< load_explicit_resources") ;
}


