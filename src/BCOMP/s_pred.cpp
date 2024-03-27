/******************************* CLEARSY **************************************
* Fichier : $Id: s_pred.cpp,v 2.0 2000-11-03 12:52:28 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Predicats
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
RCS_ID("$Id: s_pred.cpp,v 1.7 2000-11-03 12:52:28 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

// Fonction qui parse les predicates
T_predicate *syntax_get_predicate(T_item **adr_first,
										   T_item **adr_last,
										   T_item *father,
										   T_betree *betree,
										   T_lexem **adr_cur_lexem)
{
TRACE0("debut syntax_get_predicate") ;
ENTER_TRACE ;

TRACE0("Appel parseur LR") ;
T_item *res = syntax_call_LR_parser(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;

// On verifie que l'on a obtenu un predicat
if (res->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(res, CAN_CONTINUE) ;
	}

EXIT_TRACE ;
TRACE0("fin syntax_get_predicate") ;
return (T_predicate *)res ;
}

//
//	}{	Fonction qui parse les predicats ou les typages de parametres de machine
T_predicate *
	syntax_get_machine_param_type_or_predicate(T_item **adr_first,
											   T_item **adr_last,
											   T_item *father,
											   T_betree *betree,
											   T_lexem **adr_cur_lexem)
{
TRACE0("debut syntax_get_machine_param_type_or_predicate") ;
ENTER_TRACE ;

TRACE0("Appel parseur LR") ;
T_item *res = syntax_call_LR_parser(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;

// On verifie que l'on a obtenu un predicat
if (res->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(res, CAN_CONTINUE) ;
	}

EXIT_TRACE ;
TRACE0("fin syntax_get_machine_param_type_or_predicate") ;
return (T_predicate *)res ;
}


//
//	}{	Fonction qui parse les predicats ou les typages de constantes
T_predicate *syntax_get_constant_type_or_predicate(T_item **adr_first,
															T_item **adr_last,
															T_item *father,
															T_betree *betree,
															T_lexem **adr_cur_lexem)
{
TRACE0("debut syntax_get_constant_type_or_predicate") ;
ENTER_TRACE ;

TRACE0("Appel parseur LR") ;
T_item *res = syntax_call_LR_parser(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;

// On verifie que l'on a obtenu un predicat
if (res->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(res, CAN_CONTINUE) ;
	}

EXIT_TRACE ;
TRACE0("fin syntax_get_constant_type_or_predicate") ;
return (T_predicate *)res ;
}


//
//	}{	Fonction qui parse les predicats ou les typages de variablees
T_predicate *syntax_get_variable_type_or_predicate(T_item **adr_first,
															T_item **adr_last,
															T_item *father,
															T_betree *betree,
															T_lexem **adr_cur_lexem)
{
#ifdef FULL_TRACE
TRACE0("debut syntax_get_variable_type_or_predicate") ;
ENTER_TRACE ;

TRACE0("Appel parseur LR") ;
#endif

T_item *res = syntax_call_LR_parser(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;

// On verifie que l'on a obtenu un predicat
if (res->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(res, CAN_CONTINUE) ;
	}

#ifdef FULL_TRACE
EXIT_TRACE ;
TRACE0("fin syntax_get_variable_type_or_predicate") ;
#endif
return (T_predicate *)res ;
}


//
//	}{	Fin du fichier
//

