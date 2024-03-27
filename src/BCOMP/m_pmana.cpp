/******************************* CLEARSY **************************************
* Fichier : $Id: m_pmana.cpp,v 2.0 2006-11-28 09:56:05 atelierb Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*					Classe de gestion des projets
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
#include "c_port.h"
RCS_ID("$Id: m_pmana.cpp,v 1.40 2006-11-28 09:56:05 atelierb Exp $") ;


/* Includes systeme */
#include <sys/stat.h>

/* Includes bibliotheques */
#include "c_api.h"
#include "c_out.h"

/* Includes Composant Local */
#include "m_compo.h"
#include "m_file.h"
#include "m_projec.h"
#include "m_pmana.h"
#include "m_msg.h"
#include "m_clean.h"

// Constructeur
T_project_manager::T_project_manager(void) : T_object(NODE_PROJECT_MANAGER)
{
  TRACE1("T_project_manager::T_project_manager(%x)", this);

  current = NULL;
  save_flag = 0;
  file = NULL;
  file_date = -1; // on met une date negative pour etre sur qu'elle
                  // sera toujours inferieure

  // Initialisation des listes a vide
  first_library = NULL;
  last_library = NULL;
  first_missing = NULL;
  last_missing = NULL;
}

// Destructeur
T_project_manager::~T_project_manager(void)
{
  TRACE1("T_project_manager::~T_project_manager(%x)", this);
}

// Chargement des composants du projet courant (Full Path)
int T_project_manager::load_current_project(const char *full_path)
{
    TRACE2("T_project_manager(%x)::load_current_project(%s)",
           this, full_path);

    struct stat status;	  // descripteur fichier

    // !!! Destruction du projet courant (si necessaire)
    if (current != NULL)
      {
        TRACE0("Destruction du projet courant");
        current->unlink();
      }

    // Destruction du chemin associe (si necessaire)
    if (file != NULL)
      {
        TRACE0("Destruction du chemin associe");
        file = NULL;
        file_date = -1;
      }

    file = lookup_symbol(full_path);

    // Recuperation de la date du fichier .db
    if (stat(full_path, &status) == 0)
      {
        TRACE1("file_date = %d", status.st_mtime);
        file_date = status.st_mtime;
      }

    // chargement du projet
    current = load_project_xml(file->get_value());

    if (current == NULL)
      {
        // cas ou le chargement a echoue
        file = NULL;
        file_date = -1;
        // !!! Affichage des messages
        return 1;
      }

    save_flag = 0;

    // on recupere le timestamp present dans le fichier
    set_timestamp(current->get_timestamp());
    // on l'incremente pour etre sur de ne pas avoir le meme
    next_timestamp();

    // on charge les source path associes au projet
    current->load_source_paths();

    return 0;
}

// Chargement des composants du projet courant
int T_project_manager::load_current_project(const char *dir,
						     const char *name)
{
  TRACE3("T_project_manager(%x)::load_current_project(%s,%s)",
		 this, dir, name);

  // Construction du chemin du fichier
  char *path;
  path = new char[strlen(dir) + strlen(name) + 5]; // "/.db" et \0
  sprintf(path, "%s/%s.db", dir, name);

  if (load_current_project(path)) {
      return 1;
  }

  delete [] path;

  return 0;
}

int T_project_manager::reload_current_project(void)
{
  TRACE1("T_project_manager(%x)::reload_current_project()", this);
  ASSERT(file != NULL);
  ASSERT(current != NULL);

  struct stat status;	  // descripteur fichier

  // Si le fichier .db n'a pas ete modifie, on ne fait rien :
  // le projet n'a pas ete modifie.
  if ((stat(file->get_value(), &status) == 0) &&
	  status.st_mtime == file_date)
	{
	  TRACE0("Le reload est inutile ...");
	  return 0;
	}

  // on recupere la date du nouveau fichier .db que l'on va charger
  file_date = status.st_mtime;

  // chargement du projet
  T_project *new_current = load_project_xml(file->get_value());

  if (new_current == NULL)
	{
	  // cas ou le chargement a echoue
	  // !!! Affichage des messages
	  file_date = -1;
	  return 1;
	}

  // Destruction du projet courant
  current->unlink();

  // on change de projet
  current = new_current;
  save_flag = 0;

  // on recupere le timestamp present dans le fichier
  set_timestamp(current->get_timestamp());
  // on l'incremente pour etre sur de ne pas avoir le meme
  next_timestamp();

  // on charge les source path associes au projet
  current->load_source_paths();

  return 0;
}

void T_project_manager::remove_current_project(void)
{
  TRACE1("T_project_manager(%x)::remove_current_project()", this);
  ASSERT(file != NULL);
  ASSERT(current != NULL);

  // supression du fichier de description (si present)
  remove(file->get_value());

  // Destruction du projet courant
  current->unlink();
  current = NULL;
  file = NULL;
  file_date = -1;
}

void T_project_manager::unload_current_project(void)
{
  TRACE1("T_project_manager(%x)::unload_current_project()", this);
  ASSERT(file != NULL);
  ASSERT(current != NULL);

  // Destruction du projet courant
  current->unlink();
  current = NULL;
  file = NULL;
  file_date = -1;
}

static int move_file(const char *old_name, const char *new_name)
{
#ifndef WIN32
	return rename(old_name, new_name);
#else /* WIN32 */
	if(CopyFile(old_name, new_name, FALSE))
	{
		DeleteFile(old_name);
		return 0;
	}
	return 1;
#endif /* WIN32 */
}

// Sauvegarde sur disque du projet courant
int T_project_manager::save_current_project(void)
{
  TRACE1("T_project_manager(%x)::save_current_project()", this);
  ASSERT(file != NULL);
  ASSERT(current != NULL);

  // descripteur fichier
  struct stat status;

  // enregistrement du dernier timestamp
  current->set_timestamp(get_timestamp());

  char *tmp_db = (char *)s_malloc(strlen(file->get_value()) + 5);
                   /*12345 */
  sprintf(tmp_db, "%s_tmp", file->get_value());

  T_io_diagnostic io_diagnostic = current->save_project_xml(tmp_db);
  // Erreur de sauvegarde
  if (io_diagnostic != IO_OK) {
    toplevel_error(CAN_CONTINUE,
		   get_error_msg(E_PROBLEM_WRITING_FILE),
		   tmp_db,
		   strerror(errno)) ;
    unlink(tmp_db);
    s_free(tmp_db);
    return 1;
  }
  else {
    // l'ecriture du fichier .db temporaire s'est bien passee,
    // on peut faire un move dans le .db et effacer l'ancien.
    // Le rename est atomique, donc on ne risque pas d'etre interrompu
    if (move_file(tmp_db, file->get_value()) != 0) {
      unlink(tmp_db);
      s_free(tmp_db);
      toplevel_error(FATAL_ERROR,
		     get_error_msg(E_PROBLEM_WRITING_FILE),
		     file->get_value(),
		     strerror(errno)) ;
    }
  }
  s_free(tmp_db);

  // on recupere la date du nouveau fichier .db que l'on a sauvegarde
  if (stat(file->get_value(), &status) == 0)
	{
	  file_date = status.st_mtime;
	}
  else
	{
	  file_date = -1;
	}

  save_flag = 0;

  TRACE0("save_current_project -> 0");
  return 0;
}

// recherches sur les utilisateurs du projet courant
int T_project_manager::m_is_component_user(const char *username)
{
  TRACE2("T_project_manager(%x)::m_is_component_user(%s)", this, username);
  ASSERT(file != NULL);

  T_symbol *usersym = lookup_symbol(username);
  int found = 0;
  T_component *comp;
  comp = current->get_components();
  while ( (!found) && (comp != NULL) )
	{
	  if (usersym->compare(comp->get_file()->get_user()) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  comp = (T_component *)comp->get_next();
		}
	}
  return(found);
}

int T_project_manager::m_is_unique_component_user(const char *username)
{
  TRACE2("T_project_manager(%x)::m_is_unique_component_user(%s)", this, username);
  ASSERT(file != NULL);

  T_symbol *usersym = lookup_symbol(username);
  int other = 0;
  T_component *comp;
  comp = current->get_components();
  while ( (!other) && (comp != NULL) )
	{
	  if (!usersym->compare(comp->get_file()->get_user()) == TRUE)
		{
		  other = 1;
		}
	  else
		{
		  comp = (T_component *)comp->get_next();
		}
	}
  return(other);
}



// Chargement d'un bibliotheque
T_project *T_project_manager::load_library(const char *dir,
													const char *name,
													int index)
{
  TRACE4("T_project_manager(%x)::load_library(%s,%s, %d)",
		 this, dir, name, index);

  // construction du nom complet du fichier
  char *path = new char[strlen(dir) + strlen(name) + 5]; // "/.db" et \0
  sprintf(path, "%s/%s.db", dir, name);

  T_project *res = load_library(path, index) ;
  delete [] path;
  return res ;
}

// Chargement d'un bibliotheque
T_project *T_project_manager::load_library(const char *full_path,
													int index)
{
  TRACE3("T_project_manager(%x)::load_library(%s, %d)",
		 this, full_path, index);

  // lecture du fichier et creation du projet
//   T_io_diagnostic io_diagnostic;
  T_betree_file_type ftype = FILE_COMPONENT_LIST ;
//   T_project *lib = (T_project *)load_object(full_path,
// 											ftype,
// 											io_diagnostic);
  T_project *lib = load_project_xml(full_path);

  if (lib == NULL)
	{
	  // !!! Afficher les messages
	  return NULL;
	}

  // modification du type de tous les composants du projet
  lib->set_components_type(M_LIBRARY);

  // modification de l'index du projet
  lib->set_index(index);

  // modification de l'index des composants
  lib->set_components_index(index);

  // chainage du projet dans la liste des libraries
  lib->tail_insert((T_item **)&first_library, (T_item **)&last_library);

  return lib;
}

// Destruction des bibliotheques
void T_project_manager::unlink_libraries(void)
{
  TRACE1("T_project_manager(%x)::unlink_libraries()", this);
  list_unlink((T_item *)first_library);
  first_library = NULL;
  last_library = NULL;
}

// Recherche dans les bibliotheques
T_component *T_project_manager::search_library(const char *comp_name)
{
  TRACE2("T_project_manager(%x)::search_library(%s)", this, comp_name);
  int found = 0;
  T_component *comp = NULL;
  T_project *libprj;
  libprj = first_library;
  while ( (!found) && (libprj != NULL) )
	{
	  comp = libprj->search_component(comp_name);
	  if (comp != NULL)
		found = 1;
	  else
		libprj = (T_project *)libprj->get_next();
	}
  if (found)
	return comp;
  else
	return NULL;
}

// Recherche d'un composant dans le projet puis dans les bibliotheques
// Retour : chemin d'acces au composant APRES decoupage des fichiers
// multi-composants (sans le nom du fichier)
// ATTENTION le nom du composant ne doit pas avoir de suffixe
T_symbol *T_project_manager::find_component_path(const char *comp_name)
{
  TRACE2("T_project_manager(%x)::find_component_path(%s)", this, comp_name);

  T_project *libprj = first_library;

  // Recherche dans le projet courant
  if ((current != NULL) && (current->search_component(comp_name) != NULL))
	{
	  TRACE0("Composant trouve dans le projet courant");
	  TRACE1("T_project_manager::find_component_file --> %s",
			 current->get_decomp_dir()->get_value());
	  return current->get_decomp_dir();
	}
  //else
  // Recherche dans les bibliotheques
  while ( libprj != NULL )
	{
	  if ( libprj->search_component(comp_name) != NULL )
		{
		  TRACE0("Composant trouve dans une bibliotheque");
		  TRACE1("T_project_manager::find_component_file --> %x",
				 libprj->get_decomp_dir());
		  return libprj->get_decomp_dir();
		}
	  libprj = (T_project *)libprj->get_next();
	}

  TRACE0("Composant pas trouve");
  return NULL ;
}

// Ajout SL 29/03/1999
// Recherche d'un composant dans le projet puis dans les bibliotheques
// Retour : chemin d'acces au composant AVANT decoupage des fichiers
// multi-composants (sans le nom du fichier)
// ATTENTION le nom du composant ne doit pas avoir de suffixe
T_symbol *
	T_project_manager::find_original_component_path(const char *comp_name)
{
  TRACE2("T_project_manager(%x)::find_original_component_path(%s)",
		 this,
		 comp_name);

  T_project *libprj = first_library;
  T_component *component = NULL ;

  // Recherche dans le projet courant
  if (current != NULL)
	{
	  component = current->search_component(comp_name) ;

#ifdef TRACE
	  if (component != NULL)
		{
		  TRACE1("Composant trouve dans le projet courant %x", component) ;
		}
#endif // TRACE

	}

  if (component == NULL)
	{
	  // Recherche dans les bibliotheques
	  int done = (libprj == NULL) ;
	  while (done == FALSE)
		{
		  component = libprj->search_component(comp_name) ;

		  if (component == NULL)
			{
			  libprj = (T_project *)libprj->get_next();
			  if (libprj == NULL)
				{
				  done = TRUE ;
				}
			}
		  else
			{
			  TRACE1("Composant trouve dans une bibliotheque %x", component);
			  done = TRUE ;
			}
		}
	}

  if (component == NULL)
	{
	  TRACE0("Composant pas trouve");
	  return NULL ;
	}

  // Ici on a un T_component valide. Le chemin est dans le T_file_component associe
  return component->get_file()->get_path() ;
}

// Ajout SL 06/05/1999
// Renvoie l'index de la bibliotheque qui contient le composant, ou 0
int T_project_manager::find_component_library_index(const char *comp_name)
{
  TRACE2("T_project_manager(%x)::find_component_library_index(%s)",
		 this,
		 (comp_name == NULL) ? "null" : comp_name);

  // Recherche dans le projet courant
  if ((current != NULL) && (current->search_component(comp_name) != NULL))
	{
	  TRACE0("Composant trouve dans le projet courant");
	  return 0 ;
	}

  // Recherche dans les bibliotheques
  T_project *libprj = first_library;
  while (libprj != NULL)
	{
	  if (libprj->search_component(comp_name) != NULL)
		{
		  TRACE1("Composant trouve dans la bibliotheque %d", libprj->get_index()) ;
		  return libprj->get_index() ;
		}
	  libprj = (T_project *)libprj->get_next();
	}

  TRACE0("Composant pas trouve");
  return 0 ;
}

// Recherche d'un composant manquant
T_component *T_project_manager::search_missing_component(const char *name)
{
  TRACE2("T_project_manager(%x)::search_missing_component(%s)", this, name);
  T_symbol *symb = lookup_symbol(name);
  T_component *comp = first_missing;
  while (comp != NULL)
	{
	  if (symb->compare(comp->get_name()) == TRUE)
		{
		  return comp;
		}
	  comp = (T_component *)comp->get_next();
	}
  return NULL;
}

// Creation d'un composant manquant
T_component *T_project_manager::create_missing_component(const char *name)
{
  TRACE2("T_project_manager(%x)::create_missing_component(%s)", this, name);
  T_component *comp = NULL;
  comp = this->search_missing_component(name);
  if (comp == NULL)
	{
	  T_symbol *namesym = lookup_symbol(name);
	  comp = new T_component(namesym, MTYPE_SPECIFICATION, M_MISSING,
							 (T_file_component *)NULL,
							 (T_symbol *)NULL,
							 (T_item **)&first_missing,
							 (T_item **)&last_missing,
							 (T_item *)this);
	}
  return comp;
}


// Destruction des composants manquants
void T_project_manager::unlink_missing_components(void)
{
  TRACE1("T_project_manager(%x)::unlink_missing_components()", this);
  list_unlink((T_item *)first_missing);
  first_missing = NULL;
  last_missing = NULL;
}

// Ajout d'un composant manquant
void T_project_manager::add_missing_component(T_component *comp)
{
  TRACE3("T_project_manager(%x)::add_missing_component(%x:%s)", this, comp,
		 comp->get_name()->get_value());
  comp->set_file(NULL);
  comp->set_machine(NULL);
  comp->set_type(M_MISSING);
  comp->tail_insert((T_item **)&first_missing, (T_item **)&last_missing);
}

// Composants supprimes des fichiers multi-composants
void T_project_manager::unlink_removed_components(void)
{
  TRACE1("T_component_manager(%x)::unlink_removed_components()", this);
  list_unlink((T_item *)first_removed);
  first_removed = NULL;
  last_removed = NULL;
}
void T_project_manager::add_removed_component(T_component *comp)
{
  ASSERT(comp != NULL);
  TRACE3("T_project_manager(%x)::add_removed_component(%x:%s)", this, comp,
		 comp->get_name()->get_value());

  (void)new T_list_link(comp,
						LINK_OTHER,
						NULL,
						(T_item **)&first_removed,
						(T_item **)&last_removed,
						(T_item *)this,
						NULL,
						NULL);
}

// Composants ajoutes a des fichiers multi-composants
void T_project_manager::unlink_added_components(void)
{
  TRACE1("T_project_manager(%x)::unlink_added_components()", this);
  list_unlink((T_item *)first_added);
  first_added = NULL;
  last_added = NULL;
}
void T_project_manager::add_added_component(T_component *comp)
{
  ASSERT(comp != NULL);
  TRACE3("T_project_manager(%x)::add_added_component(%x:%s)", this, comp,
		 comp->get_name()->get_value());
  (void)new T_list_link(comp,
						LINK_OTHER,
						NULL,
						(T_item **)&first_added,
						(T_item **)&last_added,
						(T_item *)this,
						NULL,
						NULL);
  TRACE1("Sortie de added_component de %s", comp->get_name()->get_value());
}


// Destruction de la liste des machines de tous les composants
void T_project_manager::unlink_machines(void)
{
  TRACE1("T_project_manager(%x)::unlink_machines()", this);

  T_component *comp = current->get_components();
  T_file_component *file_compo = NULL;

  while (comp != NULL)
	{
	  comp->set_machine(NULL);
	  file_compo = comp->get_file();
	  file_compo->raz_date();
	  file_compo->set_file_definition(NULL);
	  comp = (T_component *)comp->get_next();
	}
  TRACE0("T_project_manager::unlink_machines() -> OK");
}

int T_project_manager::test_file_def(T_component *comp, const char* converterName) {
    T_file_component *cur_file = current->get_files();

    // Betree d'un fichier
    T_betree *betree = NULL;
    // Liste des definitions
    T_file_definition *file_def = NULL;

    while (cur_file != NULL)
    {
        // Chargement des betrees du fichier
        betree = cur_file->load_betrees(BC_CALL_DEPENDENCE, converterName);

        if(betree == NULL)
        {
            return 0 ;
        }
        else
        {
            TRACE2("Betree : %x, %s",
                   betree,
                   betree->get_root()->get_machine_name()->get_name()->get_value());
            file_def = betree->get_file_definitions();

            // lecture de tous les fichiers de definition du composant
            if (file_def != NULL)
            {
                this->free_betrees();
                return 1;
            }

            this->free_betrees();

            cur_file = (T_file_component *)cur_file->get_next();
        }

        TRACE0("T_project_manager::set_file_def --> fin");
    }
    return 0 ;
   }

// R�cup�ration de la liste des fichiers de definition
// retourne 0 si pas d'erreur rencontre lors du chragement
// des betrees 1 sinon
int T_project_manager::set_file_def(const char* converterName)
{
  TRACE1("T_project_manager(%x):set_file_def()", this);

  // Liste des fichiers du projet
  T_file_component *cur_file = current->get_files();

  // Betree d'un fichier
  T_betree *betree = NULL;
  // Liste des definitions
  T_file_definition *file_def = NULL;
  T_list_link *new_link = NULL;
  T_symbol *file_def_path = NULL;
  T_symbol *file_def_name = NULL;
  // Parcours de la liste des fichiers de def
  T_list_link *tmp_link = NULL;
  T_file_definition *tmp_def = NULL;
  // Flag pour tester si un file_def est deja dans la liste
  int already = FALSE;

  while (cur_file != NULL)
	{
	  // Chargement des betrees du fichier
          betree = cur_file->load_betrees(BC_CALL_DEPENDENCE, converterName);

	  if(betree == NULL)
		{
		  return 1 ;
		}
	  else
		{
		  // lecture de tous les betree des composants du fichier
		  while (betree != NULL)
			{
			  TRACE2("Betree : %x, %s",
					 betree,
					 betree->get_root()->get_machine_name()->get_name()->get_value());
			  file_def = betree->get_file_definitions();

			  // lecture de tous les fichiers de definition du composant
			  while (file_def != NULL)
				{
				  TRACE1("file_def a rajouter : %x", file_def);
				  file_def_name = file_def->get_name();
				  file_def_path = file_def->get_path();

				  TRACE1("Nom : %s", file_def_name->get_value());

				  // on verifie qu'on ne l'a pas deja
				  tmp_link = first_file_def;
				  already = FALSE;
				  while((tmp_link != NULL) && (already == FALSE))
					{
					  tmp_def =
						(T_file_definition *)tmp_link->get_object();
					  if (file_def_name->compare(tmp_def->get_name()))
						{
						  // ils ont le meme nom; ont-il le meme path ?
						  if ((file_def_path == NULL) &&
							  (tmp_def->get_path()== NULL))
							{
							  // il s'agit d'un file_def
							  // qu'on a deja ajoute
							  TRACE0("deja present dans la liste !");
							  already = TRUE;
							}
						  else if (file_def_path->compare(tmp_def->get_path()))
							{
							  // il s'agit d'un file_def
							  // qu'on a deja ajoute
							  TRACE0("deja present dans la liste !");
							  already = TRUE;
							}
						}
					  tmp_link = (T_list_link *)tmp_link->get_next();
					}

				  if (already == FALSE)
					{
					  // On peut le rajouter a la liste

					  if (file_def_path == NULL)
						{
						  // On est dans le cas d'un fichier
						  //de definition local
						  // on recupere le chemin du fichier
						  //du composant associe
						  TRACE0("cas d'un fichier de def local");
						  file_def->set_usr1(cur_file->get_path());
						}
					  TRACE1("Path : %s",
							 (file_def_path?
							  file_def_path->get_value():
							  ((T_symbol *)file_def->get_usr1())->get_value()));
					  file_def->link();
					  new_link = new T_list_link(file_def,
												 LINK_OTHER,
												 NULL,
												 (T_item **)&first_file_def,
												 (T_item **)&last_file_def,
												 (T_item *)this,
												 NULL,
												 NULL);
					  // on positionne le flag pour qu'il libere
					  // son objet
					  new_link->set_do_free();
					}
				  // sinon : il y est deja, on ne le remet pas !

				  file_def = (T_file_definition *)file_def->get_next();
				}

			  betree = (T_betree *)betree->get_next();
			}

			  this->free_betrees();

			  cur_file = (T_file_component *)cur_file->get_next();
		  }

	  TRACE0("T_project_manager::set_file_def --> fin");
	}
  return 0 ;
}


  // Destruction de la liste des fichiers de definition
void T_project_manager::unlink_file_def(void)
{
  TRACE1("T_project_manager(%x):unlink_file_def()", this);

  T_list_link *file_def = first_file_def;

  if (file_def != NULL)
	{
	  list_unlink(file_def);
	}

  first_file_def = NULL;
  last_file_def = NULL;

  TRACE0("T_project_manager::unlink_file_def --> fin");
}


// Recopie des checksums d'un ancien projet
int T_project_manager::copy_checksums(const char *dir,
											   const char *name)
{
  TRACE2("T_project_manager::copy_checksums(%s, %s)", dir, name);

  char *old_db_path = NULL;   // chemin de l'ancien fichier .db
  T_project *old_project;
  int res = FALSE;

  old_db_path = (char *)s_malloc(strlen(dir) + strlen(name) + 5);
                 /*1  2345*/
  sprintf(old_db_path, "%s/%s.db", dir, name);

  // chargement du fichier et creation du projet courant
  old_project = load_project_xml(old_db_path);

  s_free(old_db_path);

  if (old_project == NULL)
	{
	  return FALSE;
	}

  res = current->copy_checksums(old_project);

  old_project->unlink();

  return res;
}

// Liberation des betree charges dans le betree_manager
int T_project_manager::free_betrees(void)
{
  TRACE1("T_project_manager(%x)::free_betrees()", this);

  // Nouveau mode de liberation
  TRACE0("Nettoyage");
  ms_memory_cleanup();

  TRACE1("sortie de free_betrees -> %d", FALSE);
  return FALSE;
}
