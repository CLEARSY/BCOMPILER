/******************************* CLEARSY **************************************
* Fichier : $Id: p_item.cpp,v 2.0 2000/07/11 16:09:01 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_item
* 				Modelisation des elements d'une liste.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_item.cpp,v 1.6 2000/07/11 16:09:01 fl Exp $") ;

// includes head
#include "p_head.h"


//Definition des Methodes
// ___________________

// Constructeur
T_resource_item::T_resource_item(T_resource_item **adr_first_aopp,
											T_resource_item **adr_last_aopp,
											T_resource_item *new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE4("-> New ITEM %x first %x last %x father %x",
		 this,
		 (adr_first_aopp != NULL)? *adr_first_aopp:NULL,
		 (adr_first_aopp != NULL)? *adr_last_aopp:NULL,
		 new_father_aop) ;
#endif

  // Initialise l'objet
  // _____________

  // Position du type de l'objet
  set_type(RESOURCE_ITEM) ;

  // Position du pere
  set_father(new_father_aop) ;

  // Positionne la liste
  set_next(NULL) ;
  set_prev(NULL) ;

  // Insere l'objet en fin de liste
  // _____________________

  // Teste la coherence de la liste
  if ((adr_first_aopp != NULL) && (adr_last_aopp != NULL))
	{
	  if (*adr_last_aopp == NULL)
		{
		  // Chainage en premier
		  *adr_first_aopp = this ;
		  set_prev(NULL) ;
		}
	  else
		{
		  // Chainage en queue
		  (*adr_last_aopp)->set_next(this) ;
		  set_prev(*adr_last_aopp) ;
		}

	  set_next(NULL) ;
	  *adr_last_aopp = this ;
	}

#ifdef DEBUG_RESOURCES
  TRACE4("<- New ITEM %x first %x last %x father %x",
		 this,
		 (adr_first_aopp != NULL)? *adr_first_aopp:NULL,
		 (adr_first_aopp != NULL)? *adr_last_aopp:NULL,
		 new_father_aop) ;
  EXIT_TRACE ;
#endif
}

// Destructeur virtuel
T_resource_item::~T_resource_item(void)
{ // NE FAIT RIEN
}


// Definition de Fonctions
// ________________________

// Detruit une liste d'objets
void delete_list(T_resource_item *&first_item_aop,
						  T_resource_item *&last_item_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE1("Del list ITEM %x", first_item_aop) ;
#endif

  // Parcours la liste
  while(first_item_aop != NULL)
	{
	  // Pointe l'objet courant
	  T_resource_item *cur_item_lop = first_item_aop ;

	  // Pointe l'objet suivant
	  first_item_aop = first_item_aop->get_next() ;

	  if(first_item_aop == NULL)
		{
		  last_item_aop  = NULL ;
		}

	  // Detruit l'objet courant
	  delete cur_item_lop ;
	  cur_item_lop = NULL ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Detruit un element d'une liste d'objets
void delete_item(T_resource_item *item_aop,
						  T_resource_item *&first_item_aop,
						  T_resource_item *&last_item_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("Del ITEM %x from %x to %x",
		 item_aop,
		 first_item_aop,
		 last_item_aop) ;
#endif

  if(item_aop != NULL)
	{
	  // Cas du premier
	  if(first_item_aop == item_aop)
		{
		  first_item_aop = item_aop->get_next() ;
		}

	  // Cas du dernier
	  if(last_item_aop == item_aop)
		{
		  last_item_aop = item_aop->get_prev() ;
		}

	  // Cas general

	  // Retir l'objet de la liste
	  // __________________________

	  // Raccroche le precedant au suivant
	  if(item_aop->get_prev() != NULL)
		{
		  item_aop->get_prev()->set_next(item_aop->get_next()) ;
		}

	  // Raccroche le suivant au precedant
	  if(item_aop->get_next() != NULL)
		{
		  item_aop->get_next()->set_prev(item_aop->get_prev()) ;
		}

	  // Libere l'objet
	  delete item_aop ;
	  item_aop = NULL ;
	}

#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}


// Fin de fichier
// ___________
