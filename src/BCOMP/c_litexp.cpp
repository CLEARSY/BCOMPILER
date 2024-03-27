/******************************* CLEARSY **************************************
* Fichier : $Id: c_litexp.cpp,v 2.0 1999-08-09 13:47:56 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions litterales
*
* Compilation :		-DDEBUG_LITEXP pour avoir des traces
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
#include "c_port.h"
RCS_ID("$Id: c_litexp.cpp,v 1.14 1999-08-09 13:47:56 jfm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_literal_integer
//
// Constructeur avec valeur explicite
T_literal_integer::T_literal_integer(size_t new_value,
												T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_INTEGER, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_LITEXP
  TRACE5("T_literal_integer::T_literal_integer(%x, %x, %x, %x, %x)",
		 adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  value = new T_integer(new_value) ;
  T_type *type = new T_predefined_type(BTYPE_INTEGER,
									   NULL, NULL, // bounds
									   this,
									   betree,
									   ref_lexem) ;

  set_B_type(type, NULL) ;
  type->unlink() ;
}

// Constructeur avec valeur recuperee dans lexemes
T_literal_integer::T_literal_integer(T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_INTEGER, adr_first, adr_last, father, betree, ref_lexem)
{
#ifdef DEBUG_LITEXP
  TRACE5("T_literal_integer::T_literal_integer(%x, %x, %x, %x, %x)",
		 adr_first, adr_last, father, betree, ref_lexem) ;
#endif

  ASSERT(ref_lexem->get_lex_type() == L_NUMBER) ;
  value = new T_integer(lookup_symbol(ref_lexem->get_value())) ;

  T_type *type = new T_predefined_type(BTYPE_INTEGER,
									   NULL, NULL, // bounds
									   this,
									   betree,
									   ref_lexem) ;
  set_B_type(type, NULL) ;
  type->unlink() ;
}

//
//	Destructeur
//
T_literal_integer::~T_literal_integer(void)
{
  TRACE1("T_literal_integer::~T_literal_integer(%x)", this) ; 
}


//
// Methodes de comparaison
//
// Renvoie TRUE ssi this < ref ou si ref == NULL
int T_literal_integer::is_less_than(T_literal_integer *ref)
{
  // Il faut tester NULL a part car pour T_integer::is_less_than,
  // NULL vaut -infini
  if (ref == NULL)
	{
	  return TRUE ;
	}
  return value->is_less_than(ref->value) ;
}

// Renvoie TRUE ssi this > ref ou si ref == NULL
int T_literal_integer::is_greater_than(T_literal_integer *ref)
{
  // Il faut tester NULL a part car pour T_integer::is_greater_than,
  // NULL vaut +infini
  if (ref == NULL)
	{
	  return TRUE ;
	}
  return value->is_greater_than(ref->value) ;
}

// Passage a l'oppose
void T_literal_integer::set_opposite(void)
{
  value->set_opposite() ;
}

//
//	}{	Constructeur de la classe T_literal_string
//
// Constructeur a partir d'un lexeme
T_literal_string::T_literal_string(T_item **adr_first,
											  T_item **adr_last,
											  T_item *father,
											  T_betree *betree,
											  T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_STRING, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE5("T_literal_string::T_literal_string(%x, %x, %x, %x, %x)",
		 adr_first, adr_last, father, betree, ref_lexem) ;

  ASSERT(ref_lexem->get_lex_type() == L_STRING) ;
  T_type *type = new T_predefined_type(BTYPE_STRING,
									   NULL, NULL,
									   this, betree, ref_lexem) ;
  set_B_type(type, NULL) ;
  type->unlink() ;

  value = lookup_symbol(ref_lexem->get_value()) ;
}

// Constructeur a partir d'une chaine
T_literal_string::T_literal_string(const char *new_value,
											  int new_value_len,
											  T_item **adr_first,
											  T_item **adr_last,
											  T_item *father,
											  T_betree *betree,
											  T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_STRING, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_literal_string::T_literal_string(%s:%d, %x, %x, %x, %x, %x)",
		 new_value, new_value_len, adr_first, adr_last, father, betree, ref_lexem) ;

  T_type *type = new T_predefined_type(BTYPE_STRING,
									   NULL, NULL,
									   this, betree, ref_lexem) ;
  set_B_type(type, NULL) ;
  type->unlink() ;

  value = lookup_symbol(new_value, new_value_len) ;
}

//
//	Destructeur
//
T_literal_string::~T_literal_string(void)
{
  TRACE1("T_literal_string::~T_literal_string(%x)", this) ; 
}


//
//	}{	Constructeur de la classe T_literal_real
//
// Constructeur a partir d'un lexeme
T_literal_real::T_literal_real(T_item **adr_first,
                                                                                          T_item **adr_last,
                                                                                          T_item *father,
                                                                                          T_betree *betree,
                                                                                          T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_REAL, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE5("T_literal_real::T_literal_real(%x, %x, %x, %x, %x)",
                 adr_first, adr_last, father, betree, ref_lexem) ;

  ASSERT(ref_lexem->get_lex_type() == L_REAL) ;
  T_type *type = new T_predefined_type(BTYPE_REAL,
                                                                           NULL, NULL,
                                                                           this, betree, ref_lexem) ;
  set_B_type(type, NULL) ;
  type->unlink() ;

  value = lookup_symbol(ref_lexem->get_value()) ;
}

// Constructeur a partir d'une chaine
T_literal_real::T_literal_real(const char *new_value,
                                                                                          int new_value_len,
                                                                                          T_item **adr_first,
                                                                                          T_item **adr_last,
                                                                                          T_item *father,
                                                                                          T_betree *betree,
                                                                                          T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_REAL, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE7("T_literal_real::T_literal_real(%s:%d, %x, %x, %x, %x, %x)",
                 new_value, new_value_len, adr_first, adr_last, father, betree, ref_lexem) ;

  T_type *type = new T_predefined_type(BTYPE_REAL,
                                                                           NULL, NULL,
                                                                           this, betree, ref_lexem) ;
  set_B_type(type, NULL) ;
  type->unlink() ;

  value = lookup_symbol(new_value, new_value_len) ;
}

//
//	Destructeur
//
T_literal_real::~T_literal_real(void)
{
  TRACE1("T_literal_real::~T_literal_real(%x)", this) ;  
}



//
//	}{	Constructeur de la classe T_literal_boolean
//
T_literal_boolean::T_literal_boolean(T_item **adr_first,
												T_item **adr_last,
												T_item *father,
												T_betree *betree,
												T_lexem *ref_lexem)
  : T_expr(NODE_LITERAL_BOOLEAN, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE6("T_literal_boolean(%x)::T_literal_boolean(%x, %x, %x, %x, %x)",
		 this,
		 adr_first, adr_last, father, betree, ref_lexem) ;

  value = ref_lexem->get_lex_type() == L_TRUE ? TRUE : FALSE ;
  // A FAIRE : methode get_builtin_zero, get_builtin_one pour optimiser
  // dans ce cas, enlever les unlink ci-dessous
  T_expr *zero = new T_literal_integer(0,NULL,NULL,NULL, get_betree(), ref_lexem) ;
  T_expr *one = new T_literal_integer(1, NULL,NULL,NULL, get_betree(), ref_lexem) ;

  T_type *type = new T_predefined_type(BTYPE_BOOL,
									   zero,
									   one,
									   this,
									   betree,
									   ref_lexem) ;
  zero->unlink() ;
  one->unlink() ;

  set_B_type(type, NULL) ;
  TRACE3("zero=%x, one=%x, type=%x", zero, one, type) ;
  type->unlink() ;
}

//
//	Destructeur
//
T_literal_boolean::~T_literal_boolean(void)
{
}

void T_literal_boolean::check_field_offsets()
{
    T_metaclass *meta = get_metaclass("T_literal_boolean");

    check_field_offset(meta, "value", offsetof(T_literal_boolean, value));
}

//
//	}{	Fin du fichier
//

