/******************************* CLEARSY **************************************
* Fichier : $Id: m_lexsta.cpp,v 2.0 2000-10-19 08:28:00 cc Exp $
*
* (C) 2008 CLEARSY
*
* Description : analyseur lexical du fichier d'etat d'un composant
*
* Compilation :	-DDEBUG_LEXSTA pour afficher des traces
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
#ifdef  ACTION_NG
#include "c_port.h"
RCS_ID("$Id: m_lexsta.cpp,v 1.7 2000-10-19 08:28:00 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_lexsta.h"


//numero de ligne courante dans le fichier d'etat
static int curline_si = 1 ;

/* Tables des symboles des mots clefs du fichier d'etat,
   associés à leur type */
static T_symbol_lex_type tab_symbols[] =
{
	/* 00 */
	{NULL, SLT_COMPONENTCHECK },
	/* 01 */
	{NULL, SLT_POGENERATE },
	/* 02 */
	{NULL, SLT_PROVE },
	/* 03 */
	{NULL, SLT_CTRANS },
	/* 04 */
	{NULL, SLT_CPPTRANS },
	/* 05 */
	{NULL, SLT_ADATRANS },
	/* 06 */
	{NULL, SLT_PSCTRANSCVG },
	/* 07 */
	{NULL, SLT_PSCTRANSDVG },
	/* 08 */
	{NULL, SLT_NEWPSCTRANSCONV },
	/* 09 */
	{NULL, SLT_NEWPSCTRANS },
	/* 10 */
	{NULL, SLT_LATEX },
	/* 11 */
	{NULL, SLT_INTERLEAF },
	/* 12 */
	{NULL, SLT_OK },
	/* 13 */
	{NULL, SLT_KO },
	/* 14 */
	{NULL, SLT_HIATRANS},
	/* 15 */
	{NULL, SLT_ALATRANS},
	/* 16 */
	{NULL, SLT_ALBTRANS},
	/* 17 */
	{NULL, SLT_BPP}
} ;
// numéro du caratère courant dans le tableau tab
static int car_offset_si = 0 ;

//tableau de stockage du fichier d'état
static char *load_file_scp = NULL ;

//type du lexeme courant
static T_state_lex_type cur_lex_type_so ;

//valeur du lexeme courant
static char *cur_lex_value_scp = NULL ;

//nom du fichier d'état courant
static T_symbol *cur_state_file_name_sop = NULL ;


//
//	}{ fonctions auxiliaires
//

// rend TRUE si le caractère est une lettre, FALSE sinon
static int is_a_letter(int carac)
{
	//  TRACE1("<< is_a_letter(%c)", carac) ;
  return (	( (carac >= 'a') && (carac <= 'z') )
			|| ( (carac >= 'A') && (carac <= 'Z') ) ) ;
}

// rend TRUE si le caractère est un espace, FALSE sinon
static int is_space(int carac)
{
  //si c'est un retour chariot on incrémente le compteur de lignes
  if ( carac == '\n' )
	  {
		  TRACE1("cur_line : %d", curline_si) ;
		  curline_si++ ;
	  }
  return (    (carac == ' ')
			  || (carac == '\t')
			  || (carac == '\n')
			  || (carac == '\r') // Pour lire les fichiers Windows 95
			  || (carac == 12)) ; // ^L
}

// rend TRUE si le caractère est un chiffre, FALSE sinon
static int is_a_digit(int carac)
{
	//  TRACE1("<< is_a_digit(%c)", carac) ;
  return ( ( (carac >= '0') && (carac <= '9') ) ||
		   ( (carac >= 'a') && (carac <= 'f') ) ) ;
}

// rend TRUE si le caractère est une lettre ou "_", FALSE sinon
static int is_an_extended_letter(int carac)
{
	// TRACE1("<< is_an_extended_letter(%c)", carac) ;
  return (	( (carac >= 'a') && (carac <= 'z') )
			|| ( (carac >= 'A') && (carac <= 'Z') )
			|| ( (carac >= '0') && (carac <= '9') )
			|| (carac == '_')
		    ) ;
}

// Analyse d'un mot
// On est place sur le premier caractere
// On rend la main sur le dernier caractere
static int analyse_state_word(void)
{
  TRACE0("<< analyse_state_word") ;
  TRACE1("car_offset_si : %d",car_offset_si);

  int offset_val = 0 ; //indice du tableau cur_lex_value_sc
  char cur_car = load_file_scp[car_offset_si] ;
  car_offset_si++ ;

  while ( is_an_extended_letter(cur_car) == TRUE )
	{
	  cur_lex_value_scp[offset_val++] = cur_car ;
	  cur_car = load_file_scp[car_offset_si++] ;
	}

  // Fin d'identificateur atteinte !
  cur_lex_value_scp[offset_val] = '\0' ;

  TRACE2(">> analyse_state_word (%d,%s)",offset_val,cur_lex_value_scp) ;

  return offset_val ;

}

// Analyse d'un nombre
// On est place sur le premier caractere
// On rend la main sur le caractere suivant
static void analyse_number(void)
{
  TRACE0("<< analyse_number") ;

  car_offset_si++ ;
  char cur_car = load_file_scp[car_offset_si] ; //caractere courant
  int offset_val = 0 ; //indice du tableau cur_lex_value_scp

  car_offset_si++;
  while (is_a_digit(cur_car) == TRUE)
	{
	  cur_lex_value_scp[offset_val++] = cur_car ;
	  cur_car = load_file_scp[car_offset_si++] ;
	}

  // Fin de nombre atteinte !
  //ajout de \0
  cur_lex_value_scp[offset_val] = '\0' ;
  cur_lex_type_so = SLT_NUMBER ;
  TRACE0(">> analyse_number") ;
}

static void analyse_keyword(void)
{
  TRACE0("<< analyse_keyword") ;

  int nb_letters = analyse_state_word() ;

  T_symbol *symb = lookup_symbol(cur_lex_value_scp,nb_letters) ;

  //comparaison avec les mots clefs

  //nombre d'élément dans le tableau des symboles
  int nb_symbols = sizeof(tab_symbols) / sizeof(T_symbol_lex_type) ;

  //indice de parcours du tableau des symboles
  int cur_ind = 0 ;
  //booleen indiquant si on a trouve le meme symbol
  int trouve_symbol = FALSE ;

  while ( cur_ind < nb_symbols && trouve_symbol == FALSE )
	  {
		  if ( symb == (tab_symbols[cur_ind]).lexem_symb )
			  {
				  cur_lex_type_so = (tab_symbols[cur_ind]).lexem_type ;
				  trouve_symbol = TRUE ; //on sort de la boucle
			  }
		  else
			  {
				  cur_ind++ ;
			  }
	  }

  if ( trouve_symbol == FALSE )
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur lexicale dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_LEXICAL_STATE_FILE),
					 cur_lex_value_scp,
					 cur_state_file_name_sop->get_value()) ;
	  assert(FALSE) ;
	}

  TRACE0(">> analyse_keyword") ;
}



//
//	}{ fonctions externes
//

//recuperatin de la ligne courante
int get_curline(void)
{
	TRACE1(">> get_curline(%d)",curline_si) ;
	return curline_si ;
}

//compteur dans la table des symbols
static int tab_symbols_cpt_si = 0;

//initialisation d'un symbole du fichier d'état
static void init_state_one_symbol(char *name)
{
	  (tab_symbols[tab_symbols_cpt_si]).lexem_symb = lookup_symbol(name) ;
	  tab_symbols_cpt_si ++ ;
}

//initialisation des symboles du fichier d'état
void init_state_symbols(void)
{
  TRACE0(">> init_state_symbols") ;

  //on remplit la table des symboles tab_symbols
  init_state_one_symbol("_ComponentCheck") ;
  init_state_one_symbol("_POGenerate") ;
  init_state_one_symbol("_Prove") ;
  init_state_one_symbol("_CTrans") ;
  init_state_one_symbol("_CPPTrans") ;
  init_state_one_symbol("_ADATrans") ;
  init_state_one_symbol("_PscTransCvg") ;
  init_state_one_symbol("_PscTransDvg") ;
  init_state_one_symbol("_NewPscTransConv") ;
  init_state_one_symbol("_NewPscTrans") ;
  init_state_one_symbol("_Latex") ;
  init_state_one_symbol("_Interleaf") ;
  init_state_one_symbol("_ok") ;
  init_state_one_symbol("_ko") ;
  init_state_one_symbol("_HIATrans") ;
  init_state_one_symbol("_ALATrans") ;
  init_state_one_symbol("_ALBTrans") ;
  init_state_one_symbol("_bpp") ;

  TRACE0("<< init_state_symbols") ;
}

//chargement du fichier d'etat et positionnnement premier lexeme
int load_state_file(const char *state_file_name)
{
  TRACE1(">> load_state_file(%x)", state_file_name) ;

  struct stat stats_ls ; // Infos sur le fichier a charger
  size_t file_size ; //taille du fichier en octets

  int res ; // Resultat de la fonction

  cur_state_file_name_sop = lookup_symbol(state_file_name) ;
  TRACE1("nom du fichier d'état(%s)", cur_state_file_name_sop->get_value()) ;

  //ouvre le fichier d'état
  FILE *fd = fopen(state_file_name, "rb") ;

  //recuperation de la taille du fichier en octets
  if ( (fd != NULL) && (stat(state_file_name, &stats_ls) == 0) )
	{
	  TRACE0("fichier d'etat ouvert") ;
	  int cur_car ; //caractère courant
	  file_size = stats_ls.st_size ;
	  car_offset_si = 0 ;

	  //initialise la taille du tableau stockant la valeur d'un lexeme
	  //à la taille du fichier +1
	  cur_lex_value_scp = new char[file_size + 1] ;

	  //charge le fichier d'etat dans un tableau de la taille du fichier +1
	  load_file_scp = new char[file_size + 1] ;
	  cur_car = getc(fd) ;
	  while ( cur_car != EOF )
		{
		  load_file_scp[car_offset_si++] = cur_car ;
		  cur_car = getc(fd) ;
		}
	  load_file_scp[car_offset_si] = '\0' ;

#ifdef DEBUG_LEXSTA
	  printf("fichier charge : \n %s \n", load_file_scp) ;
#endif /* DEBUG_LEXSTA */

	  //positionne sur le 1er lexeme
	  car_offset_si = 0 ;
	  res = TRUE ;
	}
  else
	{
	  res = FALSE ;
	}

  fclose(fd) ;
  TRACE1("<< load_state_file(%x)", state_file_name) ;

  return res ;
}

//passage au lexeme suivant : met a jour le type et la valeur du lexeme
void state_next_lexem(void)
{
    TRACE0(">> state_next_lexem") ;

	char cur_car = load_file_scp[car_offset_si] ;
	int nb_letters ;

	// les espaces sont sautés
	while ( is_space(cur_car) )
	  {
		car_offset_si++ ;
		cur_car = load_file_scp[car_offset_si] ;
	  }

	if ( cur_car == '{')
	  {
		cur_lex_type_so = SLT_OPEN_BRACES ;
		car_offset_si++ ;
	  }
	else if ( cur_car == '}' )
	  {
		cur_lex_type_so = SLT_CLOSE_BRACES ;
		car_offset_si++ ;
	  }
	else if ( cur_car == '@' )
	  {
		//debut de nombre
		analyse_number() ;
	  }
	else if ( cur_car == '_' )
	  {
		//debut de mot clef
		analyse_keyword() ;
	  }
	else if ( is_a_letter(cur_car) )
	  {
		cur_lex_type_so =  SLT_IDENT ;
		//debut d'identificateur
	    nb_letters = analyse_state_word() ;
	  }
	else if ( cur_car == '\0' )
	  {
		cur_lex_type_so = SLT_EOF ;
		TRACE0("fin du fichier") ;
	  }
	else
	  {
		// On ne doit pas passer ici, mais on prevoit une sortie
		// propre pour le debug
		TRACE1("Erreur lexicale dans le fichier d'état(%s)",
			   cur_state_file_name_sop->get_value()) ;
		toplevel_error(FATAL_ERROR,
					   get_error_msg(E_MS_LEXICAL_STATE_FILE),
					   cur_lex_value_scp,
					   cur_state_file_name_sop->get_value()) ;
		assert(FALSE) ;
	  }

	TRACE0("<< state_next_lexem") ;
}


//recuperation du type du lexeme courant
T_state_lex_type get_state_lex_type(void)
{
  TRACE1(">> get_state_lex_type(%d)",cur_lex_type_so);
  return cur_lex_type_so ;
}

//recuperation de la valeur du lexeme courant
//s'il s'agit d'un nombre (checksum) ou d'un identificateur
T_symbol *get_state_lex_value(void)
{
    TRACE1(">> get_state_lex_value(%s)",cur_lex_value_scp) ;

	if ( cur_lex_type_so == SLT_NUMBER || cur_lex_type_so == SLT_IDENT )
	  {
		return lookup_symbol(cur_lex_value_scp) ;
	  }
	else
	  {
		// On ne doit pas passer ici, mais on prevoit une sortie
		// propre pour le debug
		TRACE1("Pas de valeur associée au lexeme de type %d",
			   cur_lex_type_so) ;
		toplevel_error(FATAL_ERROR,
					   get_error_msg(E_MS_STATE_FILE_LEXEM_WITH_NO_VALUE),
					   cur_lex_type_so,
					   cur_state_file_name_sop->get_value()) ;
		assert(FALSE) ;
	  }
}


#endif /* ACTION_NG */
