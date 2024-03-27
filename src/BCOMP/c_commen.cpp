/******************************* CLEARSY **************************************
* Fichier : $Id: c_commen.cpp,v 2.0 1999-04-20 08:02:19 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Commentaires
*
* Compilation : 	-DDEBUG_COMMENT pour tracer
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
RCS_ID("$Id: c_commen.cpp,v 1.12 1999-04-20 08:02:19 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_comment
//
T_comment::T_comment(const char *new_comment,
                              int new_keep_comment,
                              int analyse_pragma,
                              T_machine *new_ref_machine,
                              T_item **adr_first,
                              T_item **adr_last,
                              T_item *new_father,
                              T_betree *new_betree,
                              T_lexem *new_ref_lexem)
: T_item(NODE_COMMENT, adr_first, adr_last, new_father, new_betree, new_ref_lexem)
{
#ifdef DEBUG_COMMENT
TRACE6("T_comment(%x)::T_comment(%x, %x, %x, %x, %x",
	   this, adr_first, adr_last, new_father, new_betree, new_ref_lexem) ;
// Commentaire ci-dessous supprime car dans le cas de la
// construction de la machine, new_ref_machine->get_machine_name() est
// NULL
//TRACE2("Ajout du commentaire %s dans la machine %s",
//	   new_comment,
//	   (new_ref_machine == NULL)
//	   ? "NULL" : new_ref_machine->get_machine_name()->get_name()->get_value()) ;
#endif

comment = lookup_symbol(new_comment) ;
keep_comment = new_keep_comment ;
ref_machine = new_ref_machine ;

if ( (analyse_pragma == TRUE) && (get_dep_mode() == FALSE) )
  {
	// On regarde s'il y a des pragmas
	check_pragma() ;
  }
}


// Fonction auxiliaire qui regarde si le commentaire comporte un pragma
void T_comment::check_pragma(void)
{
#ifdef DEBUG_COMMENT
  TRACE2("check_pragma(%x \"%s\")", this, comment->get_value()) ;
#endif

  if (keep_comment == TRUE)
	{
	  // Pas de pragma dans les '/ * ?' par definition
	  return ;
	}


  // On initalise l'analyse lexicale
  T_pragma_lexem *cur_lexem = pragma_lex_analysis(this) ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != PL_PRAGMA) )
	{
#ifdef DEBUG_COMMENT
	  TRACE0("pas un pragma") ;
#endif
	  pragma_lex_unload() ;
	  return ;
	}

#ifdef DEBUG_COMMENT
  TRACE0("mot-cle pragma detecte") ;
#endif
  // Ici on sait que l'on analyse un pragma
  // Il doit respecter la BNF i.e. on attend maintenant
  // Ident '(' Param_Pragma ')' */
  // avec Param_Pragma = Ident | Ident, Param_Pragma

  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef DEBUG_COMMENT
  TRACE3("ici cur_lexem = %x %s value %s",
		 cur_lexem,
		 (cur_lexem == NULL)
		 ? "" : get_pragma_lex_type_name(cur_lexem->get_lex_type()),
		 (cur_lexem->get_value() == NULL) ? "null" : cur_lexem->get_value()) ;
#endif
  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != PL_IDENT) )
	{

#ifdef DEBUG_COMMENT/* Erreur : pragma sans ident */
	  TRACE0("Erreur : pragma sans ident") ;
#endif
	  // Warning a faire
	  TRACE1("cur_lexem %x", cur_lexem) ;
	  if (cur_lexem == NULL)
		{
		  syntax_error(this->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_PRAGMA_WITH_NO_NAME),
					   comment->get_value()) ;
		}
	  else
		{
		  syntax_error(cur_lexem,
					   CAN_CONTINUE,
					   get_error_msg(E_PRAGMA_WITH_NO_NAME),
					   comment->get_value()) ;
		}
	  return ;
  }

  if (ref_machine == NULL) {
      if (get_allow_pragma_in_def()) {
          syntax_warning(cur_lexem->get_ref_comment()->get_ref_lexem(),
                         BASE_WARNING,
                         get_warning_msg(W_PRAGMA_IN_DEF)) ;

      } else {
      syntax_error(cur_lexem->get_ref_comment()->get_ref_lexem(),
                     CAN_CONTINUE,
                     get_error_msg(E_PRAGMA_IN_DEF)) ;
      }
      return;
  }

  ASSERT(ref_machine != NULL) ;
  T_pragma **adr_first_global_pragma = ref_machine->get_adr_first_global_pragma() ;
  T_pragma **adr_last_global_pragma = ref_machine->get_adr_last_global_pragma() ;

  // Par construction, le pere du commentaire est l'item a qui il faut
  // s'accrocher
  T_item *father = get_father() ;
  ASSERT(father != NULL) ;
  T_pragma **adr_first_local_pragma = father->get_adr_first_pragma() ;
  T_pragma **adr_last_local_pragma = father->get_adr_last_pragma() ;

  // On cree l'objet associe

      TRACE2("on cree le pragma dans la machine %x:%s",
              ref_machine,
              (ref_machine == NULL)
              ? ref_machine->get_machine_name()->get_name()->get_value()
              : "" ) ;
      (void)new T_pragma(cur_lexem->get_value(),
              this,
              ref_machine,
              cur_lexem,
              adr_first_local_pragma,
              adr_last_local_pragma,
              (T_item **)adr_first_global_pragma,
              (T_item **)adr_last_global_pragma,
              ref_machine,
              get_betree(),
              get_ref_lexem()) ;

  pragma_lex_unload() ;
}

// Obtention des chaines d'ouverture et de fermeture de commentaire
// i.e. '/*' et '*/' pour un commentaire non traduisible
// et   '/*?' et '?*/' pour un commentaire non traduisible
// '//' et '\n' pour un commentaire C++ non traduisible
// '//?' et '\n' pour un commentaire C++ traduisible

// On ne peut pas utiliser de constructeur statique car il faut
// que l'initialisation du compilateur ait eu lieu pour pouvoir
// faire des lookup_symbol
// Donc on initialise a NULL et on fait une fonction d'init,
// appelee si besoin est
static const T_symbol *op1_sct = NULL ;
static const T_symbol *op2_sct = NULL ;
static const T_symbol *cl1_sct = NULL ;
static const T_symbol *cl2_sct = NULL ;
static const T_symbol *op3_sct = NULL ;
static const T_symbol *op4_sct = NULL;
static const T_symbol *cl34_sct = NULL;

static void init_comment_symbols(void)
{
op1_sct = lookup_symbol("/*?") ;
op2_sct = lookup_symbol("/*") ;
cl1_sct = lookup_symbol("?*/") ;
cl2_sct = lookup_symbol("*/") ;
op3_sct = lookup_symbol("//");
op4_sct = lookup_symbol("//?");
cl34_sct = lookup_symbol("");
}

//
//	Versions "standalone"
//
const T_symbol *get_open_comment(T_lex_type lex_type)
{
	if(op1_sct == NULL)
	{
		init_comment_symbols();
	}

	switch(lex_type)
	{
	case L_COMMENT:
		return op2_sct;
	case L_KEPT_COMMENT:
		return op1_sct;
	case L_CPP_COMMENT:
		return op3_sct;
	case L_CPP_KEPT_COMMENT:
		return op4_sct;
	default:
		ASSERT(FALSE);
		return 0;
	}
}

const T_symbol *get_close_comment(T_lex_type lex_type)
{
	if (op1_sct == NULL)
	{
		init_comment_symbols() ;
	}
	switch(lex_type)
	{
	case L_KEPT_COMMENT:
		return cl1_sct;
	case L_COMMENT:
		return cl2_sct;
	case L_CPP_COMMENT:
	case L_CPP_KEPT_COMMENT:
		return cl34_sct;
	default:
		ASSERT(FALSE);
		return 0;
	}
}


// Resolution des commentaires ?
static int solve_comment_si = TRUE ;

void set_solve_comment(int new_solve_comment)
{
  solve_comment_si = new_solve_comment ;
}

int get_solve_comment(void)
{
  return solve_comment_si ;
}

//
//	}{	Fin du fichier
//
