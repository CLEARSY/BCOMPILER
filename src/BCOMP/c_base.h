/******************************* CLEARSY **************************************
* Fichier : $Id: c_base.h,v 2.0 1999-08-11 09:41:24 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des classes de base
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
#ifndef _C_BASE_H_
#define _C_BASE_H_

class T_integer : public T_object
{
  // Valeur absolue, sous forme de chaine
  T_symbol	   *value ;

  // Signe : TRUE si positif, FALSE sinon
  int			is_positive ;

public :
  T_integer() : T_object(NODE_INTEGER) {} ;
  T_integer(T_symbol *new_symbol) ;
  T_integer(unsigned int new_value) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)	{ return "T_integer" ; } ;
  T_symbol *get_value(void) { return value ; } ;
  int get_is_positive(void) { return is_positive ; } ;

  // Passage a l'oppose
  void set_opposite(void)
	{ is_positive = (is_positive == TRUE) ? FALSE : TRUE ; } ;

  // Obtention de la valeur sous type forme d'int (signe compris)
  int get_int_value(void);

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si l'on est plus petit qu'un autre
  // (si ref == NULL, ref = - infini)
  int is_less_than(T_integer *ref) ;

  // Pour savoir si l'on est plus grand qu'un autre
  // (si ref == NULL, ref = + infini)
  int is_greater_than(T_integer *ref) ;

  // Pour savoir si l'on est plus petit ou egal a un autre
  // (si ref == NULL, ref = - infini)
  int is_less_than_or_equal(T_integer *ref) ;

  // Pour savoir si l'on est plus grand ou egal a un autre
  // (si ref == NULL, ref = + infini)
  int is_greater_than_or_equal(T_integer *ref) ;

  // Pour savoir si on est egal a un autre
  int is_equal_to(T_integer *ref)
	{
	  return (    (is_positive == ref->get_is_positive())
			   && (value == ref->value)) ? TRUE : FALSE ;
	} ;
} ;

#endif /* _C_BASE_H_ */


