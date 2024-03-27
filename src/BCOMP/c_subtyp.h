/******************************* CLEARSY **************************************
* Fichier : $Id: c_subtyp.h,v 2.0 1999-07-21 07:43:06 sl Exp $
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
#ifndef _C_SUBTYP_H_
#define _C_SUBTYP_H_

//
//	}{	Types de lien de composition entre deux subtitutions ('||' ou ';')
//
typedef enum
	{
	LINK_PARALLEL,
	LINK_SEQUENCE
	} T_substitution_link_type ;

#endif /* _C_SUBTYP_H_ */


