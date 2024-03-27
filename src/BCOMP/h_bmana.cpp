/******************************* CLEARSY **************************************
* Fichier : $Id: h_bmana.cpp,v 2.0 1999-03-15 11:49:19 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Dump HTML d'un Betree manager, avec construction de graphe
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
RCS_ID("$Id: h_bmana.cpp,v 1.5 1999-03-15 11:49:19 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Fonction auxilaire qui parcours les T_betree_list et cree les liens
// dans le fichier .gdl
static void browse_list(FILE *fd,
								 T_betree *cur_betree,
								 T_betree_list *ref_list,
								 const char *clause_name)
{
  T_list_link *cur = ref_list->get_links() ;
  // Nom composant
  const char *cur_name = cur_betree->get_betree_name()->get_value() ;

  // Path composant
  const char *cur_pathname = cur_betree->get_pathname()->get_value() ;

  while (cur != NULL)
	{
	  T_betree *src = (T_betree *)(cur->get_object()) ;
	  const char *src_name = src->get_betree_name()->get_value() ;
	  const char *src_pathname = src->get_pathname()->get_value() ;

	  s_fprintf(fd,
				"edge: { sourcename: \"%s (%s)\"\
targetname: \"%s (%s)\" color: %s }\n",
				src_name,
				src_pathname,
				cur_name,
				cur_pathname,
				clause_name) ;

	  cur = (T_list_link *)cur->get_next() ;
	}
}

// Creation d'un gif manager.gif representant le projet
void T_betree_manager::create_gif(void)
{
  TRACE1("T_betree_manager(%x)::create_gif", this) ;

  char *tmp = get_1Ko_string() ;
  sprintf(tmp,
		  "rm -f %s/manager.gdl %s/manager.gif %s/manager.ppm",
		  get_output_path(),
		  get_output_path(),
		  get_output_path()) ;

  system(tmp) ; // A FAIRE retour

  char *file_name = get_1Ko_string() ; // A FAIRE  ...
  sprintf(file_name, "%s/manager.gdl", get_output_path()) ;

  FILE *fd = fopen(file_name, "w") ;

  if (fd == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  s_fprintf(fd, " graph:\n {\n display_edge_labels: yes\n") ;
  s_fprintf(fd, " priority_phase: yes\n port_sharing: no\n") ;
  s_fprintf(fd, "layoutalgorithm: mindepth\n") ;

  // Creation des composants
  T_betree *cur_betree = first_betree ;

  while (cur_betree != NULL)
	{
	  T_used_machine *cur_context = cur_betree->get_root()->get_context() ;
	  s_fprintf(fd,
				"node: { title: \"%s (%s)\" } \n",
				cur_betree->get_betree_name()->get_value(),
				(cur_context == NULL)
				? cur_betree->get_betree_name()->get_value()
				: cur_context->get_pathname()->get_value()) ;
	  cur_betree = (T_betree *)cur_betree->get_next_betree() ;
	}

  // Liens REFINES
  cur_betree = first_betree ;

  while (cur_betree != NULL)
	{
	  T_machine *abst = cur_betree->get_root()->get_ref_abstraction() ;

	  if (abst != NULL)
		{
		  s_fprintf(fd,
					"edge: { sourcename: \"%s (%s)\" \
targetname: \"%s (%s)\" color: black } \n",
					cur_betree->get_betree_name()->get_value(),
					cur_betree->get_pathname()->get_value(),
					abst->get_betree()->get_betree_name()->get_value(),
					abst->get_betree()->get_pathname()->get_value()) ;
		}

	  browse_list(fd, cur_betree, cur_betree->get_seen_by(), 		"blue" ) ;
	  browse_list(fd, cur_betree, cur_betree->get_included_by(),	"red" ) ;
	  browse_list(fd, cur_betree, cur_betree->get_imported_by(),	"green" ) ;
	  browse_list(fd, cur_betree, cur_betree->get_extended_by(),	"darkgrey" ) ;
	  browse_list(fd, cur_betree, cur_betree->get_used_by(), 		"yellow" ) ;

	  cur_betree = (T_betree *)cur_betree->get_next_betree() ;
	}

  s_fprintf(fd, "}\n") ;
  fclose(fd) ;

  sprintf(tmp,
		  "vcg -tm 0cm -lm 0cm -scale 75 -silent -ppmoutput %s/manager.ppm %s/manager.gdl",
		  get_output_path(),
		  get_output_path()) ;
  TRACE1("%s", tmp) ;
  system(tmp) ;  // A FAIRE :: retour
  sprintf(tmp,
		  "ppmtogif %s/manager.ppm > %s/manager.gif 2> /dev/null",
		  get_output_path(),
		  get_output_path()) ;

  TRACE1("%s",tmp) ;
  system(tmp) ; // A FAIRE :: retour

}

//
//	}{	Classe T_betree_manager
//
FILE *T_betree_manager::dump_html(void)
{
  TRACE1("T_betree_manager(%x)::dump_html", this) ;
  FILE *fd = T_object::dump_html() ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is an instance of <FONT COLOR=\"red\">\
<kbd>%s</kbd></FONT></em></H1>\n",
                        this,
			get_class_name()) ;

  s_fprintf(fd, "rank=%u\n", (size_t)get_rank()) ;

  s_fprintf(fd, "<H2>Fields from class T_betree_manager :</H2>") ;

  s_fprintf(fd, "<LI>first_betree = %s</LI>\n", ptr_ref(first_betree)) ;
  s_fprintf(fd, "<LI>last_betree = %s</LI>\n", ptr_ref(last_betree)) ;

  s_fprintf(fd, "</UL>") ;

  s_fprintf(fd,
			"<TABLE BORDER=4 CELLPADDING=4><TR><TD>Direct access to Betrees :</TD><TD>Betree pathname</TD><TD>File Path</TD></TR>\n") ;

  T_betree *cur_betree = first_betree ;

  while (cur_betree != NULL)
	{
	  s_fprintf(fd,
                "<TR><TD><A HREF=\"%p.html\">%s</A></TD><TD>%s</TD>\
<TD>\"%s/%s\"</TD></TR>",
				(size_t)cur_betree,
				cur_betree->get_betree_name()->get_value(),
				cur_betree->get_pathname()->get_value(),
				(cur_betree->get_path() == NULL)
				? "." : cur_betree->get_path()->get_value(),
				cur_betree->get_file_name()->get_value()) ;
	  cur_betree = (T_betree *)cur_betree->get_next_betree() ;
	}

  s_fprintf(fd, "</TABLE>\n") ;

#ifndef WIN32
  s_fprintf(fd, "<P><HR><IMG SRC=\"manager.gif\"></P>\n") ;

  s_fprintf(fd, "<FONT COLOR=\"black\">REFINES</FONT>\n") ;
  s_fprintf(fd, "<FONT COLOR=\"blue\">SEES</FONT>\n") ;
  s_fprintf(fd, "<FONT COLOR=\"red\">INCLUDES</FONT>\n") ;
  s_fprintf(fd, "<FONT COLOR=\"green\">IMPORTS</FONT>\n") ;
  s_fprintf(fd, "<FONT COLOR=\"gray\">EXTENDS</FONT>\n") ;
  s_fprintf(fd, "<FONT COLOR=\"yellow\">USES</FONT>\n") ;
  // Fabrication du gif
  create_gif() ;
#endif

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

