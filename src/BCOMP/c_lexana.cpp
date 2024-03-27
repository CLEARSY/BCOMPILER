/***************************** CLEARSY **************************************
* Fichier : $Id: c_lexana.cpp,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Compilations :	*	-DCHECK_INDEX pour verifier que l'on ne deborde pas
*						du buffer de lecture
*					*	-DDUMP_LEXEMS pour dumper les lexemes avant et apres
*						expansion des definitions
*					*	-DDEBUG_LEX pour avoir les traces de l'analyse lexicale
*					*	-DDEBUG_REC_MODE pour les traces du mode record ':' -> '\:'
*
* Description :		Fonctions d'analyse lexicale de haut niveau
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
RCS_ID("$Id: c_lexana.cpp,v 1.16 2007-07-27 15:19:46 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unicode/utypes.h>
#include <unicode/ucnv.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_pstack.h"


//
// Debut Donnees partagees avec s_lex.cpp
//
// Types partages
//#define DEFINE_TEMPLATES
//#include "s_lrstck.cpp"
//template class T_lr_stack<T_symbol *> ;

// Donnees partagess
extern char *file_name_scp ;
extern T_symbol *sym_file_name_scp ;
extern UChar *load_sct_UChar ;
extern char *los_buf ;
extern T_pstack *lifo_pos_sop ;
extern int index_si ;
extern int cur_line_si ;
extern int cur_column_si ;
extern MD5Context ctx ;
extern unsigned char MD5buf[17] ;
//extern T_lr_stack<T_symbol *> *MD5_key_stack_sop ;

// Fonctions partagees
extern void get_lexems(void) ;
extern FILE *open_source_file (const char *file_name_acp,
					size_t &path_len,
					size_t &file_size);
extern void reset_lexem_context(void);


//
// Fin donnees partagees
//
UConverter* uconv;

//
// }{ Chargement en memoire du fichier a parser
//
int load_file(const char *file_name_acp,
					   const char *second_file_name_acp,
					   const char *third_file_name_acp,
					   T_lexem *load_lexem,
					   MD5Context *adr_ctx,
					   int complain_if_unreadable)
{
  TRACE3("load_file(%s, %s) complain_if_unreadable = %s",
		 file_name_acp,
		 (second_file_name_acp == NULL) ? "" : second_file_name_acp,
		 (complain_if_unreadable == TRUE) ? "TRUE" : "FALSE") ;
  FILE *fd ;
  char *load_sct;
  int s_li ; // Caractere lu
  size_t offset_li ; // Indice pour remplir le tableau load_sct
  size_t len = 0 ;
  size_t size = 0 ;

  // Reset variable premier lexeme de code
  reset_first_compo_code_lexem() ;
  set_first_component_started(FALSE) ;

#ifdef TRACE
  if (second_file_name_acp == NULL)
	{
	  TRACE1("Lecture du fichier \"%s\" ", file_name_acp) ;
	}
  else
	{
	  TRACE2("Lecture du fichier \"%s\" ou \"%s\" ",
			 file_name_acp,
			 second_file_name_acp) ;
	}
#endif // TRACE


  // On caste en (char *) car on sait qu'on n'ecrira pas dans file_name
  // sans la reallouer au prealable
  char *file_name = (char *)file_name_acp ;

  TRACE0("premier essai") ;
  fd = open_source_file(file_name_acp, len, size) ;

  if ( (fd == NULL) && (second_file_name_acp != NULL) )
	{
	  TRACE0("deuxieme essai") ;
	  fd = open_source_file(second_file_name_acp, len, size) ;
	  // On caste en (char *) car on sait qu'on n'ecrira pas dans file_name
	  // sans la reallouer au prealable
	  file_name = (char *)second_file_name_acp ;
	  if ( (fd == NULL) && (third_file_name_acp != NULL) )
		{
		  TRACE0("troisieme essai") ;
		  fd = open_source_file(third_file_name_acp, len, size) ;
		  // On caste en (char *) car on sait qu'on n'ecrira pas dans file_name
		  // sans la reallouer au prealable
		  file_name = (char *)third_file_name_acp ;
		}
	}

  if (fd == NULL)
	{
	  if (complain_if_unreadable == TRUE)
		{
		  // Attention ici on peut avoir errno = 0 dans le cas du mode
		  // projet, si on cherche un composant absent du projet. Dans
		  // ce cas il ne faut pas appeller strerror mais donne un
		  // message adequat
		  const char *msg = (errno == 0)
			? get_catalog(C_NO_SUCH_FILE_IN_PROJECT) : strerror(errno) ;

		  // On indique si possible la provenance
		  if (load_lexem == NULL)
			{
			  toplevel_error(FATAL_ERROR,
							 get_error_msg(E_NO_SUCH_FILE),
							 file_name_acp,
							 msg) ;
			}
		  else
			{
			  syntax_error(load_lexem,
						   FATAL_ERROR,
						   get_error_msg(E_NO_SUCH_FILE),
						   file_name_acp,
						   msg) ;
			}
		}
	  else
		{
		  // Le client a demande explicitement de ne rien dire en cas d'erreur
		  // Donc on se tait
		  return FALSE ;
		}
	}

  // Mise a jour du nom du fichier ...
  set_file_name(file_name) ;
  clone(&file_name_scp, file_name) ;

  const char *msg = (get_file_definitions_mode() == TRUE)
	? get_catalog(C_USING_PATH_XX_FOR_FILE_DEF)
	: get_catalog(C_USING_PATH_XX_FOR_COMPONENT) ;

  verbose_describe(msg,
				   (lex_get_path() == NULL) ? "." : lex_get_path(),
				   file_name,
				   file_name) ;


  TRACE2("ici file_name_scp = %x : %s", file_name_scp, file_name_scp) ;
  sym_file_name_scp = lookup_symbol(file_name_scp, len) ;
  TRACE2("Taille de %s = %d bytes", file_name, size) ;

  load_sct = (char *)s_volatile_malloc(size + 1) ;

  los_buf = (char *)s_volatile_malloc(size + 1) ;

#ifdef CHECK_INDEX
  size_si = stats_ls.st_size ;
#endif
  // Allocation d'une LIFO pour garder les positions
  // On se sert de cette pile pour les retours en arriere en cas de
  // probleme d'analyse lexicale de <fichier> (i.e. dans le cas
  // general du parsing de '<') DOnc la pile doit etre suffisament
  // grande et size est un bon majorant.
  lifo_pos_sop = new T_pstack(size + 1) ;

  offset_li = 0 ;
  while ((s_li = getc(fd)) != EOF)
	{
	  load_sct[offset_li++] = s_li ;

	  ASSERT (offset_li <= size + 1) ;
	}

  load_sct[offset_li++] = '\0' ;
  fclose(fd) ;

  UConverter* luconv = 0;
  UErrorCode err = U_ZERO_ERROR;
  int32_t signatureLength = 0;
  const char *encoding = ucnv_detectUnicodeSignature(load_sct,size+1,&signatureLength,&err);
  err = U_ZERO_ERROR;
  if (encoding) {
      luconv = ucnv_open(encoding, &err);
  }
  if (!luconv) {
      luconv = uconv;
  }


  load_sct_UChar = (UChar *) new UChar[size + 2];
  UChar* target = load_sct_UChar;
  const UChar* targetLimit = load_sct_UChar + sizeof(UChar)*(size+2);
  const char* source = load_sct + signatureLength;
  const char* sourceLimit = load_sct + offset_li;
  err = U_ZERO_ERROR;

  ucnv_toUnicode(luconv,
                 &target,
                 targetLimit,
                 &source,
                 sourceLimit,
                 NULL,
                 TRUE,
                 &err);

  index_si = -1 ;

  reset_lexem_context() ;
  cur_line_si = 1 ;
  cur_column_si = 0 ;

  // Initialisation du contexte MD5
  MD5Init(&ctx) ;
  lex_set_MD5_context(&ctx) ;

  // Mise a jour de la valeur de retour
  if (adr_ctx != NULL)
	{
	  adr_ctx = &ctx ;
	}

  get_lexems() ;

  // Fin du calcul de la cle MD5
  char MD5_key[33] ;
  MD5Final(MD5buf, &ctx) ;
  sprintf(MD5_key,
		  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		  MD5buf[0], MD5buf[1], MD5buf[2], MD5buf[3],
		  MD5buf[4], MD5buf[5], MD5buf[6], MD5buf[7],
		  MD5buf[8], MD5buf[9], MD5buf[10], MD5buf[11],
		  MD5buf[12], MD5buf[13], MD5buf[14], MD5buf[15]) ;

  lex_set_MD5_key(lookup_symbol(MD5_key, 32)) ;

  TRACE1("checksum <%s> : push", lex_get_MD5_key()->get_value()) ;
  lex_push_MD5_key_stack(lex_get_MD5_key()) ;

  free(file_name_scp) ; file_name_scp = NULL ;
  s_free(load_sct) ;
  s_free(los_buf) ; los_buf = NULL ;
  delete lifo_pos_sop ; lifo_pos_sop = NULL ;
  delete[] load_sct_UChar ; load_sct_UChar = NULL ;

  if (get_file_definitions_mode() == TRUE)
	{
	  TRACE0("ici : on sort") ;

	  return TRUE ;

	}

  return TRUE ;
}

// Calcul du checksum expanse
static T_symbol *compute_expanded_checksum(T_lexem *first_lexem)
{
  TRACE0(">> compute_expanded_checksum") ;
  ENTER_TRACE ;

  // Initialisation du contexte
  MD5Context ctx ;
  MD5Init(&ctx) ;

  // Parcours du flux de lexemes expanses sans commentaires
  T_lexem *cur_lexem = lex_skip_comments(first_lexem) ;

  while (cur_lexem != NULL)
	{
	  // Mise a jour du checksum
	  if (cur_lexem->get_value() != NULL)
		{
		  // identificateur
		  MD5Update(&ctx,
					(unsigned char *)cur_lexem->get_value(),
					cur_lexem->get_value_len()) ;
		}
	  else
		{
		  // Mot-cle
		  MD5Update(&ctx,
					(unsigned char *)cur_lexem->get_lex_ascii(),
					cur_lexem->get_lex_ascii_len()) ;
		}

	  // Le suivant : on utilise get_next_lexem pour sauter les commentaires
	  // et parcourir le flux expanse des definitions
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}


  // Finalisation du calcul de la cle
  char MD5_key[33] ;
  unsigned char MD5buf[17] ;
  MD5Final(MD5buf, &ctx) ;
  sprintf(MD5_key,
		  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		  MD5buf[0], MD5buf[1], MD5buf[2], MD5buf[3],
		  MD5buf[4], MD5buf[5], MD5buf[6], MD5buf[7],
		  MD5buf[8], MD5buf[9], MD5buf[10], MD5buf[11],
		  MD5buf[12], MD5buf[13], MD5buf[14], MD5buf[15]) ;

  T_symbol *expanded_checksum = lookup_symbol(MD5_key, 32) ;

  EXIT_TRACE ;
  TRACE1("<< compute_expanded_checksums -> %s", expanded_checksum->get_value()) ;
  return expanded_checksum ;
}



//
// }{ Chargement en memoire du fichier a parser
//
int lex_analysis(const char *file_name_acp,
                          const char *converterName,
                          const char *second_file_name_acp,
                          const char *third_file_name_acp,
						  T_lexem *load_lexem,
                          MD5Context *adr_ctx,
                          int complain_if_unreadable)
{
  TRACE3(">> lex_analysis(%s, %s) complain_if_unreadable = %s",
		 file_name_acp,
		 (second_file_name_acp == NULL) ? "" : second_file_name_acp,
		 (complain_if_unreadable == TRUE) ? "TRUE" : "FALSE") ;
  ENTER_TRACE ;

  UErrorCode err = U_ZERO_ERROR;
  if (converterName != NULL)
      uconv = ucnv_open(converterName, &err);
  if (converterName == NULL || U_FAILURE(err)) {
      err = U_ZERO_ERROR;
      uconv = ucnv_open("UTF8", &err);
  }

  // Chargement du fichier et calcul des flux de lexemes
  int status = load_file(file_name_acp,
						 second_file_name_acp,
						 third_file_name_acp,
						 load_lexem,
						 adr_ctx,
						 complain_if_unreadable) ;

  if ( (complain_if_unreadable == FALSE) && (status == FALSE) )
	{
	  return FALSE ;
	}

  if (get_file_definitions_mode() == TRUE)
	{
	  return TRUE ;
	}

  // Dernier lexeme de code du dernier composant
  T_lexem *last_compo_code_lexem = NULL ;

  //
  // DEBUT DU TRAITEMENT SUR TOUS LES FLUX DE LEXEMES
  //
  // Init pile copie
  reset_tmp_first_lex_stack() ;
  reset_tmp_last_lex_stack() ;

  // Init pile definitions
  reset_first_def_stack() ;
  reset_last_def_stack() ;
  reset_def_clause_stack() ;
  reset_first_file_def_stack() ;
  reset_last_file_def_stack() ;

  while (is_first_lex_stack_empty() == FALSE)
	{
	  TRACE0(">> BOUCLE ANALYSE LEXICALE") ;
	  ENTER_TRACE ;
	  pop_first_lex_stack() ;
	  pop_last_lex_stack() ;
	  pop_last_code_lexem_stack() ;
	  T_lexem *first_lexem = lex_get_first_lexem() ;
	  T_lexem *last_lexem = lex_get_last_lexem() ;
	  T_lexem *last_code_lex = lex_get_last_code_lexem() ;
	  last_compo_code_lexem = last_code_lex ;

	  // Reset contexte clause definitions
	  lex_set_definitions_clause(NULL) ;

	  // A mesure que l'on vide la pile principale, on reconstruit
	  // sa copie afin de pouvoir la restaurer en fin de traitement
	  push_tmp_first_lex_stack(first_lexem) ;
	  push_tmp_last_lex_stack(last_lexem) ;

	  // Debut traitements generiques sur le flux lexical. Ils n'ont
	  // pas lieu dans le cas file_definitions_mode car dans ce cas on
	  // a ete appele recursivement pour cacluler les lexemes du
	  // fichier de definitions. Ces lexemes sont ensuite inseres dans
	  // le flux lexical du composant en cours d'analyse. C'est
	  // ensuite que l'on passera ici
	  // mise a jour des champs prev_line, next_line, start_of_line
	  compute_lines(first_lexem) ;

	  // DEBUT ZONE NON TRAITEE
	  // Premiere estimation du dernier lexeme de code du fichier,
	  // clause DEFINITIONS non expansee

	  while (    (last_code_lex != NULL)
				 && (	(last_code_lex->get_lex_type() == L_COMMENT)
		  				|| (last_code_lex->get_lex_type() == L_KEPT_COMMENT) 
						|| (last_code_lex->get_lex_type() == L_CPP_COMMENT)
						|| (last_code_lex->get_lex_type() == L_CPP_KEPT_COMMENT)
						) )
		{
		  last_code_lex = last_code_lex->get_prev_lexem() ;
		}
	  // FIN ZONE NON TRAITEE


#ifdef DUMP_LEXEMS
	  fprintf(stdout, "\n\n--- DEBUT DUMP DES LEXEMES AVANT EXPANSION ---\n") ;
	  first_lexem->dump_lexems() ;
	  TRACE3("last_lexem = 0x%08x %s next %x",
			 lex_get_last_lexem(),
			 lex_get_last_lexem()->get_lex_name(),
			 lex_get_last_lexem()->get_next_lexem()) ;
	  fprintf(stdout, "\n---  FIN  DUMP DES LEXEMES AVANT EXPANSION ---\n") ;
#endif

	  // Sauvegarde du contexte qui peut etre ecrase en cas
	  // de fichier de definitions
	  TRACE0("sauvegarde du contexte") ;
	  void *save_stack = lex_get_MD5_key_stack() ;
	  void *save_expanded_stack = lex_get_expanded_MD5_key_stack() ;

	  // parsing des definitions
	  T_definition *first_def = NULL ;
	  T_definition *last_def = NULL ;
	  T_flag *def_clause = NULL ;
	  T_file_definition *first_file = NULL ;
	  T_file_definition *last_file = NULL ;
	  parse_DEFINITIONS(first_lexem,
                            converterName,
						&first_def,
						&last_def,
						&def_clause,
						&first_file,
						&last_file,
						last_code_lex) ;

	  // Obtention de l'ordre de dependance des definitions
	  order_DEFINITIONS(&first_def, &last_def) ;

	  // On empile les definitions
	  push_first_def_stack(first_def) ;
	  push_last_def_stack(last_def) ;
	  push_def_clause_stack(def_clause) ;
	  push_first_file_def_stack(first_file) ;
	  push_last_file_def_stack(last_file) ;

	  // expansion des definitions dans les def
	  set_expand_state(TRUE) ;
	  while (get_expand_state() == TRUE)
		{
		  // A FAIRE : ameliorer, car l'etat est global donc si
		  // un composant a une definition expansee, on les reparcourt
		  // tous
		  expand_DEFINITIONS_in_definitions(first_def, last_def) ;
		}

	  // expansion des definitions dans le source
	  set_expand_state(TRUE) ;
	  while (get_expand_state() == TRUE)
		{
		  // A FAIRE : ameliorer, car l'etat est global donc si
		  // un composant a une definition expansee, on les reparcourt
		  // tous
		  expand_DEFINITIONS_in_source(&first_lexem,
									   &last_lexem,
									   first_def,
									   last_def) ;
		}

#ifdef DUMP_LEXEMS
	  fprintf(stdout, "\n\n--- DEBUT DUMP DES LEXEMES APRES EXPANSION ---\n") ;
	  first_lexem->dump_lexems() ;
	  fprintf(stdout, "\n---  FIN  DUMP DES LEXEMES APRES EXPANSION ---\n") ;
#endif


	  // Deuxieme estimation du dernier lexeme de code du fichier,
	  // clause DEFINITIONS expansee
	  TRACE1("last_lexemp_sop = %x", last_lexem) ;

	  T_lexem *cur = last_lexem ;

	  while (    (cur != NULL)
				 && (cur->is_a_comment() == TRUE) )
		{
		  cur = cur->get_real_prev_lexem() ;
		}

	  last_code_lex = cur ;
	  push_last_code_lexem_stack(last_code_lex) ;

#ifdef DUMP_LEXEMS
	  fprintf(stdout, "\n\n--- DEBUT DUMP DES LEXEMES APRES EXPANSION ---\n") ;
	  lex_get_first_lexem()->dump_lexems() ;
	  fprintf(stdout, "\n---  FIN  DUMP DES LEXEMES APRES EXPANSION ---\n") ;
#endif


	  // Restauration du contexte qui a pu etre ecrase en cas
	  // de fichier de definitions
	  lex_set_MD5_key_stack(save_stack) ;
	  lex_set_expanded_MD5_key_stack(save_expanded_stack) ;

	  // Calcul du checksum etendu
	  lex_push_expanded_MD5_key_stack(compute_expanded_checksum(first_lexem)) ;
	  if (get_dep_mode() == TRUE)
		{
		  // Dans le mode d'analyse des dependances, on
		  // coupe dans le flux de lexemes tout ce qui n'est pas
		  // relatif aux dependances
		  shrink_for_dep(first_lexem) ;
#ifdef DUMP_LEXEMS
		  fprintf(stdout, "\n\n--- DEBUT DUMP DES LEXEMES APRES SHRINK ---\n") ;
		  first_lexem->dump_lexems() ;
		  fprintf(stdout, "\n---  FIN  DUMP DES LEXEMES APRES SHRINK ---\n") ;
#endif
		}

#ifdef MATRA
	  //Ajout CT le 23.07.97
	  //Correction de l'anomalie MATRA001 : max{a,b} remplace par max({a,b})
	  find_builtin_and_add_L_PAREN(first_lexem) ;
#endif // MATRA

	  // Il faut transformer les ':' en '\:' dans les rec et struct
	  // A FAIRE : optimiser en memorisant s'il y a eu un rec ou un struct
	  // A FAIRE : mieux, se rappeler du premier et du dernier pour ne
	  // traiter que les lexemes entre les deux
	  fix_label_col(first_lexem) ;

	  TRACE0("<< BOUCLE ANALYSE LEXICALE") ;
	  ENTER_TRACE ;
	}

  // On restaure la pile des lexems a partir de sa copie
  lex_copy_tmp_first_lex_stack() ;
  lex_copy_tmp_last_lex_stack() ;

  ucnv_close(uconv) ;

  //
  // FIN DU TRAITEMENT SUR TOUS LES FLUX DE LEXEMES
  //

  if (last_compo_code_lexem == NULL)
	{
	  toplevel_error(FATAL_ERROR, get_error_msg(E_EMPTY_FILE)) ;
	}

  EXIT_TRACE ;
  TRACE2("<< lex_analysis(%s, %s)",
		 file_name_acp,
		 (second_file_name_acp == NULL) ? "" : second_file_name_acp) ;

  return TRUE ;
}

//
// Fin du fichier
//
