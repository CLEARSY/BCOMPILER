/******************************* CLEARSY **************************************
* Fichier : $Id: v_ldreq.h,v 2.0 1997-07-08 07:45:02 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyseur sémantique
*					Chargement des machines requises
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
#ifndef _C_LDREQ_H_
#define _C_LDREQ_H_

// Chargement des machines requises avec mise a jour des references
// (via les T_used_machine) dans le Betree
extern T_betree *load_required_machines(T_betree *betree, const char* converterName) ;

#endif /* _C_LDREQ_H_ */


