/******************************* CLEARSY **************************************
* Fichier : $Id: m_lexsta.h,v 2.0 1999-10-08 11:33:32 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : analyseur lexical du fichier d'etat d'un composant
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
#ifdef  ACTION_NG

#ifndef _M_LEXSTA_H_
#define _M_LEXSTA_H_

#include "m_lextyp.h"

//definition d'un element de la table des symboles des lexemes
typedef struct S_symbol_lex_type T_symbol_lex_type ;
struct S_symbol_lex_type
	{
	/* Symbole correspondant au lexeme */
		T_symbol *lexem_symb ;

	/* Type de lexeme associe */
		T_state_lex_type lexem_type ;
	} ;

//rend le numero de ligne courante
extern int get_curline(void) ;

//initialisation des symboles du fichier d'état
extern void init_state_symbols(void) ;

//chargement d'un fichier d'etat et positionnnement premier lexeme
// Renvoie TRUE si ok, FALSE sinon
extern int load_state_file(const char *state_file_name) ;

//passage au lexeme suivant : met a jour le type et la valeur du lexeme
extern void state_next_lexem(void) ;

//recuperation du type du lexeme courant
extern T_state_lex_type get_state_lex_type(void) ;

//recuperation de la valeur du lexeme courant
//s'il s'agit d'un nombre (checksum) ou d'un identificateur
extern T_symbol *get_state_lex_value(void) ;

//déchargement du fichier d'etat
extern void unload_state_file(void) ;


#endif /* _M_LEXSTA_H_ */

#endif /* ACTION_NG */
