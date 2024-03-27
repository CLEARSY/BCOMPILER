/******************************* CLEARSY **************************************
* Fichier : $Id: c_lexstk.h,v 2.0 2001-06-13 12:55:28 fl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des methodes statiques l'analyseur lexical
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
#ifndef _C_LEXSTK_H_
#define _C_LEXSTK_H_

#ifdef __BCOMP__
// Pile des cles MD5e
//extern void push_MD5_key_stack(T_symbol *new_value) ;
//extern T_symbol *pop_MD5_key_stack(void) ;

// Pile des cles etendues MD5e
extern void push_expanded_MD5_key_stack(T_symbol *new_value) ;
extern T_symbol *pop_expanded_MD5_key_stack(void) ;

// La pile des premiers lexemes est-elle vide ?
extern int is_first_lex_stack_empty(void) ;

// Creer une nouvelle pile
extern void new_first_lex_stack(void) ;
extern void new_last_lex_stack(void) ;
extern void new_last_code_lexem_stack(void) ;

// Supprimer une ancienne pile
extern void delete_first_lex_stack(void) ;
extern void delete_last_lex_stack(void) ;
extern void delete_last_code_lexem_stack(void) ;

// Depiler un element de la pile des premiers/derniers lexemes
// Les elements sont accessibles par l'api lex_get_first...
extern void pop_first_lex_stack(void) ;
extern void pop_last_lex_stack(void) ;
extern void pop_last_code_lexem_stack(void) ;

// Empiler un element de la pile des premiers/derniers lexemes
extern void push_first_lex_stack(T_lexem *lexem) ;
extern void push_last_lex_stack(T_lexem *lexem) ;
extern void push_last_code_lexem_stack(T_lexem *lexem) ;

// Acces au "premier lexeme" courant
extern void lex_reset_first_lexem(void) ;
extern T_lexem *lex_get_first_lexem(void) ;
extern void lex_set_first_lexem(T_lexem *new_first_lexem) ;

// Acces au "dernier lexeme" courant
extern void lex_reset_last_lexem(void) ;
extern T_lexem *lex_get_last_lexem(void) ;
extern void lex_set_last_lexem(T_lexem *new_last_lexem) ;

// Acces au "dernier lexeme de code" courant
extern void lex_reset_last_code_lexem(void) ;
extern T_lexem *lex_get_last_code_lexem(void) ;
extern void lex_set_last_code_lexem(T_lexem *new_last_code_lexem) ;

// Acces a la clause definition
extern T_flag *lex_get_definitions_clause(void) ;
extern T_definition *lex_get_first_definition(void) ;
extern T_definition *lex_get_last_definition(void) ;
extern void lex_set_definitions_clause(T_flag *new_definitions_clause) ;

// Acces au contexte MD5 courant
extern void lex_reset_MD5_context(void) ;
extern MD5Context *lex_get_MD5_context(void) ;
extern void lex_set_MD5_context(MD5Context *new_MD5_context) ;

// Acces au contexte MD5 courant
extern void lex_reset_MD5_key(void) ;
extern T_symbol *lex_get_MD5_key(void) ;
extern void lex_set_MD5_key(T_symbol *new_MD5_key) ;

// Acces au "path" courant
extern const char *lex_get_path(void) ;
extern void lex_set_path(const char *new_path) ;

extern void lex_reset_path(void) ;

// Depiler un element de la pile des premiers lexemes
extern T_lexem *pop_tmp_first_lex_stack(void) ;

// Empiler un element de la pile des premiers lexemes
extern void push_tmp_first_lex_stack(T_lexem *lexem) ;

// Empiler un element de la pile des derniers lexemes
extern void push_tmp_last_lex_stack(T_lexem *lexem) ;

// Reset pile temporaire
extern void reset_tmp_first_lex_stack(void) ;
extern void reset_tmp_last_lex_stack(void) ;

// Supprimer pile temporaire
extern void delete_tmp_first_lex_stack(void) ;
extern void delete_tmp_last_lex_stack(void) ;

// Copier la pile temporaire dans la pile
extern void lex_copy_tmp_first_lex_stack() ;
extern void lex_copy_tmp_last_lex_stack() ;

// Lecture/Ecriture de la bascule : empiler le prochain lexeme
extern void lex_set_push_next_lexem(int new_push_next_lexem) ;
#ifndef DEBUG_LEX
extern int lex_get_push_next_lexem(void) ;
#else // DEBUG_LEX
#define lex_get_push_next_lexem() _lex_get_push_next_lexem(__FILE__, __LINE__)
extern int _lex_get_push_next_lexem(const char *file, int line);
#endif // !DEBUG_LEX

// Gestion du premier lexeme de code du fichier
extern void reset_first_compo_code_lexem(void) ;
extern void set_first_compo_code_lexem(T_lexem *new_first_compo_code_lex);
extern T_lexem *get_first_compo_code_lexem(void) ;

// Liberation d'un flux de lexemes. Le parametre inside_expansion est
// utilise lors de la liberation recursive de l'expansion d'une
// definition
extern void unlink_lexems(T_lexem *cur_lexem,
								   int inside_expansion = FALSE) ;



// Pile MD5 lexicale
extern void lex_new_MD5_key_stack(void) ;
extern void lex_delete_MD5_key_stack(void) ;
extern void *lex_get_MD5_key_stack(void) ;
extern void lex_set_MD5_key_stack(void *new_MD5_key_stack) ;
extern void lex_push_MD5_key_stack(T_symbol *new_value) ;
extern T_symbol *lex_pop_MD5_key_stack(void) ;

// Pile expanded_MD5 lexicale
extern void lex_new_expanded_MD5_key_stack(void) ;
extern void lex_delete_expanded_MD5_key_stack(void) ;
extern void *lex_get_expanded_MD5_key_stack(void) ;
extern void
	lex_set_expanded_MD5_key_stack(void *new_expanded_MD5_key_stack) ;
extern void lex_push_expanded_MD5_key_stack(T_symbol *new_value) ;
extern T_symbol *lex_pop_expanded_MD5_key_stack(void) ;


// Liberation des piles de l'analyseur lexical
extern void lex_unlink_lex_stacks(void) ;
extern void lex_unlink_MD5_key_stack(void) ;
//extern void *get_MD5_key_stack(void) ;
//extern void set_MD5_key_stack(void *new_MD5_key_stack) ;
//extern void *get_expanded_MD5_key_stack(void) ;
//extern void set_expanded_MD5_key_stack(void *new_expanded_MD5_key_stack) ;
#endif // __BCOMP__

#endif /* _C_LEXSTK_H_ */
