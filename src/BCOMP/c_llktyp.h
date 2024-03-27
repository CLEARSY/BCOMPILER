/******************************* CLEARSY **************************************
* Fichier : $Id: c_llktyp.h,v 2.0 1999-07-21 07:42:59 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe
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
#ifndef _C_LLKTYP_H_
#define _C_LLKTYP_H_

//
//	}{	Type de T_list_link
//
typedef enum
	{
	/* 00 */	LINK_IN = 0,			/* parametre d'entree d'operation	*/
	/* 01 */	LINK_OUT,				/* parametre de sortie d'operation	*/
	/* 02 */	LINK_CASE_SELECT,   	/* valeur dans un case				*/
	/* 03 */	LINK_ARRAY_RANGE,   	/* etendue dans un tableau			*/
	/* 04 */	LINK_CONSTANT_VALUE, 	/* valeur de constante				*/
	/* 05 */	LINK_DEFINITION,   		/* lexeme de clause DEFINITIONS		*/
	/* 06 */	LINK_BUILTIN,   		/* pointeur sur builtin				*/
	/* 07 */	LINK_OP_LIST,   		/* liste d'operations				*/
	/* 08 */	LINK_BETREE_LIST,	  	/* element d'un liste de betree	   	*/
	/* 09 */	LINK_BINOP_OPER_LIST,  	/* liste generalisee des oper d'un binop */
	/* 10 */	LINK_RELATION_SRC_SET_LIST,/* liste des sets scr d'une rela */
	/* 11 */	LINK_RELATION_DST_SET_LIST,/* liste des sets dst d'une rela */
	/* 12 */	LINK_TYPE_LIST,			/* liste de types B0 */
	/* 13 */	LINK_DOLLAR_ZERO,	   	/* ident $0 */
	/* 14 */	LINK_READ_SYMBOL,	   	/* symbole lu depuis le disque */
	/* 15 */	LINK_OTHER				/* AUTRE							*/
	} T_list_link_type ;

#endif /* _C_LLKTYP_H_ */


