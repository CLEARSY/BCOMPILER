/******************************* CLEARSY **************************************
* Fichier : $Id: v_hcolli.cpp,v 2.0 2000-11-09 17:53:02 lv Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique : "conflits caches"
*
* Compilation : 	-DDEBUG_COLLI pour tracer les tests
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
RCS_ID("$Id: v_hcolli.cpp,v 1.7 2000-11-09 17:53:02 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_tstamp.h"

static int are_in_same_instance(T_ident *cur, T_ident *clash)
{

  TRACE4("are_in_same_instance(%x:%s, %x:%s)",
		 cur, cur->get_name()->get_value(),
		 clash, clash->get_name()->get_value());


  // S'agit-il de deux identificateurs definis
  // dans la meme instance de machine ?
  if ((cur->get_ref_machine()->get_context() != NULL) &&
	  (clash->get_ref_machine()->get_context() != NULL))
	{
	  if (cur->get_ref_machine()->get_context()->get_name()->get_name() ==
		  clash->get_ref_machine()->get_context()->get_name()->get_name())
		{
		  TRACE0("Ils viennent de la meme instance de machine");
		  TRACE0("=> pas de conflit");
		  return TRUE;
		}
	}
  return FALSE;
}

// Analyse des conflits caches
T_betree *proof_clash_analysis(T_betree *betree)
{
  TRACE1("DEBUT proof_clash_analysis(%x)", betree);
  ENTER_TRACE; ENTER_TRACE;

  // Parcours de la liste de tous les identificateurs
  T_ident *ident_list = get_object_manager()->get_identifiers();
  T_ident *cur = ident_list;
  // Nom de l'identificateur en cours d'analyse
  T_symbol *symb_with_dot = NULL;
  T_symbol *symb_without_dot = NULL;
  // Idenitificateur en conflit
  T_ident *clash_ident = NULL;

  // On initialise le champ usr1 des symbols à NULL
  // Ces champs serviront a accrocher l'identificateur.
  // Si deux identificateurs doivent s'accrocher au meme symbole,
  // c'est qu'ils sont en conflit. Il ne reste plus qu'à vérifier
  // dans quel "cas" de la spec ils sont.
  while (cur != NULL)
	{
	  // Nom de l'identificateur sans les points de renommage
 	  cur->get_full_name(TRUE)->set_usr1(NULL);
	  cur = cur->get_next_ident();
	}

  // Controle des conflits
  cur = ident_list;
  while (cur != NULL)
	{
	  // Il ne s'agit d'une définition, donc on le traite
	  if (cur->get_def() == NULL)
		{

		  // Nom de l'identificateur AVEC les points de renommage
		  symb_with_dot = cur->get_full_name(FALSE);

		  // Nom de l'identificateur SANS les points de renommage
		  symb_without_dot = cur->get_full_name(TRUE);

		  clash_ident = (T_ident *)symb_without_dot->get_usr1();

		  TRACE2("Conflit sur %s de %s ?\n",
				 symb_with_dot->get_value(),
				 ((cur->get_ref_machine() == NULL) ?
				  "null" :
				  cur->get_ref_machine()->get_machine_name()->get_name()
				  ->get_value()));

		  switch(cur->get_ident_type())
			{
			case ITYPE_CONCRETE_CONSTANT :
			case ITYPE_ABSTRACT_CONSTANT :
			case ITYPE_ABSTRACT_SET :
			case ITYPE_ENUMERATED_SET :
			case ITYPE_ENUMERATED_VALUE :
			  {
				TRACE0("Cas d'une constante ou d'un ensemble");

				// C'est la premiere fois qu'on rencontre cet identificateur
				if (clash_ident == NULL)
				  {
					TRACE0("On le rencontre pour la premiere fois");
					symb_without_dot->set_usr1(cur);
					break;
				  }

				// Il y a deja un identificateur accroche au symbol
				// Donc il y a un conflit potentiel

				TRACE2("Conflit avec %x de %s ?\n",
					   clash_ident,
					   clash_ident->get_ref_machine()->get_machine_name()->
					   get_name()->get_value());

				// Avec les points il y n'y aurait pas eu de conflit,
				// donc il y a bien un conflits cache
				if (symb_with_dot != clash_ident->get_full_name(FALSE))
				  {
					TRACE2("clash entre first=%x et second=%x",
						   clash_ident, cur);
					semantic_error(clash_ident,
								   CAN_CONTINUE,
								   get_error_msg(E_KERNEL_HIDDEN_CLASH),
								   clash_ident->get_full_name(FALSE)
								   ->get_value(),
								   symb_with_dot->get_value(),
								   cur->get_ref_machine()->get_machine_name()
								   ->get_name()->get_value());
					semantic_error(cur,
								   MULTI_LINE,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					break;
				  }

				// S'agit-il de deux constantes/ensembles definis
				// dans la meme machine ?
				if (cur->get_ident_type() == clash_ident->get_ident_type()
					&&
					cur->get_ref_machine()->get_machine_name()->get_name() ==
					clash_ident->get_ref_machine()->get_machine_name()
					->get_name())
				  {
					TRACE0("Deux constantes/ensembles définis dans la meme machine => pas de conflit");
					break;
				  }

				// S'agit-il de deux constantes/ensembles homonymes ?
				if (cur->is_in_glued_ring(clash_ident) == TRUE)
				  {
					TRACE0("Deux constantes/ensembles homonymes => pas de conflit");
					break;
				  }

				// Dans tous les autres cas il y a conflit
				TRACE2("clash entre first=%x et second=%x",
					   clash_ident, cur);
				semantic_error(clash_ident,
							   CAN_CONTINUE,
							   get_error_msg(E_KERNEL_HIDDEN_CLASH),
							   clash_ident->get_full_name(FALSE)->get_value(),
							   symb_with_dot->get_value(),
							   cur->get_ref_machine()->get_machine_name()
							   ->get_name()->get_value());
				semantic_error(cur,
							   MULTI_LINE,
							   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
				break;
			  }
			case ITYPE_CONCRETE_VARIABLE :
			case ITYPE_ABSTRACT_VARIABLE :
			  {
				TRACE0("Cas d'une variable");

				// C'est la premiere fois qu'on rencontre cet identificateur
				if (clash_ident == NULL)
				  {
					TRACE0("On le rencontre pour la premiere fois");
					symb_without_dot->set_usr1(cur);
					break;
				  }

				// Il y a deja un identificateur accroche au symbol
				// Donc il y a un conflit potentiel
				TRACE3("Conflit avec %x:%s de %s ?\n",
					   clash_ident,
					   clash_ident->get_full_name(FALSE),
					   clash_ident->get_ref_machine()->get_machine_name()->
					   get_name()->get_value());

				// Avec les points il y n'y aurait pas eu de conflit,
				// donc il y a bien un conflits cache
				if (symb_with_dot != clash_ident->get_full_name(FALSE))
				  {
					TRACE2("clash entre first=%x et second=%x",
						   clash_ident, cur);
					semantic_error(clash_ident,
								   CAN_CONTINUE,
								   get_error_msg(E_KERNEL_HIDDEN_CLASH),
								   clash_ident->get_full_name(FALSE)
								   ->get_value(),
								   symb_with_dot->get_value(),
								   cur->get_ref_machine()->get_machine_name()
								   ->get_name()->get_value());
					semantic_error(cur,
								   MULTI_LINE,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					break;
				  }

				if ((clash_ident->get_ident_type() == ITYPE_CONCRETE_VARIABLE)
					||
					(clash_ident->get_ident_type() == ITYPE_ABSTRACT_VARIABLE))
				  {

					// S'agit-il de deux variables definies
					// dans la meme instance de machine ?
					if (are_in_same_instance(cur, clash_ident) == TRUE)
					  {
						TRACE0("meme instance de machine => pas de conflit");
						break;
					  }
				  }

				// Sont elles homonymes ?
				if (cur->is_in_glued_ring(clash_ident) == TRUE)
				  {
					TRACE0("homonymes => pas de conflit");
					break;
				  }

				// Dans tous les autres cas il y a conflit
				TRACE2("clash entre first=%x et second=%x", clash_ident, cur);
				semantic_error(clash_ident,
							   CAN_CONTINUE,
							   get_error_msg(E_KERNEL_HIDDEN_CLASH),
							   clash_ident->get_full_name(FALSE)->get_value(),
							   symb_with_dot->get_value(),
							   cur->get_ref_machine()->get_machine_name()
							   ->get_name()->get_value());
				semantic_error(cur,
							   MULTI_LINE,
							   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
				break;
			  }
			case ITYPE_MACHINE_SCALAR_FORMAL_PARAM :
			case ITYPE_MACHINE_SET_FORMAL_PARAM :
			  {
				TRACE0("Cas d'un parametre formel de machine");

				// Cas des parametres formels du composant analyse
				if (cur->get_ref_machine() ==
					 betree->get_root()->get_specification())
				  {
					TRACE0("Cas d'un parametre du composant analyse");
					if (clash_ident == NULL)
					  {
						TRACE0("On le rencontre pour la premiere fois");
						symb_without_dot->set_usr1(cur);
						break;
					  }

					// Il y a deja un identificateur d'accroché
					// => il y a forcement un conflit
					TRACE2("clash entre first=%x et second=%x",
						   clash_ident, cur);
					semantic_error(clash_ident,
								   CAN_CONTINUE,
								   get_error_msg(E_KERNEL_HIDDEN_CLASH),
								   clash_ident->get_full_name(FALSE)
								   ->get_value(),
								   symb_with_dot->get_value(),
								   cur->get_ref_machine()->get_machine_name()
								   ->get_name()->get_value());
					semantic_error(cur,
								   MULTI_LINE,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					break;
				  }

				// Cas des param formels d'une machine vue ou utilisée
				if ((cur->get_betree()->get_seen_by()->get_links() != NULL) ||
					(cur->get_betree()->get_used_by()->get_links() != NULL))
				  {
					TRACE0("Cas d'un parametre d'une machine vue");

					if (clash_ident == NULL)
					  {
						TRACE0("On le rencontre pour la premiere fois");
						symb_without_dot->set_usr1(cur);
						break;
					  }

					// Il y a deja un identificateur accroche au symbol
					// Donc il y a un conflit potentiel
					TRACE2("Conflit avec %x de %s ?\n",
						   clash_ident,
						   clash_ident->get_ref_machine()->get_machine_name()->
						   get_name()->get_value());

					// Avec les points il y n'y aurait pas eu de conflit,
					// donc il y a bien un conflits cache
					if (symb_with_dot != clash_ident->get_full_name(FALSE))
					  {
						TRACE2("clash entre first=%x et second=%x",
							   clash_ident, cur);
						semantic_error(clash_ident,
									   CAN_CONTINUE,
									   get_error_msg(E_KERNEL_HIDDEN_CLASH),
									   clash_ident->get_full_name(FALSE)
									   ->get_value(),
									   symb_with_dot->get_value(),
									   cur->get_ref_machine()
									   ->get_machine_name()
									   ->get_name()->get_value());
						semantic_error(cur,
									   MULTI_LINE,
									   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
						break;
					  }

					// Il s'agit de deux parametres formels de machine
					// definis dans la meme instance de machine
					if ((clash_ident->get_ident_type() == cur->get_ident_type())
						&&
						(are_in_same_instance(cur, clash_ident) == TRUE))
					  {
						TRACE0("meme instance de machine => pas de conflit");
						break;
					  }

					// Dans tous les autres cas, il y a bien conflit
					TRACE2("clash entre first=%x et second=%x",
						   clash_ident, cur);
					semantic_error(clash_ident,
								   CAN_CONTINUE,
								   get_error_msg(E_KERNEL_HIDDEN_CLASH),
								   clash_ident->get_full_name(FALSE)
								   ->get_value(),
								   symb_with_dot->get_value(),
								   cur->get_ref_machine()->get_machine_name()
								   ->get_name()->get_value());
					semantic_error(cur,
								   MULTI_LINE,
								   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					break;
				  }

				// Dans les autres cas :
				// Il s'agit des parametres formels d'une machine incluse ou
				// ou importee, ou bien d'une abstraction de la machine analysee
				// Donc il n'y a pas de conflits à detecter
				break;
			  }
			case ITYPE_MACHINE_NAME :
			case ITYPE_ABSTRACTION_NAME :
			  {
				TRACE0("Cas d'un nom de machine");

				if (cur->get_betree() == betree)
				  {
					// Il s'agit du nom du composant analyse
					// ou de son abstraction
					// Donc il y a des conflits a verifier

					TRACE0("Nom de l'abstraction ou du composant !");
					if (clash_ident != NULL)
					  {
						TRACE2("clash entre first=%x et second=%x",
							   clash_ident, cur);
						semantic_error(clash_ident,
									   CAN_CONTINUE,
									   get_error_msg(E_KERNEL_HIDDEN_CLASH),
									   clash_ident->get_full_name(FALSE)
									   ->get_value(),
									   symb_with_dot->get_value(),
									   cur->get_ref_machine()
									   ->get_machine_name()
									   ->get_name()->get_value());
						semantic_error(cur,
									   MULTI_LINE,
									   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					  }
					else
					  {
						TRACE0("On le rencontre pour la premiere fois");
						symb_without_dot->set_usr1(cur);
					  }
				  }
				break;
			  }
			case ITYPE_OP_NAME :
			case ITYPE_USED_OP_NAME :
			  {
				TRACE1("Cas d'une opération : %s",
					   cur->get_name()->get_value());

				if ((cur->get_ref_machine() ==
					 betree->get_root()->get_specification())
					// Operation de la spec du composant analyse
					||
					((get_extended_sees() == TRUE) &&
					 (betree->get_root()->get_sees(cur->get_betree()
												   ->get_root())
					  == TRUE)
					 // SEES etendu : operations des machines vues
					 ))
				  {
					// Il y a des conflits a verifier

					if (clash_ident != NULL)
					  {
						TRACE2("clash entre first=%x et second=%x",
							   clash_ident, cur);
						semantic_error(clash_ident,
									   CAN_CONTINUE,
									   get_error_msg(E_KERNEL_HIDDEN_CLASH),
									   clash_ident->get_full_name(FALSE)
									   ->get_value(),
									   symb_with_dot->get_value(),
									   cur->get_ref_machine()
									   ->get_machine_name()
									   ->get_name()->get_value());
						semantic_error(cur,
									   MULTI_LINE,
									   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
					  }
					else
					  {
						TRACE0("On le rencontre pour la premiere fois");
						symb_without_dot->set_usr1(cur);
					  }
				  }
				break;
			  }
			case ITYPE_OP_IN_PARAM :
			case ITYPE_OP_OUT_PARAM :
			  {
				TRACE1("Cas d'un parametre d'opération : %s",
					   cur->get_name()->get_value());

				// On recupere le nom de l'operation ou est defini ce parametre
				// get_full_name permet de renommer ce nom en fonction du
				// renommage eventuel de la machine qui le defini
				T_symbol *op_name = cur->get_ref_op()->get_name()
				  ->get_full_name(FALSE);

				if ((betree->get_root()->get_specification()->find_op(op_name)
					 != NULL)
					// Parametres d'une operation de la spec
					// du composant analyse
					||
					((get_extended_sees() == TRUE) &&
					 (betree->get_root()->get_sees(cur->get_betree()->get_root())
					  == TRUE)
					 // SEES etendu :
					 // parametres d'une operation d'une machine vue
					 ))
				  {
					// Il y a des conflits a verifier

					if (clash_ident != NULL)
					  {
						if ((symb_with_dot != clash_ident->get_full_name(FALSE))
							// Avec les points il y n'y aurait pas eu de
							// conflit, donc il y a bien un conflits cache
							||
							((clash_ident->get_ident_type() !=
							  ITYPE_OP_IN_PARAM)
							 &&
							 (clash_ident->get_ident_type() !=
							  ITYPE_OP_OUT_PARAM))
							// Le parametre est en conflit avec un autre ident
							// qui n'est pas un parametre
							)
						  {
							TRACE2("clash entre first=%x et second=%x",
								   clash_ident, cur);
							semantic_error(clash_ident,
										   CAN_CONTINUE,
										   get_error_msg(E_KERNEL_HIDDEN_CLASH),
										   clash_ident->get_full_name(FALSE)
										   ->get_value(),
										   symb_with_dot->get_value(),
										   cur->get_ref_machine()
										   ->get_machine_name()
										   ->get_name()->get_value());
							semantic_error(cur,
										   MULTI_LINE,
										   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION));
							break;
						  }
						// else
						// Le parametre est en conflit avec un autre parametre
						// Les conflits "normaux" ont dejà émis un message
						// d'erreur si ces deux parametres sont definis dans
						// la meme operation,
						// sinon ils sont definis dans deux operations
						// differentes, il n'y a donc pas de conflit
					  }
					else
					  {
						TRACE0("On le rencontre pour la premiere fois");
						symb_without_dot->set_usr1(cur);
					  }
				  }
				break;
			  }
			default :
			  {
				break;
			  }
			}
		}
	  // sinon il ne s'agit pas d'une definition
	  // donc il n'y a pas de conflit a verifier

	  // passage a l'identificateur suivant
	  cur = cur->get_next_ident();
	}

  // Reinitialiser les usr1
  cur = ident_list;
  while (cur != NULL)
	{
	  // Nom de l'identificateur sans les points de renommage
 	  cur->get_full_name(TRUE)->set_usr1(NULL);
	  cur = cur->get_next_ident();
	}


  EXIT_TRACE ; EXIT_TRACE ;
  TRACE0(" FIN  proof_clash_analysis()") ;
  return betree;
}

//
// Fin du fichier
//
