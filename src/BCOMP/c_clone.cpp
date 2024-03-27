/***************************** CLEARSY **************************************
* Fichier : $Id: c_clone.cpp,v 2.0 2001-07-19 16:06:58 lv Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_CLONE pour tracer le clonage
*
* Description :		Mecanisme general de clonage des objets
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
RCS_ID("$Id: c_clone.cpp,v 1.9 2001-07-19 16:06:58 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Fonction generique de clonage
T_object *T_object::clone_object(void)
{
  TRACE2("T_object(%x:%s)::clone_object()", this, get_class_name()) ;
  // On clone tous les champs. Merci la metaclasse !
  // A FAIRE :: etudier la factorisation de cet algorithme
  T_metaclass *meta = get_metaclass(get_node_type()) ;

#ifdef DEBUG_CLONE
  TRACE2("allocation of %d bytes to store the object (class %x)",
		 meta->get_size(),
		 meta->get_class_name()) ;
  TRACE2("allocation of %d bytes to store the object (class %s)",
		 meta->get_size(),
		 meta->get_class_name()) ;
#endif

  int size = 0 ; // pour passage par valeur a la fct suivante
  T_object *new_object = create_object_of_type(node_type, size) ;
  assert((size_t)size == meta->get_size()) ; // pour le warning

#ifdef DEBUG_CLONE
  TRACE2("copie des champs de %x dans %x", this, new_object) ;
#endif

  new_object->copy_fields(meta, this) ;
  return new_object ;

}

// Fonction generique de copie des champs pour une classe donnee
void T_object::copy_fields(T_metaclass *metaclass, T_object *ref_object)
{
#ifdef DEBUG_CLONE
  TRACE2("T_object(%x)::copy_fields from class %s",
		 this, metaclass->get_class_name()) ;
#endif

  if (metaclass->get_superclass() != NULL)
	{
	  // On commence par recuperer les champs de la superclasse
#ifdef DEBUG_CLONE
	  TRACE1("on va dans la classe %x", metaclass->get_superclass()) ;
	  TRACE1("on va dans la classe %s",
			 metaclass->get_superclass()->get_class_name()) ;
	  ENTER_TRACE ;
	  ENTER_TRACE ;
#endif

	  copy_fields(metaclass->get_superclass(), ref_object) ;

#ifdef DEBUG_CLONE
	  EXIT_TRACE ;
	  EXIT_TRACE ;
	  TRACE1("retour dans la classe %x", metaclass) ;
	  TRACE1("retour dans la classe %s", metaclass->get_class_name()) ;
#endif
	}

  T_field *cur_field = metaclass->get_fields() ;

  if (cur_field == NULL)
	{
	  // Classe sans champ propre : on sort
#ifdef DEBUG_CLONE
	  TRACE1("la classe %s n'a pas de champ propre", metaclass->get_class_name()) ;
#endif
	  return ;
	}

  while (cur_field != NULL)
	{
	  void *cur_adr = (void *)((size_t)this + cur_field->get_field_offset()) ;
	  void *ref_adr = (void *)((size_t)ref_object + cur_field->get_field_offset()) ;

	  T_field_type field_type = cur_field->get_field_type() ;
	  T_ref_type ref_type = cur_field->get_ref_type() ;

	  if (field_type == FIELD_USER)
		{
		  // Les champs user sont traites comme des FIELD_OBJECT a cloner
		  // s'ils contiennent des objets, comme des FIELD_POINTER sinon
		  T_object *object = *(T_object **)ref_adr ;
#ifdef DEBUG_CLONE
		  TRACE4("champ %s -> ref:%x, is_an_object %s -> FIELD_%s",
				 cur_field->get_field_name(),
				 object,
				 (object_test(object) == TRUE) ? "TRUE" : "FALSE",
				 (object_test(object) == TRUE) ? "OBJECT" : "POINTER") ;
#endif
		  field_type = (object_test(object) == TRUE) ? FIELD_OBJECT:FIELD_POINTER ;
		  ref_type = REF_SHARED ; // ne sert que si FIELD_OBJECT, pour pas cloner
		}

	  switch(field_type)
		{
		case FIELD_INTEGER :
		  {
#ifdef DEBUG_CLONE
			TRACE2("copie du champ %s : INTEGER = %d",
				   cur_field->get_field_name(),
				   *(int *)ref_adr) ;
#endif
			*(int *)cur_adr = *(int *)ref_adr ;
			break ;
		  }
		case FIELD_OBJECT :
		  {
			T_object *object = *(T_object **)ref_adr ;
#ifdef DEBUG_CLONE
			TRACE3("DEBUT COPIE DU CHAMP %s:%s : OBJECT = %x",
				   metaclass->get_class_name(),
				   cur_field->get_field_name(),
				   *(void **)ref_adr) ;
			ENTER_TRACE ; ENTER_TRACE ;
#endif
			ASSERT(object_test(object) == TRUE) ;

			if (object == NULL)
			  {
#ifdef DEBUG_CLONE
				TRACE0("recopie de la valeur NULL") ;
#endif
				*(void **)cur_adr = NULL ;
			  }
			else /* object != NULL */
			  {
				switch(ref_type)
				  {
				  case REF_PRIVATE :
					{
					  // Clonage recursif
#ifdef DEBUG_CLONE
					  TRACE2("clonage recursif de %x:%s",
							 object, object->get_class_name()) ;
#endif
					  // On ne veut surtout pas cloner de T_symbol
					  ASSERT(object->is_a_symbol() == FALSE) ;
					  *(void **)cur_adr = object->clone_object() ;
 					  break ;
					}

				  case REF_PRIVATE_LIST :
					{
					  // Clonage recursif de la liste
					  ASSERT(object->is_an_item() == TRUE) ;
					  T_item *cur_object = (T_item *)object ;
					  T_item *first_in_list = NULL ;
					  T_item *last_in_list = NULL ;
#ifdef DEBUG_CLONE
					  TRACE2("liste : clonage recursif de %x:%s",
							 cur_object, cur_object->get_class_name()) ;
#endif
					  T_item *prev = NULL ;
					  while (cur_object != NULL)
						{
						  T_item *next = cur_object->get_next() ;
#ifdef DEBUG_CLONE
						  TRACE2("--> DEBUT CLONAGE LIST_ITEM %x %s",
								 object,
								 (object->is_an_item() == TRUE) ?
								 "not an item" : ((T_item *)object)->get_class_name()) ;
						  ENTER_TRACE ;
#endif
						  // On ne veut surtout pas cloner de T_symbol
						  ASSERT(object->is_a_symbol() == FALSE);

						  T_item *res = (T_item *)(cur_object->clone_object()) ;

						  if (first_in_list == NULL)
							{
							  first_in_list = res ;
							}

						  cur_object = next ;
						  if (cur_object == NULL)
							{
							  last_in_list = res ;
							}

						  // Corrige les chainges next/prev
						  if (prev != NULL)
							{
							  prev->set_next(res) ;
							}
						  res->set_prev(prev) ;
						  prev = res ;
#ifdef DEBUG_CLONE
						  EXIT_TRACE ;
						  TRACE1("--> FIN CLONAGE LIST_ITEM %x", object) ;
#endif
						}

					  // Mise a jour de first/last
#ifdef DEBUG_CLONE
					  TRACE4("mise a jour de first/last %x/%x avec %x/%x",
							 cur_adr,
							 (size_t)cur_adr + cur_field->get_field_size(),
							 first_in_list,
							 last_in_list) ;
#endif
					  *(void **)cur_adr = first_in_list ;
					  *(void **)((size_t)cur_adr + cur_field->get_field_size())
						= last_in_list ;

					  ASSERT(cur_field->get_next() != NULL) ;
#ifdef DEBUG_CLONE
					  TRACE1("next_field %x", cur_field->get_next()) ;
					  TRACE2("next_field %s %d",
							 cur_field->get_next()->get_field_name(),
							 cur_field->get_next()->get_ref_type()) ;
#endif
					  ASSERT( (cur_field->get_next()->get_ref_type()
							   == REF_NO_COPY) ) ;
					  break ;
					}

				  case REF_SHARED :
					{
					  // Copie de pointeur
#ifdef DEBUG_CLONE
					  TRACE2("copie de pointeur de %x:%s",
							 object,
							 (object == NULL) ? "" : object->get_class_name()) ;
#endif
					  *(void **)cur_adr = object ;
 					  break ;
					}

				  case REF_NO_COPY :
					{
					  // On ne fait rien
#ifdef DEBUG_CLONE
					  TRACE0("on laisse le champ inchange") ;
#endif
 					  break ;
					}

					// Pas de default pour que le compilateur
					// nous previenne si on oublie un cas
				  }
			  }

#ifdef DEBUG_CLONE
			EXIT_TRACE ; EXIT_TRACE ;
			TRACE3(" FIN COPIE DU CHAMP %s:%s : OBJECT = %x",
				   metaclass->get_class_name(),
				   cur_field->get_field_name(),
				   *(void **)ref_adr) ;
#endif

			break ;
		  }

		case FIELD_STRING :
		case FIELD_POINTER :
		case FIELD_FILLING :
		  {
#ifdef DEBUG_CLONE
			TRACE3("copie du champ %s : %s = %x",
				   cur_field->get_field_name(),
				   (cur_field->get_field_type() == FIELD_STRING)
				   ? "STRING" : (cur_field->get_field_type() == FIELD_POINTER)
				   ? "POINTER" : "FILLING",
				   *(void **)ref_adr) ;
#endif
			*(void **)cur_adr = *(void **)ref_adr ;
			break ;
		  }

		case FIELD_USER :
		  {
			// Impossible car ce cas est change en FIELD_OBJECT ou en
			// FIELD_POINTER avant le switch
			assert(FALSE) ; // A FAIRE
		  }

		case FIELD_RESET :
		case FIELD_UPGRADE :
		  {
#ifdef DEBUG_CLONE
			TRACE2("reset du champ %s @%x", cur_adr, cur_field->get_field_name()) ;
#endif
			*(void **)cur_adr = (void *)NULL ;
			break ;
		  }

		// Pas de default pour etre sur qu'on traite tous les cas
		}

	  // On passe a la suite
	  cur_field = cur_field->get_next() ;
	}

  // Quelques rattrapages eventuels a effectuer
  fix_clone(ref_object) ;
}

// Rattrapages eventuels sur le clonage : rien dans le cas genera
void T_object::fix_clone(T_object *ref_object)
{
  return ;
  ref_object = ref_object ;
}

// Si on clone un ident unknown, il faut l'ajouter dans l'object
// manager, sinon il ne sera pas resolu par contre dans le cas general
// on ne rajoute pas les idents dans la liste des idents pour ne pas
// gener des programmes comme le Xref
void T_ident::fix_clone(T_object *ref_object)
{
  if (ident_type == ITYPE_UNKNOWN)
	{
	  def = (T_ident *)ref_object ; // cast justifie par construction du clone
	  //get_object_manager()->add_unsolved_ident(this) ;
	}

}

// Si on clone un T_op_result, il faut l'ajouter dans l'object
// manager, sinon il ne sera pas transforme en T_array_item le cas echeant
void T_op_result::fix_clone(T_object *ref_object)
{
  get_object_manager()->add_op_result(this) ;
  return ;
  ref_object = ref_object ;
}
