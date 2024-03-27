/******************************* CLEARSY **************************************
* Fichier : $Id: s_synta.h,v 2.0 1997-12-12 15:42:17 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de l'analyseur syntaxique
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
#ifndef _S_SYNTA_H_
#define _S_SYNTA_H_

// Analyse lexicale
extern T_betree *syntax_analysis(T_lexem *first_lexem,
										  const char *new_file_name,
										  const char *new_betree_name) ;

// Verifie que l'on n'est pas à la fin du fichier
// Si c'est le cas, produit un message d'erreur fatale
// cur_item est l'objet en cours d'analyse lexicale
#ifndef FULL_TRACE
extern T_lexem *syntax_check_eof(T_lexem *cur_lexem) ;
#else
extern T_lexem *_syntax_check_eof(const char *file,
										   int line,
										   T_lexem *cur_lexem) ;
#define syntax_check_eof(x) _syntax_check_eof(__FILE__, __LINE__, x) ;
#endif
#endif /* _S_SYNTA_H_ */

