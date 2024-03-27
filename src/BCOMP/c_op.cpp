/******************************* CLEARSY **************************************
* Fichier : $Id: c_op.cpp,v 2.0 2002-07-16 08:20:42 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Operations
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
RCS_ID("$Id: c_op.cpp,v 1.17 2002-07-16 08:20:42 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Constructeur de la classe T_op
T_op::T_op(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_cur_lexem,
					int new_is_a_local_op)
  : T_generic_op(NODE_OPERATION,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 *adr_cur_lexem)
{
  TRACE6("T_op::T_op(%x, %x, %x, %x, %x, local_op %s)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 *adr_cur_lexem,
		 (new_is_a_local_op == TRUE) ? "TRUE" : "FALSE") ;

  is_a_local_op = new_is_a_local_op ;

  if (get_Use_B0_Declaration() == FALSE)
    {
      syntax_analysis(adr_first,
		      adr_last,
		      new_father,
		      new_betree,
		      adr_cur_lexem) ;
    }
  else
    {
      syntax_analysis_BOM_typed(adr_first,
				adr_last,
				new_father,
				new_betree,
				adr_cur_lexem) ;
    }
  type_checked = FALSE ;

  B0_checked = FALSE;
  B0_body_checked = FALSE;
  B0_header_checked = FALSE;
  is_inlined = FALSE;
  // CTRL Ref: EREN 3
  // Le nom d'une operation ne peut pas comporter de "." dans une abstraction
  if (    (get_betree()->get_root()->get_machine_type() == MTYPE_SPECIFICATION)
		  && (name->get_node_type() == NODE_RENAMED_IDENT) )
	{
	  TRACE1("erreur ! betree = %x", get_betree()) ;
	  TRACE1("%x", get_betree()) ;
	  TRACE1("%x", get_betree()->get_root()) ;
	  TRACE1("%d", get_betree()->get_root()->get_machine_type()) ;

	  syntax_error(name->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_INVALID_RENAMED_OP_IN_SPEC),
				   name->get_name()->get_value()) ;
	}

  TRACE0("Fin de T_op::T_op") ;
}

//
//	Destructeur
//
T_op::~T_op(void)
{
}

T_symbol *T_op::get_op_name(void)
{
  TRACE2("T_op(%x)::get_op_name(%x)", this, name) ;
  return name->get_name() ;
}

T_ident *T_op::get_in_params(void)
{
  return first_in_param ;
}

T_ident *T_op::get_out_params(void)
{
  return first_out_param ;
}

// Cherche le T_op associe en parcourant les ref_op eventuels
// Pour T_op : renvoie this
T_op *T_op::get_definition(void)
{
  return this ;
}

// Constructeur pour les clients
T_op::T_op(T_symbol *new_op_name,
					T_ident *new_first_in_param,
					T_ident *new_last_in_param,
					T_ident *new_first_out_param,
					T_ident *new_last_out_param,
					T_substitution *new_first_body,
					T_substitution *new_last_body,
					T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree)
  : T_generic_op(NODE_OPERATION,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 NULL)
{
  TRACE7("T_op(%x)::T_op(%x, %x, %x, %d, %x, %x)",
		 this,
		 new_op_name,
		 first_body,
		 last_body,
		 adr_first,
		 adr_last,
		 new_betree) ;

  name = new T_ident(new_op_name->get_value(),
					 ITYPE_OP_NAME,
					 NULL,
					 NULL,
					 this,
					 new_betree,
					 NULL) ;

  first_body = new_first_body ;
  last_body = new_last_body ;

  is_a_local_op = FALSE ;

  first_in_param = new_first_in_param ;
  last_in_param = new_last_in_param ;
  first_out_param = new_first_out_param ;
  last_out_param = new_last_out_param ;
  type_checked = FALSE ;
}

// Constructeur pour les clients
T_op::T_op(T_ident *new_op_name,
					T_ident *new_first_in_param,
					T_ident *new_last_in_param,
					T_ident *new_first_out_param,
					T_ident *new_last_out_param,
					T_substitution *new_first_body,
					T_substitution *new_last_body,
					T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree)
  : T_generic_op(NODE_OPERATION,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 NULL)
{
  TRACE7("T_op(%x)::T_op(%x, %x, %x, %d, %x, %x)",
		 this,
		 new_op_name,
		 first_body,
		 last_body,
		 adr_first,
		 adr_last,
		 new_betree) ;

  name = new_op_name ;
  first_body = new_first_body ;
  last_body = new_last_body ;

  is_a_local_op = FALSE ;

  first_in_param = new_first_in_param ;
  last_in_param = new_last_in_param ;
  first_out_param = new_first_out_param ;
  last_out_param = new_last_out_param ;
  type_checked = FALSE ;

  B0_checked = FALSE;
  B0_header_checked = FALSE;
  B0_body_checked = FALSE;
  is_inlined = FALSE;
}

// Pour une operation clonee depuis une autre operation, mise a jour
// recursive des liens pour que les father, ... pointent sur
// la machine new_machine
void T_op::fix_links(T_machine *new_machine)
{
  TRACE4("T_op(%x:%s)::fix_links(%x:%s)",
		 this,
		 name->get_name()->get_value(),
		 new_machine,
		 new_machine->get_machine_name()->get_name()->get_value()) ;

  set_father(new_machine) ;
  name->set_father(this) ;
  name->reset_machine_and_op_def() ;
}

T_ident *T_op::get_name(void)
{
  TRACE3("T_op(%x)::get_name() -> %x:%s", this, name, name->get_class_name()) ;
  return name ;
}


// Calcul de la precondition et du corps de l'operation, avec
// la semantique du GOP actuel (voir commentaire dans c_op.cpp)
// Explication : le GOP ne voit pas les BEGIN ... END autour
// de PRE .. donc pour lui op = PRE P THEN S END c'est pareil
// que op = BEGIN PRE P THEN S END (autant de BEGIN ... END
// qu'on veut) - par contre dans ce cas il faut que le BEGIN
// .. END ne comporte que le PRE.
//
// Donc : une operation a une precondition ssi elle est du type :
// op = [BEGIN_PRECOND] P [THEN] S [END_PRECOND]
// avec BEGIN_PRECOND = 'BEGIN'* 'PRE'
// et END_PRECOND = 'END'* END
//
// A reconsiderer quand on fera le nouveau GOP
//
void T_op::get_precond_and_body(T_precond **adr_precond,
										 T_substitution **adr_body)
{
  TRACE2(">> T_op(%x:%s)::get_precond_and_body", this, get_op_name()->get_value()) ;
  ENTER_TRACE ;

  ASSERT(adr_precond != NULL) ;
  ASSERT(adr_body != NULL) ;

  // Memorisation des resultats
  T_precond *precond = NULL ;
  T_substitution *body = NULL ;

  // On recherche si l'operation a une precondition
  T_substitution *cur = first_body ;

  // On fait sauter tous le BEGIN .. END de haut niveau avec a
  // l'interieur une seule susbtitution, de type BEGIN .. END ou PRE P
  // THEN S
  int done = FALSE ; // vaut TRUE en fin de recherche

  while (done == FALSE)
	{
	  switch (cur->get_node_type())
		{
		case NODE_PRECOND :
		  {
			// C'est bon, on a trouve la precondition
			TRACE0("ok precondition trouvee") ;
			precond = (T_precond *)cur ;
			done = TRUE ;
			break ;
		  }

		case NODE_BEGIN :
		  {
			// Il ne faut qu'une seule substitution a l'interieur
			TRACE0("cas d'un BEGIN") ;
			T_begin *begin = (T_begin *)cur ;

			if (begin->get_body()->get_next() == NULL)
			  {
				// Une seule substitution : on continue
				TRACE0("un seule body : on contiue") ;
				cur = begin->get_body() ;
			  }
			else
			  {
				// Il faut arreter la recherche
				TRACE0("plusieurs body : on arrete") ;
				done = TRUE ;
			  }

			break ;
		  }

		default :
		  {
			// Echec
			TRACE1("cur = %s : echec", cur->get_class_name()) ;
			done = TRUE ;
		  }
		}
	}

  // Ici on sait si l'operation a une precondition
  if (precond == NULL)
	{
	  body = first_body ;
	}
  else
	{
	  body = precond->get_body() ;
	}

  *adr_body = body ;
  *adr_precond = precond ;

  EXIT_TRACE ;
  TRACE5("<< T_op(%x:%s)::get_precond_and_body -> precond %x, body %x:%s",
		 this,
		 get_op_name()->get_value(),
		 precond,
		 body,
		 body->get_class_name()) ;
}


//
//	{	Fin du fichier
//
