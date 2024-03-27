/***************************** CLEARSY **************************************
* Fichier : $Id: c_cmptyp.cpp,v 2.0 2004-01-15 09:35:03 cm Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_TYPE pour les traces generales
*
* Description :	Fonctions de comparaisons de type (is_compatible_with)
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
RCS_ID("$Id: c_cmptyp.cpp,v 1.26 2004-01-15 09:35:03 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Fonctions de compatibilite
int T_product_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
#ifdef DEBUG_TYPE
  TRACE5("T_product_type(%x:%s)::is_compatible_with(%x:%s, %s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 (ref_type == NULL) ? "null" : ref_type->get_class_name()) ;
#endif

  if (ref_type == NULL || type1 == NULL || type2 == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}

#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif

  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

  // Il faut que ref_type soit un type produit ..
  if (ref_type->get_node_type() != NODE_PRODUCT_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type pas un product type") ;
#endif

	  return FALSE ;
	}

  T_product_type *ref_prod = (T_product_type *)ref_type ;

  // Il faut que les deux type1 et que les deux type2 soient compatibles
  if (type1->is_compatible_with(ref_prod->get_type1()) == FALSE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("type1 pas compatible") ;
#endif
	  return FALSE ;
	}

  if (type2->is_compatible_with(ref_prod->get_type2()) == FALSE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("type2 pas compatible") ;
#endif
	  return FALSE ;
	}

  return TRUE ;
}

// Fonctions de compatibilite
int T_setof_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
#ifdef DEBUG_TYPE
  TRACE3("T_setof_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 (ref_type == NULL) ? "NULL" : ref_type->get_class_name()) ;
#endif
  if (ref_type == NULL || base_type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}


#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 (ref_type == NULL) ? "NULL" :ref_type->make_type_name()->get_value()) ;
#endif

  // Il faut que ref_type soit un ensemble ...
  if (ref_type->get_node_type() == NODE_SETOF_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("on regarde les base_type") ;
#endif

	  return base_type->is_compatible_with(((T_setof_type*)ref_type)->base_type) ;
	}

  // ... Sinon il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

  // Si on arrive ici c'est que les types sont incompatibles
#ifdef DEBUG_TYPE
  TRACE0("types incompatibles") ;
#endif

  return FALSE ;
}

// Fonctions de compatibilite
int T_abstract_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
  if (ref_type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_abstract_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif


  if (after_valuation_type != NULL)
	{
#ifdef DEBUG_TYPE
	  TRACE1("on demande au type apres valuation %x", after_valuation_type) ;
#endif

	  return after_valuation_type->is_compatible_with(ref_type) ;
	}

  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

  // Il faut que ref_type soit un type abstrait ...
  if (ref_type->get_node_type() != NODE_ABSTRACT_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref pas abstract") ;
#endif

	  return FALSE ;
	}

  // et que les identificateurs soient les memes
  // on utilise T_symbol::get_value pour cela !
  T_abstract_type *aref_type = (T_abstract_type *)ref_type ;

  if (aref_type->after_valuation_type != NULL)
	{
#ifdef DEBUG_TYPE
	  TRACE1("on demande au type ref_type apres valuation %x",
			 aref_type->after_valuation_type) ;
#endif

	  return is_compatible_with(aref_type->after_valuation_type) ;
	}



#ifdef DEBUG_TYPE
  TRACE2("compare les noms %s et %s",
		 set->get_name()->get_value(),
		 aref_type->set->get_name()->get_value()) ;
#endif

  return set->get_name()->compare(aref_type->set->get_name()) ;

  // A FAIRE : et les bornes ?
}

//
// Fonction de compatibilite
//
int T_enumerated_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
  if (ref_type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}

  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_enumerated_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif

  // Il faut que ref_type soit un type enumere ...
  if (ref_type->get_node_type() != NODE_ENUMERATED_TYPE)
	{
	  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
		{
#ifdef DEBUG_TYPE
	  TRACE0("ref pas enum mais generic : ok") ;
#endif
		  return TRUE ;
		}
	  // SL 26/10/1998
	  // STM 2017 : ref_type = abstrait value avec un enumere
	  // il faut regarder la valuation
	  // le plus simple est d'appeler la methode de T_abstract_type
	  else if (ref_type->get_node_type() == NODE_ABSTRACT_TYPE)
		{
#ifdef DEBUG_TYPE
		  TRACE0("ref pas enum mais abstrait : appel recursif") ;
#endif
		  return ref_type->is_compatible_with(this) ;
		}

	  return FALSE ;
	}

  // ... avec le meme type_definition
  T_symbol *cur_set = type_definition->get_name() ;
  T_symbol *ref_set = ((T_enumerated_type *)ref_type)->type_definition->get_name() ;
  return cur_set->compare(ref_set) ;
}

// Compatibilite
int T_predefined_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
  if (ref_type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}

  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_predefined_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 (ref_type == NULL) ? "NULL " : ref_type->get_class_name()) ;
#endif

  if (ref_type == NULL)
	{
#ifdef DEBUG_TYPE
	  TRACE0("PANIC, ref_type == NULL") ;
#endif

	  return FALSE ;
	}

#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif

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

  // Cas particulier d'un type generique
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  TRACE0("on demande au type generique") ;
	  return ref_type->is_compatible_with(this) ;
	}

  // Il faut que ref_type soit un type de base ...
  if (ref_type->get_node_type() != NODE_PREDEFINED_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE1("ref_type %s : non", ref_type->get_class_name()) ;
#endif


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
#ifdef DEBUG_TYPE
	  TRACE0("type != ref_type -> non") ;
#endif
	  return FALSE ;
	}

#if 0
  A FAIRE : etudier
  // ... et des bornes compatibles i.e. a droite des bornes plus selectives
#ifdef FULL_TRACE
  TRACE0("etude borne inf") ;
#endif
  if ( 	(get_lower_bound() != NULL)
		&& (get_lower_bound()->is_less_than(ref_type->get_lower_bound()) == FALSE) )
	{
	  TRACE0("Intervalle de droite non compatible (borne inf)") ;
	  return FALSE ;
	}

#ifdef FULL_TRACE
  TRACE0("etude borne sup") ;
#endif
  if ( 	(get_upper_bound() != NULL)
		&& (get_upper_bound()->is_greater_than(ref_type->get_upper_bound()) == FALSE))
	{
	  TRACE0("Intervalle de droite non compatible (borne sup)") ;
	  return FALSE ;
	}
#endif

  // C'est bon !
#ifdef DEBUG_TYPE
  TRACE0("c'est bon !") ;
#endif
  return TRUE ;
}

// Fonctions de compatibilite
int T_generic_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
#ifdef DEBUG_TYPE
  TRACE3("T_generic_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 make_class_name(ref_type)) ;
#endif

  if (ref_type == NULL)
	{
	  if (get_error_count() > 1)
		{
		  // Reprise sur erreur
		  return TRUE ;
		}
	  else
		{
		  // Un type generique n'est pas typant
		  return FALSE ;
		}
	}

#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif

  if (type == NULL)
	{
	  // Type generique non encore instancie
	  if (ref_type->has_wildcards() == TRUE)
		{
          TRACE0("deux types * -> OK") ;
          return TRUE ;
		}
	  else
		{
		  // C'est bon !
		  TRACE0("ok, avec instanciation") ;
		  type = ref_type->clone_type(this, get_betree(), get_ref_lexem()) ;
		  TRACE2("INST this %x type %x", this, type) ;
		  return TRUE ;
		}

	}
  else
	{
	  // Type deja instancie
	  TRACE2("type %s ref_type %s : appel recursif",
			 ::make_type_name(type)->get_value(),
			 ::make_type_name(ref_type)->get_value());

	  return type->is_compatible_with(ref_type, low_ctrl) ;
	}
}

//GP Corrction BUG 2146:
// low_ctrl = TRUE => Deux jokers autorises
int T_record_type::is_compatible_with(T_type *ref_type,
											   int low_ctrl/*=FALSE*/)
{
  if (ref_type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}
  // Optimisation
  if (get_unique_record_type_mode() == TRUE && this == ref_type)
    {
	  return TRUE;
    }

  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_record_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif


  if (ref_type->get_node_type() != NODE_RECORD_TYPE)
	{
#ifdef DEBUG_TYPE
		  TRACE0("ref_type pas record -> false") ;
#endif
		  return FALSE ;
	}

  // Ici on sait qu'on a deux records ...
  T_record_type *ref_record = (T_record_type *)ref_type ;

  // ... on va verifier les labels un a un dans le meme ordre
  T_label_type *cur = first_label ;
  T_label_type *ref = ref_record->first_label ;

  for (;;)
	{
	  if (cur == NULL)
		{
#ifdef DEBUG_TYPE
		  TRACE3("cur %x ref %x -> %s", cur, ref, (ref == NULL) ? "TRUE":"FALSE") ;
#endif
		  return (ref == NULL) ? TRUE : FALSE ;
		}

	  if (ref == NULL)
		{
#ifdef DEBUG_TYPE
		  TRACE2("cur %x ref %x -> FALSE", cur, ref) ;
#endif
		  return FALSE ;
		}

	  if (cur->is_compatible_with(ref, low_ctrl) == FALSE)
		{
#ifdef DEBUG_TYPE
		  TRACE0("types incompatibles") ;
#endif
		  return FALSE ;
		}

	  // Au suivant !
	  cur = (T_label_type *)cur->get_next() ;
	  ref = (T_label_type *)ref->get_next() ;
	}
}

int T_label_type::is_compatible_with(T_type *ref_type,
											  int low_ctrl/*=FALSE*/)
{
  if (ref_type == NULL || type == NULL)
	{
	  // Reprise sur erreur
	  return TRUE ;
	}

  // Optimisation
  if (get_unique_record_type_mode() == TRUE && this == ref_type)
    {
	  return TRUE;
    }


  // Il faut etudier les cas particuliers ou l'un des deux
  // est {} ou []
  if (ref_type->get_node_type() == NODE_GENERIC_TYPE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("ref_type est [] ou {} : etude du cas particulier") ;
#endif
	  // On appelle la methode adequate
	  return ref_type->is_compatible_with(this) ;
	}

#ifdef DEBUG_TYPE
  TRACE3("T_label_type(%x)::is_compatible_with(%x, %s)",
		 this,
		 ref_type,
		 ref_type->get_class_name()) ;
#endif


#ifdef DEBUG_TYPE
  TRACE2("etude de compatibilite de %s et %s",
		 make_type_name()->get_value(),
		 ref_type->make_type_name()->get_value()) ;
#endif


  if (ref_type->get_node_type() != NODE_LABEL_TYPE)
	{
#ifdef DEBUG_TYPE
		  TRACE0("ref_type pas label -> false") ;
#endif
		  return FALSE ;
	}

  // Ici on sait qu'on a deux labels ...
  T_label_type *ref_label = (T_label_type *)ref_type ;

  // GP 19/04/99 BUG 2146
  // Quand low_ctrl == TRUE, on accepte qu' aucun des deux labels n'ai de
  // nom (2 jokers).
  // C'est utile par exemple lorsque l'on regarde la compatibilite
  // des types dans un ensemble

  // Cas par defaut:
  // Il faut le meme nom ou un et un seul joker on ne traite que le
  // cas de joker a droite, donc s'il est a gauche, on rappelle la
  // methode dans l'autre sens !
  if (name == NULL)
	{
	  if (ref_label->name != NULL)
		{
		  return ref_type->is_compatible_with(this, low_ctrl) ;
		}
	  else
		{
#ifdef DEBUG_TYPE
		  TRACE0("deux jokers !") ;
#endif
		  if(low_ctrl == FALSE)
			{
			  // GP BUG 2146 : Cas par defaut: deux jokers sont interdits
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_RECORD_ITEM_WITHOUT_LABEL)) ;
			  semantic_error(ref_label,
							 MULTI_LINE,
							 get_error_msg(E_LOCATION_OF_OTHER_JOKER)) ;
			  return FALSE ;
			}
		  else
			{
			  // GP BUG 2146 : cas particulier de la compatibilite des types
			  // dans un ensemble : On accepte 2 jokers
			  TRACE0("cas particulier") ;
			  return type->is_compatible_with(ref_label->type) ;
			}
		}
	}

  // Ici on sait que name != NULL
  ASSERT(name != NULL) ;
  if (ref_label->name == NULL)
	{
	  TRACE0("clonage du nom ...") ;
	  ref_label->name = new T_ident(name, NULL, NULL, ref_label) ;

	  TRACE2("ref_label %x -> name %x", ref_label, ref_label->name) ;
	}
  else
	{
	  if (name->get_name()->compare(ref_label->get_name()->get_name()) == FALSE)
		{
#ifdef DEBUG_TYPE
	  TRACE0("noms differents") ;
#endif
	  return FALSE ;
		}
	}

  // .. et le meme type
  if (type->is_compatible_with(ref_label->type) == FALSE)
	{
#ifdef DEBUG_TYPE
	  TRACE0("types differents") ;
#endif
	  return FALSE ;
	}

#ifdef DEBUG_TYPE
	  TRACE0("ok") ;
#endif
  return TRUE ;
}

//
// Fin du fichier
//
