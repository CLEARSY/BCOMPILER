/******************************* CLEARSY **************************************
* Fichier : $Id: s_defini.cpp,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Compilateur B
*					Definitions
*
* Compilation : 	-DDEBUG_DEFINITIONS pour tracer les traitements
*					-DAUTO_PAREN_DEF pour le parenthesage automatique des def
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
RCS_ID("$Id: s_defini.cpp,v 1.53 2007-07-27 15:19:46 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <limits.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"

extern void *lex_push_context(void);
extern void lex_unlink_current_context(void);
extern void lex_pop_context(void *ctx);

#ifdef DUMP_DEFINITIONS
void dump_DEFINITIONS(T_definition *first, T_definition *last)
{
  T_definition *cur_def = first ;

  TRACE4("--> dump_DEFINITIONS first %x:%s last %x:%s",
		 first,
		 (first == NULL)
		 ? "null" : first->get_name()->get_value(),
		 last,
		 (last == NULL)
		 ? "null" : last->get_name()->get_value()) ;

  ENTER_TRACE ;
  while (cur_def != NULL)
	{
	  TRACE4("%x <-- %x:%s --> %x",
			 cur_def->get_prev(),
			 cur_def,
			 cur_def->get_name()->get_value(),
			 cur_def->get_next()) ;

	  cur_def = (T_definition *)cur_def->get_next() ;
	}

  EXIT_TRACE ;
  TRACE4("<-- dump_DEFINITIONS first %x:%s last %x:%s",
		 first,
		 (first == NULL)
		 ? "null" : first->get_name()->get_value(),
		 last,
		 (last == NULL)
		 ? "null" : last->get_name()->get_value()) ;
}
#endif // DUMP_DEFINITIONS

// fonction interne utilis�e par T_definition::syntax_analysis
// PRE: first_lexem = "=="
// POST:
//        si definition vide alors return = "=="
//        si definition non vide alors return = "dernier lexem de
//                                               la definition"
//        en cas d'erreur un message est g�n�r�

T_lexem *T_definition::find_end_of_def(T_lexem *first_lexem)
{
  ASSERT(first_lexem != NULL) ;
  ASSERT(first_lexem->get_lex_type() == L_REWRITES) ;

  T_lexem *cur_lexem = first_lexem->get_next_lexem() ;
  T_lexem *prev_cur_lexem = first_lexem ;

  // on parcours les lexem jusqu'a avoir depass� la fin de la
  // definition.
  // La fin a �t� d�pass�e si:
  //    - on est a la fin du flot de lexem
  //    - on est sur un nom de clause
  //    - on est sur un '=='

  while (
		 (cur_lexem != NULL)
		 &&
		 (is_a_clause(cur_lexem) == FALSE)
		 &&
		 (cur_lexem->get_lex_type() != L_REWRITES)
		 &&
		 ((prev_cur_lexem->get_lex_type() != L_SCOL) ||
		  (cur_lexem->get_lex_type() != L_STRING))
		 &&
		 (cur_lexem->get_lex_type() != L_LIB_FILE_NAME)
		 )
	{
	  TRACE2("valeur du lexem: %s est une clause: %d", cur_lexem->get_lex_ascii(), is_a_clause(cur_lexem));
	  prev_cur_lexem = cur_lexem ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}

  // On a donc d�pass�e la fin de la d�finition

  // premier cas on a ateint la fin du flot de lexem

  if (cur_lexem == NULL)
	{
	  if (prev_cur_lexem->get_lex_type() == L_END)
		{
		  // on a attend la fin du composant courant
		  return prev_cur_lexem->get_prev_lexem() ;
		}
	  else
		{
		  // on est sur un cas d'erreur
		  // le composant courant n'est pas termin�
		  syntax_error(first_lexem,
					   CAN_CONTINUE,
					   get_error_msg(E_EOF_FOUND_WHILE_PARSING_DEFINITION),
					   name->get_value()) ;
		  syntax_error(this->get_ref_lexem(),
					   MULTI_LINE,
					   get_error_msg(E_LOCATION_OF_ITEM_DEF),
					   name->get_value()) ;
		  stop() ;
		  return NULL ;
		}
	}


  if (is_a_clause(cur_lexem) == TRUE)
	{
	  if (is_a_begin_component_clause(cur_lexem->get_lex_type()) == TRUE)
		{
		  // on est sur un nouveau composant dans le cas de fichier multi
		  // composant
		  // dans ce cas le lexem precedent doit etre END
		  if (prev_cur_lexem->get_lex_type() == L_END)
			{
			  return prev_cur_lexem->get_prev_lexem() ;
			}
		  else
			{
			  // c'est un cas d'erreur le composant n'est pas termin� par
			  // un END
			  syntax_error(first_lexem,
						   CAN_CONTINUE,
						   get_error_msg(E_EOF_FOUND_WHILE_PARSING_DEFINITION),
						   name->get_value()) ;
			  syntax_error(this->get_ref_lexem(),
						   MULTI_LINE,
						   get_error_msg(E_LOCATION_OF_ITEM_DEF),
						   name->get_value()) ;
			  stop() ;
			  return NULL ;
			}
		}
	  else
		{
		  // on est sur un nom de clause donc prev_cur_lexem est le
		  // dernier lexem de la definition
		  return prev_cur_lexem ;
		}
	}

  if (
	  (cur_lexem->get_lex_type() == L_REWRITES)
	  ||
	  (cur_lexem->get_lex_type() == L_STRING)
	  ||
	  (cur_lexem->get_lex_type() == L_LIB_FILE_NAME)
	  )
	{
	  // on est pass� sur la definition suivante on remonte en arri�re
	  // jusqu'a trouver:
	  //     - un point virgule s'il n'y a pas d'erreur
	  //     - le lexem de depart s'il y a une erreur

	  T_lexem *cur_lexem_back = cur_lexem->get_prev_lexem() ;
	  while (
			 (cur_lexem_back->get_lex_type() != L_SCOL)
			 &&
			 (cur_lexem_back != first_lexem)
			 )
		{
		  cur_lexem_back = cur_lexem_back->get_prev_lexem() ;
		}

	  if(cur_lexem_back == first_lexem)
		{
		  syntax_error(cur_lexem,
					   FATAL_ERROR,
					   get_catalog(C_INV_TOKEN_IN_DEF),
					   cur_lexem->get_lex_ascii(),
					   name->get_value()) ;
		  return NULL ;
		}
	  else
		{
		  return cur_lexem_back->get_prev_lexem() ;
		}
	}

  // pour un compilation sans warning
  return NULL ;

}

//
//	}{	Constructeur de la classe T_definition
//
void T_definition::syntax_analysis(T_lexem **adr_ref_lexem,
											T_lexem *last_code_lexem)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("T_definition::syntax_analysis(%x --> %x)",
		 adr_ref_lexem, *adr_ref_lexem) ;
  ENTER_TRACE ;
#endif

  // On parse le nom de la definition
  T_lexem *cur_lexem = *adr_ref_lexem ;

  // Le nom de la d�finition doit etre un L_IDENT sans $0 !!
  // Remarque : il est n�cessaire de le tester au niveau lexical
  // car les d�finitions ne sont trait�es qu'� ce niveau
  if (cur_lexem->get_lex_type() != L_IDENT)
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_DEFINITION_NAME)) ;
	}

  if ( strstr(cur_lexem->get_value(), "$0") != NULL )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_DEFINITION_NAME) ) ;
	}

  name = lookup_symbol(cur_lexem->get_value(), cur_lexem->get_value_len()) ;
  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  // La macro a-t-elle des parametres ???
  first_param = last_param = NULL ;
  nb_params = 0 ;
  if (cur_lexem->get_lex_type() == L_OPEN_PAREN)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE0("la macro a des parametres") ;
#endif

	  cur_lexem = cur_lexem->get_next_lexem() ;

	  int encore = TRUE ;

	  while (encore == TRUE)
		{
		  syntax_check_eof(cur_lexem) ;

		  if (cur_lexem->get_lex_type() != L_IDENT)
			{
			  syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_DEF_ARG)) ;
			}

		  if ( strstr(cur_lexem->get_value(), "$0") != NULL )
			{
			  syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_DEF_ARG)) ;
			}

		  // On verifie que le nom est unique
		  T_ident *same_name = find_param(cur_lexem->get_value(),
										  cur_lexem->get_value_len()) ;

		  if (same_name != NULL)
			{
			  // Redefinition d'un param formel
			  syntax_error(cur_lexem,
						   CAN_CONTINUE,
						   get_error_msg(E_NAME_CLASH_IN_DEF_FPARAMS),
						   name->get_value(),
						   cur_lexem->get_value()) ;

			  already_location(same_name->get_ref_lexem()) ;
			}

		  (void)new T_ident(cur_lexem->get_value(),
							ITYPE_DEFINITION_PARAM,
							(T_item **)&first_param,
							(T_item **)&last_param,
							this,
							get_betree(),
							cur_lexem) ;

		  nb_params ++ ;
		  cur_lexem = cur_lexem->get_next_lexem() ;

		  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
			{
			  // Un argument de plus !
			  cur_lexem = cur_lexem->get_next_lexem() ;
			}
		  else
			{
			  encore = FALSE ;
			}
		}

	  // Parse ')'
	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_CLOSE_PAREN) )
		{
		  syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_CLOSE_PAREN)) ;
		}

	  cur_lexem = cur_lexem->get_next_lexem() ;
	}

  // Parse '=='
  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_REWRITES) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_REWRITES)) ;
	}


  last_rewrite_rule = find_end_of_def(cur_lexem) ;

  // si une erreur est detect�e lors de la recherche du dernier lexem
  // l'execution est arret�e. Aussi est on assur� ici d'avoir
  // last_rewrite non nul
  ASSERT(last_rewrite_rule != NULL);

  if (last_rewrite_rule == cur_lexem)
	{
	  // cas d'une definition vide
	  first_rewrite_rule = NULL ;
	  last_rewrite_rule = NULL ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  first_rewrite_rule = cur_lexem->get_next_lexem() ;
	  first_rewrite_rule->set_prev_lexem(NULL) ;

	  cur_lexem = last_rewrite_rule->get_next_lexem() ;

	  last_rewrite_rule->set_next_lexem(NULL) ;
	}


#ifdef AUTO_PAREN_DEF
#ifdef DEBUG_DEFINITIONS
  TRACE0("... on parenthese la definition obtenue ...") ;
#endif
  // Il faut parentheser la definition obtenue si elle est non vide

  if(first_rewrite_rule != NULL)
	{
	  ASSERT(last_rewrite_rule != NULL) ;
	  // 1) ajout de '(' en debut de definition
	  T_lexem *open_paren = new T_lexem(L_OPEN_PAREN,
										NULL,
										0,
										first_rewrite_rule->get_file_line(),
										first_rewrite_rule->get_file_column(),
										first_rewrite_rule->get_file_name(),
										FALSE) ; // pas de chainage auto
	  open_paren->set_next_lexem(first_rewrite_rule) ;
	  open_paren->set_prev_lexem(NULL) ;
	  first_rewrite_rule->set_prev_lexem(open_paren) ;
	  first_rewrite_rule = open_paren ;

	  // 2) ajout de ')' en fin de definition
	  T_lexem *close_paren = new T_lexem(L_CLOSE_PAREN,
										 NULL,
										 0,
										 last_rewrite_rule->get_file_line(),
										 last_rewrite_rule->get_file_column(),
										 last_rewrite_rule->get_file_name(),
										 FALSE) ; // pas de chainage auto
	  close_paren->set_next_lexem(NULL) ;
	  close_paren->set_prev_lexem(last_rewrite_rule) ;
	  last_rewrite_rule->set_next_lexem(close_paren) ;
	  last_rewrite_rule = close_paren ;
	}
#endif // AUTO_PAREN_DEF

  *adr_ref_lexem = cur_lexem ;

}

//
//	}{	Fonction auxiliaire qui produit un message d'erreur si
//	une macro n'est pas utilisee avec assez d'arguments
inline static void macro_error(T_definition *cur_def)
{
  syntax_error(cur_def->get_ref_lexem(),
			   FATAL_ERROR,
			   get_error_msg(E_MACRO_DEFINITION),
			   cur_def->get_name()->get_value(),
			   cur_def->get_nb_params()) ;
}

//
//	}{	Recherche la definition correspondant a un identifiant
//		Renvoie TRUE si la substitution a eu lieu, FALSE sinon
static T_definition *find_definition(T_lexem *cur_lexem,
											  T_definition *first_definition)
{
#ifdef DEBUG_DEFINITIONS
  TRACE4("find_definition(%x:%s, %x:%s)",
		cur_lexem,
		cur_lexem->get_lex_ascii(),
		first_definition,
		(first_definition == NULL)
		? "" : first_definition->get_name()->get_value()) ;
#endif
  if (cur_lexem->get_lex_type() != L_IDENT)
	{
	  // cur_lexem n'est pas un ident : pas la peine de chercher
	  return NULL ;
	}

  T_definition *cur_def = first_definition ;

  const char *ident_name = cur_lexem->get_value() ;
  size_t ident_len = cur_lexem->get_value_len() ;

  for (;;)
	{
	  if (cur_def == NULL)
		{
		  return NULL ;
		}

	  if ( 	(cur_def->get_name()->get_len() == ident_len)
			&& (strcmp(cur_def->get_name()->get_value(), ident_name) == 0) )
		{
		  return cur_def ;
		}

	  cur_def = (T_definition *)cur_def->get_next() ;
	}

}

//
//	}{	Recherche la definition correspondant a un identifiant
//		Renvoie TRUE si la substitution a eu lieu, FALSE sinon
static T_definition *find_definition(T_lexem *cur_lexem,
											  T_list_link *first_def_link)
{
#ifdef DEBUG_DEFINITIONS
  TRACE4("find_definition(%x:%s, %x:%s)",
		cur_lexem,
		cur_lexem->get_lex_ascii(),
		first_def_link,
		(first_def_link == NULL)
		 ? ""
		 : ((T_definition *)(first_def_link->get_object()))->get_name()->get_value());
#endif

  if (cur_lexem->get_lex_type() != L_IDENT)
	{
	  // cur_lexem n'est pas un ident : pas la peine de chercher
	  return NULL ;
	}

  T_list_link *cur_link = first_def_link ;

  const char *ident_name = cur_lexem->get_value() ;
  size_t ident_len = cur_lexem->get_value_len() ;

  for (;;)
	{
	  if (cur_link == NULL)
		{
		  return NULL ;
		}

	  T_definition *cur_def = (T_definition *)cur_link->get_object() ;
	  if ( 	(cur_def->get_name()->get_len() == ident_len)
			&& (strcmp(cur_def->get_name()->get_value(), ident_name) == 0) )
		{
		  return cur_def ;
		}

	  cur_link = (T_list_link *)cur_link->get_next() ;
	}

}

#define TMP_SOURCE_DIR_BUFFER_SIZE 256
static char tmp_source_dir_buffer[TMP_SOURCE_DIR_BUFFER_SIZE];

// Retourne le chemin du r�pertoire contenant le source en cours de
// lecture (utilis� pour d�terminer le lieu d'inclusion d'un fichier
// de d�finitions entre guillemets).
static const char *
original_source_dir_path(const char*file_name)
{
  TRACE1("-> original_source_dir_path ('%s')", file_name) ;

  // Pointe sur le d�but du basename du source courant (c�d juste
  // derri�re le '/' ou en d�but de cha�ne).
  const char *base_start = NULL ;

  // Pointe juste apr�s la fin du basename du source courant, c�d sur
  // le '.' qui commence l'extension.
  const char *base_end = NULL ;

  // nom et longueur du composant en cours d'analyse (i.e. du basename)
  char *component_name = NULL ;
  size_t component_len = 0;

  // R�sultat de la fonction.
  const char *result = NULL ;

  base_start = bcomp_last_slash(const_cast<char*>(file_name));
  if(base_start == NULL)
	{
	  base_start = file_name ;
	}
  else
	{
	  // On avance juste apr�s le '/'
	  ++ base_start ;
	}

  base_end = strrchr(file_name, '.') ;
  if (base_end == NULL)
  {
      // Chemin sans point. On positionne sur la
      // fin du composant
      base_end = base_start + strlen(base_start);
  }

  component_len = base_end - base_start ;
  component_name= new char[component_len + 1] ;
  strncpy(component_name,
		  base_start,
		  component_len) ;
  component_name[component_len] = '\0';

  T_project_manager *project_manager = get_project_manager();
  result = 0;
  if(project_manager)
  {
      T_symbol *symb = project_manager
              ->find_original_component_path(component_name);
      if(symb)
      {
          result = symb->get_value();
      }
  }

  if(!result)
  {
      if(base_start != file_name)
      {
          strncpy(&tmp_source_dir_buffer[0], file_name, TMP_SOURCE_DIR_BUFFER_SIZE);
          if(base_start - file_name < TMP_SOURCE_DIR_BUFFER_SIZE)
          {
              tmp_source_dir_buffer[base_start - file_name] = '\0';
              result = &tmp_source_dir_buffer[0];
          }
          else
          {
              result = ".";
          }
      }
      else
      {
          result = ".";
      }
  }
  delete [] component_name;

  TRACE1("<- original_source_dir_path: '%s'", result) ;
  return result;
}

// Fonction qui recupere les DEFINITIONS d'un fichier de definitions
void include_file_definitions(const char *file_name,
                                       const char *converterName,
									   T_lexem **adr_cur_lexem,
									   int local_search_only,
									   T_file_definition **adr_first,
									   T_file_definition **adr_last)
{
  TRACE3("--> include_file_definitions(%x<%s>, %x)",
		 file_name, file_name, adr_cur_lexem) ;
  ENTER_TRACE ;

  // Sauvegarde du contexte
  void *context = lex_push_context() ;
  TRACE1("context = %x", context) ;

  // Avant tout on regarde file_name pour savoir si elle contient un
  // nom de fichier avec un path "en dur" i.e. avec des "..", des
  // "/" ou des "\". En fait on ne teste pas ".."  car on ne peut
  // rien en faire si on n'utilise pas conjointement "/"
  // Idem pour ~

  if (    (strchr(file_name, '/') != NULL)
	   || (strchr(file_name, '\\') != NULL) )
	{
	  syntax_error(*adr_cur_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_FILE_DEF_NAME_WITH_PATH)) ;
	  return ;
	}

  // Analyse lexicale ...
  // ... avant qu'elle commence, on memorise le contexte "last_lexem" et file_name
  T_lexem *save_last_lexem = lex_get_last_lexem() ;
  const char *save_file_name = get_file_name() ;
  T_file_fetch_mode save_fetch_mode = get_file_fetch_mode() ;
  char original_cwd[PATH_MAX];
  (void) getcwd(original_cwd, sizeof original_cwd);
  // ... et on y va
  TRACE0(".. analyse lexicale") ;
  ENTER_TRACE ;

  set_file_definitions_mode(TRUE) ;
  lex_set_push_next_lexem(TRUE) ;
  int save_first_component_started = get_first_component_started() ;
  set_first_component_started(FALSE) ;

  // Mise en place des options de recherche de fichier
  char *sys_file_name ; // nom du fichier sur le systeme

  if (local_search_only == TRUE)
	{
	  TRACE0("recherche en local") ;
      set_file_fetch_mode(FFMODE_LOCAL) ;
	  if(save_fetch_mode == FFMODE_DEFAULT ||
             save_fetch_mode == FFMODE_MS ||
             save_fetch_mode == FFMODE_ORIGINAL_MS ||
             save_fetch_mode == FFMODE_USER)
          {
              chdir(original_source_dir_path(save_file_name));
          }
	  clone(&sys_file_name, file_name) ;
	}
  else
	{
	  TRACE0("recherche library") ;
	  set_file_fetch_mode(FFMODE_LIBRARY) ;
	  clone(&sys_file_name, file_name) ;
	}


  T_file_definition *fdef = new T_file_definition(lookup_symbol(file_name),
												  NULL,
												  (T_item **)adr_first,
												  (T_item **)adr_last,
												  NULL,
												  NULL,
												  *adr_cur_lexem) ;

  TRACE1("fdef = %x", fdef) ;

  TRACE0(">> APPEL LEX_ANALYSIS") ;
  lex_analysis(NULL, sys_file_name, converterName, NULL, NULL, *adr_cur_lexem, NULL) ;
  TRACE0("<< APPEL LEX_ANALYSIS") ;
  fdef->set_checksum(lex_pop_MD5_key_stack()) ;
  TRACE1("checksum recupere : <%s>", fdef->get_checksum()->get_value()) ;

  fdef->set_path( (lex_get_path() == NULL) ?
				  (T_symbol *)NULL : lookup_symbol(lex_get_path())) ;

  // Restauration du contexte
  set_file_fetch_mode(save_fetch_mode) ;
  set_file_definitions_mode(FALSE) ;
  set_first_component_started(save_first_component_started) ;
  chdir(original_cwd);

  EXIT_TRACE ;

  // Il faut que le premier lexeme soit un L_IDENT
  pop_first_lex_stack() ;
  pop_last_lex_stack() ;
  T_lexem *first_lexem = lex_get_first_lexem() ;
  T_lexem *last_lexem = lex_get_last_lexem() ;

  TRACE4("on a recupere le flux de lexemes de %x:%s a %x:%s",
		 first_lexem,
		 (first_lexem == NULL) ? "NULL" : first_lexem->get_lex_ascii(),
		 last_lexem,
		 (last_lexem == NULL) ? "NULL" : last_lexem->get_lex_ascii()) ;

  // Le fichier doit commencer par DEFINITIONS
  first_lexem = lex_skip_comments(first_lexem) ;

  if ( (first_lexem == NULL) || (first_lexem->get_lex_type() != L_DEFINITIONS) )
	{
	  syntax_unexpected_error(first_lexem,
							  FATAL_ERROR,
							  get_lex_type_ascii(L_DEFINITIONS)) ;
	}


  if (first_lexem->get_next_lexem() == NULL)
	{
	  syntax_error(first_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_EOF_FOUND_WHILE_PARSING_DEF_FILE),
				   file_name) ;
	}

  first_lexem = first_lexem->get_next_lexem() ;

  // Il faut remplacer dans le flux global *adr_cur_lexem par
  // le nouveau flux

  T_lexem *prev = (*adr_cur_lexem)->get_real_prev_lexem() ;
  ASSERT(prev != NULL) ; // c'est au moins le "DEFINITIONS"

  T_lexem *next = (*adr_cur_lexem)->get_real_next_lexem() ;

  TRACE4("on colle apres %x:%s le debut %x:%s",
		 prev,
		 (prev == NULL) ? "NULL" : prev->get_lex_ascii(),
		 first_lexem,
		 (first_lexem == NULL) ? "NULL" : first_lexem->get_lex_ascii()) ;

  prev->set_next_lexem(first_lexem) ;
  first_lexem->set_prev_lexem(prev) ;
  TRACE4("on colle avant %x:%s la fin %x:%s",
		 next,
		 (next == NULL) ? "NULL" : next->get_lex_ascii(),
		 last_lexem,
		 (last_lexem == NULL) ? "NULL" : last_lexem->get_lex_ascii()) ;

  // dans la mesure ou le source est incomplet next est null dans ce
  // cas le nouveau flux de lexem n'est pas inserer mais uniquement
  // coll� en queue
  if (next != NULL)
	{
	  next->set_prev_lexem(last_lexem) ;
	}
  last_lexem->set_next_lexem(next) ;

  // On reference via original_lexem l'ancien flux i.e. le nom de
  // fichier Comme ca on ne perd pas sa trace (et du coup le
  // decompilateur peut decompiler)
  T_lexem *cur_lexem = first_lexem ;
  int encore = TRUE ;
  while (encore == TRUE)
	{
	  if (cur_lexem == last_lexem)
		{
		  encore = FALSE ;
		}
	  cur_lexem->set_original_lexem(*adr_cur_lexem) ;
	  cur_lexem = cur_lexem->get_real_next_lexem() ;
	  if (cur_lexem == NULL)
		{
		  encore = FALSE ;
		}
	}

#ifdef DUMP_LEXEMS
  fprintf(stdout, "\n\n--- DEBUT DUMP DES LEXEMES APRES COLLAGE ---\n") ;
  lex_get_first_lexem()->dump_lexems() ;
  TRACE2("last_lexem = 0x%08x %s",
		 lex_get_last_lexem(),
		 lex_get_last_lexem()->get_lex_name()) ;
  fprintf(stdout, "\n---  FIN  DUMP DES LEXEMES APRES COLLAGE ---\n") ;
#endif

  // Restauration T_lexem::last_lexem et file_name
  TRACE2("restauration T_lexem::last_lexem %x:%s",
		 save_last_lexem, save_last_lexem->get_lex_ascii()) ;

  lex_set_last_lexem(save_last_lexem) ;
  set_file_name(save_file_name) ;

  // On se remet au debut du flux qu'on a expanse
  *adr_cur_lexem = first_lexem ;
  EXIT_TRACE ;
  TRACE3("<-- include_file_definitions(%s, %x:%s)",
		 file_name,
		 adr_cur_lexem,
		 (*adr_cur_lexem)->get_lex_ascii()) ;

  // Liberation du contexte temporaire
  lex_unlink_current_context() ;
  lex_unlink_MD5_key_stack() ;

  // Restauration du contexte
  lex_pop_context(context) ;
}

//
//	}{ Fonction qui parse une DEFINITION
//
T_definition *analyse_single_DEFINITION(T_lexem **adr_cur_lexem,
                                                 const char* converterName,
												 T_definition **adr_first_def,
												 T_definition **adr_last_def,
												 T_file_definition **adr_first_file,
												 T_file_definition **adr_last_file,
												 T_lexem *last_code_lexem)
{
  T_lexem *cur_lexem = *adr_cur_lexem ;

#ifdef DEBUG_DEFINITIONS
  TRACE2("debut analyse_single_DEFINITION(%x : %s)",
		 cur_lexem,
		 cur_lexem->get_lex_ascii()) ;
  ENTER_TRACE ;
#endif

  syntax_check_eof(cur_lexem) ;


  T_lex_type cur_lex_type = cur_lexem->get_lex_type() ;
  // On fait un while pour expanser les fichiers de definitions car on
  // peut avoir un fichier de definitions qui appelle un autre fichier
  // de definitions
  while ( (cur_lex_type == L_STRING) || (cur_lex_type == L_LIB_FILE_NAME) )
	{
	  if (cur_lex_type == L_STRING)
		{
		  // On recupere les definitions de ce fichier
		  include_file_definitions(cur_lexem->get_value(),
                                           converterName,
								   &cur_lexem,
								   TRUE,
								   adr_first_file,
								   adr_last_file) ;
		}
	  else // (cur_lex_type == L_LIB_FILE_NAME)
		{
		  // Syntaxe < fichier > ?
		  // On recupere les definitions de ce fichier
		  include_file_definitions(cur_lexem->get_value(),
                                           converterName,
								   &cur_lexem,
								   FALSE,
								   adr_first_file,
								   adr_last_file) ;
		}

	  // Fall into EXPRES !  include_file_definitions a remis
	  // cur_lexem au debut du flux expanse donc on continue en se
	  // contentant de sauter les eventuels commentaires
	  cur_lexem = lex_skip_comments(cur_lexem) ;
	  TRACE2("on tombe ensuite dans le cas general ... reprise sur %x:%s",
			 cur_lexem,
			 cur_lexem->get_lex_ascii()) ;

	  // On reevalue cur_lex_type pour voir s'il faut continuer
	  cur_lex_type = cur_lexem->get_lex_type() ;
	}

  // On verifie que ce nom de definition n'est pas deja utilise
  T_definition *prev = find_definition(cur_lexem, *adr_first_def) ;
  T_definition *res = NULL ;

  if (prev != NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("ici : on crie car la definition %s existe deja",
			 cur_lexem->get_lex_name()) ;
#endif

	  syntax_already_error(cur_lexem,
						   CAN_CONTINUE,
						   cur_lexem->get_lex_name()) ;

	  already_location(prev->get_ref_lexem()) ;
	  res = NULL ;
	}
  else
	{
	  res = new T_definition((T_item **)adr_first_def,
							 (T_item **)adr_last_def,
							 NULL,
							 NULL,
							 &cur_lexem,
							 last_code_lexem) ;
	}

#ifdef DEBUG_DEFINITIONS
  TRACE2("lexem apres def = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "NULL" : cur_lexem->get_lex_name()) ;
#endif

  *adr_cur_lexem = cur_lexem ;
  return res ;
}

// Detourner le flux de lexemes de la clause DEFINITIONS
void move_DEFINITIONS_lexems(T_lexem *cur_lexem,
									  T_lexem *def_clause)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("move_DEFINITIONS_lexems(%x, %x)", cur_lexem, def_clause) ;
  TRACE0("DEBUT detourne flux POUR FAIRE DISPARAITRE LA CLAUSE DEF");
#endif

  T_lexem *before = def_clause->prev_lexem ;
  T_lexem *after = cur_lexem ;
  T_lexem *before_after = (after == NULL)
	? lex_get_last_lexem() : after->prev_lexem ;

  TRACE8("ICI\n after %s:%d:%d %s\n before_after %s:%d:%d %s,",
		 after->get_file_name()->get_value(),
		 after->get_file_line(),
		 after->get_file_column(),
		 after->get_lex_ascii(),
		 (before_after == NULL) ? "" : before_after->get_file_name()->get_value(),
		 (before_after == NULL) ? 0 : before_after->get_file_line(),
		 (before_after == NULL) ? 0 : before_after->get_file_column(),
		 (before_after == NULL) ? "" : before_after->get_lex_ascii()) ;
  // S'il y a des commentaires avant after, on les veut car en fait
  // ils seront accroches a after
  while (    (before_after->get_lex_type() == L_COMMENT)
		  || (before_after->get_lex_type() == L_KEPT_COMMENT) 
		  || (before_after->get_lex_type() == L_CPP_COMMENT)
		  || (before_after->get_lex_type() == L_CPP_KEPT_COMMENT)
		  )
	{
	  after = before_after ;
	  before_after = after->prev_lexem ;
	  TRACE8
		("ICI\n after %s:%d:%d %s\n before_after %s:%d:%d %s,",
			 after->get_file_name()->get_value(),
			 after->get_file_line(),
			 after->get_file_column(),
			 after->get_lex_ascii(),
			 (before_after == NULL) ? "" : before_after->get_file_name()->get_value(),
			 (before_after == NULL) ? 0 : before_after->get_file_line(),
			 (before_after == NULL) ? 0 : before_after->get_file_column(),
			 (before_after == NULL) ? "" : before_after->get_lex_ascii()) ;
	}

#ifdef DEBUG_DEFINITIONS
  TRACE4("before %x (%s), after %x (%s)",
		 before, (before == NULL) ? "NULL" : before->get_lex_name(),
		 after, (after == NULL) ? "NULL" : after->get_lex_name()) ;
#endif


  if (before == NULL)
	{
	  lex_set_first_lexem(after) ;
	}
  else
	{
	  (before->next_lexem)->prev_lexem = NULL ; // debut chainage def
	  before->next_lexem = after ;
	  TRACE2("before(%x)->next_lexem = %x", before, after) ;
	}

  if (after == NULL)
	{
	  lex_set_last_lexem(before) ;
	}
  else
	{
	  (after->prev_lexem)->next_lexem = NULL ; // fin chainage def
	  after->prev_lexem = before ;
	}
#ifdef DEBUG_DEFINITIONS
  TRACE0("FIN detourne flux ...") ;
#endif
}

//
//	}{ Fonction qui parse la clause DEFINITIONS
//
void parse_DEFINITIONS_inside_clause(T_lexem **adr_cur_lexem,
                                              const char* converterName,
											  T_definition **adr_first_def,
											  T_definition **adr_last_def,
											  T_flag **adr_def_clause,
											  T_file_definition **adr_first_file,
											  T_file_definition **adr_last_file,
											  T_lexem *last_code_lexem)
{
  // OK on entre dans la clause DEFINITIONS
#ifdef DEBUG_DEFINITIONS
  TRACE0("debut clause DEFINITIONS") ;
  TRACE1("-->parse_DEFINITIONS_inside_clause(%x)", *adr_cur_lexem) ;
  ENTER_TRACE ;
#endif

#ifdef DEBUG_DEFINITIONS
  TRACE0("clause DEFINITIONS") ;
#endif

  T_lexem *cur_lexem = *adr_cur_lexem ;
  T_lexem *def_clause = cur_lexem ;
  set_definitions_clause(def_clause) ;

  if (lex_get_definitions_clause() != NULL)
	{
	  syntax_already_error(cur_lexem,
						   CAN_CONTINUE,
						   get_catalog(C_DEF_CLAUSE)) ;
	  already_location(lex_get_definitions_clause()->get_ref_lexem()) ;
	}

  int encore = TRUE ;

  *adr_def_clause = new T_flag(NULL, NULL, cur_lexem) ;
  TRACE1("T_lexem::definitions_clause = %x", *adr_def_clause) ;
  lex_set_definitions_clause(*adr_def_clause) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  while (encore == TRUE)
	{
	  TRACE2("avant appel analyse_single_DEFINITION(%x:%s)",
			 cur_lexem, cur_lexem->get_lex_ascii()) ;

	  T_definition *cur_def = analyse_single_DEFINITION(&cur_lexem,
                                                            converterName,
														adr_first_def,
														adr_last_def,
														adr_first_file,
														adr_last_file,
														last_code_lexem) ;

	  TRACE2("apres appel analyse_single_DEFINITION(cur_lexem=%x:%s)",
			 cur_lexem, (cur_lexem == NULL) ? "null" : cur_lexem->get_lex_ascii()) ;

	  if ( (cur_lexem != NULL)

		   && (cur_lexem->get_lex_type() == L_SCOL) )
		{
		  if (cur_def != NULL)
			{
			  cur_def->set_ref_scol_lexem(cur_lexem) ;
			}

		  cur_lexem = cur_lexem->get_next_lexem() ;
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  // Il faut detourner le flux lexical de la machine
  move_DEFINITIONS_lexems(cur_lexem, def_clause) ;

  // Mise a jour du pointeur sur le flux
  *adr_cur_lexem = cur_lexem ;

#ifdef DEBUG_DEFINITIONS
  EXIT_TRACE ;
  TRACE1("<--parse_DEFINITIONS_inside_clause(%x)", *adr_cur_lexem) ;
#endif
}

//
//	}{ Fonction qui parse la clause DEFINITIONS
//
void parse_DEFINITIONS(T_lexem *first_lexem,
                                const char* converterName,
								T_definition **adr_first_def,
								T_definition **adr_last_def,
								T_flag **adr_def_clause,
								T_file_definition **adr_first_file,
								T_file_definition **adr_last_file,
								T_lexem *last_code_lexem)
{
  // OK on entre dans la clause DEFINITIONS
#ifdef DEBUG_DEFINITIONS
  TRACE2("--> parse_DEFINITIONS last_code_lexem = %x %x",
		 last_code_lexem,
		 last_code_lexem->get_next_lexem()) ;
  TRACE3("--> parse_DEFINITIONS last_code_lexem = %s:%d:%d",
		 last_code_lexem->get_file_name()->get_value(),
		 last_code_lexem->get_file_line(),
		 last_code_lexem->get_file_column()) ;
  ENTER_TRACE ;
#endif

  T_lexem *cur_lexem = first_lexem ;
  while (cur_lexem != NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE4("%d:%d SEARCH_DEF %x %s",
			 cur_lexem->get_file_line(),
			 cur_lexem->get_file_column(),
			 cur_lexem,
			 cur_lexem->get_lex_ascii()) ;
#endif

	  if (cur_lexem->get_lex_type() == L_DEFINITIONS)
		{
		  // C'est bon, on analyse
		  TRACE0("on analyse") ;
		  parse_DEFINITIONS_inside_clause(&cur_lexem,
                                                  converterName,
										  adr_first_def,
										  adr_last_def,
										  adr_def_clause,
										  adr_first_file,
										  adr_last_file,
										  last_code_lexem) ;
		  // On continue apres car il faut detecter
		  // les cas d'erreurs ou plusieurs clauses
		  // DEFINITIONS sont presentes dans un
		  // composant
		}
	  else
		{
		  // On continue la recherche.
#ifdef DEBUG_DEFINITIONS
		  T_lexem *next_lexem = cur_lexem->get_next_lexem() ;
		  TRACE8("on avance\n cur_lexem  %s:%d:%d %s\n next_lexem %s:%d:%d %s",
				 cur_lexem->get_file_name()->get_value(),
				 cur_lexem->get_file_line(),
				 cur_lexem->get_file_column(),
				 cur_lexem->get_lex_ascii(),
				 (next_lexem == NULL) ? "" : next_lexem->get_file_name()->get_value(),
				 (next_lexem == NULL) ? 0 : next_lexem->get_file_line(),
				 (next_lexem == NULL) ? 0 : next_lexem->get_file_column(),
				 (next_lexem == NULL) ? "" : next_lexem->get_lex_ascii()) ;
#endif // DEBUG_DEFINITIONS

		  cur_lexem = cur_lexem->get_next_lexem() ;
		}
	}

#ifdef DEBUG_DEFINITIONS
  EXIT_TRACE ;
  TRACE0("<-- parse_DEFINITIONS") ;
#endif

  set_definitions_clause(NULL) ;

}

//
//	}{	Obtention d'un ordre de dependance des definitions
// ATTENTION EFFET DE BORD :: Utilise les champs usr1 et usr2 et tmp
void order_DEFINITIONS(T_definition **adr_first_definition,
								T_definition **adr_last_definition)
{
#ifdef DEBUG_DEFINITIONS
  TRACE4("--> order_DEFINITIONS (%x (%x), %x (%x))",
		 adr_first_definition,
		 *adr_first_definition,
		 adr_last_definition,
		 *adr_last_definition) ;
  ENTER_TRACE ;
#endif

#ifdef DUMP_DEFINITIONS
  dump_DEFINITIONS(*adr_first_definition, *adr_last_definition) ;
#endif

  // AR: plus nécessaire maintenant que usr1 est défini comme une
  // union
  //ASSERT(sizeof(int) == sizeof(void *)) ;


  // Fabrication de la liste auxilaire des definitions
  T_list_link *first_aux = NULL ;
  T_list_link *last_aux = NULL ;
  T_definition *cur_aux = *adr_first_definition ;

  while (cur_aux != NULL)
	{
	  (void)new T_list_link(cur_aux,
							LINK_DEFINITION,
							(T_item **)&first_aux,
							(T_item **)&last_aux,
							NULL,
							NULL,
							NULL) ;
	  cur_aux = (T_definition *)cur_aux->get_next() ;
	}

  // usr1 : fleches entrantes f->usr1 == nb de fleches qui entrent dans f
  // usr2 : derniere fleche sortant f->usr2 == g ssi g est le dernier a avoir
  // mis une fleche entrante dans f
  // tmp : compteur d'ordre
  // On en profite, au passage, pour creer la liste des sources
  T_list_link *first_src = NULL ;
  T_list_link *last_src = NULL ;
  T_item **adr_first = (T_item **)&first_src ;
  T_item **adr_last = (T_item **)&last_src ;

  T_definition *cur_def = *adr_first_definition ;
  int nb_def = 0 ;

  T_definition *new_order_first = NULL ;
  T_definition *new_order_last = NULL ;

#ifdef DEBUG_DEFINITIONS
  TRACE1("Phase d'initialisation ... cur_def = %x", cur_def) ;
#endif

  // Dans cette phase d'initalisation, on cree une liste auxiliaire de
  // T_list_link. Il faut desactiver l'accrochage des commentaires,
  // sinon ils se retrouveront accroches dans cette liste auxiliaire,
  // c'est a dire perdus
  set_solve_comment(FALSE) ;

  while (cur_def != NULL)
	{
	  cur_def->set_usr1((void *)0) ;
	  cur_def->set_usr2((void *)0) ;
	  cur_def->set_tmp((void *)0) ;

#ifdef DEBUG_DEFINITIONS
	  TRACE0("-- AVANT CREATION T_list_link -- ") ;
	  ENTER_TRACE ;
#endif

	  (void)new T_list_link(cur_def,
							LINK_OTHER,
							adr_first,
							adr_last,
							NULL,
							NULL,
							NULL) ;

#ifdef DEBUG_DEFINITIONS
	  EXIT_TRACE ;
	  TRACE0("-- APRES CREATION T_list_link -- ") ;
#endif

	  nb_def ++ ;
	  cur_def = (T_definition *)cur_def->get_next() ;
	}

  // On remet le mecanisme d'accrochage  des commentaires
  set_solve_comment(TRUE) ;

  int encore = TRUE ;
  while (encore == TRUE)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("PHASE DE TRI DES %d DEFINTIONS RESTANTES", nb_def) ;
#endif

	  //
	  //	CALCUL DES LIENS ENTRANTS
	  //
#ifdef DEBUG_DEFINITIONS
	  TRACE0("CALCUL DES LIENS ENTRANTS") ;
#endif

	  T_list_link *def_link = *(T_list_link **)adr_first ;

	  while (def_link != NULL)
		{
		  cur_def = (T_definition *)def_link->get_object() ;
#ifdef DEBUG_DEFINITIONS
		  TRACE2("-- ETUDE DE LA DEFINITION %x %s",
				 def_link,
				 cur_def->get_name()->get_value()) ;
#endif

		  T_lexem *lex = cur_def->get_rewrite_rule() ;
		  TRACE1("rewrite_rule = %x", lex) ;

		  while (lex != NULL)
			{
			  TRACE2("lex = %x:%s", lex, lex->get_lex_ascii()) ;
			  //			  T_definition *def = find_definition(lex, *adr_first_definition) ;
			  T_definition *def = find_definition(lex, first_aux) ;
			  TRACE2("find_definition(%s)->%x", lex->get_lex_ascii(), def) ;

			  if (def != NULL)
				{
				  // On a trouve une definition utilisee
#ifdef DEBUG_DEFINITIONS
				  TRACE2("%s utilise %s",
						 cur_def->get_name()->get_value(),
						 def->get_name()->get_value()) ;
#endif

				  if (cur_def->get_name() == def->get_name())
					{
					  syntax_error(lex,
								   FATAL_ERROR,
								   get_error_msg(E_DEFINITION_CAN_NOT_REWRITE_ITSELF),
								   def->get_name()->get_value()) ;
					}


				  if ( (T_definition *)(def->get_usr2()) == cur_def)
					{
#ifdef DEBUG_DEFINITIONS
					  TRACE0("... ce lien avait deja ete detecte ...") ;
#endif
					}
				  else
					{
                                          int x = def->get_usr1_int() ;
                                          def->set_usr1_int(x + 1) ;
					  def->set_usr2(cur_def) ;
#ifdef DEBUG_DEFINITIONS
					  TRACE2("%s a maintenant %d liens entrants",
							 def->get_name()->get_value(),
							 x + 1) ;
#endif

					}
				}

			  lex = lex->get_next_lexem() ;
			}
		  def_link = (T_list_link *)def_link->get_next() ;
		}

	  //
	  //	ON ENLEVE LES SOURCES
	  //
#ifdef DEBUG_DEFINITIONS
	  TRACE0("ON ENLEVE LES SOURCES") ;
#endif


	  def_link = *(T_list_link **)adr_first ;
	  int remaining_defs = 0 ;

	  while (def_link != NULL)
		{
		  T_list_link *next_def_link = (T_list_link *)def_link->get_next() ;
		  cur_def = (T_definition *)def_link->get_object() ;
                  int nb_links = cur_def->get_usr1_int() ;
#ifdef DEBUG_DEFINITIONS
		  TRACE2("-- ETUDE LA DEFINITION %s a %d liens",
				 cur_def->get_name()->get_value(), nb_links) ;
#endif


		  if (nb_links == 0)
			{
#ifdef DEBUG_DEFINITIONS
			  TRACE1("-- on supprime %s", cur_def->get_name()->get_value()) ;
#endif

			  def_link->remove_from_list(adr_first, adr_last) ;
#ifdef DEBUG_DEFINITIONS
			  TRACE1("-- on ajoute %s dans new order",
					 cur_def->get_name()->get_value()) ;
#endif

			  cur_def->tail_insert((T_item **)&new_order_first,
								   (T_item **)&new_order_last) ;
			}
		  else
			{
			  // On garde cette definition -- on fait un reset des attributs
#ifdef DEBUG_DEFINITIONS
			  TRACE1("-- on garde %s", cur_def->get_name()->get_value()) ;
#endif

			  cur_def->set_usr1(NULL) ;
			  cur_def->set_usr2(NULL) ;
			  remaining_defs++ ;
			}

		  def_link = next_def_link ;
		}

#ifdef DEBUG_DEFINITIONS
	  TRACE2("FIN PHASE DE TRI DES %d DEFINTIONS --> plus que %d",
			 nb_def,
			 remaining_defs) ;
#endif

	  if (remaining_defs == 0)
		{
#ifdef DEBUG_DEFINITIONS
		  TRACE0("FIN DU TRI") ;
#endif

		  *adr_first_definition = new_order_first ;
		  *adr_last_definition = new_order_last ;

#ifdef DEBUG_DEFINITIONS
		  TRACE0("On inverse la liste ...") ;
#endif

		  (*adr_first_definition)->reverse_list((T_item **)adr_first_definition,
												(T_item **)adr_last_definition) ;

		  EXIT_TRACE ;
		  encore = FALSE ;
		}
	  else if (remaining_defs == nb_def)
		{
		  toplevel_error(CAN_CONTINUE,
						 get_error_msg(E_CYCLE_IN_DEFINITIONS)) ;

		  T_list_link *debug = *(T_list_link **)adr_first ;
		  ASSERT(object_test(debug) == TRUE) ;
		  ASSERT(debug->get_node_type() == NODE_LIST_LINK) ;
		  while (debug != NULL)
			{
			  T_definition *cur = (T_definition *)(debug->get_object()) ;
			  ASSERT(object_test(cur) == TRUE) ;
			  ASSERT(cur->get_node_type() == NODE_DEFINITION) ;
			  syntax_error(cur->get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_PART_OF_DEF_CYCLE),
						   cur->get_name()->get_value()) ;
			  debug = (T_list_link *)debug->get_next() ;
			}

		  stop() ;
		}

	  nb_def = remaining_defs ;
#ifdef DEBUG_DEFINITIONS
	  TRACE0(" --> on reboucle") ;
#endif
	}


  // Liberation  de la liste auxilaire des definitions
  list_unlink(first_aux) ;

#ifdef DEBUG_DEFINITIONS
EXIT_TRACE ;
TRACE0("<-- order_DEFINITIONS") ;
#endif // DEBUG_DEFINITIONS
}

//
//	}{	Fonction auxiliaire qui saute un argument ie va a la
//	virgule ou a la parenthese fermante suivante ou a l'EOF
//	Le champ tmp est mis a jour : il vaut "!= NULL" pour le dernier
// lexeme de l'argument, NULL pour les autres
// On construit au fur et a mesure la liste des parametres effectifs
static T_lexem *skip_param(T_lexem *cur_lexem,
									T_list_link **adr_first_effective_param,
									T_list_link **adr_last_effective_param)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("skip_param(%x) :: %s",
		 cur_lexem, (cur_lexem == NULL) ? "NULL" : cur_lexem->get_lex_name()) ;
#endif

  T_lexem *res = cur_lexem ;
  int open_paren = 0 ;	// nombre de '(' ouvertes
  int open_braces = 0 ;	// nombre de '{' ouvertes
  int open_bracket = 0 ;	// nombre de '[' ouverts

#ifdef DEBUG_DEFINITIONS
  TRACE0("-- ICI AVANT --") ;
#endif

  set_solve_comment(FALSE) ;
  (void)new T_list_link(cur_lexem,
						LINK_DEFINITION,
						(T_item **)adr_first_effective_param,
						(T_item **)adr_last_effective_param,
						NULL,
						NULL,
						cur_lexem) ;
  set_solve_comment(TRUE) ;
#ifdef DEBUG_DEFINITIONS
  TRACE0("-- ICI APRES --") ;
#endif


  for (;;)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE2("DANS LA BOUCLE : res = %x :: %s",
			 res, (res == NULL) ? "NULL" : res->get_lex_name()) ;
#endif

	  if (res == NULL)
		{
		  return NULL ;
		}

	  switch(res->get_lex_type())
		{
		case L_OPEN_PAREN :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("une '(' de plus") ;
#endif

			open_paren ++ ;
			break ;
		  }

		case L_OPEN_BRACES :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("une '{' de plus") ;
#endif
			open_braces ++ ;
			break ;
		  }

		case L_OPEN_BRACKET :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("une '[' de plus") ;
#endif
			open_bracket ++ ;
			break ;
		  }

		case L_CLOSE_PAREN :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("une ')' de plus") ;
#endif
			if  ( (open_paren-- == 0) && (open_braces == 0) && (open_bracket == 0) )
			  {
#ifdef DEBUG_DEFINITIONS
				TRACE0("-> on sort") ;
#endif

				res->set_tmp(res) ;
#ifdef DEBUG_DEFINITIONS
				TRACE2("on met a %x le tmp de %x", res, res) ;
#endif

				return res ;
			  }
#ifdef DEBUG_DEFINITIONS
			TRACE3("une ')' mais on reste car %d '(', %d '{', %d '['",
				   open_paren,
				   open_braces,
				   open_bracket) ;
#endif

			break ;
		  }

		case L_CLOSE_BRACES :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("une '}' de plus") ;
#endif
			if (--open_braces < 0)
			  {
				assert(FALSE) ; // A FAIRE : misplaced '}'
			  }
			break ;
		  }

		case L_CLOSE_BRACKET :
		  {
#ifdef DEBUG_DEFINITIONS
			TRACE0("un ']' de plus") ;
#endif
			if (--open_bracket < 0)
			  {
				assert(FALSE) ; // A FAIRE : misplaced ']'
			  }
			break ;
		  }

		case L_COMMA :
		  {
			if ( (open_braces == 0) && (open_bracket == 0) && (open_paren == 0))
			  {
#ifdef DEBUG_DEFINITIONS
				TRACE0("une ', ' -> on sort") ;
#endif

				res->set_tmp(res) ;
#ifdef DEBUG_DEFINITIONS
				TRACE2("on met a %x le tmp de %x", res, res) ;
#endif

				return res ;
			  }

			else
			  {
#ifdef DEBUG_DEFINITIONS
				TRACE3("une ', ' mais on reste car %d '(', %d '{', %d '['",
					   open_paren,
					   open_braces,
					   open_bracket) ;
#endif
			  }
		  }

		default :
		  {
			// Sinon on continue
#ifdef DEBUG_DEFINITIONS
			TRACE0("on continue") ;
#endif

		  }

#ifdef DEBUG_DEFINITIONS
		  TRACE1("on met a NULL le tmp de %x", res) ;
#endif
		}

	  res->set_tmp(NULL) ;
	  res = res->get_next_lexem() ;
	}
}


//
//	}{	Fonction auxiliaire qui dit si un identifiant est un parametre
//	formel d'une definition. Rend le parametre effectif si oui, NULL sinon
//
static T_lexem *is_a_formal_parameter(T_lexem *cur_lexem,
											   T_definition *definition,
											   T_list_link *first_effective_param)
{
  if (cur_lexem->get_lex_type() != L_IDENT)
	{
	  return FALSE ;
	}

  const char *value = cur_lexem->get_value() ;
  size_t len = cur_lexem->get_value_len() ;
#ifdef DEBUG_DEFINITIONS
  TRACE3("is_a_formal_parameter(%s, %d, %x)", value, len, definition) ;
#endif


  T_ident *cur_param = definition->get_params() ;

  int nb_param = 0 ;

  while (cur_param != NULL)
	{
	  nb_param++ ;
	  if (	(cur_param->get_name()->get_len() == len)
			&& (strcmp(cur_param->get_name()->get_value(), value) == 0) )
		{
#ifdef DEBUG_DEFINITIONS
		  TRACE1("-> OUI ! (parametre formel %d)", nb_param) ;
#endif


		  // On cherche le parametre formel associe
		  T_list_link *cur = first_effective_param ;

		  while (--nb_param > 0)
			{
			  cur = (T_list_link *)cur->get_next() ;
#ifndef NO_CHECKS
			  assert(cur) ;
#endif
			}

		  return ((T_lexem *)cur->get_object()) ;
		}

	  cur_param = (T_ident *)cur_param->get_next() ;
	}

  // Si on arrive ici c'est que ce n'etait pas un parametre formel
#ifdef DEBUG_DEFINITIONS
  TRACE0("-> NON") ;
#endif

  return NULL ;
}

//
//	}{	Fonction qui verifie le nombre de parametres effectif
//	    d'une utilisation de definition : il doit valoir nb_params
//	Construit en meme temps la liste des parametres effectifs
//	Retourne le dernier lexem
static T_lexem *check_params(T_lexem *first_lexem,
									  T_definition *cur_def,
									  T_list_link **adr_first_effective_param,
									  T_list_link **adr_last_effective_param)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("check_params(%s, %s)",
		 first_lexem->get_lex_name(),
		 cur_def->get_name()->get_value()) ;
#endif


  int nb_params = cur_def->get_nb_params() ;
  if (nb_params == 0)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE0("pas de parametre !") ;
#endif

	  return first_lexem ;
	}

  T_lexem *last_lexem = first_lexem->get_next_lexem() ;

  if ( 	(last_lexem == NULL)
		|| (last_lexem->get_lex_type() != L_OPEN_PAREN) )
	{
	  syntax_unexpected_error(last_lexem,
							  MULTI_LINE,
							  get_catalog(C_OPEN_PAREN)) ;
	  macro_error(cur_def) ;
	}

  int encore = TRUE ;
  last_lexem = syntax_check_eof(last_lexem->get_next_lexem()) ;
  int params_trouves = 0 ;

  while (encore == TRUE)
	{
	  // Au fur et a mesure de la verification, on construit la liste des
	  // parametres effectifs
	  last_lexem = skip_param(last_lexem,
							  adr_first_effective_param,
							  adr_last_effective_param) ;
	  syntax_check_eof(last_lexem) ;
	  params_trouves ++ ;

	  if ( 	(last_lexem != NULL)
			&& (last_lexem->get_lex_type() == L_COMMA) )
		{
		  last_lexem = last_lexem->get_next_lexem() ;
		}
	  else
		{
		  encore = FALSE ;
		}
	}

#ifdef DEBUG_DEFINITIONS
  TRACE1("-> %d parametres effectifs trouves", params_trouves) ;
#endif


  if (params_trouves != nb_params)
	{
	  syntax_error(last_lexem,
				   MULTI_LINE,
				   get_error_msg(E_MACRO_INVALID_ARGS),
				   params_trouves,
				   cur_def->get_name()->get_value()) ;
	  macro_error(cur_def) ;
	}

  //
  // Verifie parenthese fermante
  //
#ifdef DEBUG_DEFINITIONS
  TRACE0("verifie ')'") ;
#endif

  if ( 	(last_lexem == NULL)
		|| (last_lexem->get_lex_type() != L_CLOSE_PAREN) )
	{
	  syntax_unexpected_error(last_lexem,
							  MULTI_LINE,
							  get_catalog(C_CLOSE_PAREN)) ;
	  macro_error(cur_def) ;
	}

#ifdef DEBUG_DEFINITIONS
  TRACE1("fin de check_params -> last_lexem = %s",
		 (last_lexem == NULL) ? "NULL" : last_lexem->get_lex_name()) ;
#endif


  return last_lexem ;
}


//
//	}{	Fonction qui clone la partie droite d'une definition
//	dans une liste propre
//
static void clone_rewrite_rule(T_lexem *original_lexem,
										T_definition *cur_def,
										T_lexem **adr_first_expansion,
										T_lexem **adr_last_expansion,
										T_list_link *first_effective_param)
{
#ifdef DEBUG_DEFINITIONS
  TRACE1("debut clone_rewrite_rule() original_lexem %s",
		 original_lexem->get_lex_name()) ;
  ENTER_TRACE ;
#endif


  T_lexem *cur_subst = cur_def->get_rewrite_rule() ;

#ifdef DEBUG_DEFINITIONS
  if (cur_subst != NULL)
	{
	  TRACE1("cur_subst = %s", cur_subst->get_lex_name()) ;
	}
#endif


  while (cur_subst != NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("lexeme courant = %s", cur_subst->get_lex_name()) ;
#endif


	  T_lexem *effective_param = is_a_formal_parameter(cur_subst,
													   cur_def,
													   first_effective_param) ;

	  if (effective_param == NULL)
		{
		  // Le lexeme courant n'est pas un parametre formel donc
		  // on le clone tout simplement
#ifdef DEBUG_DEFINITIONS
		  TRACE1("clonage simple de %s", cur_subst->get_lex_name()) ;
#endif

		  T_lexem *lexem = new T_lexem(cur_subst,
									   original_lexem,
									   adr_first_expansion,
									   adr_last_expansion) ;
		  lexem->set_rewrite_rule_lexem(cur_subst) ;
		}
	  else
		{
		  // Le lexeme courant est un parametre formel
		  // Il faut cloner sa valeur effective
#ifdef DEBUG_DEFINITIONS
		  TRACE2("clonage de la valeur effective %s de %s",
				 effective_param->get_lex_name(),
				 cur_subst->get_lex_name()) ;
#endif


#ifdef DEBUG_DEFINITIONS
		  TRACE0("DEBUT PARAMETRE EFFECTIF") ;
		  ENTER_TRACE ;
#endif


		  // on a utilise le champ tmp pour savoir la fin du parametre
		  // (mis a jour par skip_param)
		  while (effective_param->get_tmp() == NULL)
			{
#ifdef DEBUG_DEFINITIONS
			  TRACE1("... %s ...", effective_param->get_lex_name()) ;
#endif

			  T_lexem *lexem = new T_lexem(effective_param,
										   original_lexem,
										   adr_first_expansion,
										   adr_last_expansion) ;
			  lexem->set_rewrite_rule_lexem(effective_param) ;
			  effective_param = effective_param->get_next_lexem() ;
			}
#ifdef DEBUG_DEFINITIONS
		  TRACE0("FIN PARAMETRE EFFECTIF") ;
#endif

		  EXIT_TRACE ;
		}

	  // La suite
	  cur_subst = cur_subst->get_next_lexem() ;
	}

#ifdef DEBUG_DEFINITIONS
  EXIT_TRACE ;
  TRACE0("fin clone_rewrite_rule") ;
#endif

}

//
//	}{ Fonction qui remplace un appel de definition par la
//  valeur expansee
void rebuild_chain(T_lexem *first_src_lexem,
							T_lexem *last_src_lexem,
							T_lexem *first_expanded_lexem,
							T_lexem *last_expanded_lexem,
							T_lexem **adr_first_lexem,
							T_lexem **adr_last_lexem)
{
#ifdef DEBUG_DEFINITIONS
  TRACE4("rebuild_chain remplace de %s->%s par %s->%s",
		 first_src_lexem->get_lex_name(),
		 last_src_lexem->get_lex_name(),
		 first_expanded_lexem->get_lex_name(),
		 last_expanded_lexem->get_lex_name()) ;
#endif


#ifdef DEBUG_DEFINITIONS
  TRACE0("CHAINAGE") ;
#endif


#ifdef DEBUG_DEFINITIONS
  TRACE4("first_src_lexem 0x%08x \"%s\", last_src_lexem 0x%08x\"%s\"",
		 first_src_lexem,
		 (first_src_lexem == NULL) ? "NULL" : first_src_lexem->get_lex_name(),
		 last_src_lexem,
		 (last_src_lexem == NULL) ? "NULL" : last_src_lexem->get_lex_name()) ;
#endif


  T_lexem *prev = first_src_lexem->get_prev_lexem() ;
  T_lexem *next = last_src_lexem->get_next_lexem() ;

#ifdef DEBUG_DEFINITIONS
  TRACE4("prev 0x%08x \"%s\", next 0x%08x\"%s\"",
		 prev,
		 (prev == NULL) ? "NULL" : prev->get_lex_name(),
		 next,
		 (next == NULL) ? "NULL" : next->get_lex_name()) ;
#endif


#ifdef DEBUG_DEFINITIONS
  TRACE2("adr_first \"%s\", adr_last \"%s\"",
		 (*adr_first_lexem == NULL) ? "NULL" : (*adr_first_lexem)->get_lex_name(),
		 (*adr_last_lexem == NULL) ? "NULL" : (*adr_last_lexem)->get_lex_name()) ;
#endif


  if (prev == NULL)
	{
	  // On remplace le premier lexeme du fichier
	  *adr_first_lexem = first_expanded_lexem ;
	}
  else
	{
	  // Cas general : on rechaine
	  prev->next_lexem = first_expanded_lexem ;
	  TRACE2("prev(%x)->next_lexem = %x", prev, first_expanded_lexem) ;
	}

  if (next == NULL)
	{
	  // On remplace le dernier lexeme du fichier
	  *adr_last_lexem = last_expanded_lexem ;
	}
  else
	{
	  // Cas general : on rechaine
	  next->prev_lexem = last_expanded_lexem ;
	}
}

//
//	}{	Remplace un identifiant par une definition le cas echeant
//
void swap_ident(T_lexem *first_lexem,
						 T_definition *cur_def,
						 T_lexem **adr_first_lexem,
						 T_lexem **adr_last_lexem)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("swap_ident(%x::%s)", first_lexem, first_lexem->get_value()) ;
  ENTER_TRACE ;
#endif
  TRACE0("swap_ident") ;

  set_expand_state(TRUE) ;

  // On verifie le nombre de parametres
  // Au passage, on recupere le dernier lexeme � remplacer
  // Au passage, on fait construire la liste des parametres effectifs
  T_list_link *first_effective_param = NULL ;
  T_list_link *last_effective_param = NULL ;
  T_lexem *last_lexem = check_params(first_lexem,
									 cur_def,
									 &first_effective_param,
									 &last_effective_param) ;

#ifdef DEBUG_DEFINITIONS
  TRACE4("AVANT CLONE ICI last_lexem 0x%08x %s -> next 0x%08x %s",
		 last_lexem, (last_lexem == NULL) ? "NULL" : last_lexem->get_lex_name(),
		 last_lexem->get_next_lexem(),
		 (last_lexem->get_next_lexem() == NULL) ? "NULL" :
		 last_lexem->get_next_lexem()->get_lex_name()) ;
#endif

  // On clone la valeur de remplacement
  T_lexem *first_expansion = NULL ;
  T_lexem *last_expansion = NULL ;

  clone_rewrite_rule(first_lexem,
					 cur_def,
					 &first_expansion,
					 &last_expansion,
					 first_effective_param) ;

  // On numerote la regle de reecriture
  int cpt = (first_lexem->get_prev_lexem() == NULL)
	? 0 : first_lexem->get_prev_lexem()->get_position() ;
  T_lexem *cur_number = first_expansion ;
  while (cur_number != NULL)
	{
	  cur_number->set_position(++cpt) ;
	  cur_number = cur_number->get_next_lexem() ; // On veut definitions EXPANSEES
	}

  // On chaine !
#ifdef DEBUG_DEFINITIONS

  TRACE4("APRES CLONE ICI last_lexem 0x%08x %s -> next 0x%08x %s",
		 last_lexem,
		 (last_lexem == NULL) ? "NULL" : last_lexem->get_lex_name(),
		 (last_lexem == NULL) ? NULL : last_lexem->get_next_lexem(),
		 (last_lexem == NULL) ? "NULL" :
             (last_lexem->get_next_lexem() == NULL) ? "NULL" :
		          last_lexem->get_next_lexem()->get_lex_name()) ;

#endif


  T_lexem *before = first_lexem->get_prev_lexem() ;
  T_lexem *after = last_lexem->get_next_lexem() ;

#ifdef DEBUG_DEFINITIONS
  TRACE2("before %x %s",
		 before,
		 (before == NULL) ? "NULL" : (before)->get_lex_name()) ;
  TRACE2("after %x %s",
		 after,
		 (after == NULL) ? "NULL" : (after)->get_lex_name()) ;
#endif


  if (before == NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE0("insertion en tete") ;
#endif
	  if( first_expansion == NULL)
		{
		  // cas d'une definition vide
		  *adr_first_lexem = after ;
		}
	  else
		{
		  *adr_first_lexem = first_expansion ;
		}
	}
  else
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("insertion apres %s", before->get_lex_name()) ;
#endif
	  if (first_expansion == NULL)
		{
		  // cas d'une substitution vide
		  before->set_next_expanded_lexem(after) ;
		}
	  else
		{
		  before->set_next_expanded_lexem(first_expansion) ;
		}
	}

  if (after == NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE0("insertion en queue") ;
#endif
	  if (last_expansion == NULL)
		{
		  // cas d'une definition vide
		  *adr_last_lexem = before ;
		}
	  else
		{
		  *adr_last_lexem = last_expansion ;
		}
	}
  else
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("insertion avant %s", after->get_lex_name()) ;
#endif
	  if (last_expansion == NULL)
		{
		  after->set_prev_expanded_lexem(before);
		}
	  else
		{
		  after->set_prev_expanded_lexem(last_expansion) ;
		}
	}

  if (first_expansion != NULL)
	{
	  ASSERT(last_expansion != NULL) ;
	  first_expansion->set_prev_lexem(before) ;
	  last_expansion->set_next_lexem(after) ;
	}

  // Ici patch numerotation Si on a expanse une def dans une def
  // (i.e. 'g' dans f(g)) alors on a numerote f, puis g mais les
  // numeros de g expanse provoquent un decalage dans la fin de f
  // (i.e. les lexemes apres g). Affreux ! donc pour les lexemes apres
  // g qui proviennent d'une expansion, on renumerote. On repere ces
  // lexemes au fait que leur position est non NULL PS on n'est pas
  // parasite a ce niveau par les L_EVL de position 0 car ils sont
  // ajoutes plus tard par l'analyseur LR
  cur_number = after ;
  while ( (cur_number != NULL) && (cur_number->get_position() != 0) )
	{
	  cur_number->set_position(++cpt) ;
	  cur_number = cur_number->get_next_lexem() ; // On veut definitions EXPANSEES
	}

#ifdef DEBUG_DEFINITIONS
  TRACE0("fin de swap_ident") ;
#endif
  EXIT_TRACE ;
  return ;
}

//
//	}{	Remplace un identifiant par la partie droite de definition le cas echeant
//		Renvoie TRUE si la substitution a eu lieu, FALSE sinon
int expand_ident(T_lexem *cur_lexem,
						  T_lexem **adr_first_lexem,
						  T_lexem **adr_last_lexem,
						  T_definition *first_definition)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("expand_ident(%x::%s, %x, %x)", cur_lexem, cur_lexem->get_value()) ;
  ENTER_TRACE ;
#endif

#ifdef DUMP_DEFINITIONS
  dump_DEFINITIONS(first_definition, NULL) ;
#endif

  if (cur_lexem->get_expanded() == TRUE)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE0("deja fait") ;
	  EXIT_TRACE ;
#endif

	  return FALSE ;
	}

  cur_lexem->set_expanded(TRUE) ;

  T_definition *cur_def = first_definition ;

  cur_def = find_definition(cur_lexem, first_definition) ;

  if (cur_def != NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("DEBUT EXPANSION DE %s", cur_lexem->get_lex_name()) ;
	  ENTER_TRACE ;
#endif
	  swap_ident(cur_lexem,
				 cur_def,
				 adr_first_lexem,
				 adr_last_lexem) ;
#ifdef DEBUG_DEFINITIONS
	  EXIT_TRACE ;
	  TRACE1("FIN EXPANSION DE %s", cur_lexem->get_lex_name()) ;
#endif

	  return TRUE ;
	}

  EXIT_TRACE ;
#ifdef DEBUG_DEFINITIONS
  TRACE0("fin de expand_ident") ;
#endif

  return FALSE ;
}

//
//	}{	Expansion des definitions dans le code
//
static void expand_def(T_lexem **adr_first_lexem,
								T_lexem **adr_last_lexem,
								T_definition *first_def)
{
#ifdef DEBUG_DEFINITIONS
  TRACE2("expand_def de %x a %x", *adr_first_lexem, *adr_last_lexem) ;
  ENTER_TRACE ;
#endif

#ifdef DUMP_DEFINITIONS
  dump_DEFINITIONS(first_def, NULL) ;
#endif

  T_lexem *cur_lexem = *adr_first_lexem ;

  while (cur_lexem != NULL)
	{
	  T_lexem *next_lexem = cur_lexem->get_next_lexem() ;
#ifdef DEBUG_DEFINITIONS
	  TRACE2("ici cur_lexem %x %s", cur_lexem, cur_lexem->get_lex_name()) ;
#endif

	  if (cur_lexem->get_lex_type() == L_IDENT)
		{
#ifdef DEBUG_DEFINITIONS
		  TRACE1("etude de l'identifiant %s", cur_lexem->get_value()) ;
#endif
		  expand_ident(cur_lexem, adr_first_lexem, adr_last_lexem, first_def) ;
#ifdef DEBUG_DEFINITIONS
		  TRACE1("fin de l'etude de l'identifiant %s", cur_lexem->get_value()) ;
#endif
		}

	  cur_lexem = next_lexem ;
	}

#ifdef DEBUG_DEFINITIONS
  EXIT_TRACE ;
  TRACE0("fin expand_def") ;
#endif
}

//
//	}{	Expansion des definitions dans les definitions
//
void expand_DEFINITIONS_in_definitions(T_definition *first_definition,
												T_definition *last_definition)
{
#ifdef DEBUG_DEFINITIONS
  TRACE0("expand_definitions_in_definitions") ;
  ENTER_TRACE ;
#endif

#ifdef DUMP_DEFINITIONS
  dump_DEFINITIONS(first_definition, last_definition) ;
#endif

  // A FAIRE : ameliorer, car l'etat est global donc si
  // un composant a une definition expansee, on les reparcourt
  // tous
  set_expand_state(FALSE) ;

  T_definition *cur_def = first_definition ;

  while (cur_def != NULL)
	{
#ifdef DEBUG_DEFINITIONS
	  TRACE1("########### DEBUT ETUDE DE LA DEF %s #############",
			 cur_def->get_name()->get_value()) ;
	  ENTER_TRACE ;
#endif

	  expand_def(&cur_def->first_rewrite_rule,
				 &cur_def->last_rewrite_rule,
				 cur_def) ;

#ifdef DEBUG_DEFINITIONS
	  EXIT_TRACE ;
	  TRACE1("###########  FIN   ETUDE DE LA DEF %s #############",
			 cur_def->get_name()->get_value()) ;
#endif

	  cur_def = (T_definition *)cur_def->get_next() ;
	}

  EXIT_TRACE ;
#ifdef DEBUG_DEFINITIONS
  TRACE0("fin expand_definitions_in_definitions") ;
#endif
}

//
//	}{	Expansion des definitions dans le code
//
void expand_DEFINITIONS_in_source(T_lexem **adr_first_lexem,
										   T_lexem **adr_last_lexem,
										   T_definition *first_definition,
										   T_definition *last_definition)
{
#ifdef DEBUG_DEFINITIONS
  TRACE0("--> expand_definitions_in_source") ;
  ENTER_TRACE ;
#endif

  // A FAIRE : ameliorer, car l'etat est global donc si
  // un composant a une definition expansee, on les reparcourt
  // tous
  set_expand_state(FALSE) ;

#ifdef DUMP_DEFINITIONS
  dump_DEFINITIONS(first_definition, last_definition) ;
#endif

  expand_def(adr_first_lexem,
			 adr_last_lexem,
			 first_definition) ;

#ifdef DEBUG_DEFINITIONS
	EXIT_TRACE ;
  TRACE0("<-- fin expand_definitions_in_source") ;
#endif

}

//
//	}{	Fonction qui cherche un param de nom donne
T_ident *T_definition::find_param(const char *value,
										   size_t value_len)
{
#ifdef DEBUG_DEFINITIONS
  TRACE1("T_defintion::find_param(%s)", value) ;
#endif

  T_ident *cur = first_param ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return NULL ;
		}

	  if (cur->get_name()->compare(value, value_len) == TRUE)
		{
		  return cur ;
		}

	  cur = (T_ident *)cur->get_next() ;
	}
}

static T_lexem *definitions_clause_si = NULL ;
void set_definitions_clause(T_lexem *ref_lexem)
{
#ifdef DEBUG_DEFINITIONS
  TRACE1("set_definitions_clause(%x)", ref_lexem) ;
#endif
  definitions_clause_si = ref_lexem ;
}

T_lexem *get_definitions_clause(void)
{
#ifdef DEBUG_DEFINITIONS
  TRACE1("get_definitions_clause() -> %x", definitions_clause_si) ;
#endif
  return definitions_clause_si ;
}

static int expand_state_si = FALSE ;
void set_expand_state(int new_expand_state)
{
  expand_state_si = new_expand_state ;
}
int get_expand_state(void)
{
  return expand_state_si ;
}

//
//	}{	Fin du fichier
//
