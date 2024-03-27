/******************************* CLEARSY **************************************
* Fichier : $Id: c_skel.cpp,v 2.0 2007-07-27 15:19:46 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					G�n�rateur de squelette
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
RCS_ID("$Id: c_skel.cpp,v 1.5 2007-07-27 15:19:46 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

void add_op(T_generic_op *generic_op,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname,
			 T_item **adr_first,
			 T_item **adr_last);

void add_sub(T_substitution *sub,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname,
			 T_item **adr_first,
			 T_item **adr_last);

void add_pred(T_predicate *pred,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname);

void add_expr(T_expr *expr,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname);
void add_ident(T_ident *cur_var,
		T_betree *betree,
		T_machine *root,
		int *cur_line,
		int *cur_column,
		T_symbol *sym_fname,
		int cr);
void add_expr_list(T_expr *expr,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname);

/*!
 * Indicate whether the given operation is within the operation
 * list.
 *
 * \param operations the operation list
 * \param op the operation
 * \return true if the name of op is part of operations
 */
static bool list_contains(char **operations, T_generic_op *op)
{
	if(operations)
	{
		for(int i=0; 1; ++i)
		{
			const char *op_name = op->get_name()->get_name()->get_value();
			if(operations[i])
			{
				if(strcmp(operations[i], op_name) == 0)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

// Creation d'un raffinement d'un Betree (qui doit etre une spec ou
// une machine ...). Si create_imp vaut TRUE, on cree une
// implementation, sinon c'est un raffinement que l'on cree
// new_name contient le nom du betree a creer
// Si do_semantic vaut TRUE, l'analyse semantique du betree
// est effectuee si necessaire, et des informations semantiques
// sont ajoutees (operations promues)
T_betree *create_betree_refinement(T_betree *abstraction,
                                            const char* converterName,
											const char *new_name,
											int create_imp,
											int do_semantic,
											char *svdi,
											int nb_operations,
											char **operations)
{
  TRACE4(">> create_betree_refinement(%x:%s, imp=%s) new_name=%s",
		 abstraction,
		 abstraction->get_betree_name()->get_value(),
		 (create_imp == TRUE) ? "TRUE" : "FALSE",
		 new_name) ;
  ENTER_TRACE ; ENTER_TRACE ;

  // Il faut tout d'abord faire une analyse semantique de
  // l'abstraction pour avoir des informations sur les operations
  // promues
  if ( (do_semantic == TRUE) && (abstraction->get_status() == BST_SYNTAXIC) )
	{
          compiler_semantic_analysis(abstraction, converterName) ;
	}

  verbose_describe(get_catalog((create_imp == FALSE)
							   ? C_CREATING_REFINEMENT : C_CREATING_IMPLEMENTATION),
				   new_name,
				   abstraction->get_betree_name()->get_value()) ;

  // Creation du betree
  T_betree *betree = new T_betree(new_name) ;
  int len = strlen(new_name) ;
  char *new_file_name = new char[len + 4] ;
  sprintf(new_file_name, "%s.%s", new_name, (create_imp == FALSE) ? "ref" : "imp") ;
  T_symbol *sym_fname = lookup_symbol(new_name, len) ;
 // Creation de la machine
  lex_set_first_lexem(NULL) ;
  lex_set_last_lexem(NULL) ;

  TRACE1("sym_fname=%s",
		  sym_fname->get_value()) ;


  // Machine
  T_lexem *first_lexem = new T_lexem((create_imp == FALSE)
									 ? L_REFINEMENT : L_IMPLEMENTATION,
									 NULL,
									 0,
									 1,
									 1,
									 sym_fname) ;

  T_machine *root = new T_machine((create_imp == FALSE)
								  ? MTYPE_REFINEMENT : MTYPE_IMPLEMENTATION,
								  new_file_name,
								  betree,
								  &first_lexem,
								  FALSE) ;

  // Nom de la machine
  T_lexem *ident_lexem = new T_lexem(L_IDENT,
									 new_name,
									 len,
									 2,
									 get_tab_width(),
									 sym_fname) ;
  root->machine_name = new T_ident(new_name,
					ITYPE_MACHINE_NAME,
					NULL,
					NULL,
					root,
					betree,
					ident_lexem) ;

  // Refines
  T_lexem *refines_lexem = new T_lexem(L_REFINES, NULL, 0, 3, 1, sym_fname) ;
  root->refines_clause = new T_flag(root, betree, refines_lexem) ;

  // Nom de la spec
  T_lexem *abs_lexem = new T_lexem(L_IDENT,
								   abstraction->get_betree_name()->get_value(),
								   len,
								   4,
								   get_tab_width(),
								   sym_fname) ;

  root->abstraction_name = new T_ident(abstraction->get_betree_name()->get_value(),
					ITYPE_MACHINE_NAME,
					NULL,
					NULL,
					root,
					betree,
					abs_lexem) ;

  int cur_line = 6 ;
  T_machine *abs_root = abstraction->get_root() ;

  // Repeter les sees eventuels
  if (abs_root->get_sees() != NULL && svdi[0]== '1')
	{
	  T_lexem *sees_lexem = new T_lexem(L_SEES,NULL,0,cur_line,1,sym_fname) ;
	  (void)new T_flag(root, betree, sees_lexem) ;
	  cur_line ++ ;
	  int cur_column = get_tab_width() ;
	  T_used_machine *cur_sees = abs_root->get_sees() ;
	  while (cur_sees != NULL)
		{
		  cur_column = get_tab_width() ;
		  T_symbol *sees_symb = cur_sees->get_name()->get_name() ;
		  (void)new T_lexem(L_IDENT,
							sees_symb->get_value(),
							sees_symb->get_len(),
							cur_line,
							cur_column,
							sym_fname) ;
		  cur_column += sees_symb->get_len() ;
		  cur_sees = (T_used_machine *)cur_sees->get_next() ;
		  if (cur_sees != NULL)
			{
			  (void)new T_lexem(L_COMMA, NULL, 0, cur_line, cur_column, sym_fname) ;
			  cur_line ++ ; // pour le ", "
			}
		}
	  cur_line += 2 ; // c'est plus joli en sautant une ligne
	}

  // Repeter les abstract_variables eventuels
  if (abs_root->get_abstract_variables_clause() != NULL && svdi[1]== '1')
	{
	  T_lexem *ab_lexem = new T_lexem(L_ABSTRACT_VARIABLES,NULL,0,cur_line,1,sym_fname) ;
	  (void)new T_flag(root, betree, ab_lexem) ;
	  cur_line ++ ;
	  int cur_column = get_tab_width() ;
	  T_ident *cur_var = abs_root->get_abstract_variables() ;
	  add_ident(cur_var,
			  betree,
			  root,
			  &cur_line,
			  &cur_column,
			  sym_fname,
			  TRUE);
	  cur_line += 2 ; // c'est plus joli en sautant une ligne
	}

  // Repeter les définitions eventuelles
  if (abstraction->get_definitions_clause() != NULL && svdi[2]== '1')
	{
	  T_lexem *def_lexem = new T_lexem(L_DEFINITIONS,NULL,0,cur_line,1,sym_fname) ;
	  (void)new T_flag(root, betree, def_lexem) ;
	  cur_line ++ ;
	  int cur_column = get_tab_width() ;
	  T_definition *cur_def = abstraction->get_definitions() ;
	  // Il faut les parser dans l'ordre inverse:
	  if (cur_def != NULL) {
		  while (cur_def->get_next() != NULL)
			  cur_def = (T_definition *) cur_def->get_next();
		  while (cur_def != NULL)
		  {
			  cur_column = get_tab_width() ;
			  T_symbol *sees_symb = cur_def->get_name() ;
			  (void)new T_lexem(L_IDENT,
					  sees_symb->get_value(),
					  sees_symb->get_len(),
					  cur_line,
					  cur_column,
					  sym_fname) ;
			  cur_column += sees_symb->get_len();
			  if (cur_def->get_params() != NULL) {
				  (void)new T_lexem(L_OPEN_PAREN, NULL, 0,cur_line,cur_column, sym_fname) ;
				  cur_column++;
				  add_ident(cur_def->get_params(),
						  betree,
						  root,
						  &cur_line,
						  &cur_column,
						  sym_fname,
						  FALSE);
				  (void)new T_lexem(L_CLOSE_PAREN, NULL, 0,cur_line,cur_column, sym_fname) ;
				  cur_column++;

			  }
			  (void)new T_lexem(L_REWRITES, NULL, 0,cur_line,cur_column, sym_fname) ;
			  cur_column += 2;
			  T_lexem* lex = cur_def->get_rewrite_rule();
			  int ref_line = lex->get_file_line();
			  int ref_column = lex->get_file_column();
			  int line = cur_line;
			  int col = cur_column;
			  while (lex != NULL) {
				  line = cur_line+lex->get_file_line()-ref_line;
				  col = cur_column+lex->get_file_column()-ref_column;
				  (void)new T_lexem(lex->get_lex_type(),
						  lex->get_value(),
						  lex->get_value_len(),
						  line,
						  col,
						  sym_fname) ;
				  col +=  lex->get_value_len();
				  lex = lex->get_next_lexem();
			  }
			  cur_line = line;
			  cur_column = col;
			  cur_def = (T_definition *)cur_def->get_prev() ;
			  if (cur_def != NULL)
			  {
				  (void)new T_lexem(L_SCOL, NULL, 0, cur_line, cur_column, sym_fname) ;
				  cur_line ++ ; // pour le ", "
			  }
		  }
	  }
	  cur_line += 2 ; // c'est plus joli en sautant une ligne
	}


  // Repeter l'initialisation eventuelle
  if (abs_root->get_initialisation_clause() != NULL && svdi[3]== '1')
	{
	  T_lexem *ini_lexem = new T_lexem(L_INITIALISATION,NULL,0,cur_line,1,sym_fname) ;
	  (void)new T_flag(root, betree, ini_lexem) ;
	  cur_line ++ ;
	  int cur_column = get_tab_width() ;
	  T_substitution *cur_sub = abs_root->get_initialisation() ;
	  T_substitution *first = NULL ;
	  T_substitution *last = NULL ;
	  add_sub(cur_sub,
			 betree,
			 root,
			 &cur_line,
			 &cur_column,
			 sym_fname,
			 (T_item **)&first,
			 (T_item **)&last) ;
	  root->first_initialisation = first ;
	  root->last_initialisation = last ;
	  cur_line += 2 ; // c'est plus joli en sautant une ligne
	}

  // Eventuellement, les operations
  if (   nb_operations > 0 && (
		  (abs_root->get_operations() != NULL)
	   || (abs_root->get_promotes() != NULL) ))
	{
	  // Une clause OPERATIONS
	  T_lexem *op_lexem;
	  if (abs_root->get_machine_type() == MTYPE_SYSTEM)
		  op_lexem = new T_lexem(L_EVENTS,NULL,0,cur_line,1,sym_fname) ;
	  else
		  op_lexem = new T_lexem(L_OPERATIONS,NULL,0,cur_line,1,sym_fname) ;
	  (void)new T_flag(root, betree, op_lexem) ;
	  cur_line += 2 ;

	  // Et des operations
	  T_generic_op *cur_abs_op = abs_root->get_operations() ;
	  T_op *first = NULL ;
	  T_op *last = NULL ;
	  cur_line -= 1 ;
	  int promotes_done = FALSE ;
	  if ( (cur_abs_op == NULL) && (promotes_done == FALSE) )
		{
		  promotes_done = TRUE ;
		  cur_abs_op = abs_root->get_promotes() ;
		  if (    (cur_abs_op != NULL)
				  && (do_semantic == FALSE) )
			{
			  // On ne peut rien faire !!! on n'est qu'au niveau
			  // syntaxique
			  syntax_error(abs_root->get_promotes()->get_ref_lexem(),
						   FATAL_ERROR,
						   get_error_msg(E_CAN_NOT_CREATE_SYNTAXIC_SKEL_REF_PROMOTES),
						   abstraction->get_betree_name()->get_value());
			}

		}

	  int cur_op = 0;
	  while (cur_abs_op != NULL)
	  {
		  int cur_column = get_tab_width() ;
		  if (list_contains(operations, cur_abs_op))
		  {
			  cur_op++;
			  add_op(cur_abs_op,
					  betree,
					  root,
					  &cur_line,
					  &cur_column,
					  sym_fname,
					  (T_item **)&first,
					  (T_item **)&last) ;
			  cur_line++;

			  // La suite
			  cur_abs_op = (T_op *)cur_abs_op->get_next() ;
			  if (cur_op < nb_operations) {
				  (void)new T_lexem(L_SCOL, NULL,0,cur_line,cur_column, sym_fname) ;
				  cur_line++;
			  }
			  cur_line++;
			  if ( (cur_abs_op == NULL) && (promotes_done == FALSE) )
			  {
				  promotes_done = TRUE ;
				  cur_abs_op = abs_root->get_promotes() ;
				  if (    (cur_abs_op != NULL)
						  && (do_semantic == FALSE) )
				  {
					  // On ne peut rien faire !!! on n'est qu'au niveau
					  // syntaxique
					  syntax_error(abs_root->get_promotes()->get_ref_lexem(),
							  FATAL_ERROR,
							  get_error_msg(E_CAN_NOT_CREATE_SYNTAXIC_SKEL_REF_PROMOTES),
							  abstraction->get_betree_name()->get_value());
				  }

			  }
		  }
		  else
			  cur_abs_op = (T_op *)cur_abs_op->get_next() ;

	  }

	  root->first_operation = first ;
	  root->last_operation = last ;
	  cur_line += 1 ;
	}

  // END
  T_lexem *end_lexem = new T_lexem(L_END, NULL, 0, cur_line, 1, sym_fname) ;
  root->end_machine = new T_flag(root, betree, end_lexem) ;

  // Mise a jour des variables globales

  betree->first_lexem = lex_get_first_lexem() ;
  betree->last_lexem = lex_get_last_lexem() ;

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE5(">> create_betree_refinement(%x:%s, imp=%s) --> %x:%s",
		 abstraction,
		 abstraction->get_betree_name()->get_value(),
		 (create_imp == TRUE) ? "TRUE" : "FALSE",
		 betree,
		 betree->get_betree_name()->get_value()) ;

  return betree;
}

// Fonction auxiliare qui traite une operation (propre ou promue)
// Pas static car friend de T_op
void add_op(T_generic_op *generic_op,
					 T_betree *betree,
					 T_machine *root,
					 int* cur_line,
					 int* cur_column,
					 T_symbol *sym_fname,
					 T_item **adr_first,
					 T_item **adr_last)
{
  TRACE2("add_op(%x:%s)", generic_op, generic_op->get_op_name()->get_value()) ;

  int cur_col = get_tab_width() ;

  T_op *cur_op = NULL ;
  switch (generic_op->get_node_type())
	{
	case NODE_OPERATION :
	  {
		cur_op = (T_op *)generic_op ;
		break ;
	  }

	case NODE_USED_OP :
	  {
		T_generic_op *cur_genop = generic_op ;
		while (cur_genop->get_node_type() == NODE_USED_OP)
		  {
			cur_genop = ((T_used_op *)cur_genop)->get_ref_op() ;
		  }
		ASSERT(cur_genop->get_node_type() == NODE_OPERATION) ;
		cur_op = (T_op *)cur_genop ;
		break ;
	  }

	default :
	  {
		internal_error(generic_op->get_ref_lexem(),
					   __FILE__,
					   __LINE__,
					   get_error_msg(E_UNEXPECTED_NODE_TYPE),
					   generic_op->get_node_type(),
					   generic_op->get_class_name()) ;
	  }
	}


  T_ident *first_in = NULL ;
  T_ident *last_in = NULL ;
  T_ident *first_out = NULL ;
  T_ident *last_out = NULL ;

  // parametres d'entree
  T_ident *cur_out = cur_op->get_out_params() ;

  if (cur_out != NULL)
	{
	  while (cur_out != NULL)
		{
		  T_lexem *cur_lexem = new T_lexem(L_IDENT,
										   cur_out->get_name()->get_value(),
										   cur_out->get_name()->get_len(),
										   *cur_line,
										   cur_col,
										   sym_fname) ;

		  (void)new T_ident(cur_out->get_name()->get_value(),
							ITYPE_OP_OUT_PARAM,
							(T_item **)&first_out,
							(T_item **)&last_out,
							root,
							betree,
							cur_lexem) ;
		  cur_col += cur_out->get_name()->get_len() ;

		  cur_out = (T_ident *)cur_out->get_next() ;
		  if (cur_out != NULL)
			{
			  (void)new T_lexem(L_COMMA,
								NULL,
								0,
								*cur_line,
								cur_col,
								sym_fname) ;
			  cur_col += 2 ;
			}
		}

	  // Le '<--'
	  (void)new T_lexem(L_RETURNS, NULL, 0, *cur_line, cur_col, sym_fname) ;
	  cur_col += 4 ;
	}

  // Nom de la machine
  T_lexem *op_lexem = new T_lexem(L_IDENT,
								  cur_op->get_op_name()->get_value(),
								  cur_op->get_op_name()->get_len(),
								  *cur_line,
								  cur_col,
								  sym_fname) ;
  T_op *cur_ref_op = new T_op(adr_first,
							  adr_last,
							  root,
							  betree,
							  &op_lexem) ;

  cur_col += cur_op->get_op_name()->get_len() + 1 ;

  // parametres de sortie
  T_ident *cur_in = cur_op->get_in_params() ;

  if (cur_in != NULL)
	{
	  // Le '('
	  (void)new T_lexem(L_OPEN_PAREN, NULL, 0,*cur_line,cur_col, sym_fname) ;
	  cur_col += 1 ;

	  while (cur_in != NULL)
		{
		  T_lexem *cur_lexem = new T_lexem(L_IDENT,
										   cur_in->get_name()->get_value(),
										   cur_in->get_name()->get_len(),
										   *cur_line,
										   cur_col,
										   sym_fname) ;
		  (void)new T_ident(cur_in->get_name()->get_value(),
							ITYPE_OP_IN_PARAM,
							(T_item **)&first_in,
							(T_item **)&last_in,
							root,
							betree,
							cur_lexem) ;

		  cur_col += cur_in->get_name()->get_len() ;

		  cur_in = (T_ident *)cur_in->get_next() ;
		  if (cur_in != NULL)
			{
			  (void)new T_lexem(L_COMMA,
								NULL,
								0,
								*cur_line,
								cur_col,
								sym_fname) ;
			  cur_col += 2 ;
			}
		}

	  // Le ')'
	  (void)new T_lexem(L_CLOSE_PAREN, NULL,0,*cur_line,cur_col, sym_fname) ;
	  cur_col += 1 ;
	}

  // Cr�ation du "=" "skip" ";"
  (void)new T_lexem(L_EQUAL, NULL, 0, *cur_line, cur_col, sym_fname) ;
  (*cur_line)++;
  cur_col = get_tab_width();
  T_substitution *first = NULL ;
  T_substitution *last = NULL ;

  add_sub(cur_op->get_body(),
		  betree,
		  root,
		  cur_line,
		  &cur_col,
		  sym_fname,
		  (T_item **)&first,
		  (T_item **)&last) ;
  cur_ref_op->first_body = first ;
  cur_ref_op->last_body = last ;

  cur_ref_op->first_in_param = first_in ;
  cur_ref_op->last_in_param = last_in ;
  cur_ref_op->first_out_param = first_out ;
  cur_ref_op->last_out_param = last_out ;
}

void add_sub(T_substitution *sub,
		T_betree *betree,
		T_machine *root,
		int *cur_line,
		int *cur_column,
		T_symbol *sym_fname,
		T_item **adr_first,
		T_item **adr_last) {
	 TRACE3(">> add_sub(%d)%d,%d",
			 sub->get_node_type(),
			 *cur_line,
			 *cur_column) ;
	int col = *cur_column;
	switch (sub->get_node_type()) {
	case NODE_BEGIN : {
		(void) new T_lexem(L_BEGIN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_begin *) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
	case NODE_CHOICE: {
		(void) new T_lexem(L_CHOICE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_choice *) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		T_or* or_sub = ((T_choice *) sub)->get_or();
		while (or_sub != NULL) {
			(void)new T_lexem(L_OR, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_line)++;
			*cur_column = col + get_tab_width();
			add_sub(((T_or *) or_sub)->get_body(),
							betree,
							root,
							cur_line,
							cur_column,
							sym_fname,
							adr_first,
							adr_last);
			(*cur_line)++;
			*cur_column = col;
			or_sub = (T_or*) or_sub->get_next();
		}
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
	case NODE_PRECOND : {
		(void)new T_lexem(L_PRE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_pred(((T_precond*) sub)->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		 *cur_column = col;
		(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_precond*) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
	case NODE_ASSERT : {
		(void)new T_lexem(L_ASSERT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_pred(((T_assert*) sub)->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_assert*) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
        case NODE_LABEL : {
                (void)new T_lexem(L_LABEL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
                (*cur_line)++;
                *cur_column = col + get_tab_width();
                add_expr(((T_label*) sub)->get_expression(),
                                betree,
                                root,
                                cur_line,
                                cur_column,
                                sym_fname);
                (*cur_line)++;
                *cur_column = col;
                (void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
                (*cur_line)++;
                *cur_column = col + get_tab_width();
                add_sub(((T_label*) sub)->get_body(),
                                                betree,
                                                root,
                                                cur_line,
                                                cur_column,
                                                sym_fname,
                                                adr_first,
                                                adr_last);
                (*cur_line)++;
                *cur_column = col;
                (void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
                (*cur_column)+=3;
                break;
        }
        case NODE_IF :{
		(void)new T_lexem(L_IF, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		add_pred(((T_if*) sub)->get_cond(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_if*) sub)->get_then_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		T_else* else_sub = ((T_if *) sub)->get_else();
		while (else_sub != NULL) {
			if (else_sub->get_cond() == NULL) {
			(void)new T_lexem(L_ELSE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			else
			{
				(void)new T_lexem(L_ELSIF, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) += 6;
				add_pred(else_sub->get_cond(),
								betree,
								root,
								cur_line,
								cur_column,
								sym_fname);
				(*cur_line)++;
				*cur_column = col;
				(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			(*cur_line)++;
			*cur_column = col + get_tab_width();
			add_sub(else_sub->get_body(),
							betree,
							root,
							cur_line,
							cur_column,
							sym_fname,
							adr_first,
							adr_last);
			(*cur_line)++;
			*cur_column = col;
			else_sub = (T_else*) else_sub->get_next();
		}
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
	case NODE_SELECT : {
		(void)new T_lexem(L_SELECT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_pred(((T_select*) sub)->get_cond(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_select*) sub)->get_then_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		T_when* when_sub = ((T_select *) sub)->get_when();
		while (when_sub != NULL) {
			if (when_sub->get_cond() == NULL) {
				(void)new T_lexem(L_ELSE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			else {
				(void)new T_lexem(L_WHEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_line)++;
				*cur_column = col + get_tab_width();
				add_pred(when_sub->get_cond(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname);
				(*cur_line)++;
				*cur_column = col;
				(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			(*cur_line)++;
			*cur_column = col + get_tab_width();
			add_sub(when_sub->get_body(),
							betree,
							root,
							cur_line,
							cur_column,
							sym_fname,
							adr_first,
							adr_last);
			(*cur_line)++;
			*cur_column = col;
			when_sub = (T_when*) when_sub->get_next();
		}
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		break;
	}
	case NODE_CASE : {
		int first = TRUE;
		(void)new T_lexem(L_CASE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 5;
		add_expr(((T_case*) sub)->get_case_select(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_OF, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col;
		T_case_item* case_sub = ((T_case *) sub)->get_case_items();
		while (case_sub != NULL) {
			if (case_sub->get_literal_values() == NULL) {
				(void)new T_lexem(L_ELSE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			else {
				if (first) {
					(void)new T_lexem(L_EITHER, NULL, 0, *cur_line, *cur_column, sym_fname) ;
					(*cur_column) += 7;
					first = FALSE;
				}
				else {
					(void)new T_lexem(L_OR, NULL, 0, *cur_line, *cur_column, sym_fname) ;
					(*cur_column) += 3;
				}
				T_item* case_item = case_sub->get_literal_values();
				while (case_item != NULL) {
				  (void)new T_lexem(case_item->get_ref_lexem()->get_lex_type(),
						  case_item->get_ref_lexem()->get_value(),
						  case_item->get_ref_lexem()->get_value_len(),
						  *cur_line,
						  *cur_column,
						  sym_fname) ;
				  (*cur_column) += case_item->get_ref_lexem()->get_value_len();
				  if (case_item->get_next()!= NULL) {
					  (void)new T_lexem(L_COMMA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				  }
				  case_item  = case_item->get_next();
				}
				(*cur_line)++;
				*cur_column = col;
				(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			}
			(*cur_line)++;
			*cur_column = col + get_tab_width();
			add_sub(case_sub->get_body(),
							betree,
							root,
							cur_line,
							cur_column,
							sym_fname,
							adr_first,
							adr_last);
			(*cur_line)++;
			*cur_column = col;
			case_sub = (T_case_item*) case_sub->get_next();
		}
		*cur_column = col + get_tab_width();
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column)+=3;
		break;
	}
	case NODE_LET : {
		int col1;
		(void)new T_lexem(L_LET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 4;
		T_ident *cur_var = ((T_let*) sub)->get_lvalues() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_BE, NULL, 0, *cur_line, col, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		col1 = *cur_column;
		T_valuation* val = ((T_let*) sub)->get_valuations();
		while (val != NULL)
		{
			T_symbol *var_symb = val->get_ident()->get_name();
			(void)new T_lexem(L_IDENT,
					var_symb->get_value(),
					var_symb->get_len(),
					*cur_line,
					*cur_column,
					sym_fname) ;
			(*cur_column) += var_symb->get_len()+1 ;
			(void)new T_lexem(L_EQUAL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 2;
			add_expr(val->get_value(),
					betree,
					root,
					cur_line,
					cur_column,
					sym_fname);
			val = (T_valuation *)val->get_next() ;
			if (val != NULL)
			{
				(void)new T_lexem(L_AND, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_line)++;
				*cur_column = col1;
			}
		}
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_IN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		*cur_column = col + get_tab_width();
		add_sub(((T_let*) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		break;
	}
	case NODE_VAR : {
		(void)new T_lexem(L_VAR, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 4;
		T_ident *cur_var = ((T_var*) sub)->get_lvalues() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_IN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		(*cur_column) = col + get_tab_width();
		add_sub(((T_var*) sub)->get_body(),
						betree,
						root,
						cur_line,
						&col,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		(*cur_column) = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		break;
	}
	case NODE_ANY : {
		(void)new T_lexem(L_ANY, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 4;
		T_ident *cur_var = ((T_any*) sub)->get_identifiers() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_WHERE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		(*cur_column) += get_tab_width();
		add_pred(((T_any*) sub)->get_where(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_THEN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_line)++;
		(*cur_column) += get_tab_width();
		add_sub(((T_any*) sub)->get_body(),
						betree,
						root,
						cur_line,
						cur_column,
						sym_fname,
						adr_first,
						adr_last);
		(*cur_line)++;
		*cur_column = col;
		(void)new T_lexem(L_END, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		break;
	}
	case NODE_AFFECT : {
		add_expr_list(((T_affect*) sub)->get_lvalues(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_column) += 1;
		(void)new T_lexem(L_AFFECT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
                add_expr_list(((T_affect*) sub)->get_values(),
                              betree,
                              root,
                              cur_line,
                              cur_column,
                              sym_fname);
                break;
	}
	case NODE_BECOMES_MEMBER_OF : {
		T_ident *cur_var = ((T_becomes_member_of*) sub)->get_lvalues() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(*cur_column) += 1;
		(void)new T_lexem(L_BECOMES_MEMBER_OF, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(((T_becomes_member_of*) sub)->get_set(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_BECOMES_SUCH_THAT : {
		T_ident *cur_var = ((T_becomes_such_that*) sub)->get_lvalues() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(*cur_column) += 1;
		(void)new T_lexem(L_BELONGS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(((T_becomes_such_that*) sub)->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_WITNESS :
	case NODE_SKIP :
	default : {
		T_lexem* skip_lex =  new T_lexem(L_SKIP, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(void)new T_skip(adr_first, adr_last, root, betree, &skip_lex);
		(*cur_column) +=4;
		break;
	}
	}
	if (sub->get_next() != NULL) {
		switch (sub->get_link_type()) {
		case LINK_PARALLEL:
			(*cur_column) +=1;
			(void)new T_lexem(L_PARALLEL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_line)++;
			*cur_column = col;
			break;
		case LINK_SEQUENCE:
			(*cur_column) +=1;
			(void)new T_lexem(L_SCOL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_line)++;
			*cur_column = col;
			break;
		default :
			break;
		}
		add_sub((T_substitution *) sub->get_next(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				adr_first,
				adr_last);
	}
}


void add_pred(T_predicate *pred,
		T_betree *betree,
		T_machine *root,
		int *cur_line,
		int *cur_column,
		T_symbol *sym_fname) {
	TRACE1("add_pred : %d",pred->get_node_type());
	switch (pred->get_node_type()) {
	case NODE_BINARY_PREDICATE : {
		T_binary_predicate* bin = (T_binary_predicate*) pred;
		add_pred(bin->get_pred1(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_column)+=1;
		switch (bin->get_predicate_type()) {
		case  BPRED_OR :
			(void)new T_lexem(L_L_OR, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case BPRED_IMPLIES :
			(void)new T_lexem(L_IMPLIES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case BPRED_EQUIVALENT :
			(void)new T_lexem(L_EQUIVALENT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		case BPRED_CONJONCTION :
			(void)new T_lexem(L_AND, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		}
		add_pred(bin->get_pred2(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_EXISTENTIAL_PREDICATE : {
		T_existential_predicate* bin = (T_existential_predicate*) pred;
		(void)new T_lexem(L_EXISTS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		T_ident *cur_var = bin->get_variables() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(bin->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_EXPR_PREDICATE :
	case NODE_PREDICATE : {
		T_expr_predicate* bin = (T_expr_predicate*) pred;
		add_expr(bin->get_expr1(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(*cur_column)+=1;
		switch (bin->get_predicate_type()) {
		case EPRED_DIFFERENT :
			(void)new T_lexem(L_DIFFERENT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case EPRED_NOT_BELONGS :
			(void)new T_lexem(L_NOT_BELONGS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case EPRED_NOT_INCLUDED :
			(void)new T_lexem(L_NOT_INCLUDED, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		case EPRED_NOT_STRICT_INCLUDED :
			(void)new T_lexem(L_NOT_STRICT_INCLUDED, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=5;
			break;
		case EPRED_LESS_THAN :
			(void)new T_lexem(L_LESS_THAN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		case EPRED_GREATER_THAN :
			(void)new T_lexem(L_GREATER_THAN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		case EPRED_LESS_THAN_OR_EQUAL :
			(void)new T_lexem(L_LESS_THAN_OR_EQUAL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case EPRED_GREATER_THAN_OR_EQUAL :
			(void)new T_lexem(L_GREATER_THAN_OR_EQUAL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;

                }
		add_expr(bin->get_expr2(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;

	}
	case NODE_NOT_PREDICATE : {
		T_not_predicate* bin = (T_not_predicate*) pred;
		(void)new T_lexem(L_L_NOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 3;
		add_pred(bin->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_PREDICATE_WITH_PARENTHESIS : {
		T_predicate_with_parenthesis* bin = (T_predicate_with_parenthesis*) pred;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(bin->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_TYPING_PREDICATE : {
		T_typing_predicate* bin = (T_typing_predicate*) pred;
		T_expr* expr = bin->get_identifiers();
		while (expr != NULL)
		{
			add_expr(expr,
					betree,
					root,
					cur_line,
					cur_column,
					sym_fname);
			expr = (T_expr *)expr->get_next() ;
			if (expr != NULL)
			{
				(void)new T_lexem(L_COMMA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) += 1;
			}
		}
		switch (bin->get_typing_predicate_type()) {
		case TPRED_EQUAL :
			(void)new T_lexem(L_EQUAL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		case TPRED_BELONGS :
			(void)new T_lexem(L_BELONGS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		case TPRED_INCLUDED :
			(void)new T_lexem(L_INCLUDED, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		case TPRED_STRICT_INCLUDED :
			(void)new T_lexem(L_STRICT_INCLUDED, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		}
		add_expr_list((T_expr*) bin->get_type(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_UNIVERSAL_PREDICATE : {
		T_universal_predicate* bin = (T_universal_predicate*) pred;
		(void)new T_lexem(L_FORALL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		T_ident *cur_var = bin->get_variables() ;
		add_ident(cur_var,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(bin->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	default :
		TRACE1("add_pred : default %d",pred->get_node_type());
		break;
	}
}

void add_expr(T_expr *expr,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname) {
	TRACE1("add_expr : %d",expr->get_node_type());
	switch (expr->get_node_type()) {
	case NODE_ARRAY_ITEM : {
		T_array_item* e = (T_array_item*) expr;
		add_expr((T_expr*) e->get_array_name(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		add_expr(e->get_indexes(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_BINARY_OP : {
		T_binary_op* e = (T_binary_op*) expr;
		add_expr((T_expr*) e->get_op1(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		switch (e->get_operator()) {
		/* 00 '+'		*/	case BOP_PLUS :
			(void)new T_lexem(L_PLUS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 01 '-'		*/	case BOP_MINUS :
			(void)new T_lexem(L_MINUS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 02 '*'		*/	case BOP_TIMES :
			(void)new T_lexem(L_TIMES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 03 '/'		*/	case BOP_DIVIDES :
			(void)new T_lexem(L_DIVIDES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 04 'mod'		*/	case BOP_MOD :
			(void)new T_lexem(L_MOD, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 05 '**'		*/	case BOP_POWER :
			(void)new T_lexem(L_POWER, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 06 '<|'		*/	case BOP_RESTRICT :
			(void)new T_lexem(L_RESTRICTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 07 '<<|'		*/	case BOP_ANTIRESTRICT :
			(void)new T_lexem(L_ANTIRESTRICTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 08 '|>'		*/	case BOP_CORESTRICT :
			(void)new T_lexem(L_CORESTRICTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 09 '|>>'		*/	case BOP_ANTICORESTRICT :
			(void)new T_lexem(L_ANTICORESTRICTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 10 '<+'		*/	case BOP_LEFT_OVERLOAD :
			(void)new T_lexem(L_LEFT_OVERLOAD, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 11 '^'		*/	case BOP_CONCAT :
			(void)new T_lexem(L_CONCAT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 12 '->'		*/	case BOP_HEAD_INSERT :
			(void)new T_lexem(L_HEAD_INSERT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 13 '<-'		*/	case BOP_TAIL_INSERT :
			(void)new T_lexem(L_TAIL_INSERT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 14 '/|\'		*/	case BOP_HEAD_RESTRICT :
			(void)new T_lexem(L_HEAD_RESTRICT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 15 '\|/'		*/	case BOP_TAIL_RESTRICT :
			(void)new T_lexem(L_TAIL_RESTRICT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 16 '..'		*/	case BOP_INTERVAL :
			(void)new T_lexem(L_INTERVAL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 17 '/\'		*/	case BOP_INTERSECTION :
			(void)new T_lexem(L_INTERSECT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 18 '\/'		*/	case BOP_UNION :
			(void)new T_lexem(L_UNION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 19 '|->'		*/	case BOP_MAPLET :
			(void)new T_lexem(L_MAPLET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 20 '<->'		*/	case BOP_SET_RELATION :
			(void)new T_lexem(L_SET_RELATION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=4;
			break;
		/* 21 '*'		*/	case BOP_CONSTANT_FUNCTION :
			(void)new T_lexem(L_TIMES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 22 ';'		*/	case BOP_COMPOSITION :
			(void)new T_lexem(L_SCOL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
			break;
		/* 23 '><'		*/	case BOP_DIRECT_PRODUCT :
			(void)new T_lexem(L_DIRECT_PRODUCT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 24 '||'		*/	case BOP_PARALLEL_PRODUCT :
			(void)new T_lexem(L_PARALLEL, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 25 ','		*/	case BOP_COMMA :
			(void)new T_lexem(L_COMMA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=2;
            break;
		/* 26 '<->>'    */	case BOP_SURJ_RELATION :
			(void)new T_lexem(L_SURJ_RELATION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=5;
			break;
		/* 27 '<<->'    */	case BOP_TOTAL_RELATION :
			(void)new T_lexem(L_TOTAL_RELATION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=5;
			break;
		/* 28 '<<->>'    */	case BOP_TOTAL_SURJ_RELATION :
			(void)new T_lexem(L_TOTAL_SURJ_RELATION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=6;
			break;
                }
		add_expr((T_expr*) e->get_op2(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_COMPREHENSIVE_SET : {
		T_comprehensive_set* e = (T_comprehensive_set*) expr;
		(void)new T_lexem(L_OPEN_BRACES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) +=1;
		add_ident(e->get_identifiers(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
                ++*cur_column;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_BRACES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) ++;
		break;
	}
	case NODE_CONVERSE : {
		T_converse* e = (T_converse*) expr;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CONVERSE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) ++;
		break;
	}
	case NODE_EMPTY_SEQ : {
		T_empty_seq* e = (T_empty_seq*) expr;
		(void)new T_lexem(L_EMPTY_SEQ, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) +=2;
		break;
	}
	case NODE_EMPTY_SET : {
		T_empty_set* e = (T_empty_set*) expr;
		(void)new T_lexem(L_EMPTY_SET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) +=2;
		break;
	}
	case NODE_EXPR_WITH_PARENTHESIS : {
		T_expr_with_parenthesis* e = (T_expr_with_parenthesis*) expr;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_EXTENSIVE_SEQ : {
		T_extensive_seq* e = (T_extensive_seq*) expr;
		(void)new T_lexem(L_OPEN_BRACKET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr_list(e->get_items(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_BRACKET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_EXTENSIVE_SET : {
		T_extensive_set* e = (T_extensive_set*) expr;
		(void)new T_lexem(L_OPEN_BRACES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr_list(e->get_items(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_BRACES, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_GENERALISED_INTERSECTION : {
		T_generalised_intersection* e = (T_generalised_intersection*) expr;
		(void)new T_lexem(L_GINTER, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 5;
		add_expr(e->get_expressions(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_GENERALISED_UNION : {
		T_generalised_union* e = (T_generalised_union*) expr;
		(void)new T_lexem(L_GUNION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 5;
		add_expr(e->get_expressions(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_IDENT : {
		T_ident* e = (T_ident*) expr;
		T_symbol *var_symb = e->get_name();
		(void)new T_lexem(L_IDENT,
				var_symb->get_value(),
				var_symb->get_len(),
				*cur_line,
				*cur_column,
				sym_fname) ;
		(*cur_column) += var_symb->get_len() ;
		break;
	}
	case NODE_IMAGE : {
		T_image* e = (T_image*) expr;
		add_expr(e->get_relation(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_OPEN_BRACKET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_BRACKET, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_LAMBDA_EXPR : {
		T_lambda_expr* e = (T_lambda_expr*) expr;
		(void)new T_lexem(L_LAMBDA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_ident(e->get_variables(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_LITERAL_BOOLEAN : {
		T_literal_boolean* e = (T_literal_boolean*) expr;
		if (e->get_value()) {
			(void)new T_lexem(L_TRUE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 4;
		}
		else {
			(void)new T_lexem(L_FALSE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 5;
		}
		break;
	}
	case NODE_LITERAL_INTEGER : {
		T_literal_integer* e = (T_literal_integer*) expr;
		T_symbol *var_symb = e->get_value()->get_value();
		(void)new T_lexem(L_NUMBER,
				var_symb->get_value(),
				var_symb->get_len(),
				*cur_line,
				*cur_column,
				sym_fname) ;
		(*cur_column) += var_symb->get_len() ;
		break;
	}
        case NODE_LITERAL_REAL : {
                T_literal_integer* e = (T_literal_integer*) expr;
                T_symbol *var_symb = e->get_value()->get_value();
                (void)new T_lexem(L_REAL,
                                var_symb->get_value(),
                                var_symb->get_len(),
                                *cur_line,
                                *cur_column,
                                sym_fname) ;
                (*cur_column) += var_symb->get_len() ;
                break;
        }
        case NODE_OP_RESULT : {
		T_op_result* e = (T_op_result*) expr;
		T_item* i = e->get_op_name();
		switch (i->get_ref_lexem()->get_lex_type()) {
		case L_BOOL : {
			T_lexem *var_symb = i->get_ref_lexem();
			(void)new T_lexem(L_BOOL,
					var_symb->get_value(),
					var_symb->get_value_len(),
					*cur_line,
					*cur_column,
					sym_fname) ;
			(*cur_column) += 4;
			(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 1;
			add_pred((T_predicate*) e->get_in_params(),
					betree,
					root,
					cur_line,
					cur_column,
					sym_fname);
			(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 1;
			break;
		}
		default : {
			add_expr((T_expr*) i,
					betree,
					root,
					cur_line,
					cur_column,
					sym_fname);
			(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 1;
			add_expr_list((T_expr*) e->get_in_params(),
					betree,
					root,
					cur_line,
					cur_column,
					sym_fname);
			(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 1;
			break;
		}
		}
		break;
	}
        case NODE_PI :
            {
		T_pi* e = (T_pi*) expr;

                T_lex_type lex = L_PI;


                (void)new T_lexem(lex, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_ident(e->get_variables(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_QUANTIFIED_INTERSECTION : {
		T_quantified_intersection* e = (T_quantified_intersection*) expr;
		(void)new T_lexem(L_QINTER, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_ident(e->get_variables(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expression(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_QUANTIFIED_UNION : {
		T_quantified_union* e = (T_quantified_union*) expr;
		(void)new T_lexem(L_QUNION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_ident(e->get_variables(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expression(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_RELATION : {
		T_relation* e = (T_relation*) expr;
		add_expr((T_expr*) e->get_src_set(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		switch (e->get_relation_type()) {
		/* 00 '-->' */	case RTYPE_TOTAL_FUNCTION :
			(void)new T_lexem(L_TOTAL_FUNCTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
		/* 01 '>->' */	case RTYPE_TOTAL_INJECTION :
			(void)new T_lexem(L_TOTAL_INJECTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) +=3;
			break;
			/* 02 '-->>'*/	case RTYPE_TOTAL_SURJECTION :
				(void)new T_lexem(L_TOTAL_SURJECTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) +=4;
				break;
			/* 03 '+->' */	case RTYPE_PARTIAL_FUNCTION :
				(void)new T_lexem(L_PARTIAL_FUNCTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) +=3;
				break;
			/* 04 '>+>' */	case RTYPE_PARTIAL_INJECTION:
				(void)new T_lexem(L_PARTIAL_INJECTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) +=3;
				break;
			/* 05 '+->>'*/	case RTYPE_PARTIAL_SURJECTION :
				(void)new T_lexem(L_PARTIAL_SURJECTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) +=4;
				break;
			/* 06 '>->>'*/	case RTYPE_BIJECTION :
				(void)new T_lexem(L_BIJECTION, NULL, 0, *cur_line, *cur_column, sym_fname) ;
				(*cur_column) +=4;
				break;
		}
		add_expr((T_expr*) e->get_dst_set(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
        case NODE_SIGMA : {
		T_sigma* e = (T_sigma*) expr;

                T_lex_type lex = L_SIGMA;

                (void)new T_lexem(lex, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_ident(e->get_variables(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname,
				FALSE);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_DOT, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		(void)new T_lexem(L_OPEN_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_pred(e->get_predicate(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_PIPE, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		add_expr(e->get_expr(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		(void)new T_lexem(L_CLOSE_PAREN, NULL, 0, *cur_line, *cur_column, sym_fname) ;
		(*cur_column) += 1;
		break;
	}
	case NODE_UNARY_OP : {
		T_unary_op* e = (T_unary_op*) expr;
                switch (e->get_operator()) {
                case UOP_MINUS :
                        (void)new T_lexem(L_MINUS, NULL, 0, *cur_line, *cur_column, sym_fname) ;
                        (*cur_column) +=1;
                        break;
                add_expr(e->get_operand(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
    }
	case NODE_LIST_LINK : {
		T_list_link* e = (T_list_link*) expr;
		add_expr((T_expr*) e->get_object(),
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		break;
	}
	case NODE_STRUCT :
	case NODE_RECORD :
	case NODE_RECORD_ACCESS :
	case NODE_RECORD_ITEM :
	case NODE_LITERAL_STRING :
	case NODE_BOUND :
	default :
		TRACE1("add_expr : default %d",expr->get_node_type());
		break;
	}
}

void add_ident(T_ident *cur_var,
		T_betree *betree,
		T_machine *root,
		int *cur_line,
		int *cur_column,
		T_symbol *sym_fname,
		 int cr) {
	TRACE0("add_ident");
	int col = (*cur_column);
	while (cur_var != NULL)
	{
		T_symbol *var_symb = cur_var->get_name();
		(void)new T_lexem(L_IDENT,
				var_symb->get_value(),
				var_symb->get_len(),
				*cur_line,
				*cur_column,
				sym_fname) ;
		(*cur_column) += var_symb->get_len() ;
		cur_var = (T_ident *)cur_var->get_next() ;
		if (cur_var != NULL)
		{
			(void)new T_lexem(L_COMMA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			if (cr) {
				(*cur_line) += 1;
				*cur_column = col;
			}
			else {
				(*cur_column) += 1;
			}
		}
	}
}

void add_expr_list(T_expr *expr,
			 T_betree *betree,
			 T_machine *root,
			 int *cur_line,
			 int *cur_column,
			 T_symbol *sym_fname) {
	TRACE0("add_expr_list");

	int col = (*cur_column);
	while (expr != NULL)
	{
		add_expr(expr,
				betree,
				root,
				cur_line,
				cur_column,
				sym_fname);
		expr = (T_expr *)expr->get_next() ;
		if (expr != NULL)
		{
			(void)new T_lexem(L_COMMA, NULL, 0, *cur_line, *cur_column, sym_fname) ;
			(*cur_column) += 1;
		}
	}

}
//
//	}{ Fin du fichier
//
