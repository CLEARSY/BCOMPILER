/******************************* CLEARSY **************************************
* Fichier : $Id: c_betree.h,v 2.0 2004-01-08 09:33:01 cm Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Compilateur B
*					Interface d'un betree
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
#ifndef _C_BETREE_H_
#define _C_BETREE_H_

#include "c_item.h"

//
//	Declarations forward de classes
//
class T_machine ;
class T_definition ;
class T_file_definition ;
class T_flag ;
class T_betree_manager ;
class T_betree_list ;
class T_used_machine ;

#ifdef B0C
class T_list_ident;
#endif // B0C

//
//	}{	Type d'utilisation de composant
//
typedef enum
{
  /* 00 */
  USE_SEES = 0,
  /* 01 */
  USE_INCLUDES,
  /* 02 */
  USE_EXTENDS,
  /* 03 */
  USE_USES,
  /* 04 */
  USE_IMPORTS
} T_use_type ;

//
//	}{	Etat du Betree
//
typedef enum
{
  // 00 Betree en cours de construction
  BST_PARSING,
  // 01 Betree ayant passe l'etape d'analyse syntaxique
  BST_SYNTAXIC,
  // 02 Betree ayant passe l'etape d'analyse semantique
  BST_SEMANTIC,
  // 03 Betree ayant pass l'etape de B0 Check
  BST_B0_CHECKED,
  // 04 Betree de dependance de fichiers
  BST_DEPENDENCIES
} T_betree_status ;

//
//	}{	Classe T_betree
//
class T_symbol ;
class T_betree : public T_item
{
#ifdef __BCOMP__
  friend T_betree *syntax_analysis(T_lexem *, const char *) ;
  friend T_betree *internal_syntax_analysis(const char *component_name,
                                            int dep_mode,
													 const char *input_file,
                                                                                                         const char *converterName,
													 const char *s_i_file,
													 const char *t_i_file,
													 T_lexem *load_lexem,
													 int complain_if_unrdble) ;
  friend T_betree *read_betree(const char *file_name,
										T_betree_file_type file_type) ;
  friend T_betree *create_betree_refinement(T_betree *abstraction,
                                                     const char* converterName,
		  const char *new_name,
		  int create_imp,
		  int do_seman,
		  char *svdi,
		  int nb_operations,
		  char **operations) ;
#endif

  // Composant decrit
  T_machine			*root ;

  // Nom du fichier associe
  T_symbol			*file_name ;

  // Path de recherche (si NULL : repertoire courant)
  T_symbol			*path ;

  // Gestionnaire
  T_betree_manager	*manager ;

  // Etat
  T_betree_status	status ;

  // Checksum (ne prend pas en compte l'expansion des definitions des
  // fichiers de definitions)
  T_symbol   		*checksum ;

  // Checksum prenant en compte l'expansion des definitions des
  // fichiers de definitions
  T_symbol   		*expanded_checksum ;

  // Version SCCS ou RCS
  char				*revision_ID ;

  // Nom du composant, eventuellement pointe en cas
  // de renommage
  T_symbol			*betree_name ;

  //
  //	Informations sur les lexemes
  //
  // Lexemes
  T_lexem 			*first_lexem ;
  T_lexem 			*last_lexem ;

  // Dernier lexeme non commentaire
  T_lexem 			*last_code_lexem ;

  // Localisation de la clause DEFINITIONS
  T_flag 			*definitions_clause ;

  // Definitions
  T_definition		*first_definition ;
  T_definition		*last_definition ;

  // Fichiers de definitions
  T_file_definition *first_file_definition ;
  T_file_definition *last_file_definition ;

  // Chainage au sein du Betree Manager
  T_betree			*next_betree ;
  T_betree			*prev_betree ;

  // Pour savoir si on a deja verifie l'integrite composant/fichier
  // pour ce betree
  int				integrity_check_done ;

  // 5 listes de composants qui utilisent
  //
  // ATTENTION CES CINQ LISTES DOIVENT ETRE CONSECUTIVES DANS
  // LA CLASSE, seen_by DOIT ETRE LE PREMIER, ET used_by LE DERNIER
  //
  // CERTAINES METHODES UTILISENT CETTE PROPRIETE
  //
  // Betrees qui SEES ce Betree
  T_betree_list		*seen_by ;

  // Betrees qui INCLUDES ce Betree
  T_betree_list		*included_by ;

  // Betrees qui IMPORTS ce Betree
  T_betree_list		*imported_by ;

  // Betree qui EXTENDS ce Betree
  T_betree_list		*extended_by ;

  // Betree qui USES ce Betree
  T_betree_list		*used_by ;

  bool collision_analysed;

public :
  T_betree() : T_item(NODE_BETREE) {} ;
  T_betree(const char *new_file_name) ;

  virtual ~T_betree() ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

//  // Methode de sauvegarde sur disque
//  void save_object(const char *file_name,
//                            T_betree_file_type file_type) ;

//  void save_checksums(const char *file_name,
//                            T_betree_file_type file_type) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_betree" ; }  ;
  T_machine *get_root(void) 			{ return root ; } ;
  T_betree_manager *get_manager(void)	{ return manager ; } ;
  T_symbol *get_file_name(void) 		{ return file_name ; } ;
  T_symbol *get_path(void) 			{ return path ; } ;
  T_symbol *get_checksum(void)   		{ return checksum ; } ;
  T_symbol *get_expanded_checksum(void){ return expanded_checksum ; } ;
  const char *get_revision_ID(void)	{ return revision_ID ; } ;
  T_symbol *get_betree_name(void) 	{ return betree_name ; } ;
  T_flag *get_definitions_clause(void)			{ return definitions_clause ; } ;
  T_definition *get_definitions(void)	{ return first_definition ; } ;
  T_file_definition *get_file_definitions(void)
	{ return first_file_definition ; } ;
  T_betree_status get_status(void)		{ return status ; } ;
  T_betree *get_next_betree(void)		{ return next_betree ; } ;
  T_betree *get_prev_betree(void)		{ return prev_betree ; } ;
  T_betree_list *get_seen_by(void)		{ return seen_by ; } ;
  T_betree_list *get_included_by(void)	{ return included_by ; } ;
  T_betree_list *get_imported_by(void)	{ return imported_by ; } ;
  T_betree_list *get_extended_by(void)	{ return extended_by ; } ;
  T_betree_list *get_used_by(void)		{ return used_by ; } ;
  T_lexem *get_lexems(void)			{ return first_lexem ; } ;
  T_lexem *get_last_lexem(void)	   	{ return last_lexem ; } ;
  int get_integrity_check_done(void) { return integrity_check_done ; } ;

  // Methodes d'ecriture
  void set_root(T_machine *new_root) { root = new_root ; } ;
  void set_status(T_betree_status new_status)
	{ status = new_status ; } ;
  void set_manager(T_betree_manager *new_manager)
	{ manager = new_manager ; } ;
  void set_next_betree(T_betree *new_next_betree)
	{ next_betree = new_next_betree ; } ;
  void set_prev_betree(T_betree *new_prev_betree)
	{ prev_betree = new_prev_betree ; } ;
  void set_file_name(T_symbol *new_file_name)
	{ file_name = new_file_name ; } ;
  void set_path(T_symbol *new_path)
	{ path = new_path ; } ;
  void set_betree_name(T_symbol *new_betree_name)
	{ betree_name = new_betree_name ; } ;
  void set_integrity_check_done(int new_integrity_check_done)
	{ integrity_check_done = new_integrity_check_done ; } ;

  // Ajout d'une reference sur un Betree chargeur
  void add_list(T_use_type use_type,
						 T_betree *loader_betree,
						 T_lexem *new_ref_lexem) ;

  // Acces au pathname (via T_used_machine associe ou this si racine)
  T_symbol *get_pathname(void) ;

  bool get_collision_analysed() {return collision_analysed;}

  void set_collision_analysed(bool b) {collision_analysed = b;}

#ifdef B0C
  // On verifie que le Betree ne modelise pas une implementation
  // Emission d'un message d'erreur si ce n'est pas le cas
  // disant que item est interdit en implementation
  void check_is_not_an_implementation(T_item *item) ;

  // On teste si le betree modelise une implementation
  // renvoie TRUE dans ce cas, FALSE sinon.
  int check_is_an_implementation(void) ;

  // V�rification B0 sur les op�rations
  virtual void operation_B0_check();

  // controle du typage des constantes concretes
  void concrete_constant_type_check(void);

  // controle du typage des variables concretes
  void concrete_variable_type_check(void);

  // controle du typage des parametres formels scalaires
  void scalar_parameter_type_check(void);

  // Controles sur les valuations des constantes concretes et des ensembles
  // abstraits.
  virtual void valuation_check(T_list_ident **list_ident);

  // Controles sur les instanciations de parametres formels de machines
  virtual void instanciation_check(void);

  // Controle des cycles dans les initialisations
  virtual void initialisation_check(T_list_ident **list_ident) ;


#endif // BOC

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void make_standalone(void) ;
} ;

// Restauration d'un betree a partir d'un fichier disque
extern T_betree *read_betree(const char *file_name,
									  T_betree_file_type file_type) ;

// Sauvegarde d'un betree a partir d'un fichier disque
extern T_betree *read_betree(const char *file_name,
									  T_betree_file_type file_type) ;

#endif /* _C_BETREE_H_ */

