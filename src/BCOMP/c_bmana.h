/******************************* CLEARSY **************************************
* Fichier : $Id: c_bmana.h,v 2.0 1999-06-09 12:38:12 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_betree_manager
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
#ifndef _C_BETREE_MANAGER_H_
#define _C_BETREE_MANAGER_H_

class T_betree_manager : public T_object
{
  // Chainage des Betree
  T_betree		*first_betree ;
  T_betree		*last_betree ;

  //
  // FONCTIONS
  //
  // En mode multi-compo on verifie en plus l'integrite du fichier source
  // i.e. que les composants qui sont la ont bien le droit d'y etre !
  // Version pour fichier .mch, .ref ou .imp
  void single_check(T_symbol *ref_file_name,
							 T_symbol *ref_path_name,
							 T_symbol *ref_mach_name) ;
  // Version pour fichier .mod
  void multi_check(T_symbol *ref_file_name,
							T_symbol *ref_path_name,
							T_symbol *ref_mach_name) ;
  // Fonction auxiliaire que regarde si un betree est un raffinement
  // d'un betree de la liste. Renvoie ce betree si oui, NULL sinon
  T_betree *lookup_abstraction(T_betree *betree, T_list_link *list) ;

  // Fonction auxiliaire que regarde si un betree est importe/etendu
  // par un betree de la liste. Renvoie ce betree si oui, NULL sinon
  T_betree *lookup_importer(T_betree *betree, T_list_link *list) ;
public :
  // Constructeur
  T_betree_manager(void) ;

	// Destructeur
  virtual ~T_betree_manager() ;

  // Methodes d'acces
  virtual const char *get_class_name(void)
	{ return "T_betree_manager" ; } ;
  T_betree *get_betrees(void) ;
  T_betree *get_last_betree(void)  { return last_betree ; } ;

  // Ajout/Suppression d'un betree
  void add_betree(T_betree *new_betree) ;
  void delete_betree(T_betree *new_betree) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Creation d'un gif manager.gif representant le projet
  void create_gif(void) ;

  // En mode multi-compo on verifie en plus l'integrite du fichier source
  // i.e. que les composants qui sont la ont bien le droit d'y etre !
  void check_file_integrity(const char *file_name) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Obtention du Betree manager (avec creation le cas echeant)
extern T_betree_manager *get_betree_manager(void) ;

// Reset gestionnaire de betree
void reset_betree_manager(void) ;

// Recherche d'un Betree en memoire par son nom
// Renvoie NULL si le Betree n'est pas en memoire
extern T_betree *betree_manager_get_betree(T_symbol *betree_name) ;

#endif /* _C_BETREE_MANAGER_H_ */

