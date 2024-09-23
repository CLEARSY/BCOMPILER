/******************************* CLEARSY **************************************
* Fichier : $Id: p_file.cpp,v 2.0 2000/07/11 16:08:50 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_file
*				Modelisation d'un fichier.
*
This file is part of RESSOURCE
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
RCS_ID("$Id: p_file.cpp,v 1.9 2000/07/11 16:08:50 fl Exp $") ;

// Includes du systeme
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <regex>
#include <string>

// Includes head
#include "p_head.h"


// Definition de Fonctions
// ________________________

// stat securise
static int s_stat(const char *file_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> s_stat \"%s\"",
		 file_acp) ;
#endif

  // Infos sur le fichier a charger
  struct stat stats_ls ;

  // Lecture des infos du fichier
  if (stat(file_acp, &stats_ls) != 0)
	{
	  fprintf(stderr, "Unable to open resource file %s ", file_acp) ;
	  perror("because") ;
	  return -1 ;
#ifdef DEBUG_RESOURCES
	  EXIT_TRACE ;
#endif
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  return stats_ls.st_size ;
}

// rename securise
static int s_rename(const char *old_name_acp,
							  const char *new_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> s_rename file \"%s\" in \"%s\"",
		 old_name_acp,
		 new_name_acp) ;
#endif

  if(rename(old_name_acp, new_name_acp) != 0)
	{
	  fprintf(stderr,
			  "Can rename file: %s in %s ",
			  old_name_acp,
			  new_name_acp) ;
	  perror("because") ;
	  return FALSE ;
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  return TRUE ;
}

// fopen securise
FILE *s_fopen(const char *file_acp,
					   const char *format_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> s_fopen \"%s\" format \"%s\"",
		 file_acp,
		 format_acp) ;
#endif

  FILE *file_lcp = fopen(file_acp, format_acp) ;

  if(file_lcp == NULL)
	{
	  fprintf(stderr, "Can not open file: %s ", file_acp) ;
	  perror("because") ;
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  return file_lcp ;
}

// fclose securise
static int s_fclose(FILE *file_aip,
							  const char *message_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> s_fclose file %d message \"%s\"",
		 file_aip,
		 message_acp) ;
#endif

  if(fclose(file_aip) == EOF)
	{
	  fprintf(stderr, "Can not close file: %s ", message_acp) ;
	  perror("because") ;
	  return FALSE ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  return TRUE ;
}

// fprintf securise
static int s_fprintf(FILE *file_aip,
							   const char *message_acp,
							   const char *format_acp,
							   const char *string_acp,
							   int length_ai)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> s_fprintf file %d string \"%s\" length %d",
		 file_aip,
		 string_acp,
		 length_ai) ;
#endif

  if (fprintf(file_aip, format_acp, string_acp) != length_ai)
	{
	  fprintf(stderr, "Can not write in file: %s ", message_acp) ;
	  perror("because") ;
	  return FALSE ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif

  return TRUE ;
}


// Definition de Methodes
// _______________________

// Constructeur
T_resource_file::T_resource_file(const char *new_name_acp,
											T_resource_source new_source_ae,
											T_resource_item **adr_first_aopp,
											T_resource_item **adr_last_aopp,
											T_resource_item *new_father_aop)
  : T_resource_item (adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  TRACE3("T_resource_file(%x)::T_resource_file(%s, d)",
		 this,
		 new_name_acp,
		 new_source_ae) ;
#endif

  // Positionne le type de l'objet
  set_type(RESOURCE_FILE) ;

  // Copie le nom du fichier
  name_op = new T_resource_string(new_name_acp) ;

  // Positionne la source du fichier
  set_source(new_source_ae) ;

  // Positionne la liste de lignes
  first_line_op = NULL ;
  last_line_op  = NULL ;
}


// Destructeur
T_resource_file::~T_resource_file(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("T_resource_file(%x)::~T_resource_file", this, name_op->get_string()) ;
#endif

  // Libere la liste de lignes
  delete_list(first_line_op, last_line_op) ;

  // Libere la chaîne nom
  if (name_op != NULL)
	{
	  delete name_op ;
	}
#ifdef DEBUG_RESOURCES
  TRACE1("<- Del FILE %x", this) ;
  EXIT_TRACE ;
#endif
}


// Recherche d'une ligne
T_resource_line *T_resource_file::get_line(int new_number_ai)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Get FILE LINE %x N%d",
		 this,
		 new_number_ai) ;
#endif

  // Pointeur sur la premiere ligne de la liste
  T_resource_line *line_lop = (T_resource_line*)(*get_adr_first_line()) ;

  // Parcours la liste jusqu'a la fin de liste ou trouver le bon numero
  while( (line_lop != NULL) && (line_lop->get_number() < new_number_ai) )
	{
	  line_lop = (T_resource_line*)line_lop->get_next() ;
	}

#ifdef DEBUG_RESOURCES
  TRACE2("<- Get FILE LINE \"%s\" N%d",
		 (line_lop != NULL)? line_lop->get_value()->get_string():"VIDE",
		 (line_lop != NULL)? line_lop->get_number():0) ;
  EXIT_TRACE ;
#endif

  return line_lop ;
}


// Affectation du nom du fichier
void T_resource_file::set_name(const char *new_string_acp)
{
  // Detruit le nom courant
  if(name_op != NULL)
	{
	  delete name_op ;
	  name_op = NULL ;
	}

  // Cree un nouveau nom
  name_op = new T_resource_string(new_string_acp) ;
}

// Affectation de la  ligne
void T_resource_file::set_line(int new_number_ai,
										 const char *new_string_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> Set FILE LINE %x N%d value \"%s\"",
		 this,
		 new_number_ai,
		 new_string_acp) ;
#endif

  // Recherche de la ligne
  T_resource_line *line_lop = get_line(new_number_ai) ;

 // Remplacement des variables d'environnement par leur valeur
	std::string line = new_string_acp;

	{
    std::regex re(R"_((?:^|[^\$]+)(\$[A-Za-z0-9._-]+))_");
		std::smatch matches;
		std::string::const_iterator searchStart(line.cbegin());
		while (std::regex_search(searchStart, line.cend(), matches, re)) {
			const std::string envvar = matches[1];
			char *replacement = std::getenv(envvar.c_str() + 1);
			if (replacement == NULL) {
				  fprintf(stderr,
						"%s:%d: Resource file error: unknown environment variable %s\n",
						name_op->get_string(),
						new_number_ai,
						envvar.c_str()) ;
				searchStart = matches[1].first + envvar.length();
			} else {
				line.replace(matches[1].first, matches[1].second, replacement);
				searchStart = matches[1].first + strlen(replacement);
			}
		}

		const std::string target = "$$";
    const std::string replacement = "$";
    size_t pos = 0;

    while ((pos = line.find(target, pos)) != std::string::npos) {
        line.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
	}
 const char *expanded_line = line.c_str();

    // Mise a jour de la valeur de la ligne
    if (line_lop != NULL) {
      line_lop->set_value(new T_resource_string(expanded_line));
    }
    // Cree une nouvelle ligne
    else {
      // Numero de la nouvelle ligne
      int new_number_li = 1;

      // Cherche le numero de la derniere ligne
      if ((*get_adr_last_line()) != NULL) {
        // Numero nouvelle ligne = dernier numero + 1
        new_number_li =
            ((T_resource_line *)(*get_adr_last_line()))->get_number() + 1;
      }
#ifdef DEBUG_RESOURCES
	  TRACE1("   Last N%d", new_number_li) ;
#endif
	  // Creation de la ligne
      T_resource_string resource_string(expanded_line) ;
	  line_lop = new T_resource_line(new_number_li,
                                      &resource_string,
									  &first_line_op,
									  &last_line_op,
									  this) ;
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Destruction de la ligne
void T_resource_file::delete_line(int new_number_ai)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Del FILE LINE %x number %d", this, new_number_ai) ;
#endif

  // Recherche de la ligne
  T_resource_line *line_lop = get_line(new_number_ai) ;

  // Destruction de la ligne
  if(line_lop != NULL)
	{
	  delete_item(line_lop, first_line_op, last_line_op) ;
	}
#ifdef DEBUG_RESOURCES
  TRACE1("<- Del FILE LINE %x", line_lop) ;
  EXIT_TRACE ;
#endif
}


// Chargement du contenu d'un fichier
T_resource_line *T_resource_file::load(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Load FILE %x name \"%s\"",
		 this,
		 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
#endif

  // Taille du tampon du fichier
  int file_size_li = s_stat(name_op->get_string()) ;
#ifdef DEBUG_RESOURCES
  TRACE1("file_size_li = %d", file_size_li) ;
#endif

  // Lecture des infos du fichier
  if (file_size_li <= 0)
	{
#ifdef DEBUG_RESOURCES
	  TRACE2("<- Load FILE %x name \"%s\" error infos",
			 this,
			 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
	  EXIT_TRACE ;
#endif
	  return NULL ; // Echec
	}

  // Ouverture du fichier en mode "lecture seule"
  FILE *file_lip = s_fopen(get_name()->get_string(), "r") ;

  if(file_lip == NULL)
	{
#ifdef DEBUG_RESOURCES
	  TRACE2("<- Load FILE %x name \"%s\" error open",
			 this,
			 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
	  EXIT_TRACE ;
#endif

	  return NULL ; // Echec
	}

  // Numero de ligne
  int nbr_line_li = 0 ;

  // Cherche le dernier numero de ligne
  if(last_line_op != NULL)
	{
	  nbr_line_li = ((T_resource_line*)last_line_op)->get_number()+1 ;
	}

  // Tampon de ligne
  char *line_buffer_lcp = new char [file_size_li] ;

  // Position dans le tampon
  int pos_buffer_li = 0 ;

  // Caratere lu
  int cur_char_li = 0 ;

  while((cur_char_li = fgetc(file_lip)) != EOF)
	{
	  // Cas d'un retour chariot
	  if(cur_char_li == get_RESOURCE_MARK_RETURN())
		{
		  // Incremente le numero de ligne
		  nbr_line_li++ ;

		  // Termine le tampon
		  line_buffer_lcp[pos_buffer_li] = get_RESOURCE_MARK_ENDLINE() ;

		  // Cree la nouvelle ligne
		  set_line(nbr_line_li, line_buffer_lcp) ;

		  // Repart en debut de tampon
		  pos_buffer_li = 0 ;
		}
	  // Cas general
	  else
		{
		  // Ecrit dans le tampon
		  line_buffer_lcp[pos_buffer_li] = cur_char_li ;

		  // Avance la position
		  pos_buffer_li++ ;
		}
	}
  if (ferror(file_lip))
  {
    // TODO should we warn the user about the error?
  }
  // Libere le tampon
  delete [] line_buffer_lcp ;
  line_buffer_lcp = NULL ;

  // Ferme le fichier
  s_fclose(file_lip, get_name()->get_string()) ;
#ifdef DEBUG_RESOURCES
  TRACE2("<+ Load FILE %x name \"%s\"",
		 this,
		 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif
  // Retourne la premiere ligne
  return get_line(1) ;
}


// Archivage des resources d'un fichier
const char *T_resource_file::save(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Save FILE %x name \"%s\"",
		 this,
		 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
#endif

  // Prepare le nom de l'ancien fichier
  // de taille get_length()+2 car + TILDA + ENDLINE
  char *old_file_lcp = new char [get_name()->get_length()+2] ;
  strcpy(old_file_lcp, get_name()->get_string()) ;

  old_file_lcp[get_name()->get_length()]   = get_RESOURCE_MARK_TILDA() ;
  old_file_lcp[get_name()->get_length()+1] = get_RESOURCE_MARK_ENDLINE() ;

  // Renomme le fichier
  if( s_rename(get_name()->get_string(), old_file_lcp) == FALSE )
	{ // NE FAIT RIEN -> Le fichier n'exite pas
	}

  // Ouvre un nouveau fichier d'archivage
  FILE *file_lip = s_fopen(get_name()->get_string(), "w") ;

  if( file_lip == NULL )
	{
	  return old_file_lcp ; // Echec
	}

  // Archivage des lignes dans le fichier
  T_resource_line *line_lop = (T_resource_line*)(*get_adr_first_line()) ;
  while(line_lop != NULL)
	{
	  T_resource_string *string_lop = line_lop->get_value() ;

	  // Ecrit la ligne courante valide plus '\n'
	  if( s_fprintf(file_lip,
					get_name()->get_string(),
					"%s\n",
					string_lop->get_string(),
					string_lop->get_length()+1) == FALSE )
		{
		  // Test le resultat de l'ecriture
		  return old_file_lcp ; // Echec
		}

	  // Passe a la suivante
	  line_lop = (T_resource_line*)line_lop->get_next() ;
	}

  // Ferme le fichier
  s_fclose(file_lip, get_name()->get_string()) ;

#ifdef DEBUG_RESOURCES
  TRACE2("<- Save FILE %x name \"%s\"",
		 this,
		 (get_name() != NULL)? get_name()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif

  return get_name()->get_string() ; // Succes
}


// Fin de fichier
// _______________
