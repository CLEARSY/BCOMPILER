/***************************** CLEARSY **************************************
* Fichier : $Id: c_lexstk.cpp,v 2.0 2001-07-04 14:41:47 fc Exp $
* (C) 2008 CLEARSY
*
* Compilations :	*	-DDEBUG_LEX_STACK pour avoir les traces des piles
*
* Description :		Piles de l'analyseur lexical
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_lexstk.cpp,v 1.14 2001-07-04 14:41:47 fc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <unicode/utypes.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_pstack.h"

#define DEFINE_TEMPLATES
#include "s_lrstck.cpp"

// Pile des cles MD5
template class T_lr_stack<T_symbol *> ;

// Piles des lexems
//template class T_lr_stack<T_symbol *> ;
static T_lr_stack<T_lexem *> *first_lex_stack_sop = NULL ; // Pile des first_lexem
static T_lr_stack<T_lexem *> *tmp_first_lex_stack_sop = NULL ; // Pile de travail
static T_lr_stack<T_lexem *> *tmp_last_lex_stack_sop = NULL ; // Pile de travail
static T_lr_stack<T_lexem *> *last_lex_stack_sop = NULL ;  // Pile des last_lexem
// Pile des last_code_lexem
static T_lr_stack<T_lexem *> *last_code_lexem_stack_sop = NULL ;
// Cles MD5
T_lr_stack<T_symbol *> *MD5_key_stack_sop = NULL ;
T_lr_stack<T_symbol *> *expanded_MD5_key_stack_sop = NULL ;


// Donnees partagees avec s_lex.cpp
extern char *file_name_scp ;
extern UChar *load_sct_UChar ;
extern char *los_buf ;
extern T_pstack *lifo_pos_sop ;
// Fin donnees partagees avec s_lex.cpp


//
// Fonction auxiliaire de dump d'une pile
#ifdef DEBUG_LEX_STACK
static void DUMP_STACK(T_lr_stack<T_lexem *> *stack,
								const char *message)
{
  TRACE2(">> DEBUT DUMP STACK(%x) <%s>", stack, message) ;
  ENTER_TRACE ;
  int top = stack->get_nb_items() - 1 ;
  int pos = 0 ;
  while (pos <= top)
	{
	  TRACE3("#%d : %x %s",
			 pos,
			 stack->get_depth(pos),
			 stack->get_depth(pos)->get_lex_ascii()) ;
	  pos ++ ;
	}
  EXIT_TRACE ;
  TRACE2(">>  FIN  DUMP STACK(%x) <%s>", stack, message) ;
}

#else // !DEBUG_LEX_STACK
#define DUMP_STACK(x, y)
#endif // DEBUG_LEX_STACK

//
//	}{ Piles de lexemes : first
//
int is_first_lex_stack_empty(void)
{
  TRACE2("is_first_lex_stack_empty(%x) -> %s",
		 first_lex_stack_sop,
		 (first_lex_stack_sop->is_empty() == TRUE) ? "TRUE" : "FALSE") ;
  DUMP_STACK(first_lex_stack_sop, "avant is_empty") ;
  return first_lex_stack_sop->is_empty() ;
}

void new_first_lex_stack(void)
{
  first_lex_stack_sop = new T_lr_stack<T_lexem *>(STYPE_FIFO) ;
};

void delete_first_lex_stack(void)
{
  delete first_lex_stack_sop ;
  first_lex_stack_sop = NULL ;
}

// Interne, pour le contexte
static void lex_set_first_lex_stack(T_lr_stack<T_lexem *> *new_first)
{
  first_lex_stack_sop = new_first ;
}
static T_lr_stack<T_lexem *> *lex_get_first_lex_stack(void)
{
  return first_lex_stack_sop ;
}


// Empiler un element de la pile des premiers lexemes
void push_first_lex_stack(T_lexem *lexem)
{
  DUMP_STACK(first_lex_stack_sop, "avant push") ;
  TRACE6("push_first_lex_stack(%x):(%x %s:%d:%d:%s)",
		 first_lex_stack_sop,
		 lexem,
		 (lexem == NULL) ? "" : lexem->get_file_name()->get_value(),
		 (lexem == NULL) ? -1 : lexem->get_file_line(),
		 (lexem == NULL) ? -1 : lexem->get_file_column(),
		 (lexem == NULL) ? "" : lexem->get_lex_ascii()) ;
  first_lex_stack_sop->push(lexem) ;
  DUMP_STACK(first_lex_stack_sop, "apres push") ;
}

void pop_first_lex_stack(void)
{
  DUMP_STACK(first_lex_stack_sop, "avant pop") ;
  T_lexem *res = first_lex_stack_sop->pop() ;
  lex_set_first_lexem(res) ;
  TRACE6("pop_first_lex_stack(%x) -> (%x %s:%d:%d:%s)",
		 first_lex_stack_sop,
		 res,
		 (res == NULL) ? "null" : res->get_file_name()->get_value(),
		 (res == NULL) ? 0 : res->get_file_line(),
		 (res == NULL) ? 0 : res->get_file_column(),
		 (res == NULL) ? "null" : res->get_lex_ascii()) ;
  DUMP_STACK(first_lex_stack_sop, "apres pop") ;
}

//
//	}{ Piles de lexemes : last
//
void new_last_lex_stack(void)
{
  last_lex_stack_sop = new T_lr_stack<T_lexem *>(STYPE_FIFO) ;
};

void delete_last_lex_stack(void)
{
  delete last_lex_stack_sop ;
  last_lex_stack_sop = NULL ;
}

// Interne, pour le contexte
static void lex_set_last_lex_stack(T_lr_stack<T_lexem *> *new_last)
{
  last_lex_stack_sop = new_last ;
}
static T_lr_stack<T_lexem *> *lex_get_last_lex_stack(void)
{
  return last_lex_stack_sop ;
}

// Empiler un element de la pile des premiers lexemes
void push_last_lex_stack(T_lexem *lexem)
{
  TRACE5("push_last_lex_stack(%x %s:%d:%d:%s)",
		 lexem,
		 (lexem == NULL) ? "" : lexem->get_file_name()->get_value(),
		 (lexem == NULL) ? -1 : lexem->get_file_line(),
		 (lexem == NULL) ? -1 : lexem->get_file_column(),
		 (lexem == NULL) ? "" : lexem->get_lex_ascii()) ;
  last_lex_stack_sop->push(lexem) ;
}


void pop_last_lex_stack(void)
{
  T_lexem *res = last_lex_stack_sop->pop() ;
  lex_set_last_lexem(res) ;
}

//
//	}{ Piles de lexemes : tmp_first_lex
//

// Reset pile temporaire
void reset_tmp_first_lex_stack(void)
{
  tmp_first_lex_stack_sop = new T_lr_stack<T_lexem *>(STYPE_FIFO) ;
}

void delete_tmp_first_lex_stack(void)
{
  delete tmp_first_lex_stack_sop ;
  tmp_first_lex_stack_sop = NULL ;
}

// Empiler un element de la pile des premiers lexemes
void push_tmp_first_lex_stack(T_lexem *lexem)
{
#ifdef TRACE
  if (lexem != NULL)
	{
	  TRACE5("push_tmp_first_lex_stack(%x %s:%d:%d:%s)",
			 lexem,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column(),
			 lexem->get_lex_ascii()) ;
	}
  else
	{
	  TRACE0("push_tmp_first_lex_stack(NULL)") ;
	}
#endif
  tmp_first_lex_stack_sop->push(lexem) ;
}

T_lexem *pop_tmp_first_lex_stack(void)
{
  T_lexem *res = tmp_first_lex_stack_sop->pop() ;
  TRACE5("pop_tmp_first_lex_stack() -> (%x %s:%d:%d:%s)",
		 res,
		 res->get_file_name()->get_value(),
		 res->get_file_line(),
		 res->get_file_column(),
		 res->get_lex_ascii()) ;
  return res ;
}

//
//	}{ Piles de lexemes : tmp_last_lex
//

// Reset pile temporaire
void reset_tmp_last_lex_stack(void)
{
  tmp_last_lex_stack_sop = new T_lr_stack<T_lexem *>(STYPE_FIFO) ;
}

void delete_tmp_last_lex_stack(void)
{
  delete tmp_last_lex_stack_sop ;
  tmp_last_lex_stack_sop = NULL ;
}

// Empiler un element de la pile des premiers lexemes
void push_tmp_last_lex_stack(T_lexem *lexem)
{
#ifdef TRACE
  if (lexem != NULL)
	{
	  TRACE5("push_tmp_last_lex_stack(%x %s:%d:%d:%s)",
			 lexem,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column(),
			 lexem->get_lex_ascii()) ;
	}
  else
	{
	  TRACE0("push_tmp_last_lex_stack(NULL)") ;
	}
#endif
  tmp_last_lex_stack_sop->push(lexem) ;
}




//
//	}{ Piles de lexemes : last_code_lexem
//
void new_last_code_lexem_stack(void)
{
  last_code_lexem_stack_sop = new T_lr_stack<T_lexem *>(STYPE_FIFO) ;
};

void delete_last_code_lexem_stack(void)
{
  delete last_code_lexem_stack_sop ;
  last_code_lexem_stack_sop = NULL ;
}

// Interne, pour le contexte
static void lex_set_last_code_lexem_stack(T_lr_stack<T_lexem *> *new_last)
{
  last_code_lexem_stack_sop = new_last ;
}
static T_lr_stack<T_lexem *> *lex_get_last_code_lexem_stack(void)
{
  return last_code_lexem_stack_sop ;
}

// Empiler un element de la pile des premiers lexemes
void push_last_code_lexem_stack(T_lexem *lexem)
{
  TRACE5("push_last_code_lexem_stack(%x %s:%d:%d:%s)",
		 lexem,
		 (lexem == NULL) ? "" : lexem->get_file_name()->get_value(),
		 (lexem == NULL) ? -1 : lexem->get_file_line(),
		 (lexem == NULL) ? -1 : lexem->get_file_column(),
		 (lexem == NULL) ? "" : lexem->get_lex_ascii()) ;
  last_code_lexem_stack_sop->push(lexem) ;
}


void pop_last_code_lexem_stack(void)
{
  T_lexem *res = last_code_lexem_stack_sop->pop() ;
  lex_set_last_code_lexem(res) ;
}


//
// }{ Fonctions auxilaires
//

//
// POINTEURS ACTUELS

// Obtention du "premier lexeme" courant
static T_lexem *first_lexem_sop = NULL ;

void lex_reset_first_lexem(void)
{
  first_lexem_sop = NULL ;
}

T_lexem *lex_get_first_lexem(void)
{
  return first_lexem_sop ;
}

void lex_set_first_lexem(T_lexem *new_first_lexem)
{
  first_lexem_sop = new_first_lexem ;
}

// Obtention du "dernier lexeme" courant
static T_lexem *last_lexem_sop = NULL ;

void lex_reset_last_lexem(void)
{
  last_lexem_sop = NULL ;
}

T_lexem *lex_get_last_lexem(void)
{
  return last_lexem_sop ;
}

void lex_set_last_lexem(T_lexem *new_last_lexem)
{
  last_lexem_sop = new_last_lexem ;
}

// Obtention du "dernier lexeme" courant
static T_lexem *last_code_lexem_sop = NULL ;

void lex_reset_last_code_lexem(void)
{
  last_code_lexem_sop = NULL ;
}

T_lexem *lex_get_last_code_lexem(void)
{
  return last_code_lexem_sop ;
}

void lex_set_last_code_lexem(T_lexem *new_last_code_lexem)
{
  last_code_lexem_sop = new_last_code_lexem ;
}

// Obtention du "contexte MD5" courant
static MD5Context *MD5Context_sop = NULL ;

void lex_reset_MD5_context(void)
{
  MD5Context_sop = NULL ;
}

MD5Context *lex_get_MD5_context(void)
{
  return MD5Context_sop ;
}

void lex_set_MD5_context(MD5Context *new_MD5_context)
{
  MD5Context_sop = new_MD5_context ;
}

// Obtention de la "cle MD5" courante
static T_symbol *MD5Key_sop = NULL ;

void lex_reset_MD5_key(void)
{
  MD5Key_sop = NULL ;
}

T_symbol *lex_get_MD5_key(void)
{
  return MD5Key_sop ;
}

void lex_set_MD5_key(T_symbol *new_MD5_key)
{
  MD5Key_sop = new_MD5_key ;
}


//
// Path courant
//
static char *lex_path_sct = NULL ;

// Acces au "path" courant
const char *lex_get_path(void)
{
  TRACE1("lex_get_path() -> %s", lex_path_sct ? lex_path_sct : "(null)") ;
  return lex_path_sct ;
}

void lex_set_path(const char *new_path)
{
  if (lex_path_sct != NULL)
	{
	  s_free(lex_path_sct) ;
	}
  if (new_path != NULL)
	{
	  clone(&lex_path_sct, new_path) ;
	}
  else
	{
	  lex_path_sct = NULL ;
	}
}

void lex_reset_path(void)
{
  lex_path_sct = NULL ;
}

//
// COPIES DE PILE
//
// Copier la pile temporaire dans la pile
void lex_copy_tmp_first_lex_stack()
{
  delete first_lex_stack_sop ;
  first_lex_stack_sop = tmp_first_lex_stack_sop ;
  tmp_first_lex_stack_sop = NULL ;
}

// Copier la pile temporaire dans la pile
void lex_copy_tmp_last_lex_stack()
{
  delete last_lex_stack_sop ;
  last_lex_stack_sop = tmp_last_lex_stack_sop ;
  tmp_last_lex_stack_sop = NULL ;
}


//
// BASCULE PUSH_NEXT_LEXEM
//
// Lecture/Ecriture de la bascule : empiler le prochain lexeme
static int push_next_lexem_si = FALSE ;
void lex_set_push_next_lexem(int new_push_next_lexem)
{
  TRACE2("set_push_next_lexem(%s, was %s)",
		 (new_push_next_lexem == TRUE) ? "TRUE" : "FALSE",
		 (push_next_lexem_si == TRUE) ? "TRUE" : "FALSE") ;
  push_next_lexem_si = new_push_next_lexem ;
}

#ifndef DEBUG_LEX
int lex_get_push_next_lexem(void)
#else // DEBUG_LEX
int _lex_get_push_next_lexem(const char *file, int line)
#endif // !DEBUG_LEX
{
#ifdef DEBUG_LEX
  TRACE3("get_push_next_lexem() ->%s  <%s:%d>)",
		 (push_next_lexem_si == TRUE) ? "TRUE" : "FALSE",
		 file,
		 line) ;
#endif
  return push_next_lexem_si ;
}

//
// LIBERATION DES PILES DE L'ANALYSEUR LEXICAL
//
void lex_unlink_lex_stacks(void)
{
}

//
// LIBERATION DES PILES MD5
//
void lex_unlink_MD5_key_stack(void)
{
}


//
// Acces a la clause definition
//
static T_flag *definitions_clause_sop = NULL ;
static T_definition *first_definition_sop = NULL ;
static T_definition *last_definition_sop = NULL ;

T_flag *lex_get_definitions_clause(void)
{
  TRACE1("lex_get_definitions_clause() -> %x", definitions_clause_sop) ;
  return definitions_clause_sop ;
}

void lex_set_definitions_clause(T_flag *new_definitions_clause)
{
  TRACE2("lex_set_definitions_clause(%x) was %x",
		 new_definitions_clause,
		 definitions_clause_sop) ;
  definitions_clause_sop = new_definitions_clause ;
}

T_definition *lex_get_first_definition(void)
{
  return first_definition_sop ;
}

T_definition *lex_get_last_definition(void)
{
  return last_definition_sop ;
}

//
//	}{	Pile MD5
//
void lex_new_MD5_key_stack(void)
{
  MD5_key_stack_sop = new T_lr_stack<T_symbol *>(STYPE_LIFO) ;
  TRACE1("-> REALLOCATION DE MD5_key_stack_sop = %x", MD5_key_stack_sop) ;
}

void lex_delete_MD5_key_stack(void)
{
  delete MD5_key_stack_sop ;
  MD5_key_stack_sop = NULL ;
}

void *lex_get_MD5_key_stack(void)
{
  return (void *)MD5_key_stack_sop ;
}

void lex_set_MD5_key_stack(void *new_MD5_key_stack)
{
  MD5_key_stack_sop = (T_lr_stack <T_symbol *>*)new_MD5_key_stack ;
}

void lex_push_MD5_key_stack(T_symbol *new_value)
{
  MD5_key_stack_sop->push(new_value) ;
}

T_symbol *lex_pop_MD5_key_stack(void)
{
  T_symbol *res = MD5_key_stack_sop->pop() ;
  TRACE3("pop_MD5_key_stack(%x)-> %x:<%s>",
		 MD5_key_stack_sop,
		 res,
		 (res == NULL) ? "" : res->get_value()) ;
  return res ;
}

//
//	}{	Pile MD5 expansee
//
void lex_new_expanded_MD5_key_stack(void)
{
  expanded_MD5_key_stack_sop = new T_lr_stack<T_symbol *>(STYPE_LIFO) ;
  TRACE1("-> REALLOCATION DE expanded_MD5_key_stack_sop = %x",
		 expanded_MD5_key_stack_sop) ;
}

extern void lex_delete_expanded_MD5_key_stack(void)
{
  delete expanded_MD5_key_stack_sop ;
  expanded_MD5_key_stack_sop = NULL ;
}

void *lex_get_expanded_MD5_key_stack(void)
{
  return (void *)expanded_MD5_key_stack_sop ;
}

void lex_set_expanded_MD5_key_stack(void *new_expanded_MD5_key_stack)
{
  expanded_MD5_key_stack_sop = (T_lr_stack <T_symbol *>*)new_expanded_MD5_key_stack ;
}

void lex_push_expanded_MD5_key_stack(T_symbol *new_value)
{
  TRACE2("push_expanded_MD5_key_stack(%x, %x)",
		 expanded_MD5_key_stack_sop,
		 new_value) ;
  expanded_MD5_key_stack_sop->push(new_value) ;
}

T_symbol *lex_pop_expanded_MD5_key_stack(void)
{
  TRACE1("pop_expanded_MD5_key_stack(%x)", expanded_MD5_key_stack_sop) ;
  T_symbol *res = expanded_MD5_key_stack_sop->pop() ;
  TRACE3("pop_expanded_MD5_key_stack(%x)-> %x:<%s>",
		 expanded_MD5_key_stack_sop,
		 res,
		 (res == NULL) ? "" : res->get_value()) ;
  return res ;
}


//
// Gestion du contexte lexical
//
class T_lex_context
{
public :
  char *file_name ;
  char *los_buf ;
  T_pstack *lifo_pos ;
  T_lexem *first_lexem ;
  T_lexem *last_lexem ;
  T_flag *definitions_clause ;
  T_lr_stack<T_lexem *> *first_lex_stack ;
  T_lr_stack<T_lexem *> *last_lex_stack ;
  T_lr_stack<T_lexem *> *last_code_lexem_stack ;
  T_lex_context(char *new_file_name,
                                UChar *new_load,
				char *new_los_buf,
				T_pstack *new_lifo_pos_sop,
				T_lexem *new_first_lexem,
				T_lexem *new_last_lexem,
				T_flag *new_definitions_clause,
				T_lr_stack<T_lexem *> *new_first_lex_stack,
				T_lr_stack<T_lexem *> *new_last_lex_stack,
				T_lr_stack<T_lexem *> *new_last_code_lexem_stack)
	{
	  file_name = new_file_name ;
	  load = new_load ;
	  los_buf = new_los_buf ;
	  lifo_pos = new_lifo_pos_sop ;
	  first_lexem = new_first_lexem ;
	  last_lexem = new_last_lexem ;
	  definitions_clause = new_definitions_clause ;
	  first_lex_stack = new_first_lex_stack ;
	  last_lex_stack = new_last_lex_stack ;
	  last_code_lexem_stack = new_last_code_lexem_stack ;
	}
  ~T_lex_context(void) {} ;
  UChar * getLoad() { return load;}
private :
        UChar *load ;

} ;

// Sauvegarde du contexte
void *lex_push_context(void)
{
  TRACE0("lex_push_context") ;

  void *res =  new T_lex_context(file_name_scp,
                                                                 load_sct_UChar,
								 los_buf,
								 lifo_pos_sop,
								 lex_get_first_lexem(),
								 lex_get_last_lexem(),
								 lex_get_definitions_clause(),
								 lex_get_first_lex_stack(),
								 lex_get_last_lex_stack(),
								 lex_get_last_code_lexem_stack()) ;
  TRACE1("ctx = %x", res) ;
  return res ;

}

// Restauration du contexte
void lex_pop_context(void *ctx)
{
  TRACE1("lex_pop_context(%x)", ctx) ;

  T_lex_context *load = (T_lex_context *)ctx ;

  file_name_scp = load->file_name ;
  load_sct_UChar = load->getLoad() ;
  los_buf = load->los_buf ;
  lifo_pos_sop = load->lifo_pos ;
  lex_set_first_lexem(load->first_lexem) ;
  lex_set_last_lexem(load->last_lexem) ;
  lex_set_definitions_clause(load->definitions_clause) ;
  lex_set_first_lex_stack(load->first_lex_stack) ;
  lex_set_last_lex_stack(load->last_lex_stack) ;
  lex_set_last_code_lexem_stack(load->last_code_lexem_stack) ;

  delete load ;
}

// Liberation du contexte courant
void lex_unlink_current_context(void)
{
  TRACE0("lex_unlink_current_context") ;

  delete [] file_name_scp ;
  delete [] load_sct_UChar ;
  delete [] los_buf ;
  delete lifo_pos_sop ;
  file_name_scp = NULL ;
  load_sct_UChar = NULL ;
  los_buf = NULL ;
  lifo_pos_sop = NULL ;
}


//
// }{	Fin du fichier
//
