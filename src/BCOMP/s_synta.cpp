/***************************** CLEARSY **************************************
* Fichier : $Id: s_synta.cpp,v 2.0 2005-04-25 10:46:10 cc Exp $
* (C) 2008 CLEARSY
*
* Description :	Analyse syntaxique
*
* Compilation :	-DENABLE_PAREN_IDENT pour autoriser les parentheses autour
*				des identificateurs
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
****************************************************************************/
#include "c_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"
//
//	}{	Analyse syntaxique
//
T_betree *syntax_analysis(T_lexem *first_lexem,
								   const char *new_file_name)
{
  TRACE1("syntax analysis(%s)", new_file_name) ;

  T_lexem *cur_lexem = lex_skip_comments(first_lexem) ;

  // On fabrique un B Extended Tree
  T_betree *betree = new T_betree(new_file_name) ;

  switch(cur_lexem->get_lex_type())
	{
#ifdef BALBULETTE
	case L_MODEL :
	  {
		cur_lexem->set_lex_type(L_MACHINE) ;
	  }
	  // fall through
#endif

	case L_MACHINE :
	  {
		(void)new T_machine(MTYPE_SPECIFICATION,
							new_file_name,
							betree,
							&cur_lexem) ;
		break ;
	  } ;
	case L_SYSTEM :
	  {
		(void)new T_machine(MTYPE_SYSTEM,
							new_file_name,
							betree,
							&cur_lexem) ;
		break ;
	  } ;

	case L_REFINEMENT :
	  {
		(void)new T_machine(MTYPE_REFINEMENT,
							new_file_name,
							betree,
							&cur_lexem) ;
		break ;
	  } ;

	case L_IMPLEMENTATION :
	  {
		(void)new T_machine(MTYPE_IMPLEMENTATION,
							new_file_name,
							betree,
							&cur_lexem) ;
		break ;
	  } ;

	default :
	  {
		syntax_error(cur_lexem,
					 FATAL_ERROR,
					 get_error_msg(E_INVALID_COMPONENT_TYPE),
					 cur_lexem->get_lex_ascii()) ;
	  }
	}


  // Mise a jour du pointeur sur le Betree dans le Betree
  betree->set_betree(betree) ;

  // Mise a jour des liens vers les variables statiques de T_lexem
  betree->last_code_lexem = lex_get_last_code_lexem() ;
  betree->definitions_clause = lex_get_definitions_clause() ;
  betree->first_definition = lex_get_first_definition() ;
  betree->last_definition = lex_get_last_definition() ;

  //
  // Recuperation first_lexem
  //
  betree->first_lexem = lex_get_first_lexem() ;

  TRACE4("FIRST_LEXEM %x %s prev %x %s",
		 betree->first_lexem,
		 betree->first_lexem->get_lex_ascii(),
		 betree->first_lexem->get_real_prev_lexem(),
		 (betree->first_lexem->get_real_prev_lexem() == NULL) ? "" :
		 betree->first_lexem->get_real_prev_lexem()->get_lex_ascii()) ;

  T_lexem *cur = first_lexem->get_real_prev_lexem() ;

  while ( (cur != NULL) && (cur->is_a_comment() == TRUE) )
	{
	  betree->first_lexem = cur ;
	  cur = cur->get_real_prev_lexem() ;
	}

  //
  // Recuperation last_lexem
  //
  betree->last_lexem = lex_get_last_lexem() ;
  TRACE4("LAST_LEXEM %x %s prev %x %s",
		 betree->last_lexem,
		 betree->last_lexem->get_lex_ascii(),
		 betree->last_lexem->get_real_prev_lexem(),
		 (betree->last_lexem->get_real_prev_lexem() == NULL) ? "" :
		 betree->last_lexem->get_real_prev_lexem()->get_lex_ascii()) ;

  //
  // Recuperation last_code_lexem
  //
  betree->last_code_lexem = (cur_lexem == NULL)
	? lex_get_last_lexem() : cur_lexem->get_prev_lexem()  ;

  cur = betree->last_code_lexem ;

  while (cur->is_a_comment() == TRUE)
	{
	  cur = cur->get_real_prev_lexem() ;
	}

  betree->last_code_lexem = cur ;

  betree->definitions_clause = lex_get_definitions_clause() ;

  /*
  TRACE4("%x:%s LES DEFINITIONS first %x last %x",
		 betree,
		 betree->get_file_name()->get_value(),
		 T_lexem::first_definition,
		 T_lexem::last_definition) ;
  betree->first_definition = T_lexem::first_definition ;
  betree->last_definition = T_lexem::last_definition ;
  */
  betree->first_definition = pop_first_def_stack() ;
  betree->last_definition = pop_last_def_stack() ;

  TRACE4("%x:%s LES DEFINITIONS first %x last %x",
		 betree,
		 betree->get_file_name()->get_value(),
		 betree->first_definition,
		 betree->last_definition) ;
  betree->first_file_definition = pop_first_file_def_stack() ;
  betree->last_file_definition = pop_last_file_def_stack() ;

  // Correction des liens betree
  T_file_definition *cur_fdef = betree->first_file_definition ;

  while (cur_fdef != NULL)
	{
	  cur_fdef->set_betree(betree) ;
	  cur_fdef = (T_file_definition *)cur_fdef->get_next() ;
	}

  TRACE2("file_definitions  first %x last %x",
		 betree->first_file_definition,
		 betree->last_file_definition) ;

  betree->definitions_clause = pop_def_clause_stack() ;

  return betree ;
}


//
// }{	Verifie que l'on n'est pas � la fin du fichier
// 	Si c'est le cas, produit un message d'erreur fatale
#ifndef FULL_TRACE
T_lexem *syntax_check_eof(T_lexem *cur_lexem)
#else
  T_lexem *_syntax_check_eof(const char *file,
									  int line,
									  T_lexem *cur_lexem)
#endif
{
#ifdef FULL_TRACE
  TRACE3("syntax_check_eof(%x) appele depuis %s:%d", cur_lexem, file, line) ;
#endif
  if (cur_lexem == NULL)
	{
	  syntax_error(lex_get_last_lexem(),
				   FATAL_ERROR,
				   get_error_msg(E_UNEXPECTED_EOF)) ;
	  return NULL ;
	}

  return cur_lexem ;
}

#ifdef ENABLE_PAREN_IDENT
// Fonction auxilaire utilisees pour le parsing des identificateurs
// parentheses
static void enable_paren(T_lexem **adr_cur_lexem, int *adr_nb_open)
{
  for (;;)
	{
	  if (*adr_cur_lexem == NULL)
		{
		  return ;
		}

	  // Tant qu'on a des '(' et des ')', on les mange
	  switch ((*adr_cur_lexem)->get_lex_type())
		{
		case L_OPEN_PAREN :
		  {
#ifdef COMPAT
			// On marque la '(' a ignorer
			(*adr_cur_lexem)->mark_as_void() ;
#endif
			*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;
			*adr_nb_open += 1 ;
			break ;
		  }
		case L_CLOSE_PAREN :
		  {
			*adr_nb_open -= 1 ;
			if (*adr_nb_open < 0)
			  {
				// Elle n'etait pas pour nous, on ne bouge pas !
				return ;
			  }
#ifdef COMPAT
			// On marque la ')' a ignorer
			(*adr_cur_lexem)->mark_as_void() ;
#endif
			*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;
			break ;
		  }
		default :
		  {
			return ;
		  }
		}
	}
}
#endif


//
//	}{ Fonction qui parse les parametres de la machine
//
void parse_PARAMS(T_machine *machine,
						   T_lexem **adr_cur_lexem)
{
  TRACE0("debut PARAMS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;

  int encore = TRUE ;
#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  // S�parateur de param�tre de machine: , classique ; BOM
  T_lex_type sep = (get_Use_B0_Declaration() == TRUE) ? L_SCOL : L_COMMA ;
  T_ident *cur_ident;
  while (encore == TRUE)
	{
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_MACH_PARAM)) ;
		}

	  // On cherche s'il y a au moins une minuscule dans le nom
	  // pour savoir si c'est un parametre formel scalaire ou
	  // ensembliste
	  const char *value = cur_lexem->get_value() ;
	  int has_lower = FALSE ;

	  while ( (*value != '\0') && (has_lower == FALSE) )
		{
		  if ( (*value >= 'a') && (*value <= 'z') )
			{
			  has_lower = TRUE ;
			}
		  else
			{
			  value ++ ;
			}
		}

	  cur_ident = new T_ident(cur_lexem->get_value(),
							  (has_lower == TRUE)
							  ? ITYPE_MACHINE_SCALAR_FORMAL_PARAM
							  : ITYPE_MACHINE_SET_FORMAL_PARAM,
							  (T_item **)&machine->first_param,
							  (T_item **)&machine->last_param,
							  machine,
							  machine->get_betree(),
							  cur_lexem) ;
	  TRACE1("param : %s", cur_ident->get_name()->get_value());
	  cur_lexem = cur_lexem->get_next_lexem() ;
	  syntax_check_eof(cur_lexem);
	  // On parse le type de la constante en BOM
	  if (get_Use_B0_Declaration() == TRUE
		  &&
		  has_lower == TRUE)
		{
		  TRACE0("constante");
		  if (cur_lexem->get_lex_type() != L_BELONGS)
			{
			  syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_BELONGS));
			}

		  cur_ident->set_BOM_lexem_type(cur_lexem);
		  cur_lexem = cur_lexem->get_next_lexem();
		  syntax_check_eof(cur_lexem);

		  T_item *type = syntax_call_LR_parser(NULL,
											   NULL,
											   cur_ident,
											   machine->get_betree(),
											   &cur_lexem);
		  if (type == NULL
			  ||
			  type->is_an_expr() != TRUE)
			{
			  // a priori non NULL car sinon erreur signal�e par
			  // syntax_call_LR_parser mais prudence
			  syntax_unexpected_error(type == NULL ? cur_ident : type,
									  FATAL_ERROR,
									  get_catalog(C_EXPR));
			}

		  cur_ident->set_BOM_type((T_expr *)type);

		}



#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == sep) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_CLOSE_PAREN) )
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_CLOSE_PAREN)) ;
	}

  *adr_cur_lexem = cur_lexem->get_next_lexem() ;

  EXIT_TRACE ;
  TRACE0("fin PARAMS") ;
}


//
//	}{ Fonction qui parse la clause ABSTRACT_CONSTANTS
//
void parse_ABSTRACT_CONSTANTS(T_machine *machine,
							  T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause CONSTANTS
  TRACE0("debut clause ABSTRACT_CONSTANTS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  T_item **adr_first ;
  adr_first = (T_item **)&machine->first_abstract_constant ;

  T_item **adr_last ;
  adr_last = (T_item **)&machine->last_abstract_constant ;

  if (machine->first_abstract_constant != NULL)
	{
	  // CTRL Ref : CSYN 6-1
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_HCONSTANTS_CL));
	  T_lexem *prev = machine->first_abstract_constant->get_before_ref_lexem() ;
	  already_location(prev) ;
	}

  T_flag *clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;
  machine->abstract_constants_clause = clause ;

  // Cette clause est interdite en implementation
  // CTRL Ref : CSYN 1-1
  if (machine->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP)) ;
	}

  int encore = TRUE ;


  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  while (encore == TRUE)
	{
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  // Parse le nom de la constante
	  if (cur_lexem == NULL || cur_lexem->get_lex_type() != L_IDENT)
		{
		  char buf[128] ;
		  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
		}

	  T_ident *cur_ident = new T_ident(cur_lexem->get_value(),
									   ITYPE_ABSTRACT_CONSTANT,
									   adr_first,
									   adr_last,
									   clause,
									   machine->get_betree(),
									   cur_lexem) ;

	  cur_ident->set_ref_machine(machine) ;
	  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem != NULL && cur_lexem->get_lex_type() == L_COMMA)
		{
		  cur_lexem = cur_lexem->get_next_lexem();
		}
	  else
		{
		  encore = FALSE;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause ABSTRACT_CONSTANTS") ;
}
//
//	}{ Fonction qui parse la clause CONCRETE_CONSTANTS
//
void parse_CONCRETE_CONSTANTS(T_machine *machine,
				       T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause CONCRETE_CONSTANTS
  TRACE0("debut clause CONCRETE_CONSTANTS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  T_item **adr_first ;
  T_item **adr_last ;
  adr_first = (T_item **)&machine->first_concrete_constant ;
  adr_last = (T_item **)&machine->last_concrete_constant ;

  if (machine->first_concrete_constant != NULL)
	{
	  // CTRL Ref : CSYN 6-3
	  // CTRL Ref : CSYN 6-6

	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_CONSTANTS_CL)) ;
	  T_lexem *prev = machine->first_concrete_constant->get_before_ref_lexem() ;
	  already_location(prev) ;
	}

  T_flag *clause ;
  clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;
  machine->concrete_constants_clause = clause ;

  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  T_lex_type sep = (get_Use_B0_Declaration() == TRUE) ? L_SCOL : L_COMMA ;

  int encore = TRUE ;
  while (encore == TRUE)
	{
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem == NULL || cur_lexem->get_lex_type() != L_IDENT)
		{
		  char buf[128] ;
		  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
		}
	  // Parse le nom de la constante

	  T_ident *cur_ident = new T_ident(cur_lexem->get_value(),
									   ITYPE_CONCRETE_CONSTANT,
									   adr_first,
									   adr_last,
									   clause,
									   machine->get_betree(),
									   cur_lexem) ;

	  cur_ident->set_ref_machine(machine) ;

	  cur_lexem = cur_lexem->get_next_lexem() ;

	  if (get_Use_B0_Declaration() == TRUE)
		{
		  syntax_check_eof(cur_lexem);
		  if (cur_lexem->get_lex_type() != L_BELONGS
			  &&
			  cur_lexem->get_lex_type() != L_EQUAL)
			{
			  syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_EQUALS_or_BELONGS));
			}

		  cur_ident->set_BOM_lexem_type(cur_lexem);
		  cur_lexem = cur_lexem->get_next_lexem();
		  syntax_check_eof(cur_lexem);

		  T_item *type = syntax_call_LR_parser(NULL,
											   NULL,
											   cur_ident,
											   machine->get_betree(),
											   &cur_lexem);
		  if (type == NULL
			  ||
			  type->is_an_expr() != TRUE)
			{
			  // � priori non NULL car sinon erreur signal�e par
			  // syntax_call_LR_parser mais prudence
			  syntax_unexpected_error(type == NULL ? cur_ident : type,
									  FATAL_ERROR,
									  get_catalog(C_EXPR));
			}

		  cur_ident->set_BOM_type((T_expr *)type);

		}
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif
	  if (cur_lexem != NULL && cur_lexem->get_lex_type() == sep)
		{
		  cur_lexem = cur_lexem->get_next_lexem();
		}
	  else
		{
		  encore = FALSE;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause CONCRETE_CONSTANTS") ;
}

//
//	}{ Fonction qui parse la clause ABSTRACT_VARIABLES
//
void parse_ABSTRACT_VARIABLES(T_machine *machine,
									   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause ABSTRACT_VARIABLES
  TRACE0("debut clause ABSTRACT_VARIABLES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  T_item **adr_first ;
  T_item **adr_last ;

  adr_first = (T_item **)&machine->first_abstract_variable ;
  adr_last = (T_item **)&machine->last_abstract_variable ;

  if (machine->first_abstract_variable != NULL)
	{
	  // CTRL Ref : CSYN 6-2
	  syntax_already_error(cur_lexem,
						   CAN_CONTINUE,
						   get_catalog(C_HVARIABLES_CL));
	  T_lexem *prev = machine->first_abstract_variable->get_before_ref_lexem() ;
	  already_location(prev) ;
	}


  T_flag *clause ;
  clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;
  machine->abstract_variables_clause = clause ;

  // Cette clause est interdite en implementation
  // CTRL Ref : CSYN 1-2
  if (machine->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(E_ABSTRACT_VARIABLES_FORBIDDEN_IN_IMP)) ;
	}

  int encore = TRUE ;


  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  while (encore == TRUE)
	{
	  // Parse le nom de la variable
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem == NULL
		  ||
		  (cur_lexem->get_lex_type() != L_IDENT
		   &&
		   cur_lexem->get_lex_type() != L_RENAMED_IDENT))
			{
			  char buf[128] ;
			  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
			  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
			}

      if (cur_lexem->get_lex_type() == L_RENAMED_IDENT)
		{
          // pas d' ident renomm�s
		  char buf[128] ;
		  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
		}

	  T_ident *cur_ident = new T_ident(cur_lexem->get_value(),
									   ITYPE_ABSTRACT_VARIABLE,
									   adr_first,
									   adr_last,
									   clause,
									   machine->get_betree(),
									   cur_lexem) ;

	  cur_ident->set_ref_machine(machine) ;

	  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem != NULL && cur_lexem->get_lex_type() == L_COMMA)
		{
		  cur_lexem = cur_lexem->get_next_lexem();
		}
	  else
		{
		  encore = FALSE;
		}

	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause ABSTRACT_VARIABLES") ;
}
//
//	}{ Fonction qui parse la clause CONCRETE_VARIABLES
//
void parse_CONCRETE_VARIABLES(T_machine *machine,
									   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause CONCRETE_VARIABLES
  TRACE0("debut clause CONCRETE_VARIABLES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  T_item **adr_first ;
  T_item **adr_last ;
  adr_first = (T_item **)&machine->first_concrete_variable ;
  adr_last = (T_item **)&machine->last_concrete_variable ;

  if (machine->first_concrete_variable != NULL)
	{
	  // CTRL Ref : CSYN 6-4
	  syntax_already_error(cur_lexem,
						   CAN_CONTINUE,
						   get_catalog(C_VARIABLES_CL)) ;
	  T_lexem *prev = machine->first_concrete_variable->get_before_ref_lexem() ;
	  already_location(prev) ;
	}

  T_flag *clause ;
  clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;
  machine->concrete_variables_clause = clause ;



  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  T_lex_type sep = (get_Use_B0_Declaration() == TRUE) ? L_SCOL : L_COMMA ;
  int encore = TRUE ;
  while (encore == TRUE)
	{
	  // Parse le nom de la variable
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem == NULL
		  ||
		  (cur_lexem->get_lex_type() != L_IDENT
		   &&
		   cur_lexem->get_lex_type() != L_RENAMED_IDENT))
			{
			  char buf[128] ;
			  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
			  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
			}

      if (cur_lexem->get_lex_type() == L_RENAMED_IDENT)
		{
          // pas d'ident renomm�s
		  char buf[128] ;
		  sprintf(buf, "%s name", (*adr_cur_lexem)->get_lex_name()) ;
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, buf) ;
		}


	  T_ident *cur_ident = new T_ident(cur_lexem->get_value(),
									   ITYPE_CONCRETE_VARIABLE,
									   adr_first,
									   adr_last,
									   clause,
									   machine->get_betree(),
									   cur_lexem) ;
	  ASSERT(cur_ident != NULL);
	  cur_ident->set_ref_machine(machine) ;
	  cur_lexem = cur_lexem->get_next_lexem() ;

	  if (get_Use_B0_Declaration() == TRUE)
		{
		  if (cur_lexem == NULL || cur_lexem->get_lex_type() != L_BELONGS)
			{
			  syntax_unexpected_error(cur_lexem,
									  FATAL_ERROR,
									  get_catalog(C_BELONGS));
			}


		  cur_ident->set_BOM_lexem_type(cur_lexem);
		  cur_lexem = cur_lexem->get_next_lexem();
		  syntax_check_eof(cur_lexem);

		  // parse l'expression typante
		  T_item *type = syntax_call_LR_parser(NULL,
											   NULL,
											   cur_ident,
											   machine->get_betree(),
											   &cur_lexem);
		  if (type == NULL
			  ||
			  type->is_an_expr() != TRUE)
			{
			  // � priori non NULL car sinon erreur signal�e par
			  // syntax_call_LR_parser mais prudence
			  syntax_unexpected_error(type == NULL ? cur_ident : type,
									  FATAL_ERROR,
									  get_catalog(C_EXPR));
			}
		  cur_ident->set_BOM_type ((T_expr *)type);
		}
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif

	  if (cur_lexem != NULL && cur_lexem->get_lex_type() == sep)
		{
		  cur_lexem = cur_lexem->get_next_lexem();
		}
	  else
		{
		  encore = FALSE;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause CONCRETE_VARIABLES") ;
}

//
//	}{ Fonction qui parse la clause SETS
//
void parse_SETS(T_machine *machine,
						 T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause SETS
  TRACE0("debut clause SETS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_set != NULL)
	{
	  // CTRL Ref : CSYN 6-18
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_SETS_CL)) ;
	  T_lexem *previous = machine->first_set->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  int encore = TRUE ;

  machine->sets_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_set ;
  T_item **adr_last = (T_item **)&machine->last_set ;
  cur_lexem = cur_lexem->get_next_lexem() ;

#ifdef ENABLE_PAREN_IDENT
  // Nombre d'ouvrants
  int nb_open = 0 ;
#endif

  while (encore == TRUE)
	{
	  // Parse le nom de l'ensemble

#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif
	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_SET_NAME)) ;
		}

	  TRACE0("-- avant alloc ident --") ;
	  T_ident *cur_ident = new T_ident(cur_lexem->get_value(),
									   ITYPE_ABSTRACT_SET,
									   adr_first,
									   adr_last,
									   machine->sets_clause,
									   machine->get_betree(),
									   cur_lexem) ;
	  TRACE1("-- apres alloc ident %x --", cur_ident) ;

	  // Ici si on a un '=' il faut lire les valeurs enumerees
	  cur_lexem = cur_lexem->get_next_lexem() ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_EQUAL) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  syntax_check_eof(cur_lexem) ;
		  cur_ident->parse_set_values(&cur_lexem) ;
		}

	  // Ici : soit un ';' auquel cas on reboucle
	  // soit autre chose auquel cas on sort
#ifdef ENABLE_PAREN_IDENT
	  enable_paren(&cur_lexem, &nb_open) ;
#endif
	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_SCOL) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause SETS") ;
}

//
//	}{ Fonction qui parse la clause CONSTRAINTS
//
void parse_CONSTRAINTS(T_machine *machine,
								T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause CONSTRAINTS
  TRACE0("debut clause CONSTRAINTS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->constraints_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-7
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_CONSTRAINTS_CL)) ;
	  //GP correction: mauvais numero de ligne pour la redefinition.
	  //WAS: T_lexem *previous = machine->constraints->get_before_ref_lexem() ;
	  T_lexem *previous = machine->constraints_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  int encore = TRUE ;

  machine->constraints_clause = new T_flag(machine, machine->get_betree(), cur_lexem);

  // Cette clause est autorisee en spec seulement
  // CTRL Ref : CSYN 1-3
  if (machine->get_machine_type() != MTYPE_SPECIFICATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(
								 machine->get_machine_type() == MTYPE_IMPLEMENTATION ?
							   	 E_CONSTRAINTS_FORBIDDEN_IN_IMP
								 : E_CONSTRAINTS_FORBIDDEN_IN_REF)) ;
	}


  cur_lexem = cur_lexem->get_next_lexem() ;

  T_betree *betree = machine->get_betree() ;

  while (encore == TRUE)
	{
	  syntax_check_eof(cur_lexem) ;
	  machine->constraints =
		syntax_get_machine_param_type_or_predicate(NULL,
												   NULL,
												   machine->constraints_clause,
												   betree,
												   &cur_lexem) ;

	  if (machine->constraints == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_AND) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause CONSTRAINTS") ;
}

//
//	}{ Fonction qui parse la clause PROPERTIES
//
void parse_PROPERTIES(T_machine *machine,
							   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause PROPERTIES
  TRACE0("debut clause PROPERTIES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->properties_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-15
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_PROPERTIES_CL)) ;
	  T_lexem *previous = machine->properties_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  int encore = TRUE ;

  machine->properties_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  T_betree *betree = machine->get_betree() ;
  while (encore == TRUE)
	{
	  syntax_check_eof(cur_lexem) ;
	  machine->properties =
		syntax_get_constant_type_or_predicate(NULL,
											  NULL,
											  machine->properties_clause,
											  betree,
											  &cur_lexem) ;

	  if (machine->properties == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_AND) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause PROPERTIES") ;
}

//
//	}{ Fonction qui parse la clause INVARIANT
//
void parse_INVARIANT(T_machine *machine,
							  T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause INVARIANT
  TRACE0("debut clause INVARIANT") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->invariant_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-12
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_INVARIANT_CL)) ;
	  //GP correction: mauvais numero de ligne pour la redefinition.
	  //WAS: T_lexem *previous = machine->invariant->get_before_ref_lexem() ;
	  T_lexem *previous = machine->invariant_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  int encore = TRUE ;

  machine->invariant_clause = new T_flag(machine, machine->get_betree(), cur_lexem);

  cur_lexem = cur_lexem->get_next_lexem() ;

  T_betree *betree = machine->get_betree() ;
  while (encore == TRUE)
	{
	  syntax_check_eof(cur_lexem) ;
	  machine->invariant =
		syntax_get_variable_type_or_predicate(NULL,
											  NULL,
											  machine->invariant_clause,
											  betree,
											  &cur_lexem) ;

	  if (machine->invariant == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_AND) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause INVARIANT") ;
}

//
//	}{ Fonction qui parse la clause VARIANT d'une machine
// Spécificité du B événementiel
//
void parse_VARIANT(T_machine *machine,
							  T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause VARIANT
  TRACE0("debut clause VARIANT") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->variant_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-12
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_VARIANT)) ;
	  //GP correction: mauvais numero de ligne pour la redefinition.
	  //WAS: T_lexem *previous = machine->invariant->get_before_ref_lexem() ;
	  T_lexem *previous = machine->variant_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->variant_clause = new T_flag(machine, machine->get_betree(), cur_lexem);

  cur_lexem = cur_lexem->get_next_lexem() ;

  syntax_check_eof(cur_lexem) ;

  T_betree *betree = machine->get_betree() ;

  machine->variant = syntax_get_expr(NULL, NULL, machine->variant_clause, betree, &cur_lexem) ;

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause VARIANT") ;
}

//
//	}{ Fonction qui parse la clause ASSERTION
//
void parse_ASSERTIONS(T_machine *machine,
							   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause ASSERTION
  TRACE0("debut clause ASSERTIONS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->assertions_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-5
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_ASSERTION_CL)) ;
	  //GP correction: mauvais numero de ligne pour la redefinition.
	  //WAS: T_lexem *previous = machine->assertion->get_before_ref_lexem() ;
	  T_lexem *previous = machine->assertions_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  int encore = TRUE ;

  machine->assertions_clause = new T_flag(machine, machine->get_betree(),cur_lexem);

  cur_lexem = cur_lexem->get_next_lexem() ;
  T_betree *betree = machine->get_betree() ;

  while (encore == TRUE)
	{
	  syntax_check_eof(cur_lexem) ;
	  syntax_get_predicate((T_item **)(&machine->first_assertion),
						   (T_item **)(&machine->last_assertion),
						   machine->assertions_clause,
						   betree,
						   &cur_lexem) ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_SCOL) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause ASSERTION") ;
}

//
//	}{ Fonction qui parse la clause INITIALISATION
//
void parse_INITIALISATION(T_machine *machine, T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause INITIALISATION
  TRACE0("debut clause INITIALISATION") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->initialisation_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-11
	  syntax_already_error(cur_lexem,
						   CAN_CONTINUE,
						   get_catalog(C_INITIALISATION_CL));
	  //GP correction: mauvais numero de ligne pour la redefinition.
	  //WAS:
	  //T_lexem *previous = machine->first_initialisation->get_before_ref_lexem() ;
	  T_lexem *previous = machine->initialisation_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->initialisation_clause = new T_flag(machine,
											  machine->get_betree(),
											  cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_initialisation ;
  T_item **adr_last = (T_item **)&machine->last_initialisation ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  // Fabrication du initialisation_clause_ident et initialisation_clause_operation
  // CF bug 1883
  // Son pere est le T_flag pour rester compatible avec les Betrees
  // precedents (en particulier, la recherche de contexte)

  // Attention il faut desactiver l'accrochage des commentaires lors de
  // la creation de ces deux objets
  set_solve_comment(FALSE) ;
  T_ident *ident = new T_ident("INITIALISATION",
							   ITYPE_BUILTIN,
							   NULL,
							   NULL,
							   machine->initialisation_clause,
							   machine->get_betree(),
							   cur_lexem) ;

  T_op *op = new T_op(ident,
					  // in params
					  (T_ident *)NULL,
					  (T_ident *)NULL,
					  // out params
					  (T_ident *)NULL,
					  (T_ident *)NULL,
					  // body
					  machine->first_initialisation,
					  machine->last_initialisation,
					  // first, last
					  (T_item **)NULL,
					  (T_item **)NULL,
					  // father
					  (T_item *)machine->initialisation_clause,
					  // betree
					  machine->get_betree()) ;

  ident->set_father(op) ;

  // Il faut re-activer l'accrochage des commentaires qu'on avait
  // desactive lors de la creation de ces deux objets
  set_solve_comment(TRUE) ;

  syntax_get_substitution(adr_first,
						  adr_last,
						  op,
						  machine->get_betree(),
						  &cur_lexem) ;

  *adr_cur_lexem = cur_lexem ;

  op->set_body((T_substitution *)*adr_first, (T_substitution *)*adr_last) ;
  machine->initialisation_clause_operation = op ;
  machine->initialisation_clause_ident = ident ;
  ident->link() ; // car ident utilise par l'op initialisation_clause_operation
  // et en champ propre initialisation_clause_ident

  EXIT_TRACE ;
  TRACE0("fin clause INITIALISATION") ;
}

//
//	}{ Fonction qui parse la clause VALUES
//
void parse_VALUES(T_machine *machine,
						   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause VALUES
  TRACE0("debut clause VALUES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_value != NULL)
	{
	  // CTRL Ref : CSYN 6-20
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_VALUES_CL)) ;
	  T_lexem *previous = machine->first_value->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  // Cette clause est autorisee en implementation seulement
  // CTRL Ref : CSYN 1-8
  if (machine->get_machine_type() != MTYPE_IMPLEMENTATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(machine->get_machine_type() == MTYPE_SPECIFICATION ?
							   	 E_VALUES_FORBIDDEN_IN_SPEC
								 : E_VALUES_FORBIDDEN_IN_REF)) ;
	}

  machine->values_clause = new T_flag(machine,
									  machine->get_betree(),
									  cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  int can_continue = TRUE ;
  // On parse une liste de valuations separees par des ';'
  while (can_continue == TRUE)
	{
	  // On doit avoir un identificateur
	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_IDENT)) ;
		}

	  // On fabrique la valuation
	  T_valuation *valuation = new T_valuation((T_item **)&machine->first_value,
											   (T_item **)&machine->last_value,
											   machine->values_clause,
											   machine->get_betree(),
											   &cur_lexem) ;

	  if (valuation == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	  if ( (cur_lexem != NULL) && cur_lexem->get_lex_type() == L_SCOL)
		{
		  TRACE0("can_continue = TRUE") ;
		  can_continue = TRUE ;
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  can_continue = FALSE ;
			}
		}
	  else
		{
		  TRACE1("can_continue = TRUE car cur_type %s",
				 (cur_lexem == NULL)
				 ? "lexem null" : cur_lexem->get_lex_name()) ;
		  can_continue = FALSE ;
		}

	  TRACE0("reboucle") ;
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause VALUES") ;
}

//
//	}{ Fonction qui parse la clause REFINES
//
void parse_REFINES(T_machine *machine,
							T_lexem **adr_cur_lexem)
{
  TRACE0("debut REFINES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
	{
	  // On n'accepte pas ici de prefixe de renommage (i.e. on verifie
	  // que l'on a strictement un T_ident)
	  // CTRL Ref : CSYN 5-2
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_ABSTR_NAME)) ;
	}

  // La clause REFINES est interdite en specification
  // CTRL Ref : CSYN 1-6
  if (machine->machine_type == MTYPE_SPECIFICATION ||
		  machine->machine_type == MTYPE_SYSTEM )
	{
	  syntax_error(cur_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_REFINES_FORBIDDEN_IN_SPEC)) ;
	}


  if (machine->refines_clause != NULL)
	{
	  // CTRL Ref : CSYN 6-16
	  syntax_already_error(*adr_cur_lexem, CAN_CONTINUE, get_catalog(C_REFINES_CL)) ;
	  T_lexem *previous = machine->refines_clause->get_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->refines_clause = new T_flag(machine,
									   machine->get_betree(),
									   *adr_cur_lexem) ;

  machine->abstraction_name = new T_ident(cur_lexem->get_value(),
										  ITYPE_ABSTRACTION_NAME,
										  NULL,
										  NULL,
										  machine,
										  machine->get_betree(),
										  cur_lexem) ;

  *adr_cur_lexem = cur_lexem->get_next_lexem() ;

  EXIT_TRACE ;
  TRACE0("fin REFINES") ;
}

//
//	}{ Fonction qui parse la clause IMPORTS
//
void parse_IMPORTS(T_machine *machine,
							T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause IMPORTS
  TRACE0("debut clause IMPORTS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

// Cette clause est interdite en specification et en raffinement
// CTRL Ref : CSYN 1-4
  if (machine->machine_type == MTYPE_SPECIFICATION ||
		  machine->machine_type == MTYPE_SYSTEM)
	{
	  syntax_error(cur_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_IMPORTS_FORBIDDEN_IN_SPEC)) ;
	}

  if (machine->machine_type == MTYPE_REFINEMENT)
	{
	  syntax_error(cur_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_IMPORTS_FORBIDDEN_IN_REF)) ;
	}

  if (machine->first_imports != NULL)
	{
	  // CTRL Ref : CSYN 6-9
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_IMPORTS_CL)) ;
	  T_lexem *previous = machine->first_imports->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->imports_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_imports ;
  T_item **adr_last = (T_item **)&machine->last_imports ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  int encore = TRUE ;

  syntax_check_eof(cur_lexem) ;
  while (encore == TRUE)
	{
	  (void)new T_used_machine(TRUE,				// CTRL Ref : CSYN 5-3
						 USE_IMPORTS,
						 adr_first,
						 adr_last,
						 machine->imports_clause,
						 machine->get_betree(),
						 &cur_lexem) ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause IMPORTS") ;
}

//
//	}{ Fonction qui parse la clause OPERATIONS
//
void parse_OPERATIONS(T_machine *machine,
							   T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause OPERATIONS
  TRACE0("debut clause OPERATIONS") ;
  ENTER_TRACE ;
  T_lexem *cur_lexem = *adr_cur_lexem ;
  machine->operations_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  if (machine->first_operation != NULL)
	{
	  // CTRL Ref : CSYN 6-13
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_OPERATIONS_CL)) ;
	  T_lexem *previous = machine->first_operation->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  syntax_check_eof(cur_lexem) ;
  syntax_get_operations((T_item **)&(machine->first_operation),
						(T_item **)&(machine->last_operation),
						machine,
						machine->get_betree(),
						&cur_lexem) ;

  if (machine->first_operation == NULL)
	{
	  syntax_warning(*adr_cur_lexem,
					 BASE_WARNING,
					 get_warning_msg(W_EMPTY_CLAUSE),
					 get_catalog(C_OPERATIONS_CL)) ;
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause OPERATIONS") ;
}

//
//	}{ Fonction qui parse la clause LOCAL_OPERATIONS
//
void parse_LOCAL_OPERATIONS(T_machine *machine, T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause LOCAL_OPERATIONS
  TRACE0("debut clause LOCAL_OPERATIONS") ;
  ENTER_TRACE ;
  T_lexem *cur_lexem = *adr_cur_lexem ;
  machine->local_operations_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  // CTRL Ref: SYMLOC1
  // La clause LOCAL_OPERATIONS n'est admise qu'en implementation
  if (machine->get_machine_type() != MTYPE_IMPLEMENTATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(E_LOCAL_OPERATIONS_FORBIDDEN_IN_SPEC_OR_REF)) ;
	}

  if (machine->first_local_operation != NULL)
	{
	  // CTRL Ref : CSYN 6-13
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_LOCAL_OPERATIONS_CL)) ;
	  T_lexem *previous = machine->first_local_operation->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  syntax_check_eof(cur_lexem) ;

  // On parse les operations comme pour la clause OPERATIONS
  // Seule difference : toutes les substitutions doivent etre
  // des substitutions de specification
  // Pour cela, on force le machine_type a MTYPE_SPECIFICATION le
  // temps de faire le parsing
  T_machine_type save_mtype = machine->get_machine_type() ;
  machine->set_machine_type(MTYPE_SPECIFICATION) ;

  syntax_get_operations((T_item **)&(machine->first_local_operation),
						(T_item **)&(machine->last_local_operation),
						machine,
						machine->get_betree(),
						&cur_lexem,
						TRUE) ;

  // Restauration du type de machine
  machine->set_machine_type(save_mtype) ;

  if (machine->first_local_operation == NULL)
	{
	  syntax_warning(*adr_cur_lexem,
					 BASE_WARNING,
					 get_warning_msg(W_EMPTY_CLAUSE),
					 get_catalog(C_LOCAL_OPERATIONS_CL)) ;
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause LOCAL_OPERATIONS") ;
}

//
//	}{ Fonction qui parse la clause SEES
//
void parse_SEES(T_machine *machine,
						 T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause SEES
  TRACE0("debut clause SEES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_sees != NULL)
	{
	  // CTRL Ref : CSYN 6-17
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_SEES_CL)) ;
	  T_lexem *previous = machine->first_sees->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->sees_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;
  TRACE2("%x -> sees_clause = %x", machine, machine->sees_clause) ;

  T_item **adr_first = (T_item **)&machine->first_sees ;
  T_item **adr_last = (T_item **)&machine->last_sees ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  int encore = TRUE ;

  syntax_check_eof(cur_lexem) ;
  while (encore == TRUE)
	{
	  TRACE0("avant creation T_used_machine") ;
          (void)new T_used_machine(FALSE,			// CTRL Ref : CSYN 5-3
						 USE_SEES,
						 adr_first,
						 adr_last,
						 machine->sees_clause,
						 machine->get_betree(),
						 &cur_lexem) ;
	  TRACE0("apres creation T_used_machine") ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause SEES") ;
}

//
//	}{ Fonction qui parse la clause PROMOTES
//
void parse_PROMOTES(T_machine *machine,
							 T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause PROMOTES
  TRACE0("debut clause PROMOTES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_promotes != NULL)
	{
	  // CTRL Ref : CSYN 6-14
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_PROMOTES_CL)) ;
	  T_lexem *previous = machine->first_promotes->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->promotes_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_promotes ;
  T_item **adr_last = (T_item **)&machine->last_promotes ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  int encore = TRUE ;

  syntax_check_eof(cur_lexem) ;
  while (encore == TRUE)
	{
	  (void)new T_used_op(adr_first,
					adr_last,
					machine->promotes_clause,
					machine->get_betree(),
					&cur_lexem) ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause PROMOTES") ;
}

//
//	}{ Fonction qui parse la clause EXTENDS
//
void parse_EXTENDS(T_machine *machine,
							T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause EXTENDS
  TRACE0("debut clause EXTENDS") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_extends != NULL)
	{
	  // CTRL Ref : CSYN 6-8
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_EXTENDS_CL)) ;
	  T_lexem *previous = machine->first_extends->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  machine->extends_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_extends ;
  T_item **adr_last = (T_item **)&machine->last_extends ;

  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;
  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
  int encore = TRUE ;

  while (encore == TRUE)
	{
  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
	  (void)new T_used_machine(TRUE,			// CTRL Ref : CSYN 5-3
						 USE_EXTENDS,
						 adr_first,
						 adr_last,
						 machine->extends_clause,
						 machine->get_betree(),
						 &cur_lexem) ;

  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause EXTENDS") ;
}

//
//	}{ Fonction qui parse la clause USES
//
void parse_USES(T_machine *machine,
						 T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause USES
  TRACE0("debut clause USES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  if (machine->first_uses != NULL)
	{
	  // CTRL Ref : CSYN 6-19
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_USES_CL)) ;
	  T_lexem *previous = machine->first_uses->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  // Cette clause est autorisee en spec seulement
  // CTRL Ref : CSYN 1-7
  if (machine->get_machine_type() != MTYPE_SPECIFICATION)
	{
	  syntax_error(cur_lexem,
				   CAN_CONTINUE,
				   get_error_msg(machine->get_machine_type() == MTYPE_IMPLEMENTATION ?
							   	 E_USES_FORBIDDEN_IN_IMP
								 : E_USES_FORBIDDEN_IN_REF)) ;
	}

  machine->uses_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_uses ;
  T_item **adr_last = (T_item **)&machine->last_uses ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  int encore = TRUE ;

  syntax_check_eof(cur_lexem) ;
  while (encore == TRUE)
	{
	  (void)new T_used_machine(FALSE,			// CTRL Ref : CSYN 5-3
						 USE_USES,
						 adr_first,
						 adr_last,
						 machine->uses_clause,
						 machine->get_betree(),
						 &cur_lexem) ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause USES") ;
}

//
//	}{ Fonction qui parse la clause INCLUDES
//
void parse_INCLUDES(T_machine *machine,
							 T_lexem **adr_cur_lexem)
{
  // OK on entre dans la clause INCLUDES
  TRACE0("debut clause INCLUDES") ;
  ENTER_TRACE ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
  // Cette clause est interdite en implemenation
  // CTRL Ref : CSYN 1-5
  if (machine->machine_type == MTYPE_IMPLEMENTATION)
	{
	  syntax_error(cur_lexem,
				   FATAL_ERROR,
				   get_error_msg(E_INCLUDES_FORBIDDEN_IN_IMP)) ;
	}

  if (machine->first_includes != NULL)
	{
	  // CTRL Ref : CSYN 6-10
	  syntax_already_error(cur_lexem, CAN_CONTINUE, get_catalog(C_INCLUDES_CL)) ;
	  T_lexem *previous = machine->first_includes->get_before_ref_lexem() ;
	  already_location(previous) ;
	}

  TRACE2("ici cur_lexem = %x %s",
		 cur_lexem,
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
  machine->includes_clause = new T_flag(machine, machine->get_betree(), cur_lexem) ;

  T_item **adr_first = (T_item **)&machine->first_includes ;
  T_item **adr_last = (T_item **)&machine->last_includes ;

  cur_lexem = cur_lexem->get_next_lexem() ;

  syntax_check_eof(cur_lexem) ;
  int encore = TRUE ;

  while (encore == TRUE)
	{
	  TRACE2("ici cur_lexem = %x %s",
			 cur_lexem,
			 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
	  (void)new T_used_machine(TRUE,			// CTRL Ref : CSYN 5-3
						 USE_INCLUDES,
						 adr_first,
						 adr_last,
						 machine->includes_clause,
						 machine->get_betree(),
						 &cur_lexem) ;

	  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
		{
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  if (cur_lexem == NULL)
			{
			  encore = FALSE ;
			}
		}
	  else
		{
		  encore = FALSE ;
		}
	  TRACE2("ici cur_lexem = %x %s",
			 cur_lexem,
			 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii()) ;
	}

  *adr_cur_lexem = cur_lexem ;

  EXIT_TRACE ;
  TRACE0("fin clause INCLUDES") ;
}

//
//	}{	Fonction qui parse les operations
//
void syntax_get_operations(T_item **adr_first,
									T_item **adr_last,
									T_item *father,
									T_betree *betree,
									T_lexem **adr_cur_lexem,
									int parse_local_op)
{
  TRACE1("debut syntax_get_operations (parse_local_op %s)",
		 (parse_local_op == TRUE) ? "TRUE" : "FALSE") ;
  ENTER_TRACE ;

  T_lexem *prev_lexem = (*adr_cur_lexem) ;
  syntax_check_eof(prev_lexem) ;
  T_lexem *cur_lexem = prev_lexem->get_next_lexem() ;

  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() != L_END) )
	{
	  // Parse les operations
	  TRACE0("parse les operations") ;

	  int encore = TRUE ;

	  while (encore == TRUE)
		{
		  T_lex_type ltype = cur_lexem->get_lex_type() ;
		  TRACE2("ltype = %d %s", ltype, cur_lexem->get_lex_ascii()) ;
		  if ( (ltype == L_IDENT) ||
			   (ltype == L_RENAMED_IDENT) ||
			   (ltype == L_OPEN_PAREN) )
			{
			  (void)new T_op(adr_first,
							 adr_last,
							 father,
							 betree,
							 &cur_lexem,
							 parse_local_op) ;

			  if (cur_lexem == NULL)
				{
				  TRACE0("Panic :: lexem == NULL et pas END ou ';'") ;
				  *adr_cur_lexem = NULL ;
				  return ;
				}

			  if (cur_lexem->get_lex_type() == L_SCOL)
				{
				  // Encore une operation
				  TRACE0("une op de plus ..") ;
				  prev_lexem = cur_lexem ;
				  cur_lexem = cur_lexem->get_next_lexem() ;
				  syntax_check_eof(cur_lexem) ;
				}
			  else
				{
				  TRACE1("pas scol mais %s : fin des operations",
						 cur_lexem->get_lex_name()) ;
				  encore = FALSE ;
				}
			}
		  else
			{
			  // Cas en erreur
			  // Si toutefois le lexeme precedent etait un L_SCOL, alors il
			  // s'agissait d'un L_SCOL superflus. On fait marche arriere dans
			  // notre analyse
			  if (prev_lexem->get_lex_type() != L_SCOL)
				{
				  TRACE1("ici, on crie a cause du '%s'", cur_lexem->get_lex_ascii()) ;

				  syntax_unexpected_error(cur_lexem,
										  CAN_CONTINUE,
										  get_catalog(C_OP_NAME)) ;
				}
			  else
				{
				  TRACE0("ici, on a detecte un ';' superflu") ;
				  if (get_strict_B_demanded() == TRUE)
					{
					  // En mode strict, on n'accepte pas les ';' superflus
					  syntax_warning(prev_lexem,
                                     BASE_WARNING,
									 get_warning_msg(W_EXTRA_SCOL_NOT_STRICT_B)) ;
					}
				}
			  encore = FALSE ;
			}
		}
	}

  *adr_cur_lexem = cur_lexem ;
  EXIT_TRACE ;
  TRACE3("fin de syntax_get_operations cur_lexem %s (%d:%d)",
		 (cur_lexem == NULL) ? "(null)" : cur_lexem->get_lex_ascii(),
		 (cur_lexem == NULL) ? 0 : cur_lexem->get_file_line(),
		 (cur_lexem == NULL) ? 0 : cur_lexem->get_file_column()) ;
}

// Methode qui essaie de trouver un end qui manque
void T_op::check_for_missing_end(void)
{
  TRACE1("T_op(%x)::check_for_missing_end", this) ;
  ENTER_TRACE ;

  T_substitution *cur = first_body ;

  //int inside_block = FALSE ; // est-on dans un bloc ouvrant ?

  for (;;)
	{
	  if (cur == NULL)
		{
		  EXIT_TRACE ;
		  TRACE1("fin de T_op(%x)::check_for_missing_end", this) ;
		  return ;
		}

	  cur->check_for_missing_end() ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}

//
//	}{	Fin du fichier
//

