/******************************* CLEARSY **************************************
* Fichier : $Id: v_import.cpp,v 2.0 2002-02-07 10:45:24 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de haut niveau de l'analyseur semantique
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
RCS_ID("$Id: v_import.cpp,v 1.28 2002-02-07 10:45:24 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_umach.h"

//GP 19/01/99
//Quand un identifiant du type variables concretes, constantes
//concretes ou ensembles
//est defini dans l'abstraction et dans le
//le raffinement ou l'implementation, il faut distinguer
//les cas licites des erreurs
//Cas licite:
//1) ref est herite par un lien INCLUDES, EXTENDS, IMPORTS
//Erreur :
// 2) ref est colle par un lien sees (cas des constantes et ensmebles)
// 3) ref est redefini dans le raffinement (resp. l'implementation)

// Recuperation de l'environnement de l'abstraction
void T_machine::import_abstract_environment(T_lexem *ref_lexem)
{
  TRACE2("T_machine(%x:%s)::import_abstract_environment",
		 this,
		 machine_name->get_name()->get_value()) ;

  // On recupere les constantes concretes de l'abstraction
  T_ident *cur = ref_abstraction->get_concrete_constants() ;

  while (cur != NULL)
	{
	  T_ident *cur_const = find_constant(cur->get_name()) ;
	  if (cur_const == NULL)
		{
		  TRACE1("HERIT la constante concrete %s", cur->get_name()->get_value()) ;
		  T_ident *new_ident = new T_ident(cur,
										   (T_item **)&first_concrete_constant,
										   (T_item **)&last_concrete_constant,
										   this) ;
		  TRACE2("Mise a jour du def de %x avec la valeur %x", new_ident, cur) ;
		  new_ident->set_def(cur) ;
		  // mise à jour du lien sur l'homonyme de l'abstraction
		  new_ident->set_homonym_in_abstraction(cur) ;

		  new_ident->set_ref_machine(this) ;
		  fix_inherited(new_ident, cur, ref_lexem) ;
		}
	  else
		{
		  if (cur_const->get_ref_glued() == NULL)
			{
			  TRACE0("COLL 4-1 non respecte") ;
			  semantic_error(cur_const,
							 CAN_CONTINUE,
							 get_error_msg(E_CAN_NOT_REFINES),
							 cur_const->get_ident_type_name(),
							 cur_const->get_name()->get_value(),
							 cur->get_ident_type_name()) ;
			  semantic_error_details(cur,
									 get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
			}

#ifdef TRACE
		  else
			{
			  TRACE1("on HERIT pas la constante %s car elle est collee",
					 cur->get_name()->get_value()) ;
			}
#endif
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  // On recupere les variables concretes de l'abstraction
  cur = ref_abstraction->get_concrete_variables() ;

  while (cur != NULL)
	{
	  T_ident* cur_var = find_variable(cur->get_name()) ;
	  if (cur_var == NULL)
		{
		  TRACE1("HERIT la variablee concrete %s", cur->get_name()->get_value()) ;
		  T_ident *new_ident = new T_ident(cur,
										   (T_item **)&first_concrete_variable,
										   (T_item **)&last_concrete_variable,
										   this) ;
		  TRACE2("Mise a jour du def de %x avec la valeur %x", new_ident, cur) ;
		  new_ident->set_def(cur) ;
		  // mise à jour du lien sur l'homonyme de l'abstraction
		  new_ident->set_homonym_in_abstraction(cur) ;

		  new_ident->set_ref_machine(this) ;
		  fix_inherited(new_ident, cur, ref_lexem) ;
		}
	  else
		{
		  //GP 18/11/98
		  // CTRL Ref: COLL 4-1
		  if (cur_var->get_ref_glued() == NULL)
			{
			  TRACE0("COLL 4-1 non respecte") ;
			  semantic_error(cur_var,
							 CAN_CONTINUE,
							 get_error_msg(E_CAN_NOT_REFINES),
							 cur_var->get_ident_type_name(),
							 cur_var->get_name()->get_value(),
							 cur->get_ident_type_name()) ;
			  semantic_error_details(cur,
									 get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
			}

#ifdef TRACE
		  else
			{

			  TRACE1("on HERIT pas la variable %s car elle est collee",
					 cur->get_name()->get_value()) ;
			}
#endif

		}
	  cur = (T_ident *)cur->get_next() ;
	}

  // On recupere les ensembles de l'abstraction, sauf ceux qui sont colles
  // On refait la liste pour des problemes d'ordre
  T_item *tmp_first_set = NULL ;
  T_item *tmp_last_set = NULL ;
  cur = ref_abstraction->get_sets() ;

  if ( (cur != NULL) && (sets_clause == NULL) )
	{
	  // On cree une clause SETS fantome
	  TRACE0("creation d'une clause SETS fantome") ;
	  sets_clause = new T_flag(this, get_betree(), refines_clause->get_ref_lexem());
	}

  while (cur != NULL)
	{
	  T_ident *cur_set = find_set(cur->get_name()) ;

	  if (cur_set == NULL)
		{
		  TRACE1("HERIT l'ensemble %s", cur->get_name()->get_value()) ;
		  T_ident *new_ident = new T_ident(cur,
										   &tmp_first_set,
										   &tmp_last_set,
										   sets_clause) ;
		  TRACE4("Mise a jour du def de %x:%s avec la valeur %x:%s",
				 new_ident,
				 new_ident->get_name()->get_value(),
				 cur,
				 cur->get_name()->get_value()) ;
		  new_ident->set_def(cur) ;
		  // mise à jour du lien sur l'homonyme de l'abstraction
		  new_ident->set_homonym_in_abstraction(cur) ;

		  new_ident->set_ref_machine(this) ;
		  fix_inherited(new_ident, cur, ref_lexem) ;

		  if (    (new_ident->get_values() != NULL)
				  && (new_ident->get_values()->is_an_ident() == TRUE) )
			{
			  // Pour les valeurs enumeree et constantes, il faut changer
			  // le ref_machine
			  T_ident *cur_value = (T_ident *)(new_ident->get_values()) ;
			  T_ident *ref_value = (T_ident *)(cur->get_values()) ;
			  while (cur_value != NULL)
				{
				  ASSERT(ref_value != NULL) ; // garanti par le clonage
				  TRACE2("Mise a jour du def de %x avec la valeur %x",
						 cur_value, ref_value) ;
				  cur_value->set_def(ref_value) ;
				  // mise à jour du lien sur l'homonyme de l'abstraction
				  cur_value->set_homonym_in_abstraction(ref_value) ;

				  cur_value->set_ref_machine(this) ;
				  // correction bug 2975
				  fix_inherited(cur_value, ref_value, ref_lexem) ;
				  cur_value = (T_ident *)(cur_value->get_next()) ;
				  ref_value = (T_ident *)(ref_value->get_next()) ;
				}
			}
		}
	  else
		{

		  // GP 9/11/98
		  // Si cur_set n'est pas glued c'est qu'il est redefini dans
		  // dans la machine, c'est illegal
		  // Le probleme ne se pose pas avec des collages à partir des
		  // machines uses, car
		  // la clause "USES" est interdite dans un raffinement ou
		  // une implementation
		  // dans le cas de l'importation, le conflit est leve par
		  // l'algo d'anticollision.
		  // CTRL Ref: COLL 3-3

		  if (cur_set->get_ref_glued() == NULL)
			{
			  TRACE0("COLL 3-3 non respecte") ;
			  semantic_error(cur_set,
							 CAN_CONTINUE,
							 get_error_msg(E_CAN_NOT_REFINES),
							 cur_set->get_ident_type_name(),
							 cur_set->get_name()->get_value(),
							 cur->get_ident_type_name()) ;
			  semantic_error_details(cur,
									 get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
			}
		  else
			{
			  TRACE3("on HERIT pas l'ensemble %s (%x, pere %x) car il est colle",
				 cur->get_name()->get_value(),
				 cur,
				 cur->get_father()) ;
			}

		  // On change cur_set de liste. Il faut bien prendre garde
		  // a le retirer de la liste precedente sinon le chainage
		  // dans la nouvelle liste casse la precedente :
		  // (L1) first, last : A --> B --> C
		  // (L2) tmp_first, tmp_last : NULL
		  // Si on deplace B dans L2 sans l'enlever de L2 alors (L1) = A -> B !!!
		  cur_set->remove_from_list((T_item **)&first_set, (T_item **)&last_set) ;
		  cur_set->tail_insert(&tmp_first_set, &tmp_last_set) ;
		  cur_set->set_father(sets_clause) ;
		  TRACE3("--> fin changement de liste de %s (%x, pere %x) car il est colle",
				 cur_set->get_name()->get_value(),
				 cur_set,
				 cur_set->get_father()) ;

		}

	  cur = (T_ident *)cur->get_next() ;
	}

  // A cette liste, il ne faut pas oublier d'ajouter les SETS propres
  T_item *cur_merge = last_set ;

  while (cur_merge != NULL)
	{
	  T_item *prev = cur_merge->get_prev() ;
	  cur_merge->remove_from_list((T_item **)&first_set, (T_item **)&last_set) ;
	  cur_merge->head_insert(&tmp_first_set, &tmp_last_set) ;
	  ASSERT(sets_clause != NULL) ; // sinon on ne serait pas dans cette boucle
	  cur_merge->set_father(sets_clause) ;
	  cur_merge = prev ;
	}

  first_set = (T_ident *)tmp_first_set ;
  last_set = (T_ident *)tmp_last_set ;


  // Constantes abstraites, variables abstraites : detection des
  // collages implicites. Attention, si ref_glued != NULL le travail a
  // deja ete fait, il ne faut pas le refaire (sinon on eclate le
  // ref_glued !!!)
  TRACE0("--> detection des collages implicites de var abstraites/concretes") ;
  ENTER_TRACE ;
  cur = first_abstract_variable ;

  while (cur != NULL)
	{
	  if (cur->get_ref_glued() == NULL)
		{
		  T_ident *res = ref_abstraction->find_abstract_variable(cur->get_name()) ;
		  if (res != NULL)
			{
			  cur->set_ref_glued(res) ;
			  // mise à jour du lien sur l'homonyme de l'abstraction
			  cur->set_homonym_in_abstraction(res) ;

			  cur->set_inherited(FALSE) ; // corrige l'effet de set_ref_glued
			  cur->set_def(res) ;
			}
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  cur = first_concrete_variable ;

  while (cur != NULL)
	{
	  if (cur->get_ref_glued() == NULL)
		{
		  T_ident *res = ref_abstraction->find_abstract_variable(cur->get_name()) ;
		  if (res != NULL)
			{
			  cur->set_ref_glued(res) ;
			  cur->set_inherited(FALSE) ; // corrige l'effet de set_ref_glued
			  // mise à jour du lien sur l'homonyme de l'abstraction
			  cur->set_homonym_in_abstraction(res) ;
			  cur->set_def(res) ;
			}
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE0("<-- detection des collages implicites de var abstraites/concretes") ;

  TRACE0("--> detection des collages implicites de cst abstraites/concretes") ;
  ENTER_TRACE ;
  cur = first_abstract_constant ;

  while (cur != NULL)
	{
	  if (cur->get_ref_glued() == NULL)
		{
		  T_ident *res = ref_abstraction->find_abstract_constant(cur->get_name()) ;
		  if (res != NULL)
			{
			  cur->set_ref_glued(res) ;
			  cur->set_inherited(FALSE) ; // corrige l'effet de set_ref_glued
			  // mise à jour du lien sur l'homonyme de l'abstraction
			  cur->set_homonym_in_abstraction(res) ;
			  cur->set_def(res) ;
			}
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  cur = first_concrete_constant ;

  while (cur != NULL)
	{
	  if (cur->get_ref_glued() == NULL)
		{
		  T_ident *res = ref_abstraction->find_abstract_constant(cur->get_name()) ;
		  if (res != NULL)
			{
			  cur->set_ref_glued(res) ;
			  cur->set_inherited(FALSE) ; // corrige l'effet de set_ref_glued
			  // mise à jour du lien sur l'homonyme de l'abstraction
			  cur->set_homonym_in_abstraction(res) ;
			  cur->set_def(res) ;
			}
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE0("<-- detection des collages implicites de cst abstraites/concretes") ;

}

//GP 20/11/98
//Cette fonction renvoi TRUE, si le nom de T_machine apparait dans
//la liste des symbols
static int is_already_included(T_machine* machine,
										T_list_link* first_machine_name,
										T_list_link* last_machine_name)
{
  TRACE1("is_already_included(%s, ...)",
		 machine->get_machine_name()->get_name()->get_value()) ;

  T_symbol* machine_name = machine->get_machine_name()->get_name() ;
  T_list_link* cur = first_machine_name ;
  while(cur != NULL)
	{
	  if(((T_symbol*)cur->get_object())->get_value() == machine_name->get_value())
		{
		  TRACE1("%s is already included", machine_name->get_value()) ;
		  return TRUE ;
		}
	  else
		{
		  cur = (T_list_link*)cur->get_next() ;
		}
	}
  TRACE1("%s is not already included", machine_name->get_value()) ;
  return FALSE ;
}


// Verifie qu'un ident n'est pas une donnee de la machine
// Si c'est le cas, emet un message d'erreur et renvoie FALSE
// Sinon renvoie TRUE
// Doit etre appele avec une constante ou une variable
int T_machine::check_no_collision(T_ident *ident, T_symbol *prefix)
{
  TRACE5("T_machine(%x:%s)::check_no_collision(%x:%s <%s>)",
		 this,
		 machine_name->get_name()->get_value(),
		 ident,
		 ident->get_name()->get_value(),
		 ident->get_class_name()) ;

  T_ident *find = NULL ;

  switch (ident->get_definition()->get_ident_type())
	{
	case ITYPE_ABSTRACT_VARIABLE :
	case ITYPE_CONCRETE_VARIABLE :
	  {
		T_symbol *name = ident->get_name() ;

		if (prefix != NULL)
		  {
			// Construction du nom pointe
			// 1 = '.'
			int len = name->get_len() + prefix->get_len() + 1 ;
			char *full_name = new char[len + 1] ;
			sprintf(full_name, "%s.%s", name->get_value(), prefix->get_value()) ;
			name = lookup_symbol(full_name, len) ;
			delete [] full_name ;
		  }

		TRACE1("recherche %x", name) ;
		find = find_variable(name) ;
		break ;
	  }

	case ITYPE_ABSTRACT_CONSTANT :
	case ITYPE_CONCRETE_CONSTANT :
	  {
		find = find_constant(ident->get_name()) ;
		break ;
	  }

	default :
	  {
		// Pas possible (violation de la precondition d'appel)
		assert(FALSE) ;
	  }
	}

  if (find == NULL)
	{
	  // Ok pas de redefinition
	  return TRUE ;
	}

  // Erreur : redefinition
  double_identifier_error(find, ident) ;

  return FALSE ;
}




// Recuperation de l'environnement d'une machine incluse
void
	T_machine::import_included_environment(T_machine *included_machine,
										   T_lexem *ref_lexem,
										   T_list_link **first_already_included,
										   T_list_link **last_already_included)

{
  TRACE4("T_machine(%x:%s)::import_included_environment(%x:%s)",
		 this,
		 machine_name->get_name()->get_value(),
		 included_machine,
		 included_machine->get_machine_name()->get_name()->get_value()) ;

  T_used_machine *context = included_machine->get_context() ;
  T_symbol *prefix = context->get_instance_name() ;

  //GP 20/11/98
  int is_already_incl = is_already_included(included_machine,
											*first_already_included,
											*last_already_included) ;

  T_ident* cur = NULL ;
  // GP 20/11/98 Test si la machine a deja ete incluse, pour ne pas importer
  //plusieurs fois les constantes
  if (is_already_incl == FALSE)
	{
	  // On recupere les constantes concretes et abstraites de la machine incluse
	  //GP 20/11/98: les constantes, ne doivent pas etres renommees
	  cur = included_machine->get_concrete_constants() ;
	  int abstract_done = FALSE ;
	  T_item **adr_first = (T_item **)&first_concrete_constant ;
	  T_item **adr_last = (T_item **)&last_concrete_constant ;
	  if (cur == NULL)
		{
		  cur = included_machine->get_abstract_constants() ;
		  adr_first = (T_item **)&first_abstract_constant ;
		  adr_last = (T_item **)&last_abstract_constant ;
		  abstract_done = TRUE ;
		}

	  while (cur != NULL)
		{
		  // On herite de la constante sauf si on l'a deja definie
		  // (auquel cas c'est un clash)
		  if (check_no_collision(cur) == TRUE)
			{
			  TRACE1("HERIT la constante %s", cur->get_name()->get_value()) ;
			  T_ident *new_ident = new T_ident(cur, adr_first, adr_last, this) ;

			  TRACE2("Mise a jour du def de %x avec la valeur %x",
					 new_ident, cur) ;
			  new_ident->set_def(cur) ;
			  // mise à jour du lien sur l'homonyme de la machine incluse
			  new_ident->set_homonym_in_required_mach(cur);
			  new_ident->set_ref_machine(this) ;
			  fix_inherited(new_ident, cur, ref_lexem) ;
			}
		  cur = (T_ident *)cur->get_next() ;

		  if ( (cur == NULL) && (abstract_done == FALSE) )
			{
			  cur = included_machine->get_abstract_constants() ;
			  adr_first = (T_item **)&first_abstract_constant ;
			  adr_last = (T_item **)&last_abstract_constant ;
			  abstract_done = TRUE ;
			}
		}

	} //if (already_incl == FALSE)

  // On recupere les variables concretes de la machine incluse
  cur = included_machine->get_concrete_variables() ;
  int abstract_done = FALSE ;
  T_item **adr_first = (T_item **)&first_concrete_variable ;
  T_item **adr_last = (T_item **)&last_concrete_variable ;
  if (cur == NULL)
	{
	  cur = included_machine->get_abstract_variables() ;
	  adr_first = (T_item **)&first_abstract_variable ;
	  adr_last = (T_item **)&last_abstract_variable ;
	  abstract_done = TRUE ;
	}

  while (cur != NULL)
	{
	  // On herite de la variable sauf si on l'a deja definie
	  // (auquel cas c'est un clash)
	  TRACE1("HERIT la variablee concrete %s", cur->get_name()->get_value()) ;
	  if (check_no_collision(cur, prefix) == TRUE)
		{
		  T_ident *new_ident = (prefix == NULL)
			? new T_ident(cur, adr_first, adr_last, this)
			: new T_renamed_ident(cur, prefix, adr_first, adr_last, this) ;
		  TRACE2("Mise a jour du def de %x avec la valeur %x", new_ident, cur) ;
		  new_ident->set_def(cur) ;
		  // mise à jour du lien sur l'homonyme de la machine incluse
		  new_ident->set_homonym_in_required_mach(cur);
		  new_ident->set_ref_machine(this) ;
		  fix_inherited(new_ident, cur, ref_lexem) ;
		}
	  cur = (T_ident *)cur->get_next() ;

	  if ( (cur == NULL) && (abstract_done == FALSE) )
		{
		  cur = included_machine->get_abstract_variables() ;
		  adr_first = (T_item **)&first_abstract_variable ;
		  adr_last = (T_item **)&last_abstract_variable ;
		  abstract_done = TRUE ;
		}

	}

  // GP 20/11/98 Test si la machine a deja ete include, pour ne pas importer
  //plusieurs fois les SETS
  if(is_already_incl == FALSE)
	{
	  // On recupere les ensembles de l'incluse
	  cur = included_machine->get_sets() ;

	  if ( (cur != NULL) && (sets_clause == NULL) )
		{
		  // On cree une clause SETS fantome
		  TRACE0("creation d'une clause SETS fantome") ;
		  sets_clause = new T_flag(this,
								   get_betree(),
								   included_machine->get_ref_lexem());
		}
	  //GP 20/11/98 les ensembles ne doivent pas etre renommés
	  while (cur != NULL)
		{
		  TRACE1("HERIT l'ensemble %s", cur->get_name()->get_value()) ;
		  T_ident *new_ident = new T_ident(cur,
										   (T_item **)&first_set,
										   (T_item **)&last_set,
										   this) ;

		  TRACE2("Mise a jour du def de %x avec la valeur %x", new_ident, cur) ;
		  new_ident->set_def(cur) ;
		  // mise à jour du lien sur l'homonyme de la machine incluse
		  new_ident->set_homonym_in_required_mach(cur);
		  new_ident->set_ref_machine(this) ;
		  fix_inherited(new_ident, cur, ref_lexem) ;

		  if (    (new_ident->get_values() != NULL)
				  && (new_ident->get_values()->is_an_ident() == TRUE) )
			{
			  // Pour les valeurs enumeree et constantes, il faut changer
			  // le ref_machine
			  T_ident *cur_value = (T_ident *)(new_ident->get_values()) ;
			  T_ident *ref_value = (T_ident *)(cur->get_values()) ;
			  while (cur_value != NULL)
				{
				  ASSERT(ref_value != NULL) ; // garanti par le clonage
				  TRACE2("Mise a jour du def de %x avec la valeur %x",
						 cur_value, ref_value) ;
				  cur_value->set_def(ref_value) ;
				  cur_value->set_ref_machine(this) ;
				  // mise à jour du lien sur l'homonyme de la machine incluse
				  cur_value->set_homonym_in_required_mach(ref_value);
				  // correction bug 2975
				  fix_inherited(cur_value, ref_value, ref_lexem) ;

				  cur_value = (T_ident *)(cur_value->get_next()) ;
				  ref_value = (T_ident *)(ref_value->get_next()) ;
				}
			}

		  cur = (T_ident *)cur->get_next() ;
		}
	} //if (is_already_incl == FALSE)

  //GP 20/11/98 Ajout a la liste des machines incluses
  if(is_already_incl == FALSE)
	{
	  TRACE1("ajout a la liste des machines incluses: %s",
			 included_machine->get_machine_name()->get_name()->get_value()) ;


	  (void)new T_list_link(included_machine->get_machine_name()->get_name(),
							LINK_OTHER,
							(T_item**)first_already_included,
							(T_item**)last_already_included,
							NULL,
							NULL,
							NULL) ;
	}


  // Il faut stopper si on a eu des erreurs car on ne peut plus
  // continuer, et on risque d'avoir des messages qui begayent (car
  // d'autres controles sont faits plus tard dans add_list_link)
  if (check_errors() == FALSE)
	{
	  stop() ;
	}
}

//
// }{ Fin du fichier
//

