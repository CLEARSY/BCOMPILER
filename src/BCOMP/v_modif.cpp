/******************************* CLEARSY **************************************
* Fichier : $Id: v_modif.cpp,v 2.0 1999-09-29 14:21:49 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique : Une substition modifie-t-elle un
*					identificateur donne ?
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
RCS_ID("$Id: v_modif.cpp,v 1.7 1999-09-29 14:21:49 jfm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{ Fonctions "modifies"
//

// Cas general : pas de modification
int T_substitution::modifies(T_ident *ident)
{
  return FALSE ;
  assert(ident == ident) ; // pour le warning
}

// Fonction auxiliaire de recherche dans une liste
static int subst_list_modifies(T_substitution *list, T_ident *ident)
{
  T_substitution *cur = list ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return FALSE ;
		}

	  if (cur->modifies(ident) == TRUE)
		{
		  return TRUE ;
		}

	  cur = (T_substitution *)cur->get_next() ;
	}
}

// Classe T_begin
int T_begin::modifies(T_ident *ident)
{
  TRACE3("T_begin(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_affect
int T_affect::modifies(T_ident *ident)
{
  TRACE3("T_affect(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  ENTER_TRACE ;
  T_expr *cur = first_name ;

  while (cur != NULL)
	{
	  // on extrait l'identificateur (3 cas: x:=1, f(x):=1, tab(x):=1) ou NULL
	  T_ident *found_ident = fetch_ident(cur) ;

	  if (    (found_ident != NULL)
		   && (found_ident->get_definition() == ident) )
		{
		  TRACE0("-> TRUE") ;
		  EXIT_TRACE ;
		  return TRUE ;
		}

	  cur = (T_expr *)cur->get_next() ;
	}

  TRACE0("-> FALSE") ;
  EXIT_TRACE ;
  return FALSE ;
}
//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_affect::is_read_only(void)
{
  TRACE0("T_affect::is_read_only");
  T_expr *cur_exp = get_lvalues();
  while (cur_exp != NULL)
	{
	  T_ident *found_ident = fetch_ident(cur_exp) ;
	  if ( (found_ident != NULL) &&
		   ( (found_ident->get_ref_machine()->get_specification()->
			  find_variable(found_ident->get_name()))
			 != NULL))
		{
		  return FALSE;
		}
	  cur_exp = (T_expr*)cur_exp->get_next();
	}

  return TRUE;
}

// Classe T_precond
int T_precond::modifies(T_ident *ident)
{
  TRACE3("T_precond(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

int T_precond::is_read_only(void)
{
  TRACE0("T_precond::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

// Classe T_assert
int T_assert::modifies(T_ident *ident)
{
  TRACE3("T_assert(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_label
int T_label::modifies(T_ident *ident)
{
  TRACE3("T_label(%x)::modifies(%x:%s)",
                 this,
                 ident,
                 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}
// Classe T_jumpif
int T_jumpif::modifies(T_ident *ident)
{
	return 0;
}
// Classe T_witness
int T_witness::modifies(T_ident *ident)
{
  TRACE3("T_witness(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}


// Classe T_or
int T_or::modifies(T_ident *ident)
{
  TRACE3("T_or(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_choice
int T_choice::modifies(T_ident *ident)
{
  TRACE3("T_choice(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  if (subst_list_modifies(first_body, ident) == TRUE)
	{
	  return TRUE ;
	}

  T_or *cur_or = first_or ;
  while (cur_or != NULL)
	{
	  if (cur_or->modifies(ident) == TRUE)
		{
		  return TRUE ;
		}
	  cur_or = (T_or *)cur_or->get_next() ;
	}
  return FALSE ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_choice::is_read_only(void)
{
  TRACE0("T_choice::is_read_only");

  //les substitutions
  T_substitution *cur = get_body();
  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  //les OR
  T_or *cur_or = get_or();
  while ( cur_or != NULL )
	{
	  T_substitution *cur_sub = cur_or->get_body();
	  while ( cur_sub != NULL )
		{
		  if ( cur_sub->is_read_only() == FALSE )
			{
			  return FALSE ;
			}
		  //ok par construction
		  cur_sub =(T_substitution*) cur_sub->get_next();
		}
	  //ok par construction
	  cur_or = (T_or*)cur_or->get_next();
	}
  return TRUE;
}

// Classe T_else
int T_else::modifies(T_ident *ident)
{
  TRACE3("T_else(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_if
int T_if::modifies(T_ident *ident)
{
  TRACE3("T_if(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  if (subst_list_modifies(first_then_body, ident) == TRUE)
	{
	  return TRUE ;
	}

  T_else *cur_else = first_else ;
  while (cur_else != NULL)
	{
	  if (cur_else->modifies(ident) == TRUE)
		{
		  return TRUE ;
		}
	  cur_else = (T_else *)cur_else->get_next() ;
	}
  return FALSE ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_if::is_read_only(void)
{
  TRACE0("T_if::is_read_only");
  // il faut verifier que toutes les substitutions du THEN
  // sont bien read_only
  T_substitution *cur = get_then_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  //puis pour tous les elsif et le else
  T_else *cur_else = get_else();
  while ( cur_else !=NULL)
	{
	  //pour toutes les substitutions de l'aternative courante
	  T_substitution *cur_sub = cur_else->get_body();

	  while ( cur_sub != NULL )
		{
		  if ( cur_sub->is_read_only() == FALSE )
			{
			  return FALSE ;
			}
		  //ok par construction
		  cur_sub =(T_substitution*) cur_sub->get_next();
		}
	  cur_else = (T_else*)cur_else->get_next();
	}
  return TRUE;
}

// Classe T_when
int T_when::modifies(T_ident *ident)
{
  TRACE3("T_when(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}


// Classe T_select
int T_select::modifies(T_ident *ident)
{
  TRACE3("T_select(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  if (subst_list_modifies(first_then_body, ident) == TRUE)
	{
	  return TRUE ;
	}

  T_when *cur_when = first_when ;
  while (cur_when != NULL)
	{
	  if (cur_when->modifies(ident) == TRUE)
		{
		  return TRUE ;
		}
	  cur_when = (T_when *)cur_when->get_next() ;
	}
  return FALSE ;
}
//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_select::is_read_only(void)
{
  TRACE0("T_select::is_read_only");
  // ici il faut verifier que toutes les substitutions du THEN
  // sont bien read_only
  T_substitution *cur = get_then_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
		  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  // ici il faut verifier que toutes les substitutions du WHEN
  // sont bien read_only
  T_when *cur_when = get_when();

  while ( cur_when != NULL )
	{
	  T_substitution *cur_sub = cur_when->get_body();

	  while ( cur_sub != NULL )
		{
		  if ( cur_sub->is_read_only() == FALSE )
			{
			  return FALSE ;
			}
		  //ok par construction
		  cur_sub =(T_substitution*) cur_sub->get_next();
		}
	  //ok par construction
	  cur_when=(T_when*) cur_when->get_next();
	}
  return TRUE;
}


// Classe T_any
int T_any::modifies(T_ident *ident)
{
  TRACE3("T_any(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_let
// Pas la peine d'etudier les valuations car elles concernent uniquement
// les variables locales du let
int T_let::modifies(T_ident *ident)
{
  TRACE3("T_let(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_let::is_read_only(void)
{
  TRACE0("T_let::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

// Classe T_becomes_member_of
int T_becomes_member_of::modifies(T_ident *ident)
{
  TRACE3("T_becomes_member_of(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  T_ident *cur_ident = first_ident ;
  while (cur_ident != NULL)
	{
      if (cur_ident->get_definition() == ident)
		{
		  return TRUE ;
		}
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}
  return FALSE ;
}

// Classe T_becomes_such_that
int T_becomes_such_that::modifies(T_ident *ident)
{
  TRACE3("T_becomes_such_that(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  T_ident *cur_ident = first_ident ;
  while (cur_ident != NULL)
	{
      if (cur_ident->get_definition() == ident)
		{
		  return TRUE ;
		}
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}
  return FALSE ;
}

// Classe T_var
int T_var::modifies(T_ident *ident)
{
  TRACE3("T_var(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_var::is_read_only(void)
{
  TRACE0("T_var::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}
// Classe T_case_item
int T_case_item::modifies(T_ident *ident)
{
  TRACE3("T_case_item(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

// Classe T_case
int T_case::modifies(T_ident *ident)
{
  TRACE3("T_case(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  T_case_item *cur = first_case_item ;
  while (cur != NULL)
	{
	  if (cur->modifies(ident) == TRUE)
		{
		  return TRUE ;
		}

	  cur = (T_case_item *)cur->get_next() ;
	}

  return FALSE ;
}

// Classe T_op_call
int T_op_call::modifies(T_ident *ident)
{
  TRACE3("T_op_call(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  T_item *cur = first_out_param ;

  while (cur != NULL)
	{
	  if (cur->is_an_ident() == TRUE)
		{
		  if (cur->make_ident()->get_def() == ident)
			{
			  return TRUE ;
			}
		}

	  cur = (T_item *)cur->get_next() ;
	}

  return FALSE ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_op_call::is_read_only(void)
{
  TRACE0("T_op_call::is_read_only");
  //on verifie que les parametres de sortie ne sont pas des variables
  //de la spec
  T_item *cur_item = get_out_params();
   while (cur_item != NULL)
	{
	  if (  ( cur_item->get_node_type()==NODE_IDENT ) &&
			(  ( ((T_ident*)cur_item)->get_ref_machine()->get_specification()->
				 find_variable(((T_ident*)cur_item)->get_name()))
			   != NULL))
		{
		  return FALSE;
		}
	  cur_item = (T_item*)cur_item->get_next();
	}

  return TRUE ;
}

// Classe T_while
int T_while::modifies(T_ident *ident)
{
  TRACE3("T_while(%x)::modifies(%x:%s)",
		 this,
		 ident,
		 ident->get_name()->get_value()) ;
  return subst_list_modifies(first_body, ident) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_while::is_read_only(void)
{
  TRACE0("T_while::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

//
//	}	Fin des methodes de type "modifies"
//

