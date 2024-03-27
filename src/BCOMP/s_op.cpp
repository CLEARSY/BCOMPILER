/******************************* CLEARSY **************************************
* Fichier : $Id: s_op.cpp,v 2.0 2002-05-29 09:23:20 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse syntaxique d'une operation
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
RCS_ID("$Id: s_op.cpp,v 1.8 2002-05-29 09:23:20 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

// Analyse syntaxique
void T_op::syntax_analysis(T_item **adr_first,
                                    T_item **adr_last,
                                    T_item *new_father,
                                    T_betree *new_betree,
                                    T_lexem **adr_cur_lexem)
{
    TRACE6("T_op::syntax_analysis(%x, %x, %x, %x, %x, local_op %s)",
           adr_first,
           adr_last,
           new_father,
           new_betree,
           *adr_cur_lexem,
           (is_a_local_op == TRUE) ? "TRUE" : "FALSE") ;
    ENTER_TRACE ;

    T_lexem *cur_lexem = *adr_cur_lexem ;

    ASSERT(cur_lexem) ; // Verifie par l'appelant

    T_lex_type type = cur_lexem->get_lex_type() ;
    if ( (type != L_IDENT) && (type != L_RENAMED_IDENT) )
    {
        TRACE1("pb : ident attendu, %s obtenu", cur_lexem->get_lex_name()) ;
        syntax_error(cur_lexem,
                     FATAL_ERROR,
                     get_error_msg(E_OP_NAME_OR_PARAM_EXPECTED),
                     cur_lexem->get_lex_ascii()) ;
    }

    // Il faut aller un cran en avant pour savoir si l'on a un parametre
    // d'entree ou deja le nom de l'operation
    // Si on a un "," ou un "<--" alors c'etait un parametre d'entree
    // Si on a un "=" ou un "(" alors c'etait le nom de l'operation
    // Si on a autre chose alors pb !

    T_lexem *next_lexem = cur_lexem->get_next_lexem() ;

    if (next_lexem == NULL)
    {
        TRACE1("AIE : cur_lexem(%s)->next = NUL", cur_lexem->get_lex_ascii()) ;
        *adr_cur_lexem = NULL ;
        return ;
    }

    first_out_param = last_out_param = NULL ;

    TRACE2("cur_lexem %s, next_lexem %s",
           cur_lexem->get_lex_ascii(),
           next_lexem->get_lex_ascii()) ;
    switch(next_lexem->get_lex_type())
    {
    case L_COMMA :
    case L_RETURNS :
    {
        int encore = TRUE ;

        while (encore == TRUE)
        {
            if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
            {
                syntax_unexpected_error(cur_lexem,
                                        FATAL_ERROR,
                                        get_catalog(C_OP_OUTPUT_PARAM)) ;
            }

            T_ident *cur_ident = syntax_get_ident(ITYPE_OP_OUT_PARAM,
                                                  (T_item **)&first_out_param,
                                                  (T_item **)&last_out_param,
                                                  this,
                                                  get_betree(),
                                                  &cur_lexem) ;

            assert(cur_ident) ; // A FAIRE
            cur_ident->set_ref_op(this) ;
            cur_ident->set_ref_machine((T_machine *)get_father()) ;

            if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA))
            {
                TRACE0("encore un tour !") ;
                cur_lexem = cur_lexem->get_next_lexem() ;

            }
            else
            {
                encore = FALSE ;
            }
        }

        if (cur_lexem->get_lex_type() != L_RETURNS)
        {
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_RETURNS)) ;
        }

        // On rend la main sur le nom de l'operation
        cur_lexem = cur_lexem->get_next_lexem() ;
        syntax_check_eof(cur_lexem) ;
        break ;
    }

    case L_REF :
    case L_EQUAL :
    case L_OPEN_PAREN :
    case L_IDENT :
    {
        // Fall into !
        break ;
    }

    default :
    {
        syntax_error(cur_lexem,
                     MULTI_LINE,
                     get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
        syntax_unexpected_error(next_lexem,
                                FATAL_ERROR,
                                get_catalog(C_EQU_RET_OPPAR_COMMA)) ;
    }
    }

    // Il faut verifier qu'on est bien sur le nom de l'operation
    // Pas forcement le cas, exemple: "a <-- <-- op"
    syntax_check_eof(cur_lexem) ;
    if (    (cur_lexem->get_lex_type() != L_IDENT)
            && (cur_lexem->get_lex_type() != L_RENAMED_IDENT) )
    {
        syntax_ident_expected_error(cur_lexem, FATAL_ERROR) ;
    }

    // Ici cur_lexem est non NULL et pointe sur le nom de l'operation
    T_ident_type ident_type = (is_a_local_op == FALSE)
            ? ITYPE_OP_NAME
            : ITYPE_LOCAL_OP_NAME ;
    if (cur_lexem->get_lex_type() == L_IDENT)
    {
        name = new T_ident(cur_lexem->get_value(),
                           ident_type,
                           NULL,
                           NULL,
                           this,
                           new_betree,
                           cur_lexem) ;
    }
    else
    {
        name = new T_renamed_ident(cur_lexem->get_value(),
                                   ident_type,
                                   NULL,
                                   NULL,
                                   this,
                                   new_betree,
                                   cur_lexem) ;
    }

    TRACE1("--> OPERATION %s", cur_lexem->get_value()) ;

    first_in_param = last_in_param = NULL ;
    first_ref_op = last_ref_op = NULL;

    cur_lexem = cur_lexem->get_next_lexem() ;

    TRACE1("--> TYPE %d", get_betree()->get_root()->get_machine_type()) ;
    TRACE1("--> SPECIFICATION %s", get_betree()->get_root()->get_specification()->get_machine_name()->get_name()->get_value()) ;

    /* On analyse une potientielle clause ref op1,..., opn */
    if ( (get_betree()->get_root()->get_machine_type() == MTYPE_REFINEMENT ) &
         (cur_lexem != NULL) & (cur_lexem->get_lex_type() == L_REF))
    {
        cur_lexem = cur_lexem->get_next_lexem() ;
        if (cur_lexem != NULL &&
                (cur_lexem->get_lex_type() == L_IDENT))
        {
            syntax_get_ident(ITYPE_OP_NAME,
                             (T_item **)&first_ref_op,
                             (T_item **)&last_ref_op,
                             this,
                             get_betree(),
                             &cur_lexem) ;
        }
        else {
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_IDENT)) ;
        }
        int encore = TRUE ;
        while (encore == TRUE)
        {
            if ( (cur_lexem != NULL) &&
                 (cur_lexem->get_lex_type() == L_COMMA) )
            {
                TRACE0("encore un tour !") ;
                cur_lexem = cur_lexem->get_next_lexem() ;
                if (cur_lexem != NULL &&
                        (cur_lexem->get_lex_type() == L_IDENT))
                {
                    syntax_get_ident(ITYPE_OP_NAME,
                                     (T_item **)&first_ref_op,
                                     (T_item **)&last_ref_op,
                                     this,
                                     get_betree(),
                                     &cur_lexem) ;
                }
                else
                {
                    syntax_unexpected_error(cur_lexem,
                                            FATAL_ERROR,
                                            get_catalog(C_IDENT)) ;
                }
            }
            else if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_EQUAL) )
            {
                syntax_unexpected_error(cur_lexem,
                                        FATAL_ERROR,
                                        get_catalog(C_EQUALS)) ;
            }
            else
            {
                encore = FALSE ;
            }
        }
    }

    if ( (cur_lexem != NULL) & (cur_lexem->get_lex_type() == L_OPEN_PAREN) )
    {
        // Parse les parametres d'entree
        int encore = TRUE ;

        cur_lexem = cur_lexem->get_next_lexem() ;

        syntax_check_eof(cur_lexem) ;

        while (encore == TRUE)
        {
            if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
            {
                syntax_unexpected_error(cur_lexem,
                                        FATAL_ERROR,
                                        get_catalog(C_OP_INPUT_PARAM)) ;
            }

            T_ident *cur_ident = syntax_get_ident(ITYPE_OP_IN_PARAM,
                                                  (T_item **)&first_in_param,
                                                  (T_item **)&last_in_param,
                                                  this,
                                                  get_betree(),
                                                  &cur_lexem) ;

            assert(cur_ident) ; // A FAIRE
            cur_ident->set_ref_op(this) ;
            cur_ident->set_ref_machine((T_machine *)get_father()) ;

            if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
            {
                TRACE0("encore un tour !") ;
                cur_lexem = cur_lexem->get_next_lexem() ;
            }
            else
            {
                encore = FALSE ;
            }
        }

        // Parse le ')'
        if (cur_lexem->get_lex_type() != L_CLOSE_PAREN)
        {
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_CLOSE_PAREN)) ;
        }

        cur_lexem = syntax_check_eof(cur_lexem->get_next_lexem()) ;
    }

    if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_EQUAL) )
    {
        syntax_unexpected_error(cur_lexem,
                                FATAL_ERROR,
                                get_catalog(C_EQUALS)) ;
    }

    // Corps de l'operation
    TRACE0("corps de l'operation") ;
    *adr_cur_lexem = cur_lexem->get_next_lexem() ;

    first_body = last_body = NULL ;
    T_substitution *body = syntax_get_substitution((T_item **)&first_body,
                                                   (T_item **)&last_body,
                                                   this,
                                                   get_betree(),
                                                   adr_cur_lexem) ;

    assert(body != NULL) ; // A FAIRE :: erreur

  T_project *project = get_project_manager()->get_current() ;

  int isEventBProject = 0;

  if (project != NULL) {
      isEventBProject = project->isEventB() ;
  }
  else
  {
      isEventBProject = (T_project::get_default_project_type() == PROJECT_SYSTEM);
  }

  if (isEventBProject
          && !body->isValidHeadSubstitutionForEvent()) {
      syntax_error(cur_lexem,
                   CAN_CONTINUE,
                   get_error_msg(E_EVENTB_INVALID_HEAD_SUBSTITUTION)) ;
  }

  EXIT_TRACE ;
}

//
// }{ parse une liste de param�tres typ�s
//
void static parse_typed_param (T_ident_type param_type,
                                        T_item **first_param,
                                        T_item **last_param,
                                        T_op *op,
                                        T_lexem **adr_cur_lexem)
{
    TRACE0("->parse_typed_param");
    T_lexem *cur_lexem = *adr_cur_lexem;

    T_catalog_msg type_msg_param = (param_type == ITYPE_OP_IN_PARAM)
            ? C_OP_INPUT_PARAM
            : C_OP_OUTPUT_PARAM ;

    int encore = TRUE;
    while (encore == TRUE)
    {
        syntax_check_eof(cur_lexem);
        if (cur_lexem->get_lex_type() != L_IDENT)
        {
            syntax_error(cur_lexem,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(type_msg_param)) ;
        }

        // On a un ident pour param�tre le lexem suivant doit etre un ':'

        T_ident *param_ident = syntax_get_ident(param_type,
                                                (T_item **)first_param,
                                                (T_item **)last_param,
                                                op,
                                                op->get_betree(),
                                                &cur_lexem) ;
        assert(param_ident) ; // A FAIRE
        param_ident->set_ref_op(op) ;
        param_ident->set_ref_machine((T_machine *)op->get_father()) ;

        syntax_check_eof(cur_lexem);
        if (cur_lexem->get_lex_type() != L_BELONGS)
        {
            syntax_error(cur_lexem,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_BELONGS));
        }
        T_lexem *lexem_type = cur_lexem;

        cur_lexem = cur_lexem->get_next_lexem();
        syntax_check_eof(cur_lexem);

        T_item *type = syntax_call_LR_parser(NULL,
                                             NULL,
                                             param_ident,
                                             op->get_betree(),
                                             &cur_lexem);
        if (type == NULL
                ||
                type->is_an_expr() != TRUE)
        {
            // � priori non NULL car sinon erreur signal�e par
            // syntax_call_LR_parser mais prudence
            syntax_error(lexem_type,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(lexem_type,
                                    FATAL_ERROR,
                                    get_catalog(C_EXPR));
        }

        param_ident->set_BOM_type(type);
        param_ident->set_BOM_lexem_type(lexem_type);

        // cas d'arret

        if (cur_lexem->get_lex_type() != L_SCOL)
        {
            encore = FALSE;
        }
        else
        {
            cur_lexem = cur_lexem->get_next_lexem();
        }
    }

    *adr_cur_lexem = cur_lexem;
    TRACE0("<-parse_typed_param");
}

void static parse_param (T_ident_type param_type,
                                  T_item **first_param,
                                  T_item **last_param,
                                  T_op *op,
                                  T_lexem **adr_cur_lexem)
{
    T_lexem *cur_lexem = *adr_cur_lexem;

    T_catalog_msg type_msg_param = (param_type == ITYPE_OP_IN_PARAM)
            ? C_OP_INPUT_PARAM
            : C_OP_OUTPUT_PARAM ;

    int encore = TRUE;
    while (encore == TRUE)
    {
        syntax_check_eof(cur_lexem);

        if (cur_lexem->get_lex_type() != L_IDENT)
        {
            syntax_error(cur_lexem,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(type_msg_param)) ;
        }

        // On a un ident pour param�tre le lexem suivant doit etre un ':'

        T_ident *param_ident = syntax_get_ident(param_type,
                                                (T_item **)first_param,
                                                (T_item **)last_param,
                                                op,
                                                op->get_betree(),
                                                &cur_lexem) ;

        assert(param_ident) ; // A FAIRE
        param_ident->set_ref_op(op) ;
        param_ident->set_ref_machine((T_machine *)op->get_father()) ;

        if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA))
        {
            TRACE0("encore un tour !") ;
            cur_lexem = cur_lexem->get_next_lexem() ;

        }
        else
        {
            encore = FALSE ;
        }
    }

    *adr_cur_lexem = cur_lexem;
}


// Analyse syntaxique d'une op�ration qui peut etre concr�te ou non
void T_op::syntax_analysis_BOM_typed(T_item **adr_first,
                                              T_item **adr_last,
                                              T_item *new_father,
                                              T_betree *new_betree,
                                              T_lexem **adr_cur_lexem)
{
    TRACE6("T_op::syntax_analysis_BOM_typed(%x, %x, %x, %x, %x, local_op %s)",
           adr_first,
           adr_last,
           new_father,
           new_betree,
           *adr_cur_lexem,
           (is_a_local_op == TRUE) ? "TRUE" : "FALSE") ;
    ENTER_TRACE ;

    first_out_param = last_out_param = NULL ;
    T_lexem *cur_lexem = *adr_cur_lexem ;

    ASSERT(cur_lexem) ; // Verifie par l'appelant
    int type_op_determine = FALSE;
    int op_concrete = FALSE;

    /*********************************************************/
    // On a des param�tre de sortie si et seulement si on a:
    //    '('           => op_concrete
    //    ident ','     => op_abstraite
    //    ident '<--'   => op_abstraite
    /*********************************************************/

    T_lex_type type = cur_lexem->get_lex_type() ;
    T_lexem *next_lexem = cur_lexem->get_next_lexem() ;
    syntax_check_eof(next_lexem);

    if (type == L_OPEN_PAREN
            ||
            (type == L_IDENT
             &&
             (next_lexem->get_lex_type() == L_COMMA
              ||
              next_lexem->get_lex_type() == L_RETURNS)))
    {
        // on a des param�tres de retours

        type_op_determine = TRUE;
        if (type == L_OPEN_PAREN)
        {
            cur_lexem = cur_lexem->get_next_lexem();
            syntax_check_eof(cur_lexem);

            op_concrete = TRUE;
            parse_typed_param (ITYPE_OP_OUT_PARAM,
                               (T_item **)&first_out_param,
                               (T_item **)&last_out_param,
                               this,
                               &cur_lexem);

            // Parse le ')'
            if (cur_lexem->get_lex_type() != L_CLOSE_PAREN)
            {
                syntax_error(cur_lexem,
                             MULTI_LINE,
                             get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
                syntax_unexpected_error(cur_lexem,
                                        FATAL_ERROR,
                                        get_catalog(C_CLOSE_PAREN)) ;
            }
            cur_lexem = cur_lexem->get_next_lexem();
            syntax_check_eof(cur_lexem);
        }
        else
        {
            op_concrete = FALSE;
            parse_param (ITYPE_OP_OUT_PARAM,
                         (T_item **)&first_out_param,
                         (T_item **)&last_out_param,
                         this,
                         &cur_lexem);
        }


        if (cur_lexem->get_lex_type() != L_RETURNS)
        {
            syntax_error(cur_lexem,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_RETURNS)) ;
        }
        cur_lexem = cur_lexem->get_next_lexem();
        syntax_check_eof(cur_lexem);
    }

    /*********************************************************/
    // Nom de l'op�ration
    //
    /*********************************************************/

    // Il faut verifier qu'on est bien sur le nom de l'operation
    // Pas forcement le cas, exemple: "a <-- <-- op"

    if (    (cur_lexem->get_lex_type() != L_IDENT)
            && (cur_lexem->get_lex_type() != L_RENAMED_IDENT) )
    {
        syntax_error(cur_lexem,
                     MULTI_LINE,
                     get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
        syntax_ident_expected_error(cur_lexem, FATAL_ERROR) ;
    }

    // Ici cur_lexem est non NULL et pointe sur le nom de l'operation
    T_ident_type ident_type = (is_a_local_op == FALSE)
            ? ITYPE_OP_NAME
            : ITYPE_LOCAL_OP_NAME ;
    if (cur_lexem->get_lex_type() == L_IDENT)
    {
        name = new T_ident(cur_lexem->get_value(),
                           ident_type,
                           NULL,
                           NULL,
                           this,
                           new_betree,
                           cur_lexem) ;
        ASSERT(name != NULL);
    }
    else
    {
        name = new T_renamed_ident(cur_lexem->get_value(),
                                   ident_type,
                                   NULL,
                                   NULL,
                                   this,
                                   new_betree,
                                   cur_lexem) ;
        ASSERT(name != NULL);
    }
    cur_lexem = cur_lexem->get_next_lexem();
    syntax_check_eof(cur_lexem) ;

    /*********************************************************/
    // Parametre de sortie
    //
    // On a des param�tres d'entr�e si et seulement si le lexem courant
    // est une parenth�se.
    /*********************************************************/

    first_in_param = last_in_param = NULL ;
    if (cur_lexem->get_lex_type() == L_OPEN_PAREN)
    {
        cur_lexem = cur_lexem->get_next_lexem();
        syntax_check_eof(cur_lexem);
        // Si on ne sait toujours pas si l'op�ration est concr�tes ou
        // non on teste le lexem suivant
        if (type_op_determine == FALSE)
        {
            type_op_determine = TRUE;
            T_lexem *next_lexem = cur_lexem->get_next_lexem();
            syntax_check_eof(cur_lexem);

            op_concrete = (next_lexem->get_lex_type() == L_BELONGS);
        }

        if (op_concrete == TRUE)
        {
            parse_typed_param (ITYPE_OP_IN_PARAM,
                               (T_item **)&first_in_param,
                               (T_item **)&last_in_param,
                               this,
                               &cur_lexem);
            TRACE2("apres parse_typed_param %s : %s",
                   cur_lexem->get_lex_ascii(),
                   get_lex_type_name(cur_lexem->get_lex_type()));
        }
        else
        {
            parse_param (ITYPE_OP_IN_PARAM,
                         (T_item **)&first_in_param,
                         (T_item **)&last_in_param,
                         this,
                         &cur_lexem);
        }
        TRACE2("apres parse_typed_param %s : %s",
               cur_lexem->get_lex_ascii(),
               get_lex_type_name(cur_lexem->get_lex_type()));

        // Parse le ')'
        if (cur_lexem->get_lex_type() != L_CLOSE_PAREN)
        {
            syntax_error(cur_lexem,
                         MULTI_LINE,
                         get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
            syntax_unexpected_error(cur_lexem,
                                    FATAL_ERROR,
                                    get_catalog(C_CLOSE_PAREN)) ;
        }
        cur_lexem = cur_lexem->get_next_lexem();
        syntax_check_eof(cur_lexem);
    }

    if (cur_lexem->get_lex_type() != L_EQUAL)
    {
        syntax_error(cur_lexem,
                     MULTI_LINE,
                     get_error_msg(E_WHILE_PARSING_OPERATION_HEADER)) ;
        syntax_unexpected_error(cur_lexem,
                                FATAL_ERROR,
                                get_catalog(C_EQUALS)) ;
    }

    // Corps de l'operation
    TRACE0("corps de l'operation") ;
    *adr_cur_lexem = cur_lexem->get_next_lexem() ;

    first_body = last_body = NULL ;
    T_substitution *body = syntax_get_substitution((T_item **)&first_body,
                                                   (T_item **)&last_body,
                                                   this,
                                                   get_betree(),
                                                   adr_cur_lexem) ;

    assert(body != NULL) ; // A FAIRE :: erreur
    EXIT_TRACE ;
}

