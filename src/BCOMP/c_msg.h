/******************************* CLEARSY **************************************
* Fichier : $Id: c_msg.h,v 2.0 2002-11-25 16:58:56 lv Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Compilateur B
*					Interface de production de messages d'erreurs et de
*					warnings
*
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
#ifndef _C_MSG_H_
#define _C_MSG_H_

/* Niveaux d'erreur */
typedef enum
{
  FATAL_ERROR = 0,	   	/* arreter tout de suite 	  */
  CAN_CONTINUE,			/* l'execution peut continuer */
  MULTI_LINE				/* message multi-ligne 		  */
} T_error_level ;

/* Niveaux de warning */
typedef enum
{
  BASE_WARNING = 0,	   	/* warning important	*/
  EXTENDED_WARNING		/* warning pousse		*/
} T_warning_level ;

/* Les messages d'erreur */
typedef enum
{
  /* 00 */
  E_PARSE_ERROR,		/* %c */
  /* 01 */
  E_NO_SUCH_FILE,		/* %s (file name), %s ("errno")*/
  /* 02 */
  E_EOF_INSIDE_STRING,
  /* 03 */
  E_START_OF_STRING,
  /* 04 */
  E_NEW_LINE_INSIDE_STRING,
  /* 05 */
  E_TOKEN_PARSE_ERROR,	/* %s */
  /* 06 */
  E_NO_INPUT_NAME_GIVEN,
  /* 07 */
  E_INVALID_COMPONENT_TYPE,
  /* 08 */
  E_BAD_COMPONENT_NAME, /* %s (file name) %s (component name) */
  /* 09 */
  E_COMPONENT_WITHOUT_END, /* %s (component type) */
  /* 10 */
  E_MACH_INVALID_CLAUSE, /* %s (lex name) */
  /* 11 */
  E_OP_NAME_OR_PARAM_EXPECTED, /* %s (lex name) */
  /* 12 */
  E_UNEXPECTED_EOF,
  /* 13 */
  E_EMPTY_FILE,
  /* 14 */
  E_CONFUSED, /* %d (nb of errors) */
  /* 15 */
  E_BAD_COMPONENT_SUFFIX, /* %s (machine type) %s (suffix) */
  /* 16 */
  E_UNUSED_MESSAGE_16,
  /* 17 */
  E_MACRO_DEFINITION, /* %s (macro name) %d (macro args) */
  /* 18 */
  E_MACRO_INVALID_ARGS, /* %d (macro args) %s (macro name) */
  /* 19 */
  E_ERROR_PARSING_ATOMIC_SUBST, /* %s (ident name) */
  /* 20 */
  E_UNUSED_MESSAGE_20,
  /* 21 */
  E_UNUSED_MESSAGE_21,
  /* 22 */
  E_REFINES_FORBIDDEN_IN_SPEC,
  /* 23 */
  E_INCLUDES_FORBIDDEN_IN_IMP,
  /* 24 */
  E_IMPORTS_FORBIDDEN_IN_SPEC,
  /* 25 */
  E_IMPORTS_FORBIDDEN_IN_REF,
  /* 26 */
  E_NO_OUTPUT_NAME_GIVEN,
  /* 27 */
  E_BAD_MAGIC,
  /* 28 */
  E_CYCLE_IN_DEFINITIONS,
  /* 29 */
  E_OUT_OF_MEMORY, /* %d nb bytes */
  /* 30 */
  E_IO_ERROR, /* (%s error) */
  /* 31 */
  E_PART_OF_DEF_CYCLE, /* %s (def name) */
  /* 32 */
  E_NAME_CLASH_IN_DEF_FPARAMS,/* %s (def name) %s (param name) */
  /* 33 */
  E_PROBLEM_READING_FILE,/* %s (file name) %s (errno) */
  /* 34 */
  E_PROBLEM_WRITING_FILE,/* %s (file name) %s (errno) */
  /* 35 */
  E_EITHER_EXPECTED,
  /* 36 */
  E_OR_EXPECTED,
  /* 37 */
  E_PROBLEM_CLOSING_FILE,/* %s (file name) %s (errno) */
  /* 38 */
  E_FILE_NAME_HAS_NO_SUFFIX,/* %s (file name) */
  /* 39 */
  E_CLO_DOES_NOT_MATCH_OPN, /* %s (clo name) */
  /* 40 */
  E_OPN_LOCALISATION, /* %s (opn name) */
  /* 41 */
  E_UNEXPECTED_BINOP, /* %s (binop name) */
  /* 42 */
  E_NOT_AN_EXPRESSION, /* %s (formula type) */
  /* 43 */
  E_MISALIGNED_ADDRESS, /* %d (address) */
  /* 44 */
  E_BAD_DOT_CONSTRUCT,
  /* 45 */
  E_BAD_DOT_EXPLAIN,
  /* 46 */
  E_BPRED_PRED1_NOT_A_PRED, /* (class name) */
  /* 47 */
  E_BPRED_PRED2_NOT_A_PRED, /* (class name) */
  /* 48 */
  E_VAR_FORBIDDEN_IN_SPEC,
  /* 49 */
  E_WHILE_FORBIDDEN_IN_SPEC,
  /* 50 */
  E_LET_FORBIDDEN_IN_IMP,
  /* 51 */
  E_SELECT_FORBIDDEN_IN_IMP,
  /* 52 */
  E_ANY_FORBIDDEN_IN_IMP,
  /* 53 */
  E_CHOICE_FORBIDDEN_IN_IMP,
  /* 54 */
  E_PRE_FORBIDDEN_IN_IMP,
  /* 55 */
  E_SEQ_FORBIDDEN_IN_SPEC,
  /* 56 */
  E_WHILE_FORBIDDEN_IN_REF,
  /* 57 */
  E_LR_STATE_DOES_NOT_HAVE_A_SUCCESSOR, /* %d lr_state */
  /* 58 */
  E_LEXEM_WITHOUT_ASSOC,
  /* 59 */
  E_ATOM_TYPE_NOT_YET_IMPLEMENTED, /* %s atom type %s atom ascii */
  /* 60 */
  E_BINOP_TYPE_NOT_YET_IMPLEMENTED, /* %s binop type %s binop ascii */
  /* 61 */
  E_UNOP_TYPE_NOT_YET_IMPLEMENTED, /* %s unop ascii */
  /* 62 */
  E_PARENTH_COMPULSORY_IN_FUNCTION_CALL,
  /* 63 */
  E_WHILE_PARSING_OPERATION_HEADER,
  /* 64 */
  E_CL1_IMPLIES_CL2,
  /* 65 */
  E_IDENT_CLASH, /* %s n1 %s n1type %s n2 %s n2type */
  /* 66 */
  E_IDENT_CLASH_OTHER_LOCATION,
  /* 67 */
  E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP,
  /* 68 */
  E_ABSTRACT_VARIABLES_FORBIDDEN_IN_IMP,
  /* 69 */
  E_CONSTRAINTS_FORBIDDEN_IN_REF,
  /* 70 */
  E_CONSTRAINTS_FORBIDDEN_IN_IMP,
  /* 71 */
  E_COMPONENT_HAS_NO_REFINES_CLAUSE,
  /* 72 */
  E_USES_FORBIDDEN_IN_REF,
  /* 73 */
  E_USES_FORBIDDEN_IN_IMP,
  /* 74 */
  E_VALUES_FORBIDDEN_IN_REF,
  /* 75 */
  E_VALUES_FORBIDDEN_IN_SPEC,
  /* 76 */
  E_REQUIRED_COMPONENT_MISSING, /* %s component name */
  /* 77 */
  E_REF_DOES_NOT_DUPLICATE_SEES, /* %s %s seen name, %s abstra name */
  /* 78 */
  E_SEES_LOCATION, /* %s seen name */
  /* 79 */
  E_REQ_COMP_NOT_A_SPEC,
  /* 80 */
  E_REF_COMP_NOT_A_SPEC,
  /* 81 */
  E_REF_COMP_NEITHER_SPEC_NOR_REF,
  /* 82 */
  E_PARAM_N_IS_NOT_REFINED, /* %d param number, %d param name */
  /* 83 */
  E_PARAM_N_IS_NOT_SPECIFIED, /* %d param number, %s param name */
  /* 84 */
  E_LOCATION_OF_ABSTRACT_PARAMS,
  /* 85 */
  E_LOCATION_OF_PARAM_N_IN_ABSTRACTION, /* param number */
  /* 86 */
  E_PARAM_N_DIFFERS, /* %d pnum, %s pname, %d pnum, %s pname */
  /* 87 */
  E_CAN_NOT_IMPORT_A_MACHINE_THAT_USES, /* machine name */
  /* 88 */
  E_CAN_NOT_EXTEND_A_MACHINE_THAT_USES, /* machine name */
  /* 89 */
  E_CAN_NOT_SEE_A_MACHINE_THAT_USES, /* machine name */
  /* 90 */
  E_CAN_NOT_REFINE_A_MACHINE_THAT_USES, /* machine name */
  /* 91 */
  E_OPERATION_IS_NOT_SPECIFIED, /* %s op name %s spec name */
  /* 92 */
  E_OPERATION_IS_NOT_REFINED, /* %s op name %s comp name */
  /* 93 */
  E_PARALLEL_FORBIDDEN_IN_IMP,
  /* 94 */
  E_BECOMES_SUCH_THAT_FORBIDDEN_IN_IMP,
  /* 95 */
  E_BECOMES_MEMBER_OF_FORBIDDEN_IN_IMP,
  /* 96 */
  E_SPEC_SHOULD_HAVE_A_CONSTRAINTS_CL,
  /* 97 */
  E_SPEC_SHOULD_NOT_HAVE_A_CONSTRAINTS_CL,
  /* 98 */
  E_BEFORE_ONLY_IN_WHILE_AND_BEC_SUCH_THAT,
  /* 99 */
  E_COMPONENT_CAN_NOT_REFINE_ITSELF,
  /* 100*/
  E_COMPONENT_MULTIPLY_REFERENCED, /* %s component name */
  /* 101*/
  E_MULTIPLE_IMPORTATION_OF_COMPONENT, /* %s component name */
  /* 102*/
  E_LOCATION_OF_COMPONENT_IMPORT, /* %s component name */
  /* 103*/
  E_UNDECLARED_IDENT, /* %s ident name */
  /* 104*/
  E_NO_TYPE_FOR_IDENT, /* %s ident type %s variable name */
  /* 105*/
  E_LOCATION_OF_OP_SPEC,
  /* 106*/
  E_ILLEGAL_TYPE_FOR_IDENT, /* %s type %s ident type %s ident name */
  /* 107*/
  E_LOCATION_OF_IDENT_TYPE, /* %s type ident name */
  /* 108*/
  E_UNEXPECTED_ARGUMENT_IN_EQPRED,
  /* 109*/
  E_RHS_OF_BELONGS_MUST_BE_A_SET, /* %s type */
  /* 110*/
  E_UNBALANCED_AFFECTATION, /* %d nb_ident %d nb_values */
  /* 111*/
  E_WRONG_NUMBER_OF_TYPES_GIVEN, /* %s type name %d nb ident */
  /* 112*/
  E_OP_PARAM_NOT_SPECIFIED, /* %s ident_type %s ident_type */
  /* 113*/
  E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM,
  /* 114*/
  E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM2, /* %s id name %s id type */
  /* 115*/
  E_NON_VISIBLE_ENTITY, /* %s name %s rights */
  /* 116*/
  E_NON_VISIBLE_ENTITY_DECLARATION, /* %s name */
  /* 117*/
  E_INCOMPATIBLE_TYPES, /* %s prev_type %s new_type */
  /* 118*/
  E_WRONG_NUMBER_OF_EFFECTIVE_IN_PARAMS, /* %s op %d eff %d formal */
  /* 119*/
  E_WRONG_NUMBER_OF_EFFECTIVE_OUT_PARAMS, /* %s op %d eff %d fo */
  /* 120*/
  E_LOCATION_OF_ITEM_DEF, /* %s name */
  /* 121*/
  E_CALLED_ITEM_IS_NOT_AN_OPERATION, /* %s op_name */
  /* 122*/
  E_ACCESS_UNTYPED_IDENT, /* %s ident name */
  /* 123*/
  E_RHS_OF_BECOMES_MEMBER_OF_MUST_BE_A_SET, /* %s type */
  /* 124*/
  E_UNBALANCED_BECOMES_MEMBER_OF, /* %d nb_ident %d nb_sets */
  /* 125*/
  E_ITEM_OF_CARTESIAN_PROD_IS_NOT_A_SET,
  /* 126*/
  E_UNCOMPATIBLE_EXTENSIVE_ITEM_TYPE, /* %s cur_type %s ref_type */
  /* 127*/
  E_INVALID_INTERVAL,
  /* 128*/
  E_OPERAND_SHOULD_BE_AN_INTEGER, /* %s type_name */
  /* 129*/
  E_OPERAND_SHOULD_BE_AN_INTEGER_OR_A_SET, /* %s type_name */
  /* 130*/
  E_OPERAND_SHOULD_BE_A_SET, /* %s type_name */
  /* 131*/
  E_INVALID_OP_CALL, /* %s op_name */
  /* 132*/
  E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND, /* %s op name %d nb_oper */
  /* 133*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET, /* %s op name %s oper type */
  /* 134*/
  E_UNCOMPATIBLE_SET_OPERAND, /* %s type %s ref_type */
  /* 135*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_SET, /* %s op name %s oper type */
  /* 136*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ, /* %s op name %s oper type */
  /* 137*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ, /* %s op name %s oper type */
  /* 138*/
  E_OPERAND_OF_SET_RELATION_IS_NOT_A_SET, /* %s type name */
  /* 139*/
  E_OPERAND_OF_CONVERSE_IS_NOT_A_RELATION, /* %s type name */
  /* 140*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION, /* %s op name %s oper type */
  /* 141*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A, /* %s op name %s oper type */
  /* 142*/
  E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION, /* %s op name %s oper type */
  /* 143*/
  E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A, /* %s op name %s oper type */
  /* 144*/
  E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER, /* %s op name %s oper type */
  /* 145*/
  E_BUILTIN_OP_NEEDS_TWO_OPERANDS, /* %s op name %d nb oper */
  /* 146 */
  E_OPERAND_OF_COMPOSITION_SHOULD_BE_A_RELATION, /* %s oper type */
  /* 147 */
  E_OPERANDS_OF_COMPOSITION_SHOULD_HAVE_MATCHING_TYPES, /* %s t1 %s t2 */
  /* 148 */
  E_LOCATION_OF_BOGUS_COMPOSITION_OPERAND,
  /* 149 */
  E_FIRST_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER, /* %s type */
  /* 150 */
  E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER, /* %s type */
  /* 151 */
  E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_SET, /* %s type */
  /* 152 */
  E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_SET, /* %s type */
  /* 153*/
  E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET, /* %s op name %s oper type */
  /* 154*/
  E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET, /* %s op name %s oper type */
  /* 155*/
  E_VARIANT_IS_NOT_AN_INTEGER, /* %s variant type */
  /* 156*/
  E_IDENT_NOT_TYPED_IN_BECOMES_SUCH_THAT_PRED, /* %s ident_name */
  /* 157*/
  E_IDENT_IS_NOT_AN_ARRAY, /* %s ident_name %s given type */
  /* 158*/
  E_ITEM_IS_NOT_AN_ARRAY, /* %s given type */
  /* 159*/
  E_BAD_INDEXES_FOR_ARRAY, /* %s idx type %s array type */
  /* 160*/
  E_UNABLE_TO_FIND_CONTEXT_FOR_IDENT, /* %s ident_name %s ident_type*/
  /* 161*/
  E_EXPR_INCOMPATIBLE_TYPES, /* %s prev_type %s new_type */
  /* 162*/
  E_VARIABLE_IS_ASSIGNED_IN_PARRALLEL, /* %s variable */
  /* 163*/
  E_LOCATION_OF_PREVIOUS_ASSIGN, /* %s variable */
  /* 164*/
  E_LHS_OF_LET_VALUATION_MUST_BE_A_LOC_VAR, /* %s variable */
  /* 165*/
  E_MULTIPLE_TYPE_IN_LET_VALUATION, /* %s variable */
  /* 166*/
  E_INCOMPATIBLE_TYPES_IN_PRED, /* %s lhs type %s rhs type */
  /* 167*/
  E_INCOMPATIBLE_TYPES_IN_EXPR, /* %s type %s first_type */
  /* 168*/
  E_LHS_OF_BINOP_SHOULD_BE_A_SEQ, /* %s type */
  /* 169*/
  E_RHS_OF_BINOP_SHOULD_BE_A_SEQ, /* %s type */
  /* 170*/
  E_ILLEGAL_TYPE_FOR_ITEM_TO_ADD_IN_SEQ, /* %s itype %s stype */
  /* 171*/
  E_OPERAND_SHOULD_BE_A_SEQ, /* %s type */
  /* 172*/
  E_UNKNOWN_OPERATION_CAN_NOT_BE_PROMOTED, /* %s op_name */
  /* 173*/
  E_OP_PARAM_NOT_REFINED, /* %s ident_type %s ident_type */
  /* 174*/
  E_FIRST_OPERAND_OF_IMAGE_SHOULD_BE_A_RELATION, /* %s type */
  /* 175*/
  E_SECOND_OPERAND_OF_IMAGE_SHOULD_BE_A_SUBSET_OF, /* %s set %s type*/
  /* 176*/
  E_UNABLE_TO_FIND_LINK_BETWEEN_COMPONENTS, /* %s name1 %s name2 */
  /* 177*/
  E_INVALID_VALUATION_OF_ABSTRACT_SET, /* %s abs set name */
  /* 178*/
  E_ANY_DATA_CAN_NOT_BE_MODIFIED, /* %s name */
  /* 179*/
  E_NO_CLOSE_PAREN_MATCHING_OPEN_PAREN,
  /* 180*/
  E_OPEN_PAREN_LOCATION,
  /* 181*/
  E_VAR_OF_LAMBDA_EXPR_NOT_TYPED, /* %s var name */
  /* 182*/
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER, /* %s op name %s oper type */
  /* 183*/
  E_FIRST_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_SET, /* %s type */
  /* 184*/
  E_SECOND_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_RELATION, /* %s type */
  /* 185*/
  E_FIRST_OPERAND_OF_CORESTRICT_IS_NOT_A_RELATION, /* %s type */
  /* 186*/
  E_SECOND_OPERAND_OF_CORESTRICT_IS_NOT_A_SET, /* %s type */
  /* 187*/
  E_PRAGMA_WITHOUT_OPEN_PAREN,
  /* 188*/
  E_EOF_INSIDE_PRAGMA,
  /* 189*/
  E_PRAGMA_WITH_NO_NAME,
  /* 190*/
  E_SECOND_OPERAND_OF_DPROD_IS_NOT_A_RELATION, /* %s type */
  /* 191*/
  E_FIRST_OPERAND_OF_DPROD_IS_NOT_A_RELATION, /* %s type */
  /* 192*/
  E_TRAILING_GARBAGE_AFTER_PRAGMA,
  /* 193*/
  E_WRONG_NUMBER_OF_EFFECTIVE_MACHINE_PARAMS, /* %d eff %d formal %s mach name*/
  /* 194*/
  E_INCOMPATIBLE_TYPES_FOR_MACHINE_PARAMS, /* %s etype %s ftype %s pname */
  /* 195*/
  E_OP1RELDST_INCOMPATIBLE_TYPES_OP2BASE, /*%s lhs type name %s rhs type name */
  /* 196*/
  E_IMAGE_EXPR_TYPES_GEQ_REL_TYPES, /* %d nb_expr_types %d nb_rel_types */
  /* 197*/
  E_OP2RELSRC_INCOMPATIBLE_TYPES_OP1BASE, /*%s lhs type name %s rhs type name */
  /* 198*/
  E_IN_EXTENSIVE_REC_PAREN_EXPR_EXPECTED, /* %s class_name */
  /* 199*/
  E_RECORD_LABEL_EXPECTED, /* %s class name */
  /* 200*/
  E_FIRST_OPERAND_OF_LOVERLOAD_IS_NOT_A_RELATION, /* %s type */
  /* 201*/
  E_SECOND_OPERAND_OF_LOVERLOAD_IS_NOT_A_RELATION, /* %s type */
  /* 202*/
  E_INCOMPATIBLE_INDEX_TYPE, /* %s idx type %d idx nb %s fct type */
  /* 203*/
  E_LABEL_COMPULSORY,
  /* 204*/
  E_EOF_FOUND_WHILE_PARSING_DEFINITION, /*%s def name */
  /* 205 */
  E_TYPE_SHOULD_BE_A_SET_OF_SET, /* type name */
  /* 206*/
  E_SECOND_OPERAND_OF_PPROD_IS_NOT_A_RELATION, /* %s type */
  /* 207*/
  E_FIRST_OPERAND_OF_PPROD_IS_NOT_A_RELATION, /* %s type */
  /* 208*/
  E_NO_TYPE_GIVEN_FOR_EFFECTIVE_PARAMETER,
  /* 209*/
  E_CALLED_OP_NAME_SHOULD_BE_AN_IDENT, /* %s class name */
  /* 210*/
  E_RHS_OF_VALUATION_HAS_NO_TYPE,
  /* 211*/
  E_RECORD_ITEM_WITHOUT_LABEL,
  /* 212*/
  E_LOCATION_OF_OTHER_JOKER,
  /* 213 */
  E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_RECORD, /* %s field %s type name */
  /* 214 */
  E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_PART_OF_RECORD, /* %s field %s type name */
  /* 215 */
  E_CAN_NOT_TYPE_IDENT_WITH_JOKER_RECORD_TYPE, /* %s type name */
  /* 216 */
  E_FILE_DEF_NAME_WITH_PATH, /* %s file_name */
  /* 217 */
  E_UNABLE_TO_FIND_OBJECT_AT_RANK, /* %d rank */
  /* 218 */
  E_IDENT_INVALID_FOR_KERNEL, /* %s (ident name) */
  /* 219 */
  E_NO_COMPONENT_MATCH_IN_MULTI_COMPO_FILE, /* %s compo_name %s file_name */
  /* 220 */
  E_COMPONENT_CAN_NOT_BE_IN_THIS_FILE, /* %s compo name */
  /* 221 */
  E_ONLY_ONE_COMPONENT_CAN_BE_STORED_IN_FILE, /* %s compo name %s file_name */
  /* 222 */
  E_INVALID_EXPR_WITH_EQUAL,
  /* 223 */
  E_NO_TYPE_FOR_IDENT_LOC_EXPECTED, /* %s ident_type %s ident_name %s ident_name %s clause name %s compo name */
  /* 224 */
  E_IDENT_CAN_NOT_BE_TYPED_HERE, /* %s ident_type %s ident_name %s ident_name %s clause name %s clause name */
  /* 225 */
  E_CAN_NOT_WRITE_OP, /* %s op_name */
  /* 226 */
  E_IDENT_HAS_NOT_BEEN_VALUATED, /* %s ident_type %s ident_name %s mach name*/
  /* 227 */
  E_LOCATION_OF_VALUES_CLAUSE,
  /* 228 */
  E_IDENT_HAS_NOT_BEEN_INITIALISED, /* %s ident_type %s ident_name %s mach name*/
  /* 229 */
  E_LOCATION_OF_INITIALISATION_CLAUSE,
  /* 230 */
  E_INVALID_IDENTIFIER_WITH_DOLLAR, /* %s ident_name */
  /* 231 */
  E_GARBAGE_BEFORE_COMPONENT,
  /* 232 */
  E_MISSING_DOT, /* %s expr name */
  /* 233 */
  E_SET_FORMAL_PARAMETERS_CAN_NOT_BE_AN_EMPTY_INTERVAL,
  /* 234 */
  E_DEFINITION_CAN_NOT_REWRITE_ITSELF, /* %s def_name */
  /* 235 */
  E_UNEXPECTED_NODE_TYPE, /* %d type %s name */
  /* 236 */
  E_CAN_NOT_CREATE_SYNTAXIC_SKEL_REF_PROMOTES, /* %s compo_name */
  /* 237 */
  E_CONSTANTS_SHOULD_NOT_BE_RENAMED, /*%s name %s type %s machine*/
  /* 238 */
  E_VARIABLES_SHOULD_BE_RENAMED, /*%s ident_name %s ident_type %s machine def name*/
  /* 239 */
  E_CAN_NOT_REFINES, /*%s ident_type %s ident_name %s ident_type*/
  /* 240 */
  E_ENUMERATED_IDENT_DIFFER, /* %s SET's name %s glued val %s original val*/
  /* 241 */
  E_ENUMERATED_IDENT_EXCESS, /*%s SET's name %s execess val */
  /* 242 */
  E_ENUMERATED_IDENT_MISSING,/*%s SET's name %s missing value*/
  /* 243 */
  E_ITEM_IS_NOT_ALLOWED_IN_AN_IMPLEMENTATION, /* %s class_name */
  /* 244 */
  E_RECORD_IS_NOT_A_B0_RECORD, /* %s class_name */
  /* 245 */
  E_B0_INVALID_ARRAY_EXPRESSION, /* %s class_name (array access) */
  /* 246 */
  E_B0_INDEX_MUST_BE_A_TERM, /* table_access */
  /* 247 */
  E_B0_INVALID_BUILTIN_FOR_RANGE, /* %s class_name  */
  /* 248 */
  E_B0_INTERVAL_BOUND_MUST_BE_ARITH_EXPRESSION, /* %s class_name */
  /* 249 */
  E_B0_INVALID_EFFECTIVE_PARAMETERS_IN_OP_CALL, /* %s op_name %s class_name (record_access) */
  /* 250 */
  E_B0_INVALID_RANGE_FOR_ARRAY_AGGREGATE, /* %s class_name  */
  /* 251 */
  E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE, /* %s class_name  */
  /* 252 */
  E_B0_INVALID_INDEX_FOR_MAPLET, /* %s class_name  */
  /* 253 */
  E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION, /* %s class_name */
  /* 254 */
  E_B0_PARAMETER_MUST_BE_A_BOOL_EXPRESSION, /* %s class_name  */
  /* 255 */
  E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION, /* %s class_name  */
  /* 256 */
  E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM, /* %s class_name  */
  /* 257 */
  E_B0_INVALID_VALUE_FOR_MAPLET, /* %s class_name  */
  /* 258 */
  E_B0_INVALID_MAPLET, /*  */
  /* 259 */
  E_B0_INVALID_TERM, /* %s class_name */
  /* 260 */
  E_B0_INVALID_SIMPLE_TERM, /* %s class_name */
  /* 261 */
  E_B0_INVALID_EFFECTIVE_PARAMETER, /*  */
  /* 262 */
  E_B0_INVALID_RECORD_ITEM, /*   */
  /* 263 */
  E_B0_INVALID_CASE_SELECTOR, /* %s class_name   */
  /* 264 */
  E_B0_INVALID_SIMULTANEOUS_AFFECTATION,
  /* 265 */
  E_B0_INVALID_LHS_IN_AFFECTATION	, /* %s class_name   */
  /* 266 */
  E_B0_INVALID_RHS_IN_AFFECTATION	, /* %s class_name   */
  /* 267 */
  E_B0_INVALID_IDENT_TYPE_FOR_FORMAL_PARAMETER, /* %s ident_type_name  , %s name */
  /* 268 */
  E_B0_INVALID_DEST_SET_FOR_TOTAL_FUNCTION	, /* %s class_name   */
  /* 269 */
  E_B0_INVALID_RELATION_FOR_CONCRETE_TYPE	, /* %s class_name   */
  /* 270 */
  E_B0_INVALID_SRC_SET_FOR_TOTAL_FUNCTION	, /* %s class_name   */
  /* 271 */
  E_B0_INVALID_SIMPLE_SET	, /* %s class_name   */
  /* 272 */
  E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT	, /* %s class_name   */
  /* 273 */
  E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE	, /* %s class_name , %s name  */
  /* 274 */
  E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER	, /* %s class_name , %s name  */
  /* 275 */
  E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER	, /* %s class_name , %s name  */
  /* 276 */
  E_B0_INVALID_TYPING_PREDICATE	, /*    */
  /* 277 */
  E_B0_INVALID_IDENT_TYPE_FOR_ARRAY_INDEX, /* %s ident_type_name  , %s name */
  /* 278 */
  E_B0_SCALAR_EXPECTED, /* %s class_name */
  /* 279 */
  E_B0_INVALID_TYPING_EXPR_FOR_RECORD_ITEM	, /* %s class_name   */
  /* 280 */
  E_B0_INVALID_TYPE	, /* %s ident_type_name  , %s name */
  /* 281 */
  E_B0_INVALID_TYPE_FOR_RECORD_LABEL	, /*    */
  /* 282 */
  E_B0_INVALID_EXPR_FOR_INSTANCIATION	, /* %s class_name   */
  /* 283 */
  E_B0_INVALID_INDEX_FOR_ARRAY_VALUATION	, /* %s class_name (literal scalar expected)  */
  /* 284 */
  E_B0_INVALID_IDENT_TYPE_FOR_INSTANCIATION	, /* %s ident_type_name  , %s name */
  /* 285 */
  E_B0_INVALID_IDENT_TYPE_FOR_VALUATION	, /* %s ident_type_name  , %s name */
  /* 286 */
  E_B0_INVALID_EXPR_FOR_VALUATION	, /* %s class_name , %s ident_type_name, %s ident_name  */
  /* 287 */
  E_B0_INTERVAL_BOUND_MUST_BE_LITERAL_INTEGER, /* %s class_name */
  /* 288 */
  E_B0_INVALID_IDENT_TYPE_FOR_CONCRETE_VALUE, /* %s ident_type_name  , %s name */
  /* 289 */
  E_B0_IDENTIFIER_ALREADY_VALUED, /* %s ident_type_name  , %s name */
  /* 290 */
  E_COMPO_CLASH, /* %s compo_name */
  /* 291 */
  E_UNEXPECTED_TYPE_CASE_BRANCH, /*%s class_name*/
  /* 292 */
  E_BRANCH_TYPE_DIFFER_FROM_SELECTOR_TYPE, /*%s branch type, %s selector type*/
  /* 293 */
  E_BRANCH_VALUE_ALREADY_EXIST,
  /* 294 */
  E_INVALID_RENAMED_OP_IN_SPEC /* %s renop name */,
  /* 295 */
  E_LHS_OF_LET_SUBSTITUTION_CAN_NOT_BE_A_LOC_VAR, /*%s variable */
  /* 296 */
  E_INVALID_UNIVERSAL_PREDICATE,
  /* 297 */
  E_BEFORE_ONLY_IF_VAR_PART_OF_BECOMES_SUCH_THAT, /* %s ident name */
  /* 298 */
  E_BEFORE_ONLY_IF_VAR_OF_WHILE_SPECIFIED_IN_ABSTRACTION, /*  %s ident name %s mach name %s abstraction_name */
  /* 299 */
  E_B0_FORMAL_SET_PARAMETERS_CAN_NOT_BE_TYPED, /*%s ident name*/
  /* 300 */
  E_BEFORE_ONLY_IN_INVARIANT_OF_WHILE, /* %s ident name */
  /* 301 */
  E_B0_MAPLET_ALREADY_EXISTS,
  /* 302 */
  E_B0_LACKS_OF_MAPLETS_FOR_CURRENT_ARRAY,
  /* 303 */
  E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS,
  /* 304 */
  E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_FORMAL_AND_EFFECTIVE_PARAM,
  /* 305 */
  E_MACHINE_MUST_INCLUDE_USED_MACHINE, /*(%s M1) (%s M2) (%s M3) (%s M1) (%s M3)*/
  /* 306 */
  E_ILLEGAL_CALL_OF_INCLUDED_OPERATIONS_IN_PARALLEL_SUBST, /* %s used_machine name*/
  /* 307 */
  E_BEFORE_ONLY_IF_VAR_OF_WHILE_GLUED_OR_FROM_ABSTRACTION, /* %s var name */
  /* 308 */
  E_EOF_FOUND_WHILE_PARSING_DEF_FILE,
  /* 309 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE, /*%s op name %s oper type */
  /* 310 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_TREE, /*%s op name %s oper type*/
  /* 311 */
  E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE, /* %s ntype %s ttype */
  /* 312 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_SEQ_OF_INT, /* %s op name %s oper type */
  /* 313 */
  E_BUILTIN_OP_NEEDS_THREE_OPERANDS, /* %s op name %d nb oper */
  /* 314 */
  E_BUILTIN_OP_NEEDS_ONE_OR_THREE_OPERANDS, /*  %s op name %d nb oper */
  /* 315 */
  E_B0_DATA_IS_TYPED_WITH_WRONG_IDENT_TYPE,  /* %s ident_type_name  , %s name , %s ident_type_name, %s name  */
  /* 316 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_SEQ, /* %s op_name %s oper_type */
  /* 317 */
  E_INVALID_IMAGE_INDEX_TYPE, /* %s type_name %s expect_type*/
  /* 318 */
  E_IDENT_CLASHES_WITH_KERNEL_BUILTIN, /* %s ident_name */
  /* 319 */
  E_KERNEL_HIDDEN_CLASH, /* %s ident_name %s other_name %s other_component */
  /* 320 */
  E_CAN_NOT_TYPE_WITH_A_GENERIC_TYPE, /* %s ident_name %s type_name */
  /* 321 */
  E_LOCAL_OPERATIONS_FORBIDDEN_IN_SPEC_OR_REF,
  /* 322 */
  E_LOCAL_OPERATION_IS_NOT_IMPLEMENTED, /* %s local_op name */
  /* 323 */
  E_LITERAL_INTEGER_GREATER_THAN_MAXINT,/* %s sign %s val %s sign %s min val */
  /* 324 */
  E_LITERAL_INTEGER_LOWER_THAN_MININT, /* %s sign %s value */
  /* 325 */
  E_MININT_IS_GREATER_THAN_MAXINT, /* %s value of MININT %s value of MAXINT */
  /* 326 */
  E_B0_TOOL_RESTRICTS_INSTANCIATION_TO_LITERALS,  /* %s tool name %s */
  /* 327 */
  E_MS_LEXICAL_STATE_FILE, /* %s lexeme  %s state_file*/
  /* 328 */
  E_MS_STATE_FILE_LEXEM_WITH_NO_VALUE, /* %d lexeme  %s state_file*/
  /* 329 */
  E_MS_SYNTAXE_STATE_FILE,/* %s statefile %d line %d lexemetype %d lexemetype*/
  /* 330 */
  E_MS_ACTION_TODO_INEXISTANT, /* [%d,%d] index in the actions to do tabular */
  /* 331 */
  E_MS_ACTION_TOUPDATE_INEXISTANT, /* [%d,%d] index in the actions to update tabular */
  /* 332 */
  E_MS_UPDATE_ACTION_WITH_NULL_SIGNATURE, /* %x action state */
  /* 333 */
  E_MS_TYPE_OF_ACTION_INEXISTANT,  /* %d action type */
  /* 334 */
  E_MS_STATE_OF_ACTION_HAS_EVER_BEEN_MEMORIZED, /*%d action type %x actio state */
  /* 335 */
  E_MS_CAN_NOT_OPEN_STATE_FILE, /* %s state file name */
  /* 336 */
  E_CYCLE_IN_IMPLEMENTATION_OF_LOCAL_OP, /* %s local_op_name */
  /* 337 */
  E_LOCAL_OP_IS_PART_OF_THE_CYCLE, /* %s local_op_name */
  /* 338 */
  E_MULTIPLE_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP, /* %s machine name %s oploc name %s machine_name */
  /* 339 */
  E_PREVIOUS_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP, /* %s machine name %s oploc name*/
  /* 340 */
  E_INVALID_OUTPUT_PARAM_CONC_VAR_IS_MODIFIED_IN_LOCAL_OP_SPEC, /* %s op name %s var name %s var_name %s op_name */
  /* 341 */
  E_LOC_OF_MODIF_OF_CONC_VAR_IN_LOCAL_OP_SPEC, /* %s var_name %s op_name */
  /* 342 */
  E_IN_OP_LOCAL_OP_CALL_INVALID_IMPORTED_OUTPUT_PARAM_IS_MODIFIED_IN_IMPORTED_OP,  /* %s op name %s var name %s var name %s op_name */
  /* 343 */
  E_LOC_OF_IMPORTED_VAR_VAR_IN_IMPORTED_OP, /* %s var_name %s op_name */
  /* 344 */
  E_TOO_MANY_PARAM_IN_REF_OP, /* %s op_name */
  /* 345 */
  E_TOO_FEW_PARAM_IN_REF_OP, /* %s op_name */
 /* 346 */
  E_PARAM_NAME_DIFF_1, /* %s arg_name_in_spec %s arg_name_in_ref */
  /* 347 */
  E_PARAM_NAME_DIFF_2, /*%s op_name */
  /* 348 */
  E_DOUBLE_REF, /* %s ident_name */
  /* 349 */
  E_IDENT_INIT_VAR_LOCAL, /*got %s    expected %s */
  /* 350 */
  E_B0_INVALID_TYPING_EXPR,  /*%s ident_name %s ident_type*/
  /* 351 */
  E_B0_ABST_MACH_AND_CONC_INCL, /*%s machine name */
  /* 352 */
  E_B0_INTERVAL_TYPE_ONLY_IN_BASIC_TRANS_MACH,
  /* 353 */
  E_B0_INVALID_RANGE_FOR_ARRAY_TYPE, /*%s type name */
  /* 354 */
  E_B0_ARRAY_MINIMAL_BOUND,
  /* 355 */
  E_B0_ARRAY_MAX_BOUND,
  /* 356 */
  E_B0_INVALID_ARRAY_TYPE,
  /* 357 */
  E_LOCATION_OF_VALUATION,
  /* 358 */
  E_B0_OPERATION_MUST_BE_ABSTRACT,/* %s operation name */
  /* 359 */
  E_B0_INDICES_ORDERED,
  /* 360 */
  E_B0_INVALID_VALUATION_OF_ABSTRACT_SET, /* %s abs set name */
  /* 361 */
  E_INVALID_OP_REFINEMENT, /*%s op_name */
  /* 362 */
  E_B0_OP_MUST_CONCRETE_IN_IMPL, /* %s op_name*/
  /* 363 */
  E_B0_INVALID_EXPR,
  /* 364 */
  E_B0_NO_LITERAL_INTEGER_TYPE
  /* 365 */,
  E_B0_TYPE_DATA_EXPECTED ,
  /* 366 */
  E_B0_TYPE_INPUT_PARAM_EXPECTED,
  /* 367 */
  E_B0_LOC_VAR_MUST_INITIALISED, /* %s var_loc_name */
  /* 368 */
  E_B0_INVALID_TYPING_EXPR_FOR_LOCAL_VARIABLE, /*  %s class_name, %s name  */
  /* 367 */
  E_B0_INVALID_TYPING_EXPR_FOR_OP_OUT_PARAM, /*  %s class_name, %s name  */
  /* 370 */
  E_B0_INVALID_TYPE_DECL, /* %s ident_name*/
  /* 371 */
  E_B0_PARAM_FORBI_BOM,
  /* 372 */
  E_B0_OPERATION_MUST_BE_CONCRETE, /* %s operation name*/
  /* 373 */
  E_B0_INVALID_TYPE_VALUATION, /*%s type_name*/
  /* 374 */
  E_B0_INCOMPATIBLE_DECLARATIONS, /* %s data_name */
  /* 375 */
  E_B0_LOCATION_OF_FIRST_DECLARATION,
  /* 376 */
  E_B0_AGREGATE_ONLY_IN_VAR_VALUES,
  /* 377 */
  E_B0_INCOMPATIBLE_TYPE_FOR_PARAMS, /* %s effective_type %s formal_type %sformal_parameter_name*/
  /* 378 */
  E_B0_IDENT_CANT_BE_REFERED /*%s ident_name */,
  /* 379 */
  E_B0_REDEFINITION_OF_INLINE,
  /* 380 */
  E_B0_REDEFINITION_OF_INLINE_ALL,
  /* 381 */
  E_B0_INLINE_AND_INLINE_ALL,
  /* 382 */
  E_B0_IS_NOT_OP_FROM_IMPL, /* %s op_name %s implemenation_name*/
  /* 383 */
  E_B0_INLINE_ONLY_IN_IMPL_OR_MACH,
  /* 384 */
  E_B0_INLINE_SHOULD_PARAM,
  /* 385 */
  E_REDEF_PRAG_LIT_INT_TYPE,
  /* 386 */
  E_LIT_INT_TYPE_TAKES_ONE_PARAM,
  /* 387 */
  E_B0_ARRAY_EXPR_FORBIDDEN,
  /* 388 */
  E_B0_INVALID_IDENT,
  /* 389 */
  E_B0_INVALID_TYPE_FOR_BECOMES_MEMBER, /* %s type_name */
  /* 390 */
  E_B0_INVALID_BECOMES_MEMBER,
  /* 391 */
  E_B0_IS_NOT_OP_FROM_MCH, /* %s op_name %s spec_name*/
  /* 392 */
  E_B0_SET_VALUED_WITH_CONSTANTS_FROM_SAME_MODULE, /* %s set_nam %s constant_name*/
  /* 393 */
  E_B0_OPERATION_ONLY_ONE_OUT_PARAM, /* %s operation_name */
  /* 394 */
  E_B0_EXPR_STATIC,
  /* 395 */
  E_B0_OM_INVALID_RANGE_FOR_ARRAY_AGGREGATE,
  /* 396 */
  E_B0_INVALID_TYPE_FOR_PARAM, /* %s new_type_name , %s parameter_name, operation_name previous_type_name */
  /* 397 */
  E_B0_LIT_PRAGMA_PARAM_CONCRETE_CONSTANT,
  /* 398 */
  E_B0_LIT_PRAGMA_PARAM_INTEGER_TYPE,
  /* 399 */
  E_HEXADECIMAL_LITERAL_OVERFLOW,
  /* 400*/
  E_OPERAND_SHOULD_BE_A_REAL, /* %s type_name */
  /* 401*/
  E_OPERAND_SHOULD_BE_A_FLOAT, /* %s type_name */
  /* 402 */
  E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_FLOAT, /* %s type */
  /* 403 */
  E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_FLOAT, /* %s type */
  /* 404 */
  E_FIRST_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL, /* %s type */
  /* 405 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_OR_REAL_SET, /* %s op name %s oper type */
  /* 406 */
  E_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL, /* %s type_name */

  /* 407 */
  E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_REAL, /* %s type */
  /* 408 */
  E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_REAL, /* %s type */
  /* 409 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_REAL_SET, /* %s op name %s oper type */
  /* 410 */
  E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_REAL, /* %s op name %s oper type */
  /* 411 */
  E_OPERAND_SHOULD_BE_AN_INTEGER_A_REAL_OR_A_FLOAT, /* %s type_name */
  /* 412 */
  E_INVALID_EVENT_REF,  /* %s event_name */
  /* 413 */
  E_DOUBLE_EVENT_REF,  /* %s event_name */
  /* 414 */
  E_REFINED_EVENT_CANNOT_REF,  /* %s event_name */
  /* 415 */
  E_REFINE_AND_MERGE_EVENT,  /* %s event_name */
  /* 416 */
  E_EVENTB_INVALID_HEAD_SUBSTITUTION,
    /* 417 */
    E_EVENTB_AMBIGUOUS_WITNESS,
    /* 418 */
    E_VARIANT_CLAUSE_MANDATORY,
  /* 419 */
  E_PRAGMA_IN_DEF,
  /* 420 */
  E_BINARY_LITERAL_OVERFLOW = 420,
  /* 421 */
  E_TOTAL_RELATIONS_NOT_ALLOWED,
  /* 422 */
  E_AMBIGUOUS_FILE,
} T_error_code ;

/* Les warnings */
typedef enum
{
  /* 00 */
  W_NESTED_COMMENT,
  /* 01 */
  W_START_OF_COMMENT,
  /* 02 */
  W_EOF_INSIDE_COMMENT,
  /* 03 */
  W_TRAILING_GARBAGE,
  /* 04 */
  W_EMPTY_CLAUSE, /* %s (clause name) */
  /* 05 */
  W_IDENT_INVALID_FOR_KERNEL, /* %s (ident name) */
  /* 06 */
  W_OPEN_LEXEM_NOT_CLOSED,
  /* 07 */
  W_AMBIGOUS_MINUS,
  /* 08 */
  W_AMBIGOUS_MINUS_HINT, /* %s (bin ascii) %s (bin ascii) */
  /* 09 */
  W_EXTRA_SCOL_NOT_STRICT_B,
  /* 10 */
  W_UNIMPLEMENTABLE_TYPE_FOR_IDENT, /* %s type %s itype %s iname */
  /* 11 */
  W_EXTRA_PAREN_IN_TPRED_IGNORED,
  /* 12 */
  W_EXTRA_PAREN_IN_EXPR_IGNORED,
  /* 13 */
  W_B0_DETECTED_CYCLE_IN_INITIALISATION, /* %s ident_type_name  , %s name */
  /* 14 */
  W_B0_DETECTED_CYCLE_IN_VALUATION, /* %s ident_type_name  , %s name */
  /* 15 */
  W_USE_BEFORE_ON_VAR_NOT_MODIFIED_IN_WHILE, /* %s var name */
  /* 16 */
  W_ENVIRONMENT_VARIABLE_IS_OUT_OF_DATE, /*%s var name, %s res name*/
  /* 17 */
  W_COMMA_WILL_BE_TREATED_AS_A_MAPLET,
  /* 18 */
  W_ASSERTIONS_CLAUSE_WITHOUT_INVARIANT_CLAUSE,
  /* 19 */
  W_VARIANT_WITHOUT_NON_DIVERGENCE_RESSOURCE,
  /* 20 */
  W_REFINED_EVENT_NOT_CLOSED,
  /* 21 */
  W_PRAGMA_IN_DEF
} T_warning_code ;

#ifdef TRACE
#define get_error_msg(code) _get_error_msg(__FILE__, __LINE__, code)
#define get_warning_msg(code) _get_warning_msg(__FILE__, __LINE__, code)
extern const char *_get_error_msg(const char *file,
										   int line,
										   T_error_code error_code) ;
extern const char *_get_warning_msg(const char *file,
											 int line,
											 T_warning_code warning_code) ;
#else
extern const char *get_error_msg(T_error_code error_code) ;
extern const char *get_warning_msg(T_warning_code warning_code) ;
#endif

// Ajout d'un message d'erreur utilisateur
// Renvoie l'identifiant associe
typedef size_t T_user_error_code ;
extern T_user_error_code add_user_error_msg(const char *msg) ;

extern void reset_user_error_msg(void) ;

// Ajout d'un message d'avertissement utilisateur
// Renvoie l'identifiant associe
typedef size_t T_user_warning_code ;
extern T_user_warning_code add_user_warning_msg(const char *msg) ;

extern void reset_user_warning_msg(void) ;

// Obtention d'un message d'erreur utilisateur
#ifdef TRACE
#define get_user_error_msg(x) _get_user_error_msg(x, __FILE__, __LINE__)
extern const char *_get_user_error_msg(T_user_error_code error_code,
 												const char *file,
 												int line) ;
#else // ! TRACE
extern const char *get_user_error_msg(T_user_error_code error_code) ;
#endif // TRACE
extern const char *get_user_warning_msg(T_user_warning_code warning_code) ;

// Langue utilisaee
typedef enum
{
  ENGLISH,
  FRENCH
} T_language ;

extern T_language get_language(void) ;

#endif /* _C_MSG_H_ */

