/******************************* CLEARSY **************************************
* Fichier : $Id: c_auto_test.cpp,v 2.0 2005-04-25 10:43:55 cc Exp $
* (C) 2002 CLEARSY
*
* Description :		Compilateur B
*					fonctions d'auto test
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
RCS_ID("$Id: c_auto_test.cpp,v 1.5 2005-04-25 10:43:55 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_auto_test.h"

// Fonctions internes

static void check_explicitly_declared(T_ident *ident);
static void check_explicitly_declared(T_ident *ident,
											   int expected);
static void check_homonym_in_abstraction(T_ident *ident);
static void check_homonym_in_abstraction(T_ident *ident,
												  T_ident *expected);
static void check_homonym_in_required_mach(T_ident *ident);
static void check_homonym_in_required_mach(T_ident *ident,
													T_ident *expected);

static T_ident *
       lookup_ident_in_used_mach_list(T_ident *ident,
									  T_used_machine *used_mach);

static T_ident *lookup_ident_in_other_mach(T_ident *ident,
													T_used_machine *umach);

static T_symbol *
       get_ident_name_in_used_mach (T_ident *ident,
								   T_used_machine *used_mach);

static T_ident *lookup_ident_in_mach(T_symbol *ident_name,
											  T_ident *ident,
											  T_machine *mach);
static T_generic_op *lookup_op_in_mach(T_symbol *op_name,
												T_machine *my_mach,
												T_machine *other_mach);

static T_ident *lookup_ident_name_in_list(T_symbol *ident_name,
												   T_ident *first);

// Point d'entr�e:
// Lance la v�rification des champs:
//       homonym_in_abstraction,
//       homonym_in_required_mach,
//       explicitly_declared
// Pour la classe T_ident

void
auto_test_ident (void)
{

  T_object_manager *mana = get_object_manager() ;
  ASSERT(mana != NULL) ;

  T_ident *cur_ident = mana->get_identifiers() ;

  while (cur_ident != NULL)
    {
	  switch (cur_ident->get_ident_type())
		{
		case ITYPE_UNKNOWN:
		case ITYPE_BUILTIN:
		case ITYPE_MACHINE_NAME:
		case ITYPE_ABSTRACTION_NAME:
		case ITYPE_PRAGMA_PARAMETER:
		case ITYPE_DEFINITION_PARAM:
		case ITYPE_USED_MACHINE_NAME:
		case ITYPE_RECORD_LABEL:
		case ITYPE_OP_IN_PARAM:
		case ITYPE_OP_OUT_PARAM:
		  // On ne v�rifie rien.
		  break;
		case ITYPE_CONCRETE_CONSTANT:
		case ITYPE_CONCRETE_VARIABLE:
		case ITYPE_ABSTRACT_SET:
		case ITYPE_OP_NAME:
		case ITYPE_ENUMERATED_VALUE:
		case ITYPE_ABSTRACT_VARIABLE:
		case ITYPE_ABSTRACT_CONSTANT:
		case ITYPE_ENUMERATED_SET:
		case ITYPE_USED_OP_NAME:
		  check_explicitly_declared(cur_ident);
		  check_homonym_in_abstraction(cur_ident) ;
		  check_homonym_in_required_mach(cur_ident) ;
		  break;
		case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
		case ITYPE_MACHINE_SET_FORMAL_PARAM:
		  check_explicitly_declared(cur_ident, TRUE);
		  check_homonym_in_abstraction(cur_ident) ;
		  check_homonym_in_required_mach(cur_ident, NULL) ;
		  break;
		case ITYPE_LOCAL_VARIABLE:
		case ITYPE_LOCAL_QUANTIFIER:
      case ITYPE_ANY_QUANTIFIER:
		case ITYPE_LOCAL_OP_NAME:
		  check_explicitly_declared(cur_ident, TRUE) ;
		  check_homonym_in_abstraction(cur_ident, NULL) ;
		  check_homonym_in_required_mach(cur_ident, NULL) ;
		  break;
		  // Pas de default pour contr�le par gcc
		}
      cur_ident = cur_ident->get_next_ident() ;
    }
}

//
//  V�rification du champ "explicitly_declared"
//

static void
check_explicitly_declared(T_ident *ident)
{
  T_lexem *lex = ident->get_ref_lexem();

  int expected = FALSE;
  if (get_fix_inherited_lexems_mode() == TRUE)
	{
	  expected = (lex != NULL &&
				  0 == strcmp(lex->get_value(),
							  ident->get_name()->get_value()));
	}
  else
	{
	  if (lex != NULL)
		{
		  T_object *father = ident->get_ref_machine()->get_father() ;
		  ASSERT(father->get_node_type() == NODE_BETREE) ;
		  if ( (0 == strcmp(lex->get_file_name()->get_value(),
							((T_betree *)father)->get_file_name()->get_value()))
			   &&
			   0 == strcmp(lex->get_value(),
						   ident->get_name()->get_value()))
			{
			  T_lexem *cur_lexem = lex;
			  while ( (cur_lexem->get_lex_type() != L_REFINEMENT)
					  &&
					  (cur_lexem->get_lex_type() != L_IMPLEMENTATION)
#ifdef BALBULETTE
					  &&
					  (cur_lexem->get_lex_type() != L_MODEL)
#endif
					  &&
					  (cur_lexem->get_lex_type() != L_MACHINE)
					  &&
					  (cur_lexem->get_lex_type() != L_SYSTEM))
				{
				  cur_lexem=cur_lexem->get_prev_lexem();
				}
			  cur_lexem =cur_lexem->get_next_lexem();
			  expected = ( 0 ==
						   strcmp(cur_lexem->get_value(),
								  ident->get_ref_machine()->get_machine_name()->get_name()->get_value()) );
			}
		}
	}
  check_explicitly_declared(ident, expected);
}

static void
check_explicitly_declared(T_ident *ident, int expected)
{
  if (ident->get_explicitly_declared() != expected)
	{
	  user_error(ident,
				 MULTI_LINE,
				 "unexpected value in explicitly_declared field");
	  user_error(ident,
				 MULTI_LINE,
				 "expected %d, got %d",
				 expected,
				 ident->get_explicitly_declared());
	  user_error(ident,
				 CAN_CONTINUE,
				 "ident is %p (%s)",
				 ident,
				 ident->get_name()->get_value());
	}
}

//
//  V�rification du champ "homonym_in_abstraction"
//

static void
check_homonym_in_abstraction(T_ident *ident)
{
  check_homonym_in_abstraction(ident,
							   lookup_ident_in_other_mach(ident, NULL));
}

static void
check_homonym_in_abstraction(T_ident *ident, T_ident *expected)
{
  T_ident *homo = ident->get_homonym_in_abstraction();
  if (homo != expected)
	{
	  user_error(ident,
				 MULTI_LINE,
				 "unexpected value in homonym_in_abstraction field");
	  user_error(ident,
				 MULTI_LINE,
				 "expected %p (%s), got %p (%s)",
				 expected,
				 expected ? expected->get_name()->get_value() : "(null)",
				 homo,
				 homo ? homo->get_name()->get_value() : "(null)");
	  user_error(ident,
				 CAN_CONTINUE,
				 "ident is %p (%s)",
				 ident,
				 ident->get_name()->get_value());
	}
}

//
//  V�rification du champ "homonym_in_required_mach"
//

static void
check_homonym_in_required_mach(T_ident *ident)
{
  T_machine *mach_ref_ident = ident->get_ref_machine() ;

  T_ident *found = NULL;

  if (found == NULL)
	{
	  found = lookup_ident_in_used_mach_list(ident,
											 mach_ref_ident->get_sees());
	}
  if (found == NULL)
	{
	  found = lookup_ident_in_used_mach_list(ident,
											 mach_ref_ident->get_includes());
	}
  if (found == NULL)
	{
	  found = lookup_ident_in_used_mach_list(ident,
											 mach_ref_ident->get_extends());
	}
  if (found == NULL)
	{
	  found = lookup_ident_in_used_mach_list(ident,
											 mach_ref_ident->get_imports());
	}
  if (found == NULL)
	{
	  found = lookup_ident_in_used_mach_list(ident,
											 mach_ref_ident->get_uses());
	}

  check_homonym_in_required_mach(ident, found);
}

static void
check_homonym_in_required_mach(T_ident *ident, T_ident *expected)
{
  TRACE2("check_homonym_in_required_mach($s,$s)",
		 ident->get_name()->get_value(),
		 expected ? expected->get_name()->get_value() : "(null)");
  T_ident *homo = ident->get_homonym_in_required_mach();
  if (homo != expected)
	{
	  user_error(ident,
				 MULTI_LINE,
				 "unexpected value in homonym_in_required_mach field");
	  user_error(ident,
				 MULTI_LINE,
				 "expected %p (%s), got %p (%s)",
				 expected,
				 expected ? expected->get_name()->get_value() : "(null)",
				 homo,
				 homo ? homo->get_name()->get_value() : "(null)");
	  user_error(ident,
				 CAN_CONTINUE,
				 "ident is %p (%s)",
				 ident,
				 ident->get_name()->get_value());
	}
}


//
//  Recherche l'ident IDENT dans la liste de machines requises dont le
//  premier �l�ment est used_mach.
//
static T_ident *
lookup_ident_in_used_mach_list(T_ident *ident,
							   T_used_machine *used_mach)
{
  T_ident *found = NULL;
  while (used_mach != NULL && found == NULL)
	{
	  found = lookup_ident_in_other_mach(ident, used_mach) ;
	  used_mach = (T_used_machine *) used_mach->get_next() ;
	  if ( found != NULL &&  found->get_ident_type() == ITYPE_LOCAL_OP_NAME)
		{
		  found = NULL;
		}
	}
  return found;
}

//
//  Recherche l'ident IDENT dans la machine requise UMACH.
//
//  Cas particulier: si UMACH == NULL, c'est dans l'abstraction qu'on
//  recherche.
//
static T_ident *
lookup_ident_in_other_mach(T_ident *ident, T_used_machine *umach)
{
  T_ident *found = NULL;

  T_machine *abstraction = ident->get_ref_machine()->get_ref_abstraction();

  if (umach != NULL || abstraction != NULL)
	{
	  // On a un endroit o� chercher.

	  switch (ident->get_ident_type())
		{
		case ITYPE_ENUMERATED_VALUE:
		  {
			T_ident * enum_set = (T_ident *) ident->get_father();
			ASSERT(enum_set->get_ident_type() == ITYPE_ENUMERATED_SET);

			T_ident *found_set = lookup_ident_in_other_mach(enum_set, umach);
			if (found_set != NULL)
			  {
				T_ident *values = (T_ident *) found_set->get_values();
				ASSERT(values->is_an_ident());
				found = lookup_ident_name_in_list(ident->get_name(), values);
			  }
			break;
		  }
		case ITYPE_CONCRETE_CONSTANT:
		case ITYPE_ABSTRACT_CONSTANT:
		case ITYPE_CONCRETE_VARIABLE:
		case ITYPE_ABSTRACT_VARIABLE:
		case ITYPE_ABSTRACT_SET:
		case ITYPE_ENUMERATED_SET:
		case ITYPE_OP_NAME:
		case ITYPE_USED_OP_NAME:
		  if (umach != NULL)
			{
			  T_symbol *name_in_umach =
				get_ident_name_in_used_mach(ident, umach);
			  if (name_in_umach != NULL)
				{
				  TRACE3("recherche dans une machine requise: %s de l'ident: %x:%s",
						 name_in_umach->get_value(),
						 ident,
						 ident->get_name()->get_value());
				  found = lookup_ident_in_mach(name_in_umach,
											   ident,
											   umach->get_ref_machine());
				}
			}
		  else
			{
			  found = lookup_ident_in_mach(ident->get_name(),
										   ident,
										   abstraction);
			}
		  break;
		case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
		case ITYPE_MACHINE_SET_FORMAL_PARAM:
		  if (umach == NULL)
			{
			  // On ne peut chercher que dans l'abstraction !
			  found = lookup_ident_name_in_list(ident->get_name(),
												abstraction->get_params());
			}
		  break;
		case ITYPE_OP_IN_PARAM:
		case ITYPE_OP_OUT_PARAM:
		  {
			// On recherche d'abord l'op�ration coll�e avec celle qui
			// contient le param�tre IDENT.
			T_generic_op *found_op = NULL;
			T_generic_op *op = (T_generic_op *) ident->get_father();
			ASSERT(op->is_an_operation() == TRUE);
			// si la fonction du param�tre est une op�ration locale il
			// n'a pas d'homonyme ni dans l'abstraction ni dans les
			// machines requises

			if (op->get_node_type() == NODE_USED_OP ||
				((T_op *)op)->get_is_a_local_op() == FALSE)
			  {
				T_ident *op_name = op->get_name();
				if (umach != NULL)
				  {
					T_symbol *name_in_umach =
					  get_ident_name_in_used_mach(op_name, umach);
					if (name_in_umach != NULL)
					  {
						found_op = lookup_op_in_mach(name_in_umach,
													 NULL,
													 umach->get_ref_machine());
					  }
				  }
				else
				  {
					found_op = lookup_op_in_mach(op_name->get_name(),
												 ident->get_ref_machine(),
												 abstraction);
				  }
			  }

			// Maintenant, on peut rechercher le param�tre dans
			// l'op�ration coll�e.
			if (found_op != NULL)
			  {
				found = lookup_ident_name_in_list(ident->get_name(),
												  found_op->get_in_params());
				if (found == NULL)
				  {
					found =
					  lookup_ident_name_in_list(ident->get_name(),
												found_op->get_out_params());
				  }
			  }
		  }
		  break;
		default:
		  ASSERT(0);				// cas non pr�vu
		  break;
		}
	}
  return found;
}

//
//  Retourne le nom de l'identficateur IDENT tel qu'il devrait apparaitre
//  dans la machine requise USED_MACH.
//
static T_symbol *
get_ident_name_in_used_mach (T_ident *ident, T_used_machine *used_mach)
{
  TRACE2("-->get_ident_name_in_used_mach(%s,%s)",
		 ident->get_name()->get_value(),
		 used_mach->get_name()->get_name()->get_value() );

  T_symbol *mach_prefix = used_mach->get_instance_name() ;
  T_symbol *result = NULL;

  if (ident->get_node_type() == NODE_RENAMED_IDENT)
	{
	  T_renamed_ident *renamed_ident = (T_renamed_ident *)ident;
	  T_symbol *ident_prefix = renamed_ident->get_instance_name();
	  T_symbol *ident_sufix = renamed_ident->get_component_name();

	  if ( (mach_prefix != NULL)
		   &&
		   (mach_prefix->compare(ident_prefix) == TRUE) )
		{
		  // l'ident et la machine requise ont le meme pr�fixe.
		  result = ident_sufix;
		}
	  else
		{
		  // l'ident et la machine requise n'ont pas le meme pr�fixe
		  // (ou bien la machine requise n'a pas de pr�fixe).
		  // Donc, l'ident ne peut pas etre dans la machine requise.
		 result = NULL;
		}
	}
  else if (ident->get_ident_type() == ITYPE_USED_OP_NAME)
	// Dans ce cas aussi le nom de l'ident peut etre pr�fix�
	{
	  TRACE0("type used_op_name");
	  ASSERT(ident->get_father()->get_node_type() == NODE_USED_OP);
	  T_used_op *used_op = (T_used_op *)ident->get_father();
	  ASSERT(used_op != NULL);
	  T_symbol *used_op_prefix = used_op->get_instance_name() ;
	  T_symbol *used_op_sufix = used_op->get_real_op_name() ;

	  if ( (mach_prefix != NULL)
		   &&
		   (used_op_prefix != NULL)
		   &&
		   (mach_prefix->compare(used_op_prefix) == TRUE) )
		{
		  // l'op�ration et la machine requise ont le meme pr�fixe.
		  result = used_op_sufix;
		}
	  else if ( (mach_prefix == NULL)
				&&
				(used_op_prefix == NULL) )
		{
		  result = used_op_sufix;
		}
	  else
		{
		  // l'op�ration et la machine requise n'ont pas le meme pr�fixe
		  // (ou bien la machine requise n'a pas de pr�fixe).
		  // Donc, l'op�ration ne peut pas etre dans la machine requise.
		 result = NULL;
		}
	}
  else

	{
	  // L'ident n'a pas de pr�fixe
	  if (mach_prefix == NULL)
		{
		  result = ident->get_name();
		}
	  else
		{
		  switch (ident->get_ident_type())
			{
			case ITYPE_ENUMERATED_VALUE:
			case ITYPE_ABSTRACT_SET:
			case ITYPE_ENUMERATED_SET:
			case ITYPE_CONCRETE_CONSTANT:
			case ITYPE_ABSTRACT_CONSTANT:
			  result = ident->get_name();
			  break;
			case ITYPE_CONCRETE_VARIABLE:
			case ITYPE_ABSTRACT_VARIABLE:
			case ITYPE_OP_NAME:
			case ITYPE_USED_OP_NAME:
			  result = NULL;
			  break;
			default:
			  ASSERT(0);				// cas non pr�vu
			  break;
			}
		}
	}
  TRACE1("%s<-- get_ident_name_in_used_mach", result?result->get_value():"null");
  return result;
}


//
//  Recherche l'ident de nom IDENT_NAME de nature semblable � IDENT
//  dans la machine MACH.
//
static T_ident *
lookup_ident_in_mach(T_symbol *ident_name,
					 T_ident *ident,
					 T_machine *mach)
{
  T_ident *found = NULL;

  switch (ident->get_ident_type())
	{
	case ITYPE_CONCRETE_CONSTANT:
	case ITYPE_ABSTRACT_CONSTANT:
	  found = mach->find_constant(ident_name);
	  break;
	case ITYPE_CONCRETE_VARIABLE:
	case ITYPE_ABSTRACT_VARIABLE:
	  found = mach->find_variable(ident_name);
	  break;
	case ITYPE_ABSTRACT_SET:
	case ITYPE_ENUMERATED_SET:
	  found = mach->find_set(ident_name);
	  break;
	case ITYPE_OP_NAME:
	case ITYPE_USED_OP_NAME:
	  {
		T_generic_op *found_op = lookup_op_in_mach(ident_name,
												   ident->get_ref_machine(),
												   mach);
		if (found_op != NULL)
		  {
			TRACE2("On a trouve:%s dans la machine %s",
				   found_op->get_name()->get_name()->get_value(),
				   mach->get_machine_name()->get_name()->get_value());
			found = found_op->get_name();
		  }
		break;
	  }
	default:
	  ASSERT(0);				// cas non pr�vu
	  break;
	}
  return found;
}


static T_generic_op *
lookup_op_in_mach(T_symbol *op_name,
				  T_machine *my_mach,
				  T_machine *other_mach)
{
  T_generic_op *found = NULL;

  if (other_mach != NULL)
	{
	  found = other_mach->find_op(op_name);
	}
  if (found == NULL && my_mach != NULL)
	{
	  found = my_mach->find_local_op(op_name);
	}
  return found;
}


static T_ident *
lookup_ident_name_in_list(T_symbol *ident_name, T_ident *first)
{
  T_ident *cur_ident = first;

  while ( cur_ident != NULL)
	{
	  if (ident_name->compare(cur_ident->get_name()) == TRUE)
		{
		  break;
		}
	  cur_ident = (T_ident *) cur_ident->get_next();
	}
  return cur_ident;
}
