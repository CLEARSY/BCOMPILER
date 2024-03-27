/******************************* CLEARSY **************************************
* Fichier : $Id: t_record.cpp,v 2.0 2000-11-09 17:40:42 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Fabrication du type des records
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
RCS_ID("$Id: t_record.cpp,v 1.10 2000-11-09 17:40:42 lv Exp $") ;

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

// Verifie que les labels sont deux a deux distincts
// Emet un message d'erreur le cas echeant
static void check_no_double_label(T_record_item *list,
										   int allow_empty_label)
{

  // Verification bete et incrementale pour l'instant
  // Devrait suffir en terme de performances car les listes
  // sont courtes et les comparaisons de symboles optimisees
  T_record_item *cur_record_item = list ;

  for (;;)
	{
	  // On avance d'un cran
	  cur_record_item = (T_record_item *)cur_record_item->get_next() ;

	  if (cur_record_item == NULL)
		{
		  // Fin de la verification
		  return ;
		}

	  T_ident *cur = cur_record_item->get_label() ;

	  if (cur != NULL)
		{
		  T_symbol *ref = cur->get_name() ;
		  T_record_item *check_item = list ;

		  while (check_item != cur_record_item)
			{
			  T_ident *check = check_item->get_label() ;

			  if ( (check != NULL) && (check->get_name()->compare(ref) == TRUE))
				{
				  // Redefinition
				  semantic_error(cur,
								 CAN_CONTINUE,
								 get_error_msg(E_IDENT_CLASH),
								 ref->get_value(),
								 cur->get_ident_type_name(),
								 check->get_ident_type_name()) ;
				  semantic_error_details(check, get_error_msg(
				  	E_IDENT_CLASH_OTHER_LOCATION)) ;

				  // Ajout dans l'anneau de collage pour ne pas bégayer
				  cur->insert_in_glued_ring(check) ;
				}

			  check_item = (T_record_item *)check_item->get_next() ;
			}
		}
#ifndef NO_CHECKS
	  else
		{
		  // Label NULL : on verifie que c'est autorise
		  // Pas la peine : c'est fait par l'analyseur syntaxique
		  if (allow_empty_label == FALSE)
			{
			  assert(FALSE) ; // A FAIRE
			}
		}
#endif // NO_CHECKS
	}
}


void T_record::internal_make_type(void)
{
  TRACE1("--> T_record(%x)::internal_make_type", this) ;
  ENTER_TRACE ;

  // Verifier que tous les labels sont distincts
  check_no_double_label(first_record_item, TRUE) ;

  // Fabriquer le type chapeau
  T_record_type *res = new T_record_type(this, get_betree(), get_ref_lexem()) ;
  T_item **adr_first = (T_item **)res->get_adr_first_label() ;
  T_item **adr_last = (T_item **)res->get_adr_last_label() ;

  // Fabriquer les types des records
  T_record_item *cur = first_record_item ;

  while (cur != NULL)
	{
	  cur->get_value()->make_type() ;
	  T_type *cur_type = cur->get_value()->get_B_type() ;
	  (void)new T_label_type(cur->get_label(),
							 NULL,
							 cur_type,
							 adr_first,
							 adr_last,
							 res,
							 get_betree(),
							 cur->get_ref_lexem()) ;

	  cur = (T_record_item *)cur->get_next() ;
	}

  // C'est tout !
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;

  EXIT_TRACE ;
  TRACE1("<-- T_record(%x)::internal_make_type", this) ;
}


void T_struct::internal_make_type(void)
{
  TRACE1("T_struct(%x)::internal_make_type", this) ;

  // Verifier que tous les labels sont distincts
  check_no_double_label(first_record_item, FALSE) ;

  // Fabriquer le type chapeau
  T_record_type *res = new T_record_type(this, get_betree(), get_ref_lexem()) ;
  T_item **adr_first = (T_item **)res->get_adr_first_label() ;
  T_item **adr_last = (T_item **)res->get_adr_last_label() ;

  // Fabriquer les types des records
  T_record_item *cur = first_record_item ;

  while (cur != NULL)
	{
	  cur->get_value()->make_type() ;
	  T_type *cur_type = cur->get_value()->get_B_type() ;

	  if ( (cur_type == NULL)  || (cur_type->get_node_type() != NODE_SETOF_TYPE) )
		{
		  semantic_error(cur->get_value(),
						 CAN_CONTINUE,
						 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
						 make_type_name(cur_type)->get_value()) ;
		}
	  else
		{
		  // Cast justifie par le test ci-dessus
		  T_setof_type *setof = (T_setof_type *)cur_type ;
		  (void)new T_label_type(cur->get_label(),
								 setof,
								 setof->get_base_type(),
								 adr_first,
								 adr_last,
								 res,
								 get_betree(),
								 cur->get_ref_lexem()) ;
		}

	  cur = (T_record_item *)cur->get_next() ;
	}

  // C'est tout !
  T_setof_type *set = new T_setof_type(res, this, get_betree(), get_ref_lexem()) ;
  res->unlink() ;
  set_B_type(set, get_ref_lexem()) ;
  set->unlink() ;
}

void T_record_item::internal_make_type(void)
{
  TRACE1("T_record_item(%x)::internal_make_type", this) ;
}

void T_record_access::internal_make_type(void)
{
  TRACE1("T_record_access(%x)::internal_make_type", this) ;

  // Il faut verifier que l'identificateur est un record
  record->make_type() ;
  if (    (record->get_B_type() == NULL)
	   || (record->get_B_type()->get_node_type() != NODE_RECORD_TYPE) )
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_RECORD),
					 label->get_name()->get_value(),
					 make_type_name(record)->get_value()) ;
	  return ;
	}

  // Il faut ensuite que le label soit present dans le record
  // Cast justifie par le type ci-dessus
  T_record_type *rec_type = (T_record_type *)record->get_B_type() ;
  ASSERT(rec_type->get_node_type() == NODE_RECORD_TYPE) ;

  T_label_type *cur_label = rec_type->get_labels() ;
  T_symbol *ref_value = label->get_name() ;
  int trouve = FALSE ;

  while ( (cur_label != NULL) && (trouve == FALSE) )
	{
	  if (cur_label->get_name()->get_name()->compare(ref_value) == TRUE)
		{
		  trouve = TRUE ;
		}
	  else
		{
		  cur_label = (T_label_type *)cur_label->get_next() ;
		}
	}

  if (trouve == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_PART_OF_RECORD),
					 label->get_name()->get_value(),
					 make_type_name(record)->get_value()) ;
	  return ;
	}

  // Ici on sait que l'on accede un label valide
  // Le type est le type du label
  T_type *res = cur_label->get_type() ;
  set_B_type(res, get_ref_lexem()) ;
}

//
// }{ Fin du fichier
//
