/******************************* CLEARSY **************************************
* Fichier : $Id: c_uop.cpp,v 2.0 1999-06-09 12:38:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Operations utilisees
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
RCS_ID("$Id: c_uop.cpp,v 1.10 1999-06-09 12:38:31 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{ Constructeur de la classe T_used_op
//
T_used_op::T_used_op(T_item **adr_first,
							  T_item **adr_last,
							  T_item *father,
							  T_betree *betree,
							  T_lexem **adr_cur_lexem)
  : T_generic_op(NODE_USED_OP,adr_first, adr_last, father, betree, *adr_cur_lexem)
{
  TRACE2("T_used_op(%x)::T_used_op (%s)",
		 this,
		 (*adr_cur_lexem)->get_lex_name()) ;

  ref_op = NULL ;
  syntax_analysis(adr_cur_lexem) ;
}

// Constructeur simple
T_used_op::T_used_op(T_symbol *new_real_op_name,
							  T_symbol *new_instance_name,
							  T_ident *new_name,
							  T_generic_op *new_ref_op,
							  T_item **adr_first,
							  T_item **adr_last,
							  T_item *new_father,
							  T_betree *new_betree,
							  T_lexem *new_ref_lexem)
  : T_generic_op(NODE_USED_OP,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 new_ref_lexem)
{
  TRACE1("T_used_op(%x)::T_used_op", this) ;
  real_op_name = new_real_op_name ;
  instance_name = new_instance_name ;

  char *full_name ;
  if (instance_name == NULL)
	{
	  // on caste en (char *) car on sait qu'on n'ecrira pas
	  full_name = (char *)real_op_name->get_value() ;
	}
  else
	{
	  int len = 1 + instance_name->get_len() + real_op_name->get_len() ;
	  full_name = new char[len + 1] ;
	  sprintf(full_name, "%s.%s",
			  instance_name->get_value(),
			  real_op_name->get_value()) ;
	}
  name = new T_ident(full_name,
					 ITYPE_USED_OP_NAME,
					 NULL,
					 NULL,
					 this,
					 this->get_betree(),
					 new_ref_lexem) ;
  ref_op = new_ref_op ;
  // pour forcer la mise a jour du def lors de la
  // phase de resolution de portee, et pour eviter que
  // l'operation soit consideree comme une operation developpee
  // de la machine lors de l'analyse des tables de visibilite,
  // on met ident_type = ITYPE_UNKNOWN
  //  name->set_ident_type(ITYPE_UNKNOWN) ;
}

// Destructeur de la classe T_used_op
T_used_op::~T_used_op(void)
{
  TRACE1(">> T_used_op(%x)::~T_used_op", this) ;
}

T_symbol *T_used_op::get_op_name(void)
{
return name->get_name() ;
}

T_ident *T_used_op::get_in_params(void)
{
  return (ref_op == NULL) ? (T_ident *)NULL : ref_op->get_in_params() ;
}

T_ident *T_used_op::get_out_params(void)
{
  return (ref_op == NULL) ? (T_ident *)NULL : ref_op->get_out_params() ;
}

// Pour une operation clonee depuis une autre operation, mise a jour
// recursive des liens pour que les father, ... pointent sur
// la machine new_machine
void T_used_op::fix_links(T_machine *new_machine)
{
  TRACE4("T_used_op(%x:%s)::fix_links(%x:%s)",
		 this,
		 name->get_name()->get_value(),
		 new_machine,
		 new_machine->get_machine_name()->get_name()->get_value()) ;

  set_father(new_machine) ;
  name->set_father(this) ;
  name->reset_machine_and_op_def() ;
}

T_ident *T_used_op::get_name(void)
{
  return name ;
}

// Cherche le T_op associe en parcourant les ref_op eventuels
T_op *T_used_op::get_definition(void)
{
  return ref_op->get_definition() ;
}

//
//	}{	Fin du fichier
//

