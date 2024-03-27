/******************************* CLEARSY **************************************
* Fichier : $Id: v_find.cpp,v 2.0 2002-07-12 12:53:38 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique : Recherche des identificateurs
*
* Compilations :	-DDEBUG_FIND pour avoir des traces
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
RCS_ID("$Id: v_find.cpp,v 1.30 2002-07-12 12:53:38 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_tstamp.h"

//
//	Fonction de haut niveau qui lance la recherche
//
void T_ident::solve_ident(void)
{
#ifdef DEBUG_FIND
  TRACE2("T_ident(%x:\"%s\")::solve_ident", this, name->get_value()) ;
  TRACE6("SOLVE ident %x:%s type %s, <%s:%d:%d>",
		 this,
		 name->get_value(),
		 get_ident_type_name(),
		 get_ref_lexem()->get_file_name()->get_value(),
		 get_ref_lexem()->get_file_line(),
		 get_ref_lexem()->get_file_column()) ;

#endif

  // Avant la resolution, on prend le timestamp2 prochain pour
  // reinitialiser la recherche de non bouclage
  next_timestamp2() ;


  // On cherche a resoudre name, sauf si prefix != NULL auquel cas
  // name == xx$0 et prefix == xx : on cherche dans ce cas xx
  // Pour cela, les methodes find_ident utiliseront T_ident::get_base_name()

  for (;;)
	{
	  // Avant la resolution, on prend le timestamp2 prochain pour
	  // reinitialiser la recherche de non bouclage
	  next_timestamp() ;

	  // Cas particulier : resolution dans IMPORTS, ..
	  // Il ne faut pas propager la recherche au pere
	  // car sinon on passe dans les machines requises,
	  // alors qu'on n'a pas fini les recherches en cours ie on n'a pas
	  // explore la machine courante
	  //	  TRACE2("ici this %x get_father() %x", this, get_father()) ;

	  T_item *real_father =
		(get_father()->get_node_type() == NODE_USED_MACHINE)
		? get_ref_machine() : get_father() ;

	  T_ident *def_ident = real_father->find_ident(this, TRUE) ;

	  if (def_ident == NULL)
		{
		  // Erreur, identificateur non declare
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_UNDECLARED_IDENT),
						 get_name()->get_value()) ;
		  return ;
		}

#ifdef DEBUG_FIND
	  TRACE1("--------- ON A TROUVE LA DEFINITION DE %s ---", name->get_value()) ;
	  TRACE1("identifiant a resoudre ... %x", this) ;
	  TRACE2("definition associee    ... %x (-> def = %x )",
			 def_ident,
			 def_ident->def) ;
#endif
	  if (this == def_ident)
		{
		  TRACE0("PANIC !!!") ;
		  fprintf(stderr, "Panic resolution de %s:%d:%d: %s",
				  get_ref_lexem()->get_file_name()->get_value(),
				  get_ref_lexem()->get_file_line(),
				  get_ref_lexem()->get_file_column(),
				  get_name()->get_value()) ;

		  return ;
		  assert(FALSE) ;
		}

#ifdef BUG2135 //Deplace dans la fonction T_machine::find_ident
	  //GP 12/11/98
	  //EREN 1 & 2
	  //fonction virtuelle definie pour T_ident & T_renamed_ident
	  check_rename(def_ident) ;
#endif


	  // L'identificateur existe. Il faut regarder s'il est visible
	  // Pour cela, il faut determiner :
	  // 1) le type d'entite accedee (un parametre formel, une cte, ...)
	  // 2) l'endroit de definition de cette entite (une machine vue, ...)
	  // 3) le contexte d'ou l'on accede (la clause CONSTRAINTS, une operation, ...)

	  // 1) calcul du type d'entite accedee
	  T_access_entity_type etype = def_ident->make_entity_type(this) ;

	  if (etype == OTHER_ACCESS)
		{
#ifdef DEBUG_FIND
		  TRACE0("ok, pas besoin de verifier !!!") ;
		  TRACE2("Mise a jour du def de %x avec la valeur %x", this, def_ident) ;
#endif
		  set_def(def_ident) ;
		  return ;
		}

	  // 2) calcul de l'endroit de la definition
	  // Pour calculer loc, on utilise get_ref_machine() pour mettre a jour
	  // le champ ref_machine s'il n'est pas encore positionne
	  // Si l'identificateur est colle, c'est le ref_machine de ref_glued
	  // qu'il faut prendre en compte
	  T_machine *def_machine = (ref_glued == NULL) ?
		get_ref_machine() : ref_glued->get_ref_machine() ;

	  TRACE5("ici ident %x:%s ref_glued %x get_ref_machine %x:%s",
			 this,
			 name->get_value(),
			 ref_glued,
			 def_machine,
			 def_machine->get_machine_name()->get_name()->get_value()) ;

	  TRACE5("ici def_ident %x:%s ref_glued %x get_ref_machine %x:%s",
			 def_ident,
			 def_ident->name->get_value(),
			 def_ident->ref_glued,
			 def_ident->get_ref_machine(),
			 def_ident->get_ref_machine()->get_machine_name()->get_name()->get_value()) ;

	  T_access_entity_location loc = access_entity_location ;

	  //GP 12/01/99
	  //Maintenat le travail est fait par find_ident
	  //T_access_entity_location loc =
	  //make_entity_location (def_machine,
	  //					  def_ident->get_ref_machine()) ;

	  // 3) calcul du contexte d'ou l'on accede
	  T_access_from_context context = make_context(etype) ;

	  if (loc == LOC_NON_VISIBLE)
		{
		  TRACE1("no_visi = <%s>", get_catalog(C_NO_VISIBILITY)) ;
		  TRACE1("access = <%s>", get_access_entity_type_msg(etype)) ;
		  TRACE1("loc = <%s>", get_access_entity_location_msg(loc)) ;
		  TRACE1("ctx = <%s>", get_access_from_context_msg(context)) ;

		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_NON_VISIBLE_ENTITY),
						 name->get_value(),
						 get_catalog(C_NO_VISIBILITY),
						 get_access_entity_type_msg(etype),
						 name->get_value(),
						 def_ident->get_ref_machine()->get_machine_name()
						 ->get_name()->get_value(),
						 get_access_entity_location_msg(loc),
						 get_access_from_context_msg(context),
						 get_ref_machine()->get_machine_name()
						 ->get_name()->get_value()) ;
		  semantic_error_details(def_ident,
								 get_error_msg(E_NON_VISIBLE_ENTITY_DECLARATION),
								 name->get_value()) ;
		}
	  else
		{
		  // Et on verifie ...
		  char *auth_string = NULL ;
		  T_access_authorisation auth = get_auth_with_details(etype,
															  loc,
															  context,
															  &auth_string) ;

		  int ok = check_rights(def_ident, auth) ;

		  if (ok == TRUE)
			{
#ifdef DEBUG_FIND
			  TRACE2("Mise a jour du def de %x avec la valeur %x", this, def_ident) ;
#endif
			  set_def(def_ident) ;
			  return ;
			}
		  else
			{
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_NON_VISIBLE_ENTITY),
							 name->get_value(),
							 auth_string,
							 get_access_entity_type_msg(etype),
							 name->get_value(),
							 def_ident->get_ref_machine()->get_machine_name()
							 ->get_name()->get_value(),
							 get_access_entity_location_msg(loc),
							 get_access_from_context_msg(context),
							 get_ref_machine()->get_machine_name()
							 ->get_name()->get_value()) ;
			  semantic_error_details(def_ident,
									 get_error_msg(E_NON_VISIBLE_ENTITY_DECLARATION),
									 name->get_value()) ;
			}
		}

#ifdef A_FAIRE
	  // A FAIRE ::
	  // Encore une fois !
	  TRACE0("on continue la recherche ...") ;
	  next_timestamp() ;
#else
	  return ;
#endif
	}
}
// GP 16/11/98
// fonction virtuelle, pour verifier les exigences EREN 1 et 2
// Definie pour T_ident et T_renamed_ident
// Retourne NULL si:
// (1)def_ident = NULL
// (2)def_ident ne respecte pas les exigences
T_ident* T_ident::check_rename(T_ident * def_ident)
{
	/*
  TRACE3("THIS(%x)->check_rename(): this est de la classe %s, node_type = %d",
		 this,
		 get_class_name(),
		 get_node_type());
	*/

  //Pour alleger l'appel � la fonction
  if(def_ident == NULL)
	{
	  return NULL ;
	}

  switch(def_ident->get_definition()->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE:
	case ITYPE_ABSTRACT_VARIABLE:
	case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
	case ITYPE_MACHINE_SET_FORMAL_PARAM:
	case ITYPE_USED_OP_NAME:
	case ITYPE_OP_NAME: //La definition est toujours un OP_NAME
	case ITYPE_LOCAL_OP_NAME: //La definition est toujours un OP_NAME
	  {
		// CTRL Ref: EREN 1

		T_machine* local_machine = get_ref_machine() ;
		T_machine* def_machine =
		  def_ident->get_definition()->get_ref_machine() ;

		if (local_machine->get_sees(def_machine) ||
			local_machine->get_includes(def_machine) ||
			local_machine->get_extends(def_machine) ||
			local_machine->get_imports(def_machine) ||
			local_machine->get_uses(def_machine))
		  {

			T_used_machine* used_machine =  def_machine->get_context() ;
			//C'est une machine incluse ou sees ou ...
			ASSERT(used_machine != NULL) ;

			T_symbol* machine_prefix = used_machine->get_instance_name() ;


			if (machine_prefix != NULL)
			  {
				//La machine est pr�fix�e mais pas l'ident
				TRACE2("ERREUR: la machine %s a un prefixe mais pas %s",
					   used_machine->get_name()->get_name()->get_value(),
					   get_name()->get_value());

				//GP 12/03/99 Correction Bug 2135
#ifdef BUG2135 //WAS
				T_symbol* machine_def_name =
				  def_machine->get_machine_name()->get_name() ;
				semantic_error(this,
							   CAN_CONTINUE,
							   get_error_msg(E_VARIABLES_SHOULD_BE_RENAMED),
							   get_name()->get_value(),
							   def_ident->get_ident_type_name(),
							   machine_def_name->get_value());
#else
				//def_ident
				return NULL ;
#endif

			  }
		  }
		//Ref_ident est conforme aux exigences
		return def_ident ;
	  }
	case ITYPE_UNKNOWN:
	  {
		//L'identifiant n'a pas ete resolu
		// Cas de reprise sur erreur
		TRACE0("reprise sur erreur") ;
		return def_ident ;
	  }
	case ITYPE_CONCRETE_CONSTANT:
	case ITYPE_ABSTRACT_CONSTANT:
	case ITYPE_RECORD_LABEL:
	case ITYPE_BUILTIN:
	case ITYPE_MACHINE_NAME:
	case ITYPE_ABSTRACTION_NAME:
	case ITYPE_ABSTRACT_SET:
	case ITYPE_USED_MACHINE_NAME:
	case ITYPE_ENUMERATED_VALUE:
	case ITYPE_DEFINITION_PARAM:
	case ITYPE_OP_IN_PARAM:
	case ITYPE_OP_OUT_PARAM:
	case ITYPE_LOCAL_VARIABLE:
	case ITYPE_LOCAL_QUANTIFIER:
    case ITYPE_ANY_QUANTIFIER:
	case ITYPE_ENUMERATED_SET:
	case ITYPE_PRAGMA_PARAMETER:
	  {
		//Rien a controler
		return def_ident ;
	  }
	}
  //Pour les warnings
  assert(FALSE) ;
  return NULL ;
}

// GP 16/11/98
// fonction virtuelle, pour verifier les exigences EREN 1 et 2
// Definie pour T_ident et T_renamed_ident
// Retourne NULL si:
// (1)def_ident = NULL
// (2)def_ident ne respecte pas les exigences sauf pour les reprsies
// sur erreurs
T_ident* T_renamed_ident::check_rename(T_ident * def_ident)
{
  TRACE3("THIS(%x)->check_rename(): this est de la classe %s, node_type = %d",
		 this,
		 get_class_name(),
		 get_node_type());

  //Pour alleger l'appel � la fonction
  if(def_ident == NULL)
	{
	  return NULL ;
	}

  T_machine* local_machine = get_ref_machine() ;
  T_machine* def_machine =
	def_ident->get_definition()->get_ref_machine() ;
  T_symbol* machine_def_name =
		  def_machine->get_machine_name()->get_name() ;

  switch(def_ident->get_definition()->get_ident_type())
	{
	case ITYPE_CONCRETE_CONSTANT:
	case ITYPE_ABSTRACT_CONSTANT:
	case ITYPE_ABSTRACT_SET:
	case ITYPE_ENUMERATED_SET:
	case ITYPE_ENUMERATED_VALUE:
	  {
		//CTRL Ref: EREN 2

		semantic_error(this,
					   CAN_CONTINUE,
					   get_error_msg(E_CONSTANTS_SHOULD_NOT_BE_RENAMED),
					   get_name()->get_value(),
					   def_ident->get_definition()->get_ident_type_name(),
					   machine_def_name->get_value());

		//Retourne quand meme def_ident pour assurer la reprise sur erreur
		return def_ident ;
	  }
	case ITYPE_CONCRETE_VARIABLE:
	case ITYPE_ABSTRACT_VARIABLE:
	case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
	case ITYPE_MACHINE_SET_FORMAL_PARAM:
	case ITYPE_USED_OP_NAME:
	case ITYPE_OP_NAME: //La definition est toujours un OP_NAME
	case ITYPE_LOCAL_OP_NAME: //La definition est toujours un OP_NAME
	  {
		//CTRL Ref: EREN 1
		if (local_machine->get_sees(def_machine) ||
			local_machine->get_includes(def_machine) ||
			local_machine->get_extends(def_machine) ||
			local_machine->get_imports(def_machine) ||
			local_machine->get_uses(def_machine))
		  {

			T_used_machine* used_machine =  def_machine->get_context() ;
			//C'est une machine incluse ou sees ou ...
			ASSERT(used_machine != NULL) ;

			T_symbol* machine_prefix = used_machine->get_instance_name() ;



			if (machine_prefix == NULL)
			  {
				//L'ident a un prefixe mais pas la machine
				TRACE2("ERREUR: l'ident %s a un prefixe mais pas %s",
					   get_name()->get_value(),
					   used_machine->get_name()->get_name()->get_value()) ;

#ifdef BUG2135 //was
				semantic_error(this,
							   CAN_CONTINUE,
							   get_error_msg(E_VARIABLES_SHOULD_BE_RENAMED),
							   get_name()->get_value(),
							   def_ident->get_ident_type_name(),
							   machine_def_name->get_value());
#else
				// L'ident � un prefixe mais pas la machine
				return NULL ;
#endif

			  }
			else if (machine_prefix != get_instance_name())
			  {
				//les prefixes sont differents
				TRACE2("ERREUR: l'ident %s a un prefixe different %s",
					   get_name()->get_value(),
					   used_machine->get_name()->get_name()->get_value()) ;
#ifdef BUG2135 //was
				semantic_error(this,
							   CAN_CONTINUE,
							   get_error_msg(E_VARIABLES_SHOULD_BE_RENAMED),
							   get_name()->get_value(),
							   def_ident->get_ident_type_name(),
							   machine_def_name->get_value());
#else
				//les prefixes sont differents
				return NULL ;
#endif
			  }
		  }
		return def_ident ;
	  }
	case ITYPE_UNKNOWN:
	  {
		//L'identifiant n'a pas ete resolu
		// Cas de reprise sur erreur
		TRACE0("reprise sur erreur") ;

		//Pour assurer la reprise sur erreur
		return def_ident ;
	  }
	case ITYPE_BUILTIN:
	case ITYPE_MACHINE_NAME:
	case ITYPE_ABSTRACTION_NAME:
	case ITYPE_USED_MACHINE_NAME:
	case ITYPE_DEFINITION_PARAM:
	case ITYPE_OP_IN_PARAM:
	case ITYPE_OP_OUT_PARAM:
	case ITYPE_LOCAL_VARIABLE:
	case ITYPE_LOCAL_QUANTIFIER:
    case ITYPE_ANY_QUANTIFIER:
	case ITYPE_RECORD_LABEL:
	case ITYPE_PRAGMA_PARAMETER:
	  {
		//rien a faire
		return def_ident ;
	  }
	}
   //Pour les warnings
  assert(FALSE) ;
  return NULL ;
}

//
//	{	Fonction auxiliare de recherche dans une liste
//
static T_ident *list_find_ident(T_ident *find_ident,
										 T_ident *first_ident)
{
#ifdef DEBUG_FIND
  TRACE2("list_find_ident(%s, %x)",
		 find_ident->get_base_name()->get_value(), first_ident) ;
#endif
  T_ident *cur_ident = first_ident ;

  for (;;)
	{
#ifdef DEBUG_FIND
	  TRACE1("ident courant: %s",
			 cur_ident == NULL ? "null" : cur_ident->get_name()->get_value());
#endif
	  if (cur_ident == NULL)
		{
		  // TRACE0("pas trouve") ;
		  return NULL ;
		}

	  if (find_ident->get_base_name()->compare(cur_ident->get_name()) == TRUE)
		{
		  // TRACE0("trouve") ;
		  return cur_ident ;
		}

	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}

}

//
//	}{	Recherche generique :: classe T_item
//
T_ident *T_item::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE4("T_item(%x:%s)::find_ident(%s:%x)",
		 this, get_class_name(), find_ident->get_name()->get_value(), find_ident) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  //GP Correction BUG 2132
  T_item *real_father = (get_father()->get_node_type() == NODE_USED_MACHINE)
	? find_ident->get_ref_machine() : get_father() ;

  //was : if (get_father() != NULL)
  if(real_father != NULL)
	{
	  // On demande a la superclasse
	  //was return get_father()->find_ident(find_ident, root_machine) ;
	  return real_father->find_ident(find_ident, root_machine) ;
	}

  // Pas trouve
  return NULL ;
}

//
//	}{	Recherche specifique : classe T_ident
//
T_ident *T_ident::find_ident(T_ident *find_ident, int root_machine)
{
  T_symbol *cur_name = find_ident->get_base_name() ;
#ifdef DEBUG_FIND
  TRACE3("T_ident(%x:%s)::find_ident(%s)",
		 this, name->get_value(), cur_name->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE2("get_timestamp() %d ; get_tmp2() %d : deja fait",
			 get_timestamp(),
			 (size_t)get_tmp2()) ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  // On demande au p�re
  //
  ASSERT(get_father() != NULL);

  T_ident *res = get_father()->find_ident(find_ident, root_machine);
  if (res != NULL && res->get_father() == get_father())
	{
	  if (ident_type == ITYPE_CONCRETE_VARIABLE
		  ||
		  ident_type == ITYPE_CONCRETE_CONSTANT
		  ||
		  ident_type == ITYPE_LOCAL_VARIABLE)
		{
		  // La d�claration trouv� par le p�re est dans la meme clause
		  // d�clarative que this. Il faut v�rifier qu'il est plac�
		  // apr�s this sinon c'est une erreur: on r�f�rence un ident
		  // avant sa d�claration
		  T_ident *cur_ident = this;
		  while (cur_ident != res && cur_ident != NULL)
			cur_ident = (T_ident *)cur_ident->get_next();
		  if (cur_ident != NULL)
			{
			  semantic_error(find_ident,
							 CAN_CONTINUE,
							 get_error_msg(E_UNDECLARED_IDENT),
							 find_ident->get_name()->get_value());
			}
		}
	  if (ident_type == ITYPE_OP_OUT_PARAM
		  ||
		  ident_type == ITYPE_OP_IN_PARAM)
		{
		  semantic_error(find_ident,
						 CAN_CONTINUE,
						 get_error_msg(E_UNDECLARED_IDENT),
						 find_ident->get_name()->get_value());
		}
	}

  return res;

  if (name->compare(cur_name) == TRUE)
	{
#ifdef DEBUG_FIND
	  TRACE0("trouve") ;
#endif
	  if ((size_t)get_tmp() == get_timestamp2())
		{
#ifdef DEBUG_FIND
		  TRACE0("refuse car deja essaye !") ;
#endif
		}
	  else
		{
		  set_tmp((void *)get_timestamp2()) ;
		  return this ;
		}
	}


  // Recherche dans les valeurs s'il y a lieu
  T_expr *cur_value = first_value ;
#ifdef DEBUG_FIND
  if (first_value != NULL)
	{
	  ENTER_TRACE ;
	  TRACE0("recherche dans les valeurs ...") ;
	}
#endif
  while (cur_value != NULL)
	{
	  T_ident *res = cur_value->find_ident(find_ident, root_machine) ;
	  if (res != NULL)
		{
		  TRACE2("trouve dans %x : on renvoie %x", cur_value, res) ;
		  return res ;
		}
	  cur_value = (T_expr *)cur_value->get_next() ;
	}
#ifdef DEBUG_FIND
  if (first_value != NULL)
	{
	  EXIT_TRACE ;
	  TRACE0("pas trouve dans les valeurs ...") ;
	}
#endif

#ifdef DEBUG_FIND
  TRACE0("pas trouve") ;
  #endif

#ifndef NO_CHECKS
  if (get_father() == NULL)
	{
	  TRACE3("PANIC ! %x:%s %s n'a pas de pere",
			 this,
			 name->get_value(),
			 get_ident_type_name()) ;
	  TRACE3("localisation %s:%d:%d",
			 get_ref_lexem()->get_file_name()->get_value(),
			 get_ref_lexem()->get_file_line(),
			 get_ref_lexem()->get_file_column()) ;
	}
#endif
  ASSERT(get_father()) ;

  if (get_father()->get_node_type() == NODE_USED_MACHINE)
	{
	  // Cas particulier ; resolution dans IMPORTS, ..
	  // Il ne faut pas propager la recherche au pere
	  // car sinon on passe dans les machines requises,
	  // alors qu'on n'a pas fini les recherches en cours
	  TRACE0("on rend NULL !") ;
	 return NULL ;
	}
  return get_father()->find_ident(find_ident, root_machine) ;
}

//
// Recherche descendante d'un ident

T_ident *T_ident::lookup_ident(T_ident *ref_ident)
{
  TRACE2("ident recherch�: %s, nom de this: %s",
		 ref_ident->get_base_name()->get_value(),
		 get_base_name()->get_value());
  T_symbol *cur_name = ref_ident->get_base_name() ;
  T_ident *res = NULL;
  if (name->compare(cur_name) == TRUE)
	{
	  TRACE0("meme nom");
	  res = this ;
	}

  // Recherche dans les valeurs s'il y a lieu On recherche un
  // definition on peut donc la trouve uniquement dans les valeurs
  // d'un ensemble �num�r�e
  if (res == NULL && get_ident_type() == ITYPE_ENUMERATED_SET)
	{
	  T_expr *cur_value = first_value ;
	  while (cur_value != NULL && res == NULL)
		{
		  TRACE1("type: %d", cur_value->get_node_type());
		  ASSERT(cur_value->is_an_ident() == TRUE);
		  res = ((T_ident *)cur_value)->lookup_ident(ref_ident) ;
		  cur_value = (T_expr *)cur_value->get_next() ;
		}
	}
  return res;
}

//
//
//	}{	Recherche specifique : classe T_machine
//
static T_ident *FIND_LIST(T_ident *find_ident,
								   T_item *first_list_item,
								   T_symbol *prefix,
								   T_ident  *machine_name,
								   const char *list_name,
								   int root_machine)
{
  T_symbol *find_name = find_ident->get_base_name() ;
  T_ident *ident_to_delete = NULL ;
  T_item *cur = first_list_item ;

  if (prefix != NULL)
	{
	  TRACE4("recherche de %s en prenant en compte le prefixe %s dans %s:%s",
			 find_name->get_value(),
			 prefix->get_value(),
			 machine_name->get_name()->get_value(),
			 list_name) ;

	  //GP 16/11/98 pour le probleme du prefixe
	  //si l'ident = xx_ident a la place de xx.ident le test acceptait
	  //idem avec xxAident
	  //Ajout de && ((find_name->get_value())[prefix_len]=='.')

	  size_t prefix_len = prefix->get_len() ;

	  if ((strncmp(prefix->get_value(), find_name->get_value(), prefix_len)
		   == 0) &&
		  ((find_name->get_value())[prefix_len]=='.'))
		{
		  TRACE0("!!! le nom cherche commence par le prefixe !") ;
		  char *look = (char *)
			((size_t)find_name->get_value() + prefix->get_len() + 1) ;
		  TRACE1("look = %s", look) ;
		  find_name = lookup_symbol(look) ;
		  TRACE1("find_name = %s", find_name->get_value()) ;

		  // On recherche un identificateur avec "le bon nom"
		  TRACE0("avant creation") ;
		  find_ident = new T_ident(find_ident, NULL, NULL, NULL) ;
		  ident_to_delete = find_ident ;
		  TRACE0("apres creation") ;
		  find_ident->set_name(find_name) ;
		}
	}
#ifdef DEBUG_FIND
  else
	{
	  TRACE4("recherche de %s dans %s:%s, list %x",
			 find_name->get_value(),
			 machine_name->get_name()->get_value(),
			 list_name,
			 cur) ;
	}
#endif // DEBUG_FIND

  while (cur != NULL)
	{
	  T_ident *res = NULL;
	  if (cur->get_node_type() == NODE_IDENT)
		{
		  TRACE1("ident %s",((T_ident *)cur)->get_name()->get_value());
		  res = ((T_ident *)cur)->lookup_ident(find_ident);
		}
	  else
		{
		  res = cur->find_ident(find_ident, root_machine) ;
		}
	  if (res != NULL)
		{
		  TRACE4("trouve %x:%s dans %s:%s",
				 res,
				 find_name->get_value(),
				 machine_name->get_name()->get_value(),list_name) ;
		  // On supprime l'ident temporaire
		  if (ident_to_delete != NULL)
			{
			  delete ident_to_delete ;
			}
		  return res ;
		}
	  cur = cur->get_next() ;
	}

  // On supprime l'ident temporaire
  if (ident_to_delete != NULL)
	{
	  delete ident_to_delete ;
	}
  return NULL ;
}
//GP 12/01/99
//positionne le champs ident::access_entity_location
//pour les indent definis dans les machines requises
void static set_access_entity_location(T_ident * ident,
									   T_machine_type machine_type,
									   T_use_type use_type)
{
  switch(use_type)
	{
	case USE_USES:
	  {
		switch(machine_type)
		  {
		  case MTYPE_SPECIFICATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_USED) ;
			  break ;
			}
		  case MTYPE_SYSTEM:
			{
			  //Impossible car USES interdit dans un system
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_REFINEMENT:
			{
			  //Impossible car USES interdit dans un raffinement
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_IMPLEMENTATION:
			{
			  //Impossible car le USES interdit en implementation
			  assert(FALSE) ;
			  break ;
			}
		  }
		break ;
	  }
	case USE_INCLUDES:
	  {
		switch(machine_type)
		  {
		  case MTYPE_SPECIFICATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_INCLUDED) ;
			  break ;
			}
		  case MTYPE_SYSTEM:
			{
			  //IMPOSSIBLE CAR INCLUDES interdit dans un system
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_REFINEMENT:
			{
			  ident->set_access_entity_location(LOC_SPEC_INCLUDED) ;
			  break ;
			}
		  case MTYPE_IMPLEMENTATION:
			{
			  //IMPOSSIBLE CAR INCLUDES interdit dans une implementation
			  assert(FALSE) ;
			  break ;
			}
		  }
		break ;
	  }
	case USE_EXTENDS:
	  {
		switch(machine_type)
		  {
		  case MTYPE_SPECIFICATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_INCLUDED) ;
			  break ;
			}
		  case MTYPE_SYSTEM:
			{
			  //IMPOSSIBLE CAR EXTENDS interdit dans un system
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_REFINEMENT:
			{
			  ident->set_access_entity_location(LOC_SPEC_INCLUDED) ;
			  break ;
			}
		  case MTYPE_IMPLEMENTATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_IMPORTED) ;
			  break ;
			}
		  }
		break ;
	  }
	case USE_SEES:
	  {
		switch(machine_type)
		  {
		  case MTYPE_SPECIFICATION:
		  case MTYPE_SYSTEM:
			{
			  ident->set_access_entity_location(LOC_SPEC_SEEN_BY_SPEC_OR_REF) ;
			  break ;
			}
		  case MTYPE_REFINEMENT:
			{
			  ident->set_access_entity_location(LOC_SPEC_SEEN_BY_SPEC_OR_REF) ;
			  break ;
			}
		  case MTYPE_IMPLEMENTATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_SEEN_BY_IMPLEMENTATION) ;
			  break ;
			}
		  }
		break ;
	  }
	case USE_IMPORTS:
	  {
		switch(machine_type)
		  {
		  case MTYPE_SPECIFICATION:
		  case MTYPE_SYSTEM:
			{
			  //Impossible car IMPORTS interdit dans une abstraction
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_REFINEMENT:
			{
			  //Impossible car IMPORTS interdit dans une abstraction
			  assert(FALSE) ;
			  break ;
			}
		  case MTYPE_IMPLEMENTATION:
			{
			  ident->set_access_entity_location(LOC_SPEC_IMPORTED) ;
			  break ;
			}
		  }
		break ;
	  }

	}
}


//GP 12/01/99
//positionne le champs ident::access_entity_location
//pour les ident definis en local
void static set_access_entity_location(T_ident * ident,
									   T_machine_type machine_type)
{
  switch(machine_type)
	{
	case MTYPE_SPECIFICATION:
	case MTYPE_SYSTEM: //TODO LB A valider
	  {
		ident->set_access_entity_location(LOC_SAME_SPECIFICATION) ;
		break ;
	  }
	case MTYPE_REFINEMENT:
	  {
		ident->set_access_entity_location(LOC_SAME_REFINEMENT) ;
		break ;
	  }
	case MTYPE_IMPLEMENTATION:
	  {
		ident->set_access_entity_location(LOC_SAME_IMPLEMENTATION) ;
		break ;
	  }
	}
}

//GP 12/01/99
//positionne le champs ident::access_entity_location
//pour les ident definis dans l'abstraction
void static set_raf_access_entity_location(T_ident * ident,
									   T_machine_type machine_type)
{
  switch(machine_type)
	{
	case MTYPE_SPECIFICATION:
	case MTYPE_SYSTEM:
	  {
		//Impossible une sepc ne peut pas etre raffinee
		assert(FALSE) ;
		break ;
	  }
	case MTYPE_REFINEMENT:
	  {
		ident->set_access_entity_location(LOC_REFINEMENT_ABSTRACTION) ;
		break ;
	  }
	case MTYPE_IMPLEMENTATION:
	  {
		ident->set_access_entity_location(LOC_IMPLEMENTATION_ABSTRACTION) ;
		break ;
	  }
	}
}

//
//	}{	Recherche specifique : classe T_machine
//
T_ident *T_machine::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_machine(%x:%s)::find_ident(%s)",
		 this, machine_name->get_name()->get_value(), find_ident->get_name()->get_value()) ;
#endif

  T_symbol *prefix = NULL ;
  if ( (find_ident->get_ref_machine() != this) && (get_context() != NULL) )
	{
	  // Machine renommee. Il faut eventuellement enlever le prefixe de find_name
	  // Bien sur si find_ident->get_ref_machine() == this on recherche un
	  // identificateur de la machine dans la machine et on ne prend pas en
	  // compte le prefixe
	  prefix = get_context()->get_instance_name() ;
	}

  int is_a_USES_machine =
	(context != NULL && context->get_use_type()==USE_USES) ? TRUE : FALSE ;

#ifdef DEBUG_FIND
  TRACE1("prefixe de renommage pour cette machine -> %s",
		 (prefix == NULL) ? "(null)" : prefix->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
#ifdef DEBUG_FIND
	  TRACE2("get_timestamp()%d, get_tmp2()%d -> deja fait",
			 get_timestamp(),
			 get_tmp2()) ;
#endif
	  // Deja fait
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif

  set_tmp2(get_timestamp()) ;
  T_ident *res = NULL ;

  //GP 21/03/99 correction BUG 2135
  //L'appel � find_ident->check_rename(res) permet deverifier la compatibilite
  //de renomage entre l'ident (def_ident) et sa definition (res)
  if (root_machine == TRUE)
	{
	  res = FIND_LIST(find_ident, first_sees, NULL, machine_name, "SEES", FALSE) ;

	  if (find_ident->check_rename(res) != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type(), USE_SEES) ;
		  return res ;
		}

	  res = FIND_LIST(find_ident,
					  first_includes,
					  NULL,
					  machine_name,
					  "INCLUDES",
					  FALSE) ;

	  if (find_ident->check_rename(res) != NULL)
		{
		  set_access_entity_location(find_ident,
									 get_machine_type(),
									 USE_INCLUDES) ;
		  return res ;
		}

	  res = FIND_LIST(find_ident,
					  first_imports,
					  NULL,
					  machine_name,
					  "IMPORTS",
					  FALSE) ;
	  if (find_ident->check_rename(res) != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type(), USE_IMPORTS) ;
		  return res ;
		}

	  res = FIND_LIST(find_ident,
					  first_extends,
					  NULL,
					  machine_name,
					  "EXTENDS",
					  FALSE) ;
	  if (find_ident->check_rename(res) != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type(), USE_EXTENDS) ;
		  return res ;
		}

	  res = FIND_LIST(find_ident, first_uses, NULL, machine_name, "USES", FALSE) ;
	  if (find_ident->check_rename(res) != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type(), USE_USES) ;

		  return res ;
		}

	}

  // Il faut chercher dans :
  // SETS, CONCRETE_VARIABLES, ABSTRACT_VARIABLES,
  // CONCRETE_CONSTANTS, ABSTRACT_CONSTANTS ...
  // et dans toutes les valeurs enumerees
  res = FIND_LIST(find_ident, first_set, NULL, machine_name, "SETS", root_machine) ;
  if (res != NULL)
	{
	  set_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}


  if ((root_machine == TRUE) || (is_a_USES_machine == TRUE))
	{
	  //Les parametres sont visibles seulement �
	  //l'interieur de la mchine elle meme
	  //ou si c'est une machine USES

	  res = FIND_LIST(find_ident,
					  first_param,
					  prefix,
					  machine_name,
					  "FORMAL_PARAMETERS",
					  root_machine) ;

	  if (res != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type()) ;
		  return res ;
		}

	}

  res = FIND_LIST(find_ident,
				  first_concrete_variable,
				  prefix,
				  machine_name,
				  "CONCRETE_VARIABLES",
				  root_machine) ;
  if (res != NULL)
	{
	  set_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}

  res = FIND_LIST(find_ident,
				  first_abstract_variable,
				  prefix,
				  machine_name,
				  "ABSTRACT_VARIABLES",
				  root_machine) ;
  if (res != NULL)
	{
	  set_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}

  res = FIND_LIST(find_ident,
				  first_concrete_constant,
				  NULL,
				  machine_name,
				  "CONCRETE_CONSTANTS",
				  root_machine) ;
  if (res != NULL)
	{
	  set_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}

  res = FIND_LIST(find_ident,
				  first_abstract_constant,
				  NULL,
				  machine_name,
				  "ABSTRACT_CONSTANTS",
				  root_machine) ;
  if (res != NULL)
	{
	  set_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}

  // Recherche dans les operations
  if((root_machine == TRUE) || (is_a_USES_machine == FALSE))
	{
	  //Les operations d'une machine USES ne sont pas visibles

	  // On commence par chercher dans les operations locales (i.e. la spec)
	  res = FIND_LIST(find_ident,
					  first_local_operation,
					  prefix,
					  machine_name,
					  "LOCAL_OPERATIONS",
					  root_machine) ;
	  if (res != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type()) ;
		  return res ;
		}

	  // Puis dans les operations propres
	  res = FIND_LIST(find_ident,
					  first_op_list,
					  prefix,
					  machine_name,
					  "OPERATIONS",
					  root_machine) ;
	  if (res != NULL)
		{
		  set_access_entity_location(find_ident,get_machine_type()) ;
		  return res ;
		}
	}

  // On cherche dans l'abstraction
  // ce n'est pas une liste mais on peut reutiliser la macro quand meme
  res = FIND_LIST(find_ident,
				  ref_abstraction,
				  NULL,
				  machine_name,
				  "ABSTRACTION",
				  FALSE) ;
  if (res != NULL)
	{
	  set_raf_access_entity_location(find_ident,get_machine_type()) ;
	  return res ;
	}


  // On a rien trouve de visible ;
  return NULL ;
}

//
//	}{	Recherche specifique : classe T_op
//
T_ident *T_op::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_op(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
  ASSERT(get_node_type() == NODE_OPERATION) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;


#ifdef DEBUG_FIND
  ENTER_TRACE ; ENTER_TRACE ;
  TRACE0("recherche dans le nom de l'operation") ;
#endif

  T_ident *res = name->lookup_ident(find_ident);//find_ident(find_ident, root_machine) ;

#ifdef DEBUG_FIND
  EXIT_TRACE ; EXIT_TRACE ;
#endif

  if (res != NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("trouve") ;
#endif
	  return res ;
	}

  // On ne cherche dans les parametres que si on est
  // dans la bonne operation
  if (root_machine == TRUE)
	{
	  T_machine *mach = NULL ;
	  T_op *op = NULL ;
	  find_ident->find_machine_and_op_def(&mach, &op) ;

	  if (op == this)
		{
#ifdef DEBUG_FIND
		  ENTER_TRACE ; ENTER_TRACE ;
		  TRACE0("recherche dans les in_params") ;
#endif

		  T_ident *cur_ident = first_in_param ;
		  int out_parsed = FALSE ;

		  for (;;)
			{
			  if (cur_ident == NULL)
				{
#ifdef DEBUG_FIND
				  TRACE0("pas trouve") ;
#endif
				  if (out_parsed == FALSE)
					{
#ifdef DEBUG_FIND
					  EXIT_TRACE ; EXIT_TRACE ;
					  TRACE0("on passe aux out_params") ;
					  ENTER_TRACE ; ENTER_TRACE ;
#endif
					  out_parsed = TRUE ;
					  cur_ident = first_out_param ;
					  continue ;
					}
#ifdef DEBUG_FIND
				  EXIT_TRACE ; EXIT_TRACE ;
				  TRACE0("pas trouve -> on demande au pere") ;
#endif
				  return get_father()->find_ident(find_ident, root_machine) ;
				}

			  T_ident *res = cur_ident->lookup_ident(find_ident);//, root_machine) ;
              set_access_entity_location(find_ident,mach->get_machine_type());
			  if (res != NULL)
				{
#ifdef DEBUG_FIND
				  TRACE0("trouve") ;
#endif
				  return res ;
				}

			  cur_ident = (T_ident *)cur_ident->get_next() ;
			}
		}
	}
  return NULL ;
}


//
//	}{	Recherche specifique : classe T_used_op
//
T_ident *T_used_op::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_used_op(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = name->lookup_ident(find_ident);//find_ident(find_ident, root_machine) ;

  if (res != NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("trouve") ;
#endif
	  return res ;
	}

#ifdef DEBUG_FIND
  TRACE0("pas trouve") ;
#endif
  ASSERT(get_father()) ;
  return get_father()->find_ident(find_ident, root_machine) ;
}

//
//	}{	Recherche specifique : classe T_var
//
T_ident *T_var::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_var(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_ident) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_lambda_expr
//
T_ident *T_lambda_expr::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_lambda_expr(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_universal_predicate
//
T_ident *T_universal_predicate::find_ident(T_ident *find_ident,
													int root_machine)
{
#ifdef DEBUG_FIND
  TRACE4("T_universal_predicate(%x:%s)::find_ident(%s:%x)",
		 this, get_class_name(), find_ident->get_name()->get_value(), find_ident) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve, on demande au pere") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

#ifdef DEBUG_FIND
  TRACE0("trouve") ;
#endif
  return res ;
}

//
//	}{	Recherche specifique : classe T_existential_predicate
//
T_ident *T_existential_predicate::find_ident(T_ident *find_ident,
													  int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_existential_predicate(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_quantified_union
//
T_ident *T_quantified_union::find_ident(T_ident *find_ident,
												 int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_quantified_union(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_quantified_intersection
//
T_ident *T_quantified_intersection::find_ident(T_ident *find_ident,
														int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_quantified_intersection(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_sigma
//
T_ident *T_sigma::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_sigma(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_pi
//
T_ident *T_pi::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_pi(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_variable) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_used_machine
//
T_ident *T_used_machine::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_used_machine(%x:%s)::find_ident(%s)",
		 this,
		 name->get_name()->get_value(),
		 find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;


  if (ref_machine != NULL)
	{
	  // Test toujours vrai dans le cas nominal, present ici pour
	  // pouvoir continuer en cas d'erreur
#ifdef DEBUG_FIND
	  TRACE0("on passe la demande a la machine") ;
#endif
	  ENTER_TRACE ; ENTER_TRACE ;
	  T_ident *res = ref_machine->find_ident(find_ident, root_machine) ;
	  EXIT_TRACE ; EXIT_TRACE ;
	  if (res != NULL)
		{
		  return res ;
		}
	}
  else
	{
	  TRACE0("AIE AIE AIE ref_machine = NULL") ;
	}


  ASSERT(get_father()) ;
  return get_father()->find_ident(find_ident, root_machine) ;

#if 0
  //GP 24/02/99 CORRECTION DU BUG 2132
  //dans le cas d'une T_used machine, la recherche ne se fait pas dans
  //la machine associ�e � la T_used_machine mais dans la machine d'utilisation
  //du find_ident
  return find_ident->get_ref_machine()->find_ident(find_ident, root_machine) ;
#endif

}

//
//	}{	Recherche specifique : classe T_any
//
T_ident *T_any::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_any(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_ident) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
    } else {
      find_ident->set_access_entity_location(LOC_SAME_SPECIFICATION) ;
  }

  return res ;
}

//
//	}{	Recherche specifique : classe T_let
//
T_ident *T_let::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_let(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_ident) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_witness
//
T_ident *T_witness::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_witness(%x:%s)::find_ident(%s)",
                 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
        {
          // Deja fait
#ifdef DEBUG_FIND
          TRACE0("deja fait") ;
#endif
          return NULL ;
        }

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = NULL;//list_find_ident(find_ident, first_ident) ;
  T_ident *res_any = NULL;//list_find_ident(find_ident, first_ident) ;

  ASSERT(get_father()) ;
  res = get_father()->find_ident(find_ident, root_machine) ;

  bool ident_in_predicate = false;

  T_item *father = find_ident->get_father();
  while (father != this && father != NULL) {
      if (father->get_node_type() == NODE_TYPING_PREDICATE
              && father->get_father() == this) {
          ident_in_predicate = true;
          break;
      }
      father = father->get_father();
  }

  if (!ident_in_predicate) {
      return res;
  }

  T_machine *mach = NULL ;
  T_op *op = NULL ;
  find_ident->find_machine_and_op_def(&mach, &op) ;

  if (op != NULL) {

      T_ident *ref_op = op->get_ref_operation();
      if (ref_op == NULL) {
          ref_op = op->get_name();
      }
      while (ref_op != NULL) {
          bool found = false;
          T_symbol *name = ref_op->get_name();
          T_generic_op *used_spec_op;
          T_machine *abstract = mach->get_ref_abstraction() ;
          while (abstract != NULL && found == false) {
              used_spec_op = abstract->find_op(name);

              if (used_spec_op != NULL) {
                  found = true;
                  break;
              }
              abstract = abstract->get_ref_abstraction();
          }

          if (found) {
              // On cherche la definition
              while (used_spec_op->get_node_type() == NODE_USED_OP)
              {
                  used_spec_op = ((T_used_op *)used_spec_op)->get_ref_op() ;
                  ASSERT(used_spec_op != NULL) ;
              }
              T_op *spec_op = (T_op *)used_spec_op ; // Cast justifie par le while
              T_substitution *sub = spec_op->get_body();
              if (sub->get_node_type() == NODE_ANY) {
                  res_any = list_find_ident(find_ident, ((T_any*) sub)->get_identifiers()) ;
                  if (res_any != NULL) {
                      find_ident->set_access_entity_location(LOC_REFINEMENT_ABSTRACTION);
                      break;
                  }
              }
          }
          ref_op = (T_ident *)ref_op->get_next() ;
      }
  }

  if (res != NULL) {
      if (res_any != NULL) {
          // L'identificateur a ete trouve dans les variables d'un any d'un evenement raffine
          // et aussi dans le contexte local.
          semantic_error(find_ident,
                         CAN_CONTINUE,
                         get_error_msg(E_EVENTB_AMBIGUOUS_WITNESS),
                         find_ident->get_name()->get_value()) ;
          return res;
      } else {
          return res;
      }
  } else {
      return res_any;
  }

}

//
//	}{	Classe T_ident : autres methodes
//
void T_ident::extra_checks(void)
{
#ifdef DEBUG_FIND
  TRACE4("T_ident(%x:%s)::extra_checks() def = %x:%x",
		 this,
		 name->get_value(),
		 def,							// def != NULL verifie par appelant
		 def->name->get_value()) ;
#endif

  // Verifications a effectuer
  // 1) Que dans le cas x := y, y n'est pas une operation
  //    (car dans ce cas il faut ecrire x <-- y)

  // Cas 1)
  // On regarde si le pere est un T_affect et si def est un ITYPE_OP_NAME
  if (	(get_father()->get_node_type() == NODE_AFFECT)
		&& (      (def->get_ident_type() == ITYPE_OP_NAME)
		      ||  (def->get_ident_type() == ITYPE_LOCAL_OP_NAME)
		   )
	 )
	{
	  // Syntaxe illegale pour un appel d'operation !

	  // Deux cas se presentent :
	  // 1) l'ident est a droite du ':=', c'est un appel d'operation
	  // avec ':=' au lieu de '<--'
	  // 2) l'ident est a gauche du ':=', c'est un acces interdit
	  // On regarde donc de quel cote est l'ident !
	  T_affect *affect = (T_affect *)get_father() ; // cast valide car "if"
	  T_expr *cur = affect->get_lvalues() ;

	  while (cur != NULL)
		{
		  if (cur == this)
			{
			  // Cas 2 : ident a gauche du ':='
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_CAN_NOT_WRITE_OP),
							 name->get_value()) ;
			  semantic_error_details(def,
									 get_error_msg(E_LOCATION_OF_ITEM_DEF),
									 name->get_value()) ;
			  return ;
			}

		  cur = (T_expr *)cur->get_next() ;
		}

	  // On n'a pas trouve l'ident a gauche du ':=', donc il est
	  // a droite !
	  TRACE1("erreur : operation %s appelee avec :=", name->get_value()) ;
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_INVALID_OP_CALL),
					 name->get_value()) ;
	  semantic_error_details(def,
							 get_error_msg(E_LOCATION_OF_ITEM_DEF),
							 name->get_value()) ;
  } else if( def->get_ident_type() == ITYPE_OP_NAME
             || def->get_ident_type() == ITYPE_LOCAL_OP_NAME )
  {
      // cas 2)
      // l'identificateur peut se trouver dans une expression composite
      // par exemple avec xx(1) := 2, il est sous l'opérateur binaire "("

      // cherchons un père qui soit une affectation
      T_item * cursor = get_father();
      while(cursor != NULL && cursor->get_node_type() != NODE_AFFECT){
          cursor = cursor->get_father();
      }

      // si on l'a trouvé, c'est une expression erronée
      if( cursor != NULL && cursor->get_node_type() == NODE_AFFECT){
          semantic_error
                  (this,
                   CAN_CONTINUE,
                   get_error_msg(E_INVALID_OP_CALL),
                   name->get_value()) ;
          semantic_error_details
                  (def,
                   get_error_msg(E_LOCATION_OF_ITEM_DEF),
                   name->get_value()) ;
          return ;
      }

  }
}

//
//	}{	Recherche specifique : classe T_comprehensive_set
//
T_ident *T_comprehensive_set::find_ident(T_ident *find_ident,
												  int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_comprehensive_set(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  T_ident *res = list_find_ident(find_ident, first_ident) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}

//
//	}{	Recherche specifique : classe T_list_link
//
T_ident *T_list_link::find_ident(T_ident *find_ident, int root_machine)
{
#ifdef DEBUG_FIND
  TRACE3("T_list_link(%x:%s)::find_ident(%s)",
		 this, get_class_name(), find_ident->get_name()->get_value()) ;
#endif

  if (get_timestamp() == (size_t)get_tmp2())
	{
	  // Deja fait
#ifdef DEBUG_FIND
	  TRACE0("deja fait") ;
#endif
	  return NULL ;
	}

#ifdef DEBUG_FIND
  TRACE2("mise a jour de tmp2 de %x a la valeur %d", this, get_timestamp()) ;
#endif
  set_tmp2(get_timestamp()) ;

  ASSERT(object->is_an_item() == TRUE) ; // Par construction
  // Cast valide par construction, justifie par l'assertion ci-dessus
  T_ident *res = ((T_item *)object)->find_ident(find_ident, root_machine) ;

  if (res == NULL)
	{
#ifdef DEBUG_FIND
	  TRACE0("pas trouve") ;
#endif
	  ASSERT(get_father()) ;
	  return get_father()->find_ident(find_ident, root_machine) ;
	}

  return res ;
}


//
//	}	Fin des methodes de recherche
//

