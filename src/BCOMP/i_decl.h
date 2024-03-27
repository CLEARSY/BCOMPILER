/******************************* CLEARSY **************************************
* Fichier : $Id: i_decl.h,v 2.0 2002-07-16 09:37:16 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Module de vérification B0 des déclarations de données
*
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
#ifndef _I_DECL_H_
#define _I_DECL_H_

//
// Fonction de vérification que les identificateurs n'ont pas '__'
// dans leurs nom
//
void B0_OM_name_check(T_ident *name);

#endif // _I_DECL_H_
