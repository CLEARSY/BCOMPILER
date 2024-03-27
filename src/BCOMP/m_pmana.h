/******************************* CLEARSY **************************************
* Fichier : $Id: m_pmana.h,v 2.0 2000-02-24 14:12:59 clb Exp $
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
#ifndef _M_PMANA_H_
#define _M_PMANA_H_

//
//	Declarations forward de classes
//

//
//	}{	Classe T_project_manager
//
class T_project_manager : public T_object
{
  // project courant
  T_project *current;

  // flag indiquant qu'il faut sauver le projet courant
  int save_flag;

  // flag indiquant que lors de la derniere analyse des dependances
  // le type d'un composant a change.
  int modified_flag;

  // fichier .db associe au projet courant
  T_symbol *file;

  // date du dernier fichier .db charge
  time_t file_date;

  // bibliotheques
  T_project *first_library;
  T_project *last_library;

  // composants absents
  T_component *first_missing;
  T_component *last_missing;

  // composants supprimes d'un multi-composant
  T_list_link *first_removed;
  T_list_link *last_removed;

  // composants ajoutes a un multi-composants
  T_list_link *first_added;
  T_list_link *last_added;

  // Fichiers de definition du projet en cours
  T_list_link *first_file_def;
  T_list_link *last_file_def;


public :
  // Constructeurs
  T_project_manager(void);

  // Destructeurs
  ~T_project_manager();

  // Methodes de lecture
  virtual const char *get_class_name(void) {
	return "T_project_manager"; };
  T_project *get_current(void) { return current; };
  T_project *get_libraries(void) { return first_library; };
  T_component *get_missing(void) { return first_missing; };
  T_symbol  *get_file(void) { return file; };
  T_list_link *get_removed(void) { return first_removed; };
  T_list_link *get_added(void) { return first_added; };
  int get_save_flag(void) { return save_flag; };
  int get_modified_flag(void) { return modified_flag; };
  T_list_link *get_file_def(void) { return first_file_def;};

  // modification du flag de sauvegarde
  void set_save_flag(void) { save_flag = 1; };

  // modification du flag de modification
  void set_modified_flag(int newval) { modified_flag = newval; };

  // Chargement des composants du projet courant
  int load_current_project(const char *full_path);
  int load_current_project(const char *dir, const char *name);
  int reload_current_project(void);
  void remove_current_project(void);
  void unload_current_project(void);

  // Sauvegarde du projet courant
  int save_current_project();

  // recherches sur les utilisateurs du projet courant
  int m_is_component_user(const char *username);
  int m_is_unique_component_user(const char *username);

  // Chargement d'un bibliotheque
  T_project *load_library(const char *full_path, int index = 0) ;
  T_project *load_library(const char *dir, const char *name,
								   int index);

  // Destruction des bibliotheques
  void unlink_libraries(void);

  // Recherche dans les bibliotheques
  T_component *search_library(const char *comp_name);

  // Recherche d'un composant dans le projet puis dans les bibliotheques
  // Retour : chemin d'acces au composant APRES decoupage des fichiers
  // multi-composants (sans le nom du fichier)
  // ATTENTION le nom du composant ne doit pas avoir de suffixe
  T_symbol *find_component_path(const char *comp_name);

  // Ajout SL 29/03/1999
  // Recherche d'un composant dans le projet puis dans les bibliotheques
  // Retour : chemin d'acces au composant AVANT decoupage des fichiers
  // multi-composants (sans le nom du fichier)
  // ATTENTION le nom du composant ne doit pas avoir de suffixe
  T_symbol *find_original_component_path(const char *comp_name);

  // Ajout SL 06/05/1999
  // Renvoie l'index la bibliotheque qui contient le composant, ou 0
  int find_component_library_index(const char *comp_name);

  // Creation d'un composant manquant
  T_component *create_missing_component(const char *name);

  // Recherche d'un composant manquant
  T_component *search_missing_component(const char *name);

  // Destruction des composants manquants
  void unlink_missing_components(void);

  // Ajout d'un composant manquant
  void add_missing_component(T_component *comp);

  // Composants supprimes des fichiers multi-composants
  void unlink_removed_components(void);
  void add_removed_component(T_component *comp);

  // Composants ajoutes a des fichiers multi-composants
  void unlink_added_components(void);
  void add_added_component(T_component *comp);

  // Destruction de la liste des machines de tous les composants
  void unlink_machines(void);

  int test_file_def(T_component *comp, const char* converterName);

  // Récupération de la liste des fichiers de definition
  int set_file_def(const char* converterName);

  // Destruction de la liste des fichiers de definition
  void unlink_file_def(void);

  // Recopie des checksums d'un ancien projet
  int copy_checksums(const char *dir, const char *name);
  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  // Liberation des betree charges dans le betree_manager
  int free_betrees(void);

};

// Methode de gestion
extern void m_init_project_manager(void);

#endif /* _M_PMANA_H_ */
