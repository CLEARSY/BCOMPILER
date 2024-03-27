/******************************* CLEARSY **************************************
* Fichier : $Id: s_misc.cpp,v 2.0 2001-07-19 16:31:48 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*				  	Methodes d'analyse syntaxiques diverses
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
RCS_ID("$Id: s_misc.cpp,v 1.4 2001-07-19 16:31:48 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

//
//	}{ Analyse syntaxique de la classe T_used_machine
//
void T_used_machine::syntax_analysis(int parse_params,
											  T_use_type new_use_type,
											  T_lexem **adr_cur_lexem)
{
TRACE1("T_used_machine::syntax_analysis(%s)", (*adr_cur_lexem)->get_lex_name()) ;

T_lexem *cur_lexem = *adr_cur_lexem ;
#ifdef ALLOW_PAREN_AROUND_IDENT
int nb_paren = 0 ;
#endif

#ifdef ALLOW_PAREN_AROUND_IDENT
while (cur_lexem != NULL && cur_lexem->get_lex_type() == L_OPEN_PAREN)
    {
    ++nb_paren ;
    cur_lexem = cur_lexem->get_next_lexem() ;
    }
#endif

if ( (cur_lexem == NULL)
	 || ( 	 (cur_lexem->get_lex_type() != L_IDENT)
	      && (cur_lexem->get_lex_type() != L_RENAMED_IDENT) ) )
	{
	syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_SEEN_MACH_NAME)) ;
	}

use_type = new_use_type ;
first_use_param = last_use_param = NULL ;

char *full_name ;
char *p ;
size_t len ;
len = clone(&full_name, cur_lexem->get_value()) ;

name = (cur_lexem->get_lex_type() == L_IDENT)
	? new T_ident(full_name,
				  ITYPE_USED_MACHINE_NAME,
				  NULL,
				  NULL,
				  this,
				  get_betree(),
				  cur_lexem)
	: new T_renamed_ident(full_name,
						  ITYPE_USED_MACHINE_NAME,
						  NULL,
						  NULL,
						  this,
						  get_betree(),
						  cur_lexem) ;
TRACE1("name = %x", name) ;

p = (char *)strchr(full_name, '.') ;

if (p == NULL)
	{
	TRACE1("utilisation de %s sans renommage", full_name) ;
	component_name = name->get_name() ;
	instance_name = NULL ;
	}
else
	{
	component_name = lookup_symbol((char *)((size_t)p + 1)) ;
	*p = '\0' ;
	instance_name = lookup_symbol(full_name) ;
	}

free(full_name) ;

cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ALLOW_PAREN_AROUND_IDENT
while (nb_paren != 0)
    {
    if (cur_lexem == NULL || cur_lexem->get_lex_type() != L_CLOSE_PAREN)
        {
        syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_CLOSE_PAREN)) ;
        }
    --nb_paren ;
    cur_lexem = cur_lexem->get_next_lexem() ;
    }
#endif

if ( 	(parse_params == TRUE)
	 && (cur_lexem != NULL)
	 && (cur_lexem->get_lex_type() == L_OPEN_PAREN) )
	{
	// On parse les parametres d'utilisation
	TRACE0("parse params") ;

	cur_lexem = cur_lexem->get_next_lexem() ;
        int encore = TRUE;//(cur_lexem != NULL && cur_lexem->get_lex_type() != L_CLOSE_PAREN) ;

	while (encore == TRUE)
		{
		syntax_check_eof(cur_lexem) ;
		T_expr *param = syntax_get_instanciation(NULL,
												 NULL,
												 this,
												 get_betree(),
												 &cur_lexem) ;

		if (param == NULL)
			{
			// A FAIRE
			assert(FALSE) ;
			}

		// On convertit le T_expr qui utilise un binary_op
		// avec BOP_COMMA en liste
		param->extract_params((int)BOP_COMMA,
							 this,
							 (T_item **)&first_use_param,
							 (T_item **)&last_use_param) ;

		// A FAIRE : virer ce code stupide (le parseur LR mange les espaces)
		if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
			{
			TRACE0("un parametre de plus") ;
			cur_lexem = cur_lexem->get_next_lexem() ;
			}
		else
			{
			encore = FALSE ;
			}
		}

	if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_CLOSE_PAREN) )
		{
		syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_CLOSE_PAREN)) ;
		}

	cur_lexem = cur_lexem->get_next_lexem() ;
	}

*adr_cur_lexem = cur_lexem ;
}

//
//	}{	Analyse syntaxique de la classe T_valuation
//
void T_valuation::syntax_analysis(T_item **adr_first,
										   T_item **adr_last,
										   T_item *new_father,
										   T_betree *new_betree,
										   T_lexem **adr_ref_lexem)
{
TRACE5("T_valuation::syntax_analysis(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;

// On cherche les identificateurs apres avoir saute le "LET_VALUATION"
T_lexem *cur_lexem = *adr_ref_lexem ;

// Identifiant (deja verifie par l'appelant)
#ifndef NO_CHECKS
assert(cur_lexem->get_lex_type() == L_IDENT) ;
#endif

ident = new T_ident(cur_lexem->get_value(),
					ITYPE_UNKNOWN,
					NULL,
					NULL,
					this,
					get_betree(),
					cur_lexem) ;

// Parse le '='
//TRACE0("parse =") ;
cur_lexem = cur_lexem->get_next_lexem() ;

if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_EQUAL) )
	{
	syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_EQUALS)) ;
	}

// Parse expression
//TRACE0("parse expression") ;
cur_lexem = cur_lexem->get_next_lexem() ;

syntax_check_eof(cur_lexem) ;

value = syntax_get_expr(NULL,
						NULL,
						this,
						get_betree(),
						&cur_lexem) ;

//TRACE0("fin de T_valuation::T_syntax_analysis") ;
*adr_ref_lexem = cur_lexem ;
}

//
//	}{ Analyse syntaxique de la classe T_used_op
//
void T_used_op::syntax_analysis(T_lexem **adr_cur_lexem)
{
TRACE2("T_used_op(%x)::syntax_analysis(%s)",
	   this,
	   (*adr_cur_lexem)->get_lex_name()) ;

T_lexem *cur_lexem = *adr_cur_lexem ;

if ( (cur_lexem == NULL)
	 || ( 	 (cur_lexem->get_lex_type() != L_IDENT)
	      && (cur_lexem->get_lex_type() != L_RENAMED_IDENT) ) )
	{
	syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_SEEN_MACH_NAME)) ;
	}

char *full_name ;
clone(&full_name, cur_lexem->get_value()) ;
if (cur_lexem->get_lex_type() == L_IDENT)
	{
	TRACE0("utilisation sans renommage") ;
	name = new T_ident(full_name,
					   ITYPE_USED_OP_NAME,
					   NULL,
					   NULL,
					   this,
					   get_betree(),
					   cur_lexem) ;
	real_op_name = lookup_symbol(cur_lexem->get_value()) ;
	instance_name = NULL ;
	}
#ifndef NO_CHECKS
else if (cur_lexem->get_lex_type() == L_RENAMED_IDENT)
#else
else
#endif
	{
	name = new T_renamed_ident(full_name,
							   ITYPE_USED_OP_NAME,
							   NULL,
							   NULL,
							   this,
							   get_betree(),
							   cur_lexem) ;
	char *p = strchr(full_name, '.') ;
	ASSERT(p != NULL) ; // A FAIRE
	real_op_name = lookup_symbol((char *)((size_t)p + 1)) ;
	*p = '\0' ;
	instance_name = lookup_symbol(full_name) ;
	}
#ifndef NO_CHECKS
else
	{
	assert(FALSE) ; // A FAIRE
	}
#endif

free(full_name) ;

*adr_cur_lexem = cur_lexem->get_next_lexem() ;

TRACE3("apres analyse syntax : T_used_op(%x) instance_name %s real_op_name %s",
	   this,
 	   (instance_name == NULL) ? "(null)" : instance_name->get_value(),
 	   (real_op_name == NULL) ? "(null)" : real_op_name->get_value()) ;
}

//
//	}{	Fin du fichier
//
