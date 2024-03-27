/******************************* CLEARSY **************************************
* Fichier : $Id: c_pralex.cpp,v 2.0 1999-06-09 12:38:27 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyseur lexical de pragmas Lexemes
*
* Compilation  : 	-DDEBUG_COMMENT pour voir les commentaires
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
RCS_ID("$Id: c_pralex.cpp,v 1.6 1999-06-09 12:38:27 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Debug lexemes
static const char *debug_lex_type[] =
{
  /* 000 */
  "PL_IDENT",
  /* 001 */
  "PL_PRAGMA",
  /* 002 */
  "PL_OPEN_PAREN",
  /* 003 */
  "PL_CLOSE_PAREN",
  /* 004 */
  "PL_COMMA",
  /* 005 */
  "PL_STRING",
  /* 006 */
  "PL_GARBAGE",
} ;

//
//	}{	Constructeur
//
T_pragma_lexem::T_pragma_lexem(T_pragma_lex_type new_lex_type,
										T_comment *new_ref_comment,
										const char *new_value,
										size_t new_value_len,
										int new_file_line,
										int new_file_column)
  : T_object(NODE_PRAGMA_LEXEM)
{
#ifdef DEBUG_COMMENT
  TRACE4("%s:%d:%d: T_pragma_lexem::T_pragma_lexem(%s)\n",
		 (new_ref_comment->get_betree() == NULL)
		 ? "*lex-analysis*" :
		 new_ref_comment->get_betree()->get_file_name()->get_value(),
		 new_file_line,
		 new_file_column,
		 debug_lex_type[new_lex_type]) ;
#endif
  set_tmp2(FALSE) ;

  ENTER_TRACE ;
  ref_comment = new_ref_comment ;
  lex_type = new_lex_type ;
  file_line = new_file_line ;
  file_column = new_file_column ;

  ASSERT(file_column > 0) ;

  if (new_value == NULL)
	{
	  value_len = 0 ;
	  value = NULL ;
	}
  else
	{
	  clone(&value, new_value) ;
	  value_len = new_value_len ;
	}

#ifdef DEBUG_COMMENT
  TRACE5("T_pragma_lexem(%x) : lex_type %d<%s> value %d<%s>",
		 this,
		 lex_type,
		 debug_lex_type[lex_type],
		 value_len,
		 (value == NULL) ? "null" : value) ;
#endif // DEBUG_COMMENT

  if (first_lexem == NULL)
	{
	  first_lexem = this ;
	}
  else
	{
	  last_lexem->next_lexem = this ;
	}

  prev_lexem = last_lexem ;
  next_lexem = NULL ;
  last_lexem = this ;

  EXIT_TRACE ;
#ifdef DEBUG_COMMENT
  TRACE0("fin de T_pragma_lexem::T_pragma_lexem") ;
#endif
}

//
//	}{	Destructeur
//
T_pragma_lexem::~T_pragma_lexem(void)
{
#ifdef DEBUG_COMMENT
  TRACE1("debut T_pragma_lexem(%x)::~T_pragma_lexem()", this) ;
  ENTER_TRACE ;
#endif

  free(value) ;

#ifdef DEBUG_COMMENT
  EXIT_TRACE ;
  TRACE1("fin T_pragma_lexem(%x)::T_pragma_lexem()", this) ;
#endif
}

#ifdef DUMP_LEXEMS
//
//	]{	Dump
//
void T_pragma_lexem::dump_lexems(void)
{
#ifdef FULL_TRACE
  TRACE1("dump_lexems(%x)", this) ;
#endif

  if (value == NULL)
	{
	  fprintf(stdout,"%s:%d:%d: Lexem %x type = %03d :: %s\n",
			  get_file_name(),
			  file_line,
			  file_column,
			  (size_t)this,
			  lex_type,
			  debug_lex_type[lex_type]) ;
	  TRACE5("%s:%d:%d: Lexem type = %03d :: %s\n",
			 get_file_name(),
			 file_line,
			 file_column,
			 lex_type,
			 debug_lex_type[lex_type]) ;
	}
  else
	{
	  fprintf(stdout,"%s:%d:%d: Lexem %x type = %03d :: %s value = %x \"%s\"\n",
			  get_file_name(),
			  file_line,
			  file_column,
			  (size_t)this,
			  lex_type,
			  debug_lex_type[lex_type],
			  (size_t)value,
			  value) ;
	  TRACE6("%s:%d:%d: Lexem type = %03d :: %s value = \"%s\"\n",
			 get_file_name(),
			 file_line,
			 file_column,
			 lex_type,
			 debug_lex_type[lex_type],
			 value) ;
	}

  T_pragma_lexem *after = next_lexem ;

  if (after != NULL)
	{
	  after->dump_lexems() ;
	}
}
#endif

//
// }{ Obtention du nom de lexeme a partir d'un lex_type
// exemple : get_pragma_lex_type_name(PL_IDENT) -> PL_IDENT"
const char *get_pragma_lex_type_name(T_pragma_lex_type lex_type)
{
return debug_lex_type[lex_type] ;
}

