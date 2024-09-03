/******************************* CLEARSY **************************************
* Fichier : $Id: m_projec.h,v 2.0 2006-11-28 09:56:05 atelierb Exp $
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
#ifndef M_PROJECT_H
#define M_PROJECT_H

#include<set>    // welcome to XXI century
#include<string>

#include "c_item.h"

typedef enum T_project_type
{
    PROJECT_SYSTEM,
    PROJECT_SOFTWARE,
    PROJECT_VALIDATION,
    PROJECT_TYPE_UNKNWON
} T_project_type;

//
//	Declarations forward de classes
//

class T_component;
class T_io_diagnost;
class T_symbol;
class T_list_link;
class T_object;
class T_file_component;

class T_project : public T_item
{
  // index pour les projets bibliotheque
  int index;

  // repertoire pour les fichier decompiles avec les
  // options pour la compatibilite avec l'ancienne
  // chaine.
  T_symbol *old_tools_decomp_dir;

  // repertoire pour les fichiers decompiles normalement
  T_symbol *decomp_dir;

  // repertoire du bdp
  T_symbol *bdp_dir;

  // timestamp lors de la derniere sauvegarde
  size_t timestamp;

  // liste des repertoires pour les definitions
  T_list_link *first_source_path;
  T_list_link *last_source_path;

  // liste des fichiers
  T_file_component *first_file;
  T_file_component *last_file;

  // liste des composants
  T_component *first_component;
  T_component *last_component;

  // Type tel qu'il est défini par T_project_type
  int project_type;

  // Type de projet à utiliser dans le cas ou aucun projet n'est chargé
  static T_project_type default_project_type;

  // liste des noms de mécanismes de preuve utilisables
  T_list_link* first_proof_mechanism;
  T_list_link* last_proof_mechanism;

public :
  // Constructeurs
  T_project() ;
  T_project(T_item **adr_first,
			T_item **adr_last,
			T_item *father);


  // Destructeurs
  ~T_project();

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_project"; }
  T_file_component *get_files(void) { return first_file; }
  T_component *get_components(void) { return first_component; }
  int get_index(void) { return index; }
  T_symbol *get_old_tools_decomp_dir(void)
    { return old_tools_decomp_dir; }
  T_symbol *get_decomp_dir(void) { return decomp_dir; }
  T_symbol *get_bdp_dir(void) { return bdp_dir; }
  size_t get_timestamp(void) { return timestamp; }
  T_list_link *get_sources_path(void) { return first_source_path; }
  T_list_link* get_proof_mechanisms(void) { return first_proof_mechanism; }
  bool has_proof_mechanism(const char* name) const;

  int isEventB() { return project_type == PROJECT_SYSTEM; }

  int isValidation() { return project_type == PROJECT_VALIDATION; }

  // ajout d'un repertoire de sources
  int add_source_path_project(const char *new_path);

  // suppression d'un repertoire de sources
  int remove_source_path(const char *old_path);

  // chargement des repertoires de sources dans le compilateur
  void load_source_paths(void);

  // modification du timestamp
  void set_timestamp(size_t newval) { timestamp = newval; }

  // modification de l'index
  void set_index(int new_value) { index = new_value; }

  // modification du repertoire de decompilation pour
  // l'ancienne chaine
  void set_old_tools_decomp_dir(const char *dir, const char *subdir);
  void set_old_tools_decomp_dir(const char *fullpath);

  // modification du repertoire de decompilation
  void set_decomp_dir(const char *dir, const char *subdir);
  void set_decomp_dir(const char *fullpath);

  // modification du type de projet
  void set_project_type(const int s) { project_type = s; }

  // modification du repertoire bdp
  void set_bdp_dir(const char *fullpath);

  // modification du type de tous les composants du projet
  void set_components_type(T_component_type new_type);

  // modification de l'index de tous les composants du projet
  void set_components_index(int index);

  // Liste des composants : fonctions de base
  void         add_component_to_list(T_component *new_component);
  void         remove_component(T_component *old_component);

  // Mise a jour des checksums des composants suite a un "restore"
  int copy_checksums(T_project *old_project);

  // Recherche d'un composant
  T_component *search_component(const char *name);
  T_component *search_component(T_symbol *name);

  // Liste des fichiers : fonctions de base
  T_file_component *search_file(const char *name);
  void         add_file_to_list(T_file_component *new_file);
  void         remove_generated_files(const char *component);
  void         remove_file(const char *filename);


  // Ajout d'un fichier au projet
  int add_file(const char *name,                // nom + suffixe
                        const char* converterName,
						const char *dir,                 // repertoire
						const char *user,                // utilisateur
                        const char *generated_from = nullptr); // label indiquant par quoi le fichier a été généré

  // fonction de debug
  void debug_print();

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  // Fonction de sauvegarde
  T_io_diagnostic save_project_xml(const char *file_name);

  static T_project_type get_default_project_type(void);
  static void set_default_project_type(T_project_type project_type);
public:
  // ajout d'un mécanisme de preuve
  int add_proof_mechanism(const char *name);

  // suppression d'un mécanisme de preuve
  int remove_proof_mechanism(const char *name);

} ;

// Methode de gestion
extern void m_init_project(void);

/* chargement d'un projet */
T_project *load_project_xml(const char *file_name);


#endif /* M_PROJECT_H */
