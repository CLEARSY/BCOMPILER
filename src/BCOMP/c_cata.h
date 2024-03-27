/******************************* CLEARSY **************************************
* Fichier : $Id: c_cata.h,v 2.0 2002-07-12 10:03:04 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Catalogue de messages multilingue
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
#ifndef _C_CATA_H_
#define _C_CATA_H_

// Messages du catalogue
typedef enum
{
  /* 00 */
  C_EXPR_OR_PRED,
  /* 01 */
  C_MACHINE_NAME,
  /* 02 */
  C_IO_ERROR,
  /* 03 */
  C_ABORTING,
  /* 04 */
  C_OPN_MIGHT_NOT_BE_CLOSED,			/* %s opn name */
  /* 05 */
  C_BEGINNING_OF, 	   				/* %s opn name */
  /* 06 */
  C_END_OF, 							/* %s opn name */
  /* 07 */
  C_UNEXPECTED,
  /* 08 */
  C_UNEXPECTED_EOF_AFTER_THIS_TOKEN,
  /* 09 */
  C_UNEXPECTED_IDENT_VALUE,			   			/* %s ident name */
  /* 10 */
  C_UNEXPECTED_STRING_VALUE,			   			/* %s value */
  /* 11 */
  C_UNEXPECTED_LITERAL_NUMBER_VALUE,   			/* %s value */
  /* 12 */
  C_UNEXPECTED_TOKEN_VALUE, 		   				/* %s ascii */
  /* 13 */
  C_WHERE,
  /* 14 */
  C_EXPECTED_VALUE,					/* %s expected value */
  /* 15 */
  C_IS_A_MACRO_DEF_HERE,   			/* %s macro name */
  /* 16 */
  C_REDEFINITION_OF,					/* %s redefined */
  /* 17 */
  C_LOCATION_OF_PREVIOUS_OCCURENCE,
  /* 18 */
  C_EXPR,
  /* 19 */
  C_PRED,
  /* 20 */
  C_IDENT,
  /* 21 */
  C_SUBST,
  /* 22 */
  C_INTERNAL_ERROR,
  /* 23 */
  C_EOF,
  /* 24 */
  C_THEN,
  /* 25 */
  C_VAR_OF_LAMBDA_EXPR,
  /* 26 */
  C_PAREXPR,
  /* 27 */
  C_PREDICATE_PIPE_EXPR,
  /* 28 */
  C_VAR_OF_SIGMA,
  /* 29 */
  C_VAR_OF_PI,
  /* 30 */
  C_VAR_OF_QUNION,
  /* 31 */
  C_VAR_OF_QINTER,
  /* 32 */
  C_QUANT_VAR,
  /* 33 */
  C_PARPRED,
  /* 34 */
  C_IDENT_OR_FCALL,
  /* 35 */
  C_OP_NAME,
  /* 36 */
  C_UNABLE_TO_OPEN_TRACE_FILE,
  /* 37 */
  C_DEF_ARG,
  /* 38 */
  C_CLOSE_PAREN,
  /* 39 */
  C_OPEN_PAREN,
  /* 40 */
  C_CLOSE_BRACES,
  /* 41 */
  C_OPEN_BRACES,
  /* 42 */
  C_REWRITES,
  /* 43 */
  C_DEF_CLAUSE,
  /* 44 */
  C_SET_ITEM,
  /* 45 */
  C_ATOM_UN_BINOP_OPN,
  /* 46 */
  C_ATOM_UNOP_OPN,
  /* 47 */
  C_OPN_PAREN_OR_BRACKET,
  /* 48 */
  C_BINOP_OR_OPN_OR_TILDA,
  /* 49 */
  C_ATOM_UN_BINOP_OPN_CLO,
  /* 50 */
  C_SEEN_MACH_NAME,
  /* 51 */
  C_OP_OUTPUT_PARAM,
  /* 52 */
  C_RETURNS,
  /* 53 */
  C_EQU_RET_OPPAR_COMMA,
  /* 54 */
  C_AT_TOPLEVEL,
  /* 55 */
  C_EQUALS,
  /* 56 */
  C_OR_END,
  /* 57 */
  C_ELSEIF_ELSE_END,
  /* 58 */
  C_WHEN_ELSE_END,
  /* 59 */
  C_OF,
  /* 60 */
  C_EITHER,
  /* 61 */
  C_BE,
  /* 62 */
  C_IDENT_IN,
  /* 63 */
  C_LET_VALUATION,
  /* 64 */
  C_DO,
  /* 65 */
  C_INVARIANT,
  /* 66 */
  C_OP_INPUT_PARAM,
  /* 67 */
  C_END,
  /* 68 */
  C_IN,
  /* 69 */
  C_VARIANT,
  /* 70 */
  C_MACH_PARAM,
  /* 71 */
  C_CONSTANTS_CL,
  /* 72 */
  C_HCONSTANTS_CL,
  /* 73 */
  C_VARIABLES_CL,
  /* 74 */
  C_HVARIABLES_CL,
  /* 75 */
  C_SETS_CL,
  /* 76 */
  C_SET_NAME,
  /* 77 */
  C_CONSTRAINTS_CL,
  /* 78 */
  C_PROPERTIES_CL,
  /* 79 */
  C_INVARIANT_CL,
  /* 80 */
  C_ASSERTION_CL,
  /* 81 */
  C_INITIALISATION_CL,
  /* 82 */
  C_VALUES_CL,
  /* 83 */
  C_IMPORTS_CL,
  /* 84 */
  C_OPERATIONS_CL,
  /* 85 */
  C_SEES_CL,
  /* 86 */
  C_PROMOTES_CL,
  /* 87 */
  C_EXTENDS_CL,
  /* 88 */
  C_USES_CL,
  /* 89 */
  C_INCLUDES_CL,
  /* 90 */
  C_ABSTR_NAME,
  /* 91 */
  C_BCOMP_SUCCESS,
  /* 92 */
  C_LOCALISATION_OF_CLAUSE,
  /* 93 */
  C_REFINES_CL,
  /* 94 */
  C_LOADING_BETREE, /* %s betree name */
  /* 95 */
  C_SAVING_BETREE, /* %s betree name */
  /* 96 */
  C_DUMPING_HTML_BETREE,
  /* 97 */
  C_SYNTAX_ANALYSIS, /* %s betree name */
  /* 98 */
  C_SEMANTIC_ANALYSIS, /* %s betree name */
  /* 99 */
  C_IT_IS_AN_IMPLEMENTATION,
  /* 100*/
  C_IT_IS_A_REFINEMENT,
  /* 101*/
  C_NO_TYPE_GIVEN,
  /* 102 */
  C_USING_PATH_XX_FOR_COMPONENT, /* %s path %s compon %s compon */
  /* 103 */
  C_B0_CHECK, /* %s betree name */
  /* 104 */
  C_REMOVING_PREV_HTML,
  /* 105 */
  C_CREATING_HTML,
  /* 106 */
  C_DEP_ANALYSIS, /* %s betree name */
  /* 107 */
  C_USING_PATH_XX_FOR_FILE_DEF, /* %s path %s compon %s compon */
  /* 108 */
  C_LOCAL_QUANTIFIER,
  /* 109 */
  C_NO_VISIBILITY,
  /* 110 */
  C_ABSTRACT_SET_LOST, /* %s set name */
  /* 111 */
  C_CREATING_REFINEMENT, /* %s ref name %s abs name */
  /* 112 */
  C_CREATING_IMPLEMENTATION, /* %s imp name %s abs name */
  /* 113 */
  C_COMPONENT_CHECKSUM, /* %s file name %s checksum %s checksum */
  /* 114 */
  C_CLAUSE_CHECKSUM, /* %s clause name %s checksum */
  /* 115 */
  C_FORMULA,
  /* 116 */
  C_OP_PRECOND_CHECKSUM, /* %s op_name %s checksum */
  /* 117 */
  C_OP_BODY_CHECKSUM, /* %s op_name %s checksum */
  /* 118 */
  C_LOCAL_OPERATIONS_CL,
  /* 119 */
  C_NO_SUCH_FILE_IN_PROJECT,
  /* 120 */
  C_IDENT_IN_MULTIPLE_AFFECT,
  /* 121 */
  C_REDEFINITION_OF_MININT_VALUE, /* value */
  /* 122 */
  C_REDEFINITION_OF_MAXINT_VALUE, /* value */
  /* 123 */
  C_DEFINITION_NAME,
  /* 124 */
  C_OR_ELSE_END
  /* 125 */,
  C_INV_TOKEN_IN_DEF,
  /* 126 */
  C_EQUALS_or_BELONGS,
  /* 127 */
  C_BELONGS,
  /* 128 */
  C_TYPING_PREDICAT ,
  /* 129 */
  C_WITNESS_VALUATION ,
  /* 130 */
  C_TO,
  /* 131 */
  C_PRECOND,

} T_catalog_msg ;

// Obtention d'un message du catalogue
extern const char *get_catalog(T_catalog_msg catalog_msg) ;

#endif /* _C_CATA_H_ */


