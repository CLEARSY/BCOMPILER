/******************************* CLEARSY **************************************
* Fichier : $Id: p_trace.cpp,v 2.0 1998/11/02 14:16:39 rp Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_trace
*				Messages de trace.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_trace.cpp,v 1.3 1998/11/02 14:16:39 rp Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef WIN32
#include <unistd.h>
#endif

// Includes Composants externes
#include "p_trace.h"

typedef enum
{
  TRACE_NOT_OPENED,
  TRACE_OPENED,
  TRACE_NO_TRACE,
} T_trace_state ;

// Taille max d'une ligne : 64 Ko
static const int TRACE_LINE_SIZE = 65536 ;

// Marge pour l'ecriture des noms de fichiers
#ifdef WIN32
static const int MARGE=50 ; // Plus de place car path complet !
#else
static const int MARGE=25 ;
#endif

/* Structures locales */

/* Variables statiques locales */
static FILE *trace_fd_sip = NULL ;
static T_trace_state etat_trace_si = TRACE_NOT_OPENED ;

#ifdef IPX
static char errno_buf[128] ;
#endif

static int indent_level = 0 ;

// Tampon de trace
static char trace_buffer[TRACE_LINE_SIZE] ;

// Les fonctions ont des prototypes C pour l'interfacage Betree -> B0tree

EXTERN void trace(const char *file_acp, long line_ai, const char *format_acp, ...)
{
  char *p ;
  va_list ap ;
#ifndef WIN32
  int file_li ;
#endif
  int i ;

  if (etat_trace_si == TRACE_NO_TRACE)
	{
	  return ;
	}

  if (etat_trace_si == TRACE_NOT_OPENED)
	{
	  char *fname_lcp = getenv("TRACE_FILE") ;
	  if (fname_lcp == NULL)
		{
		  etat_trace_si = TRACE_NO_TRACE ;
		  return ;
		}

	  trace_fd_sip = fopen(fname_lcp, "w") ;

	  if (trace_fd_sip == NULL)
		{
		  // A faire en premier : marquer etat_trace_si
		  // sinon on boucle en appelant une fonction
		  // qui demande des traces (comme par exemple
		  // get_catalog)
		  etat_trace_si = TRACE_NO_TRACE ;
		  //perror(get_catalog(C_UNABLE_TO_OPEN_TRACE_FILE)) ;
		  fprintf(stderr, "($TRACE_FILE=%s)\n", fname_lcp) ;
		  exit(1) ;
		}

	  etat_trace_si = TRACE_OPENED ;
	}


  /* Il faut preparer le buffer de trace */
  sprintf(trace_buffer,
		  "(%03d)%s:%ld                              ",
		  indent_level,
		  file_acp,
		  line_ai) ;

  for (i = 0 ; i < indent_level ; i++)
	{
	  trace_buffer[MARGE+i] = ' ' ;
	}

  va_start(ap, format_acp) ;

  vsprintf(&trace_buffer[MARGE+indent_level], format_acp, ap) ;

  va_end(ap) ;

  p = &trace_buffer[0] + strlen(trace_buffer) - 1  ;

  if (*p != '\n')
	{
	  // On force un retour chariot
	  *(++p) = '\n' ;
	  *(++p) = '\0' ;
	}

  // On ecrit avec write pour etre sur que la trace est flushee
#ifndef WIN32
  file_li = fileno(trace_fd_sip) ;
  write(file_li, trace_buffer, strlen(trace_buffer)) ;
#else
  // Pas de write sous Windows95
  char *q = trace_buffer ;
  while (*q != '\0')
	{
	  fputc((int)(*q), trace_fd_sip) ;
	  q++ ;
	}
#endif

}

// Limiteur de niveaux de traces
static const int MAX_INDENT_LEVEL = 30 ;

EXTERN void enter_trace(void)
{
  if (indent_level < MAX_INDENT_LEVEL)
	{
	  indent_level++ ;
	}
}

EXTERN void exit_trace(void)
{
  if (indent_level > 0)
	{
	  indent_level-- ;
	}
}

// Fonction cachee pour c_secure.cpp
void reset_trace(void)
{
  indent_level = 0 ;
}


// Fin de fichier
// _______________
