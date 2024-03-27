/******************************* CLEARSY **************************************
* Fichier : $Id: main.cpp,v 2.0 1999/12/07 15:37:32 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : test du gestionnaire de ressource
*
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
RCS_ID("$Id: main.cpp,v 1.1 1999/12/07 15:37:32 clb Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Local Components */
#include <c_api.h>
#include <p_api.h>
#include "load.h"


//
// Pour getopt on n'inclue pas <getopt.h> qui n'est pas fourni
// partout, on se contente de donner le prototype. Le module
// c_port.h fournit une implementation pour les cibles qui
// ne connaissent pas getopt
//
extern int getopt(int argc, char **argv, char *optstring) ;
extern char *optarg ;


 /************************************************************
 * Description : main
 ************************************************************/

int main(int argc, char *argv[])
{
  TRACE0("-- DEBUT DU PROGRAMME") ;

  char command[1024] ; //commande complete donnee par l'utilisateur
  const char *cmd = NULL ; //nom de la commandes

  // Initialisations du gestionnaire de ressources
  init_resource_manager() ;

  //récupération de la premiere commandes
  gets(command) ;
  cmd = strtok(command, " \t\n") ;

  while (strcmp(cmd,"q") != 0)
	{
  TRACE0("ici") ;
	  if(strcmp(cmd, "car") == 0)
		  {
			  //récupération des fichiers de ressource system et utilisateur
			  load_atelier_resources() ;
		  }
	  else if(strcmp(cmd, "cxr") == 0)
		  {
			  //récupération du fichier de ressource explicite
			  load_explicit_resources() ;
		  }
	  else if(strcmp(cmd, "d") == 0)
		  {
			  //effacement des ressources
			  delete_all_resources() ;
		  }
	  else if(strcmp(cmd, "p") == 0)
		  {
			  print_resources(stdout) ;
		  }
	  else if(strcmp(cmd, "v") == 0)
		  {
			  // Usage
			  printf("test_res -car system_resource_file_name user_resource_file_name \n -cxr explicit_resource_file_name\n -d \n -p \n -q \n") ;
		  }

		else
		  {
			  // Usage
			  printf("test_res -car system_resource_file_name user_resource_file_name \n -cxr explicit_resource_file_name\n -d \n -p \n -q \n") ;
		  }

	  //récupération de la commande suivante
	  gets(command) ;
	  cmd = strtok(command, " \t\n") ;

	}

  return 0;
}
