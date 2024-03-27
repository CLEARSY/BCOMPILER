/***************************** CLEARSY **************************************
* Fichier : $Id: c_equtyp.cpp,v 2.0 1999-06-22 15:00:47 sl Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_TYPE pour les traces generales
*
* Description :	Fonctions d'egalite de type (is_equal_to)
*				Provient de c_type.cpp 1.11
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_equtyp.cpp,v 1.9 1999-06-22 15:00:47 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Fonctions d'egalite de type
int T_product_type::is_equal_to(T_type *ref_type)
{
#ifdef DEBUG_TYPE
  TRACE4("T_product_type(%x, %s)::is_equal_to(%x, %s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 ref_type->make_type_name()->get_value()) ;
#endif

  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // On appelle la methode T_generic_type::is_equal_to
	  // qui est plus adaptee
	  return ref_type->is_equal_to(this) ;
	}


  if (ref_type->get_node_type() != NODE_PRODUCT_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref pas product -> FALSE") ;
#endif
	  return FALSE ;
	}

  T_product_type *ref = (T_product_type *)ref_type ;

  if (ref->get_type1()->is_equal_to(get_type1()) == FALSE)
	{
#ifdef DEBUG_TYPE
	  TRACE4("type1 %x:%s et %x:%s pas egaux",
			 ref->get_type1(),
			 ref->get_type1()->make_type_name()->get_value(),
			 get_type1(),
			 get_type1()->make_type_name()->get_value()) ;
#endif
	  return FALSE ;
	}

  if (ref->get_type2()->is_equal_to(get_type2()) == FALSE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("get_type2() pas egaux") ;
#endif
	  return FALSE ;
	}

  return TRUE ;
}

// Fonctions d'egalite de type
int T_setof_type::is_equal_to(T_type *ref_type)
{
#ifdef DEBUG_TYPE
  TRACE3("T_setof_type(%x)::is_equal_to(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif

  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // On appelle la methode T_generic_type::is_equal_to
	  // qui est plus adaptee
	  return ref_type->is_equal_to(this) ;
	}

  // Il faut que ref_type soit un type enumere ...
  if (ref_type->get_node_type() != NODE_SETOF_TYPE)
	{
	  return FALSE ;
	}

  // ... et que le type "dedans" soit compatible
#ifdef DEBUG_TYPE
  TRACE0("on regarde les predefined_type") ;
#endif

  return base_type->is_equal_to(((T_setof_type*)ref_type)->base_type) ;
}

// Fonctions d'egalite de type
int T_abstract_type::is_equal_to(T_type *ref_type)
{
  if (get_after_valuation_type() != NULL)
	{
	  return get_after_valuation_type()->is_equal_to(ref_type) ;
	}
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // On appelle la methode T_generic_type::is_equal_to
	  // qui est plus adaptee
	  return ref_type->is_equal_to(this) ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_abstract_type(%x)::is_equal_to(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


  // Il faut que ref_type soit un type abstrait ...
  if (ref_type->get_node_type() != NODE_ABSTRACT_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("autre types pas abstrait") ;
#endif

	  return FALSE ;
	}

  // ... avec le meme type_definition
  T_symbol *cur_set = set->get_name() ;
  T_symbol *ref_set = ((T_abstract_type *)ref_type)->set->get_name() ;
#ifdef DEBUG_TYPE
  TRACE1("resultat comparaison %s",
		 (cur_set->compare(ref_set) == TRUE) ? "TRUE" : "FALSE") ;
#endif

  return cur_set->compare(ref_set) ;
}

// Fonctions d'egalite de type
int T_enumerated_type::is_equal_to(T_type *ref_type)
{
#ifdef DEBUG_TYPE
  TRACE3("T_enumerated_type(%x)::is_equal_to(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif

  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // On appelle la methode T_generic_type::is_equal_to
	  // qui est plus adaptee
	  return ref_type->is_equal_to(this) ;
	}


  // Il faut que ref_type soit un type enumere ...
  if (ref_type->get_node_type() != NODE_ENUMERATED_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("autre types par enumerated") ;
#endif

	  return FALSE ;
	}

  // ... avec le meme type_definition
  T_symbol *cur_set = type_definition->get_name() ;
  T_symbol *ref_set = ((T_enumerated_type *)ref_type)->type_definition->get_name() ;
#ifdef DEBUG_TYPE
  TRACE1("resultat comparaison %s",
		 (cur_set->compare(ref_set) == TRUE) ? "TRUE" : "FALSE") ;
#endif

  return cur_set->compare(ref_set) ;
}

// Fonctions d'egalite de type
int T_predefined_type::is_equal_to(T_type *ref_type)
{
#ifdef DEBUG_TYPE
  TRACE3("T_predefined_type(%x)::is_equal_to(%x, %s)",
		 this,
		 ref_type,
		 (ref_type == NULL) ? "NULL " : ref_type->get_class_name()) ;
#endif

  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // On appelle la methode T_generic_type::is_equal_to
	  // qui est plus adaptee
	  return ref_type->is_equal_to(this) ;
	}


  if (ref_type == NULL)
	{
#ifdef DEBUG_TYPE
	  TRACE0("PANIC, ref_type == NULL") ;
#endif

	  return FALSE ;
	}

  if (ref_type->get_node_type() == NODE_ABSTRACT_TYPE)
	{
	  T_abstract_type *atype = (T_abstract_type *)ref_type ;
	  if (atype->get_after_valuation_type() != NULL)
		{
#ifdef DEBUG_TYPE
		  TRACE0("on demande aux types apres valuation") ;
#endif
		  return is_compatible_with(atype->get_after_valuation_type()) ;
		}
	}

  // Il faut que ref_type soit un type de base ...
  if (ref_type->get_node_type() != NODE_PREDEFINED_TYPE)
	{
	  return FALSE ;
	}

  // ... avec le meme ref_type ...
#ifdef DEBUG_TYPE
  TRACE1("type = %d", type) ;
  TRACE1("ref_type = %d", ((T_predefined_type *)ref_type)->type) ;
#endif

  T_predefined_type *rtype = (T_predefined_type*)ref_type ;
  if  (type != rtype->type)
	{
	  return FALSE ;
	}

#if 0
  // ... et des bornes compatibles i.e. a droite des bornes plus selectives
#ifdef FULL_TRACE
  TRACE0("etude borne inf") ;
  if ( 	(get_lower_bound() != NULL)
		&& (get_lower_bound()->is_less_than(ref_type->get_lower_bound()) == FALSE) )
	{
	  TRACE0("Intervalle de droite non compatible (borne inf)") ;
	  return FALSE ;
	}

  TRACE0("etude borne sup") ;
  if ( 	(get_upper_bound() != NULL)
		&& (get_upper_bound()->is_greater_than(ref_type->get_upper_bound()) == FALSE))
	{
	  TRACE0("Intervalle de droite non compatible (borne sup)") ;
	  return FALSE ;
	}
#endif
#endif

  // C'est bon !
  TRACE0("c'est bon !") ;
  return TRUE ;
}

int T_generic_type::is_equal_to(T_type *ref_type)
{
#ifdef DEBUG_TYPE
  TRACE3("T_generic_type(%x)::is_equal_to(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


  // Pour les types generiques, egalite et compatibilite
  // sont la meme chose
#ifdef DEBUG_TYPE
  TRACE0("on appelle is_compatible_with") ;
#endif

  return is_compatible_with(ref_type) ;
}

int T_record_type::is_equal_to(T_type *ref_type)
{
  // Egal et compatible represente la meme notion pour les records
  return is_compatible_with(ref_type) ;
}

int T_label_type::is_equal_to(T_type *ref_type)
{
  // Egal et compatible represente la meme notion pour les records
  return is_compatible_with(ref_type) ;
}
