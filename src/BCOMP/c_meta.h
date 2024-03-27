/******************************* CLEARSY **************************************
* Fichier : $Id: c_meta.h,v 2.0 1999-08-27 13:29:56 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des metaclasses
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
#ifndef C_META_H_
#define C_META_H_

#include<stddef.h>
#include<stdio.h>

#include "c_nodtyp.h"

//
//	Definition d'un type de champ de classe
//
typedef enum
{
  /* 00 Champ de type (int)			*/
  FIELD_INTEGER,
  /* 01 Champ de type (T_object *)	*/
  FIELD_OBJECT,
  /* 02 Champ de type (char *)		*/
  FIELD_STRING,
  /* 03 Champ de type (void *)		*/
  FIELD_POINTER,
  /* 04 Champ de type "remplissage" (pour les pointeurs sur les fct virtuelles)	*/
  FIELD_FILLING,
  /* 05 Champ de type utilisateur */
  FIELD_USER,
  /* 06 Champ a reinitialiser */
  FIELD_RESET,
  /* 07 Champ de type "upgrade" (non lu mais sauve) */
  FIELD_UPGRADE,
  /* 08 Champ de type (time_t) */
  FIELD_TIME_T
} T_field_type ;

//
//	Definition d'un type de reference
//
typedef enum
{
  /* 00 Reference partagee : l'objet reference est conserve si on clone */
  /* l'objet qui le reference */
  REF_SHARED,
  /* 01 Reference privee : l'objet reference doit etre clone si on clone */
  /* l'objet qui le reference */
  REF_PRIVATE,
  /* 02 Reference privee, de type liste : cloner l'objet et tous */
  /* ses suivants */
  REF_PRIVATE_LIST,
  /* 03 Reference a ne pas copier. Utilise pour le LAST d'une liste */
  /* qui a deja ete copie car le FIRST etait de type private_list */
  REF_NO_COPY
} T_ref_type ;

//
//	Definition d'un champ de classe
//
class T_metaclass ;
class T_field
{
  // Metaclasse
  T_metaclass			*metaclass ;

  // Champs suivant, precedent
  T_field				*next ;
  T_field				*prev ;

  // Nom du champ
  const char			*field_name ;

  // Type du champ
  T_field_type			field_type ;

  // Type de reference
  T_ref_type			ref_type ;

  // Offset du champ (calcule automatiquement)
  size_t				field_offset ;

  // Offset du champ avant upgrade (calcule automatiquement)
  size_t				field_offset_before_upgrade ;

public :
  T_field(const char *new_field_name,
		  T_field_type new_field_type,
		  T_ref_type new_ref_type,
		  T_field **adr_first,
		  T_field **adr_last,
		  T_metaclass *new_metaclass) ;

  // Methodes d'acces
  T_field *get_next(void) 			{ return next ; }
  T_field *get_prev(void) 			{ return prev ; }
  const char *get_field_name(void)	{ return field_name ; }
  T_field_type get_field_type(void){ return field_type ; }
  void set_field_type(T_field_type new_field_type)
    { field_type = new_field_type ; }
  T_ref_type get_ref_type(void)	{ return ref_type ; }
  size_t get_field_offset(void)   	{ return field_offset ; }
  size_t get_field_offset_before_upgrade(void)
    { return field_offset_before_upgrade ; }
  size_t get_field_size(void) ;

  // Recalculer l'offset
  void fix_offsets(void) ;
} ;

//
//	Definition d'une metaclasse
//
class T_metaclass
{
  // Nom de la classe
  const char			*class_name ;

  // Description des champs propres a la classe
  T_field				*first_field ;
  T_field				*last_field ;

  // Taille, en octets, des instances de la classe
  size_t				size ;
  size_t				size_before_upgrade ;

  // Timestamp
  size_t				timestamp ;

  // Acces a la superclasse de la metaclasse
  T_metaclass  		    *superclass ;

  //
  // Methodes pour la production automatique de la liste des classes
  //

  // Profondeur dans la hierarchie des classes (0 = pas initialise)
  size_t	   			depth ;

  // Premier de la liste des classes derivees (niveau depth-1)
  T_metaclass           *first_derived ;

  // Suivant dans la liste a laquelle j'appartiens (niveau depth)
  T_metaclass           *next_derived ;

public :
  // Constructeur
  T_metaclass(const char *new_class_name,
			  size_t new_size,
			  T_metaclass *new_superclass) ;
  ~T_metaclass() ;

  // Ajout d'un champ
  void add_field(const char *new_field_name,
						  T_field_type new_field_type,
						  T_ref_type new_ref_tytpe = REF_SHARED) ;

  // Methodes de lecture
  const char *get_class_name(void)		{ return class_name ; }
  T_field *get_fields(void)			{ return first_field ; }
  T_field *get_last_field(void)		{ return last_field ; }
  size_t get_size(void)				{ return size ; }
  size_t get_size_before_upgrade(void)
    { return size_before_upgrade ; }
  void set_size(size_t new_size) { size = new_size ; }
  void set_size_before_upgrade(size_t new_size_before_upgrade)
    { size_before_upgrade = new_size_before_upgrade ; }
  T_metaclass *get_superclass(void)		{ return superclass ; }
  size_t get_timestamp(void) { return timestamp ; }
  void set_timestamp(size_t new_timestamp)
    { timestamp = new_timestamp;}
  T_metaclass *get_next_derived(void) const  { return next_derived ; }
  void set_next_derived(T_metaclass *derived)
	{ next_derived = derived; }
  size_t get_depth(void) { return depth ; }


  // Recherche d'un champ par son nom (renvoie NULL si n'existe pas)
  T_field *find_field(const char *field_name) ;

  //
  // Methodes pour la production automatique de la liste des classes
  //

  // Calcule la profondeur d'une classe en appelant recursivement la
  // methode pour la superclasse si besoin est.
  // Au passage, met a jour la liste des classes derivees de la superclasse
  void make_depth(void) ;

  // Ajout d'une sous-classe derived
  // Les sous-classes sont triees par ordre alphabetique
  void add_derived_class(T_metaclass *derived) ;

  // Cree dans output_path une page decrivant la classe
  // Ajoute dans index une entree pour la classe
  void print_class_hierarchy(const char *output_path,
									  size_t output_path_len,
									  FILE *index) ;

  // Retourne la taille la plus grande que peut avoir un champ de la métaclasse
  size_t get_max_field_size(void) ;
} ;

//
//	Methodes de gestion
//

// Initialisation/Liberation des metaclasses
extern void	init_metaclasses(void) ;
extern void	unlink_metaclasses(void) ;

// Obtention de la metclasse pour un node_type donne
extern T_metaclass *get_metaclass(T_node_type node_type) ;

// Obtention de la taille de la classe pour un node_type donne
extern size_t get_class_size(T_node_type node_type) ;
extern size_t get_class_size_before_upgrade(T_node_type node_type) ;

// Obtention d'une metaclasse d'un nom donne, ou NULL si elle n'existe pas
extern T_metaclass *get_metaclass(const char *class_name) ;

// Mise a jour de la metclasse pour un node_type donne
extern void set_metaclass(T_node_type node_type, T_metaclass *metaclass) ;

// Recalcul de tous les offsets de tous les champs de toutes les metaclasses
extern void fix_metaclasses_offsets(void) ;

// Affichage de la hierarchie des classes du BCOMP
extern void dump_classes_hierarchy(const char *output_path) ;

#endif /* C_META_H_ */


