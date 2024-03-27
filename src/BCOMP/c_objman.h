/******************************* CLEARSY **************************************
* Fichier : $Id: c_objman.h,v 2.0 2000-10-20 12:12:40 fl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface du gestionnaire d'objets
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
#ifndef _C_OBJMAN_H_
#define _C_OBJMAN_H_

class T_object ;
class T_object_tag ;
class T_ident ;
class T_lexem ;
class T_op_result ;
class T_abstract_type ;
class T_item ;

//
//	Classe T_object_manager : modelisation d'un gestionnaire d'objets
//
class T_object_manager
{
  // Liste des objets
  T_object_tag		*first_tag ;
  T_object_tag		*last_tag ;

  // Liste de tous les identificateurs
  T_ident			*first_ident ;
  T_ident			*last_ident ;

  // Liste des identificateurs non resolus
  // (Un chainage simple suffit)
  T_ident				*first_unsolved_ident ;
  T_ident				*last_unsolved_ident ;

  // Liste des T_op_result
  // (Un chainage simple suffit)
  T_op_result			*first_op_result ;
  T_op_result			*last_op_result ;

  // Liste des commentaires non accroches
  // (Un chainage simple suffit)
  T_lexem				*first_unsolved_comment ;
  T_lexem				*last_unsolved_comment ;

  // Liste des types abstraits non encore values
  T_abstract_type		*first_abstract_type ;
  T_abstract_type		*last_abstract_type ;

  // Liste des lexemes du betree courant
  // (Un chainage simple suffit)
  T_lexem				*first_lexem ;
  T_lexem				*last_lexem ;

  // Rang et contexte courants
  size_t				cur_rank ;
  size_t				cur_context ;

  // Nombre d'objets alloues
  size_t				nb_objects ;

public :
  // Constructeur
  T_object_manager(void) ;

  // Destructeur
  virtual ~T_object_manager(void) ;

#ifdef DUMP_UNSOLVED
  // Dump des identificateurs non resolus
  void dump(const char *msg) ;
#endif // DUMP_UNSOLVED

  // Reset variables globales
  void reset(void) ;
  void reset_context(void) ;

  // Obtention des tags
  T_object_tag *get_tags(void) { return first_tag ; } ;

  // Ajout d'un objet
  void add_object(T_object *new_object) ;
  void delete_object(T_object *object_to_delete) ;

  // Ajout/Suppression d'un identificateur
  void add_ident(T_ident *new_ident) ;
  void delete_ident(T_ident *new_ident) ;

  // Ajout d'un identificateur non resolu
  void add_unsolved_ident(T_ident *new_ident) ;
  // Suppresion d'un identificateur non resolu si il est
  // dans la liste (methode qui fait le test)
  void delete_unsolved_ident(T_ident *new_ident) ;

  // Ajout d'un op_result
  void add_op_result(T_op_result *new_op_result) ;

  // Suppression d'un op_result
  void remove_op_result(T_op_result *remove_op_result) ;

  // Reset de la liste
  void reset_op_result_list(void)
	{ first_op_result = last_op_result = NULL ; } ;

  // Ajout d'un lexeme
  void add_lexem(T_lexem *new_lexem) ;

  // Reset de la liste des lexems
  void reset_lexems_list(void)
	{ first_lexem = last_lexem = NULL ; } ;

	  // Obtention de la plus petite addresse memoire d'objet allouee
  size_t get_min_address(void) ;

  // Obtention des commentaires non encore accroches dans l'arbre
  T_lexem *get_unsolved_comments(void) ;

  // Obtention des types abstraits non encore values
  T_abstract_type *get_abstract_types(void)
	{ return first_abstract_type ; } ;

  // Ajouter un commentaire non resolu en fin de liste
  void add_unsolved_comment(T_lexem *comment) ;

  // Ajouter/Supprimer un type abstrait en fin de liste
  void add_abstract_type(T_abstract_type *type) ;
  void delete_abstract_type(T_abstract_type *abstract_type) ;

#ifdef DUMP_TYPES
  void dump_atype_list(void) ;
#endif

  // Prevenir que le premier commentaire de la liste a ete accroche
  // ATTENTION ne marche que pour le premier commentaire non accroche
  void set_solved_comment(T_lexem *solved_comment) ;

  // Reset de la liste des commentaires non resolus
  // A appeler en fin d'analyse syntaxique
  void reset_unsolved_comments(void)
	{
	  TRACE0("reset_unsolved_comments") ;
	  first_unsolved_comment = last_unsolved_comment = NULL ;
	} ;

	// Obtention des lexems
  T_lexem *get_lexems(void)		{ return first_lexem ; } ;
  T_lexem *get_last_lexem(void) {
	TRACE1("get_object_manager()->get_last_lexem() -> %x", last_lexem) ;
	return last_lexem ; } ;

  // Obtention de la liste des identificateurs
  T_ident *get_identifiers(void) { return first_ident ; } ;

#if 0 // NON NON NON
  void reset_identifiers(void)
	{
	  //	  first_ident = last_ident = NULL ;
	}
#endif

  // Obtention du dernier
  T_ident *get_last_ident(void)
	{
	  return last_ident ;
	}

  // Mise a jour du dernier (avec troncage de la liste apres)
  void set_last_ident(T_ident *new_last_ident) ;

  // Obtention de la liste des identificateurs non resolus
  T_ident *get_unsolved_idents(void) ;

  // Obtention du dernier non resolu
  T_ident *get_last_unsolved_ident(void)
	{
	  return last_unsolved_ident ;
	}

  // Mise a jour du dernier non resolu (avec troncage de la liste apres)
  void set_last_unsolved_ident(T_ident *new_last_unsolved_ident) ;

  // Obtention de la liste des op_result
  T_op_result *get_op_results(void) { return first_op_result ; } ;

  // Obtention d'un rang unique dans le contexte courant
  // Le rang est > 0
  size_t get_unique_rank(void)		{ return ++cur_rank ; } ;

  // Obtention du nombre d'objets alloues
  size_t get_nb_objects(void)		{ return nb_objects ; } ;

  // Obtention du contexte courant
  size_t get_context(void)			{ return cur_context ; } ;

  // Declaration d'un nouveau contexte
  size_t new_context(void)
	{ cur_rank = 0 ; return ++cur_context ; } ;

  // Recherche d'un objet
  T_object *find_object(size_t object_rank, size_t object_context) ;

  // Dump HTML
  FILE *dump_html(void) ;

  // Liberer tous les lexemes qui restend (utilise par exemple en mode
  // "dependance" car on a casse le flux donc potentiellement on ne
  // libere pas tout)
  void unlink_lexems(void) ;
} ;

//
//	Classe T_object_tag : modelisation d'une "etiquette" permettant de
//	referencer un objet dans le gestionnaire d'objets
//
class T_object_tag
{
  // Fonctions amies
  friend void T_object_manager::add_object(T_object *new_object) ;

  // Objet reference
  T_object			*object ;

  // Contexte de l'objet
  size_t		   	context ;

  // Chainage dans la liste du gestionnaire d'objets
  T_object_tag		*next_tag ;
  T_object_tag		*prev_tag ;

public :
  // Constructeur
  // Il ne prend pas de parametre, afin de pouvoir allouer d'un seul
  // coup un tableau de T_object_tags
  T_object_tag(void) ;

  // Destructeur
  virtual ~T_object_tag(void) ;

  // Methodes de lecture
  T_object *get_object(void)			{ return object ; } ;
  size_t get_context(void)				{ return context ; } ;
  T_object_tag *get_next_tag(void)		{ return next_tag ; } ;
  T_object_tag *get_prev_tag(void)		{ return prev_tag ; } ;

  // Methodes d'ecriture
  void set_next_tag(T_object_tag *new_next_tag)
	{ next_tag = new_next_tag ; } ;
  void set_prev_tag(T_object_tag *new_prev_tag)
	{ prev_tag = new_prev_tag ; } ;
  // Initialisation i.e. prise en compte d'un objet
  void init(T_object *new_object, size_t new_context) ;

  // Dump HTML
  FILE *dump_html(void) ;
} ;

// Creation/Liberation du gestionnaire d'objets
extern void new_object_manager(void) ;
extern void unlink_object_manager(void) ;


// Obtention du gestionnaire d'objets
extern T_object_manager *get_object_manager(void) ;

// Reset du gestionnaire d'objets (pour le mode cleanup)
extern void reset_object_manager(void) ;
extern void reset_object_manager_context(void) ;

#ifdef WATCH
// Demande d'observation d'une adresse memoire
extern void watch(void **adr) ;

// Affichage de la montre
#define show_watch _show_watch(__FILE__, __LINE__)
extern  void _show_watch(const char *file, int line) ;

  // Obtention de l'adresse observee
extern void **get_watch(void) ;
#else
#define show_watch
#endif

#endif /* _C_OBJMAN_H_ */
