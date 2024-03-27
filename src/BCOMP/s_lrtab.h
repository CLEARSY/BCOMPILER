/******************************* CLEARSY **************************************
* Fichier : $Id: s_lrtab.h,v 2.0 1997-07-08 07:44:59 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Tables de l'analyseur LR
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
#ifndef _S_LRTAB_H_
#define _S_LRTAB_H_

// Familles de lexemes
typedef enum
	{
	FAMILY_ATOM,		// atomes : identificateurs, entiers, booleens
	FAMILY_UNR,			// operateurs unaires
	FAMILY_BIN,			// operateurs binaires
	FAMILY_OPN,			// lexemes ouvrants
	FAMILY_CLO,			// lexemes fermants
	FAMILY_PST,			// tilde
	FAMILY_END			// fin de fichier ou lexemes inconnus pour l'analyseur LR
	} T_lexem_family ;

// Associativite des lexemes
typedef enum
	{
	ASSOS_LEFT,				// associatif a gauche
	ASSOS_RIGHT,			// associatif a droite
	ASSOS_ERROR				// pas d'associativite
	} T_lexem_associativity ;

// Priorite "pas de priorite"
extern const int ERROR_NO_PRIO ;

// Obtention de la famille d'un lexem
extern T_lexem_family get_lexem_family(T_lexem *cur_lexem) ;

//	Obtention de la priorite d'un lexeme
extern int get_lexem_prio(T_lexem *cur_lexem) ;

//	Obtention de l'associativite d'un lexeme
extern T_lexem_associativity get_lexem_assoc(T_lexem *cur_lexem) ;

//
//	}{	Dump des tables au format LaTeX 2e
//
extern void latex2e_dump_LR_tables(const char *file_name,
											const char *output_path) ;

//
//	}{	Dump des tables au format HTML
//
extern void html_dump_LR_tables(const char *file_name,
										 const char *output_path) ;

#endif /* _S_LRTAB_H_ */


