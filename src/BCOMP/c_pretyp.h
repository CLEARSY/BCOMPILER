/******************************* CLEARSY **************************************
* Fichier : $Id: c_pretyp.h,v 2.0 1999-07-21 07:43:05 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Types de predicats
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
#ifndef _C_PREDTYP_H_
#define _C_PREDTYP_H_

//
//	}{	Type de predicat binaire
//
typedef enum
{
  /* 'or'		*/
  BPRED_OR,
  /* '=>'		*/
  BPRED_IMPLIES,
  /* '<=>'	*/
  BPRED_EQUIVALENT,
  /* '&'		*/
  BPRED_CONJONCTION
} T_binary_predicate_type ;

//
//	}{	Type de predicat entre deux expressions
//
typedef enum
{
  /* '/='		*/
  EPRED_DIFFERENT,
  /* '/:'		*/
  EPRED_NOT_BELONGS,
  /* '/<:'	*/
  EPRED_NOT_INCLUDED,
  /* '/<<:'	*/
  EPRED_NOT_STRICT_INCLUDED,
  /* '<'		*/
  EPRED_LESS_THAN,
  /* '>'		*/
  EPRED_GREATER_THAN,
  /* '<='		*/
  EPRED_LESS_THAN_OR_EQUAL,
  /* '>='		*/
  EPRED_GREATER_THAN_OR_EQUAL
} T_expr_predicate_type ;

//
//	}{	Type de predicat de typage
//
typedef enum
{
  /* '='		*/
  TPRED_EQUAL,
  /* ':'		*/
  TPRED_BELONGS,
  /* '<:'		*/
  TPRED_INCLUDED,
  /* '<<:'	*/
  TPRED_STRICT_INCLUDED
} T_typing_predicate_type ;

//
//	}{	Type d'inference de type pour un predicat de typage
typedef enum
{
  /* typage par ':' */
  ITYPE_1_1,
  /* typage par '<:' pour '<<:' */
  ITYPE_1_2,
  /* typage par '=' */
  ITYPE_1_3
} T_typing_predicate_case ;

#endif /* _C_PREDTYP_H_ */


