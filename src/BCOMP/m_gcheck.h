/******************************* CLEARSY **************************************
* Fichier : $Id: m_gcheck.h,v 2.0 1999-10-27 07:49:03 clb Exp $
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
******************************************************************************/
#ifdef  ACTION_NG

#ifndef _M_GCHECK_H_
#define _M_GCHECK_H_

//initialisation de la classe T_global_component_checksum
extern void init_T_global_component_checksum(void) ;

//ecriture d'un retour chariot dans le fichier d'etat
extern void write_state_rc(FILE *fd) ;

//classe définissant le checksum d'un composant
class T_global_component_checksum : public T_item
{
	//attributs

	//nom du composant
	T_component *component ;

	//checksum du composant
	T_symbol *check ;

public :

	//constructeurs
	T_global_component_checksum(T_component *comp,
										 T_symbol *checksum,
										 T_item** adr_first,
										 T_item** adr_last,
										 T_item *father,
										 T_betree *betree,
										 T_lexem *ref_lexem) ;

	T_global_component_checksum(T_component *component) ;

	//destructeur
	virtual ~T_global_component_checksum(void) ;

	//ecriture du nom du composant et de son checksum dans le fichier d'etat
	void write_state_file(FILE *fd) ;

	//comparaison de deux checksums
	//rend TRUE si ils sont egaux, FALSE sinon
	int compare(T_global_component_checksum *other_checksum) ;

	//methodes de consultation

	//retourne le nom du composant
	T_symbol *get_component_name(void) ;

	//retourne le checksum
	T_symbol *get_checksum(void) ;

  //retourne le composant
  T_component *get_component(void) ;

} ;

#endif /* _M_GCHECK_H_ */

#endif /* ACTION_NG */

