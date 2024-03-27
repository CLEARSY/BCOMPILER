/******************************* CLEARSY **************************************
* Fichier : $Id: m_synsta.cpp,v 2.0 2000-10-19 08:28:01 cc Exp $
*
* (C) 2008 CLEARSY
*
* Description : analyseur syntaxique du fichier d'etat d'un composant
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
RCS_ID("$Id: m_synsta.cpp,v 1.6 2000-10-19 08:28:01 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_synsta.h"
#include "m_lexsta.h"

//nom du fichier d'état courant
static T_symbol *cur_state_file_name_sop = NULL ;

//
//	}{ fonctions auxiliaires
//
//reconnaissance d'une parenthèse ouvrante
//avance d'un lexeme
static void check_open_braces(void)
{
  TRACE0(">> check_open_braces") ;

  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  if ( cur_lex_type == SLT_OPEN_BRACES )
	{
	  state_next_lexem() ;
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_OPEN_BRACES) ;
	  assert(FALSE) ;

	}

  TRACE0("<< check_open_braces") ;
}

//reconnaissance d'un résultat d'une action
//retourne ce résultat et avance d'un lexeme
static T_action_result check_result(void)
{
  TRACE0("<< check_result") ;

  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  if ( cur_lex_type == SLT_OK )
	{
	  state_next_lexem() ;
	  TRACE1(">> check_result(%d)",AS_OK) ;
	  return AS_OK ;
	}
  else if ( cur_lex_type == SLT_KO )
	{
	  state_next_lexem() ;
	  TRACE1(">> check_result(%d)",AS_KO) ;
	  return AS_KO ;
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_OK) ;
	  assert(FALSE) ;
	}
}



//reconnaissance d'un identificateur représentant le nom d'un composant
//avance d'un lexeme
static T_symbol *check_component_name(void)
{
  TRACE0(">> check_component_name") ;

  T_symbol *ident = NULL ;
  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  if ( cur_lex_type == SLT_IDENT )
	{
	  ident = get_state_lex_value() ;
	  state_next_lexem() ;
	  TRACE1("<< check_component_name(%s)",ident->get_value()) ;
	  return ident ;
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_IDENT) ;
	  assert(FALSE) ;

	}
}


//reconnaissance d'un checksum
//avance d'un lexeme
static T_symbol *check_checksum(void)
{
  TRACE0(">> check_checksum") ;

  T_symbol *cur_checksum = NULL ;
  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  if ( cur_lex_type == SLT_NUMBER )
	{
	  cur_checksum = get_state_lex_value() ;
	  state_next_lexem() ;
	  TRACE1("<< check_checksum(%s)",cur_checksum->get_value()) ;
	  return cur_checksum ;
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_NUMBER) ;
	  assert(FALSE) ;

	}
}


//reconnaissance d'un checksum de composant
//retourne ce checksum de composant
//avance d'un lexeme
static T_global_component_checksum *
check_global_component_checksum(T_item **ref_adr_first,
								T_item **ref_adr_last)
{
  TRACE0(">> check_global_component_checksum") ;

  T_global_component_checksum *result = NULL;
  T_state_lex_type cur_lex_type = get_state_lex_type() ;

  if ( cur_lex_type == SLT_IDENT )
	{
	  //analyse du checksum du composant
	  T_symbol *cur_component_name = check_component_name() ;
	  T_symbol *cur_checksum = check_checksum() ;

	  // recherche du composant dans le projet et les librairies.
	  T_component *cur_comp =
		m_get_component(cur_component_name->get_value()) ;
	  if( cur_comp == NULL )
		{
		  TRACE1("pas de composant de nom (%s) dans le projet",
				 cur_component_name->get_value()) ;
		  cur_comp = m_get_library(cur_component_name->get_value()) ;
		  if( cur_comp == NULL )
			{
			  TRACE1("pas de composant de nom (%s) en bibliotheque",
					 cur_component_name->get_value()) ;
			}
		}
	  if (cur_comp != NULL)
		{
		  result = new T_global_component_checksum(cur_comp,
												   cur_checksum,
												   ref_adr_first,
												   ref_adr_last,
												   NULL,
												   NULL,
												   NULL) ;

		}
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_IDENT) ;
	  assert(FALSE) ;
	}

  TRACE1("<< check_global_component_checksum(%p)", result) ;
  return result;
}



//reconnaissance d'ne signature
//retourne cette signature
//avance d'un lexeme
static T_signature *check_signature(void)
{
  TRACE0(">> check_signature") ;

  T_global_component_checksum *main =
	check_global_component_checksum(NULL, NULL);

  // Liste des checksums des composants dependants
  T_global_component_checksum *adr_first = NULL ;
  T_global_component_checksum *adr_last = NULL ;

  //analyse des checksums des composants dependants
  //pour chaque (nom_composant : checksum)
  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  int bad_checksum_found = FALSE;
  while ( cur_lex_type != SLT_CLOSE_BRACES )
	{
	  T_global_component_checksum *cur_component_checksum =
		check_global_component_checksum((T_item **) &adr_first,
										(T_item **) &adr_last);

	  if (cur_component_checksum == NULL)
		{
		  bad_checksum_found = TRUE;
		}

	  //récupération du nouveau type de lexeme
	  cur_lex_type= get_state_lex_type() ;
	}

  if (main != NULL && bad_checksum_found == FALSE)
	{
	  //creation d'une signature
	  TRACE3("creation d'une signature(%x,%x,%x)",
			 main,
			 adr_first,
			 adr_last) ;
	  T_signature *cur_sign = new T_signature(main, adr_first, adr_last) ;

	  return cur_sign ;
	}
  else
	{
	  TRACE0("signature invalide, pas de création") ;
	  return NULL;
	}

  TRACE0("<< check_signature") ;
}



//reconnaissance d'une parenthèse fermante
//avance d'un lexeme
static void check_close_braces(void)
{
  TRACE0(">> check_close_braces") ;

  T_state_lex_type cur_lex_type = get_state_lex_type() ;
  if ( cur_lex_type == SLT_CLOSE_BRACES )
	{
	  state_next_lexem() ;
	}
  else
	{
	  // On ne doit pas passer ici, mais on prevoit une sortie
	  // propre pour le debug
	  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
			 cur_state_file_name_sop->get_value()) ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
					 cur_state_file_name_sop->get_value(),
					 get_curline(),
					 cur_lex_type,
					 SLT_CLOSE_BRACES) ;
	  assert(FALSE) ;
	}

  TRACE0("<< check_close_braces") ;
}

//analyse syntaxique de la partie relative à une action
//dans le fichier d'etat
//retourne l'état d'une action stocké dans le fichier d'état
static T_action_state *action_syntax_analysis(void)
{
  TRACE0(">> action_syntax_analysis") ;

  T_signature *signature ;
  T_action_result result ;
  T_action_state *action_state = NULL ;

  state_next_lexem() ;
  check_open_braces() ;
  result = check_result() ;
  signature = check_signature() ;
  check_close_braces() ;

  if (signature != NULL)
	{
	  action_state = new T_action_state(signature,result) ;
	}

  TRACE1("<< action_syntax_analysis(%x)", action_state) ;
  return action_state ;
}


//
//	}{ fonctions externes
//

//analyse syntaxique du fichier d'etat
T_component_state *state_syntax_analysis(const char *state_file_name)
{
  TRACE1(">> state_syntax_analysis(%x)", state_file_name) ;

  cur_state_file_name_sop = lookup_symbol(state_file_name) ;
  TRACE1("nom du fichier d'état(%s)", cur_state_file_name_sop->get_value()) ;


  //chargement d'un fichier d'etat et positionnnement premier lexeme
  // Renvoie TRUE si ok, FALSE sinon
  TRACE0("chargement du fic d'état") ;
  int res = load_state_file(state_file_name) ;
  if ( res == TRUE )
	  {
		  TRACE0("chargement du fichier réussi ") ;
	  }
  else
	  {
		  TRACE0("ECHEC du chargement du fichier ") ;
	  }

  state_next_lexem() ;

  //construction de l'etat du composant
  T_component_state *comp_state =
	  new T_component_state(cur_state_file_name_sop) ;
  //type du lexeme courant
  T_state_lex_type cur_lex_type = SLT_OPEN_BRACES ;
  //analyse des lexemes par action

  while ( cur_lex_type != SLT_EOF )
	  {
		  cur_lex_type = get_state_lex_type() ;
		  TRACE1("type du lexeme : %d ", cur_lex_type) ;

		  switch(cur_lex_type)
			  {
			  case SLT_COMPONENTCHECK :
				  {
					  TRACE0("analyse syntaxique de componentcheck ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action componentcheck
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_COMPONENTCHECK) ;
					  break ;
				  }
			  case SLT_POGENERATE :
				  {
					  TRACE0("analyse syntaxique de pogenerate ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action pogenerate
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_POGENERATE) ;
					  break ;
				  }
			  case SLT_PROVE :
				  {
					  TRACE0("analyse syntaxique de prove ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action prove
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_PROVE) ;
					  break ;
				  }
			  case SLT_CTRANS :
				  {
					  TRACE0("analyse syntaxique de ctrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action ctrans
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_CTRANS) ;
					  break;
				  }
			  case SLT_CPPTRANS :
				  {
					  TRACE0("analyse syntaxique de cpptrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action cpptrans
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_CPPTRANS) ;
					  break ;
				  }
			  case SLT_ADATRANS :
				  {
					  TRACE0("analyse syntaxique de adatrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action adatrans
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_ADATRANS) ;
					  break ;
				  }
			  case SLT_PSCTRANSCVG :
				  {
					  TRACE0("analyse syntaxique de psctranscvg ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action psctranscvg
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_PSCTRANSCVG) ;
					  break ;
				  }
			  case SLT_PSCTRANSDVG :
				  {
					  TRACE0("analyse syntaxique de psctransdvg ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action psctransdvg
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_PSCTRANSDVG) ;
					  break ;
				  }
			  case SLT_NEWPSCTRANSCONV :
				  {
					  TRACE0("analyse syntaxique de newpsctransconv ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action newpsctransconv
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_NEWPSCTRANSCONV) ;
					  break ;
				  }
			  case SLT_NEWPSCTRANS :
				  {
					  TRACE0("analyse syntaxique de newpsctrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action newpsctrans
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_NEWPSCTRANS) ;
					  break ;
				  }
			  case SLT_LATEX :
				  {
					  TRACE0("analyse syntaxique de latex ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action latex
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_LATEX) ;
					  break ;
				  }
			  case SLT_INTERLEAF :
				  {
					  TRACE0("analyse syntaxique de interleaf ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action interleaf
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_INTERLEAF) ;
					  break ;
				  }
			  case SLT_HIATRANS :
				  {
					  TRACE0("analyse syntaxique de hiatrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action interleaf
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_HIATRANS) ;
					  break ;
				  }
			  case SLT_ALATRANS :
				  {
					  TRACE0("analyse syntaxique de alatrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action traducteur alstom A
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_ALATRANS) ;
					  break ;
				  }
			  case SLT_ALBTRANS :
				  {
					  TRACE0("analyse syntaxique de albtrans ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action traducteur alstom B
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_ALBTRANS) ;
					  break ;
				  }
			  case SLT_BPP :
				  {
					  TRACE0("analyse syntaxique de bpp ") ;
					  //rangement dans T_component_state de l'etat
					  //de l'action interleaf
					  comp_state->add_action_state(action_syntax_analysis(),
												   STA_BPP) ;
					  break ;
				  }
			  default :
				  {
					  // On ne doit pas passer ici, mais on prevoit une sortie
					  // propre pour le debug
					  TRACE1("Erreur syntaxique dans le fichier d'etat %s",
							 cur_state_file_name_sop->get_value()) ;
					  toplevel_error(FATAL_ERROR,
									 get_error_msg(E_MS_SYNTAXE_STATE_FILE),
									 cur_state_file_name_sop->get_value(),
									 get_curline(),
									 cur_lex_type,
									 SLT_PROJECTCHECK) ;
					  assert(FALSE) ;
				  }
			  }
		  cur_lex_type = get_state_lex_type() ;
		  TRACE1("type du lexeme : %d ", cur_lex_type) ;
	  }

  return comp_state ;

  TRACE1("<< state_syntax_analysis(%x)", state_file_name) ;

}

#endif /* ACTION_NG */
