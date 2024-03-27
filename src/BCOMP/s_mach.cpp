/******************************* CLEARSY **************************************
* Fichier : $Id: s_mach.cpp,v 2.0 2005-04-25 10:46:10 cc Exp $
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
RCS_ID("$Id: s_mach.cpp,v 1.20 2005-04-25 10:46:10 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

// Analyse syntaxique d'une machine
T_lexem *T_machine::syntax_analysis(T_lexem *cur_lexem)
{
TRACE3("T_machine(%x, get_ref_lexem %x)::syntax_analysis(%x)",
	   this, get_ref_lexem(), cur_lexem) ;

cur_lexem = cur_lexem->get_next_lexem() ;

// On regarde si la machine a des parametres
if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_OPEN_PAREN) )
	{
	parse_PARAMS(this, &cur_lexem) ;
	}

// On etudie les clauses
for (;;)
	{
#ifdef FULL_TRACE
	  TRACE3("BOUCLE PRINCIPALE -> LEXEME %s (ligne %d, colonne %d)",
			 (cur_lexem == NULL) ? "NULL" : cur_lexem->get_lex_ascii(),
			 (cur_lexem == NULL) ? 0 : cur_lexem->get_file_line(),
			 (cur_lexem == NULL) ? 0 : cur_lexem->get_file_column()) ;
#endif

	  if (cur_lexem == NULL)
		{
		// Erreur fatale : machine pas terminee par END
		TRACE1("ref_lexem %x", get_ref_lexem()) ;
		TRACE1("ref_lexem->lex_name %x", get_ref_lexem()->get_lex_name()) ;
		TRACE1("catalog_msg %x", get_catalog(C_EOF)) ;
		TRACE1("catalog_msg %s", get_catalog(C_EOF)) ;
		syntax_error(get_ref_lexem(),
					 CAN_CONTINUE,
					 get_error_msg(E_COMPONENT_WITHOUT_END),
					 (get_ref_lexem() == NULL) ?
					 get_catalog(C_EOF)
					 : get_ref_lexem()->get_lex_name()) ;

		// On essaie de trouver le end qui manque
		T_op *cur_op = first_operation ;

		while (cur_op != NULL)
			{
			cur_op->check_for_missing_end() ;
			cur_op = (T_op *)cur_op->get_next() ;
			}

		// On arrete
		stop() ;
		}

	T_lex_type lex_type = cur_lexem->get_lex_type() ;

#ifdef FULL_TRACE
	TRACE1("lex_type = %d", lex_type) ;
	TRACE1("value = %s",
		   (cur_lexem->get_value() == NULL) ? "NULL" : cur_lexem->get_value()) ;
#endif

	switch(lex_type)
		{
		case L_END :
			{
			// Fin de la machine
			TRACE0("OK fin de la machine atteinte") ;

			// On cree le drapeau de fin de machine
			end_machine = new T_flag(this, get_betree(), cur_lexem) ;
			TRACE2("%x->end_machine=%x", this, end_machine) ;

			T_lexem *next = cur_lexem->get_next_lexem() ;
			return next ;
			}

		case L_EVENTS :
			{
			cur_lexem->set_lex_type(L_OPERATIONS) ;
			}
			// fall through
		case L_OPERATIONS :
			{
			parse_OPERATIONS(this, &cur_lexem) ;
			break ;
			}

		case L_LOCAL_OPERATIONS :
			{
			  parse_LOCAL_OPERATIONS(this, &cur_lexem) ;
			  break ;
			}

		case L_CONSTRAINTS :
			{
			parse_CONSTRAINTS(this, &cur_lexem) ;
			break ;
			}

		case L_SEES :
			{
			parse_SEES(this, &cur_lexem) ;
			break ;
			}

		case L_INCLUDES :
			{
			parse_INCLUDES(this, &cur_lexem) ;
			break ;
			}

		case L_PROMOTES :
			{
			parse_PROMOTES(this, &cur_lexem) ;
			break ;
			}

		case L_EXTENDS :
			{
			parse_EXTENDS(this, &cur_lexem) ;
			break ;
			}

		case L_USES :
			{
			parse_USES(this, &cur_lexem) ;
			break ;
			}

		case L_SETS :
			{
			parse_SETS(this, &cur_lexem) ;
			break ;
			}

		case L_CONCRETE_CONSTANTS :
			{
			parse_CONCRETE_CONSTANTS(this, &cur_lexem) ;
			break ;
			}

		case L_ABSTRACT_CONSTANTS :
			{
			parse_ABSTRACT_CONSTANTS(this, &cur_lexem) ;
			break ;
			}

		case L_CONCRETE_VARIABLES :
			{
			parse_CONCRETE_VARIABLES(this, &cur_lexem) ;
			break ;
			}
		case L_ABSTRACT_VARIABLES :
			{
			parse_ABSTRACT_VARIABLES(this, &cur_lexem) ;
			break ;
			}

		case L_PROPERTIES :
			{
			parse_PROPERTIES(this, &cur_lexem) ;
			break ;
			}

		case L_INVARIANT :
			{
			parse_INVARIANT(this, &cur_lexem) ;
			break ;
			}

#ifdef BALBULETTE
		case L_THEOREMS :
			{
			cur_lexem->set_lex_type(L_ASSERTIONS) ;
			}
			// fall through
#endif

		case L_ASSERTIONS :
			{
			parse_ASSERTIONS(this, &cur_lexem) ;
			break ;
			}

		case L_INITIALISATION :
			{
			parse_INITIALISATION(this, &cur_lexem) ;
			break ;
			}

		case L_VALUES :
			{
			parse_VALUES(this, &cur_lexem) ;
			break ;
			}

		case L_REFINES :
			{
			parse_REFINES(this, &cur_lexem) ;
			break ;
			}

		case L_IMPORTS :
			{
			parse_IMPORTS(this, &cur_lexem) ;
			break ;
            }
        case L_VARIANT :
            {
            if (get_machine_type() == MTYPE_REFINEMENT) {
                if (get_B0_generate_eventb_non_divergence_po() != TRUE) {
                    syntax_warning(cur_lexem,
                                   BASE_WARNING,
                                   get_warning_msg(W_VARIANT_WITHOUT_NON_DIVERGENCE_RESSOURCE),
                                   cur_lexem->get_lex_ascii()) ;
                }
                parse_VARIANT(this, &cur_lexem) ;
            } else {
                TRACE2("lex_type %d ascii=<%c>",
                   cur_lexem->get_lex_type(),
                   cur_lexem->get_lex_ascii()) ;
                syntax_error(cur_lexem,
                         FATAL_ERROR,
                         get_error_msg(E_MACH_INVALID_CLAUSE),
                         cur_lexem->get_lex_ascii()) ;

            return NULL ;
            }
            break;
            }
		default :
			{
			TRACE2("lex_type %d ascii=<%c>",
				   cur_lexem->get_lex_type(),
				   cur_lexem->get_lex_ascii()) ;
			syntax_error(cur_lexem,
						 FATAL_ERROR,
						 get_error_msg(E_MACH_INVALID_CLAUSE),
						 cur_lexem->get_lex_ascii()) ;

			return NULL ;
			}
		}
	}
}



