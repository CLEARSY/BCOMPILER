/******************************* CLEARSY **************************************
* Fichier : $Id: i_ctx.h,v 2.0 2002-09-24 09:42:09 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Contextes utilises comme parametres des
*					fonctions virtuelles
*                   de la verification de B0 et definissant des "contextes"
*                   influant sur le comportement de la fonction consideree
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

#ifndef _I_CTX_H_
#define _I_CTX_H_

//
// Contexte pour la verification de B0
//
typedef enum
{
  /* 00 */
  B0CTX_DEFAULT = 0,
  /* 01 */
  B0CTX_ARRAY_CONTEXT,
  /* 02 */
  B0CTX_FORMAL_PARAMETER_TYPE_CONTEXT,
  /* 03 */
  B0CTX_VALUATION_CONTEXT,
  /* 04 */
  B0CTX_ARRAY_VALUATION_CONTEXT,
  /* 05 */
  B0CTX_INSTANCIATION_CONTEXT,
  /* 06 */
  B0CTX_MAPLET_INDEX_CONTEXT
} T_B0_context ;


typedef enum
{
  /* 00 */
  B0_CTX_INIT_VAR_LOC = 0,
  /* 01 */
  B0_CTX_VALUATION,
  /* 02 */
  B0_CTX_GENERAL
} T_B0_type_context ;

typedef enum
{
  /* 00 */
  B0_OM_UNDETERMINED_ST, // statut est indeterminé
  /* 01 */
  B0_OM_CONCRETE_ST, // statut concret
  /* 02 */
  B0_OM_ABSTRACT_ST // statut abstrait
} T_B0_OM_status;

#endif /* _I_CTX_H_ */

