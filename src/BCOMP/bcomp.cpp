/******************************* CLEARSY **************************************
* Fichier : $Id: bcomp.cpp,v 2.0 2008-02-28 11:10:35 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Front-end du compilateur B
*
* Compilations :	-DENABLE_PAREN_SUBST pour autoriser le parenthesage des
*					substitutions
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
RCS_ID("$Id: bcomp.cpp,v 1.59 2008-02-28 11:10:35 arequet Exp $") ;

// Includes systeme
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef JETON
#include <ddeLicence.h>
#endif

//Gestionnaire de ressources
#include <p_api.h>

// Includes Composant Local */
#include "c_api.h"

#include "c_auto_test.h"

#include "version.h"

#ifdef LICENCE_BALBULETTE

/**
 * La table utilisée pour décoder les caractères
 */
static const char *TABLEAU_NOMBRES = "nafxvjgtzr";

/**
 * Renvoie le chiffre correspondant au caractère passé en
 * paramètre
 *
 * @param c le caractère duquel nous voulons le chiffre
 * @return le chiffre correspondant, ou -1 en cas d'erreur
 */
static int convertit_chiffre(char c)
{
	int i;
	for(i=0; i<10; ++i)
	{
		if(TABLEAU_NOMBRES[i] == c)
		{
			return i;
		}
	}
	return -1;
}

/**
 * Renvoie la voyelle de contrôle du mois, ou 0xffff dans le cas ou
 * l'entrée est incorrecte (mois non-compris entre 1 et 12)
 *
 * @param mois le numéro du mois
 * @return la voyelle correspondante, ou 0xffff en cas d'entrée
 *   incorrecte
 */
static int lit_voyelle(int mois)
{
  switch(mois) {
  case 1:
  case 7:
    return 'A';
  case 2:
  case 8:
    return 'E';
  case 3:
  case 9:
    return 'I';
  case 4:
  case 10:
    return 'O';
  case 5:
  case 11:
    return 'U';
  case 6:
  case 12:
    return 'Y';
  default:
    return 0xffff;
  }
}

/**
 * Lit un nombre à deux chiffers encodé dans la chaine à l'offset
 * spécifié.
 *
 * @param str la chaine de caractères
 * @param offset l'offset du nombre dans la chaine
 * @return le chiffre décodé (entre 0 et 99) ou -1 en cas d'erreur
 */
static int lit_nombre_a_deux_chiffres(char *str, int offset)
{
	int tmp, resultat;

	resultat = convertit_chiffre(str[offset]);
	if(resultat == -1)
	{
		return -1;
	}

	tmp = convertit_chiffre(str[offset+1]);
	if(tmp == -1)
	{
		return -1;
	}

	resultat = resultat * 10 + tmp;

	return resultat;
}

/**
 * Lit l'année encodée à l'offset spécifié
 *
 * @param str la chaine de caractères contenant la licence
 * @param offset l'offset ou l'année doit être lue
 * @return l'année, ou -1 en cas d'erreur
 */
static int lit_annee(char *str, int offset)
{
  char premier = str[offset];
  char deuxieme = str[offset+1];
  int delta;

  if(premier < 'A' || premier > 'Z') {
    return -1;
  }

  // verifie coherence entre premier et deuxieme caractères
  delta = premier - 'A';
  if(('Z' - deuxieme) != delta) {
    return -1;
  }

  return 2006 + delta;
}

/**
 * Valeur renvoyée en cas d'erreur
 */
#define ERREUR_CONVERSION 1
/**
 * Valeur renvoyée en cas de succes
 */
#define CONVERSION_OK 0

/**
 * Offset du mois dans la licence
*/
#define OFFSET_MOIS 0
/**
 * Offset de la voyelle du mois dans la licence
 */
#define OFFSET_VOYELLE 2

/**
 * Offset de l'année dans la chaine de licence
 */
#define OFFSET_ANNEE 3

#define OFFSET_JOUR 5

#define TAILLE_LICENCE 7

/**
 * Convertit la chaine de licence en jour mois et année
 *
 * @param licence la chaine de charactères contenant la licence
 * @param jour pointeur ou placer le jour
 * @param mois pointeur ou le mois va être écrit
 * @param annee pointeur ou l'année va être écrite
 * @return zero en cas de succes, different de zero en cas d'erreur
 */
static int convertit_licence(char *licence, int *jour, int *mois, int *annee)
{
  int jour_decode, mois_decode, annee_decodee;

  if(licence == NULL || strlen(licence) != TAILLE_LICENCE) {
    return ERREUR_CONVERSION;
  }

  mois_decode = lit_nombre_a_deux_chiffres(licence, OFFSET_MOIS);

  // verifie la voyelle. Ce test gère aussi le cas ou le mois n'est
  // pas compris entre 1 et 12, car lit_voyelle
  if(lit_voyelle(mois_decode) != licence[OFFSET_VOYELLE]) {
    return ERREUR_CONVERSION;
  }


  // get day
  jour_decode = lit_nombre_a_deux_chiffres(licence, OFFSET_JOUR);
  if(jour_decode == -1 || jour_decode > 31) {
    return ERREUR_CONVERSION;
  }

  annee_decodee = lit_annee(licence, OFFSET_ANNEE);
  if(annee_decodee == -1) {
    return ERREUR_CONVERSION;
  }

  *annee = annee_decodee;
  *mois = mois_decode;
  *jour = jour_decode;

  return CONVERSION_OK;
}

/**
 * Lit la date contenue dans le fichier de licence, et place
 * le résultat aux destinations données en paramètres.
 *
 * Cette fonction arrête le programme si le fichier de licence
 * ne peut être lu correctement.
 *
 * @param jour le jour
 * @param mois le mois
 * @param annee l'année
 */
static int lit_licence(int *jour, int *mois, int *annee)
{
  char *s_lf;
  char *home;
  FILE *lic_file;
  char *str;

  /* fichier de licence */
  home = getenv ("HOME");
  s_lf = (char *)malloc(strlen(home) + 9);
  sprintf(s_lf, "%s/.b4free", home);
  lic_file = fopen(s_lf, "r");
  if (lic_file != NULL) {
    /* lecture du fichier de licence */
    str = (char *)malloc(TAILLE_LICENCE+1);
    if(str != NULL) {
      /* Lit les données de licence */
      if(fread(str, 1, TAILLE_LICENCE, lic_file) == TAILLE_LICENCE) {
	str[TAILLE_LICENCE] = '\0';
	if(convertit_licence(str, jour, mois, annee) != 0) {
	  // Licence Error
	  fprintf(stderr, "Corrupted license file\n");
	  exit(1);
	}
      } else {
	fprintf(stderr, "Cannot read license\n");
	exit(1);
      }
    } else {
      fprintf(stderr, "Out of memory\n");
      exit(1);
    }

    free(str);
    fclose(lic_file);
  } else {
    fprintf(stderr, "Cannot find license file: %s\n", s_lf);
    fprintf(stderr, "Go to http://www.b4free.com to get a licence\n");
    exit(1);
  }

  free(s_lf);
}


#endif /* LICENCE_BALBULETTE */

#ifdef TIME_LIMIT

#ifndef JOUR
#error "Les variables JOUR MOIS et ANNEE doivent être définies si l'option TIME_LIMIT est activée"
#endif // JOUR

#ifndef MOIS
#error "Les variables JOUR MOIS et ANNEE doivent être définies si l'option TIME_LIMIT est activée"
#endif // JOUR

#ifndef ANNEE
#error "Les variables JOUR MOIS et ANNEE doivent être définies si l'option TIME_LIMIT est activée"
#endif // JOUR


/**
 * Convertit la date passée en paramètre en struct tm.
 *
 * @param jour la jour de la date
 * @param mois le mois de la date
 * @param annee l'année de la date
 * @return la date
 */
static time_t convertit_date(int jour, int mois, int annee)
{
	struct tm tmp;
	time_t result;

	memset(&tmp, 0, sizeof(struct tm));
	tmp.tm_year = annee - 1900;
	tmp.tm_mon = mois - 1;
	tmp.tm_mday = jour;

	result = mktime(&tmp);

	return result;
}
#endif /* TIME_LIMIT */

//
// Pour getopt on n'inclue pas <getopt.h> qui n'est pas fourni
// partout, on se contente de donner le prototype. Le module
// c_port.h fournit une implementation pour les cibles qui
// ne connaissent pas getopt
//
extern int getopt(int argc, char * const argv[], const char *optstring) ;
extern char *optarg ;
extern T_betree *internal_syntax_analysis(const char *component_name,
                                          int dep_mode,
	const char *input_file,
        const char *converterName,
	const char *second_input_file,
	const char *third_input_file,
	T_lexem *load_lexem,
	int complain_if_unreadable);

static void print_dependences(const char *filename, const char* converterName) ;


/* Point d'entree */
int main(int argc, char *argv[])
{
  T_symbol *input_file = NULL ;
  int c ;
  int html = FALSE ;
  int report = FALSE ;
  int seman = FALSE ;
  int B0_check = FALSE ;
  int dependencies = FALSE ;
  char *latex_lr_table_file = NULL ;
  char *html_lr_table_file = NULL ;
  char *latex_visi_table_file = NULL ;
  char *html_visi_table_file = NULL ;
  char *html_class_hierarchy_directory = NULL ;
  char *dependence_filename = NULL ;
  char *converterName = NULL;
  // Nombre de compilations a effectuer
  int nb_compile = 1 ;
  int check_memory = FALSE ;
  int nb_memory_errors = 0 ;
  int show_component_checksums = FALSE ;
  int show_clause_checksums = FALSE ;
  int auto_tests = FALSE ;
#ifdef TIME_LIMIT
int date_limite_fin;
time_t date;
#endif /* TIME_LIMIT */
#ifdef LICENCE_BALBULETTE
#ifdef TIME_LIMIT
	int date_limite_debut, date_limite_dl;
#endif /* TIME_LIMIT */

/* lit la license. Etant-donné qu'il n'y a pas de date limite, le simple
   fait de pouvoir lire la licence autorise l'exécution */
int jour, mois, annee = 1;
lit_licence(&jour, &mois, &annee);

#ifdef TIME_LIMIT
date_limite_debut = convertit_date(JOUR, MOIS, ANNEE);
date_limite_fin = convertit_date(JOUR, MOIS, ANNEE+1);
date_limite_dl =  convertit_date(jour, mois, annee);

/* Évaluation de la limite d'utilisation de la balbulette */
 if (! (date_limite_dl <= date_limite_fin) )
   {
     fprintf(stderr, "This software has expired.\n");
	  exit(1);
   }
#endif /* TIME_LIMIT */
#endif /* BALBULETTE */

#ifdef TIME_LIMIT
 	date_limite_fin = convertit_date(JOUR, MOIS, ANNEE+1);
 	date = time(0);
 	if(date > date_limite_fin)
 	{
 		fprintf(stderr, "This software has expired.\n");
 		exit(1);
 	}
#endif /* TIME_LIMIT */


  //Initialisation du systeme des ressources
  init_resource_manager() ;

  // Initialisation du compilateur
  init_compiler() ;

  set_localedir(argv[0]);

  const char options[] = "Si:VCH:hs:l:RP:t:T:acvb:B:I:dD:L:W:kKr:M:pA" ;

  while ( (c = getopt(argc, argv, options)) != EOF)
	{
	  switch (c)
		{
		case 'a' : { seman = TRUE ; break ; }
		case 'c' : { B0_check = TRUE ; seman = TRUE ; break ; }
		case 'h' : { html = TRUE ; break ; }
		case 'i' : { input_file = lookup_symbol(optarg) ; break ;}
		case 'd' : { dependencies = TRUE ; break ; } ;
		case 'D' :
		  {
            load_project_database(optarg) ;
			set_file_fetch_mode(FFMODE_MS) ;
			break ;
		  }
		case 'L' :
		  {
            load_project_database(optarg) ;
			set_file_fetch_mode(FFMODE_MS) ;
			break ;
		  }
		case 'P' : { set_output_path(optarg) ; break ; }
		case 'p' :
		  {
			// Detection des "conflits chaches" :
			// 1) detection des conflits caches
			set_proof_clash_detection(TRUE) ;
			// 2) traiter comme une erreur les idents a une lettre
			set_kernel_joker_is_an_error(TRUE) ;
			// 3) corriger les lexemes pour les donnees heritees afin
			// d'avoir une bonne localisation dans les messages
			// d'erreur
			set_fix_inherited_lexems_mode(TRUE) ;
			break ;
		  }
		case 'R' :
		  {
			report = TRUE ;
			nb_compile = 2 ;
			check_memory = TRUE ;
			set_no_exit_mode(TRUE) ;
			break ;
		  }
		case 'S' : { set_strict_B_demanded() ; break ; }
 		case 't' : { clone(&latex_lr_table_file, optarg) ; break ; }
		case 'T' : { clone(&html_lr_table_file, optarg) ; break ; }
 		case 'b' : { clone(&latex_visi_table_file, optarg) ; break ; }
		case 'B' : { clone(&html_visi_table_file, optarg) ; break ; }
		case 'H' : { clone(&html_class_hierarchy_directory, optarg) ; break ;}
		case 'V' : { set_verbose_mode() ; break ; }
		case 'k' : { show_component_checksums = TRUE ; break ; } ;
		case 'K' : { show_clause_checksums = TRUE ; break ; } ;
		case 'I' : { add_source_path(optarg) ; break ; }
		case 'C' :
		  {
			/* Informations de compilation */
			betree_info() ;
			return 0;
			break ;
		  }
		  new T_project_manager();
		case 'r' :
		  //Charger le fichier de ressources utilisateur
		  {
			TRACE0("load_resources") ;
			load_resources(optarg, NULL, NULL) ;
			break ;
		  }

		case 'W' :
		  {
			if (strcmp(optarg, "no") == 0)
			  {
				compiler_disable_warnings() ;
				break ;
			  }
			else if (strcmp(optarg, "all") == 0)
			  {
				compiler_enable_extended_warnings() ;
				break ;
			  }
			else
			  {
				s_fprintf(stderr, "Illegal option W%s\n", optarg) ;
				// Fall into
			  }
		  }


		case 'M' :
		  // Nom du fichier de sortie pour les dépendances
		  {
			dependence_filename = optarg ;
			// Il faut la passe sémantique pour calculer les dépendances.
			seman = TRUE ;
			break ;
                  }
                case 'e' :
                  // Nom du format du fichier
                  {
                  converterName = optarg ;
                  break ;
              }

		case 'A' :
		  {
			auto_tests = TRUE ;
			break ;
		  }

		case 'v' :
		  {
		    print("bcomp -- %s (%s)\n", BCOMP_VERSION, BCOMP_SOURCE_CODE_SHA) ;
		    print("         %s\n", BCOMP_COPYRIGHT) ;
		    // Falls through on purpose
		  }
		default :
		  {
			print("\n");
			print("Based on the ICU4C library, copyright (c) 2016 and later Unicode, Inc. and others.\n") ;
			print("A copy of the license should have been distributed with this program.\n");
			print("See also: <https://icu.unicode.org>\n");
			print("\n");
			print("Usage :\n") ;
			print("--------- INPUT/OUTPUT options ------------\n") ;
			print("\t-i <file>  : input file for syntax analysis\n") ;
			print("\t-[s|l] <file>  : [save|load] Betree [into|from] file\n") ;
			print("\t-h         : create an HTML dump after proceeding\n") ;
			print("\t-I <path>  : add include path for source files\n") ;
			print("\t-L <.db path>  : add include path Atelier B libraries\n") ;
			print("\t-D <project_database>  : use database information for source files\n") ;
			print("\t-P <path>  : set output path\n") ;
			print("\t-r <file>  : load resource file\n") ;
			print("\t-M <file>  : output dependencies in file\n") ;
                        print("\t-e <name>  : encoding\n") ;

			print("------------- ANALYSIS options ------------\n") ;
			print("\t-d         : dependencies analysis only\n") ;
			print("\t-a         : semantic analysis\n") ;
			print("\t-c         : B0 check\n") ;

			print("------------- COMPILER options ------------\n") ;
			print("\t-C         : report compilation informations\n") ;
			print("\t-S         : strict B mode\n") ;
			print("\t-V         : verbose mode\n") ;
			print("\t-Wall/-Wno : enable extended warnings/disable all warnings\n");
			print("\t-k         : show component checksums\n") ;
			print("\t-K         : show clause checksums\n") ;
			print("\t-p         : detect Proof clashes\n") ;
			print("\t-A         : auto-tests\n");

			print("---------- DOCUMENTATION options ----------\n") ;
			printf("\t-[t|T] <file> : dump internal lexems table into [LaTeX|HTML] file\n") ;
			printf("\t-[b|B] <file> : dump visibility tables into [LaTeX|HTML] file\n") ;
			printf("\t-v            : prints version (sha) and copyright\n") ;
			/* A faire : etudier la liberation (versions DOS) */
			return 1 ;
		  }
		}
	}


#ifdef JETON
  int jeton;

  // Demander un jeton au serveur
	jeton = dl_demandeJeton (DL_APPLI_BCOMP, DL_BCOMP);

  if (jeton != JETON_ACCORDE)
	{
	  dl_perror (jeton);
	  exit (1);
	}

  // Rendre le jeton au serveur
  jeton = dl_rendsJeton();

  if ((jeton != JETON_RECUPERE) && (jeton != JETON_RESERVE))
	{
	  dl_perror (jeton);
	}

#endif




  TRACE0("ICI commence la compilation") ;
  T_betree *betree = NULL ;

  if (latex_lr_table_file != NULL)
	{
	  latex2e_dump_LR_tables(latex_lr_table_file, get_output_path()) ;
	  exit(0) ;
	}

  if (html_lr_table_file != NULL)
	{
	  html_dump_LR_tables(html_lr_table_file, get_output_path()) ;
	  exit(0) ;
	}

  if (latex_visi_table_file != NULL)
	{
	  latex2e_dump_visibility_tables(latex_visi_table_file, get_output_path()) ;
	  exit(0) ;
	}

  if (html_visi_table_file != NULL)
	{
	  html_dump_visibility_tables(html_visi_table_file, get_output_path()) ;
	  exit(0) ;
	}
  if (html_class_hierarchy_directory != NULL)
	{
	  dump_classes_hierarchy(html_class_hierarchy_directory) ;
	  exit(0) ;
	}


  if (input_file == NULL)
	{
	  toplevel_error(FATAL_ERROR, get_error_msg(E_NO_INPUT_NAME_GIVEN)) ;
	}

  while (nb_compile-- > 0)
	{
	  if (dependencies == TRUE)
		{
                  betree = compiler_dep_analysis(input_file->get_value(), converterName) ;
		}
	  else
		{
#ifdef DEBUG_LEAKS
		  int cpt = 1 ;
		  while (cpt < 300)
			{
			  TRACE1("DEBUT DEBUG_LEAKS passe %d", cpt) ;
			  ENTER_TRACE ; ENTER_TRACE ;
#endif // DEBUG_LEAKS
                          betree = compiler_syntax_analysis(NULL, input_file->get_value(), converterName) ;
#ifdef DEBUG_LEAKS
			  EXIT_TRACE ; EXIT_TRACE ;
			  TRACE1(" FIN  DEBUG_LEAKS passe %d", cpt) ;
			  cpt++ ;
			}
#endif // DEBUG_LEAKS

		  if ( (seman == TRUE) && (get_error_count() == 0) )
			{
			  // On fait en plus l'analyse semantique
                          betree = compiler_semantic_analysis(betree, converterName) ;
			}

		  if ( (B0_check == TRUE)
			   /* AB le 15/01/99
				  && (get_error_count() == 0)*/ )
			{
			  // On fait en plus le B0 Check
			  betree = compiler_B0_check(betree) ;
			}

		  if ( (auto_tests == TRUE) && (get_error_count() == 0) )
			{
			  // On fait quelques vérifications internes
			  auto_test_ident() ;
			}

		  if (show_component_checksums == TRUE)
			{
			  printf(get_catalog(C_COMPONENT_CHECKSUM),
					 betree->get_file_name()->get_value(),
					 betree->get_checksum()->get_value(),
					 betree->get_expanded_checksum()->get_value()) ;
			}
		  if (show_clause_checksums == TRUE)
			{
			  // D'abord les clauses
			  T_metaclass *metaclass = get_metaclass(NODE_MACHINE) ;
			  T_field *cur_field = metaclass->get_fields() ;

			  while (cur_field != NULL)
				{
				  const char *name = cur_field->get_field_name() ;
				  // 123456789
				  // _checksum
				  int len = strlen(name) ;
				  if (len > 10)
					{
					  // On recupere la fin
					  const char *suffix = (char *)((size_t)name + len - 8) ;
					  if (strcmp(suffix, "checksum") == 0)
						{
						  T_symbol *checksum =
							*(T_symbol **) ((size_t)betree->get_root()
										  + cur_field->get_field_offset()) ;
						  TRACE4("root %x offset %d checksum = %x is_an_object %s",
								 betree->get_root(),
								 cur_field->get_field_offset(),
								 checksum,
								 (object_test(checksum) == TRUE)
								 ? "TRUE" : "FALSE") ;
						  printf(get_catalog(C_CLAUSE_CHECKSUM),
								 name,
								 (checksum == NULL)
								 ? "NULL" : checksum->get_value()) ;
						}
					}
				  cur_field = cur_field->get_next() ;
				}

			  // Ensuite les operations
			  T_op *cur_op = betree->get_root()->get_operations() ;

			  while (cur_op != NULL)
				{
				  printf(get_catalog(C_OP_PRECOND_CHECKSUM),
						 cur_op->get_name()->get_name()->get_value(),
						 (cur_op->get_precond_checksum() == NULL)
						 ? "NULL" : cur_op->get_precond_checksum()->get_value()) ;
				  printf(get_catalog(C_OP_BODY_CHECKSUM),
						 cur_op->get_name()->get_name()->get_value(),
						 (cur_op->get_body_checksum() == NULL)
						 ? "NULL" : cur_op->get_body_checksum()->get_value()) ;
				  cur_op = (T_op *)cur_op->get_next() ;
				}
			  //...et l'initialisation
			  T_op *init = betree->get_root()->get_initialisation_clause_operation() ;
			  if (init != NULL)
				{
				  printf(get_catalog(C_OP_BODY_CHECKSUM),
						 init->get_name()->get_name()->get_value(),
						 (init->get_body_checksum() == NULL)
						 ? "NULL" : init->get_body_checksum()->get_value()) ;
				}
			}
		}

	  if (dependence_filename != NULL)
		{
		  // On imprime les dépendances entre fichiers
                  print_dependences(dependence_filename, converterName) ;
		}

	  if (html == TRUE)
		{
		  // Dump HTML
		  TRACE0("DUMP HTML (debut)") ; ENTER_TRACE ;
		  compiler_dump_HTML_betree(betree) ;
		  EXIT_TRACE ; TRACE0("DUMP HTML (fin)") ;
		}

	  if (check_memory == FALSE)
		{
		  // Sortie standard en cas d'erreur : confused + fatal_error
		  if (get_error_count() != 0)
			{
			  // Le message d'erreur est affiche par get_error_count
			  toplevel_error(FATAL_ERROR,
							 get_error_msg(E_CONFUSED),
							 get_error_count()) ;
			}
		}

#ifdef STATS_CHAIN
	  get_chain_stats() ;
#endif

	  if ( (get_error_count() == 0) && (nb_memory_errors == 0) )
		{
		  if (report == TRUE)
			{
			  print(get_catalog(C_BCOMP_SUCCESS)) ;
			}
		}
	  // Le bilan est affiche par get_error_count
	  else
		{
		  // Bilan du nombre d'erreurs
		  toplevel_error(CAN_CONTINUE,
						 get_error_msg(E_CONFUSED),
						 get_error_count()) ;
		}

	}

  return nb_memory_errors ;
}


// Résumé : Donne le chemin d'accès relatif du fichier à partir duquel
// a ete lu betree
static const char *get_path(T_betree *betree)
{
  const char *result ;
  T_symbol *path = betree->get_path() ;
  T_symbol *file_name = betree->get_file_name() ;

  if (path != NULL)
	{
	  size_t len = path->get_len() + 1 + file_name->get_len() + 1 ;
	  char *full_name = new char[len] ;
	  sprintf( full_name, "%s/%s", path->get_value(), file_name->get_value() ) ;
	  result = full_name ;
	}
  else
	{
	  result = file_name->get_value() ;
	}
  TRACE2("<- get_path(%x) returns '%s'", betree, result) ;
  return result;
}

static char *concat_string(const char *base_name, const char *ext)
{
	int length = strlen(base_name) + strlen(ext) + 1;
	char *result = new char[length];
	result[0] = '\0';

	strcat(result, base_name);
	strcat(result, ext);

	return result;
}

/**
 * Ajoute la machine referencée au fichier de dépendances
 *
 * @param file le fichier de dépendances
 * @param name le nom de la machine a ajouter
 * @param check_refinements indique si on doit éventuellement rechercher
 *    un raffinement lorsqu'on recherche le composant (sinon, on ne
 *    recherche que des machines d'extension .mch
 */
static void
print_referenced_machine(FILE *file,
                         const char *converterName, T_machine_type type_mch, T_symbol *name, int check_refinements)
{
	const char *base_name = name->get_value();
	// charge le betree correspondant à l'abstraction

	const char *file_1 =
		(type_mch == MTYPE_SYSTEM) ? concat_string(base_name, ".sys")
				: concat_string(base_name, ".mch");
	const char *file_2 = NULL;

	if(check_refinements)
	{
		file_2 = concat_string(base_name, ".ref");
	}

	// charge la machine
        internal_syntax_analysis(base_name, TRUE, converterName, file_1, file_2, NULL, NULL, TRUE);

	T_betree *tree = betree_manager_get_betree(name);
	if(tree != NULL)
	{
		fprintf(file, " \\\n\t%s", get_path(tree));
	}

	delete [] file_1;
	delete [] file_2;
}

// Résumé : Donne la liste des machines dont dépend le betree principal
//
// Méthode : Parcours + impression des éléments de la liste de
// T_betree (T_betree_list) ayant pour origine betree => on obtient
// toutes les machines incluses, importées, vues...
//
// Résultat :On range le résultat dans un fichier au format lisible par
// make.
static void
print_dependences(const char *filename,
                  const char *converterName)
{
  TRACE1("-> print_dependences(%s)", filename);

  // Ouverture du fichier de sortie
  FILE *output_file = fopen(filename,"w") ;
  if (output_file == NULL)
	{
	  // Erreur d'ouverture!!
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 filename,
					 strerror(errno)) ;
	}

    T_betree *cur_betree = get_betree_manager()->get_betrees();
	fprintf(output_file, "%s:", get_path(cur_betree));
	// définitions
	if (cur_betree->get_file_definitions() != NULL)
  	{
    	T_file_definition * cur = cur_betree->get_file_definitions();
    	do
      	{
      		fprintf(output_file, " \\\n\t%s%s%s",
				(cur->get_path() == NULL) ? "" : cur->get_path()->get_value(),
				(cur->get_path() == NULL) ? "" : "/",
				cur->get_name()->get_value());
      	} while ((cur = (T_file_definition *)cur->get_next()) != NULL);
	}

	if (cur_betree->get_root() != NULL &&
		cur_betree->get_root()->get_abstraction_name() != NULL)
	{
                print_referenced_machine(output_file, converterName,
				cur_betree->get_root()->get_specification()->get_machine_type(),
			cur_betree->get_root()->get_abstraction_name()->get_name(),
			TRUE);
	}

	// include
	T_used_machine *inc = cur_betree->get_root()->get_includes();
	while (inc != NULL) {
                print_referenced_machine(output_file,converterName,
				inc->get_betree()->get_root()->get_specification()->get_machine_type(),
				inc->get_component_name(), FALSE);
    	inc = (T_used_machine *)inc->get_next();
  	}

  	// sees
  	T_used_machine *sees = cur_betree->get_root()->get_sees();
  	while (sees != NULL) {
                print_referenced_machine(output_file,converterName,
				sees->get_betree()->get_root()->get_specification()->get_machine_type(),
				sees->get_component_name(), FALSE);
    	sees = (T_used_machine *)sees->get_next();
  	}

  	// extends
	T_used_machine *extends = cur_betree->get_root()->get_extends();
	while (extends != NULL) {
                print_referenced_machine(output_file,converterName,
				extends->get_betree()->get_root()->get_specification()->get_machine_type(),
				extends->get_component_name(), FALSE);
		extends = (T_used_machine *)extends->get_next();
  	}

	// import
	T_used_machine *import = cur_betree->get_root()->get_imports();
	while (import != NULL) {
                print_referenced_machine(output_file,converterName,
				import->get_betree()->get_root()->get_specification()->get_machine_type(),
				import->get_component_name(), FALSE);
    	import = (T_used_machine *)import->get_next();
	}

	// uses
	T_used_machine *uses = cur_betree->get_root()->get_uses();
	while (uses != NULL) {
                print_referenced_machine(output_file,converterName,
				uses->get_betree()->get_root()->get_specification()->get_machine_type(),
				uses->get_component_name(), FALSE);
		uses = (T_used_machine *)uses->get_next();
	}

	cur_betree = cur_betree->get_next_betree();

	fprintf(output_file, "\n");

	fclose(output_file) ;


	TRACE0("<- print_dependences");
}
