/******************************* CLEARSY **************************************
* Fichier : $Id: m_projec.cpp,v 2.0 2007-01-08 17:15:25 atelierb Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Moniteur de session
*					Classe de gestion d'un projet
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
RCS_ID("$Id: m_projec.cpp,v 1.21 2007-01-08 17:15:25 atelierb Exp $") ;


/* Includes systeme */

/* Includes bibliotheques */
#include "c_api.h"

/* Includes Composant Local */
#include "m_api.h"
#include "m_compo.h"
#include "m_file.h"
#include "m_projec.h"
#include "m_msg.h"

/* Include XML*/
#include "tinyxml.h"
#include "tinystr.h"
#include "version_atb"		// Version de l'atelier inscrite dans le fichier xml

// Type de projet à utiliser dans le cas ou aucun projet n'est chargé.
T_project_type T_project::default_project_type = PROJECT_TYPE_UNKNWON;

T_project_type T_project::get_default_project_type(void)
{
    return default_project_type;
}

void T_project::set_default_project_type(T_project_type project_type)
{
    default_project_type = project_type;
}

// Constructeur

T_project::T_project(T_item **adr_first,
							  T_item **adr_last,
							  T_item *father)
  : T_item(NODE_PROJECT, adr_first, adr_last, father, nullptr, nullptr)
{
  TRACE1("T_project::T_project(%x)", this);
  TRACE2("T_project::T_project(%x):%s", this, this->get_class_name());
  index = -1;
  old_tools_decomp_dir = nullptr;
  decomp_dir = nullptr;
  timestamp = get_timestamp();

  // Initialisation des listes a vide
  first_source_path = nullptr;
  last_source_path = nullptr;
  first_file = nullptr;
  last_file = nullptr;
  first_component = nullptr;
  last_component = nullptr;
  first_proof_mechanism = nullptr;
  last_proof_mechanism = nullptr;
}

// Constructeur vide
T_project::T_project(void)
: T_item(NODE_PROJECT)
{
}

// Destructeur
T_project::~T_project(void)
{
  TRACE1("T_project::~T_project(%x)", this);

  TRACE0("On libere les fichiers");
  list_unlink(first_file);
  TRACE0("On libere les fichiers de definitions");
  list_unlink(first_source_path);

  // destruction de la liste des composants
  //TRACE1("Destruction liste des composant %x", first_component);
  //list_unlink((T_item *)first_component);
  TRACE0("On libere la liste des mécanismes de preuve.");
  list_unlink(first_proof_mechanism);
}

// ajout d'un repertoire de sources
int T_project::add_source_path_project(const char *new_path)
{
  ASSERT(new_path != NULL);
  TRACE2("T_project(%x)::add_source_path(%s)", this, new_path);
  T_symbol *symb = lookup_symbol(new_path);
  int found = 0;
  T_list_link *cur = first_source_path;
  T_symbol *cur_symb = NULL;
  while ( (!found) && (cur != NULL) )
	{
	  cur_symb = (T_symbol *)cur->get_object();
	  ASSERT(cur_symb != NULL);
	  TRACE1("cur_symb = %s", cur_symb->get_value());
	  if (symb->compare(cur_symb) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  cur = (T_list_link *)cur->get_next();
		}
	}
  if (found)
	{
	  // message d'erreur
	  user_error(CAN_CONTINUE,
				 get_user_error_msg(U_ADD_SOURCE_PATH),
				 new_path);
	  return TRUE;
	}
  else
	{
	  // chainage en queue
	  (void)new T_list_link(symb, LINK_OTHER,
							(T_item **)&first_source_path,
							(T_item **)&last_source_path,
							(T_item *)this,
							NULL, NULL);

	  // marquage pour liberation memoire
	  // il ne faut pas liberer l'objet associe car c'est un T_symbol ...
	  // new_link->set_do_free();
	  return FALSE;
	}
}

// suppression d'un repertoire de sources
int T_project::remove_source_path(const char *old_path)
{
  ASSERT(old_path != NULL);
  TRACE2("T_project(%x)::remove_source_path(%s)", this, old_path);
  T_symbol *symb = lookup_symbol(old_path);
  int found = 0;
  T_list_link *cur = first_source_path;
  T_symbol *cur_symb = NULL;
  while ( (!found) && (cur != NULL) )
	{
	  cur_symb = (T_symbol *)cur->get_object();
	  ASSERT(cur_symb != NULL);
	  if (symb->compare(cur_symb) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  cur = (T_list_link *)cur->get_next();
		}
	}
  if (!found)
	{
	  // message d'erreur
	  user_error(CAN_CONTINUE,
				 get_user_error_msg(U_REMOVE_SOURCE_PATH),
				 old_path);
	  return TRUE;
	}
  else
	{
	  // suppression de l'element
	  cur->remove_from_list((T_item **)&first_source_path,
							(T_item **)&last_source_path);
	  return FALSE;
	}
}

// chargement des repertoires de sources dans le compilateur
void T_project::load_source_paths(void)
{
  TRACE1("T_project(%x)::load_sources_path", this);
  reset_source_paths();
  T_list_link *cur = first_source_path;
  T_symbol *cur_symb = NULL;
  while (cur != NULL)
	{
	  cur_symb = (T_symbol *)cur->get_object();
	  TRACE1("cur_symb = %s", cur_symb->get_value());
	  add_source_path(cur_symb->get_value());
	  cur = (T_list_link *)cur->get_next();
	}
}

// modification du repertoire de decompilation (ancienne chaine)
void T_project::set_old_tools_decomp_dir(const char *dir, const char *subdir)
{
  TRACE3("T_project(%x)::set_old_tools_decomp_dir(%s,%s)", this, dir, subdir);
  ASSERT(dir != NULL);
  ASSERT(subdir != NULL);
  char *fullpath = new char [strlen(dir) + strlen(subdir) + 2]; // "/" et "\0"
  sprintf(fullpath, "%s%s%s", dir, MS_PATH_SEPARATOR_STRING, subdir);
  old_tools_decomp_dir = lookup_symbol(fullpath);
  delete [] fullpath;
}


// modification du repertoire de decompilation
void T_project::set_decomp_dir(const char *dir, const char *subdir)
{
  TRACE3("T_project(%x)::set_decomp_dir(%s,%s)", this, dir, subdir);
  ASSERT(dir != NULL);
  ASSERT(subdir != NULL);
  size_t len = strlen(dir) + 1 + strlen(subdir);
  char *fullpath = new char [len + 1]; // "/" et "\0"
  sprintf(fullpath, "%s%s%s", dir,MS_PATH_SEPARATOR_STRING,subdir);
  decomp_dir = lookup_symbol(fullpath, len);
  delete [] fullpath;
}

// modification du repertoire bdp
void T_project::set_bdp_dir(const char *fullpath)
{
  TRACE2("T_project(%x)::set_old_tools_decomp_dir(%s)", this, fullpath);
  ASSERT(fullpath != NULL);
  bdp_dir = lookup_symbol(fullpath);
}

// modification du repertoire de decompilation (ancienne chaine)
void T_project::set_old_tools_decomp_dir(const char *fullpath)
{
  TRACE2("T_project(%x)::set_old_tools_decomp_dir(%s)", this, fullpath);
  ASSERT(fullpath != NULL);
  old_tools_decomp_dir = lookup_symbol(fullpath);
}

// modification du repertoire de decompilation
void T_project::set_decomp_dir(const char *fullpath)
{
  TRACE2("T_project(%x)::set_decomp_dir(%s)", this, fullpath);
  ASSERT(fullpath != NULL);
  size_t len = strlen(fullpath) + 1;
  decomp_dir = lookup_symbol(fullpath, len);
}

// modification du type de tous les composants du projet
void T_project::set_components_type(T_component_type new_type)
{
  TRACE2("T_project(%x):set_components_type(%d)", this, new_type);

  T_component *comp = get_components();
  while (comp != NULL)
	{
	  comp->set_type(new_type);
	  comp = (T_component *)comp->get_next();
	}
}

// modification de l'index de tous les composants du projet
void T_project::set_components_index(int index)
{
  TRACE2("T_project(%x):set_components_index(%d)", this, index);

  T_component *comp = get_components();
  while (comp != NULL)
	{
	  comp->set_index(index);
	  comp = (T_component *)comp->get_next();
	}
}

// Ajout d'un composant avec tri par ordre alphabetique
void T_project::add_component_to_list(T_component *new_component)
{
  TRACE3("T_project(%x):add_component_to_list(%x:%s)",
		 this, new_component, new_component->get_name()->get_value());

  int found = FALSE;
  T_component *cur = first_component;
  T_component *prev = NULL;
  const char *new_name = new_component->get_name()->get_value();
  const char *list_name;

  // Recherche de la position d'insertion
  while ( (!found) && (cur != NULL) )
	{
	  list_name = cur->get_name()->get_value();
	  TRACE2("ici list_name %x <%s>",
			 list_name,
			 (list_name == NULL) ? "NULL" : list_name) ;
	  if (strcmp(list_name, new_name) > 0)
		{
		  found = 1;
		}
	  else
		{
		  prev = cur;
		  cur = (T_component *)cur->get_next();
		}
	} // fin while

  // insertion de l'element
  new_component->insert_after(prev,
							  (T_item **)&first_component,
							  (T_item **)&last_component);
  TRACE0("add_component_to_list --> OK");
}

// Recherche d'un composant dans la liste du projet
T_component *T_project::search_component(const char *name)
{
  TRACE2("T_project(%x):search_component(%s)",
		 this, name);

  T_component *cur = NULL;
  T_symbol *symb = lookup_symbol(name);

  cur = search_component(symb);

  TRACE1("search_component --> %x", cur);
  return(cur);
}

// Recherche d'un composant dans la liste du projet
T_component *T_project::search_component(T_symbol *name)
{
  TRACE2("T_project(%x):search_component(%x)",
		 this, name);

  int found = 0;
  T_component *cur = first_component;

  while ((!found) && (cur != NULL))
	{
	  if (name->compare(cur->get_name()) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  cur = (T_component *)cur->get_next();
		}
	}

  TRACE1("search_component --> %x", cur);
  return(cur);
}

// Suppression d'un composant
// Le composant est suppose chaine dans la liste
void T_project::remove_component(T_component *old_component)
{
  TRACE3("T_project(%x):remove_component(%x:%s)",
		 this, old_component, old_component->get_name()->get_value());

  old_component->remove_from_list((T_item **)&first_component,
								  (T_item **)&last_component);
}


// Mise a jour des checksums des composants suite a un "restore"
int T_project::copy_checksums(T_project *old_project)
{
  TRACE2("T_project(%x)::copy_checksums(%x)", this, old_project);

  T_component *cur_compo = first_component; // parcourt de la liste des
                                            // composants
  T_component *old_compo = NULL; // ancien composant
  int res = TRUE;                // resultat de la methode
  T_symbol *checksum = NULL;     // checksum des chaque composants du projet

  while (cur_compo != NULL)
	{
	  old_compo = old_project->search_component(cur_compo->get_name());
	  if (old_compo == NULL)
		{
		  res = FALSE;
		}
	  else
		{
		  checksum = old_compo->get_checksum();
		  TRACE2("Composant %s, checksum %s",
				 old_compo->get_name()->get_value(),
				 (checksum == NULL ? "NULL" : checksum->get_value()));
		  cur_compo ->set_checksum(checksum);
		}

	  cur_compo = (T_component *)cur_compo->get_next();
	}
  return res;
}

// Recherche d'un fichier
T_file_component *T_project::search_file(const char *name)
{
  TRACE2("T_project(%x):search_file(%s)",
		 this, name);

  int found = 0;
  T_file_component *cur = first_file;
  T_symbol *symb = lookup_symbol(name);

  while ( (!found) && (cur != NULL) )
	{
	  if (symb->compare(cur->get_name()) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  cur = (T_file_component *)cur->get_next();
		}
	}

  TRACE1("search_file --> %x", cur);
  return(cur);
}


// Ajout d'un fichier
void T_project::add_file_to_list(T_file_component *new_file)
{
  TRACE3("T_project(%x):add_file_to_list(%x:%s)",
		 this, new_file, new_file->get_name()->get_value());

  int found = FALSE;
  T_file_component *cur = first_file;
  T_file_component *prev = NULL;
  const char *new_name = new_file->get_name()->get_value();
  const char *list_name;

  // Recherche de la position d'insertion
  while ( (!found) && (cur != NULL) )
	{
	  list_name = cur->get_name()->get_value();
	  if (strcmp(list_name, new_name) > 0)
		{
		  found = 1;
		}
	  else
		{
		  prev = cur;
		  cur = (T_file_component *)cur->get_next();
		}
	} // fin while

  // insertion de l'element
  new_file->insert_after(prev, (T_item **)&first_file, (T_item **)&last_file);
}

// Supprime tous les fichiers générés par le composants passé en paramètre
void T_project::remove_generated_files(const char *component)
{
	T_file_component *current = get_files();

	while(current != NULL)
	{
		T_symbol *generated_from = current->get_generated_from_component();
		T_file_component *to_remove = NULL;
		if(generated_from && strcmp(generated_from->get_value(), component) == 0)
		{
			to_remove = current;
		}

		// On passe au fichier suivant avant d'enlever le fichier, afin
		// de pouvoir continuer à itérer
		current = (T_file_component*)current->get_next();

		if(to_remove)
		{
			remove_file(to_remove->get_name()->get_value());
		}
	}
}

// Suppression d'un fichier
// Le fichier est suppose chaine dans le manager
// Cette methode supprime les composants associes
void T_project::remove_file(const char *filename)
{
  TRACE2("T_project(%x):remove_file(%s)",
		 this, filename);

  // Recherche du fichier
  T_file_component *old_file = search_file(filename);
  ASSERT(old_file != NULL);

  // on enleve le fichier de la liste des fichiers
  old_file->remove_from_list((T_item **)&first_file, (T_item **)&last_file);

  // on enleve les composants du fichier de la liste des composants
  T_list_link *comp_link = old_file->get_components();
  T_component *comp;
  while (comp_link != NULL)
	{
	  comp = (T_component *)comp_link->get_object();
	  ASSERT(comp != NULL);
	  comp->remove_from_list((T_item **)&first_component, (T_item **)&last_component);
	  comp_link = (T_list_link *)comp_link->get_next();
	}

  // supression du fichier ==> supression des composants associes
  old_file->unlink();
  TRACE0("remove_file -> OK");
}

/*!
 * Returns the last occurence of the path separators characters, or
 * -1 if the given name does not contains any path separator
 *
 * \param name the name to test
 * \return the last path index
 */
static int last_path_index(const char *name)
{
  int result = -1;
  int index = 0;

  while(name[index] != '\0') {
    if(name[index] == '\\' || name[index] == '/') {
      result = index;
    }

    ++index;
  }

  return result;
}

static bool is_absolute_path(const char *file_name)
{
#ifndef WIN32
	return file_name[0] == '/';
#else
	return strlen(file_name) >= 2 && file_name[1] == ':';
#endif /* WIN32 */
}

// Ajout d'un fichier dans la liste du projet. Si "name" correspond à un chemin
// le répertoire est modifié.
int T_project::add_file(const char *file_name,  // nom + suffixe
                                 const char* converterName,
				 const char *directory,  // repertoire
				 const char *user,       // utilisateur courant
				 const char *generated_from)
{
  TRACE5("T_project(%x):add_file(%s,%s,%s,%s)",
	 this, file_name, directory, user, generated_from);
  ASSERT(file_name != NULL);
  ASSERT(directory != NULL);

  T_component *compo;
  char *fullname;
  T_betree *betree;
  T_file_component *new_file;
  T_component *new_comp;
  T_machine *machine;
  int erreur_flag = 0;
  char *name = NULL;
  char *dir = NULL;

  int last_index = last_path_index(file_name);
  if(last_index >= 0) {
    name = (char*)s_persistent_malloc(strlen(&file_name[last_index+1]) + 1);
    strcpy(name, &file_name[last_index+1]);

    if(is_absolute_path(file_name)) {
      // absolute path. Replaces directory with start of the path
      dir = (char*)s_persistent_malloc(last_index + 1);
      strncpy(dir, file_name, last_index);
      dir[last_index] = '\0';
    } else {
      // relative path: concatenate
      int dir_len = strlen(directory);
      dir = (char*)s_persistent_malloc(dir_len + 1 + last_index + 1);
      strcpy(dir, directory);
      dir[dir_len] = PATH_SEPARATOR_CHAR;
      strncpy(&dir[dir_len+1], file_name, last_index);
      dir[dir_len+1+last_index] = '\0';
    }
  } else {
    name = (char*)s_persistent_malloc(strlen(file_name) + 1);
    strcpy(name, file_name);
    dir = (char*)s_persistent_malloc(strlen(directory) + 1);
    strcpy(dir, directory);
  }

  // initialisation du compteur d'erreur
  reset_error_count();


  // Calcul du basename et du suffixe
  char *basename = (char *)s_persistent_malloc(strlen(name) + 1);
  strcpy(basename, name);
  char *p = strrchr(basename, '.');
  ASSERT(p != NULL);
  char *suffix = (char *)s_persistent_malloc(strlen(name) + 1);
  strcpy(suffix, p+1);
  *p = 0;

  // on regarde si il n'y a pas deja un fichier de meme nom
  new_file = search_file(basename);
  if (new_file != NULL)
    {
      user_warning(BASE_WARNING,
		   get_user_warning_msg(U_EXISTING_FILE),
		   name,
		   new_file->get_path()->get_value(),
		   new_file->get_name()->get_value(),
		   new_file->get_suffix()->get_value());
      s_free(basename);
      s_free(suffix);
      s_free(name);
      s_free(dir);

      return FALSE;
    }

  // appel au compilateur - passe dependances
  fullname = new char [strlen(name) + strlen(dir) + 2];
  sprintf(fullname, "%s%c%s", dir, PATH_SEPARATOR_CHAR, name);
  TRACE1("Recherche des dependances : %s", fullname);
  set_file_fetch_mode(FFMODE_LOCAL);
  compiler_dep_analysis(fullname, converterName);
  delete [] fullname;

  TRACE1("Nb Erreurs dependances : %d", get_error_count());

  // recuperation du ou des Betree
  betree = get_betree_manager()->get_betrees();
  if ( (betree == NULL) || (get_error_count() != 0) )
    {
      // Erreur : analyse des dependances impossible
      if (betree != NULL)
	{
	  get_project_manager()->free_betrees();
	}

      // on determine le type (a priori) en fonction du suffixe
      // pour les .mod on defini une machine par defaut.
      T_machine_type typeB;
      if (strcmp(suffix, "mch") == 0)
	typeB = MTYPE_SPECIFICATION;
      else if (strcmp(suffix, "ref") == 0)
	typeB = MTYPE_REFINEMENT;
      else if (strcmp(suffix, "imp") == 0)
	typeB = MTYPE_IMPLEMENTATION;
      else if (strcmp(suffix, "mod") == 0)
	typeB = MTYPE_SPECIFICATION;
      else if (strcmp(suffix, "sys") == 0)
	typeB = MTYPE_SYSTEM;
      else
	{
	  // cas d'un mauvais suffixe, on sort en erreur
	  // l'erreur est affichee par le compilateur
	  s_free(suffix);
	  s_free(basename);
	  s_free(name);
	  s_free(dir);

	  return TRUE;
	}
      TRACE1("Genere depuis : %s", generated_from);
      // creation d'un nouveau fichier
      new_file = new T_file_component(basename, suffix, dir, user, NULL, NULL, this);
      if(generated_from) {
	new_file->set_generated_from_component(lookup_symbol(generated_from));
      }

      // ajout du fichier a la liste
      this->add_file_to_list(new_file);

      // on cree uniquement un composant avec le nom du fichier
      new_comp = new T_component(lookup_symbol(basename),
				 typeB,
				 M_LOCAL,
				 new_file,
				 NULL,
				 NULL,
				 NULL,
				 this);
      // ajout du composant au fichier
      new_file->add_component(new_comp);

      // ajout du composant a la liste des composants
      this->add_component_to_list(new_comp);

      // liberation memoire
      s_free(basename);
      s_free(suffix);
      s_free(name);
      s_free(dir);

      TRACE0("T_project::add_file -> OK");
      return FALSE;
    }


  // parcours de la liste des betree pour verifier qu'il
  // n'y a pas de composant redefini.
  TRACE0("Boucle sur les betree pour verifier qu'il n'y a pas de composant redefini");
  while (betree != NULL)
    {
      machine = betree->get_root();
      TRACE2("Betree(%x) : nom machine : %s", betree, machine->get_machine_name()->get_name()->get_value());
      compo = search_component(machine->get_machine_name()->get_name());
      if (compo != NULL)
	{
	  // Erreur !! le fichier multi-composant contient un composant deja defini
	  user_error(CAN_CONTINUE,
		     get_user_error_msg(U_EXISTING_COMPONENT),
		     machine->get_machine_name()->get_name()->get_value(),
		     name);
	  erreur_flag = 1;
	}

      // suivant
      betree = betree->get_next_betree();
    }

  // Si pas d'erreur creation des donnees
  if (!erreur_flag)
    {
      // recuperation du premier betree
      betree = get_betree_manager()->get_betrees();


      // creation d'un nouveau fichier
      new_file = new T_file_component(basename, suffix, dir, user, NULL, NULL, this);
      if(generated_from) {
	new_file->set_generated_from_component(lookup_symbol(generated_from));
      }

      // ajout du fichier a la liste
      this->add_file_to_list(new_file);

      // liberation basename et suffixe
      s_free(basename);
      s_free(suffix);

      // Creation des composants
      while (betree != NULL)
	{
	  machine = betree->get_root();

	  // le checksum est NULL car on n'a pas fait l'expansion des
	  // des definitions.
	  new_comp = new T_component(machine->get_machine_name()->get_name(),
				     machine->get_machine_type(),
				     M_LOCAL,
				     new_file,
				     NULL,
				     NULL,
									 NULL,
				     this);

	  // ajout du composant au fichier
	  new_file->add_component(new_comp);

	  // ajout du composant a la liste des composants
	  this->add_component_to_list(new_comp);

	  // suivant
	  betree = betree->get_next_betree();
	}
    }

  // liberation des betree
  get_project_manager()->free_betrees();

  s_free(name);
  s_free(dir);

  // Sortie
  return erreur_flag;
}

// Fonctions de debug --- Affichage des informations sur le projet courant
void T_project::debug_print(void)
{
  TRACE0("T_project::debug_print");
  T_file_component *file = NULL;
  T_component *comp = NULL;
  T_list_link *link = NULL;

  file = first_file;
  while (file != NULL)
	{
          printf("File(%p): %s Components : ",
                         file,
			 file->get_name()->get_value());
	  link = file->get_components();
	  while (link != NULL)
		{
		  comp = (T_component *)link->get_object();
		  printf("%s ", comp->get_name()->get_value());
		  link = (T_list_link *)link->get_next();
		}
	  printf("\n");
	  file = (T_file_component *)file->get_next();
	}
  comp = first_component;
  printf("Components : ");
  while (comp != NULL)
	{
          printf("%s(%p) ", comp->get_name()->get_value(), comp->get_file());
	  comp =  (T_component *)comp->get_next();
	}
  printf("\n");
}

T_io_diagnostic T_project::save_project_xml(const char *file_name)
{
    // initialisation XML
  TiXmlNode* node = 0;
  TiXmlElement* dbElt = 0;
  TiXmlElement* projElt = 0;
  TiXmlElement* defdElt = 0;
  TiXmlElement* compdElt = 0;
  TiXmlElement* compElt = 0;
  TiXmlElement* pmElt {nullptr};

  // creation du document xml
  TiXmlDocument mydoc;

  // Déclaration
  TiXmlDeclaration mydec("1.0", "ISO-8859-1", "");
  mydoc.InsertEndChild(mydec);

  // balise db
  TiXmlElement mydb("db_xml");

  // balise atelierb et son attribut version
#ifndef VERSION_ATB
  const char *version = "3.7";
#else
  const char *version = VERSION_ATB;
#endif
  TiXmlElement myatelierb("atelierb");
  myatelierb.SetAttribute("version", ATB_VERSION);

  // balise project
  TiXmlElement myproject("project");
  myproject.SetAttribute("decomp_dir", this->get_decomp_dir()->get_value());
  myproject.SetAttribute("old_decomp_dir", this->get_old_tools_decomp_dir()->get_value());

  if (project_type == 0)
	  myproject.SetAttribute("type", "SYSTEM");
  if (project_type == 2)
          myproject.SetAttribute("type", "VALIDATION");

  // les sources paths du projet
  // commentaires XML
  TiXmlComment myComSP(" Definition directory list ");
  myproject.InsertEndChild(myComSP);
  // les definitions
  T_list_link *spath;
  spath = (T_list_link *)this->get_sources_path();
  while (spath != NULL)
    {
      T_symbol *sym = (T_symbol *)spath->get_object();
      TiXmlElement mydefd("definition_dir");
      mydefd.SetAttribute("path", sym->get_value());
      myproject.InsertEndChild(mydefd);
      spath = (T_list_link *)spath->get_next();
    }
  TiXmlComment pmXC(" Proof mechanisms ");
  myproject.InsertEndChild(pmXC);
  T_list_link* pmitr{this->get_proof_mechanisms()};
  while (pmitr != nullptr) {
      TiXmlElement pmXE("proof_mechanism");
      T_symbol* symb {dynamic_cast<T_symbol*>(pmitr->get_object())};
      pmXE.SetAttribute("name", symb->get_value());
      myproject.InsertEndChild(pmXE);
      pmitr = dynamic_cast<T_list_link*>(pmitr->get_next());
  }
  // fichiers du projets
  // commentaires XML
  TiXmlComment myComCP(" Component file list ");
  myproject.InsertEndChild(myComCP);
  // les fichiers
  T_file_component * my_file;
  T_list_link *my_compo;
  const char *my_mtype = NULL;
  my_file = this->get_files();
  while (my_file != NULL )
    {
      // fichiers
      TiXmlElement mycf("component_file");
      mycf.SetAttribute("path", my_file->get_path()->get_value());
      mycf.SetAttribute("name", my_file->get_name()->get_value());
      mycf.SetAttribute("suffix", my_file->get_suffix()->get_value());
      mycf.SetAttribute("user", my_file->get_user()->get_value());
      if(my_file->is_generated())
      {
    	  mycf.SetAttribute("generated_from", my_file->get_generated_from_component()->get_value());
      }

      // composants
      my_compo = my_file->get_components();
      while ( my_compo != NULL )
	{
	  // type
	  T_component *sym = (T_component *)my_compo->get_object();
	  switch (sym->get_machine_type())
	    {
	    case MTYPE_SPECIFICATION:
	      my_mtype = "specification";
	      break;
	    case MTYPE_SYSTEM:
		      my_mtype = "system";
		      break;
	    case MTYPE_REFINEMENT:
	      my_mtype = "refinement";
	      break;
	    case MTYPE_IMPLEMENTATION:
	      my_mtype = "implementation";
	      break;
	    default:
	      my_mtype = "erreur";
	    }

	  // composant
	  TiXmlElement myc("component");
	  myc.SetAttribute("name", sym->get_name()->get_value());
	  myc.SetAttribute("type", my_mtype);
	  mycf.InsertEndChild(myc);

	  // suite de la boucle
	  my_compo = (T_list_link *)my_compo->get_next();
	}
      myproject.InsertEndChild(mycf);

      // suite de la boucle
      my_file = (T_file_component *)my_file->get_next();
    }

  // imbrication
  mydb.InsertEndChild(myatelierb);
  mydb.InsertEndChild(myproject);
  mydoc.InsertEndChild(mydb);

  if (!mydoc.SaveFile(file_name)){
    TRACE0("Echec de mydoc.SaveFile()");
    return IO_ERROR;
  }
  return IO_OK;
}


// chargement d'un fichier xml en projet
T_project *load_project_xml(const char *file_name)
{
  TRACE1("--> load_project_xml(%s)", file_name);
  ENTER_TRACE ; ENTER_TRACE ;


  T_project *projet = new T_project(NULL, NULL, NULL);

  // chargement du fichier xml:
  TiXmlDocument doc(file_name);
  if (! doc.LoadFile()) {
    TRACE1("Echec du chargement(%s)", file_name);
    toplevel_error(CAN_CONTINUE,
		   get_error_msg(E_NO_SUCH_FILE),
		   file_name,
		   strerror(errno));
    return NULL;
  }

  // initialisation des variables xml:
  TiXmlNode* node = 0;
  TiXmlElement* dbElt = 0;
  TiXmlElement* projElt = 0;
  TiXmlElement* defdElt = 0;
  TiXmlElement* compdElt = 0;
  TiXmlElement* compElt = 0;
  TiXmlElement* pmElt {nullptr};

  // Element atelerB: version
  node = doc.FirstChild("db_xml");
  if (node == 0){
    toplevel_error(CAN_CONTINUE,
		   get_error_msg(E_NO_SUCH_FILE),
		   file_name,
		   strerror(errno));
    return NULL;
  }

  dbElt = node->ToElement();
  TRACE1("fichier XML pour version de l'atelierB: %s", dbElt->FirstChildElement("atelierb")->Attribute("version"));

  // Element project
  projElt = dbElt->FirstChildElement("project");
  if (projElt == 0){
    toplevel_error(CAN_CONTINUE,
		   get_error_msg(E_NO_SUCH_FILE),
		   file_name,
		   strerror(errno));
    return NULL;
  }

  // [old]decomp dir
  projet->set_decomp_dir(projElt->Attribute("decomp_dir"));
  TRACE1("decomposition directory: %s", projElt->Attribute("decomp_dir"));
  projet->set_old_tools_decomp_dir(projElt->Attribute("old_decomp_dir"));
  TRACE1("old decomposition  directory: %s", projElt->Attribute("old_decomp_dir"));

  if (projElt->Attribute("type") == 0)
	  projet->set_project_type(1);
  else if (strcmp(projElt->Attribute("type"), "VALIDATION") == 0)
  {
          projet->set_project_type(2);
  }
  else	  projet->set_project_type(0);

  // Element proof_mechanism
  for(pmElt = projElt->FirstChildElement("proof_mechanism");
      pmElt != nullptr;
      pmElt = pmElt->NextSiblingElement("proof_mechanism"))
    {
      TRACE1("proof mechanism: %s", pmElt->Attribute("name"));
      projet->add_proof_mechanism(pmElt->Attribute("name"));
    }


  // Element definition_dir
  for(defdElt = projElt->FirstChildElement("definition_dir");
      defdElt;
      defdElt = defdElt->NextSiblingElement("definition_dir"))
    {
      TRACE1("definition directory: %s", defdElt->Attribute("path"));
      projet->add_source_path_project(defdElt->Attribute("path"));
    }


  // Element component file
  T_file_component *new_file;
  T_component *new_comp;
  for(compdElt = projElt->FirstChildElement("component_file");
      compdElt != NULL;
      compdElt = compdElt->NextSiblingElement())
    {

      // creation d'un nouveau fichier
      new_file = new T_file_component(compdElt->Attribute("name"),
				      compdElt->Attribute("suffix"),
				      compdElt->Attribute("path"),
				      compdElt->Attribute("user"),
				      NULL, NULL, projet);
      TRACE4("file: %s, suffix: %s, path: %s, user: %s",
	     compdElt->Attribute("name"),
	     compdElt->Attribute("suffix"),
	     compdElt->Attribute("path"),
	     compdElt->Attribute("user"));

      const char *generator = compdElt->Attribute("generated_from");
      if(generator)
      {
    	  new_file->set_generated_from_component(lookup_symbol(generator));
      }
      // ajout du fichier a la liste
      projet->add_file_to_list(new_file);

      // Element component
      for(compElt = compdElt->FirstChildElement("component");
	  compElt;
	  compElt = compElt->NextSiblingElement("component"))
	{
	  // Type du composant
	  T_machine_type typeB;
	  if (strcmp(compElt->Attribute("type"), "specification") == 0)
	    typeB = MTYPE_SPECIFICATION;
	  else if (strcmp(compElt->Attribute("type"), "system") == 0)
	    typeB = MTYPE_SYSTEM;
	  else if (strcmp(compElt->Attribute("type"), "refinement") == 0)
	    typeB = MTYPE_REFINEMENT;
	  else if (strcmp(compElt->Attribute("type"), "implementation") == 0)
	    typeB = MTYPE_IMPLEMENTATION;
	  else {
	    fprintf(stderr, "Suffix inconnu: %s\n", compElt->Attribute("type"));
	    ASSERT(0);
	  }

	  // composant:
	  new_comp = new T_component(lookup_symbol(compElt->Attribute("name")),
				     typeB,
				     M_LOCAL,
				     new_file,
				     NULL,
				     NULL,
				     NULL,
				     projet);
	  TRACE2("composant: %s, type: %s", compElt->Attribute("name"), compElt->Attribute("type"));

	  // ajout du composant au fichier
	  new_file->add_component(new_comp);
	  // ajout du composant à la liste
	  projet->add_component_to_list(new_comp);
	}
    }

  int l_file_db = strlen(file_name);
  int l_path;
  for (l_path = l_file_db; l_path > 0 && file_name[l_path] != '/'; l_path--);

  char * path = (char *)s_malloc(l_path + 1);
  strncpy (path, file_name, (size_t)l_path);
  path[l_path] = '\0';

  projet->set_bdp_dir(path);

  s_free(path);

  return projet;

}

int T_project::add_proof_mechanism(const char *proof_mechanism)
{
  ASSERT(proof_mechanism != nullptr);
  TRACE2("T_project(%x)::add_proof_mechanism(%s)", this, proof_mechanism);
  T_symbol *symb = lookup_symbol(proof_mechanism);
  int found = 0;
  T_list_link *cur {first_proof_mechanism};
  T_symbol *cur_symb {nullptr};
  while ( !found && cur != nullptr )
    {
      cur_symb = dynamic_cast<T_symbol*>(cur->get_object());
      ASSERT(cur_symb != nullptr);
      TRACE1("cur_symb = %s", cur_symb->get_value());
      if (symb->compare(cur_symb) == TRUE)
          found = 1;
      else
          cur = dynamic_cast<T_list_link*>(cur->get_next());
    }
  if (found)
    {
      // message d'erreur
      user_error(CAN_CONTINUE,
                 get_user_error_msg(U_ADD_PROOF_MECHANISM),
                 proof_mechanism);
      return TRUE;
    }
  else
    {
      // chainage en queue
      (void)new T_list_link(symb, LINK_OTHER,
                            reinterpret_cast<T_item **>(&first_proof_mechanism),
                            reinterpret_cast<T_item **>(&last_proof_mechanism),
                            dynamic_cast<T_item*>(this),
                            nullptr, nullptr);

      // marquage pour liberation memoire
      // il ne faut pas liberer l'objet associe car c'est un T_symbol ...
      // new_link->set_do_free();
      return FALSE;
    }
}


// suppression d'un mécanisme de preuve
int T_project::remove_proof_mechanism(const char *proof_mechanism)
{
  ASSERT(proof_mechanism != nullptr);
  TRACE2("T_project(%x)::remove_proof_mechanism(%s)", this, proof_mechanism);
  int found {0};
  T_list_link *cur {first_proof_mechanism};
  T_symbol *cur_symb {nullptr};
  while ( !found && cur != nullptr )
    {
      cur_symb = dynamic_cast<T_symbol *>(cur->get_object());
      ASSERT(cur_symb != nullptr);
      if (strcmp(proof_mechanism, cur_symb->get_value()) == 0)
          found = 1;
      else
          cur = dynamic_cast<T_list_link *>(cur->get_next());
    }
  if (!found)
    {
      // message d'erreur
      user_error(CAN_CONTINUE,
                 get_user_error_msg(U_REMOVE_PROOF_MECHANISM),
                 proof_mechanism);
      return TRUE;
    }
  else
    {
      // suppression de l'element
      cur->remove_from_list(reinterpret_cast<T_item**>(&first_proof_mechanism),
                            reinterpret_cast<T_item**>(&last_proof_mechanism));
      return FALSE;
    }
}

// Recherche d'un composant dans la liste du projet
bool T_project::has_proof_mechanism(const char *name) const
{
  TRACE2("T_project(%x):has_proof_mechanism(%s)",
         this, name);

  T_list_link *itr {first_proof_mechanism};

  while (itr != nullptr) {
      T_symbol* symb{dynamic_cast<T_symbol*>(itr->get_object())};
      if (strcmp(name, symb->get_value()) == 0)
          return true;
      itr = dynamic_cast<T_list_link *>(itr->get_next());
  }
  return false;
}

