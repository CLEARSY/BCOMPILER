/******************************* CLEARSY **************************************
* Fichier : $Id: t_locop.h,v 2.0 1999-10-08 13:36:55 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe T_op_call_tree
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
#ifndef _T_LOCOP_H_
#define _T_LOCOP_H_

// Cette classe modelise un arbre d'appels d'operations, elle perrmet
// de detecter qu'il n'existe aucun cycle dans les appels d'operation
// locale du source B
class T_op_call_tree : public T_item
{
private:
  //premier appel a une op loc dans le corps de la fonction
  //correspondante au noeud courant
  T_item * first_son;
  //dernier appel a une op loc dans le corps de la fonction
  //correspondante au noeud courant
  T_item * last_son;
public:
  T_op_call_tree(void);
//   T_op_call_tree(T_item ** new_first_son,
// 						  T_item ** new_last_son,
// 						  T_item * new_father);

  //rend le nom de la classe
  const char *get_class_name(void) const ;
  //rend le noeud pere, il correspond a l'operation ou est effectue
  //l'appel a la fonction representee par le noeud courant
  const T_op_call_tree *get_father() const;
  //rend le symbole representant le nom de la fonction
  const T_symbol *get_op_name() const;

  //dump html
  FILE *dump_html(void);

  //fonctions utilisateur
  int user_defined_f1(void);
  T_object *user_defined_f2(void);
  int user_defined_f3(int arg);
  T_object *user_defined_f4(T_object *arg);
  int user_defined_f5(T_object *arg);

};

#endif // _T_LOCOP_H_
