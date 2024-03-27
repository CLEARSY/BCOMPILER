/******************************* CLEARSY **************************************
* Fichier : $Id: m_gcheck.cpp,v 2.0 1999-10-27 07:49:03 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : classe T_global_component_checksum
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
******************************************************************************/#ifdef  ACTION_NG
#include "c_port.h"
RCS_ID("$Id: m_gcheck.cpp,v 1.3 1999-10-27 07:49:03 clb Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

#include "m_gcheck.h"

//fonction static d'ecriture d'un blanc dans le fichier d'etat
static void write_space(FILE *fd)
{
	TRACE0("write_space") ;
	fputc(' ', fd) ;
}

//fonction static d'ecriture d'un @ dans le fichier d'etat
static void write_acommercial(FILE *fd)
{
	TRACE0("write_acommercial") ;
	fputc('@', fd) ;
}

//fonction static d'ecriture d'un @ dans le fichier d'etat
extern void write_state_rc(FILE *fd)
{
	TRACE0("write_state_rc") ;
	fputc('\n', fd) ;
}

//initialisation de la classe T_global_component_checksum
void init_T_global_component_checksum(void)
{
	TRACE0(">> init_T_global_component_checksum") ;
	ENTER_TRACE ;

	//crée la description de métaclasse pour T_global_component_checksum
	// (NODE_USER_DEFINED1)
	T_metaclass *meta_global_component_checksum =
		new T_metaclass("T_global_component_checksum",
						sizeof(T_global_component_checksum),
						get_metaclass(NODE_ITEM)) ;

	//ajout de la description des champs de T_global_component_checksum :
	//nom et checksum du composant
	meta_global_component_checksum->add_field("component",
											  FIELD_OBJECT,
											  REF_SHARED) ; //ou REF_PRIVATE ??
	meta_global_component_checksum->add_field("check",
											  FIELD_OBJECT,
											  REF_SHARED) ; //ou REF_PRIVATE ??

  //affectation de la métaclasse à la classe
  set_metaclass(NODE_USER_DEFINED1, meta_global_component_checksum) ;

  EXIT_TRACE ;
  TRACE0("<< init_T_global_component_checksum") ;
}


//
//classe T_global_component_checksum définissant le checksum d'un composant
//

//constructeurs
T_global_component_checksum ::T_global_component_checksum
(T_component *comp,
 T_symbol *checksum,
 T_item** adr_first,
 T_item** adr_last,
 T_item *father,
 T_betree *betree,
 T_lexem *ref_lexem)
 : T_item(NODE_USER_DEFINED1, adr_first, adr_last, father, betree, ref_lexem)
{
	TRACE2(">> constructeur T_global_component_checksum(%s,%s)",
		   comp->get_name()->get_value(),
		   checksum->get_value()) ;

	component = comp ;
	check = checksum ;
	check->link() ;

 	set_block_state(this, MINFO_PERSISTENT) ;
	//	set_block_state(component, MINFO_PERSISTENT) ;
	set_block_state(check, MINFO_PERSISTENT) ;

	TRACE2("<< constructeur T_global_component_checksum(%s,%s)",
		   component->get_name()->get_value(),
		   check->get_value()) ;
}

T_global_component_checksum::T_global_component_checksum(T_component
														 *comp)
: T_item(NODE_USER_DEFINED1, NULL, NULL, NULL, NULL, NULL)
{
	TRACE1(">> constructeur T_global_component_checksum(%s)",
		   comp->get_name()->get_value()) ;

	component = comp ;
	check = component->get_checksum() ;
	check->link() ;

	set_block_state(this, MINFO_PERSISTENT) ;
	//	set_block_state(checkomponent, MINFO_PERSISTENT) ;
	set_block_state(check, MINFO_PERSISTENT) ;

	TRACE0("<< constructeur T_global_component_checksum") ;
}

//destructeur
T_global_component_checksum::~T_global_component_checksum(void)
{
	TRACE0(">> ~T_global_component_checksum") ;
}

//ecriture du nom du composant et de son checksum dans le fichier d'etat
void T_global_component_checksum::write_state_file(FILE *fd)
{
	TRACE2(">> T_global_component_checksum::write_state_file(%x,%x)",
		   this,
		   fd) ;
	TRACE1("name(%x)", component->get_name()) ;
	TRACE1("name(%s)", component->get_name()->get_value()) ;
	fputs(component->get_name()->get_value(), fd) ;
	TRACE0("write_space") ;
	write_space(fd) ;
	TRACE0("write_acommercial") ;
	write_acommercial(fd) ;
	TRACE0("write_check") ;
	if (check != NULL)
		{
			fputs(check->get_value(), fd) ;
		}
	else
		{
			fputs("0", fd) ;
		}
	TRACE0("write_rc") ;
	write_state_rc(fd) ;

	TRACE0("<< T_global_component_checksum::write_state_file") ;
}

//comparaison de deux checksum
int T_global_component_checksum::compare(T_global_component_checksum
												  *other_checksum)
{
  TRACE1(">> T_global_component_checksum::compare(%x)", other_checksum) ;

  int res =
	((component->get_name())->compare(other_checksum->get_component_name()) &&
	 check->compare(other_checksum->get_checksum())) ;

  TRACE1(">> T_global_component_checksum::compare(%d)", res) ;

  return res ;
}

//retourne le nom du composant
T_symbol *T_global_component_checksum::get_component_name(void)
{
  	TRACE1(">> T_global_component_checksum::get_component_name(%s)",
		   component->get_name()->get_value()) ;
	return component->get_name() ;
}

//retourne le checksum
T_symbol *T_global_component_checksum::get_checksum(void)
{
  	TRACE1(">> T_global_component_checksum::get_checksum(%s)",
		   check->get_value()) ;
	return check ;
}

//retourne le composant
T_component *T_global_component_checksum::get_component(void)
{
  TRACE1(">> T_global_component_checksum::get_component(%x)", component) ;
  return component ;
}

#endif /* ACTION_NG */
