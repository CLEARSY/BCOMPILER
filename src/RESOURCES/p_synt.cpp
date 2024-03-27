/******************************* CLEARSY **************************************
* Fichier : $Id: p_synt.cpp,v 2.0 2000/07/11 16:10:38 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_anal
*				Analyse syntaxique.
*
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
RCS_ID("$Id: p_synt.cpp,v 1.10 2000/07/11 16:10:38 fl Exp $") ;

// Includes systeme
#include <stdio.h>
#include <sys/stat.h>

// includes head
#include "p_head.h"


// Definition de Fonctions
// ________________________

// Cree un nouveau tableau de caratere
static char *set_array(char* array_acp, size_t length_ai)
{
  // Alloue le tableau de taille length_ai+1
  char *array_lcp = new char [length_ai+1] ;

  // Copie la chaîne array_acp de taille length_ai dans array_lcp
  strncpy(array_lcp, array_acp, length_ai) ;

  // Positionne le marqueur de fin de line
  array_lcp[length_ai] = get_RESOURCE_MARK_ENDLINE() ;

  // Retourne l'adresse du tableau remplie
  return array_lcp ;
}

// Efface les "blancs" en fin de chaine(espaces, tabulations, retours chariots)
static char *erase_blank(char *end_lexem_lcp)
{
  //on revient avant fin de ligne
  end_lexem_lcp -- ;

  while( (*end_lexem_lcp == get_RESOURCE_MARK_TAB())	||
		 (*end_lexem_lcp == get_RESOURCE_MARK_SPACE())	)
	{
	  (end_lexem_lcp)-- ;
	}

  //retourne apres la valeur de la ressource
  end_lexem_lcp++ ;

  return end_lexem_lcp ;
}

// Recherche la fin d'un lexem dans une chaîne
static char *find_end_resource_value(char *lexem_acp)
{
  // Pointeur de fin du lexem
  char *end_lexem_lcp = lexem_acp ;
  int finished = 0;

  while(!finished)
  {
	  if(*end_lexem_lcp == get_RESOURCE_MARK_ENDLINE() ||
	     *end_lexem_lcp == get_RESOURCE_MARK_RETURN())
	  {
		  finished = 1;
	  }
	  else if(*end_lexem_lcp == get_RESOURCE_MARK_NEXTLINE())
	  {
		  if(end_lexem_lcp[1] == get_RESOURCE_MARK_ENDLINE() ||
				  end_lexem_lcp[1] == get_RESOURCE_MARK_RETURN())
		  {
			  finished = 1;
		  }
		  else
		  {
			  ++end_lexem_lcp;
		  }
	  }
	  else
	  {
		  ++end_lexem_lcp;
	  }

  }
//  while( (*end_lexem_lcp != get_RESOURCE_MARK_NEXTLINE())	&&
//		 (*end_lexem_lcp != get_RESOURCE_MARK_ENDLINE())	&&
//		 (*end_lexem_lcp != get_RESOURCE_MARK_RETURN()))
//	{
//	  end_lexem_lcp++ ;
//	}

  //efface les blanc en fin de valeur de ressource
  end_lexem_lcp = erase_blank(end_lexem_lcp) ;

  // Retourne le pointeur sur la fin
  return end_lexem_lcp ;
}


// Saute les "blancs" (espaces, tabulations, retours chariots)
static char *jump_blank(char *line_acp)
{
  while( (*line_acp == get_RESOURCE_MARK_RETURN()) ||
		 (*line_acp == get_RESOURCE_MARK_TAB())	||
		 (*line_acp == get_RESOURCE_MARK_SPACE())	)
	{
	  (line_acp)++ ;
	}
  return line_acp ;
}



// Analayse d'un commentaire
static int nextline_analysis(char *line_acp)
{
  if( (line_acp != NULL) && (*line_acp == get_RESOURCE_MARK_NEXTLINE()) )
	{
	  return TRUE ; // Susses
	}
  return FALSE ; // Echec
}


// Attache la liste des commentaires libres a la resource
static void stick_comment( T_resource_ident *cur_resource_aop,
									T_resource_item *&first_comment_aop,
									T_resource_item *&last_comment_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> Catch COMMENT to RESOURCE %x", cur_resource_aop) ;
#endif

  // Rattache la liste a la resource
  cur_resource_aop->set_first_comment(first_comment_aop) ;
  cur_resource_aop->set_last_comment(last_comment_aop) ;

  // Pointeur sur le commentaire courant
  T_resource_item *cur_comment_lop = first_comment_aop ;

  // Parcours la liste des commentaires
  while(cur_comment_lop != NULL)
	{
#ifdef DEBUG_RESOURCES
	  TRACE2("   Catch COMMENT %x to RESOURCE %x",
			 cur_comment_lop,
			 cur_resource_aop) ;
#endif

	  // Associe le commentaire a la resource
	  cur_comment_lop->set_father(cur_resource_aop) ;

	  // Passe au suivant
	  cur_comment_lop = cur_comment_lop->get_next() ;
	}

  // Repositionne la liste a NULL apres l'avoir attache
  first_comment_aop = NULL ;
  last_comment_aop = NULL ;

#ifdef DEBUG_RESOURCES
  TRACE1("<- Catch COMMENT to RESOURCE %x", cur_resource_aop) ;
  EXIT_TRACE ;
#endif
}


// Creation d'une nouvelle resource
static int new_resource(T_resource_line *cur_line_aop,
								   T_resource_line *multi_line_aop,
								   const char *tool_name_acp,
								   const char *ress_name_acp,
								   const char *ress_value_acp,
								   T_resource_item *&first_comment_aop,
								   T_resource_item *&last_comment_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> New RESOURCE LINE %x \"%s\" TOOL %s",
		 cur_line_aop,
		 ress_name_acp,
		 tool_name_acp) ;
#endif

  // Appelle le manager
  T_resource_manager *manager_lop = get_resource_manager() ;

  if(manager_lop != NULL)
	{
	  // Cherche l'outil
	  T_resource_tool *tool_lop = manager_lop->get_tool(tool_name_acp) ;

	  // S'il n'existe pas, alors le creer
	  if(tool_lop == NULL)
		{
		  tool_lop = manager_lop->set_tool(tool_name_acp) ;
		}

	  // S'il existe, alors le modifier
	  if(tool_lop != NULL)
		{
		  // Met a jour ou cree la nouvelle resource
		  // et l'attache a la ligne transmise
		  T_resource_ident *resource_lop
			= tool_lop->set_resource(ress_name_acp,
									  ress_value_acp,
									  cur_line_aop) ;

		  // Si la resource est sur deux lignes
		  while( (multi_line_aop != NULL) &&
				 (multi_line_aop != cur_line_aop) )
			{
			  // Attribut la resource a la ligne
			  multi_line_aop->set_item(resource_lop) ;
			  // Passe au suivant
			  multi_line_aop = (T_resource_line*)multi_line_aop->get_next() ;
			}

		  // Attache les commentaires libres
		  stick_comment(resource_lop, first_comment_aop, last_comment_aop) ;

#ifdef DEBUG_RESOURCES
		  TRACE3("<+ New RESOURCE LINE %x \"%s\" TOOL %s",
				 cur_line_aop,
				 ress_name_acp,
				 tool_name_acp) ;
		  EXIT_TRACE ;
#endif

		  return TRUE ; // Succes
		}
	}
#ifdef DEBUG_RESOURCES
  TRACE3("<- New RESOURCE LINE %x \"%s\" TOOL %s",
		 cur_line_aop,
		 ress_name_acp,
		 tool_name_acp) ;
  EXIT_TRACE ;
#endif

  return FALSE ; // Echec
}


// Affichage de message securise
static void s_message(const char *message_acp,
							   T_resource_line *line_aop)
{
  fprintf(stderr,
		  "%s:%d: Resource syntax error: %s\n",
		  ((T_resource_file*) line_aop->get_father())->get_name()->get_string(),
		  line_aop->get_number(),
		  message_acp) ;
}


// Recherche le separateur dans une chaine de carateres
static char *find_sep(char *string)
{
  char *ptr_lcp = string ;

  while(*ptr_lcp != get_RESOURCE_MARK_ENDLINE())
	{
	  if( (*ptr_lcp == get_RESOURCE_MARK_STAR()) ||
		  (*ptr_lcp == get_RESOURCE_MARK_POINT()) )
		{
		  return ptr_lcp ;
		}

	  ptr_lcp++ ;
	}
  return NULL ;
}

// Analayse d'une ressource
static int resource_analysis(T_resource_line *&line_aop,
										char *line_acp,
										T_resource_item *&first_comment_aop,
										T_resource_item *&last_comment_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
#endif

  // Pointeur sur la premiere ligne d'une ressource ecrite sur deux lignes
  T_resource_line *multi_line_lop = NULL ;

  // Calcule la longueur de get_RESOURCE_MARK_RESOURCE()
  int mark_length_li = strlen(get_RESOURCE_MARK_RESOURCE()) ;

  // Format resource : ATB*TOOL*PREFIX*RESOURCE: VALUE
  // Test le marqueur de resource
  if( (strncmp(line_acp, get_RESOURCE_MARK_RESOURCE(), mark_length_li)
	   == 0) &&
	  ( (line_acp[mark_length_li] == get_RESOURCE_MARK_STAR()) ||
		(line_acp[mark_length_li] == get_RESOURCE_MARK_POINT()) ) )
	{
#ifdef DEBUG_RESOURCES
	  TRACE0("RESOURCE MARK found") ;
#endif

	  // Passe le code get_RESOURCE_MARK_RESOUCE()
	  // Pointeur sur le nom de l'outil (+1 pour passer le separateur)
	  char *tool_lcp = line_acp + (mark_length_li + 1) ;

	  // Pointeur sur le nom de la resource (comprenant le prefix)
	  char *ress_lcp = find_sep(tool_lcp) ;

	  if(ress_lcp == NULL)
		{
#ifdef DEBUG_RESOURCES
		  TRACE2("<- RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
		  EXIT_TRACE ;
#endif

		  s_message("tool name not found", line_aop) ;
		  return FALSE ; // Echec
		}
	  // +1 pour passer le separateur
	  ress_lcp++ ;
#ifdef DEBUG_RESOURCES
	  TRACE1("RESOURCE name \"%s\"", ress_lcp) ;
#endif

	  // Cherche la fin du nom de resource (ici ":")
	  char *end_ress_lcp = strchr(ress_lcp, get_RESOURCE_MARK_COLON()) ;

	  if(end_ress_lcp == NULL)
		{
#ifdef DEBUG_RESOURCES
		  TRACE2("<- RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
		  EXIT_TRACE ;
#endif

		  s_message("resource name or colon not found", line_aop) ;
		  return FALSE ; // Echec
		}

#ifdef DEBUG_RESOURCES
	  TRACE1("   End RESOURCE name \"%s\"", end_ress_lcp) ;
#endif

	  // Pointe la valeur de la resource (+1 pour passer les ":")
	  char *value_lcp = end_ress_lcp + 1 ;
	  // Saute les "blancs"
	  value_lcp = jump_blank(value_lcp) ;

	  // Test la fin de ligne
	  if(*value_lcp == get_RESOURCE_MARK_ENDLINE())
		{
#ifdef DEBUG_RESOURCES
		  TRACE2("<- RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
		  EXIT_TRACE ;
#endif

		  s_message("resource value not found", line_aop) ;
		  return FALSE ; // Echec
		}

	  // Test le marqueur de saut de ligne
	  char* line_lcp = value_lcp ;
	  while(nextline_analysis(line_lcp))
		{
		  // Affecte l'indicateur de la premiere ligne
		  if(multi_line_lop == NULL)
			{
			  multi_line_lop = line_aop ;
			}
		  // Passe a la ligne suivante
		  line_aop = (T_resource_line*)line_aop->get_next() ;
		  // Et, pointe la ligne suivante
		  line_lcp = (char*)line_aop->get_value()->get_string() ;
		  // Saute les "blancs"
		  line_lcp = jump_blank(line_lcp) ;
		  // Repositionne le pointeur de la valeur
		  value_lcp = line_lcp ;
		}

#ifdef DEBUG_RESOURCES
	  TRACE1("   RESOURCE value \"%s\"", value_lcp) ;
#endif

	  // Cherche la fin de valeur de la resource
	  char *end_value_lcp = find_end_resource_value(line_lcp) ;

	  if(end_value_lcp == NULL)
		{
#ifdef DEBUG_RESOURCES
		  TRACE2("<- RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
		  EXIT_TRACE ;
#endif

		  s_message("end of resource value not found", line_aop) ;
		  return FALSE ; // Echec
		}
#ifdef DEBUG_RESOURCES
	  TRACE1("   End RESOURCE value \"%s\"", end_value_lcp) ;
#endif

	  // Recopie les chaînes reconnues
	  char *tool_name_lcp =
		set_array(tool_lcp, ((ress_lcp-1) - tool_lcp)) ;// -1 pour separateur
	  char *ress_name_lcp =
		set_array(ress_lcp, (end_ress_lcp - ress_lcp)) ;
	  char *ress_value_lcp =
		set_array(value_lcp, (end_value_lcp - value_lcp)) ;
#ifdef DEBUG_RESOURCES
	  TRACE2("<+ RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
	  EXIT_TRACE ;
#endif

	  // Cree ou met a jour la resource
	  new_resource(line_aop,
					multi_line_lop,
					tool_name_lcp,
					ress_name_lcp,
					ress_value_lcp,
					first_comment_aop,
					last_comment_aop) ;

	  delete [] ress_value_lcp ;
	  delete [] ress_name_lcp ;
	  delete [] tool_name_lcp ;

	  return TRUE ; // Succes
	}
#ifdef DEBUG_RESOURCES
  TRACE2("<- RESOURCE analysis LINE %x \"%s\"", line_aop, line_acp) ;
  EXIT_TRACE ;
#endif

  return FALSE ; // Echec
}


// Analayse d'un commentaire
static int comment_analysis(T_resource_line *line_aop,
									  char *line_acp,
									  T_resource_item *&first_comment_aop,
									  T_resource_item *&last_comment_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> COMMENT analysis LINE %x \"%s\"",
		 line_aop,
		 line_acp) ;
#endif

  // Recherche le marqueur d'un commentaire
  if(line_acp[0] == get_RESOURCE_MARK_COMMENT())
	{
	  // Cree le nouveau commentaire
	  line_aop->set_item(new T_resource_comment(line_aop,
												 &first_comment_aop,
												 &last_comment_aop,
												 NULL)) ;

#ifdef DEBUG_RESOURCES
	  TRACE2("<+ COMMENT analysis LINE %x \"%s\"",
			 line_aop,
			 line_acp) ;
	  EXIT_TRACE ;
#endif

	  return TRUE ; // Succes
	}

#ifdef DEBUG_RESOURCES
  TRACE2("<- COMMENT analysis LINE %x \"%s\"",
		 line_aop,
		 line_acp) ;
  EXIT_TRACE ;
#endif

  return FALSE ; // Echec
}


// Analyse de lignes
static void line_analysis(T_resource_line *&line_aop,
								   T_resource_item *&first_comment_aop,
								   T_resource_item *&last_comment_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> Line analysis LINE %x", line_aop) ;
#endif

  // Pointeur de ligne
  char *line_acp = (char*)line_aop->get_value()->get_string() ;

  // Saute les "blancs"
  line_acp = jump_blank(line_acp) ;

  // Test la fin de ligne
  if(*line_acp == get_RESOURCE_MARK_ENDLINE())
	{

#ifdef DEBUG_RESOURCES
	  TRACE1("<+ Line analysis LINE %x", line_aop) ;
	  EXIT_TRACE ;
#endif

	  return ;
	}

  // Test un debut de commentaire
  if(TRUE == comment_analysis(line_aop,
							  line_acp,
							  first_comment_aop,
							  last_comment_aop))
	{
#ifdef DEBUG_RESOURCES
	  TRACE1("<+ Line analysis LINE %x", line_aop) ;
	  EXIT_TRACE ;
#endif

	  return ;
	}

  // Test un debut de resource
  if(TRUE == resource_analysis(line_aop,
								line_acp,
								first_comment_aop,
								last_comment_aop))
	{
#ifdef DEBUG_RESOURCES
	  TRACE1("<+ Line analysis LINE %x", line_aop) ;
	  EXIT_TRACE ;
#endif

	  return ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- Line analysis LINE %x", line_aop) ;
  EXIT_TRACE ;
#endif
}


// Analyse syntaxique
void syntax_analysis(T_resource_line *first_line_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("-> Syntax analysis LINE %x", first_line_aop) ;
#endif

  // Pointeurs sur la liste des commentaires libres
  T_resource_item *first_comment_lop = NULL ;
  T_resource_item *last_comment_lop  = NULL ;

  // Ligne courante
  T_resource_line *line_aop = first_line_aop ;

  while(line_aop != NULL)
	{
	  // Analyse de lignes
	  line_analysis(line_aop, first_comment_lop, last_comment_lop) ;

	  // Passe a la ligne suivante
	  line_aop = (T_resource_line*)line_aop->get_next() ;
	}
#ifdef DEBUG_RESOURCES
  TRACE1("<- Syntax analysis LINE %x", first_line_aop) ;
  EXIT_TRACE ;
#endif
}


// Fin de fichier
// _______________
