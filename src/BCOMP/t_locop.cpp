/******************************* CLEARSY **************************************
* Fichier : $Id: t_locop.cpp,v 2.0 1999-10-18 14:31:55 jfm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Controles semantiques supplementaires pour les
*					operations locales
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
RCS_ID("$Id: t_locop.cpp,v 1.4 1999-10-18 14:31:55 jfm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_tstamp.h"
#include "t_locop.h"

T_op_call_tree::T_op_call_tree(void)
  : T_item(NODE_OP_CALL_TREE), first_son(NULL), last_son(NULL)
{}

const char *T_op_call_tree::get_class_name(void) const
{
  return "T_op_call_tree" ;
}

const T_op_call_tree *T_op_call_tree::get_father() const
{
  //  return ((const T_op_call_tree*) T_item::get_father());
  return 0;
}

const T_symbol *T_op_call_tree::get_op_name() const
{
  return ((T_op*)this)->get_name()->get_name() ;
}

//
// Controle supplementaire pour les operations locales : pas de cycle (SEMLOC3)
//
// CTRL Ref : SEMLOC3

void T_op::check_local_op_cycle(void)
{
  TRACE2("T_op(%x:%s)::check_local_op_cycle",
		 this,
		 get_op_name()->get_value()) ;

  // On ne fait ce controle qu'en implementation ...
  T_machine *ref_machine = NULL ;

  find_machine_and_op_def(&ref_machine, NULL) ;

  if (ref_machine->get_machine_type() != MTYPE_IMPLEMENTATION)
	{
	  TRACE1("%s pas une implementation",
			 ref_machine->get_machine_name()->get_name()->get_value()) ;
	  return ;
	}

  // ... et pour les implementations d'operations locales
  if (is_a_local_op == TRUE)
	{
	  TRACE0("spec d'op loc, pas implementation") ;
	  return ;
	}

  if (ref_machine->find_local_op(get_op_name()) == NULL)
	{
	  TRACE0("pas une operation locale") ;
	  return ;
	}

  TRACE0("c'est une implementation d'op loc\n");

  //noeud de base de l'arbre d'appel d'op loc
  T_list_link * new_op_call= new T_list_link(get_op_name(),
											 LINK_OTHER,
											 NULL,
											 NULL,
											 NULL,
											 get_betree(),
											 get_ref_lexem());

  // enrichissement de cette liste avec la liste des op transitivement
  // appellees
  TRACE0("avant T_op::build_local_op_call_tree");

  build_local_op_call_tree(new_op_call) ;
}

void T_op::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE2("T_op(%x:%s)::build_local_op_call_tree() -> on propage au corps",
		 this,
		 get_op_name()->get_value()) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call) ;
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_op_call::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_op_call(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  if (get_ref_op() == NULL)
	{
	  TRACE4("%s:%d:%d panic %x ref_op NULL\n",
			 get_ref_lexem()->get_file_name()->get_value(),
			 get_ref_lexem()->get_file_line(),
			 get_ref_lexem()->get_file_column(),
			 this) ;
	  return ;
	}
  TRACE0("T_op_call::build_local_op_call_tree() -> on propage au corps");

  if (get_ref_op()->get_node_type() != NODE_OPERATION)
	{
	  TRACE0("pas un appel d'op loc") ;
	}

  T_op *ref_op = (T_op *)get_ref_op() ;
  if (ref_op->get_is_a_local_op() == TRUE)
	{
	  TRACE0("c'est un appel d'operation locale !") ;

	  //est ce que cette fonction est deja dans l'arbre d'appel ?
	  T_list_link *cur_link = cur_op_call;
	  while(cur_link != NULL)
		{
		  TRACE1("ref_op->get_op_name()->get_value() : %s",
				 ref_op->get_op_name()->get_value());
		  TRACE1("cur_link->get_name())->get_name()->get_value() : %s",
				 ((T_symbol*)(cur_link->follow_indirection()))->
				 get_value());

		  if(  ((T_symbol*)(cur_link->follow_indirection())) ==
			   ref_op->get_op_name())
			{
			  // Erreur cycle
			  semantic_error(this,
							 CAN_CONTINUE,
							 //FATAL_ERROR,
							 get_error_msg(E_CYCLE_IN_IMPLEMENTATION_OF_LOCAL_OP),
							 ref_op->get_op_name()->get_value()) ;
			  //a rajouter : la liste des op du cycle voir dans
			  //T_op::add_local_op_call_list
			  T_list_link *cur_dump = cur_op_call;

			  while (cur_dump != NULL)
				{
				  T_lexem *cur_lexem = cur_dump->get_ref_lexem() ;
				  T_symbol *cur_name = (T_symbol *)cur_dump->get_object() ;
				  semantic_error(cur_lexem,
								 MULTI_LINE,
								 get_error_msg(E_LOCAL_OP_IS_PART_OF_THE_CYCLE),
								 cur_name->get_value()) ;
				  cur_dump = (T_list_link *)cur_dump->get_father() ;

				}
			  stop();
			}
		  cur_link=(T_list_link *)cur_link->get_father();
		}

	  // Appel recursif sur l'implementation
	  TRACE0("Appel recursif sur l'implementation");

	  T_machine *ref_machine = NULL ;
	  ref_op->find_machine_and_op_def(&ref_machine, NULL) ;

	  if (ref_machine != NULL) // reprise sur erreur
		{
		  // recherche de l'implementation
		  T_generic_op *ref_impl = ref_machine->find_op(ref_op->get_op_name());

		  if (ref_impl != NULL) // reprise sur erreur
			{
			  TRACE1(">> appel recursif sur implementation de %s",
					 ref_op->get_op_name()->get_value()) ;
			  ENTER_TRACE ;
			  ((T_op*)ref_impl)->
				build_local_op_call_tree(new T_list_link(ref_op->get_op_name(),
														 LINK_OTHER,
														 NULL,
														 NULL,
														 cur_op_call,
														 get_betree(),
														 get_ref_lexem()));
			  EXIT_TRACE ;
			  TRACE1("<< appel recursif sur implementation de %s",
					 ref_op->get_op_name()->get_value()) ;
			}
		}
	}
}

void T_begin::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_begin(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution *)cur->get_next() ;
	}
}

void T_var::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_var(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution*) cur->get_next() ;
	}
}

void T_assert::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_assert(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_label::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_label(%x)::build_local_op_call_tree() -> on propage au corps",
                 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
        {
          cur->build_local_op_call_tree(cur_op_call);
          cur = (T_substitution*)cur->get_next() ;
        }
}
void T_jumpif::build_local_op_call_tree(T_list_link * cur_op_call) {
}

void T_witness::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_witness(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_if::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_if(%x)::build_local_op_call_tree() -> on propage au corps", this) ;

  T_substitution *cur = get_then_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution*)cur->get_next() ;
	}

  T_else *cur_else = get_else() ;

  while (cur_else != NULL)
	{
	  cur = cur_else->get_body() ;

	  while (cur != NULL)
		{
		  cur->build_local_op_call_tree(cur_op_call);
		  cur = (T_substitution*)cur->get_next() ;
		}

	  cur_else = (T_else *)cur_else->get_next() ;
	}
}

void T_while::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_while(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_case::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_case(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_case_item *cur_case_item = get_case_items() ;

  while (cur_case_item != NULL)
	{
	  T_substitution *cur = cur_case_item->get_body() ;

	  while (cur != NULL)
		{
		  cur->build_local_op_call_tree(cur_op_call);
		  cur = (T_substitution*)cur->get_next() ;
		}

	  cur_case_item = (T_case_item *)cur_case_item->get_next() ;
	}
}

void T_precond::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_precond(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  //par construction...
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_choice::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_choice(%x)::build_local_op_call_tree() -> on propage au corps",
		 this) ;

  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  //par construction...
	  cur = (T_substitution*)cur->get_next() ;
	}
  T_or * cur_or = get_or();

  while (cur_or != NULL)
	{
	  T_substitution *cur_sub = cur_or->get_body() ;
	  while (cur_sub != NULL)
		{
		  cur_sub->build_local_op_call_tree(cur_op_call);
		  //par construction...
		  cur_sub=(T_substitution*)cur_sub->get_next();
		}
	  //par construction...
	  cur_or=(T_or*)cur_or->get_next();
	}
}

void T_any::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_any(%x)::build_local_op_call_tree() -> on propage au corps",
		 this);
  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  //par construction...
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_let::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_let(%x)::build_local_op_call_tree() -> on propage au corps",
		 this);
  T_substitution *cur = get_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  //par construction...
	  cur = (T_substitution*)cur->get_next() ;
	}
}

void T_select::build_local_op_call_tree(T_list_link * cur_op_call)
{
  TRACE1("T_select(%x)::build_local_op_call_tree() -> on propage au corps",
		 this);
  T_substitution *cur = get_then_body() ;

  while (cur != NULL)
	{
	  cur->build_local_op_call_tree(cur_op_call);
	  //par construction...
	  cur = (T_substitution*)cur->get_next() ;
	}

  T_when * cur_when = get_when();

  while ( cur_when != NULL )
	{
	  T_substitution *cur_sub = cur_when->get_body();
	  while (cur_sub != NULL)
		{
		  cur_sub->build_local_op_call_tree(cur_op_call);
		  //par construction...
		  cur_sub=(T_substitution*)cur_sub->get_next();
		}
	  cur_when = (T_when*) cur_when->get_next();
	}
}

//
// Controle supplementaire pour les operations locales :
//
// b) pas d'appel en // d'op d'une meme instance de machine importee
// dans une spec d'op loc
// CTRL Ref : SEMLOC 5
void T_op::check_local_op_calls(void)
{
  TRACE2("T_op(%x:%s)::check_local_op_cycle",
		 this,
		 get_op_name()->get_value()) ;

  // On construit la liste sans doublon des machines des operations appellees
  T_list_link *first_call = NULL ;
  T_list_link *last_call = NULL ;

  build_op_call_ref_machine_list(this,
								 (T_item **)&first_call,
								 (T_item **)&last_call) ;

}

// Ajout d'un appel d'op loc dans une liste sans doublon
// la methode verifie l'unicite du nom et emet un message d'erreur le cas echeant
void T_op::add_op_call_ref_machine_list(T_symbol *ref_machine_name,
												 T_lexem *ref_lexem,
												 T_item **adr_first,
												 T_item **adr_last)
{
  TRACE2("T_op(%x)::add_op_call_ref_machine_list(%s)",
		 this,
		 ref_machine_name->get_value()) ;

  // Recherche de ref_machine_name dans la liste
  // Cast valide par construction
  T_list_link *cur = *(T_list_link **)adr_first ;

  while (cur != NULL)
	{
	  // Cast valide par construction
	  T_symbol *cur_name = (T_symbol *)cur->get_object() ;

	  if (cur_name == ref_machine_name)
		{
		  // Erreur cycle
		  semantic_error(ref_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_MULTIPLE_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP),
						 ref_machine_name->get_value(),
						 get_op_name()->get_value(),
						 ref_machine_name->get_value()) ;

		  T_list_link *cur_dump =  *(T_list_link **)adr_first ;

		  while (cur_dump != NULL)
			{
			  T_lexem *cur_lexem = cur_dump->get_ref_lexem() ;
			  T_symbol *cur_name = (T_symbol *)cur_dump->get_object() ;
			  semantic_error(cur_lexem,
							 MULTI_LINE,
							 get_error_msg(E_PREVIOUS_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP),
							 ref_machine_name->get_value(),
							 cur_name->get_value()) ;
			  cur_dump = (T_list_link *)cur_dump->get_next() ;
			}

		  // On ne peut pas continuer (sinon on a un bouclage)
		  stop() ;

		}

	  cur = (T_list_link *)cur->get_next() ;
	}

  // Ici on sait que ref_machine_name n'est pas dans la liste : on l'insere en queue
  new T_list_link(ref_machine_name,
				  LINK_OTHER,
				  adr_first,
				  adr_last,
				  get_father(),
				  get_betree(),
				  ref_lexem) ;

}

// cur_op est l'operation depuis laquelle la verification a debute
// la liste doit etre sans doublon (la methode T_op::add_op_call_ref_machine
// s'occupe de la verification et de l'ajout)
void T_item::build_op_call_ref_machine_list(T_op *start_op,
													 T_item **adr_first,
													 T_item **adr_last)
{
  TRACE2("T_item(%x:%s)::build_op_call_ref_machine) -> on ne fait rien",
		 this,
		 get_class_name()) ;
}

void T_op::build_op_call_ref_machine_list(T_op *start_op,
												   T_item **adr_first,
												   T_item **adr_last)
{
  TRACE2(">> T_op(%x:%s)::build_op_call_ref_machine_list",
		 this,
		 get_op_name()->get_value()) ;
  ENTER_TRACE ;

  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE2("<< T_op(%x:%s)::build_op_call_ref_machine_list",
		 this,
		 get_op_name()->get_value()) ;
}

void T_begin::build_op_call_ref_machine_list(T_op *start_op,
													  T_item **adr_first,
													  T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_let::build_op_call_ref_machine_list(T_op *start_op,
													T_item **adr_first,
													T_item **adr_last)
{
  TRACE1(">> T_let(%x)::build_op_call_ref_machine_list", this) ;
  ENTER_TRACE ;
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE1("<< T_let(%x)::build_op_call_ref_machine_list", this) ;
}

void T_if::build_op_call_ref_machine_list(T_op *start_op,
												   T_item **adr_first,
												   T_item **adr_last)
{
  T_item *cur = first_then_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}

  cur = first_else ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_case::build_op_call_ref_machine_list(T_op *start_op,
													 T_item **adr_first,
													 T_item **adr_last)
{
  T_item *cur = first_case_item ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_any::build_op_call_ref_machine_list(T_op *start_op,
													T_item **adr_first,
													T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_else::build_op_call_ref_machine_list(T_op *start_op,
													 T_item **adr_first,
													 T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_case_item::build_op_call_ref_machine_list(T_op *start_op,
														  T_item **adr_first,
														  T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_when::build_op_call_ref_machine_list(T_op *start_op,
													 T_item **adr_first,
													 T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_assert::build_op_call_ref_machine_list(T_op *start_op,
													   T_item **adr_first,
													   T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}
void T_label::build_op_call_ref_machine_list(T_op *start_op,
                                                                                                           T_item **adr_first,
                                                                                                           T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
        {
          cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
          cur = cur->get_next() ;
        }
}
void T_jumpif::build_op_call_ref_machine_list(T_op *start_op,
                                                                                                           T_item **adr_first,
                                                                                                           T_item **adr_last)
{
}

void T_witness::build_op_call_ref_machine_list(T_op *start_op,
													   T_item **adr_first,
													   T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_precond::build_op_call_ref_machine_list(T_op *start_op,
														T_item **adr_first,
														T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_or::build_op_call_ref_machine_list(T_op *start_op,
													   T_item **adr_first,
													   T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_select::build_op_call_ref_machine_list(T_op *start_op,
													   T_item **adr_first,
													   T_item **adr_last)
{
  T_item *cur = first_then_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}

  cur = first_when ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_choice::build_op_call_ref_machine_list(T_op *start_op,
													   T_item **adr_first,
													   T_item **adr_last)
{
  T_item *cur = first_body ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}

  cur = first_or ;

  while (cur != NULL)
	{
	  cur->build_op_call_ref_machine_list(start_op, adr_first, adr_last) ;
	  cur = cur->get_next() ;
	}
}

void T_op_call::build_op_call_ref_machine_list(T_op *start_op,
														T_item **adr_first,
														T_item **adr_last)
{
  TRACE2("T_op_call(%x:%s)::build_op_call_ref_machine_list",
		 this,
		 get_ref_op()->get_op_name()->get_value()) ;
  // Si on appelle une operation d'une machine inclue ou etendue alors
  // on ajoute le nom de cette operation
  if (get_ref_op()->get_node_type() == NODE_OPERATION)
	{
	  T_op *op = (T_op *)get_ref_op() ;

	  if (op->get_is_a_local_op() == TRUE)
		{
		  TRACE0("appel d'op loc") ;
		  return ;
		}
	}

  T_generic_op *op_def = get_ref_op() ;

  T_machine *def_machine = NULL ;
  T_machine *start_machine = NULL ;

  op_def->find_machine_and_op_def(&def_machine, NULL) ;
  start_op->find_machine_and_op_def(&start_machine, NULL) ;


  TRACE2("start_machine %s def_machine %s",
		 start_machine->get_machine_name()->get_name()->get_value(),
		 def_machine->get_machine_name()->get_name()->get_value()) ;

  if (    (start_machine->get_imports(def_machine) == TRUE)
	   || (start_machine->get_extends(def_machine) == TRUE) )
	{
	  // On ajoute
	  TRACE0("on ajoute l'appel") ;
	  T_symbol *def_name = def_machine->get_machine_name()->get_name() ;
	  start_op->add_op_call_ref_machine_list(def_name,
											 get_ref_lexem(),
											 adr_first,
											 adr_last) ;
	}
#ifdef TRACE
  else
	{
	  TRACE0("machine ni vue ni importee") ;
	}
#endif
}


// Controles supplementaires en cas d'appel d'une operation locale
// SEMLOC 4a : on ne peut pas utiliser comme parametre de sortie une variable
// concrete modifiee dans la spec de l'op loc
// CTRL REF SEMLOC 4a
void T_op_call::semloc_4a(void)
{
  if (get_ref_op()->get_node_type() != NODE_OPERATION)
	{
	  return ;
	}

  T_op *def = (T_op *)get_ref_op() ;

  if (def->get_is_a_local_op() == FALSE)
	{
	  return ;
	}

  T_item *cur = first_out_param ;

  while (cur != NULL)
	{
	  if (cur->is_an_ident() == TRUE)
		{
		  T_ident *cur_ident = (T_ident *)cur ;
		  cur_ident = cur_ident->get_definition() ;
		  T_ident_type cur_itype ;
		  cur_itype = cur_ident->get_definition()->get_ident_type() ;

		  if (cur_itype == ITYPE_CONCRETE_VARIABLE)
			{
			  T_substitution *cur_subst = def->get_body() ;

			  while (cur_subst != NULL)
				{
				  if (cur_subst->modifies(cur_ident) == TRUE)
					{
					  // Erreur !
					  semantic_error(cur,
									 CAN_CONTINUE,
									 get_error_msg(E_INVALID_OUTPUT_PARAM_CONC_VAR_IS_MODIFIED_IN_LOCAL_OP_SPEC),
									 def->get_op_name()->get_value(),
									 cur_ident->get_name()->get_value(),
									 cur_ident->get_name()->get_value(),
									 def->get_op_name()->get_value()) ;
				  semantic_error(cur_subst,
								 MULTI_LINE,
								 get_error_msg(E_LOC_OF_MODIF_OF_CONC_VAR_IN_LOCAL_OP_SPEC),
								 cur_ident->get_name()->get_value(),
								 def->get_op_name()->get_value()) ;
					}
				  cur_subst = (T_substitution *)cur_subst->get_next() ;
				}
			}
		}

	  // Parametre de sortie suivant
	  cur = cur->get_next() ;
	}
}

// SEMLOC 4b : on ne peut pas appeller une operation importee dans
// une specification d'operation locale avec comme parametre de
// sortie une variable importee modifiee dans la specification de
// l'operation importee
// CTRL REF SEMLOC 4b
void T_op_call::semloc_4b(void)
{
  T_op *op_def = NULL ;
  T_machine *machine_def = NULL ;
  find_machine_and_op_def(&machine_def, &op_def) ;

  if (op_def == NULL)
	{
	  return ;
	}

  if (op_def->get_is_a_local_op() == FALSE)
	{
	  return ;
	}

  // Ok c'est un appel d'operation dans une spec d'op loc
  // On regarde si l'op appelle provient d'une machine importee
  T_machine *ref_machine ;
  get_ref_op()->find_machine_and_op_def(&ref_machine, NULL) ;

  if (    (machine_def->get_imports(ref_machine) == FALSE)
	   && (machine_def->get_extends(ref_machine) == FALSE) )
	{
	  // Ce n'est pas le cas
	  return ;
	}

  T_op *def = get_ref_op()->get_definition() ;

  if (def == NULL)
	{
	  // reprise sur erreur
	  TRACE0("panic") ;
	  return ;
	}

  T_item *cur = first_out_param ;

  while (cur != NULL)
	{
	  if (cur->is_an_ident() == TRUE)
		{
		  T_ident *cur_ident = (T_ident *)cur ;
		  cur_ident = cur_ident->get_definition() ;
		  T_ident_type cur_itype ;
		  cur_itype = cur_ident->get_definition()->get_ident_type() ;

		  T_substitution *cur_subst = def->get_body() ;

		  while (cur_subst != NULL)
			{
			  if (cur_subst->modifies(cur_ident) == TRUE)
				{
				  // Erreur !
				  semantic_error(cur,
								 CAN_CONTINUE,
								 get_error_msg(E_IN_OP_LOCAL_OP_CALL_INVALID_IMPORTED_OUTPUT_PARAM_IS_MODIFIED_IN_IMPORTED_OP),
								 def->get_op_name()->get_value(),
								 cur_ident->get_name()->get_value(),
								 cur_ident->get_name()->get_value(),
								 def->get_op_name()->get_value()) ;
			  semantic_error(cur_subst,
							 MULTI_LINE,
							 get_error_msg(E_LOC_OF_IMPORTED_VAR_VAR_IN_IMPORTED_OP),
							 cur_ident->get_name()->get_value(),
							 def->get_op_name()->get_value()) ;
				}
			  cur_subst = (T_substitution *)cur_subst->get_next() ;
			}
		}

	  // Parametre de sortie suivant
	  cur = cur->get_next() ;
	}


}

//
//	}{	Fin des controles supplementaires pour les op loc
//
