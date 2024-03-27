/******************************* CLEARSY **************************************
* Fichier : $Id: c_out.cpp,v 2.0 2002-07-12 13:05:53 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Gestionnaire de messages
*
* Compilations :
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
RCS_ID("$Id: c_out.cpp,v 1.41 2002-07-12 13:05:53 cm Exp $") ;
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

/* Includes Composant Local */
#include "c_api.h"

// Nombre d'erreurs deja rencontrees
static int error_count_si = 0 ;
static int warning_count_si = 0 ;



// Gestion du nom d'outil
// Par defaut : bcomp
static T_symbol *tool_name_sop = NULL ;
void set_tool_name(const char *tool_name)
{
  TRACE1("set_tool_name(%s)", tool_name) ;
  tool_name_sop = lookup_symbol(tool_name) ;
}

T_symbol *get_tool_name(void)
{
  return tool_name_sop ;
}

// Choix de la destination des messages
// Par defaut c'est MSG_DEST_STANDARD_STREAMS
static T_msg_destination msg_destination_si = MSG_DEST_STANDARD_STREAMS ;
void set_msg_destination(T_msg_destination new_msg_destination)
{
  msg_destination_si = new_msg_destination ;
}

// Buffer du gestionnaire
static const int BUFFER_SIZE = 64*1024 ;
static char msg_buffer_sct[BUFFER_SIZE] ;
char *get_msg_buffer(void)
{
  return msg_buffer_sct ;
}

// Ajout d'un "\n" en fin du buffer
void msg_buffer_add_new_line(void)
{
  char *p = (char *)((size_t)get_msg_buffer() + strlen(get_msg_buffer())) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;
}

// Onbtention du flux correspondant a msg_stream
static FILE *msg_stream_fd_sot[] =
{
  // MSG_ERROR_STREAM
  stderr,
  //MSG_NOMINAL_STREAM
  stdout
} ;

static inline FILE *get_msg_stream_fd(T_msg_stream msg_stream)
{
  return msg_stream_fd_sot[msg_stream] ;
}

// Ligne courante
static T_msg_line *cur_line_sop = NULL ;

// Creation d'une ligne
static void new_msg(T_msg_stream msg_stream)
{
  if (msg_destination_si != MSG_DEST_STANDARD_STREAMS)
	{
	  TRACE0("new_msg") ;
	  cur_line_sop = get_msg_line_manager()->create_line(msg_stream) ;
	}
}

static void LOCATE(T_msg_stream msg_stream, T_lexem *x)
{
  if (x == NULL)
	{
	  locate_msg(msg_stream) ;
	}
  else
	{
	  locate_msg(msg_stream,
				 x->get_file_name()->get_value(),
				 x->get_file_line(),
				 x->get_file_column()) ;
	}
}

static void LOCATE_OUT(T_lexem *x)
{
  LOCATE(MSG_NOMINAL_STREAM, x) ;
}

static void LOCATE_ERR(T_lexem *x)
{
  LOCATE(MSG_ERROR_STREAM, x) ;
}

// Affichage d'un positionnement
// version fichier/ligne/colonne
void locate_msg(T_msg_stream msg_stream,
						 const char *file_name,
						 int file_line,
						 int file_column)
{
  TRACE4("locate_msg(%d, %s:%d:%d)", msg_stream, file_name, file_line, file_column);
  if (msg_destination_si != MSG_DEST_BUFFERS)
	{
	  // Affichage en direct
	  fprintf(get_msg_stream_fd(msg_stream),
			  "%s:%d:%d: ",
			  file_name,
			  file_line,
			  file_column) ;
	}

  if (msg_destination_si != MSG_DEST_STANDARD_STREAMS)
	{
	  ASSERT(cur_line_sop != NULL) ;
	  cur_line_sop->set_position(lookup_symbol(file_name),
								 file_line,
								 file_column) ;
	}
}

// version fichier/ligne/colonne
void locate_msg(T_msg_stream msg_stream)
{
  if (msg_destination_si != MSG_DEST_BUFFERS)
	{
	  // Affichage en direct
          fprintf(get_msg_stream_fd(msg_stream), "%s", get_catalog(C_AT_TOPLEVEL)) ;
	}

}


// Prise en compte du message dans le buffer (affichage immediat et/ou
// ajout dans les buffers)
void deliver_msg(T_msg_stream msg_stream)
{
  if (msg_destination_si != MSG_DEST_BUFFERS)
	{
	  // Affichage en direct
          fprintf(get_msg_stream_fd(msg_stream), "%s", msg_buffer_sct) ;
	}


  if (msg_destination_si != MSG_DEST_STANDARD_STREAMS)
	{
	  ASSERT(cur_line_sop != NULL) ;
	  cur_line_sop->set_contents(lookup_symbol(msg_buffer_sct)) ;
	}

}


//
//	Parametrage
//
static int extended_warnings_sb = FALSE ;
static int enable_warnings_sb = TRUE ;

// Permet tous les warnings
void compiler_enable_extended_warnings(void)
{
  extended_warnings_sb = TRUE ;
}

// Supprime tous le warnings
void compiler_disable_warnings(void)
{
  enable_warnings_sb = FALSE ;
}

// Obtention du nombre d'erreurs deja rencontrees
int get_error_count(void)
{
  TRACE1("get_error_count()->%d", error_count_si) ;
  return error_count_si ;
}

// Reset du compteur d'erreurs
void reset_error_count(void)
{
  error_count_si = 0 ;
}

// Obtention du nombre d'erreurs deja rencontrees
int get_warning_count(void)
{
  TRACE1("get_warning_count()->%d", warning_count_si) ;
  return warning_count_si ;
}

// Reset du compteur d'erreurs
void reset_warning_count(void)
{
  warning_count_si = 0 ;
}

// Pour savoir s'il faut afficher un warning
static inline int do_warning(T_warning_level warning_level)
{
  if (enable_warnings_sb == FALSE)
	{
	  return FALSE ;
	}

  if (warning_level == EXTENDED_WARNING)
	{
	  return extended_warnings_sb ;
	}

  return TRUE ;
}

// Mode verbeux ? Non par defaut !
static int verbose_mode_si = FALSE ;
// Passage en mode verbeux
void set_verbose_mode(void)
{
  verbose_mode_si = TRUE ;
}

// Description verbeuse
void verbose_describe(const char *format, ...)
{
  if (verbose_mode_si == FALSE)
	{
	  return ;
	}

  va_list ap ;

  va_start(ap, format) ;

  vfprintf(stdout,
		   format,
		   ap) ;

  fflush(stdout) ;
  va_end(ap) ;
}

// Fonction qui regarde s'il y a eu des erreurs
// Si c'est le cas, elle renvoie FALSE. Sinon elle renvoie TRUE
int check_errors(void)
{
  TRACE2("check_errors(error_count = %d) -> %s",
		 get_error_count(),
		 (get_error_count() == 0) ? "TRUE" : "FALSE") ;
  if (get_error_count() != 0)
	{
	  toplevel_error(CAN_CONTINUE, get_error_msg(E_CONFUSED), get_error_count()) ;
	  return FALSE ;
	}

  return TRUE ;
}

//
// }{ Interface de production des messages
//
//
// Erreur de syntaxe
//
void syntax_error(T_lexem *lexem,
						   T_error_level error_level,
						   const char *format,
						   ...)
{
  TRACE2("syntax_error(lexem %x, original_lexem %x)",
		 lexem,
		 (lexem == NULL) ? NULL : lexem->get_original_lexem()) ;
  va_list ap ;

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  T_lexem *definition = NULL ;

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;
  TRACE1("lexem->get_file_name = %x", lexem->get_file_name()) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

//
// Erreur de syntaxe (pour un pragma_lexem)
//
void syntax_error(T_pragma_lexem *plexem,
						   T_error_level error_level,
						   const char *format,
						   ...)
{
  va_list ap ;

  T_lexem *lexem = plexem->get_ref_comment()->get_ref_lexem() ;

  TRACE2("lexem = %x format=\"%s\"", lexem, format) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

void parse_error(const char *file,
						  int line,
						  int column,
						  T_error_level error_level,
						  const char *format,
						  ...)
{
  va_list ap ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM, file, line, column) ;

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;
  TRACE1("buffer = \"%s\"", get_msg_buffer()) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

void toplevel_error(T_error_level error_level,
							 const char *format,
							 ...)
{
  va_list ap ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM) ;
  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

void lex_warning(const char *file,
						  int line,
						  int column,
						  T_warning_level warning_level,
						  const char *format,
						  ...)
{
  TRACE4("lex_warning(%s, %d, %d, %d)",
		 file, line, column, warning_level) ;

  if (do_warning(warning_level) == TRUE)
	{
	  new_msg(MSG_ERROR_STREAM) ;
	  locate_msg(MSG_ERROR_STREAM, file, line, column) ;
	  va_list ap ;
	  va_start(ap, format) ;
	  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
	  ASSERT(len < BUFFER_SIZE) ;
#endif
	  va_end(ap) ;

	  char *p = (char *)((size_t)get_msg_buffer() + len) ;
	  *p = '\n' ;
	  *(p + 1) = '\0' ;

	  deliver_msg(MSG_ERROR_STREAM) ;

	  warning_count_si ++ ;
	}
}


// Production d'un message
void print(const char *format, ...)
{
  va_list ap ;
  TRACE0("betree_print") ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif

  va_end(ap) ;

  deliver_msg(MSG_NOMINAL_STREAM) ;
}

// Arret immediat
void stop(void)
{
  TRACE0("stop") ;

  if (get_exit_handler() != NULL)
	{
	  // On incremente le compteur d'erreurs pour prevenir l'appelant
	  error_count_si ++ ;
	  //On retourne a l'appelant
	  TRACE0("on retourne a l'appelant") ;

	  longjmp(*get_exit_handler(), 1) ;
	}
  fprintf(stderr, "%s", tool_name_sop->get_value()) ;
  fprintf(stderr, "%s", get_catalog(C_ABORTING)) ;

  exit(1) ;
}

// Warning de type END non aligne
void end_warning(const char *name, T_lexem *start, T_lexem *end)
{
  TRACE3("end_warning(%s, %x, %x)", name, start, end) ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(start) ;
  sprintf(get_msg_buffer(), get_catalog(C_OPN_MIGHT_NOT_BE_CLOSED), name) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(start) ;
  sprintf(get_msg_buffer(), get_catalog(C_BEGINNING_OF), name) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(end) ;
  sprintf(get_msg_buffer(), get_catalog(C_END_OF), name) ;
  deliver_msg(MSG_ERROR_STREAM) ;
}

// Erreur du type : lexeme non attendu
void syntax_unexpected_error(T_lexem *cur_lexem,
									  T_error_level error_level,
									  const char *expected_value)
{
  T_lexem *lexem = (cur_lexem == NULL) ? lex_get_last_lexem() : cur_lexem ;
  T_lexem *definition = NULL ;

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  if (cur_lexem == NULL)
	{
          sprintf(get_msg_buffer(), "%s",
			  get_catalog(C_UNEXPECTED_EOF_AFTER_THIS_TOKEN)) ;
	}
  else switch (cur_lexem->get_lex_type())
	{
	case L_IDENT :
	  {
		sprintf(get_msg_buffer(),
				get_catalog(C_UNEXPECTED_IDENT_VALUE),
				cur_lexem->get_value()) ;
		break ;
	  }

	case L_STRING :
	  {
		sprintf(get_msg_buffer(),
				get_catalog(C_UNEXPECTED_STRING_VALUE),
				cur_lexem->get_value()) ;
		break ;
	  }

	case L_NUMBER :
        case L_REAL :
	  {
		sprintf(get_msg_buffer(),
				get_catalog(C_UNEXPECTED_LITERAL_NUMBER_VALUE),
				cur_lexem->get_value()) ;
		break ;
	  }

	default :
	  {
		sprintf(get_msg_buffer(),
				get_catalog(C_UNEXPECTED_TOKEN_VALUE),
				get_lex_type_ascii(cur_lexem->get_lex_type())) ;
	  }
	}

  msg_buffer_add_new_line() ;
  deliver_msg(MSG_ERROR_STREAM) ;
  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(lexem) ;
  sprintf(get_msg_buffer(), get_catalog(C_EXPECTED_VALUE), expected_value) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

// Erreur du type : item non attendu
void syntax_unexpected_error(T_item *cur_item,
									  T_error_level error_level,
									  const char *expected_value)
{
  TRACE1("syntax_unexpected_error(%x)", expected_value) ;
  TRACE1("syntax_unexpected_error(%s)", expected_value) ;

  TRACE1("ici cur_item %x", cur_item) ;
  T_lexem *lexem = (cur_item->get_ref_lexem() == NULL)
	? lex_get_last_lexem() : cur_item->get_ref_lexem() ;
  T_lexem *definition = NULL ;
  const char *class_name = make_class_name(cur_item) ;

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(lexem) ;
  sprintf(get_msg_buffer(), "%s", get_catalog(C_UNEXPECTED)) ;
  deliver_msg(MSG_ERROR_STREAM) ;
  sprintf(get_msg_buffer(), " %s\n", class_name) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(lexem) ;
  sprintf(get_msg_buffer(), get_catalog(C_EXPECTED_VALUE), expected_value) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

void syntax_warning(T_lexem *lexem,
							 T_warning_level warning_level,
							 const char *format,
							 ...)
{
  if (do_warning(warning_level) == TRUE)
	{
	  va_list ap ;

	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(lexem) ;

	  va_start(ap, format) ;
	  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
	  ASSERT(len < BUFFER_SIZE) ;
#endif
	  va_end(ap) ;

	  char *p = (char *)((size_t)get_msg_buffer() + len) ;
	  *p = '\n' ;
	  *(p + 1) = '\0' ;

	  deliver_msg(MSG_ERROR_STREAM) ;
	  warning_count_si ++ ;
	}
}

void semantic_warning(T_item *item,
							 T_warning_level warning_level,
							 const char *format,
							 ...)
{
  if (do_warning(warning_level) == TRUE)
	{
	  va_list ap ;

	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(item->get_ref_lexem()) ;
	  va_start(ap, format) ;
	  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
	  ASSERT(len < BUFFER_SIZE) ;
#endif
	  va_end(ap) ;

	  char *p = (char *)((size_t)get_msg_buffer() + len) ;
	  *p = '\n' ;
	  *(p + 1) = '\0' ;

	  deliver_msg(MSG_ERROR_STREAM) ;
	}
}

// Erreur du type : lexeme redefinition
static T_error_level already_error_level_si = CAN_CONTINUE ;
void syntax_already_error(T_lexem *cur_lexem,
								   T_error_level error_level,
								   const char *redefined_item)
{
  TRACE1("syntax_already_error(%s)", redefined_item) ;
  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(cur_lexem) ;
  sprintf(get_msg_buffer(), get_catalog(C_REDEFINITION_OF), redefined_item) ;
  deliver_msg(MSG_ERROR_STREAM) ;
  already_error_level_si = error_level ;

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}


// Erreur du type : lexeme redefinition -- info de positionnement
void already_location(T_lexem *cur_lexem)
{
  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(cur_lexem) ;
  sprintf(get_msg_buffer(), "%s", get_catalog(C_LOCATION_OF_PREVIOUS_OCCURENCE)) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  if (already_error_level_si == FATAL_ERROR)
	{
	  stop() ;
	}
}

// Erreur de type : expression attendue, got ...
void syntax_expr_expected_error(T_item *cur_item,
								T_error_level error_level)
{
  TRACE0("syntax_expr_expected_error") ;
  ASSERT(cur_item != NULL) ;

  syntax_unexpected_error(cur_item, error_level, get_catalog(C_EXPR)) ;
}

// Erreur de type : predicat attendu, got ...
void syntax_predicate_expected_error(T_item *cur_item,
									 T_error_level error_level)
{
  syntax_unexpected_error(cur_item, error_level, get_catalog(C_PRED)) ;
}

// Erreur de type : ident attendu, got ...
void syntax_ident_expected_error(T_item *cur_item,
								 T_error_level error_level)
{
  syntax_unexpected_error(cur_item, error_level, get_catalog(C_IDENT)) ;
}

// Erreur de type : ident attendu, got ...
void syntax_ident_expected_error(T_lexem *cur_lexem,
								 T_error_level error_level)
{
  syntax_unexpected_error(cur_lexem, error_level, get_catalog(C_IDENT)) ;
}

// Erreur de type : substitution attendue, got ...
void syntax_substitution_expected_error(T_item *cur_item,
										T_error_level error_level)
{
  syntax_unexpected_error(cur_item, error_level, get_catalog(C_SUBST)) ;
}

//
//	}{	Erreur interne du compilateur
//
void internal_error(T_lexem *lexem,
							 const char *file,
							 int line,
							 const char *format,
							 ...)
{
  va_list ap ;

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  TRACE3("internal_error at %s:%d (%s)", file, line, format) ;

  new_msg(MSG_ERROR_STREAM) ;
  sprintf(get_msg_buffer(), get_catalog(C_INTERNAL_ERROR), file, line) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(lexem) ;
  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;
  stop() ;
}

//	Erreur semantique
void semantic_error(T_lexem *lexem,
							 T_error_level error_level,
							 const char *format,
							 ...)
{
  va_list ap ;

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

//	Erreur semantique
void semantic_error(T_item *item,
							 T_error_level error_level,
							 const char *format,
							 ...)
{
  va_list ap ;

  T_lexem *lexem = item->get_ref_lexem() ;
  T_lexem *definition = NULL ; 	// definition, le cas echeant

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

//	Erreur semantique : details
void semantic_error_details(T_lexem *lexem, const char *format, ...)
{
  va_list ap ;


  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

}

//	Erreur semantique : details
void semantic_error_details(T_item *item, const char *format, ...)
{
  va_list ap ;

  T_lexem *lexem = item->get_ref_lexem() ;

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;
}

// Indication de la localisation d'une clause en cas d'erreur
void clause_location_error(T_item *cur_item,
									const char *clause_name)
{
  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(cur_item->get_ref_lexem()) ;
  sprintf(get_msg_buffer(), get_catalog(C_LOCALISATION_OF_CLAUSE), clause_name) ;
  deliver_msg(MSG_ERROR_STREAM) ;
}

// Erreur de type "clause 1 implique clause 2"
void cl1_implies_cl2_error(T_flag *cl1, T_lex_type cl2)
{
  const char *cl1_name = get_lex_type_ascii(cl1->get_ref_lexem()->get_lex_type()) ;

  semantic_error(cl1->get_betree()->get_root(),
				 CAN_CONTINUE,
				 get_error_msg(E_CL1_IMPLIES_CL2),
				 cl1_name,
				 get_lex_type_ascii(cl2)) ;
  clause_location_error(cl1, cl1_name) ;
}

// Erreur de type : double definition d'identificateur
void double_identifier_error(T_ident *new_def,
									  T_ident *old_def)
{
  const char *new_name = new_def->get_name()->get_value() ;

  new_msg(MSG_ERROR_STREAM) ;
  LOCATE_ERR(new_def->get_ref_lexem()) ;
  sprintf(get_msg_buffer(),
		  get_error_msg(E_IDENT_CLASH),
		  new_name,
		  new_def->get_ident_type_name(),
		  old_def->get_ident_type_name()) ;
  msg_buffer_add_new_line() ;
  deliver_msg(MSG_ERROR_STREAM) ;

  new_msg(MSG_ERROR_STREAM) ;
  error_count_si++ ;
  LOCATE_ERR(old_def->get_ref_lexem()) ;
  sprintf(get_msg_buffer(), "%s", get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
   msg_buffer_add_new_line() ;
  deliver_msg(MSG_ERROR_STREAM) ;
  // GP 14/10/98 resol pb duplic message "double identifier"

  // Merge des deux anneaux de collage pour ne pas bégayer par la suite.
  old_def->insert_in_glued_ring(new_def) ;
}

void toplevel_warning(T_warning_level warning_level,
							   const char *format,
							   ...)
{
  if (do_warning(warning_level) == FALSE)
	{
	  return ;
	}

  va_list ap ;

  new_msg(MSG_ERROR_STREAM) ;
  sprintf(get_msg_buffer(), "%s", get_catalog(C_AT_TOPLEVEL)) ;
  deliver_msg(MSG_ERROR_STREAM) ;

  va_start(ap, format) ;

  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;
	  warning_count_si ++ ;
}

// Production d'une erreur utilisateur
void user_error(T_item *item,
						 T_error_level error_level,
						 const char *format,
						 ...)
{
  va_list ap ;

  T_lexem *lexem = item->get_ref_lexem() ;
  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  if (lexem != NULL)
	{
	  locate_msg(MSG_ERROR_STREAM,
				 lexem->get_file_name()->get_value(),
				 lexem->get_file_line(),
				 lexem->get_file_column()) ;
	}

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

// Production d'un avertissement utilisateur
void user_warning(T_item *item,
						   T_warning_level warning_level,
						   const char *format,
						   ...)
{
  TRACE1("user_warning<%s>", format) ;

  if (do_warning(warning_level) == FALSE)
	{
	  return ;
	}

  va_list ap ;

  T_lexem *lexem = item->get_ref_lexem() ;
  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  if (lexem != NULL)
	{
	  locate_msg(MSG_ERROR_STREAM,
				 lexem->get_file_name()->get_value(),
				 lexem->get_file_line(),
				 lexem->get_file_column()) ;
	}

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;
	  warning_count_si ++ ;
}

// Production d'une erreur utilisateur
void user_error(T_error_level error_level,
						 const char *format,
						 ...)
{
  va_list ap ;
  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  new_msg(MSG_ERROR_STREAM) ;
  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}

// Production d'un avertissement utilisateur
void user_warning(T_warning_level warning_level,
						   const char *format,
						   ...)
{
  if (do_warning(warning_level) == FALSE)
	{
	  return ;
	}

  new_msg(MSG_ERROR_STREAM) ;
  va_list ap ;
  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  deliver_msg(MSG_ERROR_STREAM) ;
	  warning_count_si ++ ;
}

//
//	}{ Gestion des lignes
//
T_msg_line::T_msg_line(T_msg_stream new_msg_stream,
								T_item **adr_first,
								T_item **adr_last,
								T_item *father)
  : T_item(NODE_MSG_LINE, adr_first, adr_last, father, NULL, NULL)
{
  TRACE5("T_msg_line(%x)::T_msg_line(%d, %x, %x, %x)",
		 this, new_msg_stream, adr_first, adr_last, father) ;

  msg_stream = new_msg_stream ;

  TRACE1("this %x", this) ;
  TRACE1("this %s", this->get_class_name()) ;
  TRACE1("this->fname %x", this->get_file_name()) ;

  file_name = NULL ;
  contents = NULL ;
}

T_msg_line::~T_msg_line(void)
{
  TRACE1("T_msg_line(%x)::~T_msg_line", this) ;
}

//
//	}{ Acces au gestionnaire de lignes
//
T_msg_line_manager::T_msg_line_manager(void) : T_item(NODE_MSG_LINE_MANAGER)
{
  TRACE1("T_msg_line_manager(%x)::T_msg_line_manager", this) ;

  first_msg = last_msg = NULL ;
}

T_msg_line_manager::~T_msg_line_manager(void)
{
  TRACE1("T_msg_line_manager(%x)::~T_msg_line_manager(void)", this) ;
}

// Acces au gestionnaire de lignes (avec creation le cas echeant)
static T_msg_line_manager *msg_line_manager_sop = NULL ;
extern T_msg_line_manager *get_msg_line_manager(void)
{
  if (msg_line_manager_sop == NULL)
	{
	  msg_line_manager_sop = new T_msg_line_manager() ;
	}

  return msg_line_manager_sop ;
}

// Reset du gestionnaire de lignes
void reset_msg_line_manager(void)
{
  TRACE0("reset_msg_line_manager") ;
  msg_line_manager_sop = NULL ;
}

// Creation d'une ligne
T_msg_line *T_msg_line_manager::create_line(T_msg_stream new_msg_stream)
{
  return new T_msg_line(new_msg_stream,
						(T_item **)&first_msg,
						(T_item **)&last_msg,
						this) ;
}

// Liberation de toutes les lignes
void T_msg_line_manager::unlink_lines(void)
{
  TRACE0("T_msg_line_manager::unlink_lines") ;

  list_unlink(first_msg) ;
  first_msg = last_msg = NULL ;
}


//
//
// MESSAGES DU B0 CHECKER
//
//

//
// Erreur de syntaxe
//

void B0_syntax_error(T_item *item,
							  T_error_level error_level,
							  const char *format,
							  ...)
{
  T_lexem *lexem = item->get_ref_lexem() ;
  TRACE2("B0_syntax_error(lexem %x, original_lexem %x)",
		 lexem,
		 (lexem == NULL) ? NULL : lexem->get_original_lexem()) ;
  va_list ap ;

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  T_lexem *definition = NULL ;

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;
  TRACE1("lexem->get_file_name = %x", lexem->get_file_name()) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  va_start(ap, format) ;
  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif
  va_end(ap) ;

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  char *tmp ;
  clone(&tmp, get_msg_buffer()) ;
  sprintf(get_msg_buffer(), "(B0Check) %s", tmp) ;
  s_free(tmp) ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}
}


//
// Erreur semantique
//

static void B0_semantic_error(T_lexem *lexem,
									   T_error_level error_level,
									   const char *format,
									   va_list *ap)
{
  T_lexem *definition = NULL ; 	// definition, le cas echeant

  if (lexem == NULL)
	{
	  // Cas d'erreur en fin de fichier !
	  lexem = lex_get_last_lexem() ;
	}

  if (lexem->get_original_lexem() != NULL)
	{
	  definition = lexem ;
	  lexem = lexem->get_original_lexem() ;
	}

  TRACE1("lexem = %x", lexem) ;

  new_msg(MSG_ERROR_STREAM) ;
  locate_msg(MSG_ERROR_STREAM,
			 lexem->get_file_name()->get_value(),
			 lexem->get_file_line(),
			 lexem->get_file_column()) ;

  int len = vsprintf(get_msg_buffer(), format, *ap) ;
#ifndef IPX // ne marche pas sur les IPX
  ASSERT(len < BUFFER_SIZE) ;
#endif

  char *p = (char *)((size_t)get_msg_buffer() + len) ;
  *p = '\n' ;
  *(p + 1) = '\0' ;

  char *tmp ;
  clone(&tmp, get_msg_buffer()) ;
  sprintf(get_msg_buffer(), "(B0Check) %s", tmp) ;
  delete [] tmp ;

  deliver_msg(MSG_ERROR_STREAM) ;

  if (definition != NULL)
	{
	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(definition) ;
	  sprintf(get_msg_buffer(),
			  get_catalog(C_IS_A_MACRO_DEF_HERE),
			  lexem->get_lex_name()) ;
	  deliver_msg(MSG_ERROR_STREAM) ;
	}

  if (error_level == FATAL_ERROR)
	{
	  stop() ;
	}

  if (error_level != MULTI_LINE)
	{
	  error_count_si++ ;
	}

}

void B0_semantic_error(T_lexem *lexem,
								T_error_level error_level,
								const char *format,
								...)
{
  va_list ap ;

  va_start(ap, format) ;

  B0_semantic_error(lexem,
					error_level,
					format,
					&ap);
  va_end(ap);
}

void B0_semantic_error(T_item *item,
								T_error_level error_level,
								const char *format,
								...)
{
  va_list ap ;

  T_lexem *lexem = item->get_ref_lexem() ;

  va_start(ap, format) ;
  B0_semantic_error(lexem,
					error_level,
					format,
					&ap);

  va_end(ap) ;
}


//
// warning semantique
//
void B0_semantic_warning(T_item *item,
								  T_warning_level warning_level,
								  const char *format,
								  ...)
{
  if (do_warning(warning_level) == TRUE)
	{
	  va_list ap ;

	  new_msg(MSG_ERROR_STREAM) ;
	  LOCATE_ERR(item->get_ref_lexem()) ;
	  va_start(ap, format) ;
	  int len = vsprintf(get_msg_buffer(), format, ap) ;
#ifndef IPX // ne marche pas sur les IPX
	  ASSERT(len < BUFFER_SIZE) ;
#endif
	  va_end(ap) ;

	  char *p = (char *)((size_t)get_msg_buffer() + len) ;
	  *p = '\n' ;
	  *(p + 1) = '\0' ;

	  char *tmp ;
	  clone(&tmp, get_msg_buffer()) ;
	  sprintf(get_msg_buffer(), "(B0Check) %s", tmp) ;
	  delete [] tmp ;

	  deliver_msg(MSG_ERROR_STREAM) ;
	  warning_count_si ++ ;
	}
}


//
// }{ Fin du fichier
//
