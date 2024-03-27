/******************************* CLEARSY **************************************
* Fichier : $Id: c_symb.h,v 2.0 2001-07-19 16:24:28 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des symboles
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
#ifndef _C_SYMB_H_
#define _C_SYMB_H_

#if defined(__cplusplus)

#include <iostream>
#include "c_object.h"
//#include "c_lextyp.h"

class T_symbol : public T_object
{
  // Chaine
  char 			*value ;
  size_t   		len ;

  // Chainage dans la table
  // On utilise un chainage specifique par taille de chaine croissante
  // Pour cette raison, on n'utilise pas les champs herites de T_item
  T_symbol		*next_symb ;

protected :
  T_symbol(T_node_type new_node_type) : T_object(new_node_type) {}
public :
  T_symbol() : T_object(NODE_SYMBOL) {}
  T_symbol(const char *new_value,
		   size_t new_len,
		   int h,
		   T_symbol *next_symbol,
		   T_symbol *prev_symbol) ;
  virtual ~T_symbol() ;

  // Methodes d'acces
  T_symbol *get_next_symb(void)	{ return next_symb ; }
  const char *get_value(void)	{ return value ; }
  char **get_adr_value(void) { return &value ; }
  size_t get_len(void)			{ return len ; }

  // Passage en mode persistant
  void set_persistent(void) ;

  // Comparaison de nom avec une chaine
  // Renvoie TRUE si les noms sont les memes, FALSE sinon
  int compare(const char *ref_value, size_t ref_value_len)
	{
#if 0
	  TRACE5("compare(this <%x:%s:%d>, ref <%s:%d>)",
			 this,
			 value,
			 len,
			 ref_value,
			 ref_value_len) ;
#endif

	  return ( 	(len == ref_value_len)
				&& (strcmp(value, ref_value) == 0) ) ;
	}

  // Comparaison de nom avec un autre symbole
  // Renvoie TRUE si les noms sont les memes, FALSE sinon
  // A FAIRE :: quand la question du save/restore de la tsymb
  // sera elucide, voir si un = ne suffit pas
  int compare(const T_symbol *ref)
	{
#if 0
	  TRACE6("compare(this <%x:%s:%d>, ref <%x:%s:%d>)",
			 this,
			 value,
			 len,
			 ref,
			 ref->value,
			 ref->len) ;
#endif

	  return ( 	(len == ref->len)
				&& (strcmp(value, ref->value) == 0) ) ;
	}

  //ajout JFM pour correction BUG 2218
  // Les fonctions suivante comparent les valeurs de deux symboles
  // et rendent respectivement TRUE (la notion d'ordre est au sens
  // chaine de caracteres)
  // - si le symbole est superieur (strictement) (ref ==NULL ;ref= + infini)
  // - si il est superieur ou egal (ref ==NULL ;ref= + infini)
  // - si il est inferieur (strictement) (ref ==NULL ;ref= - infini)
  // - si il est inferieur ou egal (ref ==NULL ;ref= - infini)
  // FALSE sinon
  int is_greater_than(T_symbol *ref)
	{
	  if(ref == NULL)
		{
		  return FALSE;
		}
	  return (strcmp(value,ref->get_value()) > 0) ? TRUE : FALSE;
	}
  int is_greater_than_or_equal(T_symbol *ref)
	{
	  if(ref == NULL)
		{
		  return FALSE;
		}
	  return (strcmp(value,ref->get_value()) >= 0) ? TRUE : FALSE;
	}
  int is_less_than(T_symbol *ref)
	{
	  if(ref == NULL)
		{
		  return FALSE;
		}
	  return (strcmp(value,ref->get_value()) < 0) ? TRUE : FALSE;
	}
  int is_less_than_or_equal(T_symbol *ref)
	{
	  if(ref == NULL)
		{
		  return FALSE;
		}
	  return (strcmp(value,ref->get_value()) <= 0) ? TRUE : FALSE;
	}

  // Methode de dump HTML


  virtual FILE *dump_html(void) ;

  // Obtention du nom de la classe
  virtual const char *get_class_name(void) { return "T_symbol" ; } ;

  // Les symboles sont des symboles
  virtual int is_a_symbol(void) { return TRUE ; } ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;

  friend std::ostream &operator<<(std::ostream &os, const T_symbol &sub);
  friend std::istream &operator>>(std::istream &is, T_symbol &sub);
} ;


//
//	}{	Fonction d'initialisation/liberation
//
extern void init_symbol_table(void) ;
extern void unlink_symbol_table(void) ;
// Passage de tous les symboles en mode persistant
extern void fix_symbol_table(void) ;


//
//	}{	Recherche d'une chaine avec eventuellement creation
//
extern T_symbol *lookup_symbol(const char *new_value, size_t new_len) ;
extern T_symbol *lookup_symbol(const char *new_value) ;

//
//	}{	Ajout d'un mot cle
//
class T_keyword ;
extern T_keyword *insert_builtin(T_lex_type new_lex_type,
										  const char *new_value) ;

//
//	}{	Fonction qui donne renvoie TRUE ssi une chaine est un mot-cle, FALSE
//		sinon. Si le resultat est true, lex_type est rempli avec le lex_type
//		du mot-cle
extern	int lookup_builtin(const char *string,
									int string_len ,
									T_lex_type &lex_type) ;
#ifdef FULL_TRACE
//
//	}{	Dump complet de la table des symboles
//
void dump_symtab(void) ;


#endif

//
// Methodes auxilaires de gestion d'une liste de noms sans doublons
//
// Recherche un element dans une liste avec ajout si absent
// Initialement il faut que *adr_first = *adr_last = NULL
extern T_symbol *list_lookup(T_symbol *new_symbol,
									  T_list_link **adr_first,
									  T_list_link **adr_last) ;
#endif // __cplusplus

// C-Wrapper pour list_lookup
// Initialement il faut que adr_first = adr_last = NULL :
// void *first = NULL
// void *last = NULL
// C_list_lookup(new_char, &first, &last) ;
EXTERN char *C_list_lookup(char *new_char,
									void **adr_first,
									void **adr_last) ;


#endif /* _C_SYMB_H_ */

