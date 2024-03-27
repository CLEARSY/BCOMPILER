/******************************* CLEARSY **************************************
* Fichier : $Id: c_defini.cpp,v 2.0 1999-06-09 12:38:13 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Definitions
*
* Compilation : 	-DDEBUG_DEFINITIONS pour tracer les traitements
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
RCS_ID("$Id: c_defini.cpp,v 1.25 1999-06-09 12:38:13 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_definition
//
T_definition::T_definition(T_item **adr_first,
									T_item **adr_last,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem **adr_ref_lexem,
									T_lexem *last_code_lex)
  : T_item(NODE_DEFINITION,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   *adr_ref_lexem)
{
#ifdef DEBUG_DEFINITIONS
  TRACE6("T_definition(%x)::T_definition(%x, %x, %x, %x, %x)",
		 this,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;
  ENTER_TRACE ;
#endif

  syntax_analysis(adr_ref_lexem, last_code_lex) ;
}

T_definition::~T_definition(void)
{
  TRACE2("T_definition(%x:%s)::~T_definition()", this, name->get_value()) ;
}

#define DEFINE_TEMPLATES
#include "s_lrstck.cpp"


// Pile des definitions
// FIRST
template class T_lr_stack<T_definition *> ;
static T_lr_stack<T_definition *> *first_def_stack_sop = NULL ;

void reset_first_def_stack(void)
{
  TRACE0("reset_first_def_stack") ;
  first_def_stack_sop = new T_lr_stack<T_definition *>(STYPE_FIFO) ;
  TRACE1("STACK first_def_stack_sop = %x", first_def_stack_sop) ;
}

void delete_first_def_stack(void)
{
  delete first_def_stack_sop ;
  first_def_stack_sop = NULL ;
}

void _push_first_def_stack(T_definition *def, const char *file, int line)
{
  TRACE4("DEF_STACK push_first_def_stack(%x:%s) appele depuis %s:%d",
		 def, (def == NULL) ? "NULL" : def->get_name()->get_value(),
		 file, line) ;
  first_def_stack_sop->push(def) ;
}

T_definition *pop_first_def_stack(void)
{
  TRACE1("ici first_def_stack_sop = %x", first_def_stack_sop) ;
  ASSERT(first_def_stack_sop != NULL) ;
  T_definition *res = first_def_stack_sop->pop() ;
  TRACE3("DEF_STACK pop_first_def_stack(%x)->%x:%s",
		 first_def_stack_sop,
		 res,
		 (res == NULL) ? "NULL" : res->get_name()->get_value()) ;
  return res ;
}

int is_first_def_stack_empty(void)
{
  return first_def_stack_sop->is_empty() ;
}

// Pile de travail
static T_lr_stack<T_definition *> *tmp_first_def_stack_sop = NULL ;
void reset_tmp_first_def_stack(void)
{
  tmp_first_def_stack_sop = new T_lr_stack<T_definition *>(STYPE_FIFO) ;
}

void push_tmp_first_def_stack(T_definition *def)
{
  TRACE3("DEF_STACK (%x), push_tmp_first_def_stack(%x:%s)",
		 tmp_first_def_stack_sop,
		 def, (def == NULL) ? "NULL" : def->get_name()->get_value()) ;
  tmp_first_def_stack_sop->push(def) ;
}

void copy_tmp_first_def_stack(void)
{
  first_def_stack_sop = tmp_first_def_stack_sop ;
  TRACE1("STACK first_def_stack_sop = %x", first_def_stack_sop) ;
}

// Pile des definitions
// LAST
//template class T_lr_stack<T_definition *> ;
static T_lr_stack<T_definition *> *last_def_stack_sop = NULL ;

void reset_last_def_stack(void)
{
  last_def_stack_sop = new T_lr_stack<T_definition *>(STYPE_FIFO) ;
}

void delete_last_def_stack(void)
{
  delete last_def_stack_sop ;
  last_def_stack_sop = NULL ;
}

void push_last_def_stack(T_definition *def)
{
  TRACE2("DEF_STACK push_last_def_stack(%x:%s)",
		 def,
		 (def == NULL) ? "null" : def->get_name()->get_value()) ;
  last_def_stack_sop->push(def) ;
}

T_definition *pop_last_def_stack(void)
{
  T_definition *res = last_def_stack_sop->pop() ;
  TRACE3("DEF_STACK pop_last_def_stack(%x)->%x:%s",
		 last_def_stack_sop,
		 res,
		 (res == NULL) ? "null" : res->get_name()->get_value()) ;
  return res ;
}

int is_last_def_stack_empty(void)
{
  return last_def_stack_sop->is_empty() ;
}

// Echange des piles
void swap_def_stacks(void)
{
  TRACE2("avant swap_def_stacks first %x last %x",
		 first_def_stack_sop, last_def_stack_sop) ;
  T_lr_stack<T_definition *>* tmp = first_def_stack_sop ;
  first_def_stack_sop = last_def_stack_sop ;
  TRACE1("STACK first_def_stack_sop = %x", first_def_stack_sop) ;
  last_def_stack_sop = tmp ;
  TRACE2("apres swap_def_stacks first %x last %x",
		 first_def_stack_sop, last_def_stack_sop) ;
}

// Pile de travail
static T_lr_stack<T_definition *> *tmp_last_def_stack_sop = NULL ;
void reset_tmp_last_def_stack(void)
{
  tmp_last_def_stack_sop = new T_lr_stack<T_definition *>(STYPE_FIFO) ;
}

void push_tmp_last_def_stack(T_definition *def)
{
  tmp_last_def_stack_sop->push(def) ;
}

void copy_tmp_last_def_stack(void)
{
  last_def_stack_sop = tmp_last_def_stack_sop ;
}

// Pile des definitions
template class T_lr_stack<T_flag *> ;
// Clauses
static T_lr_stack<T_flag *> *def_clause_stack_sop = NULL ;

void reset_def_clause_stack(void)
{
  TRACE0("reset_def_clause_stack") ;
  def_clause_stack_sop = new T_lr_stack<T_flag *>(STYPE_FIFO) ;
}

void delete_def_clause_stack(void)
{
  delete def_clause_stack_sop ;
  def_clause_stack_sop = NULL ;
}

void push_def_clause_stack(T_flag *def)
{
  TRACE1("DEF_STACK push_def_clause_stack(%x)", def) ;
  def_clause_stack_sop->push(def) ;
}

T_flag *pop_def_clause_stack(void)
{
  T_flag *res = def_clause_stack_sop->pop() ;
  TRACE2("DEF_STACK pop_def_clause_stack(%x)->%x", def_clause_stack_sop, res) ;
  return res ;
}

int is_def_clause_stack_empty(void)
{
  return def_clause_stack_sop->is_empty() ;
}

//
//	}{	Constructeur de la classe T_file_definition
//
T_file_definition::T_file_definition(T_symbol *new_name,
											  T_symbol *new_checksum,
											  T_item **adr_first,
											  T_item **adr_last,
											  T_item *new_father,
											  T_betree *new_betree,
											  T_lexem *ref_lexem)
  : T_item(NODE_FILE_DEFINITION,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   ref_lexem)
{
#ifdef DEBUG_FILE_DEFINITIONS
  TRACE7("T_file_definition(%x)::T_file_definition(%x, %x, %x, %x, %x, %x)",
		 this,
		 new_name,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 ref_lexem) ;
  ENTER_TRACE ;
#endif

  name = new_name ;
  path = NULL ;
  checksum = new_checksum ;
}

T_file_definition::~T_file_definition(void)
{
  TRACE2("T_file_definition(%x:%s)::~T_file_definition()", this, name->get_value());
}

// Pile des definitions
// FIRST
template class T_lr_stack<T_file_definition *> ;
static T_lr_stack<T_file_definition *> *first_file_def_stack_sop = NULL ;

void reset_first_file_def_stack(void)
{
  TRACE0("reset_first_file_def_stack") ;
  first_file_def_stack_sop = new T_lr_stack<T_file_definition *>(STYPE_FIFO) ;
  TRACE1("STACK first_file_def_stack_sop = %x", first_file_def_stack_sop) ;
}

void delete_first_file_def_stack(void)
{
  delete first_file_def_stack_sop ;
  first_file_def_stack_sop = NULL ;
}

void _push_first_file_def_stack(T_file_definition *def,
										 const char *file,
										 int line)
{
  TRACE3("DEF_STACK push_first_file_def_stack(%x) appele depuis %s:%d",
		 def,
		 file, line) ;
  TRACE4("DEF_STACK push_first_file_def_stack(%x:%s) appele depuis %s:%d",
		 def, (def == NULL) ? "NULL" :
		 (def->get_name() == NULL) ? "(null)" : def->get_name()->get_value(),
		 file, line) ;
  first_file_def_stack_sop->push(def) ;
}

T_file_definition *pop_first_file_def_stack(void)
{
  TRACE1("ici first_file_def_stack_sop = %x", first_file_def_stack_sop) ;
  T_file_definition *res = first_file_def_stack_sop->pop() ;
  TRACE3("DEF_STACK pop_first_file_def_stack(%x)->%x:%s",
		 first_file_def_stack_sop,
		 res,
		 (res == NULL) ? "NULL" : res->get_name()->get_value()) ;
  return res ;
}

int is_first_file_def_stack_empty(void)
{
  return first_file_def_stack_sop->is_empty() ;
}



// Pile des definitions
// LAST
//template class T_lr_stack<T_file_definition *> ;
static T_lr_stack<T_file_definition *> *last_file_def_stack_sop = NULL ;

void reset_last_file_def_stack(void)
{
  TRACE0("reset_last_file_def_stack") ;
  last_file_def_stack_sop = new T_lr_stack<T_file_definition *>(STYPE_FIFO) ;
  TRACE1("STACK last_file_def_stack_sop = %x", last_file_def_stack_sop) ;
}

void delete_last_file_def_stack(void)
{
  delete last_file_def_stack_sop ;
  last_file_def_stack_sop = NULL ;
}

void push_last_file_def_stack(T_file_definition *def)
{
  TRACE2("DEF_STACK push_last_def_stack(%x:%s)",
		 def, (def == NULL) ? "NULL" : def->get_name()->get_value()) ;
  last_file_def_stack_sop->push(def) ;
}

T_file_definition *pop_last_file_def_stack(void)
{
  TRACE1("ici last_file_def_stack_sop = %x", last_file_def_stack_sop) ;
  T_file_definition *res = last_file_def_stack_sop->pop() ;
  TRACE3("DEF_STACK pop_last_file_def_stack(%x)->%x:%s",
		 last_file_def_stack_sop,
		 res,
		 (res == NULL) ? "NULL" : res->get_name()->get_value()) ;
  return res ;
}

int is_last_file_def_stack_empty(void)
{
  return last_file_def_stack_sop->is_empty() ;
}

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void T_file_definition::make_standalone(void)
{
  TRACE0("T_file_definition::make_standalone(void)");

  // On deconnecte le betree, car tous les constituants du fichier
  // de definitions font reference a lui
  if (this->get_betree() != NULL)
	{
	  this->get_betree()->make_standalone();
	}

  // On coupe les liens herites de T_item
  set_betree(NULL);
  set_father(NULL);
  set_next(NULL);
  set_prev(NULL);
  set_ref_lexem(NULL);
}

// Liberation des piles de l'analyseur de definitions
void lex_unlink_def_stacks(void)
{
}


//
//	}{	Fin du fichier
//



