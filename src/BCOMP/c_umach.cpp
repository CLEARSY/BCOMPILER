/******************************* CLEARSY **************************************
* Fichier : $Id: c_umach.cpp,v 2.0 2002-02-07 10:38:35 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Machines utilisees
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
RCS_ID("$Id: c_umach.cpp,v 1.12 2002-02-07 10:38:35 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{ Constructeur de la classe T_used_machine
//
T_used_machine::T_used_machine(int parse_params,
										T_use_type new_use_type,
										T_item **adr_first,
										T_item **adr_last,
										T_item *father,
										T_betree *betree,
										T_lexem **adr_cur_lexem)
: T_item(NODE_USED_MACHINE,adr_first, adr_last, father, betree, *adr_cur_lexem)
{
  TRACE3("T_used_machine(%x)::T_used_machine(%s) use_type=%d",
		 this,
		 (*adr_cur_lexem)->get_lex_name(),
		 new_use_type) ;

  ref_machine = NULL ;
  syntax_analysis(parse_params, new_use_type, adr_cur_lexem) ;
}

// Destructeur
T_used_machine::~T_used_machine(void)
{
  TRACE1("T_used_machine(%x)::~T_used_machine()", this) ;
}

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void T_used_machine::make_standalone(void)
{
  TRACE0("T_used_machine::make_standalone(void)");

  // On coupe tous les liens vers un objet à partir du quel on peut atteindre
  // d'autres machines ou betree.
  ref_machine = NULL;

  // On coupe les liens herites de T_item
  // On garde next car le MS s'en sert
  set_betree(NULL);
  set_father(NULL);
  set_prev(NULL);
  set_ref_lexem(NULL);
}

// Calcul du pathname
void T_used_machine::make_pathname(void)
{
  if (pathname != NULL)
	{
	  // Deja fait
	  return ;
	}

  TRACE2(">> T_used_machine(%x:%s)::make_pathname",
		 this,
		 name->get_name()->get_value()) ;
  ENTER_TRACE ;

  // On calcule le pathname
  // pathname = pathname pere (jusqu'au dernier '.') + name
  T_symbol *ref_name = get_name()->get_name() ;

  // Calcul du pere :
  // reference->get_father() = T_flag de chargement
  // T_flag->father = le pere
  // Cast valide par construction
  ASSERT(get_father()->get_node_type() == NODE_FLAG) ;
  ASSERT(get_father()->get_father()->get_node_type() == NODE_MACHINE) ;
  T_machine *father = (T_machine *)get_father()->get_father() ;

  // Calcul du pathname du pere
  T_used_machine *father_context = father->get_context() ;

  if (father_context == NULL)
	{
	  // Charge par la racine
	  TRACE1("%s est charge par la racine", ref_name->get_value()) ;
	  pathname = ref_name ;
	}
  else
	{
	  // Cas general
	  TRACE2("%s est charge par %s",
			 ref_name->get_value(),
			 father_context->get_name()->get_name()->get_value()) ;

	  father_context->make_pathname() ;

	  // Path du pere (i.e. i1.BB dans i1.BB)
	  char *father_pathname ;

	  // Prefixe du path du pere (i.e. i1 dans i1.BB)
	  char *prefix ;

	  // Calcul
	  father_pathname = new char[father_context->get_pathname()->get_len() + 1] ;
          sprintf(father_pathname, "%s", father_context->get_pathname()->get_value()) ;

	  prefix = strrchr(father_pathname, '.') ;
	  TRACE1("prefix = %s", (prefix == NULL) ? "null" : prefix) ;

	  if (prefix == NULL)
		{
		  // Pas de renommage dans cette branche
		  pathname = ref_name ;
		}
	  else
		{
		  // Branche avec renommage : on coupe au niveau du '.'
		  *prefix = '\0' ;
		  int len = strlen(father_pathname) + 1 + ref_name->get_len() ;
		  char *tmp_pathname = new char[len + 1] ;
		  sprintf(tmp_pathname, "%s.%s", father_pathname, ref_name->get_value()) ;
		  TRACE1("tmp_pathname = <%s>", tmp_pathname) ;
		  pathname = lookup_symbol(tmp_pathname, len) ;
		  delete [] tmp_pathname ;
		}

	  delete [] father_pathname ;
	}

  EXIT_TRACE ;
  TRACE3("<< T_used_machine(%x:%s)::make_pathname -> %s",
		 this,
		 name->get_name()->get_value(),
		 pathname->get_value()) ;
}

//
//	}{	Fin du fichier
//

