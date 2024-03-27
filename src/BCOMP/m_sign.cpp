/******************************* CLEARSY **************************************
* Fichier : $Id: m_sign.cpp,v 2.0 1999-11-08 15:28:32 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : classe T_signature
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
#ifdef  ACTION_NG
#include "c_port.h"
RCS_ID("$Id: m_sign.cpp,v 1.6 1999-11-08 15:28:32 clb Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_sign.h"

//initialisation de la classe T_signature
void init_T_signature(void)
{
	TRACE0(">> init_T_signature") ;
	ENTER_TRACE ;

	//crée la description de métaclasse pour T_signature (NODE_USER_DEFINED4)
	T_metaclass *meta_signature =
		new T_metaclass("T_signature",
						sizeof(T_signature),
						get_metaclass(NODE_ITEM)) ;

	//ajout de la description des champs de T_signature :
	//checksum du composant
	//premier et dernier  de la liste des checksum des composants dependants
	meta_signature->add_field("main_checksum", FIELD_OBJECT, REF_SHARED) ;
	meta_signature->add_field("first_list_checksum",
							  FIELD_OBJECT,
							  REF_SHARED) ; //ou REF_PRIVATE ??
	meta_signature->add_field("last_list_checksum",
							  FIELD_OBJECT,
							  REF_SHARED) ; //ou REF_PRIVATE ??

	meta_signature->add_field("cur_checksum",
							  FIELD_OBJECT,
							  REF_SHARED) ; //ou REF_PRIVATE ??

  //affectation de la métaclasse à la classe
  set_metaclass(NODE_USER_DEFINED4, meta_signature) ;

  EXIT_TRACE ;
  TRACE0("<< init_T_signature") ;
}




//
//classe T_signature définissant la signature d'un composant
//

//constructeurs

//construit une signature complete
T_signature::T_signature(T_global_component_checksum *main,
						 T_global_component_checksum *adr_first,
						 T_global_component_checksum *adr_last)

{
	TRACE3(">> constructeur T_signature(%x,%x,%x)",
		   main,
		   adr_first,
		   adr_last) ;

	main_checksum = main ;
	first_list_checksum = adr_first ;
	last_list_checksum = adr_last ;
	cur_checksum = main ;

	set_block_state(this, MINFO_PERSISTENT) ;

	TRACE3("<<constructeur T_signature(%x,%x,%x)",
		   main_checksum,
		   first_list_checksum,
		   last_list_checksum) ;
}

//construit une signature avec uniquement le checksum de la racine
T_signature::T_signature(T_global_component_checksum *main)

{
	TRACE1(">> constructeur T_signature(%x)", main) ;

	main_checksum = main ;
	first_list_checksum = NULL ;
	last_list_checksum = NULL ;
	cur_checksum = main ;

	set_block_state(this, MINFO_PERSISTENT) ;

	TRACE1("<< constructeur T_signature(%x)", main_checksum) ;
}


//destructeur
T_signature::~T_signature(void)
{
	TRACE0(">> ~T_signature");

	//destruction du checksum principal
	//	delete[] main_checksum;

	//destruction de la liste
	//   	T_global_component_checksum *cur_checksum = first_list_checksum ;

	/*while ( cur_checksum != NULL )
		{
		T_global_component_checksum *next =
		  (T_global_component_checksum *) (cur_checksum->get_next()) ;
		cur_checksum->remove_from_list((T_item **)first_list_checksum,
									   (T_item **)last_list_checksum) ;
		 cur_checksum = next ;
		}
	*/
	TRACE2("<< ~T_signature(first_list_checksum=%x, last_list_checksum=%x)",
		   first_list_checksum,
		   last_list_checksum);
}

//ajout d'un checkum d'un composant dependant a une signature
void T_signature::add_checksum(T_global_component_checksum
											   *dependant_checksum)
{
  TRACE1(">> T_signature::add_checksum (%s)",
		 dependant_checksum->get_component_name()->get_value()) ;

  //ajout du checksum en queue de la liste
  dependant_checksum->tail_insert((T_item **)&first_list_checksum,
								  (T_item **)&last_list_checksum) ;

  TRACE0("<< T_signature::add_checksum") ;
}

//redefinition de la methode virtuelle de T_object get_class_name
//pour la classe T_signature
const char* T_signature::get_class_name(void)
{
	return "T_signature" ;
}

//comparaison de deux signatures
int T_signature::compare(T_signature *other_sign)
{
  TRACE1(">> T_signature::compare(%x)", other_sign) ;

  //comparaison du checksum du composant principal
  int res = main_checksum->compare(other_sign->get_main_checksum()) ;

  //comparaison des checksums des composants dependants
  T_global_component_checksum *cur_checksum1 = first_list_checksum ;
  T_global_component_checksum *cur_checksum2 =
	other_sign->get_first_checksum() ;

  //on verifie que tous les checksums de sign
  //sont bien dans other_sign
  while ( cur_checksum1 != NULL  && res == TRUE )
	{
		res = FALSE ;
		while (cur_checksum2 != NULL  && res != TRUE)
			{
				res = cur_checksum1->compare(cur_checksum2) ;
				cur_checksum2 = (T_global_component_checksum *)
					(cur_checksum2->get_next()) ;
			}
		cur_checksum2 = other_sign->get_first_checksum() ;
		cur_checksum1 = (T_global_component_checksum *)
			(cur_checksum1->get_next()) ;
	}


  //on verifie que tous les checksums de other_sign
  //sont bien dans sign
   cur_checksum1 = first_list_checksum ;
   cur_checksum2 = other_sign->get_first_checksum() ;

   while ( cur_checksum2 != NULL  && res == TRUE )
	{
		while (cur_checksum1 != NULL  && res != TRUE)
			{
				res = cur_checksum2->compare(cur_checksum1) ;
				cur_checksum1 = (T_global_component_checksum *)
					(cur_checksum1->get_next()) ;
			}
		cur_checksum1 = first_list_checksum ;
		cur_checksum2 = (T_global_component_checksum *)
			(cur_checksum2->get_next()) ;
	}


  TRACE1("<< T_signature::compare(%d)", res) ;

  return res ;
}

//compare le checksum principal a check
//rend TRUE si ils sont egaux, FALSE sinon
int
  T_signature::compare_main_checksum(T_global_component_checksum *check)
{
  TRACE2(">> T_signature::compare_main_checksum(%x,%x)", this, check) ;
  return main_checksum->compare(check) ;
}

//ecriture de la signature dans le fichier d'etat fd
void T_signature::write_state_file(FILE *fd)
{
	TRACE2(">> T_signature::write_state_file(%x,%x)", this, fd) ;

	//ecriture du nom du composant principal suivi de son checksum
	main_checksum->write_state_file(fd) ;
	//saut de ligne
	write_state_rc(fd) ;

	//ecriture des noms des composants dependants suivis de leurs checksums
	T_global_component_checksum *cur_checksum = first_list_checksum ;

	while ( cur_checksum != NULL )
		{
			cur_checksum->write_state_file(fd) ;
			cur_checksum = (T_global_component_checksum *)
				(cur_checksum->get_next()) ;
		}

	TRACE0("<< T_signature::write_state_file") ;
}

//rend le checksum principal
T_global_component_checksum *T_signature::get_main_checksum(void)
{
  TRACE1(">> T_signature::get_main_checksum(%x)", main_checksum) ;
  return main_checksum ;
}

//rend le premier checksum de la liste des checksum des composants dependants
T_global_component_checksum *T_signature::get_first_checksum(void)
{
  TRACE1(">> T_signature::get_first_checksum(%x)",
		 first_list_checksum) ;
  return first_list_checksum ;
}

//recuperation du checksum suivant
T_global_component_checksum *T_signature::get_next_checksum(void)
{
  TRACE0(">> T_signature::get_next_checksum") ;

  if ( cur_checksum == main_checksum)
	{
	  cur_checksum = first_list_checksum ;
	}
  else
	{
	  cur_checksum =
		(T_global_component_checksum *)(cur_checksum->get_next()) ;
	}


  TRACE1("<< T_signature::get_next_checksum (%x)", cur_checksum) ;

  return cur_checksum ;
}

#endif /* ACTION_NG */
