/******************************* CLEARSY **************************************
* Fichier : $Id: f_file.cpp,v 2.0 2006-11-28 09:56:05 atelierb Exp $
* (C) 2008 CLEARSY
*
* Description :		Ecriture dans un fichier avec gestion de l'indentation
*					et controle de la taille des lignes produites
*
* Compilations :	-DDEBUG_FILE pour avoir des traces
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

RCS_ID("$Id: f_file.cpp,v 1.32 2006-11-28 09:56:05 atelierb Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "c_api.h"


// Niveau d'indentation courant
static int indent_si = 0 ;

// Booleen indiquant s'il faut indenter indent_si
static int push_indent_si = FALSE;

// Booleen indiquant s'il faut decrementer indent_si
static int pop_indent_si = FALSE;

// Nombre de pop_indent_si successifs appeles
static int pop_indent_level_si = 0 ;

// Booleen indiquant s'il faut sortir en erreur ewn cas de depasement
// de la taille max des lignes, ou seulement emettre un warning
static int use_warning = FALSE;

// Fichier
static FILE *fd_sp ;

static int TAB_SIZE = 4 ;		// taille des tabulations
static const char *TAB = "\t" ;		// tabulation utilisee
static size_t MAX_LINE_SIZE = 80 ; // taille maximum des lignes produites
// static char *TAB = " " ;		// tabulation utilisee
// static size_t MAX_LINE_SIZE = 180 ; // taille maximum des lignes produites
static int use_sep = FALSE ;	// utilise-t-on un caractere de separation ligne ?
static int sep_size = 0 ;		// taille de ce char (0 si pas utilise)
static char sep_char = '?' ;	// char utilise
static int do_split = TRUE ;	// Faut-il couper ?
static int align_param = FALSE; // Quand un appel d'operation est trop long,
								// faut-il aligner le premier parametre, ou
								// le mettre a la ligne ?

// Buffer courant
#define BUFFER_SIZE (64*1024)	// 64Ko pour ne jamais deborder j'espere !
static char buffer_sct[BUFFER_SIZE] ;
static char *cur_buf_pos = buffer_sct ;

static size_t file_strlen(const char *src);

// Faut-il crier si une ligne est ins�cable, ou continuer ?
static int do_warn_if_unsplittable_si = FALSE ;
void file_set_accept_unsplittable_lines(int new_do_warn)
{
  do_warn_if_unsplittable_si = new_do_warn ;
}
int file_get_accept_unsplittable_lines(void)
{
  return do_warn_if_unsplittable_si ;
}


// Changement de la taille de la ligne et des tabulations,
// use_space precise si on doit utiliser des espaces ou des tabulations
EXTERN void file_set_configuration(int new_tab_size,
											int new_max_line_size,
											int use_space)
{
  TRACE3("file_set_configuration(%d,%d,%d)",
		 new_tab_size,
		 new_max_line_size,
		 use_space) ;

  int i = 0;
  TAB_SIZE = new_tab_size ;
  MAX_LINE_SIZE = new_max_line_size ;

  if (use_space == TRUE)
	{
	  char *tmp = (char *)s_volatile_malloc(TAB_SIZE + 1);
	  while (i < TAB_SIZE)
		{
		  tmp[i++] = ' ';
		}
	  tmp[i] = '\0';
	  TAB = tmp;
	}
}

// Mise a zero du niveau d'indentation
EXTERN void file_reset_indent(FILE *fd)
{
  TRACE1("file_reset_indent(%x)", fd) ;
  indent_si = 0 ;
  push_indent_si = FALSE;
  pop_indent_si = FALSE;

#ifdef DEBUG_FILE
  TRACE0("ACHTUNG ! on change fd") ;
#endif

  fd_sp = fd ;
  buffer_sct[0] = '\0' ;
  cur_buf_pos = buffer_sct ;
  use_sep = FALSE ;	// utilise-t-on un caractere de separation ligne ?
  sep_size = 0 ;		// taille de ce char (0 si pas utilise)
  sep_char = '?' ;	// char utilise
  //do_split = TRUE ; // SL 01/12/98 reset pas pratique pour les traducteurs
}

// Renvoie TRUE si file est ouvert sur un descripteur
EXTERN int file_is_open(void)
{
  return (fd_sp == NULL) ? FALSE : TRUE ;
}


// Etablissement du mode sans separation des lignes
EXTERN void file_set_no_split(void)
{
  TRACE0("file_set_no_split") ;
  do_split = FALSE ;
}

// Utilisation d'un caractere de separation des lignes
EXTERN void file_enable_line_separator(char new_sep)
{
  use_sep = TRUE ;
  sep_size = 1 ;
  sep_char = new_sep ;
}

// Suprresion d'un caractere de separation des lignes
EXTERN void file_disable_line_separator(void)
{
  use_sep = FALSE ;
}

// new_align_param vaut TRUE si on souhaite interdire le saut de ligne
// avant le premier appel parametre d'un appel d'operation quand
// celui-ci est trop long, sinon il vaut FALSE
EXTERN void align_first_param(int new_align_param)
{
  align_param = new_align_param;
}

// indique a la bibliotheque de ne pas sortir en erreur si les lignes
// sont trop longues
EXTERN void file_use_warning(void)
{
#ifdef DEBUG_FILE
  TRACE0("use_warning = TRUE ... ");
#endif
  use_warning = TRUE;
}

// Ajout d'un niveau d'indentation apres le prochain new_line
EXTERN void file_push_indent(void)
{
#ifdef DEBUG_FILE
  TRACE1("file_push_indent -> %d", indent_si + 1) ;
#endif
  // indent_si++ ;
  push_indent_si = TRUE;

  file_new_line() ;

}

EXTERN void file_old_push_indent(void)
{
#ifdef DEBUG_FILE
  TRACE1("file_old_push_indent -> %d", indent_si + 1) ;
#endif
  // indent_si++ ;
  push_indent_si = TRUE;
}

extern void file_push_indent(const char *msg)
{
  file_fprintf(msg) ;
  file_push_indent() ;
  file_new_line() ;
}

// Ajout d'un niveau d'indentation apres le prochain new_line
EXTERN void file_pop_indent(void)
{
  // indent_si-- ;
  if (pop_indent_si == FALSE)
	{
	  // Premier pop depuis un new_line : on remet le compteur de pop a 1
	  pop_indent_level_si = 1 ;
	}
  else
	{
	  // nieme pop depuis un new_line : on incremente le compteur de pop
	  pop_indent_level_si ++ ;
	}
  pop_indent_si = TRUE;

#ifdef DEBUG_FILE
  TRACE1("pop_indent_level_si = %d", pop_indent_level_si) ;
#endif

  file_new_line() ;
}

EXTERN void file_old_pop_indent(void)
{
  // indent_si-- ;
  if (pop_indent_si == FALSE)
	{
	  // Premier pop depuis un new_line : on remet le compteur de pop a 1
	  pop_indent_level_si = 1 ;
	}
  else
	{
	  // nieme pop depuis un new_line : on incremente le compteur de pop
	  pop_indent_level_si ++ ;
	}
  pop_indent_si = TRUE;

#ifdef DEBUG_FILE
  TRACE1("pop_indent_level_si = %d", pop_indent_level_si) ;
#endif
}

extern void file_pop_indent(const char *msg)
{
  file_fprintf(msg) ;
  file_pop_indent() ;
  file_new_line() ;
}

// Fonction auxiliaire qui fait un strchr en sautant les chaines
// Ex :    strchr("f(\"a, b\", c", ',') --> ",b..."
//      my_strchr("f(\"a,b\", c", ',') --> ", c..."
static char *my_strchr(char *s, int c)
{
  char *cur = s ;
  char prev = '\0' ;
  int inside_string = FALSE ;

  for (;;)
	{
	  if (*cur == '\0')
		{
		  // Fin de chaine
		  return NULL ;
		}

	  if ( ((int)(*cur) == c) && (inside_string == FALSE) )
		{
		  // Trouve !
		  return cur ;
		}

	  if ( (*cur == '\"') && (prev != '\\') ) // '"' pour le hilit d'emacs :-)
		{
		  // Debut ou fin de chaine
		  inside_string = (inside_string == TRUE) ? FALSE : TRUE ;
		}

	  prev = *cur ;
	  cur ++ ;
	}
}

// Flush de la ligne courante dans le fichier si c'est un appel de fonction
static void flush_long_function_call(int cur_len)
{
  char *cur = buffer_sct ;
  char *pos = NULL;
  char mem ;
  char *end ;	// dernier caractere utile de la ligne (i.e. pas un espace)
#ifdef DEBUG_FILE
  TRACE1("flush_long_function_call <%s>", buffer_sct) ;
  TRACE1("indent_si = %d", indent_si) ;
#endif

  // On ecrit le nom de la fonction, jusqu'a la parenthese ouvrante
  // si align_param vaut FALSE, jusqu'a la premiere virgule s'il vaut TRUE
  if (align_param == TRUE)
	{
	  pos = my_strchr(cur, ',') ;
	  if (pos == NULL)
		{
		  pos = my_strchr(cur, ';') ;
		}
	}
  if (pos == NULL)
	{
	  // soit align_param vaut FALSE, soit il n'y a qu'un seul parametre
	  // donc pas de virgule
	  pos = my_strchr(cur, '(') ;

	  //CT : il peut y avoir d'autres parametres...
	  if ( pos == NULL )
		{
		  pos = my_strchr(cur, ',') ;

		  if ( pos == NULL )
			{
			  pos = my_strchr(cur,';') ;
			}
		}

	}
  assert(pos) ; // sinon c'est la galere ...
#ifdef DEBUG_FILE
  TRACE1("pos : %s", pos);
#endif // DEBUG_FILE

  // Recherche du dernier caractere utile de la ligne
  end = cur + strlen(cur) - 1 ;
  while (*(end) == ' ')
	{
#ifdef DEBUG_FILE
	  TRACE0("end--") ;
#endif
	  *end-- = '\0';
	}

  cur = pos + 1 ;
  mem = *cur ;
  *cur = '\0' ;

#ifdef DEBUG_FILE
  TRACE1("s_fprintf(%s)", buffer_sct) ;
#endif
  s_fprintf(fd_sp, buffer_sct) ;

  *cur = mem ;

  // Ensuite les arguments, un par ligne, avec une tabulation de plus
  for (;;)
	{
	  int i ;
	  char *next ;

	  // decalage du aux tabulations
	  int tab_decal = (TAB_SIZE - 1) * (indent_si + 1) ;

	  // On ecrit la ligne suivante avec un tab de plus ...
	  s_fprintf(fd_sp, "\n") ;
#ifdef DEBUG_FILE
	  TRACE1("indent : %d",indent_si ) ;
#endif
	  for (i = 0 ; i <= indent_si ; i++)
		{
		  s_fprintf(fd_sp, TAB) ;
		}

	  // Recherche d'une ',' ou d'un ';' s�parant deux arguments.
	  pos = my_strchr(cur, ',') ;
	  if (pos == NULL)
		{
		  pos = my_strchr(cur, ';') ;
		  if (pos == end)
			{
			  // Attention au ';' final en C, ce n'est pas un point
			  // pour d�couper.
			  pos = NULL;
			}
		}

#ifdef DEBUG_FILE
	  TRACE1("pos                  : %p", pos) ;
	  TRACE1("end                  : %p", end) ;
	  TRACE1("longueur chaine      : %d", end - cur) ;
	  TRACE1("longueur avec indent : %d", end - cur + tab_decal) ;
	  TRACE1("longueur maximale    : %d", MAX_LINE_SIZE) ;
#endif
	  // Si pos == NULL alors il n'a y a plus qu'un seul argument.  On
	  // regarde alors si il est trop long.
	  if (pos == NULL && (file_strlen(cur) + tab_decal) >= MAX_LINE_SIZE)
		{
		  // L'argument est effectivement trop long, on essaie alors
		  // de d�couper suivant des op�rateurs arithm�tiques.
#ifdef DEBUG_FILE
		  TRACE1("le dernier argument \"%s\" est encore trop long", cur) ;
#endif

		  if (pos == NULL)
			{
#ifdef DEBUG_FILE
			  TRACE0("desespoir : pas de ',' on cherche un '+'") ;
#endif
			  pos = my_strchr(cur, '+') ;
			  if (pos != NULL && (pos[1] == '+' ||
								  pos[1] == '='))
				{
				  // Traitement des op�rateurs "++" et "+=" du C.
				  ++ pos;
				}
			}

// Suppression coupure sur '-' � cause des commentaires Ada
#if 0
		  if (pos == NULL)
			{
#ifdef DEBUG_FILE
			  TRACE1("desespoir : pas de '+' on cherche un '-'") ;
#endif
			  pos = my_strchr(cur, '-') ;
			  if (pos != NULL && (pos[1] == '-' ||
								  pos[1] == '=' ||
								  pos[1] == '>'))
				{
				  // Traitement des op�rateurs "--", "-=" et "->" du C.
				  ++ pos;
				}
			}
#endif

		  if (pos == NULL)
			{
#ifdef DEBUG_FILE
			  TRACE0("desespoir : pas de '-' on cherche un '/'") ;
#endif
			  pos = my_strchr(cur, '/') ;
			  if (pos != NULL && pos[1] == '=')
				{
				  // Traitement de l'op�rateur "/=" du C.
				  ++ pos;
				}
			}

// Suppression coupure sur '*' pour ne pas couper les d�clarations de
// param�tres en C-ISO.
#if 0
		  if (pos == NULL)
			{
#ifdef DEBUG_FILE
			  TRACE1("desespoir : pas de '/' on cherche une '*'") ;
#endif
			  pos = my_strchr(cur, '*') ;
			  if (pos != NULL && (pos[1] == '*' ||
								  pos[1] == '='))
				{
				  // Traitement des op�rateurs "**" d'Ada et "*=" du C.
				  ++ pos;
				}
			}
#endif

		  if (pos == NULL)
			{
#ifdef DEBUG_FILE
			  TRACE0("desespoir : pas de '*' on cherche un '='") ;
#endif
			  pos = my_strchr(cur, '=') ;
			  if (pos != NULL && (pos[1] == '>' ||
								  pos[1] == '='))
				{
				  // Traitement des op�rateurs "=>" d'Ada et "==" du C.
				  ++ pos;
				}
			}
		}

	  if ( (pos == NULL) || (pos == end) )
		{
#ifdef DEBUG_FILE
		  TRACE0("on espere etre en fin de ligne ...") ;
#endif // DEBUG_FILE
		  // fin de la ligne, du moins on espere
		  while (*(cur++) == ' ') ;
		  s_fprintf(fd_sp, "%s", --cur) ;
		  buffer_sct[0] = '\0' ;
		  cur_buf_pos = buffer_sct ;
		  return ;
		}

	  next = pos + 1 ;
	  mem = *next ;
	  *next = '\0' ;

	  // On saute les espaces
	  while (*(cur++) == ' ') ;

	  s_fprintf(fd_sp, --cur) ;
	  *next = mem ;
	  cur = next ;
	}
}


// Renvoie TRUE si p est un debut d'ident possible i.e. [ '#' | lettre ]
static int file_is_a_begin_ident(const char p)
{
  if (      (p == '#')
	   || ( (p >= 'a') && (p <= 'z') )
	   || ( (p >= 'A') && (p <= 'Z') )
			)
	{
	  return TRUE ;
	}

  return FALSE ;
}

// Renvoie TRUE si p est un interieur d'ident possible i.e.
// [ '#' | lettre | chiffre | '_' ]
static int file_is_a_inside_ident(const char p)
{
  if (      (p == '#')
	   || ( (p >= 'a') && (p <= 'z') )
	   || ( (p >= 'A') && (p <= 'Z') )
	   || ( (p >= '0') && (p <= '9') )
	   ||   (p == '_')
			)
	{
	  return TRUE ;
	}

  return FALSE ;
}

// Passage en mode  'split avance' prenant en compte les '#' du B-Linker
static int advanced_split_mode_si = FALSE ;
void file_set_advanced_split_mode(int new_advanced_split_mode)
{
  advanced_split_mode_si = new_advanced_split_mode ;
}


// Fonction auxiliaire de calcul de longueur d'une chaine, sans
// prendre en compte les informations de relocation entre deux '#'
// (pour les traducteurs)
// ex : file_strlen("#toto#A") = 1
// ex : file_strlen("#toto#titi#A") = 1
//
// Ce calcul n'est fait qu'en mode advanced_split_mode ; sinon strlen est retourne
static size_t file_strlen(const char *src)
{
  if (advanced_split_mode_si == FALSE)
	{
	  // Calcul de base
	  return strlen(src) ;
	}

  const char *s = src ;
  size_t res = 0 ;
  int inside_ident = FALSE ;

  // Buffer local pour copier l'ident en cours d'analyse
  char *buf = new char[strlen(src) + 1] ;
  // Position de recopie courante
  char *ident_buf = buf ;

  while (*s != '\0')
	{
	  if (inside_ident == FALSE)
		{
		  if (file_is_a_begin_ident(*s) == TRUE)
			{
			  // on regarde si on est sur un ident
			  inside_ident = TRUE ;
			  ident_buf = buf ;
			  *(ident_buf++) = *s ;
			}
		  else
			{
			  // Le caractere compte
			  res ++ ;
			}
		}
	  else
		{
		  if (file_is_a_inside_ident(*s) == TRUE)
			{
			  // On est encore en train de parser un ident
			  *(ident_buf++) = *s ;
			}
		  else
			{
			  // Fin de parsing de l'ident
			  inside_ident = FALSE ;
			  *ident_buf = '\0' ;
			  TRACE1("file_strlen() : cur_ident=\"%s\"", buf) ;
			  char *p = strrchr(buf, '#') ;

			  if (p == NULL)
				{
				  // Pas de '#' : la longeur est la longeur de l'ident
				  res += strlen(buf) ;
				}
			  else
				{
				  // A partir de p+1 on a l'ident sans les '#'
				  p ++ ;
				  TRACE2("ident_sans_diezes = \"%s\" -> lgr %d",
						 p,
						 strlen(p)) ;
				  res += strlen(p) ;
				}

			  // On n'oublie pas de compter le caractere courant
			  res ++ ;
			}
		}

	  // Caractere suivant
	  s++ ;
	}

  TRACE2("file_strlen(\"%s\") = %d", src, res) ;
  return res ;
}


// Flush de la ligne courante dans le fichier
static void flush_current_line(void)
{
  int tab_decal = (TAB_SIZE - 1) * indent_si ; 	// decalage du aux tabulations
  size_t cur_len = file_strlen(buffer_sct) ;	// taille a ecrire
  int warning_already_printed = FALSE ; // pour ne pas afficher
  // plusieurs fois le meme msg

  TRACE1("flush_current_line <%s>", buffer_sct) ;

#if 0
  // Code a remettre pour provoquer un arret au flush desire
  // (parametre la valeur suivante :)
  static const int stop_at_call_number = 59 ;
  static int cpt_si = 0 ;
  TRACE1("c'est l'appel %d", ++cpt_si) ;
  assert(cpt_si != stop_at_call_number) ;
#endif

#ifndef NO_SPLIT
  if ( (cur_len + tab_decal <= MAX_LINE_SIZE) || (do_split == FALSE) )
#else
	if (TRUE)
#endif // NO_SPLIT
	  {
		// Ok, le buffer est suffisament petit pour tenir dans la ligne
#ifdef DEBUG_FILE
		TRACE0("ok le buffer est < MAX_LINE_SIZE") ;
#endif
		s_fprintf(fd_sp, "%s", buffer_sct) ;
	  }
	else
	  {
		// Il faut couper
		char *p = buffer_sct ;
		while ( (*p == ' ') || (*p == '\t'))
		  {
			// On saute les blancs en debut de ligne
			p ++ ;
		  }

		if ( 	 ( (*p == '-') && (*(p + 1) == '-') )
				 || ( (*p == '/') && (*(p + 1) == '*') )
				 || ( (*p == '/') && (*(p + 1) == '/') ) )
		  {
			// C'est un commentaire , on n'a pas besoin de le couper
#ifdef  DEBUG_FILE
			TRACE0("On ne coupe pas les commentaires") ;
#endif
			s_fprintf(fd_sp, buffer_sct) ;
		  }
		else if ( strstr(buffer_sct, "//") != NULL )
		  {
			//CT le 12.12.97 : cas des commentaires rajoutes par le
			//traducteur C++ sur une ligne : par ex. "{ // BEGIN"
#ifdef  DEBUG_FILE
			TRACE0("Commentaire rajoute par le traducteur : non coupe");
#endif
			s_fprintf(fd_sp, buffer_sct) ;

		  }
		else
		  {
			// Il faut tronconner !
			char *cur = buffer_sct ;
			char *next ;
			int pos = 0 ;
			int i ;
			int first = TRUE ;
			unsigned max_size = MAX_LINE_SIZE - tab_decal - sep_size ;
			char *point_virgule = strchr(cur, ';') ;
			char *virgule = strchr(cur, ',') ;
			char *end = cur + cur_len - 1 ;
			char *debut_chaine = strchr(cur, '"') ;
			char *fin_chaine = NULL ;
#ifdef  DEBUG_FILE
			TRACE0("il faut couper ...") ;
#endif

			// S'il y a un guillemet ouvrant, alors il s'agit d'un
			// debut de chaine
			//Les chaines ne sont pas secables.
			//On appelle la fonction qui formate les chaines
			if (debut_chaine != NULL)
			  {
				// Il y a une chaine !

#ifdef  DEBUG_FILE
				TRACE0("la ligne comporte une chaine") ;
#endif

				fin_chaine = strchr(debut_chaine + 1, '"') ;
				if (fin_chaine == NULL)
				  {
					// Les '"' fermants ne sont pas sur la ligne ...
					// On ne gere pas la chaine ...
					debut_chaine = NULL ;
				  }
			  }

			// S'il y a une virgule ou un point virgule, il s'agit
			// certainement d'un appel de fonction. On appelle alors
			// la fonction qui formatte les appels de fonction. Cas
			// particulier : point virgule en fin de ligne, alors ce
			//n'est pas un appel de fonction
			if (    (virgule != NULL)
					|| ( (point_virgule != NULL) && (point_virgule != end) ) )
			  {
				// C'est un appel de fonction long !
				TRACE1("XX <%s> is a long function call\n", cur) ;
				flush_long_function_call(cur_len) ;
				return ;
			  }

			int unable_to_split = FALSE ;
#ifdef  DEBUG_FILE
			TRACE4("ici unable_to_split %s cur %s len %d, max_size %d",
				   (unable_to_split == TRUE) ? "TRUE" : "FALSE",
				   cur,
				   file_strlen(cur),
				   max_size) ;
#endif

			while ( (unable_to_split == FALSE) && (file_strlen(cur) >= max_size) )
			  {
				pos =  MAX_LINE_SIZE - tab_decal ;
				next = cur + pos ;

				TRACE3("cur = %c debut_chaine %c on prend next=%c  ...",
					   *cur,
					   (debut_chaine == NULL) ? '0' : *debut_chaine ,
					   (next == NULL) ?  '0' : *next) ;
				if ( (debut_chaine != NULL)
					 && (next >= debut_chaine) && (next <= fin_chaine) )
				  {
					if (cur == debut_chaine)
					  {
						// On est au debut de la chaine. Le decoupage
						// s'impose de lui-meme : il faut couper la
						// chaine !
						TRACE0("on coupe au debut de la chaine") ;
						next = fin_chaine + 1 ;
					  }
					else
					  {
						// Le decoupage coupe la chaine ... il faut eviter cela
						TRACE3("cur = %c debut_chaine %c on prend next=%c  ...",
							   *cur,
							   *debut_chaine,
							   *next) ;
						next = debut_chaine - 1 ;
					  }
				  }
				else
				  {
					// On redescend pour trouver une virgule,
					// un point virgule ou sinon espace
					TRACE1("pos = <%d>", pos) ;
					TRACE1("cur = <%s>", cur) ;
					TRACE1("next = <%s>", next) ;

					// Le dernier caractere ne compte pas ca r sinon on
					// tronque "xxx ;" en "xxx" puis ";"
					next-- ;
					TRACE1("pos = <%d>", pos) ;
					TRACE1("cur = <%s>", cur) ;
					TRACE1("next = <%c>", *next) ;

					while ( (*next != ';') && (*next != ',') && (next > cur) )
					  {
						TRACE1("next = <%c>", *next) ;
						next -- ;
					  }

					TRACE1("next = <%c>", *next) ;

					if (next == cur)
					  {
						// On cherche un espace  !
						// On ne veut pas couper les lignes du type
						// "xxxxx ;" a l'espace de fin
						next = cur + pos - 2 ;
						while ( (*next != ' ') && (next > cur) )
						  {
							next -- ;
						  }
					  }

					if (next == cur)
					  {
						// On cherche une parenthese ouvrante en
						// desespoir de cause !
						next = cur + pos ;
						while ( (*next != '(') && (next > cur) )
						  {
							next -- ;
						  }
					  }

					if (next == cur)
					  {
						if (file_get_accept_unsplittable_lines() == TRUE)
						  {
							; // On accepte la ligne
						  }
						else
						  {
							if (use_warning == TRUE)
							  {
								if ( warning_already_printed  == FALSE )
								  {
								//le msg ne sera affiche qu'une seule fois....
									warning_already_printed = TRUE ;

									fprintf(stderr,
											"Warning :: Unable to split <%s> in lines of less than %d characters\n",
											buffer_sct,
                                                                                        (int)MAX_LINE_SIZE) ;

									fprintf(stderr,
											"(Try to decrease tab space or to increase max line size)\n") ;
								// On memorise le fait que la ligne est insecable
								// Elle sera ensuite traitee comme si elle etait
								// suffisament petite pour etre ecrite d'un bloc

									unable_to_split = TRUE ;
								  }

							  }
							else
							  {
								fprintf(stderr,
										"Error :: Unable to split <%s> in lines of less than %d characters\n",
										buffer_sct,
                                                                                (int)MAX_LINE_SIZE) ;

								fprintf(stderr,
										"(Try to decrease tab space or to increase max line size)\n") ;
								exit(1);
							  }
						  }
					  }
				  }

				if (unable_to_split == FALSE)
				  {
					// Ok, la ligne qui va de cur a next est de bonne taille
					TRACE2("ok on va de '%c' a '%c'", *next, *cur) ;

					if (*next == ' ')
					  {
						*next =  '\0' ;
						if (use_sep == TRUE)
						  {
							s_fprintf(fd_sp, "%s%c\n", cur, sep_char) ;
						  }
						else
						  {
							s_fprintf(fd_sp, "%s\n", cur) ;
						  }
#ifdef  DEBUG_FILE
						TRACE1("on ecrit <%s>", cur) ;
#endif
						*next = ' ' ;
						cur = next + 1 ; // On saute l'espace !
					  }
					else
					  {
						char mem = *(next + 1) ;
						*(next + 1) = '\0' ;
						if (use_sep == TRUE)
						  {
							s_fprintf(fd_sp, "%s%c\n", cur, sep_char) ;
						  }
						else
						  {
							s_fprintf(fd_sp, "%s\n", cur) ;
						  }
#ifdef  DEBUG_FILE
						TRACE1("on ecrit <%s>", cur) ;
#endif
						*(next + 1) = mem ;
						cur = next + 1 ;
					  }

					// On ecrit la ligne suivante avec un tab de plus ...
					for (i = 0 ; i <= indent_si ; i++)
					  {
						s_fprintf(fd_sp, TAB) ;
					  }

					if (first == TRUE)
					  {
						// Une fois la premiere ligne ecrite, les autres
						// sont decalees d'une tabulation donc on pourra y
						// ecrire TAB_SIZE caracteres de moins
						first = FALSE ;
						max_size -= TAB_SIZE ;
						tab_decal += TAB_SIZE ;
					  }
				  }
			  }

			// La ligne qui reste a ecrire est petite (ou insecable) !
#ifdef  DEBUG_FILE
			TRACE1("ok le reste est petit -> <%s>", cur) ;
#endif
			// pas de retour chariot il y est deja !
			s_fprintf(fd_sp, "%s", cur) ;
		  }
	  }

  buffer_sct[0] = '\0' ;
  cur_buf_pos = buffer_sct ;
}

// Renvoie TRUE si la ligne est vide, FALSE sinon
static int is_an_empty_line(const char *line)
{
  const char *p = line ;

  while (*p != '\0')
	{
	  if ( (*p != ' ' ) && (*p != '\t') )
		{
		  return FALSE ;
		}

	  p++ ;
	}

  return TRUE ;
}

// Sauter une ligne
EXTERN void file_skip_line(void)
{
  TRACE0("!! file_skip_line !!") ;
  file_fprintf_raw("\n") ;
}


EXTERN void file_old_new_line(void)
{
  // Affichage de la ligne courante
  flush_current_line() ;
  s_fprintf(fd_sp, "\n") ;

  // Modification de indent_si si push_indent ou pop_indent ont ete utilises
  if (push_indent_si == TRUE)
	{
	  indent_si++;
	  push_indent_si = FALSE;
	}
  if (pop_indent_si == TRUE)
	{
	  indent_si -= pop_indent_level_si ;
	  pop_indent_si = FALSE;
	}

  // Production de l'indentation courante
  for (int i = 0 ; i < indent_si ; i++)
	{
	  file_fprintf(TAB) ;
	}
}


// Passage a la ligne
EXTERN void file_new_line(void)
{
  TRACE2("file_new_line (buffer_sct <%s> is_empty %s)",
		 buffer_sct,
		 (is_an_empty_line(buffer_sct) == TRUE) ? "TRUE" : "FALSE") ;

  if (is_an_empty_line(buffer_sct) == TRUE)
	{
	  // On ne produit pas de ligne vide
	  buffer_sct[0] = '\0' ;
	  cur_buf_pos = buffer_sct ;
	}
  else
	{
	  flush_current_line() ;
	  s_fprintf(fd_sp, "\n") ;
	}

  // Modification de indent_si si push_indent ou pop_indent ont ete utilises
  if (push_indent_si == TRUE)
	{
	  indent_si++;
	  push_indent_si = FALSE;
	}

  if (pop_indent_si == TRUE)
	{
	  indent_si-= pop_indent_level_si ;
#ifdef DEBUG_FILE
	  TRACE2("pop_indent_level_si = %d => indent_si = %d", pop_indent_level_si, indent_si) ;

	  if (indent_si < 0)
		{
		  printf("PANIC : indent_level < 0 -> on le met a 5 !!!\n") ;
		  TRACE0("PANIC : indent_level < 0 -> on le met a 5 !!!") ;
		  indent_si = 5 ;
		}
	  //	ASSERT(indent_si >= 0) ;
#endif
	  pop_indent_si = FALSE;
	}

#ifdef DEBUG_FILE
  TRACE1("-- FILE_NEW_LINE indent_si<%d>", indent_si) ;
#endif // DEBUG_FILE

  // Production de l'indentation courante
  int i ;
  for (i = 0 ; i < indent_si ; i++)
	{
	  file_fprintf(TAB) ;
	}

}

// Ecriture d'une chaine
// fprintf securise
EXTERN void file_fprintf(const char *format_acp, ...)
{
  va_list ap ;

#ifdef DEBUG_FILE
  TRACE0("-- FILE FPRINTF") ;
  {
	char buf[4096] ;
	va_list ap2 ;
	va_start(ap2, format_acp) ;
	if (vsprintf(buf, format_acp, ap2) < 0)
	  {
		perror("I/O error :") ;
		exit(1) ;
	  }

	TRACE1("on a ajoute -> \"%s\"", buf) ;
	va_end(ap2) ;
  }
#endif

  va_start(ap, format_acp) ;

  if (vsprintf(cur_buf_pos, format_acp, ap) < 0)
	{
	  perror("I/O error :") ;
	  exit(1) ;
	}

  cur_buf_pos += strlen(cur_buf_pos) ;

  va_end(ap) ;
}

// Ecriture d'une chaine avec retour chariot force
// fprintf securise
EXTERN void file_fprintf_line(const char *format_acp, ...)
{
  va_list ap ;

#ifdef DEBUG_FILE
  TRACE0("-- FILE FPRINTF_LINE") ;
#endif

  // A FAIRE : regarder si l'on peut enlever le ifdef
#if defined(HPUX)
  va_start(ap, format_acp) ;
#else
  va_start(ap, format_acp) ;
#endif

#if 0
  TRACE1("buffer_sct=<%s>", buffer_sct) ;
#endif

  if (vsprintf(cur_buf_pos, format_acp, ap) < 0)
	{
	  perror("I/O error :") ;
	  exit(1) ;
	}

  va_end(ap) ;
  cur_buf_pos += strlen(cur_buf_pos) ;
  file_new_line() ;
}

// Fonction auxiliaire qui ajoute un entete a la body
EXTERN void add_file_header(T_betree *betree,
									 const char *suffix,
									 const char *tool_name)
{
  time_t now ;
  T_machine *mach = betree->get_root() ;
  if (mach->get_ref_specification() != NULL)
	{
	  mach = mach->get_ref_specification() ;
	}
  const char *machine_name = mach->get_machine_name()->get_name()->get_value() ;
  const char *checksum = betree->get_checksum()->get_value() ;
  char *date ;

  TRACE1("add_file_header(\"%s\", %x)", machine_name) ;

  time(&now) ;
  date = ctime(&now) ; // ctime rend une chaine qui contient le \n
  *(date + strlen(date) - 1) = '\0' ; // On enleve le \n de fin

  file_fprintf_line("-- File %s.%s", machine_name, suffix) ;
  file_fprintf_line("-- generated by Atelier-B/%s on %s", tool_name, date) ;
  file_fprintf_line("-- from input_file %s.imp, checksum %s", machine_name, checksum);
}

// Fonction auxiliaire qui ecrit instance.classe ou classe si instance == NULL
void file_dump_instance(const char *instance_name,
								 const char *class_name)
{
  if (instance_name != NULL)
	{
	  file_fprintf("%s_", instance_name) ;
	}

  file_fprintf("%s", class_name) ;
}

// Ecriture d'une chaine en mode raw (pas de controle de taille)
EXTERN void file_fprintf_raw(const char *format_acp, ...)
{
  va_list ap ;

  TRACE0("-- FILE FPRINTF_RAW") ;

  // On ecrit la ligne en cours ...
  flush_current_line() ;


  // A FAIRE : regarder si l'on peut enlever le ifdef
#if defined(HPUX)
  va_start(ap, format_acp) ;
#else
  va_start(ap, format_acp) ;
#endif

  // En on y va
  if (vfprintf(fd_sp, format_acp, ap) < 0)
	{
	  perror("I/O error :") ;
	  exit(1) ;
	}

  va_end(ap) ;
}

// Ecriture d'une chaine en mode raw2 (pas de controle de taille, pas d'argument)
EXTERN void file_fprintf_raw2(const char *string)
{
  TRACE0("-- FILE FPRINTF_RAW2") ;

  // On ecrit la ligne en cours ...
  flush_current_line() ;
  s_fprintf_raw(fd_sp, string) ;
}


