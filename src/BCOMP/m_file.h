/******************************* CLEARSY **************************************
* Fichier : $Id: m_file.h,v 2.0 2004-01-08 09:13:06 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*					Classe representant un fichier contenant un composant
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
#ifndef _M_FILE_H_
#define _M_FILE_H_

// Definition de types
typedef enum
{
  BC_CALL_DEPENDENCE,
  BC_CALL_SYNTAX,
  BC_CALL_SEMANTIC,
  BC_CALL_B0CHECKER
} T_bcomp_call_type;


//
// Classe T_file_component
//
class T_file_component : public T_item
{
  // Chemin d'acces
  T_symbol *path;

  // nom du fichier (sans chemin ni suffixe)
  T_symbol *name;

  // suffixe
  T_symbol *suffix;

  // utilisateur qui ajoute le composant
  T_symbol *user;

  // date du fichier lu
  time_t date;

  // liste des composants associ�s
  T_list_link *first_component ;
  T_list_link *last_component ;

  // liste des fichiers de definition lies au fichier
  T_file_definition *first_file_definition;
  T_file_definition *last_file_definition;

  // The component from which the file has been generated, if relevant
  T_symbol *generated_from_component;

  // mise a jour des listes de fichiers de definition
  void insert_file_definition(T_file_definition *file_def);

  // retourne TRUE si les T_machine du fichier sont a jour
  int is_up_to_date(char *file_name);

  // calcule la nouvelle date associee aux T_machine generees
  void construct_new_date(char *file_name);

public :
  // Constructeurs
  T_file_component() : T_item(NODE_FILE_COMPONENT) {} ;
  T_file_component(const char *filename,
				   const char *filesuffix,
				   const char *filedir,
				   const char *username,
				   T_item **adr_first,
				   T_item **adr_last,
				   T_item *father);

  // Destructeurs
  virtual ~T_file_component() ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_file_component" ; } ;
  T_symbol    *get_path(void)           { return path; } ;
  T_symbol    *get_name(void)           { return name; } ;
  T_symbol    *get_suffix(void)         { return suffix; } ;
  T_symbol    *get_user(void)           { return user; } ;
  T_list_link *get_components(void)     { return first_component;};
  time_t       get_date(void)           { return date;};
  T_file_definition *get_file_definitions(void)
	{ return first_file_definition;};

	T_symbol *get_generated_from_component(void) { return generated_from_component; }

	void set_generated_from_component(T_symbol* generator) { generated_from_component = generator; }

	bool is_generated() const { return generated_from_component != NULL; }

  // Est-ce un fichier multi-composants ??
  int is_a_multi_components_file();

  // Ajout d'un composant a la liste
  void add_component(T_component *new_comp);

  // Recherche d'un composant dans la liste
  T_component *search_component(T_symbol *comp);

  // Recherche des dependances
  int check_dependencies(const char* converterName);

  // Expansion des definitions
  int expand(const char* converterName);

  // Decoupage des fichiers multi-composants
  int cut(const char* converterName);

  // Chargement des betree d'un fichier
  T_betree *load_betrees(T_bcomp_call_type bc_type,
                                  const char *converterName,
								  T_component *compo = NULL);

  // initialisation de la date
  void raz_date(void) {TRACE0("raz_date"); date = (time_t)(-1);};

  // Recuperation de la T_machine semantique d'un composant
  // ATTENTION :cette fonction charge les betree, il faut les effacer
  // lorsque le traitement est termine
  T_machine *get_semantic_machine(T_component *compo, const char* converterName);

#ifdef ACTION_NG

	// Recuperation du T_betree semantique d'un composant
	// ATTENTION :cette fonction charge les betree, il faudra penser a faire
	// du menage quand le traitement sera termine ...
	T_betree *get_B0Checked_betree(T_component *compo) ;

#endif /* ACTION_NG */

  // Recuperation de la T_machine b0 d'un composant
  // ATTENTION :cette fonction charge les betree, il faut les effacer
  // lorsque le traitement est termine
  T_machine *get_b0checked_machine(T_component *compo, const char* converterName);

  // Fonction de reinitialisation de la liste des fichiers de definition
  void set_file_definition(T_file_definition *file_def);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
};

// Methodes de gestion
extern void m_init_file_component(void);

#endif /* _M_FILE_H */

#ifndef WIN32
#define MS_PATH_SEPARATOR_STRING "/"
#define MS_PATH_SEPARATOR_CHAR   '/'
#else
#define MS_PATH_SEPARATOR_STRING "\\"
#define MS_PATH_SEPARATOR_CHAR   '\\'
#endif /* WIN32 */
