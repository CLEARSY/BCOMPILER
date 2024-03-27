/******************************* CLEARSY **************************************
* Fichier : $Id: m_api.cpp,v 2.0 2007-09-17 09:05:39 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*					Interface pour les applications utilisatrices
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
RCS_ID("$Id: m_api.cpp,v 1.54 2007-09-17 09:05:39 arequet Exp $") ;

/* Includes systeme */

/* Includes bibliotheques */
#include "c_api.h"

/* Includes Composant Local */
#include "m_compo.h"
#include "m_file.h"
#include "m_projec.h"
#include "m_pmana.h"
#include "m_msg.h"
#include "m_api.h"

#ifdef ACTION_NG

#include "m_comsta.h"
#include "m_synsta.h"

#endif /* ACTION_NG */

// Variables globales
// manager des projets
static T_project_manager *manager_sop = NULL;


// Flag d'initialisation
static int init_flag = FALSE;

// lecture du project manager
T_project_manager *get_project_manager(void)
{
  // SL 27/04/1999 : suprresion de ASSERT(manager_sop != NULL), car
  // sinon comment faire pour savoir s'il y a un project_manager ???
  return manager_sop;
}

void init_project_manager(void)
{
  if (init_flag == FALSE)
	{
          init_compiler();
	  init_decompiler();
	  init_flag = TRUE;
	  m_init_messages();
	}
  if (manager_sop == NULL)
	{
	  manager_sop = new T_project_manager();
	}
}

// Initialisation
EXTERN void m_init(void)
{
  if (init_flag == FALSE)
	{
	  init_compiler();
	  set_no_exit_mode(TRUE); // pour ne pas avoir d'exit du compilateur
	  set_kernel_joker_is_an_error(TRUE); // pour les identificateurs a une lettre
	  set_proof_clash_detection(TRUE); // pour controler les conflits caches
	  set_fix_inherited_lexems_mode(TRUE); // ameliorer la gestion des lexemes
	  //compiler_disable_warnings(); // pour enlever les warnings
	  compiler_enable_extended_warnings() ;//autorise warnings etendus
	  set_msg_destination(MSG_DEST_BUFFERS); // messages bufferises
	  set_strict_B_demanded(); // pour les ; qui genent stephane
	  init_decompiler();
	  init_flag = TRUE;
	  m_init_messages();
	  set_file_fetch_mode(FFMODE_LOCAL);
	  // pour rechercher les fichiers au bon endroit

	  // fonctions virtuelles permettant de parcourir le corps d'une operation
	  // et de creer la liste des operations appelees (cas de l'Atelier B
	  // differentiel.
	  reset_user_defined_switches();
	  set_user_defined_f5(NODE_SKIP,
						  (user_defined_5)add_called_op_skip);
	  set_user_defined_f5(NODE_BEGIN,
						  (user_defined_5)add_called_op_begin);
	  set_user_defined_f5(NODE_CHOICE,
						  (user_defined_5)add_called_op_choice);
	  set_user_defined_f5(NODE_PRECOND,
						  (user_defined_5)add_called_op_precond);
          set_user_defined_f5(NODE_ASSERT,
                                                  (user_defined_5)add_called_op_assert);
          set_user_defined_f5(NODE_LABEL,
                                                  (user_defined_5)add_called_op_label);
          set_user_defined_f5(NODE_JUMPIF,
                                                  (user_defined_5)add_called_op_jumpif);
          set_user_defined_f5(NODE_WITNESS,
						  (user_defined_5)add_called_op_witness);
	  set_user_defined_f5(NODE_IF,
						  (user_defined_5)add_called_op_if);
	  set_user_defined_f5(NODE_SELECT,
						  (user_defined_5)add_called_op_select);
	  set_user_defined_f5(NODE_CASE,
						  (user_defined_5)add_called_op_case);
	  set_user_defined_f5(NODE_LET,
						  (user_defined_5)add_called_op_let);
	  set_user_defined_f5(NODE_VAR,
						  (user_defined_5)add_called_op_var);
	  set_user_defined_f5(NODE_WHILE,
						  (user_defined_5)add_called_op_while);
	  set_user_defined_f5(NODE_ANY,
						  (user_defined_5)add_called_op_any);
	  set_user_defined_f5(NODE_AFFECT,
						  (user_defined_5)add_called_op_skip);
	  set_user_defined_f5(NODE_BECOMES_MEMBER_OF,
						  (user_defined_5)add_called_op_skip);
	  set_user_defined_f5(NODE_BECOMES_SUCH_THAT,
						  (user_defined_5)add_called_op_skip);
	  set_user_defined_f5(NODE_OP_CALL,
						  (user_defined_5)add_called_op_opcall);
	  set_user_defined_f5(NODE_SUBSTITUTION,
						  (user_defined_5)add_called_op_substitution);
	}
  if (manager_sop == NULL)
	{
	  manager_sop = new T_project_manager();
	}
}

EXTERN void m_unlink(void)
{
  if (init_flag == TRUE)
    {
      unlink_compiler();
      init_flag = FALSE;
    }
}

// Fonctions pour la gestion des messages d'erreur et d'avertissement
static T_msg_line *current_message = NULL;
static char *value_current_message = NULL;
static char *m_set_current_message(T_msg_line *cur_line)
{
  TRACE1("m_set_current_message(%x)", cur_line);
  int len;

  if (value_current_message != NULL)
	{
	  s_free(value_current_message);
	}
  len = 0;

  TRACE1("cur_line %x", cur_line) ;
  TRACE1("cur_line %s", cur_line->get_class_name()) ;
  TRACE1("cur_line->fname %x", cur_line->get_file_name()) ;
  if (cur_line->get_file_name() != NULL)
	{
	  len += cur_line->get_file_name()->get_len() +
		10 + 10 ; // 9 caracteres pour chaque nombre (ligne, colonne) + "::"
	}
  len += cur_line->get_contents()->get_len();
  len += 2; // "\n" et "\0"
  value_current_message = (char *)s_malloc(len);
  value_current_message[0] = 0;
  if (cur_line->get_file_name() != NULL)
	{
	  sprintf(value_current_message, "%s:%d:%d ",
			  cur_line->get_file_name()->get_value(),
			  cur_line->get_file_line(),
			  cur_line->get_file_column());
	}
  strcat(value_current_message, cur_line->get_contents()->get_value());
  return value_current_message;
}
EXTERN char *m_get_first_message(void)
{
  TRACE0("m_get_first_message");

  current_message = get_msg_line_manager()->get_messages() ;
  if (current_message == NULL)
	{
	  TRACE0("pas de message");
	  return NULL;
	}
  return m_set_current_message(current_message);
}
EXTERN char *m_get_next_message(void)
{
  ASSERT(current_message != NULL);
  current_message = (T_msg_line *)current_message->get_next();
  if (current_message == NULL) return NULL;
  return m_set_current_message(current_message);
}
EXTERN void m_reset_messages(void)
{
  TRACE0("m_reset_messages");
  get_msg_line_manager()->unlink_lines() ;
}

// Fonction pour la gestion des repertoire d'include de definition
static T_list_link *current_source_path;
EXTERN const char *m_get_first_source_path(void)
{
  TRACE0("m_get_first_source_path()");
  current_source_path = manager_sop->get_current()->get_sources_path();
  if (current_source_path == NULL)
	{
	  TRACE0("m_get_first_source_path --> NULL");
	  return NULL;
	}
  else
	{
	  T_symbol *sym = (T_symbol *)current_source_path->get_object();
	  TRACE1("m_get_first_source_path --> %s", sym->get_value());
	  return(sym->get_value());
	}
}
EXTERN const char *m_get_next_source_path(void)
{
  ASSERT(current_source_path != NULL);
  TRACE0("m_get_next_source_path()");
  current_source_path = (T_list_link *)current_source_path->get_next();
  if (current_source_path == NULL)
	{
	  TRACE0("m_get_next_source_path --> NULL");
	  return NULL;
	}
  else
	{
	  T_symbol *sym = (T_symbol *)current_source_path->get_object();
	  TRACE1("m_get_next_source_path --> %s", sym->get_value());
	  return(sym->get_value());
	}
}
EXTERN int m_add_source_path(const char *new_path)
{
  TRACE1("m_add_source_path(%s)", new_path);
  return manager_sop->get_current()->add_source_path_project(new_path);
}
EXTERN int m_remove_source_path(const char *new_path)
{
  return manager_sop->get_current()->remove_source_path(new_path);
}

EXTERN T_project *m_get_current()
{
	return manager_sop->get_current();
}

/* Fonctions pour la compatibilite */
EXTERN T_component *m_get_library(const char *name)
{
  return manager_sop->search_library(name);
}
EXTERN T_component *m_get_component(const char *name)
{
  return manager_sop->get_current()->search_component(name);
}
EXTERN const char *m_component_name(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_name()->get_value();
}
EXTERN const char *m_component_suffix(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_file()->get_suffix()->get_value();
}
EXTERN const char *m_component_location(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_file()->get_path()->get_value();
}
EXTERN const char *m_component_user(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_file()->get_user()->get_value();
}
EXTERN const char *m_component_generated_from(T_component *compo)
{
	ASSERT(compo != NULL);
	T_symbol *symbol = compo->get_file()->get_generated_from_component();
	if(symbol)
	{
		return symbol->get_value();
	}
	else
	{
		return NULL;
	}
}

EXTERN time_t m_component_date(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_file()->get_date();
}
static char *path_scp = NULL; // Variable statique pour stocker un chemin
EXTERN const char *m_component_path(T_component *compo)
{
  ASSERT(compo != NULL);
  int len;

  if (path_scp != NULL)
	{
	  s_free(path_scp);
	  path_scp = NULL;
	}

  len = compo->get_file()->get_path()->get_len() +
	compo->get_file()->get_name()->get_len() +
	compo->get_file()->get_suffix()->get_len() + 3; /* "/." et \0 */
  path_scp = (char *)s_malloc(len);
  sprintf(path_scp, "%s/%s.%s",
		  compo->get_file()->get_path()->get_value(),
		  compo->get_file()->get_name()->get_value(),
		  compo->get_file()->get_suffix()->get_value());

  TRACE1("<< m_component_path(%s)", path_scp) ;
  return path_scp;
}
EXTERN const char *m_component_type_string(T_component *compo)
{
  ASSERT(compo != NULL);
  return get_machine_type_name(compo->get_machine_type()) ;
}

EXTERN int m_component_index(T_component *compo)
{
  ASSERT(compo != NULL);
  return compo->get_index();
}
EXTERN int m_component_is_missing(T_component *compo)
{
  ASSERT(compo != NULL);
  if (compo->get_type() != M_MISSING)
	{
	  TRACE0("m_component_is_missing -> FALSE");
	  return FALSE;
	}
  else
	{
	  TRACE0("m_component_is_missing -> TRUE");
	  return TRUE;
	}
}
EXTERN int m_component_belongs_to_library(T_component *compo)
{
  ASSERT(compo != NULL);
  if (compo->get_type() != M_LIBRARY)
	return FALSE;
  else
	return TRUE;
}
EXTERN int m_component_is_an_implementation(T_component *compo)
{
  ASSERT(compo != NULL);
  if (compo->get_machine_type() != MTYPE_IMPLEMENTATION)
	{
	  return FALSE;
	}
  else
	{
	  return TRUE;
	}
}
EXTERN int m_component_is_a_specification(T_component *compo)
{
  ASSERT(compo != NULL);
  if (compo->get_machine_type() != MTYPE_SPECIFICATION &&
		  compo->get_machine_type() != MTYPE_SYSTEM)
	{
	  return FALSE;
	}
  else
	{
	  return TRUE;
	}
}
EXTERN CONST char *m_component_extension(T_component *compo)
{
  ASSERT(compo != NULL);
  return get_machine_suffix_name(compo->get_machine_type()) ;
}
EXTERN void m_component_reset_checksum(T_component *compo)
{
  ASSERT(compo != NULL);
  compo->set_checksum(NULL);
}
EXTERN int m_component_has_checksum(T_component *compo)
{
  ASSERT(compo != NULL);
  if (compo->get_checksum() != NULL)
	{
	  TRACE0("m_component_has_checksum -> TRUE");
	  return TRUE;
	}
  TRACE0("m_component_has_checksum -> FALSE");
  return FALSE;
}

// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs et warnings eventuels trouves par le bcomp
// et les afficher
EXTERN int m_component_expand(T_component *compo, const char* converterName)
{
  ASSERT(compo != NULL);
  return compo->expand(converterName);
}

// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs et warnings eventuels trouves par le bcomp
// et les afficher
EXTERN int m_component_cut(T_component *compo, const char* converterName)
{
  ASSERT(compo != NULL);
  return compo->cut(converterName);
}

EXTERN int m_component_is_eventB(T_component *compo)
{
	ASSERT(compo != NULL);
    T_project *prj = compo->getProject();
    if(prj)
    {
        return prj->isEventB();
    }
    else
    {
        return T_project::get_default_project_type() == PROJECT_SYSTEM;
    }
}

EXTERN int m_component_is_validation(T_component *compo)
{
	ASSERT(compo != NULL);
    T_project *prj = compo->getProject();
    if(prj)
    {
        return prj->isValidation();
    }
    else
    {
        return T_project::get_default_project_type() == PROJECT_VALIDATION;
    }
}

EXTERN int m_project_is_eventB(T_project *pro)
{
	ASSERT(pro != NULL);
	return pro->isEventB();
}

EXTERN int m_project_is_validation(T_project *pro)
{
        ASSERT(pro != NULL);
        return pro->isValidation();
}

/* creation d'un fichier vide de description de composants */
EXTERN int m_create_empty_project(const char *dir, const char *name,
										   const char *subdir,
										   const char *subdir_oldtools,
										   const int project_type)
{
  TRACE4("m_create_empty_project(%s,%s,%s,%s)", dir, name, subdir, subdir_oldtools);
  T_project *new_project;
  char *path;

  // creation d'un nouveau projet
  new_project = new T_project(NULL, NULL, NULL);

  // modification des sous-repertoires
  new_project->set_decomp_dir(dir, subdir);
  new_project->set_old_tools_decomp_dir(dir, subdir_oldtools);

  // modification du type du projet
  new_project->set_project_type(project_type);

  // modification du timestamp
  new_project->set_timestamp(get_timestamp());

  // sauvegarde du nouveau projet
  path = new char[strlen(dir) + strlen(name) + 5]; // "/.db" + \0
  sprintf(path, "%s/%s.db", dir, name);
  T_io_diagnostic io_diagnostic = new_project->save_project_xml(path);
  delete [] path;

  // destruction du projet
  new_project->unlink();

  // cas d'erreur
  if (io_diagnostic != IO_OK)
	{
	  return TRUE;
	}

  return FALSE;
}

/* chargement des machines du projet courant */
EXTERN int m_load_current_project(const char *dir, const char *name)
{
  TRACE2("m_load_current_project(%s, %s)", dir, name);
  return manager_sop->load_current_project(dir, name);
}
EXTERN int m_reload_current_project(void)
{
  TRACE0("m_reload_current_project");
  int res = manager_sop->reload_current_project();
  // manager_sop->get_current()->debug_print();
  return res;
}
EXTERN void m_remove_current_project(void)
{
  manager_sop->remove_current_project();
}
EXTERN void m_unload_current_project(void)
{
  manager_sop->unload_current_project();
}

/* modification du repertoire de decompilation (anciens outils) */
EXTERN void m_set_old_tools_decomp_dir(const char *dir, const char *subdir)
{
  manager_sop->get_current()->set_old_tools_decomp_dir(dir, subdir);
}

/* modification du repertoire de decompilation normale */
EXTERN void m_set_decomp_dir(const char *dir, const char *subdir)
{
  manager_sop->get_current()->set_decomp_dir(dir, subdir);
}

/* sauvegarde sur disque des machines du projet courant */
EXTERN int m_save_current_project()
{
  TRACE0("m_save_current_project");
  TRACE1("manager_sop = %x", manager_sop);
  manager_sop->unlink_machines();
  return  manager_sop->save_current_project();
}


/* Est-ce qu'un fichier peut-etre ajoute au projet courant ? */
EXTERN int m_file_can_be_added(const char *filename)
{
  T_component *compo;
  char *basename = new char[strlen(filename) + 1];
  strcpy(basename, filename);
  char *p = strrchr(basename, '.');
  if (p == NULL)
	basename[strlen(filename) - 1] = 0;
  else
	*p = 0;
  compo = manager_sop->get_current()->search_component(basename);
  delete [] basename;
  if (compo == NULL)
	return TRUE;
  else
	return FALSE;
}

EXTERN void m_remove_generated_files(CONST char *generated_tag)
{
  manager_sop->get_current()->remove_generated_files(generated_tag);
}

/* ajout d'un fichier au projet courant */
EXTERN int m_add_file(const char *filename,
                               const char *converterName,
			       const char *dir,
			       const char *user,
			       const char *generator)
{
  return(manager_sop->get_current()->add_file(filename, converterName, dir, user, generator));
}

/* suppression d'un fichier au projet courant */
EXTERN void m_remove_file(const char *filename)
{
  manager_sop->get_current()->remove_file(filename);
}

/* caracteristiques d'un composant */
EXTERN int m_is_a_specification(const char *name)
{
  T_component *compo;
  compo = manager_sop->get_current()->search_component(name);
  ASSERT(compo != NULL);
  if (compo->get_machine_type() == MTYPE_SPECIFICATION ||
		  compo->get_machine_type() == MTYPE_SYSTEM)
	{
	return TRUE;
	}
  else
	{
	return FALSE;
	}
}

EXTERN int m_is_a_refinement(const char *name)
{
  T_component *compo;
  compo = manager_sop->get_current()->search_component(name);
  ASSERT(compo != NULL);
  if (compo->get_machine_type() == MTYPE_REFINEMENT)
	{
	return TRUE;
	}
  else
	{
	return FALSE;
	}
}

EXTERN int m_is_an_implementation(const char *name)
{
  T_component *compo;
  compo = manager_sop->get_current()->search_component(name);
  ASSERT(compo != NULL);
  if (compo->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	return TRUE;
	}
  else
	{
	return FALSE;
	}
}

EXTERN const char *m_get_component_user(const char *name)
{
  TRACE1("m_get_component_user(%s)", name);
  T_component *compo;
  compo = manager_sop->get_current()->search_component(name);
  ASSERT(compo != NULL);
  return(compo->get_file()->get_user()->get_value());
}

/* teste si un user est proprietaire d'un composant */
EXTERN int m_is_component_user(const char *username)
{
  return manager_sop->m_is_component_user(username);
}

/* teste si il existe un autre user que celui donne en parametre */
EXTERN int m_is_unique_component_user(const char *username)
{
  return manager_sop->m_is_unique_component_user(username);
}

/* teste si un composant appartient a un fichier multi-composants */
EXTERN int m_belongs_to_a_multi_components_file(const char *compname)
{
  T_component *comp = manager_sop->get_current()->search_component(compname);
  if (comp == NULL)
	{
	  /* cas ou compname a �t� supprim� d'un fichier multi-composant */
	  return FALSE;
	}
  TRACE1(">> m_belongs_to_a_multi_components_file(%d)",
		 comp->get_file()->is_a_multi_components_file()) ;
  return comp->get_file()->is_a_multi_components_file();
}

EXTERN const char *m_get_component_filename(const char *compname)
{
  T_component *comp = manager_sop->get_current()->search_component(compname);
  ASSERT(comp != NULL);
  return comp->get_file()->get_name()->get_value();
}

#ifdef ACTION_NG
EXTERN int m_component_belongs_to_file(const char *compname,
												const char *filename)
{
  T_component *comp = manager_sop->get_current()->search_component(compname);
  ASSERT(comp != NULL);
  T_symbol *symb_filename = lookup_symbol(filename) ;
  return symb_filename->compare(comp->get_file()->get_name());
}
#endif /* ACTION_NG */

/* caracteristiques d'un fichier */
EXTERN int m_file_is_a_multi_components(T_file_component *file_ptr)
{
  return(file_ptr->is_a_multi_components_file());
}
EXTERN int m_is_a_multi_components_file(const char *filename)
{
  T_file_component *file = manager_sop->get_current()->search_file(filename);
  ASSERT(file != NULL);
  return file->is_a_multi_components_file();
}

EXTERN const char *m_file_name(T_file_component *file_ptr)
{
  return file_ptr->get_name()->get_value();
}

EXTERN const char *m_file_location(T_file_component *file_ptr)
{
  return file_ptr->get_path()->get_value();
}
EXTERN const char *m_file_suffix(T_file_component *file_ptr)
{
  return file_ptr->get_suffix()->get_value();
}

/* liste des fichiers du projet courant */

// Variable globale dernier fichier lu
static T_file_component *current_file = NULL;

// Premier de la liste
EXTERN T_file_component *m_get_first_file()
{
  T_file_component *file = manager_sop->get_current()->get_files();
  current_file = file;
  return file;
}

// Suivant dans la liste
EXTERN T_file_component *m_get_next_file()
{
  if (current_file == NULL)
	return NULL;
  else
	{
	  current_file = (T_file_component *)current_file->get_next();
	  return current_file;
	}
}


/* liste des composants d'un fichier */

// Variables globales : fichier courant et composant courant
static T_list_link *current_component_of_current_file = NULL;

// Premier de la liste
EXTERN const char *m_get_first_file_component(const char *filename)
{
  TRACE1("m_get_first_file_component(%s)", filename);
  T_component *compo;
  T_file_component *file = manager_sop->get_current()->search_file(filename);
  if (file == NULL)
	{
	  current_component_of_current_file = NULL;
	  return NULL;
	}
  else
	{
	  current_component_of_current_file = file->get_components();
	  ASSERT(current_component_of_current_file != NULL);
	  compo = (T_component *)current_component_of_current_file->get_object();
	  return compo->get_name()->get_value();
	}
}

// Suivant dans la liste
EXTERN const char *m_get_next_file_component()
{
  TRACE0("m_get_next_file_component()");
  if (current_component_of_current_file == NULL)
	return NULL;

  T_component *compo;
  current_component_of_current_file =
	(T_list_link *)current_component_of_current_file->get_next();
  if (current_component_of_current_file == NULL)
	return NULL;
  else
	{
	  compo = (T_component *)current_component_of_current_file->get_object();
	  return compo->get_name()->get_value();
	}
}

/* liste des composants du projet courant */

// Variable globale dernier composant lu
static T_component *current_component = NULL;

// Premier de la liste
EXTERN const char *m_get_first_component_name()
{
  TRACE0("m_get_first_component_name()");
  T_component *compo = manager_sop->get_current()->get_components();
  current_component = compo;
  if (compo != NULL)
	return compo->get_name()->get_value();
  else
	return NULL;
}
EXTERN T_component *m_get_first_component()
{
  TRACE0("m_get_first_component()");
  T_component *compo = manager_sop->get_current()->get_components();
  current_component = compo;
  return compo;
}

// Suivant dans la liste
EXTERN const char *m_get_next_component_name()
{
  TRACE0("m_get_next_component_name()");
  if (current_component == NULL)
	return NULL;
  else
	{
	  current_component = (T_component *)current_component->get_next();
	  if (current_component == NULL)
		return NULL;
	  else
		return current_component->get_name()->get_value();
	}
}
EXTERN T_component *m_get_next_component()
{
  TRACE0("m_get_next_component()");
  if (current_component == NULL)
	return NULL;
  else
	{
	  current_component = (T_component *)current_component->get_next();
	  return current_component;
	}
}


/* composants associes aux bibliotheques */
EXTERN T_project *m_load_library(const char *dir, const char *name, int index)
{
  return manager_sop->load_library(dir, name, index);
}
EXTERN void m_unlink_libraries(void)
{
  manager_sop->unlink_libraries();
}
// Variable globale dernier composant de bibliotheque lu
static T_component *current_library_component = NULL;
static T_project *current_library = NULL;
EXTERN CONST  char*m_get_current_library_bdp()
{
	return current_library->get_bdp_dir()->get_value();
}

EXTERN T_component *m_get_first_library_component()
{
  current_library = manager_sop->get_libraries();
  if (current_library == NULL)
	{
	  return NULL;
	}
  else
	{
	  current_library_component = current_library->get_components();
	  // on prend la premiere bibliotheque avec des composants
	  while ( (current_library_component == NULL) && (current_library != NULL) )
		{
		  current_library = (T_project *)current_library->get_next();
		  if (current_library != NULL)
			{
			  current_library_component = current_library->get_components();
			}
		}
	  return current_library_component;
	}
}
EXTERN T_component *m_get_next_library_component()
{
  if (current_library == NULL)
	{
	  return NULL;
	}
  else
	{
	  ASSERT(current_library_component != NULL);
	  current_library_component = (T_component *)current_library_component->get_next();
	  if (current_library_component == NULL)
		{
		  // on prend la bibliotheque suivante avec des composants
		  while ( (current_library_component == NULL) && (current_library != NULL) )
			{
			  current_library = (T_project *)current_library->get_next();
			  if (current_library != NULL)
				{
				  current_library_component = current_library->get_components();
				}
			}
		}
	  return current_library_component;
	}
}

/* composants manquants */
EXTERN T_component *m_create_missing_component(const char *compname)
{
  return manager_sop->create_missing_component(compname);
}
EXTERN void m_unlink_missing_components(void)
{
  manager_sop->unlink_missing_components();
}

// Variable globale dernier composant  absent lu
static T_component *current_missing_component = NULL;
EXTERN T_component *m_get_first_missing_component()
{
  T_component *compo = manager_sop->get_missing();
  current_missing_component = compo;
  return compo;
}
EXTERN T_component *m_get_next_missing_component()
{
  if (current_missing_component == NULL)
	return NULL;
  else
	{
	  current_missing_component = (T_component *)current_missing_component->get_next();
	  return current_missing_component;
	}
}

/* fichiers de definition */

/* variable globale du dernier fichier de definition lu */
static T_list_link *current_file_def = NULL;

/* lecture du fichier de definition courant */
static T_file_def_type get_current_file_def(char **path, char **name)
{
  TRACE0("get_current_file_def_path");
  T_file_definition *file_def = NULL;
  T_symbol *path_symb = NULL;
  T_symbol *tmp_path = NULL;
  T_symbol *tmp_name = NULL;

  if (current_file_def != NULL)
	{
	  file_def = (T_file_definition *)current_file_def->get_object();

	  // recuperation du nom du fichier
	  tmp_name = file_def->get_name();
	  *name = (char *)s_malloc(tmp_name->get_len() + 1);
	  strcpy(*name, tmp_name->get_value());
	  TRACE1("Nom : %s", *name);

	  // Recuperation du chemin du fichier
	  path_symb = file_def->get_path();
	  if (path_symb == NULL)
		{
		  TRACE0("Cas d'un fichier local");
		  tmp_path = (T_symbol *)file_def->get_usr1();
		}
	  else
		{
		  TRACE0("Cas d'un fichier de librarie");
		  tmp_path = path_symb;
		}

	  *path = (char *)s_malloc(tmp_path->get_len() + 1);
	  strcpy(*path, tmp_path->get_value());
	  TRACE1("Path : %s", *path);

	  if (path_symb == NULL)
		{
		  TRACE1("get_current_file_def_path --> %d", DEF_LOCAL);
		  return DEF_LOCAL;
		}
	  else
		{
		  TRACE1("get_current_file_def_path --> %d", DEF_LIBRARY);
		  return DEF_LIBRARY;
		}
	}
  else
	{
	  TRACE1("get_current_file_def_path --> %d", DEF_UNDEFINED);
	  return DEF_UNDEFINED;
	}
}

/* Test si le composant utilise des fichiers de definition */
EXTERN int m_test_file_def(T_component *comp, const char* converterName) {
    return manager_sop->test_file_def(comp, converterName);
}

/* calcul de la liste  des fichiers de definition
   attention :  cette fonction charge les betrees
   il faut recuperer les erreurs eventuelles trouvees par le bcomp
   et les afficher
   retourne 0 si pas d'erreur rencontree lors du chargement des betrees
   1 si erreur */

EXTERN int m_set_file_def(const char* converterName)
{
	ASSERT(manager_sop != NULL);
	if (manager_sop->get_file_def() != NULL)
		{
			/* Les fichiers de definition ont deja ete calcule.
			   On les nettoie car ils ne sont pas forcement a jour */
			manager_sop->unlink_file_def();
		}

	/* Recuperation des fichiers de definitions dans le betree */
        return manager_sop->set_file_def(converterName);
}

/* lecture du premier fichier de definition */
EXTERN T_file_def_type m_get_first_file_def(char **path, char **name)
{
  TRACE0("m_get_first_file_def");

  ASSERT(manager_sop != NULL);

  /* Lecture du premier fichier */
  current_file_def = manager_sop->get_file_def();

  return get_current_file_def(path, name);
}


/* lecture du fichier de definition suivant*/
EXTERN T_file_def_type m_get_next_file_def(char **path, char **name)
{
  TRACE0("m_get_next_file_def");
  ASSERT(current_file_def != NULL);


  /* Lecture du fichier suivant*/
  current_file_def = (T_list_link *)current_file_def -> get_next();

  return get_current_file_def(path, name);
}






// gestion des dependances
// -----------------------
// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs eventuelles trouvees par le bcomp
// et les afficher
EXTERN const char *m_get_bdp()
{
  return get_project_manager()->get_current()->get_bdp_dir()->get_value();
}

EXTERN void unlink_removed_components (void)
{
  manager_sop->unlink_removed_components();
}

EXTERN void unlink_added_components (void)
{
  manager_sop->unlink_added_components();
}

EXTERN int m_component_check_dependencies(T_component *comp, const char* converterName)
{
  ASSERT(comp != NULL);
  manager_sop->unlink_removed_components();
  manager_sop->unlink_added_components();
  return comp->check_dependencies(converterName);
}

EXTERN int m_is_expand_dep_needed (T_component *comp)
{
  ASSERT(comp != NULL);
  return is_expand_dep_needed(m_component_path(comp));
}

// Composants supprimes d'un fichier multi-composants
T_list_link *current_removed = NULL;
EXTERN const char *m_get_first_removed_component(T_component *comp)
{
  ASSERT(comp != NULL);
  current_removed =  manager_sop->get_removed();
  if (current_removed == NULL)
	{
	  return NULL;
	}
  else
	{
	  T_component *elem = (T_component *)current_removed->get_object();
	  return elem->get_name()->get_value();
	}
}

EXTERN const char *m_get_next_removed_component(void)
{
  ASSERT(current_removed != NULL);
  current_removed = (T_list_link *)current_removed->get_next();
  if (current_removed == NULL)
	{
	  return NULL;
	}
  else
	{
	  T_component *elem = (T_component *)current_removed->get_object();
	  return elem->get_name()->get_value();
	}
}


// Composants ajoutes a un fichier multi-composants
T_list_link *current_added = NULL;
EXTERN const char *m_get_first_new_component(T_component *comp)
{
  ASSERT(comp != NULL);
  current_added =  manager_sop->get_added();
  if (current_added == NULL)
	{
	  return NULL;
	}
  else
	{
	  T_component *elem = (T_component *)current_added->get_object();
	  return elem->get_name()->get_value();
	}
}
EXTERN const char *m_get_next_new_component(void)
{
  ASSERT(current_added != NULL);
  current_added = (T_list_link *)current_added->get_next();
  if (current_added == NULL)
	{
	  return NULL;
	}
  else
	{
	  T_component *elem = (T_component *)current_added->get_object();
	  return elem->get_name()->get_value();
	}
}


// abstraction d'un composant
EXTERN const char *m_component_abstraction(T_component *comp)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  if (comp->get_machine()->get_abstraction_name() != NULL)
	{
	  return comp->get_machine()->get_abstraction_name()->get_name()->get_value();
	}
  else
	{
	  return NULL;
	}
}

// liste des machines incluses
T_used_machine *current_includes = NULL;
EXTERN const char *m_get_first_includes(T_component *comp, const char **name, const char **rename)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  current_includes = comp->get_machine()->get_includes();
  if (current_includes == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_includes->get_component_name()->get_value();
	  if (current_includes->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_includes->get_instance_name()->get_value();
		}
	  return *name;
	}
}
EXTERN const char *m_get_next_includes(const char **name, const char **rename)
{
  ASSERT(current_includes != NULL);
  current_includes = (T_used_machine *)current_includes->get_next();
  if (current_includes == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_includes->get_component_name()->get_value();
	  if (current_includes->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_includes->get_instance_name()->get_value();
		}
	  return *name;
	}
}


// liste des machines vues
T_used_machine *current_sees = NULL;
EXTERN const char *m_get_first_sees(T_component *comp, const char **name, const char **rename)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  current_sees = comp->get_machine()->get_sees();
  if (current_sees == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_sees->get_component_name()->get_value();
	  if (current_sees->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_sees->get_instance_name()->get_value();
		}
	  return *name;
	}
}
EXTERN const char *m_get_next_sees(const char **name, const char **rename)
{
  ASSERT(current_sees != NULL);
  current_sees = (T_used_machine *)current_sees->get_next();
  if (current_sees == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_sees->get_component_name()->get_value();
	  if (current_sees->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_sees->get_instance_name()->get_value();
		}
	  return *name;
	}
}

// liste des machines importees
T_used_machine *current_imports = NULL;
EXTERN const char *m_get_first_imports(T_component *comp, const char **name, const char **rename)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  current_imports = comp->get_machine()->get_imports();
  if (current_imports == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_imports->get_component_name()->get_value();
	  if (current_imports->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_imports->get_instance_name()->get_value();
		}
	  return *name;
	}
}
EXTERN const char *m_get_next_imports(const char **name, const char **rename)
{
  ASSERT(current_imports != NULL);
  current_imports = (T_used_machine *)current_imports->get_next();
  if (current_imports == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_imports->get_component_name()->get_value();
	  if (current_imports->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_imports->get_instance_name()->get_value();
		}
	  return *name;
	}
}


// liste des machines etendues
T_used_machine *current_extends = NULL;
EXTERN const char *m_get_first_extends(T_component *comp, const char **name, const char **rename)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  current_extends = comp->get_machine()->get_extends();
  if (current_extends == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_extends->get_component_name()->get_value();
	  if (current_extends->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_extends->get_instance_name()->get_value();
		}
	  return *name;
	}
}
EXTERN const char *m_get_next_extends(const char **name, const char **rename)
{
  ASSERT(current_extends != NULL);
  current_extends = (T_used_machine *)current_extends->get_next();
  if (current_extends == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_extends->get_component_name()->get_value();
	  if (current_extends->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_extends->get_instance_name()->get_value();
		}
	  return *name;
	}
}


// liste des machines uses
T_used_machine *current_uses = NULL;
EXTERN const char *m_get_first_uses(T_component *comp, const char **name, const char **rename)
{
  ASSERT(comp != NULL);
  ASSERT(comp->get_machine() != NULL);
  current_uses = comp->get_machine()->get_uses();
  if (current_uses == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_uses->get_component_name()->get_value();
	  if (current_uses->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_uses->get_instance_name()->get_value();
		}
	  return *name;
	}
}
EXTERN const char *m_get_next_uses(const char **name, const char **rename)
{
  ASSERT(current_uses != NULL);
  current_uses = (T_used_machine *)current_uses->get_next();
  if (current_uses == NULL)
	{
	  return NULL;
	}
  else
	{
	  *name = current_uses->get_component_name()->get_value();
	  if (current_uses->get_instance_name() == NULL)
		{
		  *rename = NULL;
		}
	  else
		{
		  *rename = current_uses->get_instance_name()->get_value();
		}
	  return *name;
	}
}

// flag de sauvegarde du projet courant
EXTERN int m_get_save_flag(void)
{
  TRACE0("m_get_save_flag");
  return manager_sop->get_save_flag();
}


// flag de modification du type B d'un composant
EXTERN int m_get_modified_flag(void)
{
  return manager_sop->get_modified_flag();
}

// copie des checksums d'un vieux projet dans un nouveau projet
// retourne TRUE si ca s'est bien pass�, et FALSE sinon
EXTERN int m_copy_checksums(CONST char *bdp,
							CONST char *new_prj_name,
							CONST char *old_prj_name)
{
  TRACE3("m_copy_checksums(%s, %s, %s)",
		 bdp, new_prj_name, old_prj_name);

  int res1 = FALSE;
  int res2 = FALSE;

  if (m_load_current_project(bdp, new_prj_name) != 0)
	{
	  TRACE0("m_copy_checksums --> FALSE");
	  return FALSE;
	}

  res1 = manager_sop->copy_checksums(bdp, old_prj_name);

  res2 = m_save_current_project();

  m_unload_current_project();

  if ((res1 == FALSE) || (res2 != 0))
	{
  TRACE0("m_copy_checksums --> FALSE");
	  return FALSE;
	}
  TRACE0("m_copy_checksums --> TRUE");
  return TRUE;
}


//
// ATELIERB DIFFERENTIEL
//
static T_operation_checksums *current_modified_op = NULL;

/* rend 1 si le contexte du composant a ete modifie 0 sinon et -1 si erreur */
EXTERN int m_is_component_context_modified(T_component *comp, char *state_file)
{
  TRACE1(">> m_component_context_modified(%x)", comp);

  // Resultat
  int res = FALSE;

  // On regarde si le contexte du composant a ete modifie
  res = comp->is_context_modified(state_file);

  TRACE1("<< m_component_context_modified(%d)", res);
  return res;
}

// met � jour l'etat (i.e. la liste des checksums) du composant name
// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs eventuelles trouvees par le bcomp
// et les afficher
EXTERN int m_set_component_checksums(T_component *comp, const char* converterName)
{
  TRACE1("m_set_component_checksums(%x)", comp);
  return comp->set_component_checksums(converterName);
}

// Cree la liste des operations modifiees
EXTERN int m_set_modified_op_list(T_component *comp,
								  char *state_file)
{
  TRACE1("m_set_modified_op_list(%x)", comp);

  // Resultat
  int res = 0;

  // On cree la liste
  res = comp->set_modified_op_list(state_file);

  // On recupere la liste
  if (res == 0)
	{
	  current_modified_op = comp->get_component_checksums()->
		get_operations_checksums();
	}

  return res;
}

// Retourne le premier element de la liste des operations modifiees
EXTERN const char *m_get_first_modified_op()
{
  TRACE0("m_get_first_modified_op()");

  // On saute celles qui n'ont pas ete modifiees
  while( (current_modified_op != NULL) &&
		(current_modified_op->is_modified() == FALSE) )
	{
	  current_modified_op = (T_operation_checksums *)current_modified_op
		->get_next();
	}

  if ( current_modified_op == NULL )
	{
	  TRACE0("m_get_first_modified_op->NULL");
	  return NULL;
	}
  else
	{
	  TRACE1("m_get_first_modified_op->%s",
			 current_modified_op->get_operation_name()->get_value());
	  return current_modified_op->get_operation_name()->get_value();
	}
}

// lit l'element suivant de la liste des operations modifiees
EXTERN const char *m_get_next_modified_op()
{
  TRACE0("m_get_next_modified_op()");
  ASSERT(current_modified_op != NULL);

  // On passe au suivant
  current_modified_op = (T_operation_checksums *)current_modified_op
	->get_next();

  // On lit de la meme maniere que pour le premier
  return m_get_first_modified_op();
}

// sauvegarde l'etat d'un composant dans le fichier state_file
EXTERN int m_save_state(T_component *comp, char *state_file)
{
  TRACE2("m_save_state(%x, %s)", comp, state_file);

  current_modified_op = NULL;
  return comp->save_state(state_file);
}

// on efface l'etat du composant
EXTERN void m_unlink_state(T_component *comp)
{
  ASSERT(comp != NULL);
  current_modified_op = NULL;
  comp->unlink_state();
}

// fonction de debug
EXTERN void m_print_current_project(void)
{
  manager_sop->get_current()->debug_print();
}

//
// clb : NOUVELLE GESTION DES ACTIONS
//
#ifdef ACTION_NG
static int init_action_ng_flag = FALSE ;

/* initialisation de la nouvelle gestion des actions */
EXTERN void m_init_action_ng(void)
{

	TRACE0("m_init_action_ng") ;

	if ( init_action_ng_flag == FALSE )
		{
			/*initialisation du project manager */
			init_project_manager() ;

			/* initialisation des symbols des actions */
			init_action_symbols() ;

			/*initialisation des symbols des mots clefs du fichier d'etat */
			init_state_symbols() ;

			/* initialisation des classes */
			init_T_global_component_checksum() ;
			init_T_signature() ;
			init_T_action_state() ;
			init_T_component_state() ;

			init_action_ng_flag = TRUE ;
		}
}
/* chargement du fichier d'etat */
EXTERN T_component_state *m_load_state_file(char *state_file_name)
{
	TRACE1("m_load_state_file(%s)", state_file_name) ;
    return state_syntax_analysis(state_file_name) ;
}

/* creation d'un etat du composant vide */
EXTERN T_component_state *m_new_component_state(char *state_file_name)
{
	TRACE1("m_new_component_state(%s)", state_file_name) ;
	T_symbol *file_name = lookup_symbol(state_file_name) ;
	T_component_state *comp = new T_component_state(file_name) ;
	return comp ;
}
/* creation d'une signature avec uniquement le checksum de la racine */
// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs eventuelles trouvees par le bcomp
// et les afficher
EXTERN T_signature *m_create_signature(T_component *racine)
{
	TRACE1("m_create_signature(%s)", m_component_name(racine)) ;
	TRACE1("m_create_signature(%x)", racine->get_checksum()) ;
	//si le checksum est null, on expanse pour l'avoir
	if ( racine->get_checksum() == NULL)
		{
			racine->expand() ;
		}
	TRACE1("m_create_signature(%s)", racine->get_checksum()->get_value()) ;
	T_global_component_checksum *new_check =
		new T_global_component_checksum(racine,
										racine->get_checksum(),
										NULL,
										NULL,
										NULL,
										NULL,
										NULL) ;

	T_signature *new_sign = new T_signature(new_check) ;

	return new_sign ;
}

/* ajout d'un checksum d'un composant d�pendant � la signature cr��e */
EXTERN void m_add_checksum(T_signature *sign,
						   T_component *dependant_component)
{
	TRACE2("m_add_checksum(%x,%s)",
		   sign,
		   m_component_name(dependant_component)) ;

	T_global_component_checksum *dependant_checksum = new
		T_global_component_checksum(dependant_component) ;
	sign->add_checksum(dependant_checksum) ;

}

//dit si l'action est � jour :
//rend TRUE si on ne doit pas faire l'action
//FALSE si il faut la faire
//
//compare la signature courante cur_sign a celle stockee dans le fichier d'�tat
//et regarde le r�sultat de cette action si c'est la meme signature
//si le resultat est ok : rend TRUE
//si les signatures sont differentes ou si le resultat est ko rend FALSE
EXTERN int m_is_update_action(T_signature *current_sign,
							  T_component_state *component_state,
							  T_state_type_action action)
{
	TRACE3(">> m_is_update_action(%s,%x,%d)",
		   current_sign->get_main_checksum()->get_component_name()->get_value(),
		   component_state,
		   action) ;
	int res = component_state->is_update_action(current_sign, action) ;
	TRACE3("<< m_is_update_action(%s,%d,%d)",
		   current_sign->get_main_checksum()->get_component_name()->get_value(),
		   action,
		   res) ;
		   return res ;
}

//dit si une action a deja ete faite sur un composant
EXTERN int m_has_been_done_action(T_component_state *component_state,
								  T_state_type_action action)
{
	TRACE2("m_has_been_done_action(%x,%d)", component_state, action) ;
	return component_state->has_been_done_action(action) ;
}

//dit s'il existe une action precedente a realiser
//current_action est l'action courante a realiser
//first_action est la 1ere action lanc�e : elle sert de reference pour
//connaitre la liste d'actions
EXTERN int m_exist_action_before(T_state_type_action current_action,
								 T_state_type_action first_action)
{
	TRACE2("m_exist_action_before(%d,%d)", current_action, first_action) ;
	return exist_action_todo_before(current_action, first_action) ;
}

//rend l'action pr�c�dente dans la suite d'actions
//current_action est l'action courante a realiser
//first_action est la 1ere action lanc�e : elle sert de reference pour
//connaitre la liste d'actions
EXTERN T_state_type_action m_get_action_before(T_state_type_action
											   current_action,
											   T_state_type_action
											   first_action)
{
	TRACE2("m_get_action_before(%d,%d)", current_action, first_action) ;
	return get_action_todo_before(current_action, first_action) ;
}

//met a jour le fichier d'etat du composant apres la realisation d'une action
EXTERN void m_update_component_state(T_signature *current_sign,
									 T_component_state *component_state,
									 T_state_type_action action,
									 T_action_result result)
{
	TRACE4("m_update_component_state(%x,%x,%d,%d)",
		   current_sign,
		   component_state,
		   action,
		   result) ;
	component_state->update(current_sign, action, result) ;
}

//ecriture du fichier d'etat a partir du nouvel etat du composant
EXTERN void m_write_state_file(T_component_state *component_state)
{
	TRACE1("m_write_state_file(%x)", component_state) ;
	component_state->write_state_file() ;
}


//appel du generateur de forme normale
//calcul de l'etat du composant pour le GOP differentiel
// attention :  cette fonction charge les betrees
// il faut recuperer les erreurs eventuelles trouvees par le bcomp
// et les afficher
//retourne FALSE si erreur TRUE sinon
EXTERN int m_do_generate_normal_form(T_component *component,
									 const char *output_file,
									 T_component *abstraction,
									 const char *abstraction_output_file)
{
	int res = TRUE ; //retour de fonction
	TRACE4(">> m_do_generate_normal_form (%s,%s,%x,%x)",
		   m_component_name(component),
		   output_file,
		   abstraction,
		   abstraction_output_file) ;

	/* generation de la forme normale pour l'abstraction du composant
	   (composant B juste au dessus : (n-1) peut etre un raffinement
	   ou une implementation.
	   Seulement si le composant a une abstraction*/
	if ( abstraction != NULL )
		{
			TRACE2("generation de la forme normale pour l'abstraction(%s,%s)",
				   m_component_name(abstraction),
				   abstraction_output_file) ;

			res =
			  abstraction->comp_generate_normal_form(abstraction_output_file,
													 FALSE) ;
		}
	if (res == TRUE)
		{
			//generation de la forme normale du composant
			//et calcul de l'etat du composant pour le GOP differentiel
			res = component->comp_generate_normal_form(output_file,
													   TRUE) ;
		}
	TRACE2(" <<  m_do_generate_normal_form(%d),FALSE(%d)", res, FALSE) ;
	return res ;
}

//rend le premier T_global_component_checksum d'une signature
EXTERN T_global_component_checksum *m_get_main_checksum(T_signature *sign)
{
	TRACE1("m_get_main_cheksum (%x)", sign) ;
	ASSERT( sign != NULL ) ;
	return sign->get_main_checksum() ;
}

//rend le T_global_component_checksum suivant d'une signature
EXTERN T_global_component_checksum *m_get_next_checksum(T_signature *sign)
{
	TRACE1("m_get_next_cheksum(%x)", sign) ;
	ASSERT( sign != NULL ) ;
	return sign->get_next_checksum() ;
}

//compare le checksum check au checksum principal de la signature sign
//rend TRUE si ils sont egaux FALSE sinon
EXTERN int m_compare_sign_to_checksum(T_signature *sign,
									  T_global_component_checksum *check)
{
	TRACE2("m_compare_sign_to_checksum(%x,%x)", sign, check) ;
	ASSERT( sign != NULL ) ;
	return sign->compare_main_checksum(check) ;
}

//rend le composant d'un T_global_component_checksum
EXTERN T_component *
m_get_component_from_checksum(T_global_component_checksum *check)
{
	TRACE1("m_get_component_from_checksum(%x)", check) ;
	ASSERT( check != NULL ) ;
	return check->get_component() ;
}

//rend la signature de l'etat d'un composant pour une action
EXTERN T_signature *
m_get_state_signature(T_component_state *comp_state,
					  T_state_type_action action)
{
	TRACE2("m_get_state_signature(%x,%d)", comp_state, action) ;
	ASSERT (comp_state != NULL ) ;
	return (comp_state->get_signature(action)) ;
}

#endif /* ACTION_NG */

//appel du compilateur B et du B0Checker : action componentcheck
//retourne TRUE si pas d'erreur
//FALSE si erreur
//est utilise pour le nouveau b0Checker
EXTERN int m_do_component_check(T_component *component, const char* converterName)
{
	TRACE1( ">> m_do_component_check(%s)",
			m_component_name(component) ) ;

        int res = component->do_component_check(converterName) ;

	TRACE2("<< m_do_component_check(%d),FALSE(%d)", res, FALSE) ;

	return res ;

}

#ifdef ACTION_NG
EXTERN T_state_type_action m_get_first_remake_action(void)
{
	return STA_BPP ;
}
#endif // ACTION_NG

// Fonction pour la gestion des repertoire d'include de definition
static T_list_link *current_proof_mechanism;
EXTERN const char *m_get_first_proof_mechanism(void)
{
  TRACE0("m_get_first_proof_mechanism()");
  current_proof_mechanism = manager_sop->get_current()->get_proof_mechanisms();
  if (current_proof_mechanism == nullptr)
    {
      TRACE0("m_get_first_proof_mechanism --> NULL");
      return nullptr;
    }
  else
    {
      T_symbol *sym = (T_symbol *)current_proof_mechanism->get_object();
      TRACE1("m_get_first_proof_mechanism --> %s", sym->get_value());
      return sym->get_value();
    }
}

EXTERN const char *m_get_next_proof_mechanism(void)
{
  ASSERT(current_proof_mechanism != nullptr);
  TRACE0("m_get_next_proof_mechanism()");
  current_proof_mechanism = (T_list_link *)current_proof_mechanism->get_next();
  if (current_proof_mechanism == nullptr)
    {
      TRACE0("m_get_next_proof_mechanism --> NULL");
      return nullptr;
    }
  else
    {
      T_symbol *sym = (T_symbol *)current_proof_mechanism->get_object();
      TRACE1("m_get_next_proof_mechanism --> %s", sym->get_value());
      return sym->get_value();
    }
}

EXTERN int m_add_proof_mechanism(const char *name)
{
  TRACE1("m_add_proof_mechanism(%s)", name);
  return manager_sop->get_current()->add_proof_mechanism(name);
}

EXTERN int m_remove_proof_mechanism(const char *name)
{
  TRACE1("m_add_proof_mechanism(%s)", name);
  return manager_sop->get_current()->remove_proof_mechanism(name);
}

