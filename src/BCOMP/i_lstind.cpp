/******************************* CLEARSY **************************************
* Fichier : $Id: i_lstind.cpp,v 2.0 1999-07-19 15:58:24 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		corps des classes T_list_index et T_index
*
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
RCS_ID("$Id: i_lstind.cpp,v 1.3 1999-07-19 15:58:24 sl Exp $") ;

// Includes systeme
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Includes Composant Local */
#include "c_api.h"

#ifdef B0C
#include "i_globdat.h"
#include "i_lstind.h"


//
//
// Methodes de T_index
//

T_index::T_index(void)
{
  literal = -1;
  next = NULL;
}

T_index::T_index(T_type *type)
{
  literal = -1;
  next = NULL;

  switch (type->get_node_type())
	{
	case NODE_ENUMERATED_TYPE :
	  {
		// On peut caster, protege par le case
		T_enumerated_type * enumerated = (T_enumerated_type *) type;

		T_ident *type_def =enumerated->get_type_definition()->get_definition();

		ASSERT(type_def->get_last_value()->get_node_type()
			   == NODE_LITERAL_ENUMERATED_VALUE);

		// On peut caster, protege par l'assertion...
		T_literal_enumerated_value *enumerated_value  =
		  (T_literal_enumerated_value *) type_def->get_last_value();

		literal = 0;
		next = new T_index(enumerated_value->get_value());
		break;
	  }
	case NODE_PREDEFINED_TYPE :
	  {
		// On peut caster, protege par le case
		T_predefined_type *predefined = (T_predefined_type *) type;

		switch (predefined->get_type())
		  {
		  case BTYPE_INTEGER :
			{
			  // Par construction...
			  ASSERT(predefined->get_lower_bound() != NULL);
			  ASSERT(predefined->get_upper_bound() != NULL);
			  ASSERT(predefined->get_lower_bound()->get_node_type()
					 == NODE_LITERAL_INTEGER);
			  ASSERT(predefined->get_upper_bound()->get_node_type()
					 == NODE_LITERAL_INTEGER);

			  T_literal_integer *low_bound =
				(T_literal_integer *) predefined->get_lower_bound();
			  T_literal_integer *up_bound =
				(T_literal_integer *) predefined->get_upper_bound();

			  literal = low_bound->get_value()->get_int_value();
			  next = new T_index(up_bound->get_value()->get_int_value());

			  break;
			}
		  case BTYPE_BOOL :
			{
			  literal = 0;
			  next = new T_index(1);
			  break;
			}
		  default :
			{
			  // Catastrophe si on arrive ici
			  ASSERT(FALSE);
			}
		  };
		break;
	  }
	default :
	  {
		// Normalement. on ne peut y passer par construction
		ASSERT(FALSE);
	  }
	}
}

// Constructeur clone
T_index::T_index(T_index *index)
{
  T_index *cur_index = index;
  ASSERT(cur_index != NULL);
  literal = cur_index->get_literal();

  cur_index = cur_index->get_next();
  if (cur_index == NULL)
	{
	  next = NULL;
	}
  else
	{
	 next = new T_index(cur_index);
	}
}

T_index::T_index(int item)
{
  literal = item;
  next = NULL;
}

T_index::~T_index(void)
{
}


// Verifie si les indices du  maplet en parametre correspondent a la liste
int T_index::is_equal(T_item *maplet)
{
  // Par construction maplet est un maplet...
  // Donc un T_binary_op...
  // On peut caster

  T_item * cur_item = ((T_binary_op *) maplet)->get_expr(FALSE);
  T_item * cur_next_item = cur_item->get_next_expr(FALSE);

  T_index *cur_index = this;
  // On parcourt les indices du maplet
  while (cur_next_item != NULL)
	{
	  switch (cur_item->get_node_type())
		{
		case NODE_LITERAL_INTEGER :
		  {
			// On peut caster, protege par le case
			if ( ((T_literal_integer *)cur_item)->get_value()->get_int_value()
				 != cur_index->get_literal() )
			  {
				return FALSE;
			  }

			break;
		  }
		case NODE_IDENT :
		  {
			// C'est une valeur enumere par construction
			// On peut caster, protege par le CASE...
			T_literal_enumerated_value *value=(T_literal_enumerated_value *)
			  ((T_ident *) cur_item)->get_definition();

			// On cast le size_t en int pour eviter le warning...
			if ( int(value->get_value())
				 != cur_index->get_literal() )
			  {
				return FALSE;
			  }
			break;
		  }
		case NODE_LITERAL_BOOLEAN :
		  {
			// On peut caster, protege par le case
			if ( ((T_literal_boolean *)cur_item)->get_value()
				 != cur_index->get_literal() )
			  {
				return FALSE;
			  }
			break;
		  }
		default :
		  {
			// Cas non prevu et non attendu par construction
			ASSERT(FALSE);
			return FALSE;
		  }
		}

	  cur_item = cur_next_item;
	  cur_next_item = cur_next_item->get_next_expr(FALSE);

	  cur_index = cur_index->get_next();
	}

  // La boucle est terminee, on a donc l'egalite...
  return TRUE;

}

// Ajoute un entier a la liste (en queue)
void T_index::add(int item)
{
  if (next == NULL)
	{
	  next = new T_index(item);
	}
  else
	{
	  next->add(item);
	}
}



//
//
// Methodes de T_list_index
//

T_list_index::T_list_index(void)
{
  index = NULL;
  next = NULL;
}

T_list_index::T_list_index(T_index *item)
{
  index = item;
  next = NULL;
}

T_list_index::~T_list_index(void)
{
}

// Ajoute un index a la liste (en queue)
void T_list_index::add(T_index *item)
{
  ASSERT(item != NULL);

  if (index == NULL)
	{
	  index = item;
	  return;
	}

  if (next == NULL)
	{
	  next = new T_list_index(item);
	}
  else
	{
	  next->add(item);
	}
}

// controle si le maplet en parametre est conforme a un index de la liste
T_list_index * T_list_index::check_maplet(T_item *maplet)
{
  // On verifie si le maplet a sa correspondance dans le premier index
  ASSERT(get_index() != NULL);
  if (get_index()->is_equal(maplet) == TRUE)
	{
	  // On supprime le premier element et on sort

	  return this->get_next();
	}

  // Le maplet n'a pas eu sa correspondance dans le premier index...
  T_list_index *pred_list_index = this;
  T_list_index *cur_list_index = this->get_next();

  while (cur_list_index != NULL)
	{
	  T_index *cur_index = cur_list_index->get_index();
	  if (cur_index->is_equal(maplet) == TRUE)
		{

		  // On a trouve l'index correspondant au maplet
		  // Il faut alors supprimer l'index de la liste
		  pred_list_index->set_next(cur_list_index->get_next());

		  // On sort...
		  return this;
		}

	  pred_list_index = cur_list_index;
	  cur_list_index = cur_list_index->get_next();
	}
  // On est toujours la : le maplet n'a pas de correspondance dans la liste
  // Par construction, c'est un doublon!

  if (get_check() != B0CHK_INIT)
	// si on ne fait pas une 2eme passe pour RINIT 1-1
	{
	  B0_semantic_error(maplet,
						CAN_CONTINUE,
						get_error_msg(E_B0_MAPLET_ALREADY_EXISTS) );
	}


  return this;
}

#endif // B0C
