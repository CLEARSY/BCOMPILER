/******************************* CLEARSY **************************************
* Fichier : $Id: c_api.cpp,v 2.0 2007-12-03 14:18:08 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la bibliotheque
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
RCS_ID("$Id: c_api.cpp,v 1.92 2007-12-03 14:18:08 arequet Exp $") ;

/* Includes systeme */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_chain2.h"
#include "s_lr.h"
#include "i_toplvl.h"

// Version de la bibliotheque
#include "version.h"

// Forwards
// Analyse syntaxique du niveau "composant" (-> v_compo.cpp)
extern T_betree *component_syntax_analysis(T_betree *betree) ;
extern void lex_unload(void);

// Obtention de la version de la bibliotheque
const char *get_bcomp_version(void)
{
  return BCOMP_VERSION ;
}

// Initialisation du compilateur
static int init_compiler_done_si = FALSE ;

int get_init_compiler_done(void)
{
#ifdef FULL_TRACE
  TRACE1("get_init_compiler_done -> %s",
		 (init_compiler_done_si == TRUE) ? "TRUE": "FALSE") ;
#endif
  return init_compiler_done_si ;
}

// Forcer un reset du compilateur (fonction cache, pour MEMORY_CLEANUP)
void reset_compiler(void)
{
  TRACE0("reset_compiler") ;

  //reset_object_manager() ; // Non, fait au debut de memory_cleanup()
  //reset_symbol_table() ; // Non, persistante
  reset_LR_stacks() ;
  reset_error_count() ;
  reset_betree_manager() ;
  reset_decompiler() ;
  //  reset_user_error_msg() ; NON ils sont conserves
  //  reset_user_warning_msg() ; idem user_error_msg
  // reset_msg_line_manager() ; NON, ILS SONT PERSISTANTS POUR LE MS
  lex_reset_path() ;
}

void init_compiler(void)
{
  if (init_compiler_done_si == TRUE)
	{
	  // Deja fait
	  TRACE0("pas init_compiler (deja fait)") ;
	  return ;
	}


  TRACE0("init_compiler") ;

  init_user_create_methods() ;
  init_metaclasses() ;

  if (get_object_manager() == NULL)
	{
	  TRACE0("creation du gestionnaire d'objets") ;
	  new_object_manager() ;
	}

  // Non ! necessite la connaissance du fichier de ressources, non
  // disponible dans le cas general lors de l'appel de init_compiler
  // (i.e. au debut du main)
  //init_visibility_tables() ;

  set_inside_values(FALSE) ;

  init_symbol_table() ;
  set_solve_comment(TRUE) ;

  if (get_tool_name() == NULL)
	{
	  set_tool_name("bcomp") ;
	}

  // Initialisation de la gestion du param�trage
  init_env() ;

  // Initialisation pour BOM
  literal_integer_type = NULL ;
  literal_integer_type_checked = FALSE ;

  // C'est fini !
  init_compiler_done_si = TRUE ;
}

// Liberation du compilateur
void unlink_compiler(void)
{
  TRACE0("unlink_compiler") ;

  if (init_compiler_done_si == FALSE)
    {
      return ;
    }

  unlink_metaclasses() ;

  // C'est fini !
  init_compiler_done_si = FALSE ;
}

// ajout jfm pour correction bug 2221
// Fonction qui fait la verifications syntaxiques
// 1 - d'utilisation de $0
static void syntax_check(void)
{
  TRACE0("syntax_check");

  T_betree *cur_betree = get_betree_manager()->get_betrees() ;

  while (cur_betree != NULL)
	{
	  // Verifications syntaxiques d'utilisation de $0
	  syntaxic_check_dollar_zero(cur_betree) ;

	  cur_betree = cur_betree->get_next_betree() ;
	}
  TRACE0("end syntax_check");
}

// fin ajout jfm pour correction bug 2221

// Fonction qui lit un fichier source, fait l'analyse syntaxique et
// rend un Betree
// second_input_file = 2e essai pour trouver le nom du composant
// (ex: REFINES MM, input_file = MM.mch, second_input_file = MM.ref)
T_betree *internal_syntax_analysis(int dep_mode,
											const char *input_file,
                                                                                        const char *converterName,
                                            const char *second_input_file = NULL,
                                            const char *third_input_file = NULL,
                                            T_lexem *load_lexem = NULL,
                                            int complain_if_unreadable = TRUE)
{
  TRACE2("internal_syntax_analysis(%s, %s)",
		 input_file,
		 (second_input_file == NULL) ? "(null)" : second_input_file) ;

  init_compiler() ;
  set_dep_mode(dep_mode) ;
  verbose_describe(get_catalog((dep_mode == TRUE)
							   ? C_DEP_ANALYSIS : C_SYNTAX_ANALYSIS),
				   input_file) ;

  // Analyse lexicale
  TRACE0("ANALYSE LEXICALE (debut)") ; ENTER_TRACE ;
  get_object_manager()->reset_lexems_list() ;

  // Dernier Betree charge avant les Betree courants
  T_betree *before_last_betree = get_betree_manager()->get_last_betree() ;

  // On recupere une pile de flux de lexemes
  int status = lex_analysis(input_file,
                            converterName,
							second_input_file,
							third_input_file,
							load_lexem,
							NULL, // MD5_Ctx
							complain_if_unreadable) ;
  EXIT_TRACE ; TRACE0("ANALYSE LEXICALE (fin)") ;

  if ( (status == FALSE) && (complain_if_unreadable == FALSE) )
	{
	  // Le client a explicitement demande de ne pas crier
	  return NULL ;
	}
  if (get_dep_mode() == FALSE)
	{
	  // Reset de la liste des identificateurs a verifier
	  reset_chain2(NULL) ;
	}

  // Analyse syntaxique
  input_file = get_file_name() ;
  TRACE1("ANALYSE SYNTAXIQUE de <%s> (debut)", input_file) ; ENTER_TRACE ;

#ifdef TRACE
  int cpt = 0 ;
#endif

  // Cas d'erreur dans le premier composant
  if (    (is_first_lex_stack_empty() == TRUE)
	   && (get_first_compo_code_lexem() != NULL) )
	{
	  syntax_error(get_first_compo_code_lexem(),
				   FATAL_ERROR,
				   get_error_msg(E_INVALID_COMPONENT_TYPE),
				   get_first_compo_code_lexem()->get_lex_ascii()) ;
	}

  while (is_first_lex_stack_empty() == FALSE)
	{
	  TRACE1("DEBUT ANALYSE SYNTAXIQUE COMPOSANT %d", ++cpt) ;
	  ENTER_TRACE ;
	  pop_first_lex_stack() ;
	  pop_last_lex_stack() ;
	  pop_last_code_lexem_stack() ;

	  T_lexem *first_lexem = lex_get_first_lexem() ;
	  syntax_analysis(first_lexem, input_file) ;
	  TRACE1("FIN ANALYSE SYNTAXIQUE COMPOSANT %d", cpt) ;
	  EXIT_TRACE ;
	}

  // On regarde s'il n'y a pas des composants invalide commencant par
  // autre chose que MACHINE, RAFINEMENT ou IMPLEMENTATION. Ces
  // composants n'ont pas de flux de lexeme propre, leurs lexemes sont
  // chaines en fin d'autres composants
  for (T_betree *cur = get_betree_manager()->get_betrees(); cur; cur = cur->get_next_betree())
	{
	  T_machine *cur_mach = cur->get_root() ;
	  T_flag *end = cur_mach->get_end_machine() ;

      if (!end) continue;

	  TRACE4("ici : end %x %s:%d:%d",
			 end->get_ref_lexem(),
			 end->get_ref_lexem()->get_file_name()->get_value(),
			 end->get_ref_lexem()->get_file_line(),
			 end->get_ref_lexem()->get_file_column()) ;

	  if (end->get_ref_lexem()->get_next_lexem() != NULL)
		{
		  TRACE4("ici : end->next %x %s:%d:%d",
				 end->get_ref_lexem()->get_next_lexem(),
				 end->get_ref_lexem()->get_next_lexem()->get_file_name()->get_value(),
				 end->get_ref_lexem()->get_next_lexem()->get_file_line(),
				 end->get_ref_lexem()->get_next_lexem()->get_file_column()) ;
		}

      if (end->get_ref_lexem()->get_next_lexem() != NULL)
		{
		  syntax_error(end->get_ref_lexem()->get_next_lexem(),
					   FATAL_ERROR,
					   get_error_msg(E_INVALID_COMPONENT_TYPE),
					   end->get_ref_lexem()->get_next_lexem()->get_lex_ascii()) ;
		}
	}

  if (dep_mode == FALSE)
	{
	  // Il faut accrocher les derniers commentaires a la machine
	  T_lexem *cur = lex_get_last_lexem() ;

	  if (cur->is_a_comment() == TRUE)
		{
		  T_betree *last_betree = get_betree_manager()->get_last_betree() ;
		  T_machine *last_machine = last_betree->get_root() ;
		  T_item **adr_first_comment = (T_item **)
			last_machine->get_adr_first_end_comment();
		  T_item **adr_last_comment = (T_item **)
			last_machine->get_adr_last_end_comment() ;

          while (   cur->get_real_prev_lexem() != NULL
                 && cur->get_real_prev_lexem()->is_a_comment() == TRUE)
            {
              cur = cur->get_real_prev_lexem() ;
            }

          while (cur != NULL)
			{
			  T_comment *comm = new T_comment(cur->get_value(),
				  cur->is_a_kept_comment(),
											  TRUE, // on veut analyser les pragmas
											  last_machine,
											  NULL,
											  NULL,
											  last_machine,
											  last_betree,
											  cur) ;

              comm->tail_insert(adr_first_comment, adr_last_comment) ;
			  get_object_manager()->set_solved_comment(cur) ;
              cur = cur->get_real_next_lexem() ;
			}

		}
	}

  // On accroche les cles MD5, recuperee dans la pile
  // Il faut une LIFO car les checksums sont recuperes en parcourant
  // la liste des Betrees a l'envers
  T_betree *cur_betree = get_betree_manager()->get_last_betree() ;

  while (cur_betree != before_last_betree)
	{
	  TRACE4("suite boucle cur_betree %x:%s before_last_betree %x %s",
			 cur_betree,
			 (cur_betree == NULL)
			 ? "null" : cur_betree->get_file_name()->get_value(),
			 before_last_betree,
			 (before_last_betree == NULL)
			 ? "null" : before_last_betree->get_file_name()->get_value()) ;
	  cur_betree->checksum = lex_pop_MD5_key_stack() ;
	  cur_betree->expanded_checksum = lex_pop_expanded_MD5_key_stack() ;
	  TRACE4("les checksum de %x %s sont %x (raw) et %x (expanded)",
			 cur_betree,
			 cur_betree->get_betree_name()->get_value(),
			 cur_betree->checksum,
			 cur_betree->expanded_checksum) ;

	  cur_betree = cur_betree->get_prev_betree() ;
	}

  EXIT_TRACE ; TRACE0("ANALYSE SYNTAXIQUE (fin)") ;

  TRACE3("--> DEBUT DUMP TOUS BETREE first %x last %x before_last %x",
		 get_betree_manager()->get_betrees(),
		 get_betree_manager()->get_last_betree(),
		 before_last_betree) ;
  ENTER_TRACE ;
  cur_betree = get_betree_manager()->get_betrees() ;
  while (cur_betree != NULL)
	{
	  TRACE4("%x:%s prev %x next %x",
			 cur_betree,
			 cur_betree->get_betree_name()->get_value(),
			 cur_betree->get_prev_betree(),
			 cur_betree->get_next_betree()) ;
	  cur_betree = cur_betree->get_next_betree() ;
	}
  EXIT_TRACE ;
  TRACE0("<-- FIN DUMP TOUS BETREE") ;

  T_betree *betree = get_betree_manager()->get_last_betree() ;
  while (betree != before_last_betree)
	{
	  betree->path = (lex_get_path() == NULL)
		? (T_symbol *)NULL : lookup_symbol(lex_get_path()) ;
	  betree = betree->get_prev_betree() ;
	}

  if (dep_mode == FALSE)
	{
	  syntax_check();
	}

  // Passe ou on met a jour les liens "betree" de toutes les
  // definitions de tous les betree. Ils n'ont pas ete mis a jour lors
  // de la construction car les definitions sont crees au cours de
  // l'analyse lexicale, avant la creation du betree correspodant.
  // en MULTI_COMPO on a deja defini cette variable
  cur_betree = get_betree_manager()->get_betrees() ;

  while (cur_betree != NULL)
	{
	  // On accroche les definitions au Betree
	  T_item *cur_def = cur_betree->get_definitions() ;

	  while (cur_def != NULL)
		{
		  cur_def->set_father(cur_betree) ;
		  cur_def->set_betree(cur_betree) ;
		  cur_def = cur_def->get_next() ;
		}

	  cur_betree = cur_betree->get_next_betree() ;
	}

  // Liberation des elements de l'analyse lexicale
  // Au passage, calcul des checksums de la machine
  betree = get_betree_manager()->get_last_betree() ;

  while (betree != before_last_betree)
	{
	  betree->set_status((dep_mode == TRUE) ? BST_DEPENDENCIES : BST_SYNTAXIC) ;
	  if (dep_mode == FALSE)
		{
		  component_syntax_analysis(betree) ;
		  betree->get_root()->compute_checksums() ;
		}
	  betree = betree->get_prev_betree() ;
	}

  // En mode multi-compo on verifie en plus l'integrite du fichier source
  // i.e. que les composants qui sont la ont bien le droit d'y etre !
  if (get_error_count() == 0)
	{
	  get_betree_manager()->check_file_integrity(input_file) ;
	}

  lex_delete_MD5_key_stack() ;
  lex_delete_expanded_MD5_key_stack() ;
  delete_first_lex_stack() ;
  delete_last_lex_stack() ;
  delete_last_code_lexem_stack() ;
  delete_tmp_first_lex_stack() ;
  delete_tmp_last_lex_stack() ;
  delete_first_def_stack() ;
  delete_last_def_stack() ;
  delete_first_file_def_stack() ;
  delete_last_file_def_stack() ;
  delete_def_clause_stack() ;

  TRACE2("fin de compiler_syntax_analysis(%s) -> %x", input_file, betree) ;
  return get_betree_manager()->get_betrees() ;
}

// Fonction qui lit un fichier source, fait l'analyse syntaxique et
// rend un Betree
// second_input_file = 2e essai pour trouver le nom du composant
// (ex: REFINES MM, input_file = MM.mch, second_input_file = MM.ref)
T_betree *compiler_syntax_analysis(const char *input_file,
                                            const char *converterName,
											const char *second_input_file,// = NULL
											const char *third_input_file,// = NULL
											T_lexem *load_lexem)// = NULL)
{
  TRACE2("compiler_syntax_analysis(%s, %s)",
		 input_file,
		 (second_input_file == NULL) ? "(null)" : second_input_file) ;

  if (get_no_exit_mode() == TRUE)
	{
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans compiler_syntax_analysis") ;
		  TRACE0("debut menage") ;
		  lex_unload() ;
		  lex_unlink_lex_stacks() ;
		  lex_unlink_def_stacks() ;
		  TRACE0("fin menage") ;
		  return NULL ;
		}

	  set_exit_handler(&env) ;
	}

  force_full_lexems_reset() ;

  T_betree *res = internal_syntax_analysis(FALSE,
										   input_file,
                                                                                   converterName,
										   second_input_file,
										   third_input_file,
										   load_lexem) ;

#ifdef FIX_MAPLETS
  TRACE0(">> fix_maplets") ;
  ENTER_TRACE ;

  // Correction des maplets
  T_object_tag *cur_tag = get_object_manager()->get_tags() ;

  while (cur_tag != NULL)
	{
	  // Cast justifie si necessaire par test du node_type
	  T_binary_op *cur_object = (T_binary_op *)cur_tag->get_object() ;
	  if (    (cur_object->get_node_type() == NODE_BINARY_OP)
		   && (cur_object->get_operator() == BOP_COMMA) )
		{
		  // C'est a priori une virgule qui est un maplet, sauf dans
		  // le cas particulier des records. Dans ce cas,
		  // extract_record_items a ete appele et le signe distinctif
		  // est que op1 = op2 = NULL
		  if (    (cur_object->get_op1() == NULL)
			   && (cur_object->get_op2() == NULL) )
			{
			  // Cas d'un struct ou un record : on ne fait rien
			}
		  else
			{
			  // C'est une virgule qui est maplet

			  // On corrige le Betree
			  cur_object->set_operator(BOP_MAPLET) ;

			  // On corrige le flux de lexemes pour le decompilateur
			  cur_object->get_ref_lexem()->set_lex_type(L_MAPLET) ;

			  // On previent l'utilisateur
			  syntax_warning(cur_object->get_ref_lexem(),
							 EXTENDED_WARNING,
							 get_warning_msg(W_COMMA_WILL_BE_TREATED_AS_A_MAPLET)) ;
			}
		}

	  cur_tag = cur_tag->get_next_tag() ;
	}

  EXIT_TRACE ;
  TRACE0("<< fix_maplets") ;
#endif // FIX_MAPLETS

  // Liberation des piles de l'analyseur lexical
  lex_unlink_lex_stacks() ;
  lex_unlink_def_stacks() ;

  if (get_no_exit_mode() == TRUE)
	{
	  set_exit_handler(NULL) ;
	}
  return res ;
}

// my_fgets copie de CTOOLS/MS/MsFile.c
// getlines realloc la line au besoin ce qui plante le garbage collector
char *my_fgets(char **line, int length, FILE *file)
{
  /* taille de la ligne lue */
  int len = 0;

  /*TRACE3("my_fgets(%d, %x)", *line, length, file);*/

  if (fgets(*line, length, file) != NULL)
    {
      /* taille de la ligne */
      len = strlen(*line);

      /* on verifie qu'on a bien lu toute la ligne et eventuellement on
	 lit ce qui manque */
      while((*line)[len - 1] != '\n')
	{
	  *line = (char *)s_realloc(*line, len + length);
	  fgets(&((*line)[len]), length, file);
	  len = strlen(*line);
	}
      /*TRACE1("my_fgets -> %s", *line);*/
      return *line;
    }
  else
    {
      return NULL;
    }
}

char *calc_md5_file(const char *filename, char *MD5_key)
{
  FILE *f;
  unsigned char buf[8];
  size_t read;

  TRACE1("calc_md5_file(%s)", filename);

  // Initialisation du contexte
  MD5Context ctx ;
  MD5Init(&ctx) ;

 // open file
  f = fopen(filename, "r");
  if (f == NULL) {
    TRACE1("calc_md5_file: unable to open %s", filename);
    snprintf(MD5_key, 33, "00000000000000000000000000000000");
    return MD5_key;
  }

  // parse le fichier
  while ((read = fread(buf, sizeof(char), 8, f)) > 0) {
    MD5Update(&ctx, buf, read);
  }

  // calcul du md5 du fichier
  unsigned char MD5buf[17] ;
  MD5Final(MD5buf, &ctx) ;
  sprintf(MD5_key,
	  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	  MD5buf[0], MD5buf[1], MD5buf[2], MD5buf[3],
	  MD5buf[4], MD5buf[5], MD5buf[6], MD5buf[7],
	  MD5buf[8], MD5buf[9], MD5buf[10], MD5buf[11],
	  MD5buf[12], MD5buf[13], MD5buf[14], MD5buf[15]) ;

  fclose(f);
  TRACE1("calc_md5_file: MD5_key: %s\n", filename);
  return MD5_key;
}


// Fonction qui lit un fichier source, fait l'analyse syntaxique et
// rend un Betree
// second_input_file = 2e essai pour trouver le nom du composant
// (ex: REFINES MM, input_file = MM.mch, second_input_file = MM.ref)
T_betree *compiler_dep_analysis(const char *input_file,
                                         const char *converterName,
										 int complain_if_unreadable)
{
  char MD5_key[33];
  int l_expanded_input_file;
  const char * c_input_file;	// clean_input_file: input_file sans dossier
  T_betree *cur_res;

  TRACE2("compiler_dep_analysis(%s) complain_if_unreadable %s",
		 input_file,
		 (complain_if_unreadable == TRUE) ? "TRUE" : "FALSE") ;
  if (get_no_exit_mode() == TRUE)
	{
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans compiler_syntax_analysis") ;
		  return NULL ;
		}

	  set_exit_handler(&env) ;
	}

  force_full_lexems_reset() ;
  T_betree *res = internal_syntax_analysis(TRUE,
										   input_file,
                                                                                   converterName,
										   NULL,
										   NULL,
										   NULL, // load_lexem
										   complain_if_unreadable) ;

  c_input_file = bcomp_last_slash(const_cast<char*>(input_file));
  if (c_input_file == NULL)
    c_input_file = input_file;
  else
    c_input_file = &c_input_file[1];

  int is_exp_needed = is_expand_dep_needed(input_file);

  cur_res = res;

  if (is_exp_needed == TRUE &&
  	  get_project_manager() != NULL &&
  	  get_project_manager()->get_current() != NULL) { // get_project_manager()->get_current() == NULL dans XREF...
    while(cur_res != NULL) {
      // absolute path du .dep
      const char * bdp = get_project_manager()->get_current()->get_bdp_dir()->get_value();
      int l_dep_file = strlen(bdp) + 1 + strlen(cur_res->get_root()->get_machine_name()->get_name()->get_value()) + 5; // */*.dep\0
      char *dep_file = (char *)s_malloc(l_dep_file);
      snprintf(dep_file, l_dep_file, "%s/%s.dep", bdp, cur_res->get_root()->get_machine_name()->get_name()->get_value());
      FILE * f_dep_file;

      // Ecriture du fichier .dep
      f_dep_file = fopen(dep_file, "w");

      if (f_dep_file != NULL) {
	fprintf(f_dep_file, "file:%s\n", calc_md5_file(c_input_file, MD5_key) );

	// d�finitions
	if (cur_res->get_file_definitions() != NULL)
	  {
	    T_file_definition * cur = cur_res->get_file_definitions();
	    do
	      {
		fprintf(f_dep_file, "dep:%s:%s%s%s\n",
			calc_md5_file(cur->get_name()->get_value(), MD5_key),
			(cur->get_path() == NULL) ? "" : cur->get_path()->get_value(),
			(cur->get_path() == NULL) ? "" : "/",
			cur->get_name()->get_value());
	      } while ((cur = (T_file_definition *)cur->get_next()) != NULL);
	  }

	// d�pendances
	if (cur_res->get_root() != NULL) {

	  // abstraction
	  if (cur_res->get_root()->get_abstraction_name() != NULL) {
	    fprintf(f_dep_file, "abst:%s\n", cur_res->get_root()->get_abstraction_name()->get_name()->get_value());
	  }

	  // include
	  T_used_machine *inc = cur_res->get_root()->get_includes();
	  while (inc != NULL) {
	    if (inc->get_component_name() != NULL) {
	      fprintf(f_dep_file, "incl:%s",
		      inc->get_component_name()->get_value());
	      if (inc->get_instance_name() != NULL) {
		fprintf(f_dep_file, ":%s",
			inc->get_instance_name()->get_value());
	      }
	      fprintf(f_dep_file, "\n");
	    }
	    inc = (T_used_machine *)inc->get_next();
	  }

	  // sees
	  T_used_machine *sees = cur_res->get_root()->get_sees();
	  while (sees != NULL) {
	    if (sees->get_component_name() != NULL) {
	      fprintf(f_dep_file, "sees:%s",
		      sees->get_component_name()->get_value());
	      if (sees->get_instance_name() != NULL) {
		fprintf(f_dep_file, ":%s",
			sees->get_instance_name()->get_value());
	      }
	    }
	    fprintf(f_dep_file, "\n");
	    sees = (T_used_machine *)sees->get_next();
	  }

	  // extends
	  T_used_machine *extends = cur_res->get_root()->get_extends();
	  while (extends != NULL) {
	    if (extends->get_component_name() != NULL) {
	      fprintf(f_dep_file, "exte:%s",
		      extends->get_component_name()->get_value());
	      if (extends->get_instance_name() != NULL) {
		fprintf(f_dep_file, ":%s",
			extends->get_instance_name()->get_value());
	      }
	    }
	    fprintf(f_dep_file, "\n");
	    extends = (T_used_machine *)extends->get_next();
	  }

	  // import
	  T_used_machine *import = cur_res->get_root()->get_imports();
	  while (import != NULL) {
	    if (import->get_component_name() != NULL) {
	      fprintf(f_dep_file, "impo:%s",
		      import->get_component_name()->get_value());
	      if (import->get_instance_name() != NULL) {
		fprintf(f_dep_file, ":%s",
			import->get_instance_name()->get_value());
	      }
	    }
	    fprintf(f_dep_file, "\n");
	    import = (T_used_machine *)import->get_next();
	  }

	  // uses
	  T_used_machine *uses = cur_res->get_root()->get_uses();
	  while (uses != NULL) {
	    if (uses->get_component_name() != NULL) {
	      fprintf(f_dep_file, "uses:%s",
		      uses->get_component_name()->get_value());
	      if (uses->get_instance_name() != NULL) {
		fprintf(f_dep_file, ":%s",
			uses->get_instance_name()->get_value());
	      }
	    }
	    fprintf(f_dep_file, "\n");
	    uses = (T_used_machine *)uses->get_next();
	  }

	}
	fclose(f_dep_file);
      } else {
	TRACE2("compiler_dep_analysis: impossible d'�crire %s/%s\n", bdp, dep_file);
      }

      s_free(dep_file);

      if (strcmp(strrchr(c_input_file, '.'), ".mod") == 0) {
	cur_res = cur_res->get_next_betree();
      } else {
	cur_res = NULL;
      }
    } // while(cur_res != NULL)
  } // if (is_exp_needed == TRUE)

  // Liberation des piles de l'analyseur lexical
  lex_unlink_lex_stacks() ;
  lex_unlink_def_stacks() ;

  if (get_no_exit_mode() == TRUE)
	{
	  set_exit_handler(NULL) ;
	}

  return res ;
}

int is_expand_dep_needed (const char *input_file)
{
	char MD5_key[33];
	char * line;
	size_t read;
	int max_line_length = 240;
	int l_expanded_input_file;
    char * c_input_file;	// clean_input_file: input_file sans dossier
	char *path_input_file;
	char *fullname;
	int l_fullname;
	char *oldmd5;
	char *olddep_md5;
	char *olddep_name;
	char *cur;
	int identique = 0;
	char *toc;

	TRACE1("is_expand_dep_needed(%s)", input_file);

	// path ou se trouve le src
	path_input_file = (char *)s_malloc(strlen(input_file) + 1);
	sprintf(path_input_file, "%s", input_file);
    c_input_file = bcomp_last_slash(path_input_file);

	if (c_input_file == NULL) {
		c_input_file = path_input_file;
		// TODO : Bug probable ici (pmauduit, 02/10/2008)
		//   s_free(path_input_file);
		//   path_input_file = NULL;
    } else {
        c_input_file[0] = '\0';
        c_input_file = &c_input_file[1];
    }

	TRACE1("is_expand_dep_needed: c_input_file=%s", c_input_file);

	// absolute path du .dep
	TRACE0("is_expand_dep_needed 0");
	if ((get_project_manager() == NULL)
			|| (get_project_manager()->get_current() == NULL)) { // get_project_manager()->get_current() == NULL dans XREF...
		if (path_input_file != NULL)
			s_free(path_input_file);
		return TRUE;
	}

	const char * bdp = get_project_manager()->get_current()->get_bdp_dir()->get_value();
	int l_dep_file = strlen(bdp) + 1 + strlen(c_input_file) + 1; // */*\0
	char *dep_file = (char *)s_malloc(l_dep_file);
	snprintf(dep_file, l_dep_file, "%s/%s", bdp, c_input_file);
	snprintf(&dep_file[l_dep_file - 4], 4, "dep");

	// ouverture du .dep
	FILE * f_dep_file;
	f_dep_file = fopen(dep_file, "r");
	TRACE0("is_expand_dep_needed 6");
	s_free(dep_file);

	line = (char *)s_malloc(max_line_length);

	TRACE0("is_expand_dep_needed: is f_dep_file est null?");

	if (f_dep_file != NULL) {

		// Check md5 de inputfile
		char *res = my_fgets(&line, max_line_length, f_dep_file);

		if (res != NULL) {
			toc = strtok(line, ":\n");
		} else {
			toc = NULL;
		}

		if (toc != NULL) {
			if (strcmp(toc, "file") != 0)
			{
				identique = 1;
			}
			else
			{
				//AR: Inutile, puisque identique==1 (?)
				oldmd5 = strtok(NULL, ":\n");

				calc_md5_file(input_file, MD5_key);

				if (!oldmd5 || strcmp(oldmd5, MD5_key) != 0)
				{
					identique = 1;
				}
			}
		} else {
			identique = 1;
		}

		// check md5 des dep files
		while (my_fgets(&line, max_line_length, f_dep_file) != NULL && identique == 0) {
			toc = strtok(line, ":\n");
			if (toc == NULL)
				continue;
			if (strcmp(toc, "dep") == 0)
			{
				olddep_md5 = strtok(NULL, ":\n");
				olddep_name = strtok(NULL, ":\n");
				l_fullname = (path_input_file == NULL ? 0 : strlen(path_input_file) + 1 ) + strlen(olddep_name) + 3;
				fullname = (char *)s_malloc(l_fullname);
				if (path_input_file == NULL)
					snprintf(fullname, l_fullname, "%s", olddep_name);
				else
					snprintf(fullname, l_fullname, "%s/%s", path_input_file, olddep_name);
				calc_md5_file(fullname, MD5_key);
				s_free(fullname);
				if (strcmp(olddep_md5, MD5_key) != 0 || strcmp("00000000000000000000000000000000", MD5_key) == 0) {
					identique = 1;
				}
			}
		}
		fclose(f_dep_file);

		TRACE0("is_expand_dep_needed, near end");


		// aucun md5 different, inutile de faire un comp dep analysis:
		if (identique == 0) {
			if (path_input_file != NULL)
				s_free(path_input_file);
			s_free(line);
			return FALSE;
		}
	}

	if (path_input_file != NULL)
		s_free(path_input_file);
	s_free(line);
	return TRUE;
}

//// Fonction qui sauvegarde un betree
//function void compiler_save_betree(T_betree *betree, const char *output_file)
//{
//  TRACE2("compiler_save_betree(%x, \"%s\")", betree, output_file) ;

//  verbose_describe(get_catalog(C_SAVING_BETREE), output_file) ;

//  TRACE0("SAUVEGARDE DISQUE (debut)") ; ENTER_TRACE ;
////  betree->save_object(output_file,
////					  (betree->get_status() == BST_SYNTAXIC)
////					  ? FILE_SYNTAXIC_BETREE
////					  : FILE_SEMANTIC_BETREE) ;
//  betree->save_checksums(output_file,
//                      (betree->get_status() == BST_SYNTAXIC)
//                      ? FILE_SYNTAXIC_BETREE
//                      : FILE_SEMANTIC_BETREE) ;
//  EXIT_TRACE ;
//  TRACE0("SAUVEGARDE DISQUE (fin)") ;
//}

//#if 0 // A VIRER OU PORTER AVEC LOAD_OBJECT

//// Fonction qui charge un betree a partir d'un nom de fichier
//T_betree *compiler_load_betree(T_betree_file_type file_type,
//										const char *input_file)
//{
//  TRACE2("compiler_load_betree(%d, \"%s\")", file_type, input_file) ;

//  init_compiler() ;

//  TRACE0("LECTURE DISQUE (debut)") ; ENTER_TRACE ;
//  T_betree *betree = read_betree(input_file, file_type) ;
//  EXIT_TRACE ; TRACE0("LECTURE DISQUE (fin)") ;

//  return betree ;
//}

//// Fonction qui charge un betree a partir d'un T_symbol
//T_betree *compiler_load_betree(T_betree_file_type file_type,
//										T_symbol *reference)
//{
//  TRACE3("compiler_load_betree(%d, %x \"%s\")",
//		 file_type,
//		 reference,
//		 reference->get_value()) ;

//  TRACE0("LECTURE DISQUE (debut)") ; ENTER_TRACE ;
//  char *input_file = get_1Ko_string() ;  // A FAIRE :: dynamique
//  sprintf(input_file, "%s.bin", reference->get_value()) ;

//  verbose_describe(get_catalog(C_LOADING_BETREE), input_file) ;

//  T_betree *betree = read_betree(input_file, file_type) ;
//  EXIT_TRACE ; TRACE0("LECTURE DISQUE (fin)") ;
//  return betree ;
//}

//// Fonction qui charge un betree a partir d'un T_used_machine
//T_betree *compiler_load_betree(T_betree_file_type file_type,
//										T_used_machine *reference)
//{
//  return compiler_load_betree(file_type, reference->get_name()->get_name()) ;
//}
//#endif


// Fonction qui dumpe un betree sous format HTML
void compiler_dump_HTML_betree(T_betree *betree)
{
  TRACE1("DEBUT compiler_dump_HTML_betree(%x)", betree) ;
  ENTER_TRACE ;

  // Initialisations
  chain_reset(betree) ; //Reset du BeTree

  verbose_describe(get_catalog(C_DUMPING_HTML_BETREE), get_output_path()) ;

#ifndef WIN32
  size_t len = get_output_path_len() + 20 ;
  char *buf = get_tmp_string(len) ;
  //				         1111111111
  //			  123  4567890123456789
  sprintf(buf, "cd %s && rm -f *.html", get_output_path()) ;
#else
  size_t len = get_output_path_len() + 10 ;
  char *buf = get_tmp_string(len) ;
  //            1234  56789
  sprintf(buf, "del %s*.html", get_output_path()) ;
#endif
  TRACE1("system(\"%s\")", buf) ;
  // On ne teste pas le retour car par exemple sous Windows95 on a une
  // erreur car del crie s'il n'y a pas de fichier
  // Politique revisable !
  verbose_describe(get_catalog(C_REMOVING_PREV_HTML)) ;
  system(buf) ;

  // Dump HTML de l'object manager
  get_object_manager()->dump_html() ;

  // Dump HTML
  verbose_describe(get_catalog(C_CREATING_HTML)) ;
  TRACE1("ici betree %x", betree) ;
  betree->dump_html() ;

#ifndef WIN32
  // Fabrication du lien
  char *buf2 = get_tmp_string(46 + get_output_path_len() + ADR_SIZE) ;
  sprintf(buf2,
		  //		            111111111122222222223  333333333444444
		  //		 123  456789012345678901234567890  123456789012345
                  "cd %s ; rm -f main.html ; ln -s %p.html main.html",
		  get_output_path(),
                  betree) ;

  TRACE1("system(%s)", buf2) ;
  system(buf2) ;
#else
  // Fabrication du lien
  // A FAIRE :: mieux !
  char *buf2 = get_tmp_string(21 + 2*get_output_path_len() + ADR_SIZE) ;
  sprintf(buf2,
		  //                    11  111111112
		  //       12345    678901  234567890
          "copy %s%p.html %smain.html",
		  get_output_path(),
		  (size_t)betree,
		  get_output_path()) ;

  TRACE1("system(%s)", buf2) ;
  system(buf2) ;
#endif


  EXIT_TRACE ;
  TRACE1("FIN   compiler_dump_HTML_betree(%x)", betree) ;
}

// Informations
EXTERN void betree_info(void)
{
  if (file_is_open() == FALSE)
	{
	  file_reset_indent(stdout) ;
	}

  file_fprintf_line("- B Compiler version %s", get_bcomp_version()) ;

  file_fprintf("The following flags have been used : ") ;
#ifdef TRACE
  file_fprintf("TRACE ") ;
#endif
#ifdef FULL_TRACE
  file_fprintf("FULL_TRACE ") ;
#endif
#ifdef NO_CHECKS
  file_fprintf("NO_CHECKS ") ;
#endif
#ifdef DEBUG_LINE
  file_fprintf("DEBUG_LINE ") ;
#endif
#ifdef MEMORY_CHECK
  file_fprintf("MEMORY_CHECK ") ;
#endif
#ifdef PATCH_CHECK
  file_fprintf("PATCH_CHECK ") ;
#endif
#ifdef STATS_ALLOC
  file_fprintf("STATS_ALLOC ") ;
#endif
  file_new_line() ;
#ifdef __GNUC__
  file_fprintf_line("Compiled with gcc version %s", __VERSION__) ;
#endif
#ifdef WIN32
  file_fprintf_line("Compiled for Windows 32 target") ;
#endif
#ifdef IPX
  file_fprintf_line("Compiled for IPX target") ;
#endif
#ifdef HPUX
  file_fprintf_line("Compiled for HP-UX target") ;
#endif

#ifdef SOLARIS_5_3
  file_fprintf_line("Compiled for Solaris 5.3 target") ;
#endif
#ifdef SOLARIS_5_4
  file_fprintf_line("Compiled for Solaris 5.4 target") ;
#endif
#ifdef SOLARIS_5_5
  file_fprintf_line("Compiled for Solaris 5.5 target") ;
#endif
#ifdef SOLARIS_5_5_1
  file_fprintf_line("Compiled for Solaris 5.5.1 target") ;
#endif
#ifdef SOLARIS_5_6
  file_fprintf_line("Compiled for Solaris 5.6 target") ;
#endif
#ifdef SOLARIS_5_7
  file_fprintf_line("Compiled for Solaris 7 target") ;
#endif
#ifdef SOLARIS_5_8
  file_fprintf_line("Compiled for Solaris 8 target") ;
#endif
#ifdef SOLARIS_5_10
  file_fprintf_line("Compiled for Solaris 10 target") ;
#endif 

#ifdef LINUX
  file_fprintf_line("Compiled for Linux target") ;
#if (CPU==PENTIUM)
  file_fprintf_line("(Pentium version)") ;
#endif // CPU
#endif // LINUX

#ifdef MAC_OS_X
  file_fprintf_line("Compiled for Mac OS X target") ;
#endif

#ifdef WIN32
  file_fprintf_line("Compiled for Windows target") ;
#endif

#ifdef PROTECTION
  file_fprintf_line("You must have a licence to use this application") ;
#endif
#ifdef EVOL_INCLUDES
  file_fprintf_line("This version supports the extended includes semantics") ;
#endif
#ifdef ENABLE_PAREN_SUBST
  file_fprintf_line("This version allows the use of extra '(' ')' around substitutions") ;
#endif
#ifdef ENABLE_PAREN_IDENT
  file_fprintf_line("This version allows the use of extra '(' ')' around identifiers") ;
#endif
#ifdef MATRA
  file_fprintf_line("This version corrects the problems with parenthesis in Matra sources (max{(1,2)}, ...)") ;
#endif
#ifdef AUTO_PAREN_DEF
  file_fprintf_line("This version automatically adds parenthesis around the rewrite rules of the definitions") ;
#endif
}

//
// Gestionnaire de sortie
//
// Pointeur de retour pour longjump
static jmp_buf *adr_env_sop = NULL ;

// Initialisation du gestionnaire
void set_exit_handler(jmp_buf *adr_env)
{
TRACE1("set_exit_handler(%x)", adr_env) ;
adr_env_sop = adr_env ;
}

jmp_buf *get_exit_handler(void)
{
  TRACE1("get_exit_handler()->%x", adr_env_sop) ;
  return adr_env_sop ;
}

// Mise en place ou sortie du mode "pas d'exit"
static int no_exit_mode_si = FALSE ;
void set_no_exit_mode(int new_no_exit_mode)
{
  no_exit_mode_si = new_no_exit_mode ;
}

int get_no_exit_mode(void)
{
  return no_exit_mode_si ;
}

// Mode ou les lexemes correspondant aux donnees incluses sont
// "corriges", i.e. si A include B alors les donnees heritees de B
// auront des lexemes correspondant a la clause INCLUDES ou EXTENDS de
// A qui a provoque l'heritage
// Valeur par defaut : FALSE
static int fix_inherited_lexems_mode = FALSE ;
int get_fix_inherited_lexems_mode(void)
{
  return fix_inherited_lexems_mode ;
}
void set_fix_inherited_lexems_mode(int new_mode)
{
  fix_inherited_lexems_mode = new_mode ;
}

// Fonction qui met a jour l'identificateur new_ident, herite
// depuis src_ident, et "a cause" de la clause au lexeme
// inherit_lexem, pour que le Betree soit exploitable par des outils
// comme le Xref : corrige les champs ref_lexem et ref_inherited
void fix_inherited(T_ident *new_ident,
							T_ident *src_ident,
							T_lexem *inherit_lexem)
{
  if (get_fix_inherited_lexems_mode() == TRUE)
	{

	  TRACE8("fix %s %x %s:%d:%d vers %s:%d:%d\n",
			 new_ident->get_name()->get_value(),
			 new_ident,
			 new_ident->get_ref_lexem()->get_file_name()->get_value(),
			 new_ident->get_ref_lexem()->get_file_line(),
			 new_ident->get_ref_lexem()->get_file_column(),
			 inherit_lexem->get_file_name()->get_value(),
			 inherit_lexem->get_file_line(),
			 inherit_lexem->get_file_column()) ;
	  new_ident->set_ref_lexem(inherit_lexem) ;
	  new_ident->set_ref_inherited(src_ident) ;
	}

  new_ident->set_inherited(TRUE) ;
}

//
//	}{ Fin du fichier
//
