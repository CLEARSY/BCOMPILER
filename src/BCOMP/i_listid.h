/******************************* CLEARSY **************************************
* Fichier : $Id: i_listid.h,v 2.0 1999-01-14 16:19:12 ab Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Interface de la classe T_list_ident
*					Gestion d'une liste d'identificateur pour le controle
*                   RINIT 1-1 (La partie droite d'une affectation dans la
*                   clause INITIALISATION ne doit comporter que des variables
*                   initialisees)
*
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
#ifdef B0C

#ifndef _I_LISTID_H_
#define _I_LISTID_H_

class T_item;
class T_ident;
class T_list_link;
//
// Classe T_LIST_IDENT
//
class T_list_ident
{
  // pointeur sur le premier element de la liste
  T_item *first ;

  // pointeur sur le dernier element de la liste
  T_item *last;


public :
  // Constructeur, Destructeur
  T_list_ident(void);

  T_list_ident(T_list_ident *list_ident);

  ~T_list_ident(void);

  // Methodes de lecture
  T_list_link * get_list_ident(void)
	{ return (T_list_link *) first ;};


  // Recherche si l'identificateur en parametre fait partie de la liste
  // Rend TRUE dans ce cas
  int find_identifier(T_ident *ident) ;

  // Ajoute un identificateur a la liste
  void add_identifier(T_ident *ident);

  // Realise l'intersection avec une autre T_list_ident
  void make_intersection(T_list_ident *list_ident);

} ;

#endif /* _I_LISTID_H_ */

#endif // B0C
