/******************************* CLEARSY **************************************
* Fichier : $Id: c_exptyp.h,v 2.0 2000-11-03 12:52:16 lv Exp $
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
#ifndef _C_EXPTYP_H_
#define _C_EXPTYP_H_

//
//	}{	Type d'operateur unaire
//
typedef enum
{
  /* '-'		*/ 	UOP_MINUS,			// OK
} T_unary_operator ;

//
//	}{	Type d'operateur binaire
//
typedef enum
{
					/* 00 '+'		*/	BOP_PLUS,						// OK
					/* 01 '-'		*/	BOP_MINUS,						// OK
					/* 02 '*'		*/	BOP_TIMES,						// OK
					/* 03 '/'		*/	BOP_DIVIDES,   					// OK
					/* 04 'mod'		*/	BOP_MOD,						// OK
					/* 05 '**'		*/	BOP_POWER,						// OK
					/* 06 '<|'		*/	BOP_RESTRICT,					// OK
					/* 07 '<<|'		*/	BOP_ANTIRESTRICT,				// OK
					/* 08 '|>'		*/	BOP_CORESTRICT,					// OK
					/* 09 '|>>'		*/	BOP_ANTICORESTRICT,				// OK
					/* 10 '<+'		*/	BOP_LEFT_OVERLOAD, 				// OK
					/* 11 '^'		*/	BOP_CONCAT,						// OK
					/* 12 '->'		*/	BOP_HEAD_INSERT,				// OK
					/* 13 '<-'		*/	BOP_TAIL_INSERT,				// OK
					/* 14 '/|\'		*/	BOP_HEAD_RESTRICT,				// OK
					/* 15 '\|/'		*/	BOP_TAIL_RESTRICT,				// OK
					/* 16 '..'		*/	BOP_INTERVAL,					// OK
					/* 17 '/\'		*/	BOP_INTERSECTION,	   			// OK
					/* 18 '\/'		*/	BOP_UNION,						// OK
					/* 19 '|->'		*/	BOP_MAPLET,						// OK
					/* 20 '<->'		*/	BOP_SET_RELATION,				// OK
					/* 21 '*'		*/	BOP_CONSTANT_FUNCTION, 			// OK
					/* 22 ';'		*/	BOP_COMPOSITION,   				// OK
					/* 23 '><'		*/	BOP_DIRECT_PRODUCT,		   		// OK
					/* 24 '||'		*/	BOP_PARALLEL_PRODUCT,  	   		// OK
					/* 25 ','		*/	BOP_COMMA,						// OK
					/* 26 '<->>'	*/	BOP_SURJ_RELATION,				// OK
					/* 27 '<<->'	*/	BOP_TOTAL_RELATION,				// OK
					/* 28 '<<->>'	*/	BOP_TOTAL_SURJ_RELATION,		// OK
					// OK

	//
	//	Les operateurs ci-apres sont reserves au compilateur B
	//  Ils n'ont qu'une existence temporaire et ne sont jamais
	//	visibles dans un Betree termine
	//
                                        /* 28 '|'		*/	BOP_PIPE   					// OK
} T_binary_operator ;

//
//	}{	Type d'identificateur
//
typedef enum
{
  /* 00 */
  ITYPE_UNKNOWN = 0,
  /* 01 */
  ITYPE_CONCRETE_CONSTANT,
  /* 02 */
  ITYPE_CONCRETE_VARIABLE,
  /* 03 */
  ITYPE_BUILTIN,
  /* 04 */
  ITYPE_MACHINE_NAME,
  /* 05 */
  ITYPE_ABSTRACTION_NAME,
  /* 06 */
  ITYPE_MACHINE_SCALAR_FORMAL_PARAM,
  /* 07 */
  ITYPE_ABSTRACT_SET,
  /* 08 */
  ITYPE_OP_NAME,
  /* 09 */
  ITYPE_LOCAL_OP_NAME,
  /* 10 */
  ITYPE_MACHINE_SET_FORMAL_PARAM,
  /* 11 */
  ITYPE_USED_MACHINE_NAME,
  /* 12 */
  ITYPE_ENUMERATED_VALUE,
  /* 13 */
  ITYPE_DEFINITION_PARAM,
  /* 14 */
  ITYPE_OP_IN_PARAM,
  /* 15 */
  ITYPE_OP_OUT_PARAM,
  /* 16 */
  ITYPE_LOCAL_VARIABLE,
  /* 17 */
  ITYPE_LOCAL_QUANTIFIER,
  /* 18 */
  ITYPE_ABSTRACT_VARIABLE,
  /* 19 */
  ITYPE_ABSTRACT_CONSTANT,
  /* 20 */
  ITYPE_ENUMERATED_SET,
  /* 21 */
  ITYPE_USED_OP_NAME,
  /* 22 */
  ITYPE_RECORD_LABEL,
  /* 23 */
  ITYPE_ANY_QUANTIFIER,
  /* 24 */
  ITYPE_PRAGMA_PARAMETER // DOIT OBLIGATOIREMENT ETRE LE DERNIER
} T_ident_type ;

#endif /* _C_EXPTYP_H_ */


