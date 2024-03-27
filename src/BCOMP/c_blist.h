/******************************* CLEARSY **************************************
* Fichier : $Id: c_blist.h,v 2.0 1998-11-30 10:51:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_betree_list
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
#ifndef _C_BETREE_LIST_H_
#define _C_BETREE_LIST_H_

class T_betree_list : public T_object
{
  // Chainage des Betree via un T_list_link
  T_list_link		*first_link ;
  T_list_link		*last_link ;

public :
  // Constructeur
  T_betree_list(void) ;

  // Destructeur
  virtual ~T_betree_list() ;

  // Methodes d'acces
  virtual const char *get_class_name(void) { return "T_betree_list" ; } ;
  T_list_link *get_links(void) 			  { return first_link ; } ;

	// Ajout d'un betree
	// Renvoie la premiere occurence en cas de redefinition, NULL sinon
  T_list_link *add_betree(T_betree *new_betree, T_lexem *new_ref_lexem) ;

  // Recherche si un betree est dans la liste. Renvoie la definition si oui,
  // et NULL sinon
  T_list_link *search_betree(T_betree *new_betree) ;

	// Dump HTML
  virtual FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_BETREE_LIST_H_ */

