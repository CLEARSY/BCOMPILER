/******************************* CLEARSY **************************************
* Fichier : $Id: p_api.cpp,v 2.0 2007/12/03 14:21:06 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_api
*				Interface utilisateur.
*
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

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_api.cpp,v 1.11 2007/12/03 14:21:06 arequet Exp $") ;

// Includes systeme
#include <cstdio>

// Includes head
#include "p_head.h"


// Definition de Variables
// ________________________

// Pointeur sur l'unique manager
static T_resource_manager *init_manager_sop = NULL ;

// Definition de Fonctions
// ________________________

// Initialisation du Gestionnaire de Resources
EXTERN void init_resource_manager(void)
{
  ENTER_TRACE ;
  TRACE0("MANAGER INIT") ;

  TRACE1("Old MANAGER %x", init_manager_sop) ;

  if (init_manager_sop == NULL)
	{
	  init_manager_sop = new T_resource_manager(NULL, NULL, NULL) ;
	}

  TRACE1("New MANAGER %x", init_manager_sop) ;
  EXIT_TRACE ;
}

// Adresse du Gestionnaire de Resources
T_resource_manager *get_resource_manager(void)
{
  return init_manager_sop ;
}

// Lecture de la valeur d'une ressource
EXTERN const char *lookup_resource(const char *tool_name_acp,
											const char *resource_name_acp)
{
  ENTER_TRACE ;
  TRACE2("Lookup RESOURCE \"%s\" TOOL \"%s\"",
		 resource_name_acp,
		 tool_name_acp) ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  // Recherche l'outil
	  T_resource_tool *tool_lop = manager_lop->get_tool(tool_name_acp) ;

	  if (tool_lop != NULL)
		{
		  // Recherche la resource
		  T_resource_ident *resource_lop =
			tool_lop->get_resource(resource_name_acp) ;

		  if (resource_lop != NULL)
			{
			  // Recherche la valeur
			  T_resource_string *value_lop = resource_lop->get_value() ;

			  if (value_lop != NULL)
				{

				  TRACE1("Lookup RESOURCE result \"%s\"", value_lop->get_string()) ;
				  EXIT_TRACE ;

				  // Retourne la valeur
				  return value_lop->get_string() ;
				}
			}
		}
	}
  TRACE0("Lookup RESOURCE abort") ;
  EXIT_TRACE ;

  return NULL ;
}

// Lance l'analyse de lignes et de syntaxe d'un fichier
static void file_analysis(T_resource_file *file_aop)
{
  ENTER_TRACE ;
  TRACE1("-> FILE analysis \"%s\"",
		 (file_aop != NULL)? file_aop->get_name()->get_string():"VIDE") ;

  // Genere la liste de lignes du fichier
  T_resource_line *line_lop = file_aop->load() ;

  // Genere la liste de resources du fichier
  if (line_lop != NULL)
	{
	  syntax_analysis(line_lop) ;
	}

  TRACE1("<- FILE analysis \"%s\"",
		 (file_aop != NULL)? file_aop->get_name()->get_string():"VIDE") ;

  EXIT_TRACE ;
}

// Prise en compte d'un fichier de resources
EXTERN void load_resources(const char *system_file_name_acp,
									const char *user_file_name_acp,
									const char *explicit_file_name_acp)
{
  TRACE3(">> load_resources(\"%s\", \"%s\", \"%s\")",
		 (system_file_name_acp != NULL)? system_file_name_acp:"VIDE",
		 (user_file_name_acp != NULL)? user_file_name_acp:"VIDE",
		 (explicit_file_name_acp != NULL)? explicit_file_name_acp:"VIDE") ;
  ENTER_TRACE ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  T_resource_file *file_lop = NULL ;

	  // Cree le fichier systeme
	  if (system_file_name_acp != NULL)
		{
		  file_lop = manager_lop->set_file(system_file_name_acp,
										   RESOURCE_ATELIERB) ;

		  // Analyse du fichier
		  file_analysis(file_lop) ;
		}

	  // Cree le fichier utilisateur
	  if (user_file_name_acp != NULL)
		{
		  file_lop = manager_lop->set_file(user_file_name_acp, RESOURCE_USER) ;

		  // Analyse du fichier
		  file_analysis(file_lop) ;
		}

	  // Cree le fichier explicit
	  if (explicit_file_name_acp != NULL)
		{
		  file_lop = manager_lop->set_file(explicit_file_name_acp,
										   RESOURCE_EXPLICIT) ;

		  // Analyse du fichier
		  file_analysis(file_lop) ;
		}
	}

  EXIT_TRACE ;
  TRACE3("<< load_resources(\"%s\", \"%s\", \"%s\")",
		 (system_file_name_acp != NULL)? system_file_name_acp:"VIDE",
		 (user_file_name_acp != NULL)? user_file_name_acp:"VIDE",
		 (explicit_file_name_acp != NULL)? explicit_file_name_acp:"VIDE") ;
}

// Archivage des resources dans un fichier
void save_resources(void)
{
  ENTER_TRACE ;
  TRACE0("-> Save RESOURCE") ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  // Sauve le fichier explicit
	  if (manager_lop->get_file(RESOURCE_EXPLICIT) != NULL)
		{
		  TRACE0("-> Save explicit file") ;
		  manager_lop->get_file(RESOURCE_EXPLICIT)->save() ;
		}
	  // Sauve le fichier utilisateur
	  else
		{
		  T_resource_file *file_lop = manager_lop->get_file(RESOURCE_USER) ;

		  if (file_lop == NULL)
			{
			  // Si besion, cree un nouveau fichier utilisateur
			  file_lop = manager_lop->set_file(get_default_user_file(),
											   RESOURCE_USER) ;
			}

		  TRACE0("-> Save user file") ;
		  file_lop->save() ;
		}
	  // Le fichier systeme n'est jamais modifie
	}

  TRACE0("<- Save RESOURCE") ;
  EXIT_TRACE ;
}

// Ecriture directe de resources
void set_resource(const char *tool_name_acp,
						   const char *resource_name_acp,
						   const char *resource_value_acp)
{
  ENTER_TRACE ;
  TRACE3("-> Set RESOURCE \"%s\" value \"%s\" TOOL \"%s\"",
		 resource_name_acp,
		 resource_value_acp,
		 tool_name_acp) ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  // Recherche l'outil
	  T_resource_tool *tool_lop = manager_lop->get_tool(tool_name_acp) ;

	  // Ajoute le nouvel outils, si besoin
	  if (tool_lop == NULL)
		{
		  tool_lop = manager_lop->set_tool(tool_name_acp) ;
		}

	  // Met a jour la valeur de la resource
	  tool_lop->set_resource(resource_name_acp,
							 resource_value_acp,
							 NULL) ;
	}

  TRACE3("<- Set RESOURCE \"%s\" value \"%s\" TOOL \"%s\"",
		 resource_name_acp,
		 resource_value_acp,
		 tool_name_acp) ;
  EXIT_TRACE ;
}

// Liberation d'une ressource
void delete_resource(const char *tool_name_acp,
							  const char *resource_name_acp)
{
  ENTER_TRACE ;
  TRACE2("-> Del RESOURCE \"%s\" TOOL \"%s\"",
		 resource_name_acp,
		 tool_name_acp) ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  T_resource_tool *tool_lop = manager_lop->get_tool(tool_name_acp) ;

	  if (tool_lop != NULL)
		{
		  tool_lop->delete_resource(resource_name_acp) ;
		}
	}

  TRACE2("<- Del RESOURCE \"%s\" TOOL \"%s\"",
		 resource_name_acp,
		 tool_name_acp) ;
  EXIT_TRACE ;
}

// Liberation de toutes les resources d'un outil
void delete_tool_resources(const char *tool_name_acp)
{
  ENTER_TRACE ;
  TRACE1("-> Del RESOURCE of TOOL \"%s\"", tool_name_acp) ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  // Detruit l'outil
	  manager_lop->delete_tool(tool_name_acp) ;
	}

  TRACE1("<- Del RESOURCE of TOOL \"%s\"", tool_name_acp) ;
  EXIT_TRACE ;
}

// Liberation de toutes les resources
void delete_all_resources(void)
{
  ENTER_TRACE ;
  TRACE0("-> Del ALL RESOURCE") ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  manager_lop->delete_all_tools() ;
	}

  TRACE0("<- Del ALL RESOURCE") ;
  EXIT_TRACE ;
}

// Liberation de toutes les resources definies par un fichier
static void delete_file_resources(T_resource_source source_ae)
{
  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if (manager_lop != NULL)
	{
	  // Detruit le fichier
	  manager_lop->delete_file(source_ae) ;
	}
}

// Liberation de toutes les resources definies explicitement
void delete_explicit_resources(void)
{
  ENTER_TRACE ;
  TRACE0("-> Del explicit file") ;

  delete_file_resources(RESOURCE_EXPLICIT) ;

  TRACE0("<- Del explicit file") ;
  EXIT_TRACE ;
}

// Liberation de toutes les resources definies par l'environnement utilisateur
void delete_user_resources(void)
{
  ENTER_TRACE ;
  TRACE0("-> Del user file") ;

  delete_file_resources(RESOURCE_USER) ;

  TRACE0("<- Del user file") ;
  EXIT_TRACE ;
}

// Liberation de toutes les resources definies par l'Atelier B
void delete_atelierb_resources(void)
{
  ENTER_TRACE ;
  TRACE0("-> Del system file") ;

  delete_file_resources(RESOURCE_ATELIERB) ;

  TRACE0("<- Del system file") ;
  EXIT_TRACE ;
}


// Version de la bibliotheque
#include "version.h"

// Informations
EXTERN void resource_info(void)
{
  printf("- Resource Manager version %s (%s)\n"
	 "                   %s\n",
	 RESOURCES_VERSION, RESOURCES_SOURCE_CODE_SHA,
	 RESOURCES_COPYRIGHT);

  printf("The following flags have been used : ") ;
#ifdef TRACE
  printf("TRACE ") ;
#endif
#ifdef NO_CHECKS
  printf("NO_CHECKS ") ;
#endif
  printf("\n") ;
#ifdef __GNUC__
  printf("Compiled with gcc version %s\n", __VERSION__) ;
#endif
#ifdef WIN32
  printf("Compiled for Windows 32 target\n") ;
#endif
#ifdef IPX
  printf("Compiled for IPX target\n") ;
#endif
#ifdef HPUX
  printf("Compiled for HP-UX target\n") ;
#endif

#ifdef SOLARIS_5_3
  printf("Compiled for Solaris 5.3 target\n") ;
#endif
#ifdef SOLARIS_5_4
  printf("Compiled for Solaris 5.4 target\n") ;
#endif
#ifdef SOLARIS_5_5
  printf("Compiled for Solaris 5.5 target\n") ;
#endif
#ifdef SOLARIS_5_5_1
  printf("Compiled for Solaris 5.5.1 target\n") ;
#endif
#ifdef SOLARIS_5_6
  printf("Compiled for Solaris 5.6 target\n") ;
#endif
#ifdef SOLARIS_5_7
  printf("Compiled for Solaris 7 target\n") ;
#endif
#ifdef SOLARIS_5_8
  printf("Compiled for Solaris 8 target\n") ;
#endif
#ifdef SOLARIS_5_10
  printf("Compiled for Solaris 10 target\n");
#endif 

#ifdef LINUX
  printf("Compiled for Linux target\n") ;
#if (CPU==PENTIUM)
  printf("(Pentium version)\n") ;
#endif
#ifdef MAC_OS_X
  printf("Compiled for Mac OS X target\n") ;
#endif
#ifdef WIN32
  printf("Compiled for Windows target\n") ;
#endif
#endif
}

//Impression de toutes les ressources dans le flux stream_file
EXTERN void print_resources(FILE *stream_file)
{
  TRACE1("-> print_resources(%x)", stream_file) ;

  // Recherche le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  //impression de toutes les ressources du manager
  manager_lop->print_resources(stream_file) ;

  TRACE1("<- print_resources(%x)", stream_file) ;
}
//
// Fin de fichier
//
