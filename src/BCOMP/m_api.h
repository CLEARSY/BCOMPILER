/******************************* CLEARSY **************************************
* Fichier : $Id: m_api.h,v 2.0 2007/09/17 09:05:39 arequet Exp $
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
#ifndef _M_API_H_
#define _M_API_H_

#include "c_port.h"

#ifdef ACTION_NG
/* includes */
#ifdef __MS__
#include "m_action.h"
#else /* __MS__ */
#include <m_action.h>
#endif /* __MS__ */
#endif /* ACTION_NG */

/* Fonctions pour la compatibilite */
#if !defined(__cplusplus) // C, pas CC++
typedef void T_project;
typedef void T_component;
typedef void T_file_component;
#ifdef ACTION_NG
typedef void T_component_state;
typedef void T_signature;
typedef void T_global_component_checksum;
#endif /* ACTION_NG */
#else // C++
class T_project_manager ;
class T_project;
class T_component;
class T_file_component;
extern void init_project_manager(void);
extern T_project_manager *get_project_manager(void);
#ifdef ACTION_NG
class T_component_state;
class T_signature;
class T_global_component_checksum;
#endif /* ACTION_NG */
#endif // !C++

#ifndef CONST
#define CONST const
#endif



/* constantes */
typedef enum {DEF_UNDEFINED, DEF_LOCAL, DEF_LIBRARY} T_file_def_type;

EXTERN CONST char *m_get_bdp();
EXTERN CONST char *m_get_current_library_bdp();
EXTERN T_project *m_get_current();
EXTERN T_component *m_get_library(CONST char *name);
EXTERN T_component *m_get_component(CONST char *name);
EXTERN CONST char *m_component_name(T_component *compo);
EXTERN CONST char *m_component_suffix(T_component *compo);
EXTERN CONST char *m_component_location(T_component *compo);
EXTERN CONST char *m_component_user(T_component *compo);
EXTERN const char *m_component_generated_from(T_component *compo);
EXTERN time_t m_component_date(T_component *compo);
EXTERN CONST char *m_component_path(T_component *compo);
EXTERN CONST char *m_component_type_string(T_component *compo);
EXTERN int m_component_index(T_component *compo);
EXTERN int m_component_is_missing(T_component *compo);
EXTERN int m_component_belongs_to_library(T_component *compo);
EXTERN int m_component_is_an_implementation(T_component *compo);
EXTERN int m_component_is_a_specification(T_component *compo);
EXTERN CONST char *m_component_extension(T_component *compo);
EXTERN void m_component_reset_checksum(T_component *compo);
EXTERN int m_component_has_checksum(T_component *compo);
EXTERN int m_component_expand(T_component *compo, const char* converterName);
EXTERN int m_component_cut(T_component *compo, const char* converterName);


/* initialisation */
EXTERN void m_init(void);

/* libération */
EXTERN void m_unlink(void);

EXTERN void set_localedir(char *l);

/* Fonctions pour la gestion des messages d'erreur et d'avertissement */
EXTERN char *m_get_first_message(void);
EXTERN char *m_get_next_message(void);
EXTERN void m_reset_messages(void);

/* Fonction pour la gestion des repertoires d'include de definition */
EXTERN CONST char *m_get_first_source_path(void);
EXTERN CONST char *m_get_next_source_path(void);
EXTERN int m_add_source_path(CONST char *new_path);
EXTERN int m_remove_source_path(CONST char *new_path);

/* creation d'un fichier vide de description de projet */
EXTERN int m_create_empty_project(CONST char *dir, CONST char *name,
								  CONST char *subdir,
								  CONST char *subdir_oldtools,
								  const int system_or_software);

/* chargement des machines du projet courant */
EXTERN int m_load_current_project(CONST char *dir, CONST char *name);
EXTERN int m_reload_current_project(void);
EXTERN void m_remove_current_project(void);
EXTERN void m_unload_current_project(void);

/* modification du repertoire de decompilation (anciens outils) */
EXTERN void m_set_old_tools_decomp_dir(const char *dir, const char *subdir) ;

/* modification du repertoire de decompilation normale */
EXTERN void m_set_decomp_dir(const char *dir, const char *subdir);

/* sauvegarde sur disque des machines du projet courant */
EXTERN int m_save_current_project();

/* Est-ce qu'un fichier peut-etre ajoute au projet courant ? */
EXTERN int m_file_can_be_added(CONST char *filename);

/* ajout d'un fichier au projet courant */
EXTERN int m_add_file(const char *filename,
                      const char *converterName,
                      const char *dir,
                      const char *user,
                      const char *generator);

/* supression d'un fichier au projet courant */
EXTERN void m_remove_file(CONST char *filename);
/*! Enlève du projet tous les fichiers ayant le tag generated passé
 * en paramètre.
 *
 * \param generated_tag le tag utilisé par le générateur
 */
EXTERN void m_remove_generated_files(CONST char *generated_tag);

/* caracteristiques d'un composant */
EXTERN int m_is_a_specification(CONST char *name);
EXTERN int m_is_a_refinement(CONST char *name);
EXTERN int m_is_an_implementation(CONST char *name);
EXTERN CONST char *m_get_component_user(CONST char *name);
EXTERN int m_is_component_user(CONST char *username);
EXTERN int m_is_unique_component_user(CONST char *username);
EXTERN int m_belongs_to_a_multi_components_file(CONST char *compname);
EXTERN CONST char *m_get_component_filename(CONST char *compname);
#ifdef ACTION_NG
EXTERN int m_component_belongs_to_file(CONST char *compname,
									   CONST char *filename);
#endif /* ACTION_NG */

/* caracteristiques d'un fichier */
EXTERN int m_file_is_a_multi_components(T_file_component *file_ptr);
EXTERN int m_is_a_multi_components_file(CONST char *filename);
EXTERN CONST char *m_file_name(T_file_component *file_ptr);
EXTERN CONST char *m_file_location(T_file_component *file_ptr);
EXTERN CONST char *m_file_suffix(T_file_component *file_ptr);

/* liste des fichiers du projet courant */
EXTERN T_file_component *m_get_first_file();
EXTERN T_file_component *m_get_next_file();

/* liste des composants d'un fichier */
EXTERN CONST char *m_get_first_file_component(CONST char *filename);
EXTERN CONST char *m_get_next_file_component();

/* liste des composants du projet courant */
EXTERN T_component *m_get_first_component();
EXTERN T_component *m_get_next_component();
EXTERN CONST char *m_get_first_component_name();
EXTERN CONST char *m_get_next_component_name();

/* composants associes au librairies */
EXTERN T_project *m_load_library(CONST char *dir, CONST char *name, int index);
EXTERN void m_unlink_libraries(void);
EXTERN T_component *m_get_first_library_component();
EXTERN T_component *m_get_next_library_component();

/* composants manquants */
EXTERN T_component *m_create_missing_component(CONST char *compname);
EXTERN void m_unlink_missing_components(void);
EXTERN T_component *m_get_first_missing_component();
EXTERN T_component *m_get_next_missing_component();

/* fichiers de definition */
EXTERN int m_test_file_def(T_component *comp, const char* converterName);
EXTERN int m_set_file_def(const char* converterName);
EXTERN T_file_def_type m_get_first_file_def(char **path, char **name);
EXTERN T_file_def_type m_get_next_file_def(char **path, char **name);

/* fonctions pour la gestion des dependances */
EXTERN int m_is_expand_dep_needed (T_component *comp);
EXTERN void unlink_removed_components (void);
EXTERN void unlink_added_components (void);
EXTERN int m_component_check_dependencies(T_component *comp, const char* converterName);
EXTERN CONST char *m_get_first_removed_component(T_component *comp);
EXTERN CONST char *m_get_next_removed_component(void);
EXTERN CONST char *m_get_first_new_component(T_component *comp);
EXTERN CONST char *m_get_next_new_component(void);
EXTERN CONST char *m_component_abstraction(T_component *comp);
EXTERN CONST char *m_get_first_includes(T_component *comp, CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_next_includes(CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_first_sees(T_component *comp, CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_next_sees(CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_first_imports(T_component *comp, CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_next_imports(CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_first_extends(T_component *comp, CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_next_extends(CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_first_uses(T_component *comp, CONST char **name, CONST char **rename);
EXTERN CONST char *m_get_next_uses(CONST char **name, CONST char **rename);

/* flag de sauvegarde du projet courant */
EXTERN int m_get_save_flag(void);

/* flag de modification du type B d'un composant */
EXTERN int m_get_modified_flag(void);

/* copie des checksums d'un vieux projet dans un nouveau projet */
/* retourne TRUE si ca s'est bien pass�, et FALSE sinon */
EXTERN int m_copy_checksums(CONST char *bdp,
							CONST char *new_prj_name,
							CONST char *old_prj_name);

//
// ATELIERB DIFFERENTIEL
//

/* rend 1 si le contexte du composant a ete modifie 0 sinon et -1 si erreur */
EXTERN int m_is_component_context_modified(T_component *comp, char *state_file);

/* met � jour la liste des operations modifiees du composant name */
EXTERN int m_set_component_checksums(T_component *comp, const char* converterName);

/* Cree la liste des operations modifiees */
EXTERN int m_set_modified_op_list(T_component *comp, char *state_file);

/* Retourne le premier element de la liste des operations modifiees*/
EXTERN const char *m_get_first_modified_op();

/* lit l'element suivant de la liste des operations modifiees */
EXTERN const char *m_get_next_modified_op();

/* sauvegarde de l'etat d'un composant dans le fichier state_file */
EXTERN int m_save_state(T_component *comp, char *state_file);

/* on efface l'etat du composant */
EXTERN void m_unlink_state(T_component *comp);


/* fonction de debug */
EXTERN void m_print_current_project(void);

//
// clb : NOUVELLE GESTION DES ACTIONS
//
#ifdef ACTION_NG

/* initialisation de la nouvelle gestion des actions */
EXTERN void m_init_action_ng(void) ;

/* chargement du fichier d'etat */
EXTERN T_component_state *m_load_state_file(char *state_file_name) ;

/* creation d'un etat du composant vide */
EXTERN T_component_state *m_new_component_state(char *state_file_name) ;

/* creation d'une signature avec uniquement le checksum de la racine */
EXTERN T_signature *m_create_signature(T_component *racine) ;

/* ajout d'un checksum d'un composant d�pendant � la signature cr��e */
EXTERN void m_add_checksum(T_signature *sign,
						   T_component *dependant_component) ;

#ifdef __MS__
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
							  T_state_type_action action) ;

//dit si une action a deja ete faite sur un composant
EXTERN int m_has_been_done_action(T_component_state *component_state,
								  T_state_type_action action) ;

//dit s'il existe une action precedente a realiser
//current_action est l'action courante a realiser
//first_action est la 1ere action lanc�e : elle sert de reference pour
//connaitre la liste d'actions
EXTERN int m_exist_action_before(T_state_type_action current_action,
								 T_state_type_action first_action) ;

//rend l'action pr�c�dente dans la suite d'actions
//current_action est l'action courante a realiser
//first_action est la 1ere action lanc�e : elle sert de reference pour
//connaitre la liste d'actions
EXTERN T_state_type_action m_get_action_before(T_state_type_action
											   current_action,
											   T_state_type_action
											   first_action) ;

//met a jour le fichier d'etat du composant apres la realisation d'une action
EXTERN void m_update_component_state(T_signature *current_sign,
									 T_component_state *component_state,
									 T_state_type_action action,
									 T_action_result result) ;

//ecriture du fichier d'etat a partir du nouvel etat du composant
EXTERN void m_write_state_file(T_component_state *component_state) ;

//appel du compilateur B et du B0Checker : action componentcheck
//retourne FALSE si erreur TRUE sinon
EXTERN int m_do_generate_normal_form(T_component *component,
									 const char *output_file,
									 T_component *abstraction,
									 const char *abs_output_file) ;

//rend le premier T_global_component_checksum d'une signature
EXTERN T_global_component_checksum *m_get_main_checksum(T_signature *sign) ;

//rend le T_global_component_checksum suivant d'une signature
EXTERN T_global_component_checksum *m_get_next_checksum(T_signature *sign) ;

//compare le checksum check au checksum principal de la signature sign
//rend TRUE si ils sont egaux FALSE sinon
EXTERN int m_compare_sign_to_checksum(T_signature *sign,
									  T_global_component_checksum *check) ;

//rend le composant d'un T_global_component_checksum
EXTERN T_component *
m_get_component_from_checksum(T_global_component_checksum *check) ;

//rend la signature de l'etat d'un composant pour une action
EXTERN T_signature * m_get_state_signature(T_component_state *comp_state,
										   T_state_type_action action) ;


#endif // __MS__
#endif /* ACTION_NG */

//appel du compilateur B et du B0Checker : action componentcheck
//retourne TRUE si pas d'erreur
//FALSE sinon
EXTERN int m_do_component_check(T_component *component, const char* converterName) ;

EXTERN CONST char *m_get_first_proof_mechanism(void);
EXTERN CONST char *m_get_next_proof_mechanism(void);

EXTERN int m_add_proof_mechanism(const char* name);
EXTERN int m_remove_proof_mechanism(const char* name);
#endif // _M_API_H_
