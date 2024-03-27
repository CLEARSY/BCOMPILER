/******************************* CLEARSY **************************************
* Fichier : $Id: i_lstind.h,v 2.0 1999-07-19 15:58:24 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Interface des classes T_list_index et T_index
*				    T_list_index est une liste de T_index
*                   T_index represente une liste d'entiers, representant
*                   les indices d'un acces a un tableau B0
*                   Ces deux classes sont utilisees dans le verifications
*                   des ensembles de maplets B0 (couverture totales des
*                   ensembles indices, inexistance de doublons)
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
*
******************************************************************************/
#ifdef B0C

#ifndef _I_LSTIND_H_
#define _I_LSTIND_H_

class T_type;
class T_item;
//
// Classe T_INDEX
//
class T_index
{
  // Element de la liste
  int literal ;

  // pointeur sur l'element suivant dans la liste
  T_index *next;


public :
  // Constructeur, Destructeur
  T_index(void);

  T_index(T_type *type);

  T_index(T_index *index);

  T_index(int item);

  ~T_index(void);

  // Compatibilite ascendante
#define get_litteral get_literal

  // Methodes de lecture
  int get_literal(void) { return literal ;};

  T_index * get_next(void) { return next ; };

  // Verifie si les indices du  maplet en parametre correspondent a la liste
  int is_equal(T_item *maplet);

  // Ajoute un entier a la liste (en queue)
  void add(int item);

} ;



//
// Classe T_LIST_INDEX
//
class T_list_index
{
  // pointeur sur le premier element de la liste
  T_index *index ;

  // pointeur sur le suivant dans la liste
  T_list_index *next;


public :
  // Constructeur, Destructeur
  T_list_index(void);

  T_list_index(T_index *item);

  ~T_list_index(void);

  // Methodes de lecture
  T_index * get_index(void)
	{ return index ;};

  T_list_index * get_next(void)
	{ return next ;};

  // Methodes d'ecriture
  void set_next(T_list_index *next_list_index)
	{ next = next_list_index ;};

  // Ajoute un index a la liste (en queue)
  void add(T_index *item);

  // controle si le maplet en parametre est conforme a un index de la liste
  T_list_index * check_maplet(T_item *maplet);

} ;

#endif /* _I_LSTIND_H_ */

#endif // B0C
