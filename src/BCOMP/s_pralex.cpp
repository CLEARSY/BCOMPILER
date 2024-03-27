/***************************** CLEARSY **************************************
* Fichier : $Id: s_pralex.cpp,v 2.0 1999-06-09 12:38:42 sl Exp $
* (C) 2008 CLEARSY
*
* Compilations :	*	-DDEBUG_PRAGMA_LEX pour avoir les traces
*						de l'analyse lexicale
*
* Description :
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
RCS_ID("$Id: s_pralex.cpp,v 1.14 1999-06-09 12:38:42 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_pstack.h"

// Declarations forward
#ifndef DEBUG_PRAGMA_LEX
static char next_car(void) ;
static char cur_car(void) ;
static char prev_car(void) ;
#else
#define next_car() _next_car(__FILE__, __LINE__)
#define cur_car() _cur_car(__FILE__, __LINE__)
#define prev_car() _prev_car(__FILE__, __LINE__)

static char _next_car(const char *file, int line) ;
static char _cur_car(const char *file, int line) ;
static char _prev_car(const char *file, int line) ;
#endif /* DEBUG_PRAGMA_LEX */

// Variables statiques
static int index_si ;
#ifdef CHECK_INDEX
static int size_si = 0 ;
#endif
static char *load_sct = NULL ;
static char *los_buf = NULL ;
static int cur_line_si = 0 ;
static int cur_column_si = 0 ;
static T_pstack *lifo_pos_sop = NULL ;
const int LEX_LIFO_SIZE = 10 ; 	// taille de la LIFO qui memorise les positions

T_pragma_lexem *T_pragma_lexem::first_lexem = NULL ;
T_pragma_lexem *T_pragma_lexem::last_lexem = NULL ;

// Methodes auxiliaires
static int is_a_letter(int carac)
{
  return (	( (carac >= 'a') && (carac <= 'z') )
			|| ( (carac >= 'A') && (carac <= 'Z') ) ) ;
}

static int is_an_extended_letter(int carac)
{
  return (	( (carac >= 'a') && (carac <= 'z') )
			|| ( (carac >= 'A') && (carac <= 'Z') )
			|| ( (carac >= '0') && (carac <= '9') )
			|| (carac == '_')
			//		 || (carac == '.')				TRAITE A PART
			|| (carac == '$') ) ;
}

static int is_space(int carac)
{
  return (    (carac == ' ')
			  || (carac == '\t')
			  || (carac == '\n')
			  || (carac == '\r') // Pour lire les fichiers Windows 95
			  || (carac == 12)) ;// ^L
}

// Pragma_Analyse d'un mot
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
static void pragma_analyse_word(T_comment *ref_comment)
{
#ifdef DEBUG_PRAGMA_LEX
TRACE0("pragma_analyse_word DEBUT") ;
ENTER_TRACE ;
#endif

char *p = los_buf ;
*(p++) = cur_car() ;

int line = cur_line_si ;
int column = cur_column_si ;

for (;;)
	{
	  int cour = next_car() ;

	// Pas la peine de tester is_a_letter pour le premier cas
	// car ce test a deja ete fait par l'appelant
	// Attention, si on a un DOT, il ne faut continue que si le
	// DOT est suivi d'un identifiant. Une CNS pour cela est
	// que l'item qui suit le DOT soit une lettre.
	// Sinon, on s'arrete avant le DOT et il sera transforme en PL_DOT
	// le cas suivant.
	  if (is_an_extended_letter(cour) == TRUE)
		{
		  *(p++) = cour ;
		}
	  else
		{
		  // Fin de mot atteinte !
		  *p = '\0' ;

		  if ( (p - los_buf == 6)
			  && (strcmp(los_buf, "pragma") == 0) )
			{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE1("on a reconnu le mot-cle pragma", los_buf) ;
#endif
			  (void)new T_pragma_lexem(PL_PRAGMA,
									   ref_comment,
									   NULL, 0,
									   line,
									   column) ;
			}
		  else
			{
#ifdef DEBUG_PRAGMA_LEX
			  TRACE1("on a reconnu le mot \"%s\"", los_buf) ;
#endif
			  (void) new T_pragma_lexem(PL_IDENT,
										ref_comment,
										los_buf,
										p - los_buf,
										line,
										column) ;
			}

		  // C'est tout !
		  // On retourne sur le dernier caractere
		  prev_car() ;

#ifdef DEBUG_PRAGMA_LEX
		  EXIT_TRACE ; TRACE0("pragma_analyse_word FIN") ;
#endif
	 return ;
		}
	}
}


// Pragma_Analyse d'une chaine
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
// Pas static car friend
void pragma_analyse_string(T_comment *ref_comment)
{
#ifdef DEBUG_PRAGMA_LEX
  TRACE0("pragma_analyse_string DEBUT") ;
  ENTER_TRACE ;
#endif

  char *p = los_buf ;
  *(p++) = cur_car() ;

  int line = cur_line_si ;
  int column = cur_column_si ;

  int prev_is_a_backslash = FALSE ;
  for (;;)
	{
	  int cour = next_car() ;

	  switch(cour)
		{
		case '\0' :
		  {
			TRACE0("EOF inside string") ;
			// Ici il y a erreur en cas d'analyse de pragma
			// Sinon, on sort
			// Pour savoir si on analyse un pragma il suffit de regarder
			// first_lexem
			T_pragma_lexem *first = T_pragma_lexem::first_lexem ;
			const char *file_name ;
			file_name = ref_comment->get_ref_lexem()->get_file_name()->get_value() ;
			if (    (first != NULL)
				 && (first->get_lex_type() == PL_PRAGMA) )
			  {
				parse_error(file_name,
							cur_line_si,
							cur_column_si,
							MULTI_LINE,
							get_error_msg(E_EOF_INSIDE_STRING)) ;

				parse_error(file_name,
							line,
							column,
							FATAL_ERROR,
							get_error_msg(E_START_OF_STRING)) ;
			  }
			else
			  {
				// Pas dans un pragma : on ne dit rien
                // On depile la lecture de \0
                prev_car();
				return ;
			  }
		  }
		case '"' :
		  {
			TRACE1("cas guillemet : prev_is_a_backslash %s",
				   (prev_is_a_backslash == TRUE) ? "TRUE" : "FALSE") ;
			if (prev_is_a_backslash == FALSE)
			  {
				// Fin de chaine atteinte !
				*p = '\0' ;

#ifdef DEBUG_PRAGMA_LEX
				TRACE1("on a reconnu la chaine <%s>", los_buf) ;
#endif
				(void)new T_pragma_lexem(PL_STRING,
										 ref_comment,
										 los_buf + 1, // sauter le " "
										 p - los_buf - 1,
										 line,
										 column) ;

				// C'est tout !
				// On retourne sur le dernier caractere
				//prev_car() ;
				// NON !!!
#ifdef DEBUG_PRAGMA_LEX
				EXIT_TRACE ; TRACE0("pragma_analyse_string FIN") ;
#endif
				return ;
			  }
			else
			  {
				// cas \" :
				// On memporise la chaine
				*(p++) = cour ;
			  }
			prev_is_a_backslash = FALSE ;
			break ;
		  }
		case '\\' :
		  {
			// On saute les antislash
			TRACE0("On saute les antislash") ;
			prev_is_a_backslash = TRUE ;
			break ;
		  }
		default :
		  {
			// On memporise la chaine
			*(p++) = cour ;
			prev_is_a_backslash = FALSE ;
		  }
		}
	}
}

static void get_pragma_lexems(T_comment *ref_comment)
{
#ifdef DEBUG_PRAGMA_LEX
  TRACE0("get_pragma_lexems") ;
#endif

  for (;;)
	{
	  int cour = next_car() ;

#ifdef DEBUG_PRAGMA_LEX
	  TRACE4("cour = '%c' (%03d) L%dC%d", cour, cour, cur_line_si, cur_column_si) ;
#endif

	  // On saute les espaces
	  while (is_space(cour))
		{
		  cour = next_car() ;
#ifdef DEBUG_PRAGMA_LEX
		  TRACE2("skip cour = '%c' (%03d)", cour, cour) ;
#endif
		}

#ifdef DEBUG_PRAGMA_LEX
	  TRACE4("cour = '%c' (%03d) L%dC%d", cour, cour, cur_line_si, cur_column_si) ;
#endif

	  // On pragma_analyse
	  if (cour == '\0')
		{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("fin du fichier") ;
#endif
		  return ;
		}
	  else if (is_a_letter(cour) == TRUE)
		{
		  // Debut de mot
		  pragma_analyse_word(ref_comment) ;
		}
	  else if (cour == '"')
		{
		  // Debut de chaine
		  pragma_analyse_string(ref_comment) ;
		}
	  else if (cour == '(')
		{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("on a reconnu PL_OPEN_PAREN") ;
#endif
		  (void)new T_pragma_lexem(PL_OPEN_PAREN,
								   ref_comment,
								   NULL,
								   0,
								   cur_line_si,
								   cur_column_si) ;
		}
	  else if (cour == ')')
		{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("on a reconnu PL_CLOSE_PAREN") ;
#endif
		  (void)new T_pragma_lexem(PL_CLOSE_PAREN,
								   ref_comment,
								   NULL,
								   0,
								   cur_line_si,
								   cur_column_si) ;
		}
	  else if (cour == ',')
		{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("on a reconnu PL_COMMA") ;
#endif
		  (void)new T_pragma_lexem(PL_COMMA,
								   ref_comment,
								   NULL,
								   0,
								   cur_line_si,
								   cur_column_si) ;
		}
	  else
		{
#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("on a reconnu PL_GARBAGE") ;
#endif
		  (void)new T_pragma_lexem(PL_GARBAGE,
								   ref_comment,
								   NULL,
								   0,
								   cur_line_si,
								   cur_column_si) ;

#ifdef DEBUG_PRAGMA_LEX
		  TRACE0("GARBAGE -> ON ARRETE LA") ;
#endif
		  return ;
		}
	}
}

//
// }{ Chargement en memoire d'un commentaire a parser
//
T_pragma_lexem *pragma_lex_analysis(T_comment *ref_comment)
{
  char *p ; // Caractere lu */
  size_t offset_li ; // Indice pour remplir le tableau load_sct */

  size_t comment_len = ref_comment->get_comment()->get_len() ;

#ifdef DEBUG_PRAGMA_LEX
  TRACE1("Pragma_Analyse lexicale du commentaire %s",
		 ref_comment->get_comment()->get_value()) ;
#endif

  load_sct = (char *)s_volatile_malloc(comment_len + 1) ;
  los_buf = (char *)s_volatile_malloc(comment_len + 1) ;

  offset_li = 0 ;
  lifo_pos_sop = new T_pstack(LEX_LIFO_SIZE) ;

  // On triche car on sait qu'on ecrira pas
  p = (char *)ref_comment->get_comment()->get_value() ;

  while (*p != '\0')
	{
	  load_sct[offset_li++] = (int)*p ;

#ifndef NO_CHECKS
	  if (offset_li > comment_len + 1)
		{
		  assert(FALSE) ;
		}
#endif

	  p++ ;
	}

  load_sct[offset_li++] = '\0' ;
  index_si = -1 ;

  T_pragma_lexem::first_lexem = NULL ;
  T_pragma_lexem::last_lexem = NULL ;
  T_lexem *src_lexem = ref_comment->get_ref_lexem() ;
  cur_line_si = src_lexem->get_file_line() ;
  cur_column_si = src_lexem->get_file_column() ;

  // On peut caster le (const T_symbol *) en (T_symbol *) car on
  // sait qu'on ne l'ecrira pas
  T_symbol *open_comment = (T_symbol *)get_open_comment(src_lexem->get_lex_type()) ;
#ifdef DEBUG_PRAGMA_LEX
  TRACE1("on ajoute %d pour le debut de commentaire", open_comment->get_len()) ;
#endif
  cur_column_si += open_comment->get_len() - 1 ; // 1 a cause du next_car

#ifdef DEBUG_PRAGMA_LEX
  TRACE2("cur_line_si %d cur_column_si %d", cur_line_si, cur_column_si) ;
#endif

  get_pragma_lexems(ref_comment) ;

  free(load_sct) ;
  free(los_buf) ;
  delete lifo_pos_sop ;

  return T_pragma_lexem::first_lexem ;
}

// Liberations memoire
void pragma_lex_unload(void)
{
  TRACE0("pragma_lex_unload") ;
}

/* Caractere suivant du fichier */
#ifdef DEBUG_PRAGMA_LEX
static char _next_car(const char *file, int line)
#else
  static char next_car(void)
#endif
{
#ifdef CHECK_INDEX
  assert(index_si <= size_si) ;
#endif
  char res = load_sct[++index_si] ;

  lifo_pos_sop->push(cur_line_si,cur_column_si) ;

  if (res == '\n')
	{
	  cur_line_si++ ;
	  cur_column_si = 0 ;
	}
  else if (res != '\t')
	{
	  cur_column_si++ ;
	}
  else
	{
	  cur_column_si += get_tab_width() ;
	}

#ifdef DEBUG_PRAGMA_LEX
  TRACE6("next_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif

  return res ;
}

/* Caractere courant du fichier */
#ifdef DEBUG_PRAGMA_LEX
static char _cur_car(const char *file, int line)
#else
  static char cur_car(void)
#endif
{
  char res = load_sct[index_si] ;
#ifdef DEBUG_PRAGMA_LEX
  TRACE6("cur_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif
  return res ;
}

/* Retourner un cran en arriere */
#ifdef DEBUG_PRAGMA_LEX
static char _prev_car(const char *file, int line)
#else
  static char prev_car(void)
#endif
{
  char res = load_sct[--index_si] ;
  lifo_pos_sop->pop(cur_line_si,cur_column_si) ;

#ifdef DEBUG_PRAGMA_LEX
  TRACE6("prev_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif
  return res ;
}



//
// }{ Fin du fichier
//
