/******************************* CLEARSY *************************************
* Fichier : $Id: m_compo.h,v 2.0 2006-11-28 09:56:05 atelierb Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*					Classe representant un composant
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
#ifndef _M_COMPO_H_
#define _M_COMPO_H_

// Definition de types
typedef enum
{
  M_LOCAL,
  M_LIBRARY,
  M_MISSING
} T_component_type;


//
//	Declarations forward de classes
//
class T_file_component ;
class T_component_checksums ;


//
//	}{	Classe T_component
//
class T_component : public T_item
{
  // nom du composant
  T_symbol *name ;

  // Type du composant B (machine, refinement, spec)
  T_machine_type machine_type ;

  // Type du composant
  T_component_type type ;

  // fichier associe
  T_file_component *file ;

  // checksum calcule par le compilateur
  T_symbol *checksum ;

  // index pour les composants de bibliotheque
  int index;

  // Marque pour le parcours des listes
  int stamp;

  // Betree des dependances
  T_machine *machine;

  // Etat du composant
  T_component_checksums *compo_checksums;


public :
  // Constructeurs
  T_component() : T_item(NODE_COMPONENT) {
  } ;

	T_component(T_symbol *name,
			  T_machine_type machine_type,
			  T_component_type type,
			  T_file_component *file,
			  T_symbol *checksum,
			  T_item **adr_first,
			  T_item **adr_last,
			  T_item *father);

  // Destructeurs
  virtual ~T_component() ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_component" ; } ;
  T_symbol *        get_name(void)
		{ return name != NULL ? name : lookup_symbol("#error#"); } ;
  T_machine_type    get_machine_type(void)    { return machine_type; } ;
  T_component_type  get_type(void)     { return type; } ;
  T_file_component *get_file(void)     { return file; } ;
  T_symbol *        get_checksum(void);
  int               get_index(void)    { return index; };
  size_t            get_stamp(void)    { return stamp; };
  T_machine        *get_machine(void) { return machine; };
  T_component_checksums *get_component_checksums(void) {
	return compo_checksums;}
  T_project        *getProject(void);

  // Modification du type interne
  void set_type(T_component_type new_type) { type = new_type; };

  // Modification du type B
  void set_machine_type(T_machine_type new_type)
	{ machine_type = new_type ; };

  // Modification de l'index
  void set_index(int new_index) { index = new_index; };

  // Modification du marqueur
  void set_stamp(void) { stamp = get_timestamp(); };

  // Modification du checksum
  void set_checksum(T_symbol *new_checksum);

  // Modfication du fichier
  void set_file(T_file_component *new_file);

  // Modification de la machine associee
  void set_machine(T_machine *new_mach);

  // Gestion des dependances
  int check_dependencies(const char* converterName);

  // Expansion des definition
  int expand(const char* converterName);

  // Decoupage des fichiers multi-composants
  int cut(const char* converterName);

  // Creation de la liste des operation modifiees
  int set_component_checksums(const char* converterName);

	//b0check du composant
        int do_component_check(const char* converterName) ;

  //sauvegarde checksum
        T_io_diagnostic save_checksums(const char *file_name,
                                            T_betree_file_type file_type);
#ifdef ACTION_NG
	//creation du fichier de forme normale du composant
	//retourne fALSE si on a une erreur
	//retourne TRUE si tout s'est bien passe
	//si GOP_diff est a TRUE, on calcule l'etat du composant
	//pour le GOP differentiel
	int comp_generate_normal_form(const char *output_file,
										   int GOP_diff) ;
#endif /* ACTION_NG */

	// rend 1 si le contexte du composant a ete modifie 0 sinon
	//retourne -1 si erreur
	int is_context_modified(char *state_file);

  // Creation de la liste des operation modifiees
  // Retourne le nombre d'erreurs
  int set_modified_op_list(char *state_file);

  // Sauvegarde de l'etat du composant dans le fichier state_file
  int save_state(char *state_file);

  // Nettoyage de l'�tat du composant
  void unlink_state(void);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
};

// Methodes de gestion
extern void m_init_component(void) ;


#endif /* _M_COMPO_H */
