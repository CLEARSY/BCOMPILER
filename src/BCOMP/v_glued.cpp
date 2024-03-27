/******************************* CLEARSY **************************************
* Fichier : $Id: v_glued.cpp,v 2.0 2002-02-08 14:27:29 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Recherche des donnees collees
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
RCS_ID("$Id: v_glued.cpp,v 1.22 2002-02-08 14:27:29 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//Verifie que deux donnes colles sont compatibles
//COLL 3-1 et COLL 3-2 COLL 4-1
static void check_glued_compatibility (T_ident* original_ident,
												T_ident* glued_ident)
{
  //GP 4/11/98
  //Ici rajoute le test pour verifier que les valeurs
  //enumeree de l'ensemble colles sont dans le meme
  //ordre et egaux au valeurs enumerees de l'ensemble initial
  // CTRL Ref: COLL 3-1
  // CTRL Ref: COLL 3-2

  //Cette fonction doit etre appele avec des parametres non nul
  ASSERT (original_ident != NULL) ;
  ASSERT (glued_ident != NULL) ;
  TRACE5("cur_set %x(%s) est collé avec cur_glued_set %x(%s) de la machine: %s",
		 original_ident,
		 original_ident->get_name()->get_value(),
		 glued_ident,
		 glued_ident->get_name()->get_value(),
		 glued_ident->get_ref_machine()->get_machine_name()->
		 get_name()->get_value()
		 ) ;

  switch (original_ident->get_ident_type())
	{
	case ITYPE_ENUMERATED_SET:
	  {

		if (glued_ident->get_ident_type() != ITYPE_ENUMERATED_SET)
		  {
			TRACE0("ce n'est pas un ensemble enumere") ;
			semantic_error(glued_ident,
						   CAN_CONTINUE,
						   get_error_msg(E_IDENT_CLASH),
						   glued_ident->get_name()->get_value(),
						   glued_ident->get_ident_type_name(),
						   original_ident->get_ident_type_name()) ;
			semantic_error_details(original_ident,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
		  }
		else
		  {
			// Le cast de T_expr en T_ident est valide pour get_values
			// car on est dans le cas d'un ensemble enumere, et pas
			// d'une constante
			T_ident* original_val = (T_ident*)original_ident->get_values() ;
			T_ident* glued_val =(T_ident*)glued_ident->get_values();

			//Vrai car ident_type == ITYPE_ENUMERATED_SET
			ASSERT (original_val != NULL) ;
			ASSERT (glued_val != NULL) ;
			// Dans les ensembles enumeres la fonction
			//get_first_values renvoi obligatoirement
			//un T_literal_enumerated_value
			ASSERT(original_val->get_node_type() ==
				   NODE_LITERAL_ENUMERATED_VALUE) ;
			ASSERT(glued_val->get_node_type() ==
				   NODE_LITERAL_ENUMERATED_VALUE );

			while ((original_val != NULL) && (glued_val != NULL))
			  {

				TRACE4("compare %x(%s) et %x(%s)",
					   original_val,
					   original_val->get_name()->get_value(),
					   glued_val,
					   glued_val->get_name()->get_value()) ;

				if (original_val->get_name() !=
					glued_val->get_name())
				  {
					semantic_error(glued_val,
								   CAN_CONTINUE,
								   get_error_msg(E_ENUMERATED_IDENT_DIFFER),
								   glued_ident->get_name()->get_value(),
								   glued_val->get_name()->get_value(),
								   original_val->get_name()->get_value()) ;
					semantic_error_details(original_val,
										   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;


				  }

				original_val = (T_ident*)original_val->get_next() ;
				glued_val = (T_ident*)glued_val->get_next() ;
			  }

			//effet de bord avec original_val et glued_val
			while(original_val != NULL)
			  {
				semantic_error(glued_ident,
							   CAN_CONTINUE,
							   get_error_msg(E_ENUMERATED_IDENT_MISSING),
							   glued_ident->get_name()->get_value(),
							   original_val->get_name()->get_value()) ;
				semantic_error_details(original_val,
									   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
				original_val = (T_ident*)original_val->get_next() ;

			  }

			while(glued_val != NULL)
			  {
				semantic_error(glued_val,
							   CAN_CONTINUE,
							   get_error_msg(E_ENUMERATED_IDENT_EXCESS),
							   glued_ident->get_name()->get_value(),
							   glued_val->get_name()->get_value()) ;
				semantic_error_details(original_ident,
									   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
				glued_val = (T_ident*)glued_val->get_next() ;
			  }

		  }
		break ;
	  }
	case ITYPE_ABSTRACT_SET:
	  {
		if (glued_ident->get_ident_type() != ITYPE_ABSTRACT_SET)
		  {
			TRACE0("ce n'est pas un ensemble abstrait") ;
			semantic_error(glued_ident,
						   CAN_CONTINUE,
						   get_error_msg(E_IDENT_CLASH),
						   glued_ident->get_name()->get_value(),
						   glued_ident->get_ident_type_name(),
						   original_ident->get_ident_type_name()) ;
			semantic_error_details(original_ident,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
		  }
		break ;
	  }
	case ITYPE_ABSTRACT_VARIABLE:
	  //i.e: le type de la variable de la machine requise est abstrait
	  {
		// CTRL Ref: COLL 4-1
		if(glued_ident->get_ident_type() == ITYPE_CONCRETE_VARIABLE )
		  {
			TRACE0("COLL 4-1 non respecte") ;
			semantic_error(original_ident,
						   CAN_CONTINUE,
						   get_error_msg(E_CAN_NOT_REFINES),
						   original_ident->get_ident_type_name(),
						   original_ident->get_name()->get_value(),
						   glued_ident->get_ident_type_name()) ;
			semantic_error_details(glued_ident,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
		  }
		//ICI Les seuls types possibles pour glued_ident sont
		//ITYPE_ABSTRACT_VARIABLE & ITYPE_CONCRETE_VARIABLE
		//Car la fonction "lookup_ident_in_needed_components"
		//opere une selection sur son domaine de recherche
		break;
	  }
	case ITYPE_CONCRETE_VARIABLE:
	  {
		//ICI Les seuls types possibles pour glued_ident sont
		//ITYPE_ABSTRACT_VARIABLE & ITYPE_CONCRETE_VARIABLE
		//Car la fonction "lookup_ident_in_needed_components"
		//opere une selection sur son domaine de recherche
		break;
	  }

	case ITYPE_ABSTRACT_CONSTANT:
	  {
		// CTRL Ref: COLL 5-1
		if(glued_ident->get_ident_type() == ITYPE_CONCRETE_CONSTANT)
		   {
			 TRACE0("COLL 5-1 non respecte") ;
			 semantic_error(original_ident,
							CAN_CONTINUE,
							get_error_msg(E_CAN_NOT_REFINES),
							original_ident->get_ident_type_name(),
							original_ident->get_name()->get_value(),
							glued_ident->get_ident_type_name()) ;
			 semantic_error_details(glued_ident,
									get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));

		   }
		//ICI Les seuls types possibles pour glued_ident sont
		//ITYPE_ABSTRACT_CONSTANT & ITYPE_CONCRETE_CONSTANT
		//Car la fonction "lookup_ident_in_needed_components"
		//opere une selection sur son domaine de recherche

		break ;
	  }
	case ITYPE_CONCRETE_CONSTANT:
	  {
		//ICI Les seuls types possibles pour glued_ident sont
		//ITYPE_ABSTRACT_CONSTANT & ITYPE_CONCRETE_CONSTANT
		//Car la fonction "lookup_ident_in_needed_components"
		//opere une selection sur son domaine de recherche

		break ;
	  }
	default:
	  {
		return ;
	  }

	}
  return ;
}

// Recherche des donnees collees
void T_machine::internal_lookup_glued_data(T_ident *list_ident,
													T_item **adr_first_conc,
													T_item **adr_last_conc,
													T_item **adr_first_abs,
													T_item **adr_last_abs)
{
  TRACE3("T_machine(%s)::internal_lookup_glued_data list_ident=%x %s",
		 machine_name->get_name()->get_value(),
		 list_ident,
		 (list_ident == NULL) ? "" : list_ident->get_name()->get_value()) ;
  ENTER_TRACE ;

  T_ident *cur = list_ident ;

  while (cur != NULL)
	{
	  TRACE3("etude de %x (%s::%s)",
			 cur,
			 cur->get_ref_machine()->get_machine_name()->get_name()->get_value(),
			 cur->get_name()->get_value()) ;

	  ENTER_TRACE ; ENTER_TRACE ;

	  T_machine *ref_machine = NULL ;
	  T_ident *res = lookup_ident_in_needed_components(cur, &ref_machine) ;

	  if (res != NULL)
		{
		  TRACE3("GLU : %x:%s est colle, provenance : %s",
				 cur,
				 cur->get_name()->get_value(),
				 ref_machine->machine_name->get_name()->get_value()) ;
		  TRACE3("res : %x:%s machine %s",
				 res,
				 res->get_name()->get_value(),
				 res->get_ref_machine()->get_machine_name()->get_name()->get_value()) ;


		  //GP 11/6/98
		  //COLL 3-1 et COLL 3-2 et COLL 4-1
		  check_glued_compatibility (res, cur) ;

		  // On met a jour les anneaux d'homonymie
		  cur->insert_in_glued_ring(res) ;

		  // On choisit la liste de destination : l'ident peut
		  // devenir concret s'il est colle avec un ident concret
		  T_ident_type rtype = res->get_ident_type() ;
		  T_item **adr_first ;
		  T_item **adr_last ;

		  switch (rtype)
			{
			case ITYPE_CONCRETE_VARIABLE :
			case ITYPE_CONCRETE_CONSTANT :
			case ITYPE_ABSTRACT_SET :
			case ITYPE_ENUMERATED_SET :
			  {
				adr_first = adr_first_conc ;
				adr_last = adr_last_conc ;
				TRACE2("on utilise la liste conc (%x, %x)", adr_first, adr_last) ;
				break ;
			  }

			default :
			  {
				adr_first = adr_first_abs ;
				adr_last = adr_last_abs ;
				TRACE2("on utilise la liste abs (%x, %x)", adr_first, adr_last) ;
			  }
			}

		  TRACE7("On va ajouter l'ident %s a la machine %x:%s (%x,%x)->(%x,%x)",
				 res->get_name()->get_value(),
				 this,
				 machine_name->get_name()->get_value(),
				 adr_first,
				 adr_last,
				 *adr_first,
				 *adr_last) ;

		  // Il faut regarder si on a deja la variable
		  // c'est possible si la variable provient d'une
		  // machine incluse :
		  //
		  //      AA (abs.var : var)         CC (c.var : var)
		  //       ^                         ^
		  //       |refines                  |includes
		  //       |                         |
		  //     AA_r -----------------------|
		  // Lorsque que l'on detecte le collage de var, on
		  // a deja var dans la liste de AA_r car AA_r includes CC
		  // donc a deja recupere ses donnees

		  TRACE1("recherche d'une occurence precedente de %s",
				 res->get_name()->get_value()) ;
		  //GP 20/11/98
		  //Correction du collage des T_renamed_ident
		  T_ident *cur_loc_id = (T_ident *)*adr_first ;
		  int found = FALSE ;

		  while ( (cur_loc_id != NULL) && (found == FALSE) )
			{
			  //GP was : if (cur_loc_id->get_name()->compare(res->get_name()) == TRUE)
			  if (cur_loc_id->get_name()->compare(cur->get_name()) == TRUE)
				{
				  found = TRUE ;
				}
			  else
				{
				  cur_loc_id = (T_ident *)cur_loc_id->get_next() ;
				}
			}
		  T_ident *new_ident = NULL ;
		  if (found == TRUE)
			{
			  TRACE1("on reutilise %x", cur_loc_id) ;
			  new_ident = cur_loc_id ;


			}
		  else
			{
			  TRACE0("on cree l'ident") ;
			  //GP 20/11/98
			  // was: new_ident = new T_ident(res, adr_first, adr_last, this) ;

			  if(cur->get_node_type()==NODE_RENAMED_IDENT)
				{
				  new_ident = new T_renamed_ident(res,
												  ((T_renamed_ident*)cur)
												  ->get_instance_name(),
												  adr_first,
												  adr_last,
												  this) ;
				}
			  else
				{
				 new_ident = new T_ident(res, adr_first, adr_last, this) ;
				}

			  // Appel de reset_machine_and_op_def pour que le
			  // prochain appel a cette methode rende le resultat dans
			  // l'implementation courante, et pas dans la machine qui
			  // definit la variable collee (ref_machine)
			  new_ident->reset_machine_and_op_def() ;
			  if (new_ident->get_ident_type() == ITYPE_ENUMERATED_SET)
				{
				  T_ident *enum_value = (T_ident *)new_ident->get_values() ;

				  while (enum_value != NULL)
					{
					  enum_value->reset_machine_and_op_def() ;
					  enum_value = (T_ident *)enum_value->get_next() ;
	 				}
				}
			  // Correction du lexeme : on le met dans la clause d'imports courant
			  // Il faut trouver la clause d'utilisation de ref_machine
			  T_used_machine *context = ref_machine->get_context() ;
			  if (context != NULL)
				{
				  new_ident->set_ref_lexem(context->get_ref_lexem());
				  if (new_ident->get_ident_type() == ITYPE_ENUMERATED_SET)
					{
					  T_ident *enum_value = (T_ident *)new_ident->get_values();
					  while (enum_value != NULL)
						{
						  enum_value->set_ref_lexem(context->get_ref_lexem());
						  enum_value = (T_ident *)enum_value->get_next();
						}
					}
				}

			  TRACE9("On a ajoute id %x:%s pere %x a mach %x:%s (%x,%x)->(%x,%x)",
					 new_ident,
					 new_ident->get_name()->get_value(),
					 new_ident->get_father(),
					 this,
					 machine_name->get_name()->get_value(),
					 adr_first,
					 adr_last,
					 *adr_first,
					 *adr_last) ;
			}


		  // Mise à jour du lien sur l'homonyme de l'abstraction
		  new_ident->set_homonym_in_abstraction(cur);
		  if (new_ident->get_ident_type() == ITYPE_ENUMERATED_SET)

				{
				  // on est assuré que cur!= NULL mais on n'est pas
				  // sur que cur et new_ident ont le meme nombre
				  // d'éléments. On test l'arret de la boucle sur les
				  // deux listes.
				  T_ident *new_enum_value = (T_ident *)new_ident->get_values() ;
				  T_ident *cur_enum_value = (T_ident *)cur->get_values() ;
				  while (new_enum_value != NULL && cur_enum_value != NULL)
					{
					  new_enum_value->set_homonym_in_abstraction(cur_enum_value) ;
					  new_enum_value = (T_ident *)new_enum_value->get_next() ;
					  cur_enum_value = (T_ident *)cur_enum_value->get_next() ;
					}
				}
		  // Mise à jour du lien sur l'homonyme d'une machine requise
		  new_ident->set_homonym_in_required_mach(res);
		  // Mise a jour du def
		  new_ident->set_def(res) ;

		  // Mise a jour du  "glued"
		  new_ident->set_ref_glued(res) ;

		  if(machine_type == MTYPE_IMPLEMENTATION)
			{
			  new_ident->set_implemented_by(res);
			}

		  if (new_ident->get_ident_type() == ITYPE_ENUMERATED_SET)
			{
			  T_ident *new_enum_value = (T_ident *)new_ident->get_values() ;
			  // par construction res != NULL
			  T_ident *res_enum_value = (T_ident *)res->get_values() ;
			  while (new_enum_value != NULL && res_enum_value != NULL)
				{
				  new_enum_value->set_homonym_in_required_mach(res_enum_value) ;
				  new_enum_value->set_def(res_enum_value) ;
				  new_enum_value->set_ref_glued(res_enum_value) ;

				  if(machine_type == MTYPE_IMPLEMENTATION)
					{
					  new_enum_value->set_implemented_by(res_enum_value);
					}
				  new_enum_value = (T_ident *)new_enum_value->get_next() ;
				  res_enum_value = (T_ident *)res_enum_value->get_next() ;
				}
			}
		  TRACE5("%x:%s->ref_glued() = %x(machine %s) (father %x)",
				 new_ident,
				 new_ident->get_name()->get_value(),
				 new_ident->get_ref_glued(),
				 new_ident->get_ref_glued()->get_ref_machine()
				 	->get_machine_name()->get_name()->get_value(),
				 new_ident->get_father()) ;

		  TRACE3("%x:%s->def = %x",
				 new_ident,
				 new_ident->get_name()->get_value(),
				 new_ident->get_def()) ;

#if 0
		  // NON !
		  // Il faut accrocher ce nouvel identificateur a la machine qui
		  // implemente pour que les verifications de visibilite
		  // fonctionnent
		  new_ident->set_ref_machine(this) ;
#endif
		}

	  EXIT_TRACE ; EXIT_TRACE ;

	  cur = (T_ident *)cur->get_next() ;
	}

  EXIT_TRACE ;
}

// Recherche des donnees collees
void T_machine::lookup_glued_data(void)
{
  TRACE1("DEBUT DETECTION DES COLLAGES POUR LA MACHINE %s",
		 machine_name->get_name()->get_value()) ;
  ENTER_TRACE ;
  ENTER_TRACE ;

  ASSERT(ref_abstraction != NULL) ; // car on sait qu'on n'est pas une spec

  // On recherche, parmi les constantes abstraites de l'abstraction,
  // celles qui sont homonymes a une constante d'une machine importee,
  // incluse ou etendue
  TRACE0("Detection des constantes collees") ;
  TRACE0("etude des constantes abstraites") ;
  internal_lookup_glued_data(ref_abstraction->get_abstract_constants(),
							 (T_item **)&first_concrete_constant,
							 (T_item **)&last_concrete_constant,
							 (T_item **)&first_abstract_constant,
							 (T_item **)&last_abstract_constant) ;
  TRACE0("etude des constantes concretes") ;
  internal_lookup_glued_data(ref_abstraction->get_concrete_constants(),
							 (T_item **)&first_concrete_constant,
							 (T_item **)&last_concrete_constant,
							 (T_item **)&first_abstract_constant,
							 (T_item **)&last_abstract_constant) ;

  // On recherche, parmi les variables abstraites de l'abstraction,
  // celles qui sont homonymes a une variable d'une machine importee,
  // incluse ou etendue
  TRACE0("Detection des variables collees") ;
  TRACE0("etude des variables abstraits") ;
  internal_lookup_glued_data(ref_abstraction->get_abstract_variables(),
							 (T_item **)&first_concrete_variable,
							 (T_item **)&last_concrete_variable,
							 (T_item **)&first_abstract_variable,
							 (T_item **)&last_abstract_variable) ;

  TRACE0("etude des variables concretes") ;
  internal_lookup_glued_data(ref_abstraction->get_concrete_variables(),
							 (T_item **)&first_concrete_variable,
							 (T_item **)&last_concrete_variable,
							 (T_item **)&first_abstract_variable,
							 (T_item **)&last_abstract_variable) ;

  // On recherche, parmi les ensembles de l'abstraction,
  // celles qui sont homonymes a un ensemble d'une machine importee,
  // incluse ou etendue
  TRACE2("DETECTION DES ENSEMBLES COLLES (liste des sets de l'abs %s : %x)",
		 ref_abstraction->get_machine_name()->get_name()->get_value(),
		 ref_abstraction->get_sets()) ;
  ENTER_TRACE ; ENTER_TRACE ;
  internal_lookup_glued_data(ref_abstraction->get_sets(),
							 (T_item **)&first_set,
							 (T_item **)&last_set,
							 NULL,
							 NULL) ;
  EXIT_TRACE ; EXIT_TRACE ;

  EXIT_TRACE ;
  EXIT_TRACE ;
  TRACE1("FIN DETECTION DES COLLAGES POUR LA MACHINE %s",
		 machine_name->get_name()->get_value()) ;
}

// Fonction auxiliaire de recherche d'un identificateur dans une liste
T_ident *T_machine::browse_list(T_ident *ref_ident,
										 T_machine **adr_ref_machine,
										 T_used_machine *cur)
{
  TRACE2("T_machine(%s)::browse_list(ref_ident = %s)",
		 machine_name->get_name()->get_value(),
		 ref_ident->get_name()->get_value()) ;

  //GP 19/11/98
  //Correction: collage des variables renommées

  // Si l'identifiant n'est pas renommé
  // on recherche parmi toutes les machines
  if (ref_ident->get_node_type() == NODE_IDENT)
	{
	  while (cur != NULL)
		{
		  T_machine *mach = cur->get_ref_machine() ;

#ifndef NO_CHECKS
		  if (mach == NULL)
			{
			  fprintf(stderr,
                                          "%s:%d:%d: PANIC machine %p:%s, ref_machine = NULL\n",
					  cur->get_ref_lexem()->get_file_name()->get_value(),
					  cur->get_ref_lexem()->get_file_line(),
					  cur->get_ref_lexem()->get_file_column(),
                                          cur,
					  cur->get_name()->get_name()->get_value()) ;
			  TRACE0("Panic ! REF_MACHINE = NULL") ;
			}
		  else
			{
#endif // NO_CHECKS

			  TRACE1("cur_mach %s", mach->get_machine_name()->get_name()->get_value()) ;
			  ASSERT(mach != NULL) ;
			  T_ident *def = mach->defines(ref_ident) ;

			  if (def != NULL)
				{
				  switch(def->get_ident_type())
					{
					case ITYPE_CONCRETE_VARIABLE:
					case ITYPE_ABSTRACT_VARIABLE:
					case ITYPE_OP_NAME:
					case ITYPE_LOCAL_OP_NAME :
					case ITYPE_USED_OP_NAME:
					  {
						//GP 19/01/99
						// CTRL Ref : COLL 6-1
						//Seuls les constantes
						//et les ensembles peuvent etre collés
						//par homonymie a travers un lien sees
						if (cur->get_use_type()!=USE_SEES)
						  {
							//Conformement a EREN1 les variables, ... doivent
							//avoir le meme prefixe de renommage que la machine
							if (cur->get_instance_name() == NULL)
							  {
								*adr_ref_machine = mach ;
								return def ;
							  }
						  }
						break ;
					  }
					case ITYPE_UNKNOWN:

					case ITYPE_BUILTIN:
					case ITYPE_MACHINE_NAME:
					case ITYPE_ABSTRACTION_NAME:
					case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
					case ITYPE_MACHINE_SET_FORMAL_PARAM:
					case ITYPE_USED_MACHINE_NAME:
					case ITYPE_ENUMERATED_VALUE:
					case ITYPE_DEFINITION_PARAM:
					case ITYPE_OP_IN_PARAM:
					case ITYPE_OP_OUT_PARAM:
					case ITYPE_LOCAL_VARIABLE:
					case ITYPE_LOCAL_QUANTIFIER:
                    case ITYPE_ANY_QUANTIFIER:
					case ITYPE_RECORD_LABEL:
					case ITYPE_PRAGMA_PARAMETER:
					  {
						//GP 19/01/99
						// COLL 6-1 (suite)
						//Seuls les constantes
						//et les ensembles peuvent etre colles
						//par homnymie a travers un lien sees
						if (cur->get_use_type()!=USE_SEES)
						  {
							*adr_ref_machine = mach ;
							return def ;
						  }
						break ;
					  }
					case ITYPE_CONCRETE_CONSTANT:
					case ITYPE_ABSTRACT_CONSTANT:
					case ITYPE_ENUMERATED_SET:
					case ITYPE_ABSTRACT_SET:
					  {
						*adr_ref_machine = mach ;
						return def ;
						break ;
					  }
					}
				}
#ifndef NO_CHECKS
			}
#endif // NO_CHECKS
		  cur = (T_used_machine *)cur->get_next() ;
		}

	  //Aucune machine satisfaisant n'est trouvee
	  //pour definir un identifiant non renommé
	  return NULL ;
	}

  // GP: Si l'ident est renommé
  else if(ref_ident->get_node_type() == NODE_RENAMED_IDENT)

	{

  	  //Cast vrai grace au test ci-dessus
	  T_renamed_ident * ref_ident_rename = (T_renamed_ident*)ref_ident ;

	  while (cur != NULL)
		{
		  T_machine *mach = cur->get_ref_machine() ;

#ifndef NO_CHECKS
		  if (mach == NULL)
			{
			  fprintf(stderr,
                                          "%s:%d:%d: PANIC machine %p:%s, ref_machine = NULL\n",
					  cur->get_ref_lexem()->get_file_name()->get_value(),
					  cur->get_ref_lexem()->get_file_line(),
					  cur->get_ref_lexem()->get_file_column(),
                                          cur,
					  cur->get_name()->get_name()->get_value()) ;
			  TRACE0("Panic ! REF_MACHINE = NULL") ;
			}
		  else
			{
#endif // NO_CHECKS

			  TRACE1("cur_mach %s",
					 mach->get_machine_name()->get_name()->get_value()) ;
			  ASSERT(mach != NULL) ;

			  //GP Appel de defines(T_renamed_ident*)
			  //different de defines(T_ident*) ci dessus
			  T_ident *def = mach->defines(ref_ident_rename) ;

			  if (def != NULL)
				{
				  switch(def->get_ident_type())
					{
					case ITYPE_CONCRETE_CONSTANT:
					case ITYPE_ABSTRACT_CONSTANT:
					case ITYPE_ENUMERATED_SET:
					case ITYPE_ABSTRACT_SET:
					  {
						*adr_ref_machine = mach ;
						TRACE5("ref_ident_rename %s defini par %x(%s) dans %s apporte par %",
							   ref_ident_rename->get_name()->get_value(),
							   def,
							   def->get_name()->get_value(),
							   mach->get_machine_name()->get_name()->get_value(),
							   cur->get_name()->get_name()->get_value()) ;

						return def ;
						break ;
					  }
					default:
					  {
						//GP 19/01/99
						//COLL 6-1 (suite)
						//Seuls les constantes
						//et les ensembles peuvent etre colles
						//par homnymie a travers un lien sees
						if(cur->get_use_type() != USE_SEES)
						  {
							*adr_ref_machine = mach ;
							TRACE5("ref_ident_rename %s defini par %x(%s) dans %s apporte par %",
								   ref_ident_rename->get_name()->get_value(),
								   def,
								   def->get_name()->get_value(),
								   mach->get_machine_name()->get_name()->get_value(),
								   cur->get_name()->get_name()->get_value()) ;

							return def ;
							break ;
						  }
					  }
					}
				}

#ifndef NO_CHECKS
			}
#endif // NO_CHECKS

		  cur = (T_used_machine *)cur->get_next() ;
		} //while (cur != NULL)

	  TRACE1("ref_ident_rename %s, non defini ici",
			 ref_ident_rename->get_name()->get_value()) ;
	  //Aucune machine satisfaisant n'est trouvee
	  //pour definir un identifiant renommé
	  return NULL ;
	}

  //ref_ident* doit etre un pointeur sur un T_ident
  //ou un T_renamed_ident. donc on ne doit jamais arriver ici.
  assert(FALSE) ;
  return NULL ; //Pour les warning
}


// Recherche d'un identificateur dans les machines
// importees, includes, etendues et eventuellement vues
T_ident *
T_machine::lookup_ident_in_needed_components(T_ident *ref_ident,
											 T_machine **adr_ref_machine)
{
  TRACE3("T_machine(%x:%s)::lookup_ident_in_needed_components(%s)",
		 this,
		 machine_name->get_name()->get_value(),
		 ref_ident->get_name()->get_value()) ;


  T_ident *res = browse_list(ref_ident, adr_ref_machine, first_sees) ;
  if (res != NULL)
	{
	  return res ;
	}

  res = browse_list(ref_ident, adr_ref_machine, first_imports) ;
  if (res != NULL)
	{
	  return res ;
	}

  res = browse_list(ref_ident, adr_ref_machine, first_includes) ;
  if (res != NULL)
	{
	  return res ;
	}

  res = browse_list(ref_ident, adr_ref_machine, first_extends) ;
  if (res != NULL)
	{
	  return res ;
	}

  res = browse_list(ref_ident, adr_ref_machine, first_uses) ;
  if (res != NULL)
	{
	  return res ;
	}

  *adr_ref_machine = NULL ;
  return NULL ;
}
