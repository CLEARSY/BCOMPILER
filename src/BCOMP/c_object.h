/******************************* CLEARSY **************************************
* Fichier : $Id: c_object.h,v 2.0 2006-11-10 13:28:23 atelierb Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe de base T_object
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
#ifndef _C_OBJECT_H_
#define _C_OBJECT_H_

#include "c_nodtyp.h"
#include "c_llktyp.h"

class T_symbol ;
class T_disk_manager ;
class T_item ;
class T_metaclass ;

class T_object_tag ;

#ifdef B0C
class T_list_ident;
#endif //B0C

// Magic Number caracterisant les instances de T_object
extern const int OBJECT_MAGIC ;

// Type generique : pointeur sur fonction action
// i.e. fonction qui prend en un parametre un objet et qui retourne un int
class T_object ;
typedef int (*T_action)(T_object *object) ;

class T_object
{
  // Magic number (permet de savoir si un objet est un T_object)
  int			   	magic ;

  // Type du noeud
  T_node_type	   	node_type ;

  // Tag
  T_object_tag		*tag ;

  // Rang au sein du gestionnaire d'objets
  size_t	   		rank ;

  // Champ RESERVE POUR LE SYSTEME
  union
  {
      void				*tmp ;
      int                               tmp_int;
  } tmp;

  // Champ RESERVE POUR LE SYSTEME
  int		   		tmp2 ;

  // Champs utilisateur
  union
  {
      void				*usr1 ;
      int                               usr1_int;
  } usr1;
  union
  {
      void				*usr2 ;
      int                               usr2_int;
  } usr2;

  // Fonction qui sauve les champs d'un objet connaissant sa classe
  // La fonction s'appelle recursivement afin de remonter la hierarchie des
  // classes afin de sauvegarder tout d'abord les champs de classes de base
  void save_fields(T_metaclass *metaclass, T_disk_manager *disk_manager);

  // Fonction qui charge les champs d'un objet connaissant sa classe
  // La fonction s'appelle recursivement afin de remonter la hierarchie des
  // classes afin de charger tout d'abord les champs de classes de base
  void load_fields(T_object *object, T_metaclass *metaclass) ;

  // Fonction qui resoud les champs d'un objet connaissant sa classe
  // La fonction s'appelle recursivement afin de remonter la hierarchie des
  // classes afin de restaurer tout d'abord les champs de classes de base
  void solve_references(T_metaclass *metaclass) ;

protected:
  T_object(void) ;

public :
  // Constructeur, Destructeur
  T_object(T_node_type new_node_type) ;
  virtual ~T_object() ;

  // Methodes de lecture reservees pour le systeme
  void *get_tmp(void) 		{ return tmp.tmp ; } ;
  int get_tmp_int(void) { return tmp.tmp_int; }

  int get_tmp2(void) {return tmp2 ;} ;
  int get_magic(void) 		{ return magic ; } ;

	// Methodes de lecture
  T_node_type get_node_type(void) { return node_type ; } ;
  void *get_usr1(void)  		{ return usr1.usr1 ; } ;
  void *get_usr2(void)  		{ return usr2.usr2 ; } ;

  int get_usr1_int(void) { return usr1.usr1_int; }

  void **get_adr_usr1(void)   	{ return &usr1.usr1 ; } ;
  void **get_adr_usr2(void)   	{ return &usr2.usr2 ; } ;
  size_t get_rank(void)		{ return rank ; } ;

	// Methodes d'ecriture
  void set_tmp(void *new_tmp) { tmp.tmp = new_tmp ; } ;
  void set_tmp_int(int new_tmp) { tmp.tmp_int = new_tmp; }

  void set_tmp2(int new_tmp2) {tmp2 = new_tmp2 ;} ;
  void set_node_type(T_node_type new_node_type)
	{ node_type = new_node_type ; } ;
  void set_usr1(void *new_usr1) 		{ usr1.usr1 = new_usr1 ; } ;
  void set_usr1_int(int new_usr1) { usr1.usr1_int = new_usr1; }
  void set_usr2(void *new_usr2) 		{ usr2.usr2 = new_usr2 ; } ;
  void set_usr2_int(int new_usr2)            { usr2.usr2_int = new_usr2; }

  void set_rank(size_t new_rank)		{ rank = new_rank ; } ;

  void set_tag(T_object_tag *new_tag) { tag = new_tag ; } ;
  T_object_tag *get_tag(void) { return tag ; } ;

  // Obtention du nom de la classe, fonction virtuelle pure
  virtual const char *get_class_name(void) = 0 ;

  // Methodes de dump HTML : ouverture et fermeture de fichier
  virtual FILE *dump_html(void) ;
  void close_html(FILE *fd) ;

	// Methode d'ecriture des champs specifiques a la classe
  void dump_disk(T_disk_manager *disk_manager) ;

  // Recuperation des champs a partir d'une image disque
  void load_disk(T_object *object) ;

  // Resolution des addresses
  void solve_objects(void) ;

  // Est-ce un type ?
  // Faux dans le cas general
  virtual int is_a_type(void) ;

	// Pour savoir si un objet est un item
	// Faux dans le cas general
  virtual int is_an_item(void) { return FALSE ; } ;

  // Pour savoir si un objet est un symbole
  virtual int is_a_symbol(void) ;

  // Methode qui suit une indirection (T_list_link) dans tous les cas
  virtual T_object *follow_indirection(void)
	{
	  return this ;
	}

  // Methode qui suit une indirection (T_list_link) ssi elle est
  // d'un type donne, et rend l'objet pointe, ou this sinon
  // Cas general : on rend this
  virtual T_object *follow_indirection(T_list_link_type ref_type)
	{
	  TRACE2("T_object(%x)::follow_indirection(%d) : ras",
			 this, ref_type) ;
	  return this ;
	  // Pour le warning
	  assert(ref_type != LINK_BUILTIN) ;
	}

  // Fonction generique de clonage
  // Retourne un double exact
  T_object *clone_object(void) ;

  // Rattrapages eventuels sur le clonage : rien dans le cas general
  // Redefini dans la classe T_ident (maj liste unsolved_ident), etc
  virtual void fix_clone(T_object *ref_object) ;

  // Fonction generique de copie des champs pour une classe donnee
  void copy_fields(T_metaclass *metaclass, T_object *ref_object) ;

  // Fonctions de persistance : sauvegarde/restauration disque
  T_io_diagnostic save_object(const char *file_name,
									   T_betree_file_type file_type) ;

#ifdef B0C

  //
  // Controles semantiques sur le type des donnees concretes
  //

  // Controles sur les valuations des constantes concretes et des ensembles
  // abstraits.
  virtual void valuation_check(T_list_ident **list_ident);

  // Controles sur les instanciations de parametres formels de machines
  virtual void instanciation_check(void);

  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;

#endif // B0C

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  static void check_field_offsets();

  /*!
   * Check that the field with the given name has the same offset as the given offset.
   */
  static void check_field_offset(T_metaclass *meta, const char *field_name, int offset);
} ;

#ifndef FULL_TRACE
extern const char *ptr_ref(void *adr) ;
#else
#define ptr_ref(x) _ptr_ref(__FILE__, __LINE__, x)
extern const char *_ptr_ref(const char *file, int line, void *adr) ;
#endif

#ifndef FULL_TRACE
extern const char *sym_ref(T_symbol *adr) ;
#else
#define sym_ref(x) _sym_ref(__FILE__, __LINE__, x)
extern const char *_sym_ref(const char *file, int line, T_symbol *adr) ;
#endif
extern const char *str_ref(const char *adr) ;
extern const char *bool_ref(int new_bool) ;

/* Pour savoir si un objet est une instance de T_object */
// Si no_msg = TRUE on n'affiche pas d'erreur
extern int object_test(void *adr, int no_msg = FALSE) ;

// Dump d'un objet "memoire"
extern void dump_object(void *adr, int size) ;

// Recuperer une valeur entiere dans le buffer, et avancer d'un cran
extern size_t get_integer(void *&base) ;

// Creer un object vide d'un node_type donne
// Rend dans incr la taille en octets de l'objet cree
// Les trois derniers parametres DOIVENT etres positionnes lors d'une
// lecture depuis le disques, ils doivent contenir les pointeurs de la
// liste des symboles lus, et l'adresse courante lue
extern T_object *
	create_object_of_type(T_node_type node_type,
						  int &incr,
						  T_item **adr_first_read_symbol = NULL,
						  T_item **adr_last_read_symbol = NULL ,
						  int cur_adr = 0) ;

// Liberation d'une adresse (si elle est nulle, on ne fait rien)
// Si l'objet est un item, on decremente le compteur de liens. La liberation
// n'a lieu que si le compteur est nul
extern void object_unlink(T_object *adr) ;

// Liberation d'une liste d'items commenceant a first_item
extern void list_unlink(T_item *first_item) ;

// Restauration disque
extern T_object *load_object(const char *file_name,
									  T_betree_file_type &betree_file_type,
									  T_io_diagnostic &io_diagnostic) ;
extern T_object *load_checksums(const char *file_name,
                                      T_betree_file_type &betree_file_type,
                                      T_io_diagnostic &io_diagnostic) ;

#endif /* _C_OBJECT_H_ */


