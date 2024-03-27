/***************************** CLEARSY **************************************
* Fichier : $Id: s_lex.cpp,v 2.0 2007-06-04 08:41:33 jburlando Exp $
* (C) 2008 CLEARSY
*
* Compilations :	*	-DCHECK_INDEX pour verifier que l'on ne deborde pas
*						du buffer de lecture
*					*	-DDUMP_LEXEMS pour dumper les lexemes avant et apres
*						expansion des definitions
*					*	-DDEBUG_LEX pour avoir les traces de l'analyse lexicale
*					*	-DDEBUG_REC_MODE pour les traces du mode record ':' -> '\:'
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
RCS_ID("$Id: s_lex.cpp,v 1.79 2007-06-04 08:41:33 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
/* Includes Composant Local */
#include "c_api.h"

static int first_component_started_sb = FALSE ;

#include "c_lex.h"
#include "c_objman.h"
#include "c_api.h"
#include "s_pstack.h"
#include "c_md5.h"

/* Declarations Forward */
#ifndef FULL_TRACE
static UChar next_car(void) ;
static UChar cur_car(void) ;
static UChar prev_car(void) ;
#else
#define next_car() _next_car(__FILE__, __LINE__)
#define cur_car() _cur_car(__FILE__, __LINE__)
#define prev_car() _prev_car(__FILE__, __LINE__)

static UChar _next_car(const char *file, int line) ;
static UChar _cur_car(const char *file, int line) ;
static UChar _prev_car(const char *file, int line) ;
#endif /* FULL_TRACE */

// Constantes locales
const int DECAL = ('A' - 'a') ;

// Variables statiques locales
#ifdef CHECK_INDEX
static int size_si = 0 ;
#endif
static char starget[4];
static UChar ssource[1];
static UConverter* utf8conv;


//
// Debut donnees parages avec c_lexana.cpp et c_lexstck.cpp (donc pas static)
//
UChar *load_sct_UChar = NULL ;
char *file_name_scp = NULL ;
T_symbol *sym_file_name_scp = NULL ;
char *los_buf = NULL ;
T_pstack *lifo_pos_sop = NULL ;
int index_si ;
int cur_line_si = 0 ;
int cur_column_si = 0 ;
MD5Context ctx ;
unsigned char MD5buf[17] ;

//
// Fin donnees parages avec c_lexana.cpp (donc pas static)
//

//
//	}{ fonctions auxiliaires
//
//static int is_a_letter(int carac)
//{
//  return (	( (carac >= 'a') && (carac <= 'z') )
//			|| ( (carac >= 'A') && (carac <= 'Z') ) ) ;
//}

static int is_a_digit(int carac)
{
  return ( (carac >= '0') && (carac <= '9') ) ;
}

static int is_a_hex_digit(int c)
{
    return (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
}

static int is_a_binary_digit(int c)
{
    return (c >= '0' && c <= '1');
}
//static int is_an_extended_letter(int carac)
//{
//  return (	( (carac >= 'a') && (carac <= 'z') )
//			|| ( (carac >= 'A') && (carac <= 'Z') )
//			|| ( (carac >= '0') && (carac <= '9') )
//			|| (carac == '_')
//			//		 || (carac == '.')				TRAITE A PART
//			|| (carac == '$') ) ;
//}

static int is_space(int carac)
{
  return (    (carac == ' ')
			  || (carac == '\t')
			  || (carac == '\n')
			  || (carac == '\r') // Pour lire les fichiers Windows 95
			  || (carac == 12)) ;// ^L
}

static int is_a_file_name_letter(int carac)
{
#ifdef DEBUG_LEX
  TRACE1("is_a_file_name_letter(%c)", carac) ;
#endif
  return (	( (carac >= 'a') && (carac <= 'z') )
			|| ( (carac >= 'A') && (carac <= 'Z') )
			|| ( (carac >= '0') && (carac <= '9') )
			|| (carac == '_')
			|| (carac == '/')
			|| (carac == '\\')
			|| (carac == '~')
			|| (carac == '.') ) ;
}

// Mise a jour de l'algo de recherche des fichiers
static T_file_fetch_mode file_fetch_mode_si = FFMODE_DEFAULT ;
static T_file_fetcher *file_fetcher_user = 0;

void set_file_fetch_mode(T_file_fetch_mode new_file_fetch_mode)
{
  file_fetch_mode_si = new_file_fetch_mode ;
}

T_file_fetch_mode get_file_fetch_mode(void)
{
  return file_fetch_mode_si ;
}

void set_user_file_fetch_mode(T_file_fetcher *f)
{
    file_fetcher_user = f;
    file_fetch_mode_si = FFMODE_USER;
}

static int do_full_lexem_reset_si = FALSE ;
void force_full_lexems_reset(void)
{
  do_full_lexem_reset_si = TRUE ;
}

void reset_lexem_context(void)
{
  TRACE0("reset_lexem_context") ;

  if (do_full_lexem_reset_si == TRUE)
	{
	  TRACE0("full reset") ;
	  lex_reset_first_lexem() ;
	  lex_reset_last_lexem() ;
	  set_definitions_clause(NULL) ;
	  lex_set_definitions_clause(NULL) ;
	  do_full_lexem_reset_si = FALSE ;
	  set_file_definitions_mode(FALSE) ;
	}
  else
	{
	  TRACE0("soft reset") ;
	  // Les commentaires a la fin du flux precedent sont a
	  // recuperer pour nous !
	  T_lexem *cur_lexem = lex_get_last_lexem() ;
	  T_lexem *last_comment = NULL ;

	  while ( (cur_lexem != NULL) && (cur_lexem->is_a_comment() == TRUE) )
		{
		  last_comment = cur_lexem ;
		  cur_lexem = cur_lexem->get_real_prev_lexem() ;
		}

	  // Dans le cas ou il n'y avait pas de commentaire en fin du
	  // precedent, last_comment vaut NULL et tout va bien quand meme
	  lex_set_first_lexem(last_comment) ;
	  lex_set_last_lexem(last_comment) ;

	  lex_set_definitions_clause(NULL) ;
	}
}

void add_UChar(UChar e, char **p) {
    if (utf8conv == NULL) {
        UErrorCode err = U_ZERO_ERROR;
        utf8conv = ucnv_open("UTF8", &err);
    }
    starget[0] = 0;
    starget[1] = 0;
    starget[2] = 0;
    starget[3] = 0;
    char *target = starget;
    char *targetLimit = target + 4;
    ssource[0] = e;
    const UChar *source = ssource;
    const UChar *sourceLimit = source + 1;
    UErrorCode err  = U_ZERO_ERROR;
    ucnv_fromUnicode(utf8conv,
                     &target,
                     targetLimit,
                     &source,
                     sourceLimit,
                     NULL,
                     1,
                     &err);
    char *init = targetLimit-4;
    while (init != target) {
        *((*p)++) = *init ;
        init = init + 1;
    }
}

// Analyse d'un mot
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
void analyse_word(void)
{
#ifdef DEBUG_LEX
  TRACE2("analyse_word DEBUT cur_line_si %d, cur_column_si %d",
		 cur_line_si,
		 cur_column_si) ;
  ENTER_TRACE ;
#endif

  char *p = los_buf ;
  add_UChar(cur_car(), &p);
  //*(p++) = cur_car() ;

  int line = cur_line_si ;
  int column = cur_column_si ;

  // Pour savoir si l'identifiant contient un point
  int dot_found = FALSE ;

  // Pour savoir si il faut parser un 0 la fois d'apres (cas du '$')
  int need_zero = FALSE ;

  for (;;)
	{
          UChar cour = next_car() ;

	  // Pas la peine de tester is_a_letter pour le premier cas
	  // car ce test a deja ete fait par l'appelant
	  // Attention, si on a un DOT, il ne faut continue que si le
	  // DOT est suivi d'un identifiant. Une CNS pour cela est
	  // que l'item qui suit le DOT soit une lettre.
	  // Sinon, on s'arrete avant le DOT et il sera transforme en L_DOT
	  // le cas suivant.

	  if ( (need_zero == TRUE) && (cour != '0') )
		{
		  // A ameliorer pour la reprise sur erreur
		  *p = '\0' ;
		  parse_error(file_name_scp,
					  line,
					  column,
					  FATAL_ERROR,
					  get_error_msg(E_INVALID_IDENTIFIER_WITH_DOLLAR),
					  los_buf) ;
		}
	  need_zero = FALSE ;

          if (	(cour != 0) && ((u_isIDPart(cour) == TRUE) || (cour == '$')
                        || ( (cour == '.') && (u_isIDStart(load_sct_UChar[index_si + 1])))))
		{
              add_UChar(cour, &p);
                //*(p++) = cour ;
		  if (cour == '.')
			{
			  dot_found = TRUE ;
			}
		  else if (cour == '$')
			{
			  need_zero = TRUE ;
			}
		}
	  else
		{
		  // Fin de mot atteinte !
		  *p = '\0' ;

#ifdef DEBUG_LEX
                  TRACE1("on a reconnu le mot \"%s\"", los_buf) ;
#endif

		  // On regarde si c'est un mot-cle
		  // Inutile si l'identificateur comporte un point
		  T_lex_type lex_type ;
		  int is_a_keyword = (dot_found==TRUE) ? FALSE : lookup_builtin(los_buf,
																		p - los_buf,
																		lex_type) ;

		  if (is_a_keyword == TRUE)
			{
#ifdef DEBUG_LEX
			  TRACE0("c'est un mot-cle !") ;
#endif

			  if (is_a_begin_component_clause(lex_type) == TRUE)
				{
				  if (get_first_component_started() == FALSE)
					{
					  TRACE0("premier composant") ;
					  set_first_component_started(TRUE) ;
					}
				  else
					{
					  // On commence une nouvelle machine
					  // On calcule la cle MD5 avant de cree le lexeme
					  TRACE0("pas premier composant") ;
					  char MD5_key[33] ;
					  MD5Final(MD5buf, &ctx) ;
					  sprintf(MD5_key,
							  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
							  MD5buf[0], MD5buf[1], MD5buf[2], MD5buf[3],
							  MD5buf[4], MD5buf[5], MD5buf[6], MD5buf[7],
							  MD5buf[8], MD5buf[9], MD5buf[10], MD5buf[11],
							  MD5buf[12], MD5buf[13], MD5buf[14], MD5buf[15]) ;

					  TRACE2("checksum -> push(%s) stack %x",
							 MD5_key,
							 lex_get_MD5_key()) ;
					  lex_push_MD5_key_stack(lookup_symbol(MD5_key, 32)) ;
					  MD5Init(&ctx) ;

#ifdef TRACE
					  T_lexem *lexem = get_object_manager()->get_last_lexem() ;
					  TRACE5("on empile (%x %s:%d:%d:%s) dans last_lex_stack",
							 lexem,
							 lexem->get_file_name()->get_value(),
							 lexem->get_file_line(),
							 lexem->get_file_column(),
							 lexem->get_lex_ascii()) ;
#endif

					  T_lexem *cur_lexem = get_object_manager()->get_last_lexem() ;
					  T_lexem *last_lexem = cur_lexem ;

					  while (    (cur_lexem != NULL)
							  && (cur_lexem->is_a_comment() == TRUE) )
						{
						  last_lexem = cur_lexem ;
						  cur_lexem = cur_lexem->prev_lexem ;
						}

					  ASSERT(cur_lexem != NULL) ; // car pas premier composant
					  push_last_lex_stack(last_lexem) ;

					  // On coupe le flux

					  T_lexem *last_code_lexem = last_lexem ;

					  while (   (last_code_lexem != NULL)
							 && (last_code_lexem->is_a_comment() == TRUE) )
						{
						  last_code_lexem = last_code_lexem->prev_lexem ;
						}
					  push_last_code_lexem_stack(last_code_lexem) ;
					  TRACE4("%d:%d %x %s ON COUPE LE FLUX",
							 last_code_lexem->get_file_line(),
							 last_code_lexem->get_file_column(),
							 last_code_lexem,
							 (last_code_lexem->get_lex_ascii() == NULL)
							 	? "<null>" : last_code_lexem->get_lex_ascii()) ;
					  last_code_lexem->set_next_lexem(NULL) ;

					  // Reset variables de T_lexem
					  reset_lexem_context() ;
					}
				  TRACE0("Fin compo->set_push_next_lexem(TRUE)") ;
				  lex_set_push_next_lexem(TRUE) ;
				}
#ifdef DEBUG_LEX
			  TRACE1("lex_type %d", lex_type) ;
#endif
			  T_lexem *lexem ;
			  lexem = new T_lexem(lex_type,NULL,0, line, column,sym_file_name_scp) ;

			}
		  else
			{
			  // C'est une identifiant
#ifdef DEBUG_LEX
			  TRACE2("c'est un identifiant ! on le cree en L%dC%d",
					 line,
					 column) ;
#endif
			  T_lexem *lexem ;
			  lexem = new T_lexem((dot_found == FALSE) ? L_IDENT : L_RENAMED_IDENT,
								  los_buf,
								  p - los_buf,
								  line,
								  column,
								  sym_file_name_scp) ;

			}

		  // C'est tout !
		  // On retourne sur le dernier caractere
		  prev_car() ;

#ifdef DEBUG_LEX
		  EXIT_TRACE ;
		  TRACE0("analyse_word FIN") ;
#endif
		  return ;
		}

	}
}

static int get_hexa_value(char c)
{
    if(c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if(c >= 'a' && c <= 'f')
    {
        return 10 + c - 'a';
    }
    else if(c >= 'A' && c <= 'F')
    {
        return 10 + c - 'A';
    }

    assert(FALSE);
    return 0;
}

static int get_binary_value(char c)
{
    if(c >= '0' && c <= '1')
    {
        return c - '0';
    }

    assert(FALSE);
    return 0;
}

/*!
 * Converti une chaine de caractères contenant un nombre hexadécimal en
 * chaine de caractères contenant le même nombre en décimal.
 *
 * \param src La chaine de caractères avec le nombre décimal.
 * \param dst La chaine de caractères dans laquelle le résultat sera écrit.
 * \param max_size La taille max de caractères à écrire.
 * \return Le nombre de caractères total.
 */
static int convert_hexa(const char *src, char *dst, int max_size)
{
    // Lit la valeur sur un entier 64 bits. On ne passe pas par ssprintf/sscanf
    // car on veut s'assurer que le nombre traduit est bien le bon nombre, et
    // que les nombres que l'on ne sait pas traduire sont bien détectés, ceci
    // de manière portable.
    const char *tmp;
    uint64_t value = 0;

    for(tmp = src; *tmp != '\0'; ++tmp)
    {
        value *= 16;
        value += get_hexa_value(*tmp);
        if(tmp - src > 16)
        {
            // Erreur: nombre trop gros pour être représenté sur 64 bits
            parse_error(file_name_scp,
                        cur_line_si,
                        cur_column_si,
                        CAN_CONTINUE,
                        get_error_msg(E_HEXADECIMAL_LITERAL_OVERFLOW)) ;
            // On place un nombre "correct" afin de permettre de continuer l'analyse
            dst[0] = '0';
            dst[1] = '\0';

            return 1;
        }
    }

    // Réécrit la valeur en décimal
    int sz = 0;
    char temp_buf[21];
    // on écrit d'abord le nombre à l'envers (0xff => 552)
    do
    {
        int current_digit = value % 10;
        value /= 10;
        temp_buf[sz] = (char)('0' + current_digit);
        ++sz;
    } while(value > 0);

    int result = sz;
    if(sz >= max_size)
    {
        // Erreur: nombre trop gros pour être représenté sur 64 bits
        parse_error(file_name_scp,
                    cur_line_si,
                    cur_column_si,
                    CAN_CONTINUE,
                    get_error_msg(E_HEXADECIMAL_LITERAL_OVERFLOW)) ;
        dst[0] = '0';
        dst[1] = '\0';
        return 1;
    }
    // Ensuite, on recopie à l'endroit
    while(sz > 0)
    {
        --sz;
        *dst = temp_buf[sz];
        ++dst;
    }
    *dst = '\0';

    return result;
}

/*!
 * Convertit une chaine de caractères contenant un nombre binaire en
 * chaine de caractères contenant le même nombre en décimal.
 *
 * \param src La chaine de caractères avec le nombre décimal.
 * \param dst La chaine de caractères dans laquelle le résultat sera écrit.
 * \param max_size La taille max de caractères à écrire.
 * \return Le nombre de caractères total.
 */
static int convert_binary(const char *src, char *dst, int max_size)
{
    // Lit la valeur sur un entier 64 bits. On ne passe pas par ssprintf/sscanf
    // car on veut s'assurer que le nombre traduit est bien le bon nombre, et
    // que les nombres que l'on ne sait pas traduire sont bien détectés, ceci
    // de manière portable.
    const char *tmp;
    uint64_t value = 0;

    for(tmp = src; *tmp != '\0'; ++tmp)
    {
        value *= 2;
        value += get_binary_value(*tmp);
        if(tmp - src > 64)
        {
            // Erreur: nombre trop gros pour être représenté sur 64 bits
            parse_error(file_name_scp,
                        cur_line_si,
                        cur_column_si,
                        CAN_CONTINUE,
                        get_error_msg(E_BINARY_LITERAL_OVERFLOW)) ;
            // On place un nombre "correct" afin de permettre de continuer l'analyse
            dst[0] = '0';
            dst[1] = '\0';

            return 1;
        }
    }

    // Réécrit la valeur en décimal
    int sz = 0;
    char temp_buf[21];
    // on écrit d'abord le nombre à l'envers (0xff => 552)
    do
    {
        int current_digit = value % 10;
        value /= 10;
        temp_buf[sz] = (char)('0' + current_digit);
        ++sz;
    } while(value > 0);

    int result = sz;
    if(sz >= max_size)
    {
        // Erreur: nombre trop gros pour être représenté sur 64 bits
        parse_error(file_name_scp,
                    cur_line_si,
                    cur_column_si,
                    CAN_CONTINUE,
                    get_error_msg(E_BINARY_LITERAL_OVERFLOW)) ;
        dst[0] = '0';
        dst[1] = '\0';
        return 1;
    }
    // Ensuite, on recopie à l'endroit
    while(sz > 0)
    {
        --sz;
        *dst = temp_buf[sz];
        ++dst;
    }
    *dst = '\0';

    return result;
}

// Analyse d'un nombre
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
static void analyse_number(void)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_number DEBUT") ;
  ENTER_TRACE ;
#endif

  char *p = los_buf ;
  add_UChar(cur_car(), &p);
  //*(p++) = cur_car() ;

  int line = cur_line_si ;
  int column = cur_column_si ;
  UChar cour;

  if(cur_car() == '0')
  {
      // Maybe an hexadecimal value
      cour = next_car();
      if(cour == 'x')
      {
          // Nombre hexadecimal

          cour = next_car();
          if(is_a_hex_digit(cour))
          {
              add_UChar(cour, &p);
                //*(p++) = cour;
                for(;;)
                {
                    cour = next_car() ;

                    if (is_a_hex_digit(cour) == TRUE)
                          {
                        add_UChar(cour, &p);
                          //*(p++) = cour ;
                          }
                    else
                          {
                            // Fin de nombre hexadecimal atteinte !
                            *p = '\0' ;

                            // Taille max d'un entier 64 bits affiché en décimal: 20 + '\0'
                            char temp_buf[21];
                            int sz = convert_hexa(los_buf, temp_buf, 21);
          #ifdef DEBUG_LEX
                            TRACE1("on a reconnu le nombre %s", los_buf) ;
          #endif
                            (void)new T_lexem(L_NUMBER,
                                                    temp_buf,
                                                    sz,
                                                    line,
                                                    column,
                                                    sym_file_name_scp) ;

                            // C'est tout !
                            // On retourne sur le dernier caractere
                            prev_car() ;

          #ifdef DEBUG_LEX
                            EXIT_TRACE ;
                            TRACE0("analyse_number FIN") ;
          #endif
                            return ;
                }
                }
          }
          else
          {
              // Literal zero
              *p = '\0' ;

    #ifdef DEBUG_LEX
              TRACE1("on a reconnu le nombre %s", los_buf) ;
    #endif
              (void)new T_lexem(L_NUMBER,
                                los_buf,
                                p - los_buf,
                                line,
                                column,
                                sym_file_name_scp) ;

              // C'est tout !
              // On retourne sur le dernier caractere
              prev_car() ;
              prev_car();

    #ifdef DEBUG_LEX
              EXIT_TRACE ;
              TRACE0("analyse_number FIN") ;
    #endif
              return ;
          }
      }
      else if(cour == 'b')
      {
          // Nombre hexadecimal

          cour = next_car();
          if(is_a_binary_digit(cour))
          {
              add_UChar(cour, &p);
                for(;;)
                {
                    cour = next_car() ;

                    if (is_a_binary_digit(cour) == TRUE)
                          {
                        add_UChar(cour, &p);
                          }
                    else
                          {
                            // Fin de nombre binaire atteinte !
                            *p = '\0' ;

                            // Taille max d'un entier 64 bits affiché en décimal: 20 + '\0'
                            char temp_buf[21];
                            int sz = convert_binary(los_buf, temp_buf, 21);
          #ifdef DEBUG_LEX
                            TRACE1("on a reconnu le nombre %s", los_buf) ;
          #endif
                            (void)new T_lexem(L_NUMBER,
                                                    temp_buf,
                                                    sz,
                                                    line,
                                                    column,
                                                    sym_file_name_scp) ;

                            // C'est tout !
                            // On retourne sur le dernier caractere
                            prev_car() ;

          #ifdef DEBUG_LEX
                            EXIT_TRACE ;
                            TRACE0("analyse_number FIN") ;
          #endif
                            return ;
                }
                }
          }
          else
          {
              // Literal zero
              *p = '\0' ;

    #ifdef DEBUG_LEX
              TRACE1("on a reconnu le nombre %s", los_buf) ;
    #endif
              (void)new T_lexem(L_NUMBER,
                                los_buf,
                                p - los_buf,
                                line,
                                column,
                                sym_file_name_scp) ;

              // C'est tout !
              // On retourne sur le dernier caractere
              prev_car() ;
              prev_car();

    #ifdef DEBUG_LEX
              EXIT_TRACE ;
              TRACE0("analyse_number FIN") ;
    #endif
              return ;
          }
      }
      else if(is_a_digit(cour) == FALSE && cour != '.')
      {
          // Literal zero
          *p = '\0' ;

#ifdef DEBUG_LEX
          TRACE1("on a reconnu le nombre %s", los_buf) ;
#endif
          (void)new T_lexem(L_NUMBER,
                            los_buf,
                            p - los_buf,
                            line,
                            column,
                            sym_file_name_scp) ;

          // C'est tout !
          // On retourne sur le dernier caractere
          prev_car() ;

#ifdef DEBUG_LEX
          EXIT_TRACE ;
          TRACE0("analyse_number FIN") ;
#endif
          return ;
      }
      else {
          prev_car();
      }
  }

  int real = 0;
  int next_cour;
  // On parse la suite du nombre en décimal
  for (;;)
        {
          int cour = next_car() ;

          if (is_a_digit(cour) == TRUE)
          {
              add_UChar(cour, &p);
                //*(p++) = cour ;
          }
          else {
              next_cour = next_car();
              prev_car() ;
              if (real ==0 && cour == '.' && is_a_digit(next_cour)) {
              real = 1;
              add_UChar(cour, &p);
                //*(p++) = cour ;
          }
          else
                {
                  // Fin de nombre atteinte !
                  *p = '\0' ;

#ifdef DEBUG_LEX
                  TRACE1("on a reconnu le nombre %s", los_buf) ;
#endif
                  if (real == 0) {
                  (void)new T_lexem(L_NUMBER,
                                          los_buf,
                                          p - los_buf,
                                          line,
                                          column,
                                          sym_file_name_scp) ;
              }
                  else {
                      (void)new T_lexem(L_REAL,
                                              los_buf,
                                              p - los_buf,
                                              line,
                                              column,
                                              sym_file_name_scp) ;
                  }

                  // C'est tout !
                  // On retourne sur le dernier caractere
                  prev_car() ;

#ifdef DEBUG_LEX
                  EXIT_TRACE ;
                  TRACE0("analyse_number FIN") ;
#endif
                  return ;
                }
      }
        }
}

// Analyse d'une chaine
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
static void analyse_string(void)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_string DEBUT") ;
  ENTER_TRACE ;
#endif

  char *p = los_buf ;

  int line = cur_line_si ;
  int column = cur_column_si ;
  int enable_new_line = FALSE ;

  for(;;)
	{
          UChar cour = next_car() ;
#ifdef DEBUG_LEX
	  TRACE3("cour = '%c' (%03d) enable_new_line = %s",
			 cour,
			 cour,
			 (enable_new_line == TRUE) ? "TRUE" : "FALSE") ;
#endif

	  if (cour == '\0')
		{
		  parse_error(file_name_scp,
					  cur_line_si,
					  cur_column_si,
					  MULTI_LINE,
					  get_error_msg(E_EOF_INSIDE_STRING)) ;

		  parse_error(file_name_scp,
					  line,
					  column,
					  FATAL_ERROR,
					  get_error_msg(E_START_OF_STRING)) ;
		  return ;
		}

	  if (cour == '\n')
		{
		  if (enable_new_line == FALSE)
			{
			  parse_error(file_name_scp,
						  cur_line_si,
						  cur_column_si,
						  MULTI_LINE,
						  get_error_msg(E_NEW_LINE_INSIDE_STRING)) ;

			  parse_error(file_name_scp,
						  line,
						  column,
						  FATAL_ERROR,
						  get_error_msg(E_START_OF_STRING)) ;
			  return ;
			}
		  else
			{
			  // OK, on repart sur la ligne suivante
			  TRACE0("nl detecte") ;
			  enable_new_line = FALSE ;
			  continue ;
			}
		}

	  if (cour == '"')
		{
		  *p = '\0' ;
#ifdef DEBUG_LEX
		  TRACE1("fin de chaine atteinte ! -> valeur = \"%s\"", los_buf) ;
#endif

		  // Fabrication du lexeme
		  (void)new T_lexem(L_STRING,
					  los_buf,
					  p - los_buf,
					  line,
					  column,
					  sym_file_name_scp) ;

#ifdef DEBUG_LEX
		  EXIT_TRACE ;
		  TRACE0("analyse_string FIN") ;
#endif
		  return ;
		}

	  if (cour == '\\')
		{
		  cour = next_car() ;
#ifdef DEBUG_LEX
		  TRACE2("cour = '%c' (%03d)", cour, cour) ;
#endif
		  if (is_space(cour))
			{
			  // La chaine continue a la ligne suivante
			  enable_new_line = TRUE ;
			  prev_car() ;
			}
		  else
			{
			  // c'est une sequence de controle
			  // On memorise le caractere suivant sans condition
			  *(p++) = '\\' ;
			  // FALL_INTO
			}
		}

	  if (enable_new_line == FALSE)
		{
		  // On ignore les caracteres qui suivent un antislash
#ifdef DEBUG_LEX
		  TRACE0("on bufferise") ;
#endif
                  add_UChar(cour, &p);
                    //*(p++) = cour ;
		}
#ifdef DEBUG_LEX
	  else
		{
		  TRACE0("on ne bufferise pas") ;
		}
#endif
	}

}

// Analyse d'un commentaire C++
// On est place en debut de commentaire, sur le deuxieme '/' 
// On rend la main sur le '\n' de fin (ou le '\0')
static void analyse_cpp_comment(void)
{
	int keep_comment = FALSE ;
#ifdef DEBUG_LEX
	TRACE0("analyse_cpp_comment DEBUT") ;
	ENTER_TRACE ;
#endif

	char *p = los_buf ;

	if (next_car() == '?')
	{
		keep_comment = TRUE ;
	}
	else
	{
		prev_car() ;
	}

	int line = cur_line_si ;
	int column = cur_column_si ;

	for(;;)
	{
                UChar cour = next_car() ;

		if ((cour == '\0') || (cour == '\n'))
		{
#ifdef DEBUG_LEX
			TRACE0("fin de commentaire C++ atteinte !") ;
#endif
                        *p = '\0' ;
#ifdef DEBUG_LEX
			TRACE1("commentaire = \"%s\"", los_buf) ;
#endif

			// Fabrication du lexeme
			if (keep_comment == TRUE)
			{
				(void)new T_lexem(L_CPP_KEPT_COMMENT,
					los_buf,
                                        p - los_buf,
					line,
					column - 2,
					sym_file_name_scp) ;
			}
			else
			{
				(void)new T_lexem(L_CPP_COMMENT,
					los_buf,
					p - los_buf,
					line,
					column - 1,
					sym_file_name_scp) ;
			}

#ifdef DEBUG_LEX
			EXIT_TRACE ;
			TRACE0("analyse_cpp_comment FIN") ;
#endif
			return ;
		}

                add_UChar(cour, &p);
                  //*(p++) = cour ;
	}
}

// Analyse d'un commentaire C
// On est place en debut de commentaire, sur le '*' 
// On rend la main sur le '/' de fin 
static void analyse_c_comment(void)
{
  int star_found = FALSE ;
  int slash_found = FALSE ;
  int keep_comment = FALSE ;
#ifdef DEBUG_LEX
  TRACE0("analyse_c_comment DEBUT") ;
  ENTER_TRACE ;
#endif

  char *p = los_buf ;

  if (next_car() == '?')
	{
	  keep_comment = TRUE ;
	}
  else
	{
	  prev_car() ;
	}

  int line = cur_line_si ;
  int column = cur_column_si ;

  for(;;)
	{
          UChar cour = next_car() ;

	  if (cour == '\0')
		{
			// Si le commentaire n'est pas un commentaire C++, on affiche
			// un avertissement
			lex_warning(file_name_scp,
				cur_line_si,
				cur_column_si,
				BASE_WARNING,
				get_warning_msg(W_EOF_INSIDE_COMMENT)) ;
			TRACE2("ICI EOF INSIDE COMMENT cur_line_si %d cur_column_si %d",
				cur_line_si,
				cur_column_si) ;

			lex_warning(file_name_scp,
				line,
				column - 2,
				BASE_WARNING,
				get_warning_msg(W_START_OF_COMMENT)) ;
			return ;
		}

	  if ( (star_found == TRUE) && (cour == '/') )
		{
#ifdef DEBUG_LEX
		  TRACE0("fin de commentaire atteinte !") ;
#endif

          if ( (p - los_buf >= 2) && (*(p - 2) == '?') && (keep_comment == TRUE))
			{
#ifdef DEBUG_LEX
			  TRACE0("c'est un commentaire a conserver !") ;
#endif
			  *(p - 2) = '\0' ;
			}
		  else
			{
			  *(p - 1) = '\0' ;
			  keep_comment = FALSE ;
			}

		  *(--p) = '\0' ;
#ifdef DEBUG_LEX
		  TRACE1("commentaire = \"%s\"", los_buf) ;
#endif

		  // Fabrication du lexeme
		  if (keep_comment == TRUE)
			{
			  (void)new T_lexem(L_KEPT_COMMENT,
						  los_buf,
						  p - los_buf - 1,
						  line,
						  column - 2,
						  sym_file_name_scp) ;
			}
		  else
			{
			  (void)new T_lexem(L_COMMENT,
						  los_buf,
						  p - los_buf,
						  line,
						  column - 1,
						  sym_file_name_scp) ;
			}

#ifdef DEBUG_LEX
		  EXIT_TRACE ;
		  TRACE0("analyse_c_comment FIN") ;
#endif
		  return ;
		}
	  else if (cour == '/')
		{
			slash_found = TRUE ;
		}
	  else if (cour == '*')
		{
		  if (slash_found == TRUE)
			{
			  lex_warning(file_name_scp,
						  cur_line_si,
						  cur_column_si -1,
						  BASE_WARNING,
						  get_warning_msg(W_NESTED_COMMENT)) ;

			  lex_warning(file_name_scp,
						  line,
						  column - 1,
						  BASE_WARNING,
						  get_warning_msg(W_START_OF_COMMENT)) ;
			}

		  slash_found = FALSE ;
		  star_found = TRUE ;
		}
	  else
		{
		  slash_found = FALSE ;
		  star_found = FALSE ;
		}

          add_UChar(cour, &p);
            //*(p++) = cour ;
	}

}


// Analyse des operateurs qui commencent par '<'
static void analyse_op_starting_with_less(int cur_line_ip,
												   int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_less") ;
#endif
  UChar cour = next_car() ;

  int nb_next = 0 ; // Pour pouvoir faire machine arriere en cas de pb
  int line = cur_line_ip ;
  int column = cur_column_ip ;

  if (is_a_file_name_letter(cour) == TRUE)
	{
#ifdef DEBUG_LEX
	  TRACE0("debut de reconnaissance de <file>") ;
#endif
	  char *p = los_buf ;
	  while(is_a_file_name_letter(cour) == TRUE)
		{
              add_UChar(cour, &p);
                //*(p++) = (char)cour ;
		  cour = next_car() ;
		  nb_next ++ ;
		}

#ifdef DEBUG_LEX
	  TRACE1("cour = %c", cour) ;
#endif // DEBUG_LEX
	  if (cour == '>')
		{
		  // C'est bon !
		  *p = '\0' ;
		  (void)new T_lexem(L_LIB_FILE_NAME,
					  los_buf,
					  p - los_buf,
					  line,
					  column,
					  sym_file_name_scp) ;
		  return ;
		}
	}

  // Ici erreur, on rembobine !
#ifdef DEBUG_LEX
  TRACE1("on rembobine les %d next ....", nb_next) ;
#endif // DEBUG_LEX
  while (nb_next-- != 0)
	{
	  cour = prev_car() ;
	}

#ifdef DEBUG_LEX
  TRACE0("entree dans le cas general") ;
#endif // DEBUG_LEX

  switch(cour)
	{
	case '=' :
	  {
		cour = next_car() ;
		if (cour == '>')
		  {
			(void)new T_lexem(L_EQUIVALENT,
						NULL,
						0,
						cur_line_ip,
						cur_column_ip,
						sym_file_name_scp) ;
		  }
		else
		  {
			(void)new T_lexem(L_LESS_THAN_OR_EQUAL,
						NULL,
						0,
						cur_line_ip,
						cur_column_ip,
						sym_file_name_scp) ;
			prev_car() ;
		  }
		return ;
	  }

	case ':' :
	  {
		(void)new T_lexem(L_INCLUDED,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '<' :
	  {
		cour =next_car() ;
		if (cour == ':')
		  {
			(void)new T_lexem(L_STRICT_INCLUDED,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			return ;
		  }
		else if (cour == '|')
		  {
			(void)new T_lexem(L_ANTIRESTRICTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			return ;
		  }
		else if (cour == '-') {
			cour =next_car() ;
			if (cour == '>') {
				if (get_allow_total_and_surjective_relation_operators() == TRUE) {
  					cour =next_car() ;
					if (cour == '>') { //<<->>
						(void)new T_lexem(L_TOTAL_SURJ_RELATION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
						break;
					}
					else { //<<->
						(void)new T_lexem(L_TOTAL_RELATION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
						break;
					}
				}
				else {
					char buf[3] ;
					parse_error(file_name_scp,
								cur_line_ip,
								cur_column_ip,
								CAN_CONTINUE,
								get_error_msg(E_TOTAL_RELATIONS_NOT_ALLOWED),
								buf) ;
					}
				}

			else
				{
				char buf[4] ;
				sprintf(buf, "<<%c", cour) ;
				parse_error(file_name_scp,
							cur_line_ip,
							cur_column_ip,
							CAN_CONTINUE,
							get_error_msg(E_TOKEN_PARSE_ERROR),
							buf) ;
				}

		}
		else
			{
			char buf[4] ;
			sprintf(buf, "<<%c", cour) ;
			parse_error(file_name_scp,
						cur_line_ip,
						cur_column_ip,
						CAN_CONTINUE,
						get_error_msg(E_TOKEN_PARSE_ERROR),
						buf) ;
			}
		}

		case '-' :
		{
			cour = next_car() ;

			if (cour == '-')
			{
				(void)new T_lexem(L_RETURNS,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			}
			else if (cour == '>')
			{
				cour = next_car() ;
				if (cour == '>') // <->>
				{
					if (get_allow_total_and_surjective_relation_operators() == TRUE) {
						(void)new T_lexem(L_SURJ_RELATION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
                        break;
					}
					else {
						char buf[3] ;
						parse_error(file_name_scp,
									cur_line_ip,
									cur_column_ip,
									CAN_CONTINUE,
									get_error_msg(E_TOTAL_RELATIONS_NOT_ALLOWED),
									buf) ;
                        prev_car() ;
					}
				}
				else
				{
                    prev_car() ;
					(void)new T_lexem(L_SET_RELATION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
				}
			}
			else
			{
				(void)new T_lexem(L_TAIL_INSERT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
				prev_car() ;
			}
			return ;
		}

	case '+' :
	  {
		(void)new T_lexem(L_LEFT_OVERLOAD,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '|' :
	  {
		(void)new T_lexem(L_RESTRICTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '>' :
	  {
		(void)new T_lexem(L_EMPTY_SEQ,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_LESS_THAN,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}

}

// Analyse des operateurs qui commencent par '>'
static void analyse_op_starting_with_greater(int cur_line_ip,
													  int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_greater") ;
#endif

  UChar cour = next_car() ;

  switch(cour)
	{
    case '=' :
      {
              (void)new T_lexem(L_GREATER_THAN_OR_EQUAL,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
              return ;
     }
	case '<' :
	  {
		(void)new T_lexem(L_DIRECT_PRODUCT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '+' :
	  {
		if (next_car() == '>')
		  {
			(void)new T_lexem(L_PARTIAL_INJECTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			return ;
		  }
        char buf[4] ;
		sprintf(buf, ">+%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }

	case '-' :
	  {
		if (next_car() == '>')
		  {
			cour = next_car() ;
			if (cour == '>')
			  {
				(void)new T_lexem(L_BIJECTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
				return ;
			  }
			else
			  {
				prev_car() ;
				(void)new T_lexem(L_TOTAL_INJECTION,NULL,0,
							cur_line_ip,cur_column_ip,sym_file_name_scp) ;
				return ;
			  }
		  }
        char buf[4] ;
		sprintf(buf, ">-%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_GREATER_THAN,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}
}

// Analyse des operateurs qui commencent par '|'
static void analyse_op_starting_with_pipe(int cur_line_ip,
												   int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_pipe") ;
#endif

  UChar cour = next_car() ;

  switch(cour)
	{
	case '|' :
	  {
		(void)new T_lexem(L_PARALLEL,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '-' :
	  {
		cour = next_car() ;
		if (cour == '>')
		  {
			(void)new T_lexem(L_MAPLET,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			return ;
		  }
        char buf[4] ;
		sprintf(buf, "|-%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }

	case '>' :
	  {
		cour = next_car() ;

		if (cour == '>')
		  {
			(void)new T_lexem(L_ANTICORESTRICTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		  }
		else
		  {
			(void)new T_lexem(L_CORESTRICTION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			prev_car() ;
		  }
		return ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_PIPE,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}
}

// Analyse des operateurs qui commencent par '/'
static void analyse_op_starting_with_slash(int cur_line_ip,
													int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_slash") ;
#endif

  UChar cour  = next_car() ;

  switch(cour)
	{
	case '\\' :
	  {
		(void)new T_lexem(L_INTERSECT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

        case '=' :
	  {
		(void)new T_lexem(L_DIFFERENT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case ':' :
	  {
		(void)new T_lexem(L_NOT_BELONGS,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '|' :
	  {
		cour = next_car() ;
		if (cour == '\\')
		  {
			(void)new T_lexem(L_HEAD_RESTRICT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			return ;
		  }
        char buf[4] ;
		sprintf(buf, "/|%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }

	case '<' :
	  {
		cour = next_car() ;

		if (cour == ':')
		  {
			(void)new T_lexem(L_NOT_INCLUDED,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		  }
		else if (cour == '<')
		  {
			cour = next_car() ;

			if (cour == ':')
			  {
				(void)new T_lexem(L_NOT_STRICT_INCLUDED,NULL,0,
							cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			  }
			else
			  {
				char buf[5] ;
				sprintf(buf, "/<<%c", cour) ;
				parse_error(file_name_scp,
							cur_line_ip,
							cur_column_ip,
							CAN_CONTINUE,
							get_error_msg(E_TOKEN_PARSE_ERROR),
							buf) ;
			  }
		  }
		else
		  {
			char buf[4] ;
			sprintf(buf, "/<%c", cour) ;
			parse_error(file_name_scp,
						cur_line_ip,
						cur_column_ip,
						CAN_CONTINUE,
						get_error_msg(E_TOKEN_PARSE_ERROR),
						buf) ;
		  }
		return ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_DIVIDES,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}
}

// Analyse des operateurs qui commencent par '\\'
static void analyse_op_starting_with_backslash(int cur_line_ip,
														int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_backslash") ;
#endif

  UChar cour  = next_car() ;

  switch(cour)
	{
	case '/' :
	  {
		(void)new T_lexem(L_UNION,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '|' :
	  {
		cour = next_car() ;
		if (cour == '/')
		  {
			(void)new T_lexem(L_TAIL_RESTRICT,
						NULL,
						0,
						cur_line_ip,
						cur_column_ip,
						sym_file_name_scp) ;
			return ;
		  }
		else if (cour == '|')
		  {
			(void)new T_lexem(L_LR_PARALLEL,
						NULL,
						0,
						cur_line_ip,
						cur_column_ip,
						sym_file_name_scp) ;
			return ;
		  }
		else
		  {
			char buf[4] ;
			sprintf(buf, "\\|%c", cour) ;
			parse_error(file_name_scp,
						cur_line_ip,
						cur_column_ip,
						CAN_CONTINUE,
						get_error_msg(E_TOKEN_PARSE_ERROR),
						buf) ;
		  }
	  }
	case ';' :
	  {
		(void)new T_lexem(L_LR_SCOL,
					NULL,
					0,
					cur_line_ip,
					cur_column_ip,
					sym_file_name_scp) ;
		return ;
	  }

	default :
	  {
		char buf[3] ;
		sprintf(buf, "\\%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }
	}
}

// Analyse des operateurs qui commencent par '+'
static void analyse_op_starting_with_plus(int cur_line_ip,
                                                   int cur_column_ip)
{
#ifdef DEBUG_LEX
    TRACE0("analyse_op_starting_with_plus") ;
#endif

    UChar cour = next_car() ;

    switch(cour)
    {
    case '-' :
        {
            cour = next_car() ;
            if (cour == '>')
            {
                cour = next_car() ;
                if (cour == '>')
                {
                    (void)new T_lexem(L_PARTIAL_SURJECTION,NULL,0,
                                      cur_line_ip,cur_column_ip,sym_file_name_scp) ;
                }
                else
                {
                    prev_car() ;
                    (void)new T_lexem(L_PARTIAL_FUNCTION,NULL,0,
                                      cur_line_ip,cur_column_ip,sym_file_name_scp) ;
                }
                return ;
            }
            char buf[4] ;
            sprintf(buf, "+-%c", cour) ;
            parse_error(file_name_scp,
                        cur_line_ip,
                        cur_column_ip,
                        CAN_CONTINUE,
                        get_error_msg(E_TOKEN_PARSE_ERROR),
                        buf) ;
        }

    default :
    {
        prev_car() ;
        (void)new T_lexem(L_PLUS,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
    }
}
}

// Analyse des operateurs qui commencent par '-'
static void analyse_op_starting_with_minus(int cur_line_ip,
													int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_plus") ;
#endif

  UChar cour = next_car() ;

  switch(cour)
	{
	case '>' :
	  {
		(void)new T_lexem(L_HEAD_INSERT,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

        case '-' :
	  {
		cour = next_car() ;
		if (cour == '>')
		  {
			cour = next_car() ;
			if (cour == '>')
			  {
				(void)new T_lexem(L_TOTAL_SURJECTION,NULL,0,
							cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			  }
			else
			  {
				prev_car() ;
				(void)new T_lexem(L_TOTAL_FUNCTION,NULL,0,
							cur_line_ip,cur_column_ip,sym_file_name_scp) ;
			  }
			return ;
		  }
		char buf[4] ;
		sprintf(buf, "--%c", cour) ;
		parse_error(file_name_scp,
					cur_line_ip,
					cur_column_ip,
					CAN_CONTINUE,
					get_error_msg(E_TOKEN_PARSE_ERROR),
					buf) ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_MINUS,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}
}

// Analyse des operateurs qui commencent par '='
static void analyse_op_starting_with_equal(int cur_line_ip,
													int cur_column_ip)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_op_starting_with_equal") ;
#endif

  UChar cour = next_car() ;

  switch(cour)
	{
	case '=' :
	  {
		(void)new T_lexem(L_REWRITES,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	case '>' :
	  {
		(void)new T_lexem(L_IMPLIES,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
		return ;
	  }

	default :
	  {
		prev_car() ;
		(void)new T_lexem(L_EQUAL,NULL,0,cur_line_ip,cur_column_ip,sym_file_name_scp) ;
	  }
	}
}

// Analyse des tokens complexes
static void analyse_token(int cour)
{
#ifdef DEBUG_LEX
  TRACE0("analyse_token") ;
#endif

  switch(cour)
	{
	case '<' :
	  {
		analyse_op_starting_with_less(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '>' :
	  {
		analyse_op_starting_with_greater(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '|' :
	  {
		analyse_op_starting_with_pipe(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '+' :
	  {
		analyse_op_starting_with_plus(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '-' :
	  {
		analyse_op_starting_with_minus(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '=' :
	  {
		analyse_op_starting_with_equal(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '\\' :
	  {
		analyse_op_starting_with_backslash(cur_line_si,cur_column_si) ;
		return ;
	  }

	case '.' :
	  {
		cour = next_car() ;
		if (cour == '.')
		  {
			(void)new T_lexem(L_INTERVAL,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
		  }
		else
		  {
			prev_car() ;
			(void)new T_lexem(L_DOT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		  }

		return ;
	  }

	case '$' :
	  {
		cour = next_car() ;
		if (cour == '0')
		  {
			(void)new T_lexem(L_SCOPE,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
		  }
		else
		  {
			char buf[3] ;
			sprintf(buf, "$%c", cour) ;
			parse_error(file_name_scp,
						cur_line_si,
						cur_column_si,
						CAN_CONTINUE,
						get_error_msg(E_TOKEN_PARSE_ERROR),
						buf) ;
			// On recule d'un cran pour retourner juste apres le "$"
			prev_car() ;
		  }

		return ;
	  }

	case '\'' :
	  {
		(void)new T_lexem(L_RECORD_ACCESS,
					NULL,0,
					cur_line_si,
					cur_column_si,
					sym_file_name_scp) ;
		return ;
	  }

	default :
	  {
		TRACE1("parse error carac = %d", cour) ;
		ASSERT(file_name_scp != NULL) ;
		parse_error(file_name_scp,
					cur_line_si,
					cur_column_si,
					CAN_CONTINUE,
					get_error_msg(E_PARSE_ERROR),
					cour) ;
	  }
	}
}

void get_lexems(void)
{
  TRACE0("get_lexems") ;

  // Il faut une LIFO car les checksums sont recuperes en parcourant
  // la liste des Betrees a l'envers
  lex_new_MD5_key_stack() ;
  lex_new_expanded_MD5_key_stack() ;

  // Reallocation des piles de lexemes
  new_first_lex_stack() ;
  new_last_lex_stack() ;
  new_last_code_lexem_stack() ;

  // Reset des variables globales
  lex_reset_first_lexem() ;
  lex_reset_last_lexem() ;

  // On fait set_push_next_lexem(FALSE) pour initialiser
  // sauf si on est en train de lire un fichier de DEFINITIONS
  if (get_file_definitions_mode() == FALSE)
	{
	  TRACE0("Init->set_push_next_lexem(FALSE)") ;
	  lex_set_push_next_lexem(FALSE) ;
	}

  for (;;)
	{
          UChar cour = next_car() ;

#ifdef DEBUG_LEX
	  TRACE4("cour = '%c' (%03d) L%dC%d", cour, cour, cur_line_si, cur_column_si) ;
#endif

	  // On saute les espaces
	  while (is_space(cour))
		{
		  cour = next_car() ;
#ifdef DEBUG_LEX
		  TRACE2("skip cour = '%c' (%03d)", cour, cour) ;
#endif
		}

#ifdef DEBUG_LEX
	  TRACE4("cour = '%c' (%03d) L%dC%d", cour, cour, cur_line_si, cur_column_si) ;
#endif
	  // On analyse
	  if (cour == '\0')
		{
		  TRACE0("fin du fichier") ;
		  push_last_lex_stack(get_object_manager()->get_last_lexem()) ;
		  push_last_code_lexem_stack(get_object_manager()->get_last_lexem()) ;
		  return ;
		}
	  else if (cour == '/')
		{
		  // Debut de commentaire ?
		  cour = next_car() ;

		  if (cour == '*')
			{
			  // On est dans un commentaire C
			  analyse_c_comment() ;
			}
		  else if(cour == '/')
		  {
			  // On est dans un commentaire C++
			  analyse_cpp_comment();
		  }
		  else
			{
			  prev_car() ; // necessaire pour position
			  analyse_op_starting_with_slash(cur_line_si,cur_column_si) ;
			}
		}
          else if (cour == '"')
		{
		  // Debut de chaine
		  analyse_string() ;
		}
	  else if (cour == ';')
		{
		  (void)new T_lexem(L_SCOL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == ')')
		{
		  (void)new T_lexem(L_CLOSE_PAREN,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '(')
		{
		  (void)new T_lexem(L_OPEN_PAREN,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '{')
		{
		  if (next_car() == '}')
			{
			  (void)new T_lexem(L_EMPTY_SET,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
			}
		  else
			{
			  prev_car() ;
			  (void)new T_lexem(L_OPEN_BRACES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
			}
		}
	  else if (cour == '}')
		{
		  (void)new T_lexem(L_CLOSE_BRACES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '[')
		{
		  if (next_car() == ']')
			{
			  (void)new T_lexem(L_EMPTY_SEQ,NULL,0,cur_line_si,cur_column_si -1,sym_file_name_scp) ;
			}
		  else
			{
			  prev_car() ;
			  (void)new T_lexem(L_OPEN_BRACKET,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
			}
		}
	  else if (cour == ']')
		{
		  (void)new T_lexem(L_CLOSE_BRACKET,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
          else if (cour == ':' || cour == 0x2208)
		{
		  cour = next_car() ;
                  if (cour == ':' || cour == 0x2208)
			{
			  (void)new T_lexem(L_BECOMES_MEMBER_OF,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
			}
		  else if (cour == '=')
			{
			  (void)new T_lexem(L_AFFECT,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
			}
#ifdef BALBULETTE
		  else if (cour == '|')
			{
			  (void)new T_lexem(L_BECOMES_SUCH_THAT,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
			}
#endif
		  else
			{
			  prev_car() ;
			  (void)new T_lexem(L_BELONGS,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
			}
		}
	  else if (cour == '^')
		{
		  (void)new T_lexem(L_CONCAT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '*')
                {
              cour = next_car() ;
              if (cour == '*')
              {
                  (void)new T_lexem(L_POWER,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
              }
             else
              {
                  prev_car() ;
                  (void)new T_lexem(L_TIMES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
              }
                }
          else if (cour == '&' || cour == 0x2227)
		{
		  (void)new T_lexem(L_AND,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == ',')
		{
		  (void)new T_lexem(L_COMMA,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '~')
		{
		  (void)new T_lexem(L_CONVERSE,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
	  else if (cour == '%')
		{
		  (void)new T_lexem(L_LAMBDA,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
          else if (cour == '!' || cour == 0x2200)
		{
		  (void)new T_lexem(L_FORALL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
          else if (cour == '#' || cour == 0x2203)
		{
		  (void)new T_lexem(L_EXISTS,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
		}
          else if (cour == 0x00AC)
                {
                  (void)new T_lexem(L_L_NOT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x00D7)
                {
                  (void)new T_lexem(L_TIMES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x00F7)
                {
                  (void)new T_lexem(L_DIVIDES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x03BB)
                {
                  (void)new T_lexem(L_LAMBDA,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2025)
                {
                  (void)new T_lexem(L_INTERVAL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x207B)
          {
              cour = next_car() ;
              if (cour == 0xB9)
              {
                  (void)new T_lexem(L_CONVERSE,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
              }
              else
              {
                  char buf[3] ;
                  sprintf(buf, "$%c", cour) ;
                  parse_error(file_name_scp,
                              cur_line_si,
                              cur_column_si,
                              CAN_CONTINUE,
                              get_error_msg(E_TOKEN_PARSE_ERROR),
                              buf) ;
                  // On recule d'un cran pour retourner juste apres le "-"
                  prev_car() ;
              }
          }
          else if (cour == 0x2115)
          {
              cour = next_car() ;
              if (cour == 0x2081)
              {
                  (void)new T_lexem(L_NATURAL1,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
              }
              else
              {
                  prev_car() ;
                  (void)new T_lexem(L_NATURAL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
              }
          }
          else if (cour == 0x2119)
          {
              cour = next_car() ;
              if (cour == 0x2081)
              {
                  (void)new T_lexem(L_POW1,NULL,0,cur_line_si,cur_column_si-1,sym_file_name_scp) ;
              }
              else
              {
                  prev_car() ;
                  (void)new T_lexem(L_POW,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
              }
          }
          else if (cour == 0x2124)
                {
                  (void)new T_lexem(L_INTEGER,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2191)
                {
                  (void)new T_lexem(L_HEAD_RESTRICT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2192)
                {
                  (void)new T_lexem(L_TOTAL_FUNCTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2190)
                {
                (void)new T_lexem(L_RETURNS,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2193)
                {
                  (void)new T_lexem(L_TAIL_RESTRICT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2194)
                {
                  (void)new T_lexem(L_SET_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0xE100)
                {
                  if (get_allow_total_and_surjective_relation_operators() == TRUE) {
                     (void)new T_lexem(L_TOTAL_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                  }
                  else {
                      parse_error(file_name_scp,
                                  cur_line_si,
                                  cur_column_si,
                                  CAN_CONTINUE,
                                  get_error_msg(E_TOTAL_RELATIONS_NOT_ALLOWED)) ;
                      (void)new T_lexem(L_TOTAL_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                  }
                }
          else if (cour == 0xE101)
                {
                  if (get_allow_total_and_surjective_relation_operators() == TRUE) {
                     (void)new T_lexem(L_SURJ_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                  }
                  else {
                      parse_error(file_name_scp,
                                  cur_line_si,
                                  cur_column_si,
                                  CAN_CONTINUE,
                                  get_error_msg(E_TOTAL_RELATIONS_NOT_ALLOWED)) ;
                      (void)new T_lexem(L_SURJ_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                  }
                }
          else if (cour == 0xE102)
                {
                 if (get_allow_total_and_surjective_relation_operators() == TRUE) {
                    (void)new T_lexem(L_TOTAL_SURJ_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                 }
                 else {
                     parse_error(file_name_scp,
                                 cur_line_si,
                                 cur_column_si,
                                 CAN_CONTINUE,
                                 get_error_msg(E_TOTAL_RELATIONS_NOT_ALLOWED)) ;
                     (void)new T_lexem(L_TOTAL_SURJ_RELATION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                 }
                }
          else if (cour == 0x21A0)
                {
                  (void)new T_lexem(L_TOTAL_SURJECTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21A3)
                {
                  (void)new T_lexem(L_TOTAL_INJECTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21A6)
                {
                  (void)new T_lexem(L_MAPLET,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21D2)
                {
                  (void)new T_lexem(L_IMPLIES,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21D4)
                {
                  (void)new T_lexem(L_EQUIVALENT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21F8)
                {
                  (void)new T_lexem(L_PARTIAL_FUNCTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21FD)
                {
                  (void)new T_lexem(L_TAIL_INSERT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x21FE)
                {
                  (void)new T_lexem(L_HEAD_INSERT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2205)
                {
                  (void)new T_lexem(L_EMPTY_SET,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2209)
                {
                  (void)new T_lexem(L_NOT_BELONGS,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x220F)
                {
                  (void)new T_lexem(L_PI,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2211)
                {
                  (void)new T_lexem(L_SIGMA,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2216)
                {
                  (void)new T_lexem(L_MINUS,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2225)
                {
                  (void)new T_lexem(L_PARALLEL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2228)
                {
                  (void)new T_lexem(L_L_OR,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2229)
                {
                  (void)new T_lexem(L_INTERSECT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x222A)
                {
                  (void)new T_lexem(L_UNION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2254)
                {
                  (void)new T_lexem(L_AFFECT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2260)
                {
                  (void)new T_lexem(L_DIFFERENT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2264)
                {
                  (void)new T_lexem(L_LESS_THAN_OR_EQUAL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2265)
                {
                  (void)new T_lexem(L_GREATER_THAN_OR_EQUAL,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2282)
                {
                  (void)new T_lexem(L_STRICT_INCLUDED,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2284)
                {
                  (void)new T_lexem(L_NOT_STRICT_INCLUDED,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2286)
                {
                  (void)new T_lexem(L_INCLUDED,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2288)
                {
                  (void)new T_lexem(L_NOT_INCLUDED,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2297)
                {
                  (void)new T_lexem(L_DIRECT_PRODUCT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x22C2)
                {
                  (void)new T_lexem(L_QINTER,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x22C3)
                {
                  (void)new T_lexem(L_QUNION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x22D6 || cour == 0xE103)
                {
                  (void)new T_lexem(L_LEFT_OVERLOAD,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2312)
                {
                  (void)new T_lexem(L_CONCAT,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x25C0 || cour == 0x2A64)
                {
                  (void)new T_lexem(L_ANTIRESTRICTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x25B6 || cour == 0x2A65)
                {
                  (void)new T_lexem(L_ANTICORESTRICTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x25B7)
                {
                  (void)new T_lexem(L_CORESTRICTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x25C1)
                {
                  (void)new T_lexem(L_RESTRICTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2900)
                {
                  (void)new T_lexem(L_PARTIAL_SURJECTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2914)
                {
                  (void)new T_lexem(L_PARTIAL_INJECTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (cour == 0x2916)
                {
                  (void)new T_lexem(L_BIJECTION,NULL,0,cur_line_si,cur_column_si,sym_file_name_scp) ;
                }
          else if (u_isIDStart(cour) == TRUE)
                {
                  // Debut de mot
                  analyse_word() ;
                }
          else if (is_a_digit(cour) == TRUE)
                {
                  // Debut de nombre
                  analyse_number() ;
                }
          else analyse_token(cour) ;
	}
}

void compute_lines(T_lexem *first_lexem)
{
  TRACE0(">> compute_lines") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = first_lexem ;
  T_lexem *prev_line = NULL ;
  T_lexem *start_of_line = cur_lexem ;
  T_lexem *next_line = NULL ;
  int cur_line ;

  while (cur_lexem != NULL)
	{
	  // Recherche de la ligne suivante
	  cur_line = cur_lexem->get_file_line() ;
	  next_line = cur_lexem->get_next_lexem() ;

	  while ( (next_line != NULL) && (next_line->get_file_line() == cur_line) )
		{
		  next_line = next_line->get_next_lexem() ;
		}

	  while (cur_lexem != next_line)
		{
		  cur_lexem->set_prev_line(prev_line) ;
		  cur_lexem->set_next_line(next_line) ;
		  cur_lexem->set_start_of_line(start_of_line) ;

		  cur_lexem = cur_lexem->get_next_lexem() ;
		}

	  prev_line = start_of_line ;
	  start_of_line = next_line ;
	}

  EXIT_TRACE ;
  TRACE0("<< compute_lines") ;
}

// Chargement du fichier
FILE *open_source_file(const char *file_name_acp,
								size_t &path_len,
								size_t &file_size)
{
  TRACE2("open_source_file(\"%s\"), file_fetch_mode = %d",
		 file_name_acp,
		 get_file_fetch_mode()) ;
  struct stat stats_ls ; // Infos sur le fichier a charger

  // Reset du path de recherche
  lex_set_path(NULL) ;

  if(get_file_fetch_mode() == FFMODE_USER &&
     file_fetcher_user)
  {
      char *component_file_name = file_fetcher_user->get_file_name(file_name_acp);
      if(component_file_name)
      {
          FILE *fd;
          fd = fopen(component_file_name, "r") ;
          if ( (fd != NULL) && (stat(component_file_name, &stats_ls) == 0) )
          {
              //	  path_len = clone(&file_name_scp, file_name) ;
              //	  TRACE2("on a cree %x:%s", file_name_scp, file_name_scp) ;
              path_len = strlen(file_name_acp) ;
              TRACE1("ici file_name %s", component_file_name) ;
              file_size = stats_ls.st_size ;

              delete [] component_file_name;
              return fd ;
          }
          delete [] component_file_name;
      }

      return 0;
  }

  // On caste en (char *) car on sait qu'on n'ecrira pas dans file_name
  // sans la reallouer au prealable
  char *file_name = (char *)file_name_acp ;
  size_t ref_len ;

  if (    (get_file_fetch_mode() != FFMODE_MS)
	   && (get_file_fetch_mode() != FFMODE_ORIGINAL_MS) )
	{
	  FILE *fd;
	  //  FILE *fd = fopen(file_name_acp, "r") ;
	  if (get_file_fetch_mode() != FFMODE_LIBRARY)
		{
		   fd = fopen(file_name_acp, "r") ;
		  // On commence par essayer dans le repertoire courant
		  TRACE0("essai dans le repertoire courant") ;
		  TRACE2("fd = %x, stat = %d", fd, stat(file_name ,&stats_ls)) ;
		  if ( (fd != NULL) && (stat(file_name, &stats_ls) == 0) )
			{
			  //	  path_len = clone(&file_name_scp, file_name) ;
			  //	  TRACE2("on a cree %x:%s", file_name_scp, file_name_scp) ;
			  path_len = strlen(file_name) ;
		  TRACE1("ici file_name %s", file_name) ;
			  file_size = stats_ls.st_size ;
			  return fd ;
			}
		}

	  if (get_file_fetch_mode() == FFMODE_LOCAL)
		{
		  TRACE0("echec") ;
		  return NULL ;
		}

	  // On essaie ensuite avec les paths donnes par -I
	  TRACE0("essai avec les paths donnes par -I") ;
	  T_path *cur_path = get_source_paths() ;
	  ref_len = strlen(file_name_acp) ;

	  while (cur_path != NULL)
		{
		  path_len = cur_path->get_path_len() + ref_len + 1 ; // 1: le '/'
		  file_name = new char[path_len + 1] ;
#ifdef STATS_ALLOC
		  add_alloc(file_name, path_len + 1, __FILE__, __LINE__) ;
#endif
		  sprintf(file_name, "%s/%s", cur_path->get_path_name(), file_name_acp) ;
		  fd = fopen(file_name, "r") ;
		  sprintf(file_name, "%s/%s", cur_path->get_path_name(), file_name_acp) ;

		  TRACE1("Lecture du fichier %s", file_name) ;

		  TRACE2("fd = %x, stat = %d", fd, stat(file_name ,&stats_ls)) ;

		  if ((fd != NULL) && (stat(file_name, &stats_ls) == 0) )
			{
			  lex_set_path(cur_path->get_path_name()) ;
			  file_size = stats_ls.st_size ;
			  return fd ;
			}

		  // Le path n'etait pas bon
		  //TRACE1("ici delete %x", file_name) ;
		  //delete [] file_name ; NON ! on s'en sert peut etre apres
		  cur_path = cur_path->get_next() ;
		}
	} // fin get_file_fetch_mode != FFMODE_MS

  // On essaie ensuite avec les paths donnes par le fichier .db. On
  // arrive ici en cas d'echec de toutes les autres recherches dans le
  // cas general, ou directement si on est en mode FFMODE_MS
  init_project_manager() ;

  // 11/02/99 nouveau code utilisant API project_manager
  // Nom sans suffixe
  char *aux_file_name ;
  clone(&aux_file_name, file_name_acp) ;
  char *p = strchr(aux_file_name, '.') ;

  if (p == NULL)
	{
	  // Format invalide
	  TRACE1("echec <%s> ne contient pas de point", aux_file_name) ;
	  return NULL ;
	}

  *p = '\0' ;

  T_symbol *full_path = NULL ;
  T_project_manager *pmana = get_project_manager() ;
  if (get_file_fetch_mode() == FFMODE_MS)
	{
	  // Recherche apres decoupage multi-composants
	  full_path = pmana->find_component_path(aux_file_name) ;
	}
  else if (get_file_fetch_mode() == FFMODE_ORIGINAL_MS)
	{
	  // Recherche vant decoupage multi-composants
	  full_path = pmana->find_original_component_path(aux_file_name) ;
	}
  TRACE2("ici full_path = %x <%s>",
		 full_path,
		 (full_path == NULL) ? "null" : full_path->get_value()) ;
  if (full_path != NULL)
	{
          path_len = strlen(file_name_acp) ;
	  // On recalcule la lgr car elle n'est pas forcement a jour
          ref_len = full_path->get_len() ;
	  // 1: le '/'
	  file_name = new char[path_len + ref_len + 2] ;

	  sprintf(file_name,
			  "%s/%s",
		  full_path->get_value(),
			  file_name_acp) ;

	  FILE *fd = fopen(file_name, "r") ;
	  TRACE2("Lecture du fichier %x <%s>", file_name, file_name) ;
	  TRACE2("fd = %x, stat = %d", fd, stat(file_name ,&stats_ls)) ;

	  if ((fd != NULL) && (stat(file_name, &stats_ls) == 0) )
		{
		  TRACE1("ok, path %s !", full_path->get_value()) ;
		  lex_set_path(full_path->get_value()) ;
		  file_size = stats_ls.st_size ;
		  return fd ;
		}
	}

  TRACE0("echec") ;
  return NULL ;
}

// Il faut transformer les ':' en '\:' dans les rec et struct
void fix_label_col(T_lexem *first_lexem)
{
  TRACE0(">> fix_label_col") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = first_lexem ;

  while (cur_lexem != NULL)
	{
	  //TRACE2("%x %s", cur_lexem, get_lex_type_ascii(cur_lexem->get_lex_type())) ;
	  T_lexem *next = cur_lexem->get_next_lexem() ;

	  if (    (cur_lexem->get_lex_type() == L_REC)
			  || (cur_lexem->get_lex_type() == L_STRUCT) )
		{

		  if ( (next != NULL) && (next->get_lex_type() == L_OPEN_PAREN) )
			{
			  // On est sur une expression de record valide, i.e. du type
			  // "rec(" ou "struct("
			  // On corrige les ':' au premier niveau jusqu'a la parenthese
			  // de fin
			  int paren_level = 1 ; // la parenthese ouvrante de debut
			  int bracket_level = 0 ;
			  int braces_level = 0 ;

			  T_lexem *cur_fix = next->get_next_lexem() ;

			  while ( (cur_fix != NULL) && (paren_level > 0) )
				{
				  switch(cur_fix->get_lex_type())
					{
					case L_OPEN_PAREN :
					  {
						paren_level ++ ;
						break ;
					  }
					case L_CLOSE_PAREN :
					  {
						paren_level -- ;
						break ;
					  }
					case L_OPEN_BRACKET :
					  {
						bracket_level ++ ;
						break ;
					  }
					case L_CLOSE_BRACKET :
					  {
						bracket_level -- ;
						break ;
					  }
					case L_OPEN_BRACES :
					  {
						braces_level ++ ;
						break ;
					  }
					case L_CLOSE_BRACES :
					  {
						braces_level -- ;
						break ;
					  }
					case L_BELONGS :
					  {
						// On ne corrige les ':' qu'au premier niveau,
						// i.e paren = 1, braces = bracket = 0
						if ( 	(paren_level == 1)
								&& (braces_level == 0)
								&& (bracket_level == 0) )
						  {
#ifdef DEBUG_REC_MODE
							printf("%s:%d:%d: -> %x:%s doit etre corrige\n",
								   cur_fix->get_file_name()->get_value(),
								   cur_fix->get_file_line(),
								   cur_fix->get_file_column(),
								   cur_fix,
								   cur_fix->get_lex_name()) ;
#endif
							cur_fix->set_lex_type(L_LABEL_COL) ;
						  }
#ifdef DEBUG_REC_MODE
						else
						  {
							printf("%s:%d:%d: -> %x:%s PAS corrige (%d {%d [%d\n",
								   cur_fix->get_file_name()->get_value(),
								   cur_fix->get_file_line(),
								   cur_fix->get_file_column(),
								   cur_fix,
								   cur_fix->get_lex_name(),
								   paren_level,
								   braces_level,
								   bracket_level) ;
						  }
#endif
						break ;
					  }
#ifdef FIX_MAPLETS
					case L_MAPLET :
					  {
						// Les ',' qui ont ete a tord remplaces par des '|->'
						// dans les records doivent a nouveau etre des ',' sinon
						// la decompilation (et donc l'utilisation dans l'Atelier B)
						// se passera mal
						//
						// On ne corrige les ':' qu'au premier niveau,
						// i.e paren = 1, braces = bracket = 0
						if ( 	(paren_level == 1)
								&& (braces_level == 0)
								&& (bracket_level == 0) )
						  {
#ifdef DEBUG_REC_MODE
							printf("%s:%d:%d: -> %x:%s doit etre corrige\n",
								   cur_fix->get_file_name()->get_value(),
								   cur_fix->get_file_line(),
								   cur_fix->get_file_column(),
								   cur_fix,
								   cur_fix->get_lex_name()) ;
#endif
							cur_fix->set_lex_type(L_COMMA) ;
						  }
#ifdef DEBUG_REC_MODE
						else
						  {
							printf("%s:%d:%d: -> %x:%s PAS corrige (%d {%d [%d\n",
								   cur_fix->get_file_name()->get_value(),
								   cur_fix->get_file_line(),
								   cur_fix->get_file_column(),
								   cur_fix,
								   cur_fix->get_lex_name(),
								   paren_level,
								   braces_level,
								   bracket_level) ;
						  }
#endif
						break ;
					  }
#endif // FIX_MAPLETS
					default :
					  {
						// Par defaut, on ne fait rien
					  }
					}
				  cur_fix = cur_fix->get_next_lexem() ;
				}
			}
		}

	  cur_lexem = next ;
	}

  EXIT_TRACE ;
  TRACE0("<< fix_label_col") ;
}

// Dans le mode d'analyse des dependances, on
// coupe dans le flux de lexemes tout ce qui n'est pas
// relatif aux dependances
void shrink_for_dep(T_lexem *first_lexem)
{
  TRACE2("DEBUT shrink_for_dep(%x:%s)",
		 first_lexem,
		 (first_lexem == NULL) ? "(null)" : first_lexem->get_lex_name()) ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = first_lexem ;
  T_lexem *prev_lexem = NULL ;

  int change = FALSE ;

  while (cur_lexem != NULL)
	{
	  if (    (is_a_clause(cur_lexem) == TRUE)
			  && (is_a_dep_clause(cur_lexem) == FALSE) )
		{
		  // Ce n'est pas une clause de dependance
		  // On la supprime, i.e. on coupe tout jusqu'a la
		  // prochaine clause
		  change = TRUE ;
		  TRACE1("%s est une clause a supprimer", cur_lexem->get_lex_name()) ;
		  T_lexem *lookup_next = cur_lexem->get_next_lexem() ;
		  T_lexem *prev = cur_lexem ;

		  while ( (lookup_next != NULL) && (is_a_clause(lookup_next) == FALSE) )
			{
			  prev = lookup_next ;
			  lookup_next = lookup_next->get_next_lexem() ;
			}

		  if (lookup_next == NULL)
			{
			  TRACE0("c'etait la derniere clause du fichier !") ;
			  TRACE8("dernier lexeme  a %s:%d:%d <%s>, raccroche a %s:%d:%d <%s>",
					 prev->get_file_name()->get_value(),
					 prev->get_file_line(),
					 prev->get_file_column(),
					 prev->get_lex_name(),
					 prev_lexem->get_file_name()->get_value(),
					 prev_lexem->get_file_line(),
					 prev_lexem->get_file_column(),
					 prev_lexem->get_lex_name()) ;
			  prev_lexem->set_next_lexem(prev) ;
			  lex_set_last_lexem (prev) ;
			}
		  else
			{
			  if (prev_lexem != NULL)
				{
				  TRACE5("on accroche apres %x %s:%d:%d <%s>",
						 prev_lexem,
						 prev_lexem->get_file_name()->get_value(),
						 prev_lexem->get_file_line(),
						 prev_lexem->get_file_column(),
						 prev_lexem->get_lex_name()) ;
				  prev_lexem->set_next_lexem(lookup_next) ;
				}
			  else
				{
				  TRACE0("c'etait le premier lexem") ;
				  lex_set_first_lexem(lookup_next) ;
				}
			}

		  // On se recale dans le flux
		  cur_lexem = prev ;

		}
#ifdef TRACE
	  if (cur_lexem != NULL)
		{
		  TRACE5("on garde %x %s:%d:%d <%s>",
				 cur_lexem,
				 cur_lexem->get_file_name()->get_value(),
				 cur_lexem->get_file_line(),
				 cur_lexem->get_file_column(),
				 cur_lexem->get_lex_name()) ;
		}
#endif

	  if (change == FALSE)
		{
		  prev_lexem = cur_lexem ;
		}
	  else
		{
		  change = FALSE ;
		}
	  if (cur_lexem != NULL)
		// peut valoir NULL dans les cas degrades, ex: SETS VALUES
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		}
	}

  EXIT_TRACE ;
  TRACE0(" FIN  shrink_for_dep()") ;
}

static int file_definitions_mode_si = FALSE ;

int get_file_definitions_mode(void)
{
  TRACE1("get_file_definitions_mode() -> %s",
		 (file_definitions_mode_si == TRUE) ? "TRUE" : "FALSE") ;
  return file_definitions_mode_si ;
}

void set_file_definitions_mode(int new_file_definitions_mode)
{
  file_definitions_mode_si = new_file_definitions_mode ;
}

/* Caractere suivant du fichier */
#ifdef FULL_TRACE
static UChar _next_car(const char *file, int line)
#else
  static UChar next_car(void)
#endif
{
#ifdef DEBUG_LEX
  TRACE2("debut de next_car : L%dC%d", cur_line_si, cur_column_si) ;
#endif
#ifdef CHECK_INDEX
  assert(index_si <= size_si) ;
#endif
  UChar res = load_sct_UChar[++index_si] ;

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

#ifdef FULL_TRACE
  TRACE6("next_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif

  return res ;
}

/* Caractere courant du fichier */
#ifdef FULL_TRACE
static UChar _cur_car(const char *file, int line)
#else
  static UChar cur_car(void)
#endif
{
  UChar res = load_sct_UChar[index_si] ;
#ifdef FULL_TRACE
  TRACE6("cur_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif
  return res ;
}

/* Retourner un cran en arriere */
#ifdef FULL_TRACE
static UChar _prev_car(const char *file, int line)
#else
  static UChar prev_car(void)
#endif
{
  UChar res = load_sct_UChar[--index_si] ;

  lifo_pos_sop->pop(cur_line_si,cur_column_si) ;

#ifdef FULL_TRACE
  TRACE6("prev_car (%s:%d) -> '%c' (%03d) (L%03d)(C%03d)",
		 file, line, res, res,cur_line_si,cur_column_si) ;
#endif
  return res ;
}

/* copie privee en majuscules */
EXTERN char *upper(const char *srs_acp)
{
  ASSERT(srs_acp != NULL) ;
  char *result_lcp = (char *)s_volatile_malloc(strlen(srs_acp) + 1) ;
  const char *p = srs_acp ;
  char *q = result_lcp ;

  while (*p != '\0')
	{
	  if ( (*p >= 'a') && (*p <= 'z'))
		{
		  *(q++) = *(p++) + DECAL ;
		}
	  else
		{
		  *(q++) = *(p++) ;
		}
	}

  *q = '\0' ;

  return result_lcp ;
}

/* copie privee en minuscules */
EXTERN char *lower(const char *srs_acp)
{
  char *result_lcp = (char *)s_volatile_malloc(strlen(srs_acp) + 1) ;
  const char *p = srs_acp ;
  char *q = result_lcp ;

  while (*p != '\0')
	{
	  if ( (*p >= 'A') && (*p <= 'Z'))
		{
		  *(q++) = *(p++) - DECAL ;
		}
	  else
		{
		  *(q++) = *(p++) ;
		}
	}

  *q = '\0' ;

#ifdef DEBUG_LEX
  TRACE2("lower(\"%s\")->\"%s\"", srs_acp, result_lcp) ;
#endif
  return result_lcp ;
}


#ifdef FULL_TRACE
int _clone_(const char *file,
			int line,
			char **dest,
			const char *src)
#else
  int clone(char **dest,
			const char *src)
#endif
{
  int len = strlen(src) ;

#ifdef FULL_TRACE
  TRACE4("clone(\"%s\") -> @%x appele depuis %s:%d", src, dest, file,line) ;
#endif

  *dest = (char *)s_volatile_malloc(len + 1) ;
  strcpy(*dest, src) ;

#ifdef DEBUG_LEX
  TRACE1("res = \"%s\"", *dest) ;
#endif

  return len ;
}

// Fonction qui saute les commentaires a partir de cur_lexem et
// renvoie le premier lexeme non commentaire
// Si cur_lexem == NULL, renvoie NULL
T_lexem *lex_skip_comments(T_lexem *cur_lexem)
{
#ifdef DEBUG_LEX
  //TRACE1("syntax_skip_comment(%x)", cur_lexem) ;
#endif

  if (cur_lexem == NULL)
	{
	  return NULL ;
	}

  for (;;)
	{
	  if ( 	(cur_lexem != NULL)
			&& (	 (cur_lexem->get_lex_type() == L_COMMENT)
					 || (cur_lexem->get_lex_type() == L_KEPT_COMMENT) 
					 || (cur_lexem->get_lex_type() == L_CPP_COMMENT)
					 || (cur_lexem->get_lex_type() == L_CPP_KEPT_COMMENT)
					 ) )
		{
		  // On saute le commentaire
		  cur_lexem = cur_lexem->next_lexem ;
		}
	  else
		{
		  // Ok
		  return cur_lexem ;
		}

	}
}

// Fonction qui saute les commentaires EN ARRIERE a partir de cur_lexem et
// renvoie le premier lexeme non commentaire
// Si cur_lexem == NULL, renvoie NULL
T_lexem *lex_backwards_skip_comments(T_lexem *cur_lexem)
{
#ifdef DEBUG_LEX
  //TRACE1("lex_backwards_skip_comment(%x)", cur_lexem) ;
#endif

  if (cur_lexem == NULL)
	{
	  return NULL ;
	}

  for (;;)
	{
	  if ( 	(cur_lexem != NULL)
			&& (	 (cur_lexem->get_lex_type() == L_COMMENT)
					 || (cur_lexem->get_lex_type() == L_KEPT_COMMENT) 
					 || (cur_lexem->get_lex_type() == L_CPP_COMMENT) 
					 || (cur_lexem->get_lex_type() == L_CPP_KEPT_COMMENT)
					 ) )
		{
		  // On saute le commentaire
		  cur_lexem = cur_lexem->prev_lexem ;
		}
	  else
		{
		  // Ok
		  return cur_lexem ;
		}

	}
}

// Liberation memoire
// Decharge le fichier de la memoire. Donne son checksum
void lex_unload(void)
{
  TRACE0("lex_unload") ;
}

// Memorisation du nom de fichier courant
static const char *remember_file_name_scp = NULL ;
void set_file_name(const char *new_file_name)
{
  TRACE1("set_file_name(\"%s\")", new_file_name) ;

  remember_file_name_scp = new_file_name ;
}
const char *get_file_name(void)
{
  TRACE1("get_file_name()->\"%s\"", remember_file_name_scp) ;
  return remember_file_name_scp ;
}

// Gestion du premier lexeme de code du fichier
static T_lexem *first_compo_code_lexem_sop = NULL ;
void set_first_compo_code_lexem(T_lexem *new_first_compo_code_lexem)
{
  //  TRACE1("set_first_compo_code_lexem(%x)", new_first_compo_code_lexem) ;
  if (first_compo_code_lexem_sop == NULL)
	{
	  //	  TRACE0("ok on positionne") ;
	  first_compo_code_lexem_sop = new_first_compo_code_lexem ;
	}
#ifdef FULL_TRACE
  else
	{
	  TRACE1("pas de modif car vaut deja -> %x", first_compo_code_lexem_sop) ;
	}
#endif // TRACE
}
void reset_first_compo_code_lexem(void)
{
  TRACE0("reset_first_compo_code_lexem") ;
  first_compo_code_lexem_sop = NULL ;
}
T_lexem *get_first_compo_code_lexem(void)
{
  TRACE1("get_first_compo_code_lexem() -> %x", first_compo_code_lexem_sop) ;
  return first_compo_code_lexem_sop ;
}


int get_first_component_started(void)
{
  /*TRACE1("get_first_component_started() -> %s",
	(first_component_started_sb == TRUE) ? "TRUE" : "FALSE") ;*/
  return (first_component_started_sb == TRUE) ? TRUE : FALSE ;
}

void set_first_component_started(int new_first_component_started)
{
  TRACE2("set_first_component_started(%s) (was %s)",
		 (new_first_component_started == TRUE) ? "TRUE" : "FALSE",
		 (first_component_started_sb == TRUE) ? "TRUE" : "FALSE") ;
  first_component_started_sb = new_first_component_started ;
}

