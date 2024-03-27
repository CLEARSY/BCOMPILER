/******************************* CLEARSY **************************************
* Fichier : $Id: c_pralex.h,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de l'analyseur lexical de pragmas
*					Definition des lexemes de type pragma
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
#ifndef _C_PRALEX_H_
#define _C_PRALEX_H_

#include "c_lextyp.h"
#include "c_object.h"
#include "c_item.h"
#include "c_mach.h"
#include "c_betree.h"
#include "c_defini.h"

// Fonction qui saute les commentaires a partir de cur_lexem et
// renvoie le premier lexeme non commentaire
// Si cur_lexem == NULL, renvoie NULL
class T_lexem ;
class T_item ;
class T_betree ;

// Obtention du nom de lexeme a partir d'un lex_type
// exemple : get_pragma_lex_type_name(PL_IDENT) -> "PL_IDENT"
const char *get_pragma_lex_type_name(T_pragma_lex_type lex_type) ;

// Definition d'un lexeme

class T_pragma_lexem : public T_object
{
  // Methodes et fonctions amies
friend T_pragma_lexem *pragma_lex_analysis(T_comment *ref_comment) ;
friend void pragma_lex_unload(void) ;
friend void pragma_analyse_string(T_comment *ref_comment) ;

  // Type du lexeme
  T_pragma_lex_type				lex_type ;

  // Valeur du lexeme sous forme de chaine de caracteres
  // Contenu valide ssi lex_type vaut L_IDENT, L_COMMENT
  // L_STRING ou L_NUMBER
  char 							*value ;
  size_t  			 			value_len ;

  // Commentaire de provenance
  T_comment					*ref_comment ;

  // Numero de ligne et de colonne du lexeme
  int						file_line ;
  int						file_column ;

  // Lexemes suivants et precedents dans le fichier
  T_pragma_lexem	   		*next_lexem ;
  T_pragma_lexem	   		*prev_lexem ;

  //
  // VARIABLES DE CLASSE
  // EXISTENCE TEMPORAIRE, Information a rechercher dans le Betree
  // DES LA FIN DE L'ANALYSE SYNTAXIQUE
  //
  // Lexemes
  static T_pragma_lexem    	*first_lexem ;
  static T_pragma_lexem    	*last_lexem ;

  // dernier lexeme non commentaire
  static T_pragma_lexem    	*last_code_lexem ;

public :

  // Constructeur
  T_pragma_lexem() : T_object(NODE_PRAGMA_LEXEM) {} ;
  T_pragma_lexem(T_pragma_lex_type new_pragma_lex_type,
				 T_comment *ref_comment,
				 const char *new_value,
				 size_t new_value_len,
				 int new_file_line,
				 int new_file_column) ;

  // Destructeur
  virtual ~T_pragma_lexem() ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_pragma_lexem" ; } ;
  T_pragma_lex_type get_lex_type(void) 		{ return lex_type ; } ;
  const char *get_value(void) 	   	{ return value ; } ;
  size_t get_value_len(void)		{ return value_len ; } ;
  int get_file_line(void)	   		{ return file_line ; } ;
  int get_file_column(void)	   		{ return file_column ; } ;
  T_pragma_lexem *get_next_lexem(void) { return next_lexem ; } ;
  T_comment *get_ref_comment(void) { return ref_comment ; } ;

  // Methodes d'ecriture

  //
  //	AUTRES METHODES
  //

  // renvoie le nom ou le type
  const char *get_lex_name(void) ;

  // Dump des lexemes
  void dump_lexems(void) ;

  // Methode de dump HTML
  FILE *dump_html(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Interface de l'analyseur lexical : chargement d'un commentaire
extern T_pragma_lexem *pragma_lex_analysis(T_comment *ref_comment) ;

// Liberation de l'analyseur lexical
extern void pragma_lex_unload(void) ;

#endif /* _C_PRALEX_H_ */
