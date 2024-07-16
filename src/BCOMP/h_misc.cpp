/******************************* CLEARSY **************************************
* Fichier : $Id: h_misc.cpp,v 2.0 2002-07-12 13:10:05 cm Exp $
* (C) 20008 CLEARSY
*
* Description :		Compilateur B
*					Dump HTML d'un Betree
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
RCS_ID("$Id: h_misc.cpp,v 1.44 2002-07-12 13:10:05 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_object (services de base)
//
FILE *T_object::dump_html(void)
{
  TRACE1("T_object::dump_html(%x)", this) ;

  char *file_name = get_tmp_string(get_output_path_len() + 7 + ADR_SIZE) ;
  //                    1  23456
  sprintf(file_name, "%s/%p.html", get_output_path(), this) ;

  struct stat stats_ls ;

  if (stat(file_name, &stats_ls) == 0)
	{
	  fprintf(stderr, "erreur : le fichier %s existe deja\n", file_name) ;
	  exit(1) ;
	}


  FILE *fd = fopen(file_name, "w") ;
  //  TRACE3("T_object(%x):fopen(%s)->%d", this, file_name, fd) ;

  if (fd == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  return fd ;
}

void T_object::close_html(FILE *fd)
{
  s_fprintf(fd, "<p><hr>") ;
  s_fprintf(fd, "</BODY>\n</HTML>\n") ;
  fclose(fd) ;
}

//
//	}{	Classe T_item
//
FILE *T_item::dump_html(void)
{
  TRACE2("T_item(%x)::dump_html(%x)", this, betree) ;
  ENTER_TRACE ; ENTER_TRACE ;
  FILE *fd = T_object::dump_html() ;
  EXIT_TRACE ; EXIT_TRACE ;

  s_fprintf(fd, "<HTML>\n<HEAD>\n") ;

  if (get_node_type() != NODE_BETREE)
	{
	  if (betree != NULL)
		{
		  if ( 	(object_test(betree) == TRUE)
				&& (((T_object *)betree)->get_node_type() == NODE_BETREE) )
			{
                          s_fprintf(fd, "<TITLE>%s::%s (0x%p)</TITLE>\n",
						(betree->get_betree_name()->get_value() == NULL) ?
						"NULL" : betree->get_betree_name()->get_value(),
						get_class_name(),
                                                this) ;
			}
		  else
			{
			  s_fprintf(fd,
						"<TITLE> ??? BETREE REFERENCE CORRUPTED ??? </TITLE>\n") ;
			  toplevel_error(CAN_CONTINUE,
							 "In instance %x of class %s : betree ref corrupted\n",
							 (size_t)this,
							 get_class_name()) ;
			}
		}
	  else
		{
                  s_fprintf(fd, "<TITLE>Toplevel::%s (0x%p)</TITLE>\n",
					get_class_name(),
                                        this) ;
		}


	}
  else
	{
	  // Cas particulier du betree
	  s_fprintf(fd,
				"<TITLE>B EXTENDED TREE FOR FILE %s</TITLE>\n",
				((T_betree *)this)->get_file_name()->get_value()) ;
	}


  s_fprintf(fd, "</HEAD>\n<BODY>\n") ;

  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4><TR>") ;
  s_fprintf(fd,
            "<TD><A HREF=%p.html>object_manager</A></TD>",
			(size_t)get_object_manager()) ;
  if (next == NULL)
	{
	  s_fprintf(fd, "<TD>next</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>next</A></TD>", (size_t)next) ;
	  chain_add(next) ;
	}

  if (prev == NULL)
	{
	  s_fprintf(fd, "<TD>prev</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>prev</A></TD>", (size_t)prev) ;
	  chain_add(prev) ;
	}

  if (father == NULL)
	{
	  s_fprintf(fd,
				"<TD><FONT COLOR=\"red\"><BLINK>father = NULL</BLINK></FONT></TD>");
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>father</A></TD>", (size_t)father) ;
	  chain_add(father) ;
	}

  if ( (get_tmp() != NULL) && (object_test(get_tmp(), TRUE) == TRUE) )
	{
	  s_fprintf(fd, "<TD>tmp=%s</A></TD>", ptr_ref(get_tmp())) ;
	}

  if ( (get_usr1() != NULL) && (object_test(get_usr1(), TRUE) == TRUE) )
	{
	  s_fprintf(fd, "<TD>usr1=%s</A></TD>", ptr_ref(get_usr1())) ;
	}

  if ( (get_usr2() != NULL) && (object_test(get_usr2(), TRUE) == TRUE) )
	{
	  s_fprintf(fd, "<TD>usr2=%s</A></TD>", ptr_ref(get_usr2())) ;
	}

  s_fprintf(fd, "<TD>rank=%u</TD>", (size_t)get_rank()) ;

  s_fprintf(fd, "<TD><A HREF=\"%p.html\">BETREE</A></TD>", (size_t)get_betree()) ;
  if (ref_lexem != NULL)
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>ref_lexem</A>", (size_t)ref_lexem) ;
	  chain_add(ref_lexem) ;
	}

  if (links <= 1)
	{
	  s_fprintf(fd, "<TD>%d link</TD>", links) ;
	}
  else
	{
	  s_fprintf(fd, "<TD>%d links</TD>", links) ;
	}

  s_fprintf(fd, "<TD>node type %d</TD>", get_node_type()) ;

  if (object_test(this) == FALSE)
	{
	  s_fprintf(fd, "<TD><FONT COLOR=\"red\">magic KO</FONT></TD>") ;
	}
  else
	{
	  s_fprintf(fd, "<TD>magic OK</TD>") ;
	}

  if (first_comment != NULL)
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>comments</A></TD>", (size_t)first_comment);
	  chain_add(first_comment) ;
	  chain_add(last_comment) ;
	}
  else
	{
	  s_fprintf(fd, "<TD>no comment</TD>") ;
	}

  if (first_pragma != NULL)
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>pragmas</A></TD>", (size_t)first_pragma) ;
	  chain_add(first_pragma) ;
	  chain_add(last_pragma) ;
	}

  if (get_betree() != NULL)
	{
	  s_fprintf(fd,
				"<TD><A HREF=%s>source</A></TD>",
				get_betree()->get_file_name()->get_value()) ;
	}

  s_fprintf(fd, "</TR></TABLE>\n") ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is an instance of <FONT COLOR=\"red\">\
<kbd>%s</kbd></FONT></em></H1>\n",
                        this,
			get_class_name()) ;

  return fd ;
}

//
//	}{	Classe T_lexem
//
FILE *T_lexem::dump_html(void)
{
  //TRACE1("T_lexem(%x)::dump_html", this) ;
  FILE *fd = T_object::dump_html() ;
  s_fprintf(fd, "<HTML>\n<HEAD>") ;

  s_fprintf(fd, "<TITLE>LEXEM::%s (0x%p)</TITLE>\n",
			get_lex_type_name(lex_type),
                        this) ;

  s_fprintf(fd, "</HEAD>\n<BODY>\n") ;

  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4><TR>") ;

  s_fprintf(fd,
            "<TD><A HREF=%p.html>object_manager</A></TD>",
			(size_t)get_object_manager()) ;
  if (next_lexem == NULL)
	{
	  s_fprintf(fd, "<TD>next_lexem</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>next_lexem</A></TD>", (size_t)next_lexem) ;
	}

  if (prev_lexem == NULL)
	{
	  s_fprintf(fd, "<TD>prev_lexem</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>prev_lexem</A></TD>", (size_t)prev_lexem) ;
	}

  s_fprintf(fd, "<TD>rank=%u</TD>", (size_t)get_rank()) ;

  s_fprintf(fd, "\n") ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is a <FONT COLOR=\"red\">",
                        this) ;

  s_fprintf(fd,
			"%s</FONT> lexem </em> (%s)</H1>\n",
			get_lex_type_name(lex_type),
			(lex_type == L_IDENT) ? value : get_lex_type_ascii(lex_type)) ;

  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd,
			"<LI>value = \"%s\", value_len = %d</LI>\n",
			str_ref(value),
			value_len) ;

  s_fprintf(fd, "<LI>file_name = %s</LI>\n", ptr_ref(get_file_name())) ;

  s_fprintf(fd, "<LI>file_line = %d, file_column = %d</LI>\n", file_line, file_column) ;


  s_fprintf(fd, "<LI>next_lexem = %s, \n", ptr_ref(next_lexem)) ;
  s_fprintf(fd, "prev_lexem = %s</LI>\n", ptr_ref(prev_lexem)) ;

  s_fprintf(fd, "<LI>next_expanded_lexem = %s, \n", ptr_ref(next_expanded_lexem)) ;
  s_fprintf(fd, "prev_expanded_lexem = %s</LI>\n", ptr_ref(prev_expanded_lexem)) ;

  s_fprintf(fd, "<LI>original_lexem = %s</LI>\n", ptr_ref(original_lexem)) ;
  s_fprintf(fd, "<LI>rewrite_rule_lexem = %s</LI>\n", ptr_ref(rewrite_rule_lexem)) ;

  s_fprintf(fd, "<LI>next_line = %s, \n", ptr_ref(next_line)) ;
  s_fprintf(fd, "prev_line = %s</LI>\n", ptr_ref(prev_line)) ;

  s_fprintf(fd, "<LI>expanded = %s</LI>\n", bool_ref(expanded)) ;
  s_fprintf(fd, "<LI>start_of_line = %s</LI>\n", ptr_ref(start_of_line)) ;
  s_fprintf(fd, "<LI>position = %d</LI>\n", position) ;

  s_fprintf(fd, "</UL>\n") ;

  // Fermeture du fichier HTML
  close_html(fd) ;
  return NULL ;
}

//
//	}{	Classe T_pragma_lexem
//
FILE *T_pragma_lexem::dump_html(void)
{
  //TRACE1("T_pragma_lexem(%x)::dump_html", this) ;
  FILE *fd = T_object::dump_html() ;
  s_fprintf(fd, "<HTML>\n<HEAD>") ;

  s_fprintf(fd, "<TITLE>PRAGMA_LEXEM::%s (0x%p)</TITLE>\n",
			get_pragma_lex_type_name(lex_type),
                        this) ;

  s_fprintf(fd, "</HEAD>\n<BODY>\n") ;

  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4><TR>") ;

  s_fprintf(fd,
            "<TD><A HREF=%p.html>object_manager</A></TD>",
			(size_t)get_object_manager()) ;
  if (next_lexem == NULL)
	{
	  s_fprintf(fd, "<TD>next_lexem</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>next_lexem</A></TD>", (size_t)next_lexem) ;
	}

  if (prev_lexem == NULL)
	{
	  s_fprintf(fd, "<TD>prev_lexem</TD>") ;
	}
  else
	{
      s_fprintf(fd, "<TD><A HREF=%p.html>prev_lexem</A></TD>", (size_t)prev_lexem) ;
	}

  s_fprintf(fd, "<TD>rank=%u</TD>", (size_t)get_rank()) ;

  s_fprintf(fd, "\n") ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is a <FONT COLOR=\"red\">",
                        this) ;

  s_fprintf(fd,
			"%s</FONT> pragma_lexem </em> </H1>\n",
			get_pragma_lex_type_name(lex_type)) ;

  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd,
			"<LI>value = \"%s\", value_len = %d</LI>\n",
			str_ref(value),
			value_len) ;

  s_fprintf(fd,
			"<LI>file_line = %d, file_column = %d</LI>\n",
			file_line, file_column) ;


  s_fprintf(fd, "<LI>next_lexem = %s, \n", ptr_ref(next_lexem)) ;
  s_fprintf(fd, "prev_lexem = %s</LI>\n", ptr_ref(prev_lexem)) ;

  s_fprintf(fd, "</UL>\n") ;

  // Fermeture du fichier HTML
  close_html(fd) ;
  return NULL ;
}

//
//	}{	Classe T_betree
//
static const char *const debug_status_sct[] =
{
  "BST_PARSING",			// Betree en cours de construction
  "BST_SYNTAXIC",			// Betree ayant passe l'etape d'analyse syntaxique
  "BST_SEMANTIC",			// Betree ayant passe l'etape d'analyse semantique
  "BST_B0_CHECKED",			// Betree ayant passe l'etape de B0 Check
  "BST_DEPENDENCIES",  		// Betree de dependances
} ;

FILE *T_betree::dump_html(void)
{
  TRACE1("ici %x", this) ;
  T_object *item ;
  TRACE2("T_betree(%x \"%s\")::dump_html",
		 this,
		 get_root()->get_machine_name()->get_name()->get_value()) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd,
			"<H2>This is the betree for component %s (pathname %s)</H2>",
			betree_name->get_value(),
			get_pathname()->get_value()) ;

  s_fprintf(fd, "<H2>Fields from class T_betree :</H2>") ;

  if (root != NULL)
	{
	  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4>\n") ;
	  s_fprintf(fd, "<TR>") ;

      s_fprintf(fd, "<TD><A HREF=%p.html>root</A></TD>", (size_t)root) ;
      s_fprintf(fd, "<TD><A HREF=%p.html>manager</A></TD>", (size_t)manager) ;
	  if (prev_betree != NULL)
		{
		  s_fprintf(fd,
                    "<TD><A HREF=%p.html>prev_betree</A></TD>",
					(size_t)prev_betree) ;
		  chain_add(prev_betree) ;
		}
	  else
		{
		  s_fprintf(fd, "<TD>prev_betree</TD>") ;
		}
	  if (next_betree != NULL)
		{
		  s_fprintf(fd,
                    "<TD><A HREF=%p.html>next_betree</A></TD>",
					(size_t)next_betree) ;
		  chain_add(next_betree) ;
		}
	  else
		{
		  s_fprintf(fd, "<TD>next_betree</TD>") ;
		}
	  if (get_root()->get_context() != NULL)
		{
		  s_fprintf(fd,
                    "<TD><A HREF=%p.html>context</A></TD>",
					get_root()->get_context()) ;
		}
	  chain_add(root) ;
	  if (manager != NULL)
		{
		  chain_add(manager) ;
		}

	  // Acces rapide aux operations
	  T_op *cur_op = root->get_operations() ;
	  while (cur_op != NULL)
		{
		  if (	(object_test(cur_op) == TRUE)
				&& (cur_op->get_node_type() == NODE_OPERATION) )
			{
              s_fprintf(fd, "<TD><A HREF=%p.html>%s</A></TD>",
						(size_t)cur_op,
						cur_op->get_name()->get_name()->get_value()) ;
			  cur_op = (T_op *)cur_op->get_next() ;
			}
		  else
			{
			  cur_op = NULL ;
			}
		}

	  // Acces rapide aux clauses
#define ADD_TABLE(x, y) \
	  if (root->x != NULL) \
		{\
           s_fprintf(fd, "<TD><A HREF=%p.html>%s</A></TD>", (size_t)root->x, y) ;\
		}\

		ADD_TABLE(get_params(),		 	   	"FORMAL PARAMETERS") ;
		ADD_TABLE(get_constraints(), 	   	"CONSTRAINTS") ;
		ADD_TABLE(get_sees(), 				"SEES") ;
		ADD_TABLE(get_includes(), 			"INCLUDES") ;
		ADD_TABLE(get_imports(), 			"IMPORTS") ;
		ADD_TABLE(get_promotes(), 			"PROMOTES") ;
		ADD_TABLE(get_extends(), 		   	"EXTENDS") ;
		ADD_TABLE(get_uses(), 				"USES") ;
		ADD_TABLE(get_sets(), 				"SETS") ;
		ADD_TABLE(get_concrete_variables(), 			"CONCRETE_VARIABLES") ;
		ADD_TABLE(get_abstract_variables(), 	"ABSTRACT_VARIABLES") ;
		ADD_TABLE(get_concrete_constants(), 			"CONCRETE_CONSTANTS") ;
		ADD_TABLE(get_abstract_constants(), 	"ABSTRACT_CONSTANTS") ;
		ADD_TABLE(get_properties(), "		PROPERTIES") ;
		ADD_TABLE(get_invariant(), 			"INVARIANT") ;
		ADD_TABLE(get_variant(), 			"VARIANT") ;
		ADD_TABLE(get_assertions(), 		"ASSERTIONS") ;
		ADD_TABLE(get_initialisation(), 	"INITIALISATION") ;
		ADD_TABLE(get_values(), 			"VALUES") ;
		ADD_TABLE(get_op_list(), 			"OP_LIST") ;

		s_fprintf(fd, "</TR></TABLE>\n") ;
	}

  s_fprintf(fd, "<UL>\n") ;

  //TRACE1("sizeof(tableau) = %d", sizeof(debug_status_sct)) ;
  size_t table_size = sizeof(debug_status_sct) / sizeof(char *) ;

  if ( (status < 0) || ((size_t)status >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>status = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				status,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "status = %d is out of bounds (0 .. %d)\n",
					 status,
					 table_size - 1) ;
	}
  else
	{
	  s_fprintf(fd,
				"<LI>status = <FONT color=\"red\">%s</FONT>\n",
				debug_status_sct[status]) ;
	}


  s_fprintf(fd, "<LI>file_name = %s</LI>\n", sym_ref(file_name)) ;
  s_fprintf(fd, "<LI>path = %s</LI>\n", sym_ref(path)) ;
  s_fprintf(fd, "<LI>betree_name = %s</LI>\n", sym_ref(betree_name)) ;
  s_fprintf(fd, "<LI>checksum = %s, ", sym_ref(checksum)) ;
  s_fprintf(fd, "revision_ID = %s\n</LI>", str_ref(revision_ID)) ;

  s_fprintf(fd, "<LI>integrity_check_done = %s, ", bool_ref(integrity_check_done)) ;
  s_fprintf(fd, "<LI>first_lexem = %s\n</LI>", ptr_ref(first_lexem)) ;
  s_fprintf(fd, "<LI>last_lexem = %s\n</LI>", ptr_ref(last_lexem)) ;
  s_fprintf(fd, "<LI>last_code_lexem = %s\n</LI>", ptr_ref(last_code_lexem)) ;
  s_fprintf(fd, "<LI>definitions_clause = %s\n</LI>", ptr_ref(definitions_clause)) ;
  s_fprintf(fd, "<LI>first_definition = %s, ", ptr_ref(first_definition)) ;
  s_fprintf(fd, "last_definition = %s\n</LI>", ptr_ref(last_definition)) ;
  s_fprintf(fd, "<LI>first_file_definition = %s, ", ptr_ref(first_file_definition));
  s_fprintf(fd, "last_file_definition = %s\n</LI>", ptr_ref(last_file_definition)) ;

  s_fprintf(fd, "</UL>\n") ;
  s_fprintf(fd, "<HR>\n") ;

  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4>\n") ;
  s_fprintf(fd, "<TR>") ;

#define ADD_LINK(x, y) \
  s_fprintf(fd, "<TD>") ;\
  if (x != NULL)\
	{\
	   if ((((T_betree_list *)x)->get_links()) != NULL) \
	  {\
		 s_fprintf(fd, "<FONT COLOR=\"purple\">%s</FONT>", y) ;\
	  }\
	   else\
		 {\
			s_fprintf(fd, "%s", y) ;\
		 }\
    s_fprintf(fd, " <A HREF=%p.html>BY</A>", (size_t)x) ;\
	chain_add(x) ;\
	if ((((T_betree_list *)x)->get_links()) != NULL) \
	  {\
		 s_fprintf(fd, "</FONT>") ;\
	  }\
	}\
  else \
	{ \
		s_fprintf(fd, "%s", y) ;\
	} \
	s_fprintf(fd, "</TD>") ;

	ADD_LINK(seen_by, 	  "SEEN") ;
	ADD_LINK(included_by, "INCLUDED") ;
	ADD_LINK(imported_by, "IMPORTED") ;
	ADD_LINK(extended_by, "EXTENDED") ;
	ADD_LINK(used_by, 	  "USED") ;

	s_fprintf(fd, "</TR></TABLE>\n") ;

	// Dump des elements restants
	while ( (item = chain_get()) != NULL)
	  {
		TRACE1("get->%x", item) ;
#ifdef FULL_TRACE
		TRACE2("get->%x:%s", item, item->get_class_name()) ;
#endif

		if (object_test(item) == TRUE)
		  {
			item->dump_html() ;
		  }
	  }

	// Fermeture de la page principale
	close_html(fd) ;

	return NULL ;
}

//
//	}{	Classe T_keyword
//
FILE *T_keyword::dump_html(void)
{
  //TRACE1("T_keyword(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_symbol::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_keyword :</H2>") ;
  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd, "<LI>lex_type = %d ( lexem <FONT COLOR=\"red\">%s </FONT>, ",
			lex_type,
			get_lex_type_name(lex_type)) ;

  s_fprintf(fd, "ASCII <FONT COLOR=\"green\">%s</FONT>)",
			get_lex_type_ascii(lex_type)) ;

  s_fprintf(fd, "</UL>\n") ;

  //  fclose(fd) ;
  return NULL ;
}

//
//	}{	Classe T_comment
//
FILE *T_comment::dump_html(void)
{
  //TRACE1("T_comment(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_comment :</H2><UL>") ;

  s_fprintf(fd,
            "<LI>comment = %p.html<FONT COLOR=\"red\"><pre>\"%s\"</pre></FONT>\n",
			(size_t)comment,
			comment->get_value()) ;
  chain_add(comment) ;
  s_fprintf(fd, "<LI>keep_comment = %s</LI>\n", bool_ref(keep_comment)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</UL>\n", ptr_ref(ref_machine)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_definition
//
FILE *T_definition::dump_html(void)
{
  //TRACE1("T_definition(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_definition :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>name = %s\n", sym_ref(name)) ;

  s_fprintf(fd, "<LI>first_param = %s\n", ptr_ref(first_param)) ;
  s_fprintf(fd, "<LI>last_param = %s\n", ptr_ref(last_param)) ;

  s_fprintf(fd, "<LI>first_rewrite_rule = %s\n", ptr_ref(first_rewrite_rule)) ;
  s_fprintf(fd, "<LI>last_rewrite_rule = %s</LI>\n", ptr_ref(last_rewrite_rule)) ;
  s_fprintf(fd, "<LI>ref_scol_lexem = %s</LI></UL>\n", ptr_ref(ref_scol_lexem)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_file_definition
//
FILE *T_file_definition::dump_html(void)
{
  //TRACE1("T_file_definition(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_file_definition :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>name = %s\n", sym_ref(name)) ;
  s_fprintf(fd, "<LI>path = %s\n", sym_ref(path)) ;

  s_fprintf(fd, "<LI>checksum = %s</LI></UL>\n", ptr_ref(checksum)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	classe T_flag : dump HTML
//
FILE *T_flag::dump_html(void)
{
  //TRACE1("T_flag(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Class T_flag does not have any specific field</H2>") ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_list_link
//
static const char *debug_list_link_type_sct[] =
{
  /* 00 */
  "IN",
  /* 01 */
  "OUT",
  /* 02 */
  "CASE_SELECT",
  /* 03 */
  "ARRAY_RANGE",
  /* 04 */
  "CONSTANT_VALUE",
  /* 05 */
  "DEFINITION",
  /* 06 */
  "BUILTIN",
  /* 07 */
  "OP_LIST",
  /* 08 */
  "BETREE_LIST",
  /* 09 */
  "BINOP_OPER_LIST",
  /* 10 */
  "RELATION_SRC_SET_LIST",
  /* 11 */
  "RELATION_DST_SET_LIST",
  /* 12 */
  "TYPE_LIST",
  /* 13 */
  "DOLLAR_ZERO",
  /* 14 */
  "READ_SYMBOL",
  /* 15 */
  "OTHER",
} ;

FILE *T_list_link::dump_html(void)
{
  //TRACE1("T_list_link(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_list_link : </H2><UL>") ;

  s_fprintf(fd, "<LI>object = %s\n", ptr_ref(object)) ;

  //TRACE1("sizeof(tableau) = %d", sizeof(debug_list_link_type_sct)) ;
  size_t table_size = sizeof(debug_list_link_type_sct) / sizeof(char *) ;

  if ( (list_link_type < 0) || ((size_t)list_link_type >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>list_link_type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				list_link_type,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "list_link_type = %d is out of bounds (0 .. %d)\n",
					 list_link_type,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd,
				"<LI>list_link_type = %s</UL>\n",
				debug_list_link_type_sct[list_link_type]) ;
	}

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_machine
//
static const char *machine_type_name_sct[] =
{
  "SPECIFICATION",
  "REFINEMENT",
  "IMPLEMENTATION"
} ;

FILE *T_machine::dump_html(void)
{
  TRACE1("T_machine(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_machine :</H2>") ;
  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4>\n") ;
  s_fprintf(fd, "<TR>") ;

  // Acces rapide aux operations
  T_op *cur_op = get_operations() ;
  while (cur_op != NULL)
	{
	  if (	(object_test(cur_op) == TRUE)
			&& (cur_op->get_node_type() == NODE_OPERATION) )
		{
          s_fprintf(fd, "<TD><A HREF=%p.html>%s</A></TD>",
					(size_t)cur_op,
					cur_op->get_name()->get_name()->get_value()) ;
		  cur_op = (T_op *)cur_op->get_next() ;
		}
	  else
		{
		  cur_op = NULL ;
		}
	}

#define ADD_TABLE2(x, y) \
  if (x != NULL) \
	{\
       s_fprintf(fd, "<TD><A HREF=%p.html>%s</A></TD>", (size_t)x, y) ;\
	}\

	// Acces rapide aux clauses
	ADD_TABLE2(get_params(),		 	 "FORMAL PARAMETERS") ;
	ADD_TABLE2(get_global_pragmas(),	 "GLOBAL PRAGMAS") ;
	ADD_TABLE2(get_constraints(), 		 "CONSTRAINTS") ;
	ADD_TABLE2(get_sees(), 			   	 "SEES") ;
	ADD_TABLE2(get_includes(), 			 "INCLUDES") ;
	ADD_TABLE2(get_imports(), 			 "IMPORTS") ;
	ADD_TABLE2(get_promotes(), 			 "PROMOTES") ;
	ADD_TABLE2(get_extends(), 			 "EXTENDS") ;
	ADD_TABLE2(get_uses(), 				 "USES") ;
	ADD_TABLE2(get_sets(), 				 "SETS") ;
	ADD_TABLE2(get_concrete_variables(), "CONCRETE_VARIABLES") ;
	ADD_TABLE2(get_abstract_variables(), "ABSTRACT_VARIABLES") ;
	ADD_TABLE2(get_concrete_constants(), "CONCRETE_CONSTANTS") ;
	ADD_TABLE2(get_abstract_constants(), "ABSTRACT_CONSTANTS") ;
	ADD_TABLE2(get_properties(), 		 "PROPERTIES") ;
	ADD_TABLE2(get_invariant(), 		 "INVARIANT") ;
	ADD_TABLE2(get_variant(), 		 "VARIANT") ;
	ADD_TABLE2(get_assertions(), 		 "ASSERTIONS") ;
	ADD_TABLE2(get_initialisation(), 	 "INITIALISATION") ;
	ADD_TABLE2(get_values(), 			 "VALUES") ;

	s_fprintf(fd, "</TR></TABLE>\n") ;

	s_fprintf(fd, "<LI>machine_name = %s, ", ptr_ref(machine_name)) ;
	s_fprintf(fd, "machine_type = %s</LI>", machine_type_name_sct[machine_type]) ;
	s_fprintf(fd, "<LI>machine_clause = %s</LI>", ptr_ref(machine_clause)) ;
	s_fprintf(fd, "<LI>refines_clause = %s</LI>", ptr_ref(refines_clause)) ;
	s_fprintf(fd, "<LI>abstraction_name = %s</LI>", ptr_ref(abstraction_name)) ;
	s_fprintf(fd, "<LI>ref_abstraction = %s, ", ptr_ref(ref_abstraction)) ;
	s_fprintf(fd, "ref_specification = %s, ", ptr_ref(ref_specification)) ;
	s_fprintf(fd, "ref_refinement = %s</LI>", ptr_ref(ref_refinement)) ;

	s_fprintf(fd, "<LI>first_param = %s, \n", ptr_ref(first_param)) ;
	s_fprintf(fd,
			  "last_param = %s, checksum %s</LI>\n",
			  ptr_ref(last_param),
			  sym_ref(formal_params_checksum)) ;

	s_fprintf(fd, "<LI>first_global_pragma = %s, \n", ptr_ref(first_global_pragma));
	s_fprintf(fd, "last_global_pragma = %s</LI>\n", ptr_ref(last_global_pragma)) ;
	s_fprintf(fd, "<LI>context = %s</LI>\n", ptr_ref(context)) ;


	s_fprintf(fd, "<LI><TABLE BORDER=5 CELLPADDING=5>\n") ;
	s_fprintf(fd,
			  "<TR> <TH>Name</TH><TH>Clause</TH><TH>First</TH><TH>Last</TH> <TH>Checksum</TH></TR>\n") ;
	s_fprintf(fd,
			  "<TR> <TD>sees</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(sees_clause),
			  ptr_ref(first_sees),
			  ptr_ref(last_sees),
			  ptr_ref(sees_clause_checksum)) ;
	s_fprintf(fd,
			  "<TR> <TD>includes</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(includes_clause),
			  ptr_ref(first_includes),
			  ptr_ref(last_includes),
			  ptr_ref(includes_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>imports</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(imports_clause),
			  ptr_ref(first_imports),
			  ptr_ref(last_imports),
			  ptr_ref(imports_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>promotes</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(promotes_clause),
			  ptr_ref(first_promotes),
			  ptr_ref(last_promotes),
			  ptr_ref(promotes_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>extends</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(extends_clause),
			  ptr_ref(first_extends),
			  ptr_ref(last_extends),
			  ptr_ref(extends_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>uses</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(uses_clause),
			  ptr_ref(first_uses),
			  ptr_ref(last_uses),
			  ptr_ref(uses_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>sets</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(sets_clause),
			  ptr_ref(first_set),
			  ptr_ref(last_set),
			  ptr_ref(sets_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>concrete_variables</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(concrete_variables_clause),
			  ptr_ref(first_concrete_variable),
			  ptr_ref(last_concrete_variable),
			  ptr_ref(concrete_variables_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>abstract_variables</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(abstract_variables_clause),
			  ptr_ref(first_abstract_variable),
			  ptr_ref(last_abstract_variable),
			  ptr_ref(abstract_variables_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>concrete_constants</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(concrete_constants_clause),
			  ptr_ref(first_concrete_constant),
			  ptr_ref(last_concrete_constant),
			  ptr_ref(concrete_constants_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>abstract_constants</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(abstract_constants_clause),
			  ptr_ref(first_abstract_constant),
			  ptr_ref(last_abstract_constant),
			  ptr_ref(abstract_constants_clause_checksum)) ;


	s_fprintf(fd,
			  "<TR> <TD>constraints</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(constraints_clause),
			  ptr_ref(constraints),
			  ptr_ref(constraints_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>initialisation</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>initialisation_clause_operation=%s</TD> </TR>\n",
			  ptr_ref(initialisation_clause),
			  ptr_ref(first_initialisation),
			  ptr_ref(last_initialisation),
			  ptr_ref(initialisation_clause_operation)) ;

	s_fprintf(fd,
			  "<TR> <TD>values</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(values_clause),
			  ptr_ref(first_value),
			  ptr_ref(last_value),
			  ptr_ref(values_clause_checksum)) ;

	s_fprintf(fd,
			  "<TR> <TD>operations</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(operations_clause),
			  ptr_ref(first_operation),
			  ptr_ref(last_operation)) ;

	s_fprintf(fd,
			  "<TR> <TD>local_operations</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(local_operations_clause),
			  ptr_ref(first_local_operation),
			  ptr_ref(last_local_operation)) ;

	s_fprintf(fd,
			  "<TR> <TD>end_machine</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD></TR>\n",
			  ptr_ref(end_machine),
			  ptr_ref(first_end_comment),
			  ptr_ref(last_end_comment)) ;
	TRACE4("COMMENT %x:%x, %x:%x",
		   &first_end_comment,
		   first_end_comment,
		   &last_end_comment,
		   last_end_comment) ;

	s_fprintf(fd,
			  "<TR> <TD>properties</TD><TD>%s</TD><TD>%s</TD></TR>\n",
			  ptr_ref(properties_clause),
			  ptr_ref(properties)) ;

	s_fprintf(fd,
			  "<TR> <TD>invariant</TD><TD>%s</TD><TD>%s</TD></TR>\n",
			  ptr_ref(invariant_clause),
			  ptr_ref(invariant)) ;

	s_fprintf(fd,
			  "<TR> <TD>variant</TD><TD>%s</TD><TD>%s</TD></TR>\n",
			  ptr_ref(variant_clause),
			  ptr_ref(variant)) ;

	s_fprintf(fd,
			  "<TR> <TD>assertions</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD> </TR>\n",
			  ptr_ref(assertions_clause),
			  ptr_ref(first_assertion),
			  ptr_ref(last_assertion)) ;

	s_fprintf(fd, "</TABLE></LI>\n") ;


	s_fprintf(fd,
			  "<LI>initialisation_clause_ident %s, initialisation_clause_op %s</LI>\n",
			  ptr_ref(initialisation_clause_ident),
			  ptr_ref(initialisation_clause_operation)) ;
	s_fprintf(fd,
			  "<LI>first_op_list = %s, last_op_list = %s (SEMANTIC)\n",
			  ptr_ref(first_op_list),
			  ptr_ref(last_op_list)) ;

	s_fprintf(fd,
			  "<LI>first_dollar_zero_ident = %s, last_dollar_zero_ident = %s (SEMANTIC)\n",
			  ptr_ref(first_dollar_zero_ident),
			  ptr_ref(last_dollar_zero_ident)) ;

	s_fprintf(fd, "</UL>\n") ;

	// Fermeture du fichier HTML
	close_html(fd) ;

	return NULL ;
}

//
//	}{	Classe T_op
//
FILE *T_op::dump_html(void)
{
  TRACE1("T_op(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_generic_op::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_op :</H2>") ;
  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd, "<LI>op_name = %s</LI>\n", ptr_ref(name)) ;

  s_fprintf(fd, "<LI>first_in_param = %s, \n", ptr_ref(first_in_param)) ;
  s_fprintf(fd, "last_in_param = %s</LI>\n", ptr_ref(last_in_param)) ;

  s_fprintf(fd, "<LI>first_out_param = %s, \n", ptr_ref(first_out_param)) ;
  s_fprintf(fd, "last_out_param = %s</LI>\n", ptr_ref(last_out_param)) ;

  s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
  s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;

  s_fprintf(fd, "<LI>precond_checksum = %s\n", ptr_ref(precond_checksum)) ;
  s_fprintf(fd, "<LI>body_checksum = %s\n", ptr_ref(body_checksum)) ;
  s_fprintf(fd, "<LI>is_a_local_op = %s\n", bool_ref(is_a_local_op)) ;
  s_fprintf(fd, "<LI>is_inlined = %s\n", bool_ref(is_inlined)) ;

  s_fprintf(fd, "</UL>\n") ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_relation : methode de dump HTML
//
static const char *debug_rtype_name_sct[] =
{
  /* 00 '-->' */	"RTYPE_TOTAL_FUNCTION",
					/* 01 '>->' */	"RTYPE_TOTAL_INJECTION",
					/* 02 '-->>'*/	"RTYPE_TOTAL_SURJECTION",
					/* 03 '+->' */	"RTYPE_PARTIAL_FUNCTION",
					/* 04 '>+>' */	"RTYPE_PARTIAL_INJECTION",
					/* 05 '+->>'*/	"RTYPE_PARTIAL_SURJECTION",
					/* 06 '>->>'*/	"RTYPE_BIJECTION"
} ;

FILE *T_relation::dump_html(void)
{
  //TRACE1("T_relation(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_relation :</H2>") ;

  s_fprintf(fd, "<UL><LI>src_set = %s\n", ptr_ref(src_set)) ;
  s_fprintf(fd, "<LI>dst_set = %s\n", ptr_ref(dst_set)) ;
  s_fprintf(fd, "<LI>first_set = %s\n", ptr_ref(first_set)) ;
  s_fprintf(fd, "<LI>last_set = %s\n", ptr_ref(last_set)) ;
  s_fprintf(fd, "<LI>nb_sets = %d\n", nb_sets) ;

  size_t table_size = sizeof(debug_rtype_name_sct) / sizeof(char *) ;

  if ( (relation_type  < 0) || ((size_t)relation_type >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>relation_type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				relation_type,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "relation_type = %d is out of bounds (0 .. %d)\n",
					 relation_type,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd,
				"<LI>relation_type = %s</UL>\n",
				debug_rtype_name_sct[relation_type]) ;
	}

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_symbol
//
FILE *T_symbol::dump_html(void)
{
  //TRACE1("T_symbol(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_object::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_symbol :</H2>") ;
  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd, "<LI>value = %s, ", str_ref(value)) ;
  s_fprintf(fd, "len = %u</LI>\n", len) ;
  s_fprintf(fd, "<LI>next_symb = %s</LI>\n", ptr_ref(next_symb)) ;

  s_fprintf(fd, "</UL>\n") ;

  if (get_node_type() == NODE_SYMBOL)
	{
	  // Fermeture du fichier HTML
	  close_html(fd) ;
	  return NULL ;
	}
  else
	{
	  // Les sous-classes s'en chargeront
	  return fd ;
	}
}

//
//	}{	Classe T_used_machine
//
static const char *use_type_name_sct[] =
{
  "USE_SEES",
  "USE_INCLUDES",
  "USE_EXTENDS",
  "USE_USES",
  "USE_IMPORTS",
} ;

FILE *T_used_machine::dump_html(void)
{
  //TRACE1("T_used_machine(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_used_machine :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>instance_name = %s, \n", sym_ref(instance_name)) ;
  s_fprintf(fd, "component_name = %s</LI>\n", sym_ref(component_name)) ;
  s_fprintf(fd, "<LI>pathname = %s</LI>\n", sym_ref(pathname)) ;

  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "<LI>first_use_param = %s</LI>\n", ptr_ref(first_use_param)) ;
  s_fprintf(fd, "<LI>last_use_param = %s</LI>\n", ptr_ref(last_use_param)) ;

  s_fprintf(fd, "<LI>use_type = %s</UL>\n", use_type_name_sct[use_type]) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_valuation - methode de dump HTML
//
FILE *T_valuation::dump_html(void)
{
  //TRACE1("T_valuation(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_valuation :</H2><UL>") ;

  s_fprintf(fd, "<LI>ident = %s\n", ptr_ref(ident)) ;
  s_fprintf(fd, "<LI>value = %s</UL>\n", ptr_ref(value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_used_op
//
FILE *T_generic_op::dump_html(void)
{
  TRACE1("T_used_op(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Virtual methods from class T_generic_op :</H2>") ;

  s_fprintf(fd, "<UL><LI>get_op_name = %s</LI>\n", sym_ref(get_op_name())) ;
  s_fprintf(fd, "<LI>get_name = %s</LI></UL>\n", ptr_ref(get_name())) ;

  return fd ;
}

//
//	}{	Classe T_used_op
//
FILE *T_used_op::dump_html(void)
{
  //TRACE1("T_used_op(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_generic_op::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_used_op :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>instance_name = %s, \n", sym_ref(instance_name)) ;
  s_fprintf(fd, "real_op_name = %s</LI>\n", sym_ref(real_op_name)) ;

  s_fprintf(fd, "<LI>ref_op = %s</LI>\n", ptr_ref(ref_op)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_betree_list
//
FILE *T_betree_list::dump_html(void)
{
  //TRACE1("T_betree_list(%x)::dump_html", this) ;
  FILE *fd = T_object::dump_html() ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is an instance of <FONT COLOR=\"red\">\
<kbd>%s</kbd></FONT></em></H1>\n",
                        this,
			get_class_name()) ;

  s_fprintf(fd, "<H2>Fields from class T_betree_list :</H2>") ;

  s_fprintf(fd, "<LI>first_link = %s</LI>\n", ptr_ref(first_link)) ;
  s_fprintf(fd, "<LI>last_link = %s</LI>\n", ptr_ref(last_link)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}



//
//	}{	Classe T_object_manager (gestionnaire d'objets)
//
FILE *T_object_manager::dump_html(void)
{
  TRACE1("T_object_manager::dump_html(%x)", this) ;

  char *file_name = get_tmp_string(get_output_path_len() + 7 + ADR_SIZE) ;
  //                    1  23456
  sprintf(file_name, "%s/%p.html", get_output_path(), this) ;

  FILE *fd = fopen(file_name, "w") ;
  TRACE3("T_object_manager(%x):fopen(%s)->%d", this, file_name, fd) ;

  if (fd == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  s_fprintf(fd, "<HTML>\n<HEAD>\n") ;

  s_fprintf(fd, "<TITLE>Object Manager</TITLE>\n") ;
  s_fprintf(fd, "</HEAD>\n<BODY>\n") ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is an instance of \
<FONT COLOR=\"red\"><kbd>T_object_manager</kbd></FONT></em></H1>\n",
                        this) ;

  s_fprintf(fd, "<H2>Fields from class T_object_manager :</H2>") ;

  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd,
            "<LI>first_tag = <A HREF=\"%p.html\">%x</A></LI>\n",
			(size_t)first_tag) ;
  s_fprintf(fd,
            "<LI>last_tag = <A HREF=\"%p.html\">%x</A></LI>\n",
			(size_t)last_tag) ;

#if 0
  // On ne met pas les tags dans le cas general, car ca genere
  // beaucoup trop de fichiers !
  T_object_tag *cur_tag = first_tag ;

  while (cur_tag != NULL)
	{
	  cur_tag->dump_html() ;
	  cur_tag = cur_tag->get_next_tag() ;
	}
#endif

  s_fprintf(fd, "<LI>first_ident = %s</LI>\n",  ptr_ref(first_ident)) ;
  s_fprintf(fd, "<LI>last_ident = %s</LI>\n",  ptr_ref(last_ident)) ;

  s_fprintf(fd,
			"<LI>first_unsolved_ident = %s</LI>\n",
			ptr_ref(first_unsolved_ident)) ;
  s_fprintf(fd,
			"<LI>last_unsolved_ident = %s</LI>\n",
			ptr_ref(last_unsolved_ident)) ;

  s_fprintf(fd,
			"<LI>first_op_result = %s</LI>\n",
			ptr_ref(first_op_result)) ;
  s_fprintf(fd,
			"<LI>last_op_result = %s</LI>\n",
			ptr_ref(last_op_result)) ;

  s_fprintf(fd,
			"<LI>first_unsolved_comment = %s</LI>\n",
			ptr_ref(first_unsolved_comment)) ;
  s_fprintf(fd,
			"<LI>last_unsolved_comment = %s</LI>\n",
			ptr_ref(last_unsolved_comment)) ;


  s_fprintf(fd,
			"<LI>first_abstract_type = %s</LI>\n",
			ptr_ref(first_abstract_type)) ;
  s_fprintf(fd,
			"<LI>last_abstract_type = %s</LI>\n",
			ptr_ref(last_abstract_type)) ;

  s_fprintf(fd, "<LI>first_lexem = %s</LI>\n",  ptr_ref(first_lexem)) ;
  s_fprintf(fd, "<LI>last_lexem = %s</LI>\n",  ptr_ref(last_lexem)) ;

  s_fprintf(fd, "<LI>cur_rank = %d</LI>\n",  cur_rank) ;
  s_fprintf(fd, "<LI>cur_context = %d</LI>\n",  cur_context) ;
  s_fprintf(fd, "<LI>nb_objects = %d</LI>\n",  nb_objects) ;

  s_fprintf(fd, "</UL>\n") ;
  s_fprintf(fd, "<p><hr>") ;
  s_fprintf(fd, "</BODY>\n</HTML>\n") ;

  TRACE2("T_object_manager(%x):close(%d) ", this, fd) ;
  fclose(fd) ;

  return NULL ;

}

//
//	}{	Classe T_object_tag (gestionnaire d'objets)
//
FILE *T_object_tag::dump_html(void)
{
  //  TRACE1("T_object_tag::dump_html(%x)", this) ;

  char *file_name = get_tmp_string(get_output_path_len() + 7 + ADR_SIZE) ;
  //                    1  23456
  sprintf(file_name, "%s/%p.html", get_output_path(), this) ;

  struct stat stats_ls ;

  if (stat(file_name, &stats_ls) == 0)
	{
	  fprintf(stderr, "erreur : le fichier %s existe deja\n", file_name) ;
	  exit(1) ;
	}


  FILE *fd = fopen(file_name, "w") ;
  TRACE3("T_object_tag(%x):fopen(%s)->%d", this, file_name, fd) ;

  if (fd == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  s_fprintf(fd, "<HTML>\n<HEAD>\n") ;

  s_fprintf(fd, "<TITLE>Object Tag 0x%x</TITLE>\n", this) ;
  s_fprintf(fd, "</HEAD>\n<BODY>\n") ;

  s_fprintf(fd,
                        "<H1><P><em> Object <kbd>Ox%p</kbd> is an instance of \
<FONT COLOR=\"red\"><kbd>T_object_tag</kbd></FONT></em></H1>\n",
                        this) ;

  s_fprintf(fd, "<H2>Fields from class T_object_tag :</H2>") ;

  s_fprintf(fd, "<UL>\n") ;

  s_fprintf(fd, "<LI>object = %s</LI>\n",  ptr_ref(object)) ;
  s_fprintf(fd, "<LI>context = %d</LI>\n",  context) ;

  s_fprintf(fd, "<LI>next_tag = <A HREF=\"%p.html\"> %x</A></LI>\n",
			(size_t)next_tag,
			(size_t)next_tag) ;

  s_fprintf(fd, "</UL>\n") ;
  s_fprintf(fd, "<p><hr>") ;
  s_fprintf(fd, "</BODY>\n</HTML>\n") ;

  TRACE2("T_object_tag(%x):close(%d) ", this, fd) ;
  fclose(fd) ;

  return NULL ;

}

//
//	}{	Classe T_pragma
//
FILE *T_pragma::dump_html(void)
{
  //TRACE1("T_pragma(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_pragma :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>first_param = %s, \n", ptr_ref(first_param)) ;
  s_fprintf(fd, "last_param = %s</LI>\n", ptr_ref(last_param)) ;
  s_fprintf(fd, "ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "next_pragma = %s</LI>\n", ptr_ref(next_pragma)) ;
  s_fprintf(fd, "prev_pragma = %s</LI></UL>\n", ptr_ref(prev_pragma)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_msg_line
//
FILE *T_msg_line::dump_html(void)
{
  //TRACE1("T_msg_line(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_msg_line :</H2><UL>") ;

  s_fprintf(fd,
			"<LI>level = %s</LI>\n",
			(msg_level == T_msg_level::ERROR) ?
			"ERROR" : (msg_level == T_msg_level::WARNING ? "WARNING" : "INFO")) ;
  s_fprintf(fd, "<LI>file_name = %s</LI>\n", sym_ref(file_name)) ;
  s_fprintf(fd, "<LI>file_line = %d</LI>\n", file_line) ;
  s_fprintf(fd, "<LI>file_column = %d</LI>\n", file_column) ;
  s_fprintf(fd, "<LI>contents = %s</LI></UL>\n", sym_ref(contents)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_msg_line_manager
//
FILE *T_msg_line_manager::dump_html(void)
{
  //TRACE1("T_msg_line_manager(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_msg_line_manager :</H2><UL>") ;

  s_fprintf(fd, "<LI>first_msg = %s</LI>\n", ptr_ref(first_msg)) ;
  s_fprintf(fd, "<LI>last_msg = %s</LI></UL>\n", ptr_ref(last_msg)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}


// Classe T_integer
FILE *T_integer::dump_html(void)
{
  TRACE1("T_literal_integer(%x)::dump_html()", this) ;
  ENTER_TRACE ; ENTER_TRACE ;
  FILE *fd = T_object::dump_html() ;
  EXIT_TRACE ; EXIT_TRACE ;

  s_fprintf(fd, "<HTML>\n<HEAD>\n") ;

  s_fprintf(fd, "<H2>Fields from class T_integer :</H2>") ;
  s_fprintf(fd, "<UL><LI>value = %s</LI>\n", sym_ref(value)) ;
  s_fprintf(fd, "<LI>is_positive = %s</LI></UL>\n", bool_ref(is_positive)) ;

  close_html(fd) ;
  return NULL ;
}

FILE *T_op_call_tree::dump_html(void)
{
  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_op_call_tree :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_son = %s</LI>\n", ptr_ref(first_son)) ;
  s_fprintf(fd, "<LI>last_son = %s</UL>\n", ptr_ref(last_son)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
// }{ Fin du fichier
//
