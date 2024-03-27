/******************************* CLEARSY **************************************
* Fichier : $Id: c_lex.cpp,v 2.0 2005-04-25 10:43:55 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Lexemes
*
* Compilation  : 	*	-DDEBUG_COMMENT pour voir les commentaires
*					*	-DDUMP_LEXEMS pour dumper les lexemes avant et apres
*						expansion des definitions
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
#include "c_port.h"
RCS_ID("$Id: c_lex.cpp,v 1.58 2005-04-25 10:43:55 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Debug lexemes
static const char *debug_lex_type[] =
{
  /* 000 */
  "L_IDENT",
  /* 001 */
  "L_COMMENT",
  /* 002 */
  "L_KEPT_COMMENT",
  /* 003 */
  "L_STRING",
  /* 004 */
  "L_NUMBER",
  /* 005 */
  "L_OPEN_PAREN",
  /* 006 */
  "L_CLOSE_PAREN",
  /* 007 */
  "L_OPEN_BRACES",
  /* 008 */
  "L_CLOSE_BRACES",
  /* 009 */
  "L_OPEN_BRACKET",
  /* 010 */
  "L_CLOSE_BRACKET",
  /* 011 */
  "L_SCOL",
  /* 012 */
  "L_COMMA",
  /* 013 */
  "L_DOT",
  /* 014 */
  "L_MACHINE",
  /* 015 */
  "L_REFINEMENT",
  /* 016 */
  "L_IMPLEMENTATION",
  /* 017 */
  "L_BEGIN",
  /* 018 */
  "L_END",
  /* 019 */
  "L_SEES",
  /* 020 */
  "L_INCLUDES",
  /* 021 */
  "L_PROMOTES",
  /* 022 */
  "L_IMPORTS",
  /* 023 */
  "L_REFINES",
  /* 024 */
  "L_EXTENDS",
  /* 025 */
  "L_USES",
  /* 026 */
  "L_CONSTRAINTS",
  /* 027 */
  "L_DEFINITIONS",
  /* 028 */
  "L_INVARIANT",
  /* 029 */
  "L_ASSERTIONS",
  /* 030 */
  "L_VALUES",
  /* 031 */
  "L_SETS",
  /* 032 */
  "L_CONCRETE_CONSTANTS",
  /* 033 */
  "L_ABSTRACT_CONSTANTS",
  /* 034 */
  "L_PROPERTIES",
  /* 035 */
  "L_CONCRETE_VARIABLES",
  /* 036 */
  "L_ABSTRACT_VARIABLES",
  /* 037 */
  "L_INITIALISATION",
  /* 038 */
  "L_OPERATIONS",
  /* 039 */
  "L_BOOL",
  /* 040 */
  "L_MOD",
  /* 041 */
  "L_SUCC",
  /* 042 */
  "L_PRED",
  /* 043 */
  "L_MAX",
  /* 044 */
  "L_MIN",
  /* 045 */
  "L_CARD",
  /* 046 */
  "L_SIGMA",
  /* 047 */
  "L_PI",
  /* 048 */
  "L_GUNION",
  /* 049 */
  "L_GINTER",
  /* 050 */
  "L_QUNION",
  /* 051 */
  "L_QINTER",
  /* 052 */
  "L_ID",
  /* 053 */
  "L_PRJ1",
  /* 054 */
  "L_PRJ2",
  /* 055 */
  "L_DOM",
  /* 056 */
  "L_RAN",
  /* 057 */
  "L_SIZE",
  /* 058 */
  "L_FIRST",
  /* 059 */
  "L_LAST",
  /* 060 */
  "L_FRONT",
  /* 061 */
  "L_TAIL",
  /* 062 */
  "L_REV",
  /* 063 */
  "L_L_OR",
  /* 064 */
  "L_L_NOT",
  /* 065 */
  "L_PLUS",
  /* 066 */
  "L_MINUS",
  /* 067 */
  "L_TIMES",
  /* 068 */
  "L_DIVIDES",
  /* 069 */
  "L_POWER",
  /* 070 */
  "L_UNION",
  /* 071 */
  "L_INTERSECT",
  /* 072 */
  "L_INTERVAL",
  /* 073 */
  "L_EQUAL",
  /* 074 */
  "L_REWRITES",
  /* 075 */
  "L_DIFFERENT",
  /* 076 */
  "L_LESS_THAN",
  /* 077 */
  "L_LESS_THAN_OR_EQUAL",
  /* 078 */
  "L_GREATER_THAN",
  /* 079 */
  "L_GREATER_THAN_OR_EQUAL",
  /* 080 */
  "L_AND",
  /* 081 */
  "L_VAR",
  /* 082 */
  "L_IN",
  /* 083*/
  "L_IF",
  /* 084 */
  "L_THEN",
  /* 085 */
  "L_ELSIF",
  /* 086 */
  "L_ELSE",
  /* 087 */
  "L_CASE",
  /* 088 */
  "L_EITHER",
  /* 089 */
  "L_OR",
  /* 090 */
  "L_SKIP",
  /* 091 */
  "L_PRE",
  /* 092 */
  "L_CHOICE",
  /* 093*/
  "L_SELECT",
  /* 094 */
  "L_WHEN",
  /* 095 */
  "L_WHILE",
  /* 096 */
  "L_VARIANT",
  /* 097 */
  "L_LET",
  /* 098 */
  "L_BE",
  /* 099 */
  "L_NAT",
  /* 100 */
  "L_NAT1",
  /* 101 */
  "L_INT",
  /* 102 */
  "L_TRUE",
  /* 103*/
  "L_FALSE",
  /* 104 */
  "L_MAXINT",
  /* 105 */
  "L_MININT",
  /* 106 */
  "L_EMPTY_SET",
  /* 107 */
  "L_INTEGER",
  /* 108 */
  "L_NATURAL",
  /* 109 */
  "L_NATURAL1",
  /* 110 */
  "L_BOOL_TYPE",
  /* 111 */
  "L_STRING_TYPE",
  /* 112 */
  "L_POW",
  /* 113*/
  "L_POW1",
  /* 114 */
  "L_FIN",
  /* 115 */
  "L_FIN1",
  /* 116 */
  "L_RETURNS",
  /* 117 */
  "L_MAPLET",
  /* 118 */
  "L_FORALL",
  /* 119 */
  "L_EXISTS",
  /* 120 */
  "L_IMPLIES",
  /* 121 */
  "L_EQUIVALENT",
  /* 122 */
  "L_BELONGS",
  /* 123*/
  "L_NOT_BELONGS",
  /* 124 */
  "L_INCLUDED",
  /* 125 */
  "L_STRICT_INCLUDED",
  /* 126 */
  "L_NOT_INCLUDED",
  /* 127 */
  "L_NOT_STRICT_INCLUDED",
  /* 128 */
  "L_DIRECT_PRODUCT",
  /* 129 */
  "L_PARALLEL",
  /* 130 */
  "L_RESTRICTION",
  /* 131 */
  "L_ANTIRESTRICTION",
  /* 132 */
  "L_CORESTRICTION",
  /* 133 */
  "L_ANTICORESTRICTION",
  /* 134 */
  "L_LEFT_OVERLOAD",
  /* 135 */
  "L_PARTIAL_FUNCTION",
  /* 136 */
  "L_TOTAL_FUNCTION",
  /* 137 */
  "L_PARTIAL_INJECTION",
  /* 138 */
  "L_TOTAL_INJECTION",
  /* 139 */
  "L_PARTIAL_SURJECTION",
  /* 140 */
  "L_TOTAL_SURJECTION",
  /* 141 */
  "L_BIJECTION",
  /* 142*/
  "L_LAMBDA",
  /* 143 */
  "L_TRANS_FUNC",
  /* 144 */
  "L_TRANS_REL",
  /* 145 */
  "L_SEQUENCE",
  /* 146 */
  "L_NON_EMPTY_SEQUENCE",
  /* 147 */
  "L_INJECTIVE_SEQ",
  /* 148 */
  "L_NON_EMPTY_INJECTIVE_SEQ",
  /* 149 */
  "L_PERMUTATION",
  /* 150 */
  "L_CONCAT",
  /* 151 */
  "L_HEAD_INSERT",
  /* 152 */
  "L_TAIL_INSERT",
  /* 153 */
  "L_HEAD_RESTRICT",
  /* 154 */
  "L_TAIL_RESTRICT",
  /* 155 */
  "L_GENERALISED_CONCAT",
  /* 156 */
  "L_CONVERSE",
  /* 157 */
  "L_EMPTY_SEQ",
  /* 158 */
  "L_SET_RELATION",
  /* 159 */
  "L_PIPE",
  /* 160 */
  "L_SCOPE",
  /* 161 */
  "L_ASSERT",
  /* 162 */
  "L_OF",
  /* 163 */
  "L_DO",
  /* 164 */
  "L_ANY",
  /* 165 */
  "L_WHERE",
  /* 166 */
  "L_AFFECT",
  /* 167 */
  "L_BECOMES_MEMBER_OF",
  /* 168 */
  "L_ITERATE",
  /* 169 */
  "L_CLOSURE",
  /* 170 */
  "L_CLOSURE1",
  /* 171 */
  "L_EVL",
  /* 172 */
  "L_UMINUS",
  /* 173 */
  "L_LR_SCOL",
  /* 174 */
  "L_LR_PARALLEL",
  /* 175 */
  "L_RENAMED_IDENT",
  /* 176 */
  "L_REC",
  /* 177 */
  "L_STRUCT",
  /* 178 */
  "L_LABEL_COL",
  /* 179 */
  "L_RECORD_ACCESS",
  /* 180 */
  "L_LIB_FILE_NAME",
#ifdef COMPAT
  /* 181 */
  "L_VOID",
#endif
  /* 182 */
  "L_TREE",
  /* 183 */
  "L_BTREE",
  /* 184 */
  "L_CONST",
  /* 185 */
  "L_TOP",
  /* 186 */
  "L_SONS",
  /* 187 */
  "L_PREFIX",
  /* 188 */
  "L_POSTFIX",
  /* 189 */
  "L_SIZET",
  /* 190 */
  "L_MIRROR",
  /* 191 */
  "L_RANK",
  /* 192 */
  "L_FATHER",
  /* 193 */
  "L_SON",
  /* 194 */
  "L_SUBTREE",
  /* 195 */
  "L_ARITY",
  /* 196 */
  "L_BIN",
  /* 197 */
  "L_LEFT",
  /* 198 */
  "L_RIGHT",
  /* 199 */
  "L_INFIX",
  /* 200 */
  "L_LOCAL_OPERATIONS",
  /* 201 */
  "L_SYSTEM",
  /* 202 */
   "L_EVENTS",
   /* 203 */
   "L_WITNESS",
#ifdef BALBULETTE
  /* 201 */
  "L_BECOMES_SUCH_THAT",
  /* 202 */
  "L_MODEL",
   /* 204 */
  "L_THEOREMS",
#endif
  /* 204 */
  "L_CPP_COMMENT",
  /* 205 */
  "L_CPP_KEPT_COMMENT",
  /* 206 */
  "L_REAL_TYPE",
  /* 207 */
  "L_REAL",
  /* 208 */
  "L_FLOAT_TYPE"
  /* 209 */
  "L_JUMPIF",
  /* 210 */
  "L_LABEL",
  /* 211 */
  "L_TO",
  /* 212 */
  "L_REALOP",
  /* 213 */
  "L_FLOOR",
  /* 214 */
  "L_CEILING",
  /* 215 */
  "L_REF",
  /* 216 */
  "L_SURJ_RELATION",
  /* 217 */
  "L_TOTAL_RELATION",
  /* 218 */
  "L_TOTAL_SURJ_RELATION"
} ;


// Un lexeme est-il une clause ?
static const int lex_is_clause[] =
{
  /* 000 L_IDENT	*/					FALSE,
  /* 001 L_COMMENT	*/					FALSE,
  /* 002 L_KEPT_COMMENT	*/				FALSE,
  /* 003 L_STRING	*/					FALSE,
  /* 004 L_NUMBER	*/					FALSE,
  /* 005 L_OPEN_PAREN	*/				FALSE,
  /* 006 L_CLOSE_PAREN	*/				FALSE,
  /* 007 L_OPEN_BRACKET	*/				FALSE,
  /* 008 L_CLOSE_BRACKET */	   	    	FALSE,
  /* 009 L_OPEN_BRACKET	*/				FALSE,
  /* 010 L_CLOSE_BRACKET */				FALSE,
  /* 011 L_SCOL	*/						FALSE,
  /* 012 L_COMMA	*/					FALSE,
  /* 013 L_DOT	*/						FALSE,
  /* 014 L_MACHINE	*/				TRUE,
  /* 015 L_REFINEMENT	*/			TRUE,
  /* 016 L_IMPLEMENTATION	*/ 		TRUE,
  /* 017 L_BEGIN	*/					FALSE,
  /* 018 L_END	*/						FALSE,
  /* 019 L_SEES	*/					TRUE,
  /* 020 L_INCLUDES	*/				TRUE,
  /* 021 L_PROMOTES	*/				TRUE,
  /* 022 L_IMPORTS	*/				TRUE,
  /* 023 L_REFINES	*/				TRUE,
  /* 024 L_EXTENDS	*/				TRUE,
  /* 025 L_USES	*/					TRUE,
  /* 026 L_CONSTRAINTS	*/			TRUE,
  /* 027 L_DEFINITIONS	*/			TRUE,
  /* 028 L_INVARIANT	*/			TRUE,
  /* 029 L_ASSERTIONS	*/			TRUE,
  /* 030 L_VALUES	*/				TRUE,
  /* 031 L_SETS	*/					TRUE,
  /* 032 L_CONSTANTS	*/			TRUE,
  /* 033 L_HIDDEN_CONSTANTS	*/	   	TRUE,
  /* 034 L_PROPERTIES	*/			TRUE,
  /* 035 L_VARIABLES,	*/			TRUE,
  /* 036 L_HIDDEN_VARIABLES	*/	   	TRUE,
  /* 037 L_INITIALISATION	*/	   	TRUE,
  /* 038 L_OPERATIONS, 	*/			TRUE,
  /* 039 L_BOOL	*/						FALSE,
  /* 040 L_MOD	*/						FALSE,
  /* 041 L_SUCC	*/						FALSE,
  /* 042 L_PRED	*/						FALSE,
  /* 043 L_MAX	*/						FALSE,
  /* 044 L_MIN	*/						FALSE,
  /* 045 L_CARD	*/						FALSE,
  /* 046 L_SIGMA	*/					FALSE,
  /* 047 L_PI	*/						FALSE,
  /* 048 L_GUNION	*/					FALSE,
  /* 049 L_GINTER	*/					FALSE,
  /* 050 L_QUNION	*/					FALSE,
  /* 051 L_QINTER	*/			   		FALSE,
  /* 052 L_ID	*/						FALSE,
  /* 053 L_PRJ1	*/						FALSE,
  /* 054 L_PRJ2	*/						FALSE,
  /* 055 L_DOM	*/						FALSE,
  /* 056 L_RAN	*/						FALSE,
  /* 057 L_SIZE	*/						FALSE,
  /* 058 L_FIRST	*/					FALSE,
  /* 059 L_LAST	*/						FALSE,
  /* 060 L_FRONT	*/					FALSE,
  /* 061 L_TAIL	*/						FALSE,
  /* 062 L_REV	*/						FALSE,
  /* 063 L_L_OR	*/						FALSE,
  /* 064 L_L_NOT	*/					FALSE,
  /* 065 L_PLUS	*/						FALSE,
  /* 066 L_MINUS	*/					FALSE,
  /* 067 L_TIMES	*/					FALSE,
  /* 068 L_DIVIDES	*/					FALSE,
  /* 069 L_POWER	*/					FALSE,
  /* 070 L_UNION	*/					FALSE,
  /* 071 L_INTERSECT	*/				FALSE,
  /* 072 L_INTERVAL	*/					FALSE,
  /* 073 L_EQUAL	*/					FALSE,
  /* 074 L_REWRITES	*/					FALSE,
  /* 075 L_DIFFERENT	*/				FALSE,
  /* 076 L_LESS_THAN   */  				FALSE,
  /* 077 L_LESS_THAN_OR_EQUAL	*/ 		FALSE,
  /* 078 L_GREATER_THAN	*/ 				FALSE,
  /* 079 L_GREATER_THAN_OR_EQUAL_THAN*/ FALSE,
  /* 080 L_AND	*/ 						FALSE,
  /* 081 L_VAR	*/ 						FALSE,
  /* 082 L_IN	*/ 						FALSE,
  /* 083 L_IF	*/						FALSE,
  /* 084 L_THEN	*/						FALSE,
  /* 085 L_ELSIF	*/					FALSE,
  /* 086 L_ELSE	*/						FALSE,
  /* 087 L_CASE	*/						FALSE,
  /* 088 L_EITHER	*/					FALSE,
  /* 089 L_OR	*/						FALSE,
  /* 090 L_SKIP	*/						FALSE,
  /* 091 L_PRE	*/						FALSE,
  /* 092 L_CHOICE	*/					FALSE,
  /* 093 L_SELECT	*/					FALSE,
  /* 094 L_WHEN	*/						FALSE,
  /* 095 L_WHILE	*/					FALSE,
  /* 096 L_VARIANT	*/					FALSE,
  /* 097 L_LET	*/						FALSE,
  /* 098 L_BE	*/						FALSE,
  /* 099 L_NAT	*/						FALSE,
  /* 100 L_NAT1	*/						FALSE,
  /* 101 L_INT	*/						FALSE,
  /* 102 L_TRUE	*/						FALSE,
  /* 103 L_FALSE	*/					FALSE,
  /* 104 L_MAXINT	*/					FALSE,
  /* 105 L_MININT	*/					FALSE,
  /* 106 L_EMPTY_SET	*/				FALSE,
  /* 107 L_INTEGER	*/					FALSE,
  /* 108 L_NATURAL	*/					FALSE,
  /* 109 L_NATURAL1	*/					FALSE,
  /* 110 L_BOOL_TYPE	*/				FALSE,
  /* 111 L_STRING_TYPE	*/				FALSE,
  /* 112 L_POW	*/						FALSE,
  /* 113 L_POW1	*/						FALSE,
  /* 114 L_FIN	*/						FALSE,
  /* 115 L_FIN1	*/						FALSE,
  /* 116 L_RETURNS	*/					FALSE,
  /* 117 L_MAPLET	*/					FALSE,
  /* 118 L_FORALL	*/					FALSE,
  /* 119 L_EXISTS	*/					FALSE,
  /* 120 L_IMPLIES	*/					FALSE,
  /* 121 L_EQUIVALENT */				FALSE,
  /* 122 L_BELONGS	*/					FALSE,
  /* 123 L_NOT_BELONGS	*/				FALSE,
  /* 124 L_INCLUDED	*/					FALSE,
  /* 125 L_STRICT_INCLUDED	*/	   		FALSE,
  /* 126 L_NOT_INCLUDED	*/				FALSE,
  /* 127 L_NOT_STRICT_INCLUDED	*/ 		FALSE,
  /* 128 L_DIRECT_PRODUCT	*/	   		FALSE,
  /* 129 L_PARALLEL	*/					FALSE,
  /* 130 L_RESTRICTION	*/				FALSE,
  /* 131 L_ANTIRESTRICTION	*/			FALSE,
  /* 132 L_CORESTRICTION	*/			FALSE,
  /* 133 L_ANTICORESTRICTION	*/		FALSE,
  /* 134 L_LEFT_OVERLOAD	*/			FALSE,
  /* 135 L_PARTIAL_FUNCTION	*/			FALSE,
  /* 136 L_TOTAL_FUNCTION	*/			FALSE,
  /* 137 L_PARTIAL_INJECTION	*/		FALSE,
  /* 138 L_TOTAL_INJECTION	*/			FALSE,
  /* 139 L_PARTIAL_SURJECTION	*/		FALSE,
  /* 140 L_TOTAL_SURJECTION	*/			FALSE,
  /* 141 L_BIJECTION	*/				FALSE,
  /* 142 L_LAMBDA	*/					FALSE,
  /* 143 L_TRANS_FUNC	*/				FALSE,
  /* 144 L_TRANS_REL	*/				FALSE,
  /* 145 L_SEQUENCE	*/					FALSE,
  /* 146 L_NON_EMPTY_SEQUENCE	*/		FALSE,
  /* 147 L_INJECTIVE_SEQ	*/			FALSE,
  /* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	FALSE,
  /* 149 L_PERMUTATION	*/				FALSE,
  /* 150 L_CONCAT	*/					FALSE,
  /* 151 L_HEAD_INSERT	*/				FALSE,
  /* 152 L_TAIL_INSERT	*/				FALSE,
  /* 153 L_HEAD_RESTRICT	*/			FALSE,
  /* 154 L_TAIL_RESTRICT	*/			FALSE,
  /* 155 L_GENERALISED_CONCAT*/			FALSE,
  /* 156 L_CONVERSE	*/					FALSE,
  /* 157 L_EMPTY_SEQ	*/				FALSE,
  /* 158 L_SET_RELATION	*/				FALSE,
  /* 159 L_PIPE	*/						FALSE,
  /* 160 L_SCOPE	*/					FALSE,
  /* 161 L_ASSERT	*/					FALSE,
  /* 162 L_OF	*/						FALSE,
  /* 163 L_DO	*/						FALSE,
  /* 164 L_ANY	*/						FALSE,
  /* 165 L_WHERE  */					FALSE,
  /* 166 L_AFFECT */					FALSE,
  /* 167 L_BECOMES_MEMBER_OF */			FALSE,
  /* 168 L_ITERATE */					FALSE,
  /* 169 L_CLOSURE */					FALSE,
  /* 170 L_CLOSURE1 */					FALSE,
  /* 171 L_EVL */						FALSE,
  /* 172 L_UMINUS */					FALSE,
  /* 173 L_LR_SCOL */					FALSE,
  /* 174 L_LR_PARALLEL */				FALSE,
  /* 175 L_RENAMED_IDENT */				FALSE,
  /* 176 L_REC */						FALSE,
  /* 177 L_STRUCT */					FALSE,
  /* 178 L_LABEL_COL */ 				FALSE,
  /* 179 L_RECORD_ACCESS */  			FALSE,
  /* 180 L_LIB_FILE_NAME */				FALSE,
#ifdef COMPAT
  /* 181 L_VOID */						FALSE,
#endif
  /* 182  L_TREE */              		FALSE,
  /* 183  L_BTREE */             		FALSE,
  /* 184  L_CONST */             		FALSE,
  /* 185  L_TOP */               		FALSE,
  /* 186  L_SONS */              		FALSE,
  /* 187  L_PREFIX */            		FALSE,
  /* 188  L_POSTFIX */           		FALSE,
  /* 189  L_SIZET */             		FALSE,
  /* 190  L_MIRROR */            		FALSE,
  /* 191  L_RANK */              		FALSE,
  /* 192  L_FATHER */            		FALSE,
  /* 193  L_SON */               		FALSE,
  /* 194  L_SUBTREE */           		FALSE,
  /* 195  L_ARITY */             		FALSE,
  /* 196  L_BIN */               		FALSE,
  /* 197  L_LEFT */              		FALSE,
  /* 198  L_RIGHT */             		FALSE,
  /* 199  L_INFIX */             		FALSE,
  /* 200 L_LOCAL_OPERATIONS	*/		TRUE,
  /* 201 L_SYSTEM */                TRUE,
  /* 202  L_EVENTS */					TRUE,
  /* 203 L_WITNESS */					FALSE,
#ifdef BALBULETTE
  /* 201  L_BECOMES_SUCH_THAT */		FALSE,
  /* 202  L_MODEL */					TRUE,
  /* 204  L_THEOREMS */					TRUE,
#endif
  /* 204 L_CPP_COMMENT */               FALSE,
  /* 205 L_KEPT_CPP_COMMENT */          FALSE,
  /* 206 L_REAL_TYPE	*/				FALSE,
  /* 207 L_REAL	*/					FALSE,
  /* 208 L_FLOAT_TYPE	*/				FALSE,
  /* 209 L_JUMPIF */ FALSE,
  /* 210 L_LABEL	*/					FALSE,
  /* 211 L_TO	*/						FALSE,
  /* 212 L_REALOP */ FALSE,
  /* 213 L_FLOOR */ FALSE,
  /* 214 L_CEILING */ FALSE,
  /* 215 L_REF	*/    FALSE,
  /* 216 L_SURJ_RELATION	*/			FALSE,
  /* 217 L_TOTAL_RELATION	*/			FALSE,
  /* 218 L_TOTAL_SURJ_RELATION	*/		FALSE
} ;

// Un lexeme est-il une clause de dependance ?
static const int lex_is_dep_clause[] =
{
  /* 000 L_IDENT	*/					FALSE,
  /* 001 L_COMMENT	*/					FALSE,
  /* 002 L_KEPT_COMMENT	*/				FALSE,
  /* 003 L_STRING	*/					FALSE,
  /* 004 L_NUMBER	*/					FALSE,
  /* 005 L_OPEN_PAREN	*/				FALSE,
  /* 006 L_CLOSE_PAREN	*/				FALSE,
  /* 007 L_OPEN_BRACKET	*/				FALSE,
  /* 008 L_CLOSE_BRACKET	*/	   		FALSE,
  /* 009 L_OPEN_BRACKET	*/				FALSE,
  /* 010 L_CLOSE_BRACKET	*/			FALSE,
  /* 011 L_SCOL	*/						FALSE,
  /* 012 L_COMMA	*/					FALSE,
  /* 013 L_DOT	*/						FALSE,
  /* 014 L_MACHINE	*/				TRUE,
  /* 015 L_REFINEMENT	*/			TRUE,
  /* 016 L_IMPLEMENTATION	*/ 		TRUE,
  /* 017 L_BEGIN	*/					FALSE,
  /* 018 L_END	*/						FALSE,
  /* 019 L_SEES	*/					TRUE,
  /* 020 L_INCLUDES	*/				TRUE,
  /* 021 L_PROMOTES	*/				TRUE,
  /* 022 L_IMPORTS	*/				TRUE,
  /* 023 L_REFINES	*/				TRUE,
  /* 024 L_EXTENDS	*/				TRUE,
  /* 025 L_USES	*/					TRUE,
  /* 026 L_CONSTRAINTS	*/				FALSE,
  /* 027 L_DEFINITIONS	*/				FALSE,
  /* 028 L_INVARIANT	*/				FALSE,
  /* 029 L_ASSERTIONS	*/				FALSE,
  /* 030 L_VALUES	*/					FALSE,
  /* 031 L_SETS	*/						FALSE,
  /* 032 L_CONSTANTS	*/				FALSE,
  /* 033 L_HIDDEN_CONSTANTS	*/	   		FALSE,
  /* 034 L_PROPERTIES	*/				FALSE,
  /* 035 L_VARIABLES,	*/				FALSE,
  /* 036 L_HIDDEN_VARIABLES	*/	   		FALSE,
  /* 037 L_INITIALISATION	*/	   		FALSE,
  /* 038 L_OPERATIONS, 	*/				FALSE,
  /* 039 L_BOOL	*/						FALSE,
  /* 040 L_MOD	*/						FALSE,
  /* 041 L_SUCC	*/						FALSE,
  /* 042 L_PRED	*/						FALSE,
  /* 043 L_MAX	*/						FALSE,
  /* 044 L_MIN	*/						FALSE,
  /* 045 L_CARD	*/						FALSE,
  /* 046 L_SIGMA	*/					FALSE,
  /* 047 L_PI	*/						FALSE,
  /* 048 L_GUNION	*/					FALSE,
  /* 049 L_GINTER	*/					FALSE,
  /* 050 L_QUNION	*/					FALSE,
  /* 051 L_QINTER	*/			   		FALSE,
  /* 052 L_ID	*/						FALSE,
  /* 053 L_PRJ1	*/						FALSE,
  /* 054 L_PRJ2	*/						FALSE,
  /* 055 L_DOM	*/						FALSE,
  /* 056 L_RAN	*/						FALSE,
  /* 057 L_SIZE	*/						FALSE,
  /* 058 L_FIRST	*/					FALSE,
  /* 059 L_LAST	*/						FALSE,
  /* 060 L_FRONT	*/					FALSE,
  /* 061 L_TAIL	*/						FALSE,
  /* 062 L_REV	*/						FALSE,
  /* 063 L_L_OR	*/						FALSE,
  /* 064 L_L_NOT	*/					FALSE,
  /* 065 L_PLUS	*/						FALSE,
  /* 066 L_MINUS	*/					FALSE,
  /* 067 L_TIMES	*/					FALSE,
  /* 068 L_DIVIDES	*/					FALSE,
  /* 069 L_POWER	*/					FALSE,
  /* 070 L_UNION	*/					FALSE,
  /* 071 L_INTERSECT	*/				FALSE,
  /* 072 L_INTERVAL	*/					FALSE,
  /* 073 L_EQUAL	*/					FALSE,
  /* 074 L_REWRITES	*/					FALSE,
  /* 075 L_DIFFERENT	*/				FALSE,
  /* 076 L_LESS_THAN   */  				FALSE,
  /* 077 L_LESS_THAN_OR_EQUAL	*/ 		FALSE,
  /* 078 L_GREATER_THAN	*/ 				FALSE,
  /* 079 L_GREATER_THAN_OR_EQUAL_THAN*/	FALSE,
  /* 080 L_AND	*/ 						FALSE,
  /* 081 L_VAR	*/ 						FALSE,
  /* 082 L_IN	*/ 						FALSE,
  /* 083 L_IF	*/						FALSE,
  /* 084 L_THEN	*/						FALSE,
  /* 085 L_ELSIF	*/					FALSE,
  /* 086 L_ELSE	*/						FALSE,
  /* 087 L_CASE	*/						FALSE,
  /* 088 L_EITHER	*/					FALSE,
  /* 089 L_OR	*/						FALSE,
  /* 090 L_SKIP	*/						FALSE,
  /* 091 L_PRE	*/						FALSE,
  /* 092 L_CHOICE	*/					FALSE,
  /* 093 L_SELECT	*/					FALSE,
  /* 094 L_WHEN	*/						FALSE,
  /* 095 L_WHILE	*/					FALSE,
  /* 096 L_VARIANT	*/					FALSE,
  /* 097 L_LET	*/						FALSE,
  /* 098 L_BE	*/						FALSE,
  /* 099 L_NAT	*/						FALSE,
  /* 100 L_NAT1	*/						FALSE,
  /* 101 L_INT	*/						FALSE,
  /* 102 L_TRUE	*/						FALSE,
  /* 103 L_FALSE	*/					FALSE,
  /* 104 L_MAXINT	*/					FALSE,
  /* 105 L_MININT	*/					FALSE,
  /* 106 L_EMPTY_SET	*/				FALSE,
  /* 107 L_INTEGER	*/					FALSE,
  /* 108 L_NATURAL	*/					FALSE,
  /* 109 L_NATURAL1	*/					FALSE,
  /* 110 L_BOOL_TYPE	*/				FALSE,
  /* 111 L_STRING_TYPE	*/				FALSE,
  /* 112 L_POW	*/						FALSE,
  /* 113 L_POW1	*/						FALSE,
  /* 114 L_FIN	*/						FALSE,
  /* 115 L_FIN1	*/						FALSE,
  /* 116 L_RETURNS	*/					FALSE,
  /* 117 L_MAPLET	*/					FALSE,
  /* 118 L_FORALL	*/					FALSE,
  /* 119 L_EXISTS	*/					FALSE,
  /* 120 L_IMPLIES	*/					FALSE,
  /* 121 L_EQUIVALENT	*/				FALSE,
  /* 122 L_BELONGS	*/					FALSE,
  /* 123 L_NOT_BELONGS	*/				FALSE,
  /* 124 L_INCLUDED	*/					FALSE,
  /* 125 L_STRICT_INCLUDED	*/	   		FALSE,
  /* 126 L_NOT_INCLUDED	*/				FALSE,
  /* 127 L_NOT_STRICT_INCLUDED	*/ 		FALSE,
  /* 128 L_DIRECT_PRODUCT	*/	   		FALSE,
  /* 129 L_PARALLEL	*/					FALSE,
  /* 130 L_RESTRICTION	*/				FALSE,
  /* 131 L_ANTIRESTRICTION	*/			FALSE,
  /* 132 L_CORESTRICTION	*/			FALSE,
  /* 133 L_ANTICORESTRICTION	*/		FALSE,
  /* 134 L_LEFT_OVERLOAD	*/			FALSE,
  /* 135 L_PARTIAL_FUNCTION	*/			FALSE,
  /* 136 L_TOTAL_FUNCTION	*/			FALSE,
  /* 137 L_PARTIAL_INJECTION	*/		FALSE,
  /* 138 L_TOTAL_INJECTION	*/			FALSE,
  /* 139 L_PARTIAL_SURJECTION	*/		FALSE,
  /* 140 L_TOTAL_SURJECTION	*/			FALSE,
  /* 141 L_BIJECTION	*/				FALSE,
  /* 142 L_LAMBDA	*/					FALSE,
  /* 143 L_TRANS_FUNC	*/				FALSE,
  /* 144 L_TRANS_REL	*/				FALSE,
  /* 145 L_SEQUENCE	*/					FALSE,
  /* 146 L_NON_EMPTY_SEQUENCE	*/		FALSE,
  /* 147 L_INJECTIVE_SEQ	*/			FALSE,
  /* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	FALSE,
  /* 149 L_PERMUTATION	*/				FALSE,
  /* 150 L_CONCAT	*/					FALSE,
  /* 151 L_HEAD_INSERT	*/				FALSE,
  /* 152 L_TAIL_INSERT	*/				FALSE,
  /* 153 L_HEAD_RESTRICT	*/			FALSE,
  /* 154 L_TAIL_RESTRICT	*/			FALSE,
  /* 155 L_GENERALISED_CONCAT*/			FALSE,
  /* 156 L_CONVERSE	*/					FALSE,
  /* 157 L_EMPTY_SEQ	*/				FALSE,
  /* 158 L_SET_RELATION	*/				FALSE,
  /* 159 L_PIPE	*/						FALSE,
  /* 160 L_SCOPE	*/					FALSE,
  /* 161 L_ASSERT	*/					FALSE,
  /* 162 L_OF	*/						FALSE,
  /* 163 L_DO	*/						FALSE,
  /* 164 L_ANY	*/						FALSE,
  /* 165 L_WHERE  */					FALSE,
  /* 166 L_AFFECT */					FALSE,
  /* 167 L_BECOMES_MEMBER_OF */			FALSE,
  /* 168 L_ITERATE */					FALSE,
  /* 169 L_CLOSURE */					FALSE,
  /* 170 L_CLOSURE1 */					FALSE,
  /* 171 L_EVL */						FALSE,
  /* 172 L_UMINUS */					FALSE,
  /* 173 L_LR_SCOL */					FALSE,
  /* 174 L_LR_PARALLEL */				FALSE,
  /* 175 L_RENAMED_IDENT */				FALSE,
  /* 176 L_REC */						FALSE,
  /* 177 L_STRUCT */					FALSE,
  /* 178 L_LABEL_COL */ 				FALSE,
  /* 179 L_RECORD_ACCESS */  			FALSE,
  /* 180 L_LIB_FILE_NAME */				FALSE,
#ifdef COMPAT
  /* 181 L_VOID */						FALSE,
#endif
  /* 182 L_TREE */            			FALSE,
  /* 183 L_BTREE */            			FALSE,
  /* 184 L_CONST */            			FALSE,
  /* 185 L_TOP */              			FALSE,
  /* 186 L_SONS */             			FALSE,
  /* 187 L_PREFIX */           			FALSE,
  /* 188 L_POSTFIX */          			FALSE,
  /* 189 L_SIZET */            			FALSE,
  /* 190 L_MIRROR */           			FALSE,
  /* 191 L_RANK */             			FALSE,
  /* 192 L_FATHER */           			FALSE,
  /* 193 L_SON */              			FALSE,
  /* 194 L_SUBTREE */          			FALSE,
  /* 195 L_ARITY */            			FALSE,
  /* 196 L_BIN */              			FALSE,
  /* 197 L_LEFT */             			FALSE,
  /* 198 L_RIGHT */            			FALSE,
  /* 199 L_INFIX */            			FALSE,
  /* 200 L_LOCAL_OPERATIONS, 	*/		FALSE,
  /* 201 L_SYSTEM */                    TRUE,
  /* 202 L_EVENTS */					FALSE,
  /* 203 L_WITNESS */					FALSE,
#ifdef BALBULETTE
  /* 201 L_BECOMES_SUCH_THAT, 	*/		FALSE,
  /* 202 L_MODEL, 	*/					TRUE,
  /* 204 L_THEOREMS */					FALSE,
#endif
  /* 204 L_CPP_COMMENT */               FALSE,
  /* 205 L_KEPT_CPP_COMMENT */          FALSE,
  /* 206 L_REAL_TYPE */          FALSE,
  /* 207 L_REAL	*/					FALSE,
  /* 208 L_FLOAT_TYPE */          FALSE,
  /* 209 L_JUMPIF*/	FALSE,
  /* 210 L_LABEL	*/					FALSE,
  /* 211 L_TO	*/						FALSE,
  /* 212 L_REALOP */ FALSE,
  /* 213 L_FLOOR */ FALSE,
  /* 214 L_CEILING */ FALSE,
  /* 215 L_REF	*/	  FALSE,
  /* 158 L_SURJ_RELATION	*/			FALSE,
  /* 158 L_TOTAL_RELATION	*/			FALSE,
  /* 158 L_TOTAL_SURJ_RELATION	*/		FALSE
                    } ;

//
//	}{	Table Lex_type->ascii
//
static const char *english_ascii_lex_type[] =
{
  /* 000 */
  "Identifier", 				// L_IDENT
  /* 001 */
  "Comment", // L_COMMENT
  /* 002 */
  "Kept comment", // L_KEPT_COMMENT
  /* 003 */
  "Literal String", // L_STRING
  /* 004 */
  "Literal Number", // L_NUMBER
  /* 005 */
  "(", //L_OPEN_PAREN,
  /* 006 */
  ")", //L_CLOSE_PAREN,
  /* 007 */
  "{", //L_OPEN_BRACES,
  /* 008 */
  "}", //L_CLOSE_BRACES,
  /* 009 */
  "[", //L_OPEN_BRACKET,
  /* 010 */
  "]", //L_CLOSE_BRACKET,
  /* 011 */
  ";", //L_SCOL,
  /* 012 */
  ",", //L_COMMA,
  /* 013 */
  ".", //L_DOT,
  /* 014 */
  "MACHINE", //L_MACHINE,
  /* 015 */
  "REFINEMENT", //L_REFINEMENT,
  /* 016 */
  "IMPLEMENTATION", //L_IMPLEMENTATION,
  /* 017 */
  "BEGIN", //L_BEGIN,
  /* 018 */
  "END", //L_END,
  /* 019 */
  "SEES", //L_SEES,
  /* 020 */
  "INCLUDES", //L_INCLUDES,
  /* 021 */
  "PROMOTES", //L_PROMOTES,
  /* 022 */
  "IMPORTS", //L_IMPORTS,
  /* 023 */
  "REFINES", //L_REFINES,
  /* 024 */
  "EXTENDS", //L_EXTENDS,
  /* 025 */
  "USES", //L_USES,
  /* 026 */
  "CONSTRAINTS", //L_CONSTRAINTS,
  /* 027 */
  "DEFINITIONS", //L_DEFINITIONS,
  /* 028 */
  "INVARIANT", //L_INVARIANT,
  /* 029 */
  "ASSERTIONS", //L_ASSERTIONS,
  /* 030 */
  "VALUES", //L_VALUES,
  /* 031 */
  "SETS", //L_SETS,
  /* 032 */
  "CONCRETE_CONSTANTS", //L_CONSTANTS,
  /* 033 */
  "ABSTRACT_CONSTANTS", //L_HIDDEN_CONSTANTS,
  /* 034 */
  "PROPERTIES", //L_PROPERTIES,
  /* 035 */
  "CONCRETE_VARIABLES", //L_VARIABLES,
  /* 036 */
  "ABSTRACT_VARIABLES", //L_HIDDEN_VARIABLES,
  /* 037 */
  "INITIALISATION", //L_INITIALISATION,
  /* 038 */
  "OPERATIONS", //L_OPERATIONS,
  /* 039 */
  "bool", //L_BOOL,
  /* 040 */
  "mod", //L_MOD,
  /* 041 */
  "succ", //L_SUCC,
  /* 042 */
  "pred", //L_PRED,
  /* 043 */
  "max", //L_MAX,
  /* 044 */
  "min", //L_MIN,
  /* 045 */
  "card", //L_CARD,
  /* 046 */
  "SIGMA", //L_SIGMA,
  /* 047 */
  "PI", //L_PI,
  /* 048 */
  "union", //L_GUNION,
  /* 049 */
  "inter", //L_GINTER,
  /* 050 */
  "UNION", //L_QUNION,
  /* 051 */
  "INTER", //L_QINTER,
  /* 052 */
  "id", //L_ID,
  /* 053 */
  "prj1", //L_PRJ1,
  /* 054 */
  "prj2", //L_PRJ2,
  /* 055 */
  "dom", //L_DOM,
  /* 056 */
  "ran", //L_RAN,
  /* 057 */
  "size", //L_SIZE,
  /* 058 */
  "first", //L_FIRST,
  /* 059 */
  "last", //L_LAST,
  /* 060 */
  "front", //L_FRONT,
  /* 061 */
  "tail", //L_TAIL,
  /* 062 */
  "rev", //L_REV,
  /* 063 */
  "or", //L_OR,
  /* 064 */
  "not", //L_NOT,
  /* 065 */
  "+", //L_PLUS,
  /* 066 */
  "-", //L_MINUS,
  /* 067 */
  "*", //L_TIMES,
  /* 068 */
  "/", //L_DIVIDES,
  /* 069 */
  "**", //L_POWER,
  /* 070 */
  "\\/", //L_UNION,
  /* 071 */
  "/\\", //L_INTERSECT,
  /* 072 */
  "..", //L_INTERVAL,
  /* 073 */
  "=", //L_EQUAL,
  /* 074 */
  "==", //L_REWRITES,
  /* 075 */
  "/=", //L_DIFFERENT,
  /* 076 */
  "<", //L_LESS_THAN,
  /* 077 */
  "<=", //L_LESS_OR_EQUAL_THAN
  /* 078 */
  ">", //L_GREATER
  /* 079 */
  ">=", //L_GREATER_OR_EQUAL_THAN
  /* 080 */
  "&", //L_AND,
  /* 081 */
  "VAR", //L_VAR,
  /* 082 */
  "IN", //L_IN,
  /* 083 */
  "IF", //L_IF,
  /* 084 */
  "THEN", //L_THEN,
  /* 085 */
  "ELSIF", //L_ELSIF,
  /* 086 */
  "ELSE", //L_ELSE,
  /* 087 */
  "CASE", //L_CASE,
  /* 088 */
  "EITHER", //L_EITHER,
  /* 089 */
  "OR", //L_OR,
  /* 090 */
  "skip", //L_SKIP,
  /* 091 */
  "PRE", //L_PRE,
  /* 092 */
  "CHOICE", //L_CHOICE,
  /* 093 */
  "SELECT", //L_SELECT,
  /* 094 */
  "WHEN", //L_WHEN,
  /* 095 */
  "WHILE", //L_WHILE,
  /* 096 */
  "VARIANT", //L_VARIANT,
  /* 097 */
  "LET", //L_LET,
  /* 098 */
  "BE", //L_BE,
  /* 099 */
  "NAT", //L_NAT,
  /* 100 */
  "NAT1", //L_NAT1,
  /* 101 */
  "INT", //L_INT,
  /* 102 */
  "TRUE", //L_TRUE,
  /* 103 */
  "FALSE", //L_FALSE,
  /* 104 */
  "MAXINT", //L_MAXINT,
  /* 105 */
  "MININT", //L_MININT,
  /* 106 */
  "{}", //L_EMPTY_SET,
  /* 107 */
  "INTEGER", //L_INTEGER,
  /* 108 */
  "NATURAL", //L_NATURAL,
  /* 109 */
  "NATURAL1", //L_NATURAL1,
  /* 110 */
  "BOOL", //L_BOOL_TYPE,
  /* 111 */
  "STRING", //L_STRING_TYPE,
  /* 112 */
  "POW", //L_POW,
  /* 113 */
  "POW1", //L_POW1,
  /* 114 */
  "FIN", //L_FIN,
  /* 115 */
  "FIN1", //L_FIN1,
  /* 116 */
  "<--", //L_RETURNS,
  /* 117 */
  "|->", //L_MAPLET,
  /* 118 */
  "!", //L_FORALL,
  /* 119 */
  "#", //L_EXISTS,
  /* 120 */
  "=>", //L_IMPLIES,
  /* 121 */
  "<=>", //L_EQUIVALENT,
  /* 122 */
  ":", //L_BELONGS,
  /* 123 */
  "/:", //L_NOT_BELONGS,
  /* 124 */
  "<:", //L_INCLUDED,
  /* 125 */
  "<<:", //L_STRICT_INCLUDED,
  /* 126 */
  "/<:", //L_NOT_INCLUDED,
  /* 127 */
  "/<<:", //L_NOT_STRICT_INCLUDED,
  /* 128 */
  "><", //L_DIRECT_PRODUCT,
  /* 129 */
  "||", //L_PARALLEL,
  /* 130 */
  "<|", //L_RESTRICTION,
  /* 131 */
  "<<|", //L_ANTIRESTRICTION,
  /* 132 */
  "|>", //L_CORESTRICTION,
  /* 133 */
  "|>>", //L_ANTICORESTRICTION,
  /* 134 */
  "<+", //L_LEFT_OVERLOAD,
  /* 135 */
  "+->",//L_PARTIAL_FUNCTION
  /* 136 */
  "-->",//L_TOTAL_FUNCTION,			/* -->				*/
  /* 137 */
  ">+>",//L_PARTIAL_INJECTION,		/* >+>				*/
  /* 138 */
  ">->",//L_TOTAL_INJECTION,			/* >->				*/
  /* 139 */
  "+->>",//L_PARTIAL_SURJECTION,		/* +->>				*/
  /* 140 */
  "-->>",//L_TOTAL_SURJECTION,			/* -->>				*/
  /* 141 */
  ">->>", //L_BIJECTION,
  /* 142 */
  "%", //L_LAMBDA,
  /* 143 */
  "fnc", //L_TRANS_FUNC,
  /* 144 */
  "rel", //L_TRANS_REL,
  /* 145 */
  "seq", //L_SEQUENCE,
  /* 146 */
  "seq1", //L_NON_EMPTY_SEQUENCE,
  /* 147 */
  "iseq", //L_INJECTIVE_SEQ,
  /* 148 */
  "iseq1", //L_NON_EMPTY_INJECTIVE_SEQ,
  /* 149 */
  "perm", //L_PERMUTATION,
  /* 150 */
  "^", //L_CONCAT,
  /* 151 */
  "->", //L_HEAD_INSERT,
  /* 152 */
  "<-", //L_TAIL_INSERT,				/* <-					*/
  /* 153 */
  "/|\\", //L_HEAD_RESTRICT,
  /* 154 */
  "\\|/", //L_TAIL_RESTRICT,			/* \|/				*/
  /* 155 */
  "conc", //L_GENERALISED_CONCAT,
  /* 156 */
  "~", //L_CONVERSE,
  /* 157 */
  "[]", //L_EMPTY_SEQ,
  /* 158 */
  "<->", //L_SET_RELATION,
  /* 159 */
  "|", //L_PIPE,
  /* 160 */
  "$0", //L_SCOPE,
  /* 161 */
  "ASSERT", //L_ASSERT,
  /* 162 */
  "OF", //L_OF,
  /* 163 */
  "DO", //L_DO,
  /* 164 */
  "ANY", //L_ANY,
  /* 165 */
  "WHERE", //L_WHERE,
  /* 166 */
  ":=", //L_AFFECT,
  /* 167 */
  "::", //L_BECOMES_MEMBER_OF,
  /* 168 */
  "iterate", //L_ITERATE,
  /* 169 */
  "closure", //L_CLOSURE,
  /* 170 */
  "closure1", //L_CLOSURE1,
  /* 171 */
  "",  // L_EVL, genere par ana LR 	*/
  /* 172 */
  "-", // L_UMINUS,	Moins unaire, genere par ana LR 	*/
  /* 173 */
  "\\;", //L_LR_SCOL, "\;" genere par ana LR 	*/
  /* 174 */
  "\\||", // L_LR_PARALLEL, \||" , genere par ana LR 	*/
  /* 175 */
  "renamed identifier", // L_RENAMED_IDENT
  /* 176 */
  "rec",
  /* 177 */
  "struct",
  /* 178 */
  "\\:",
  /* 179 */
  "'",
  /* 180 */
  "Library file name",
#ifdef COMPAT
  /* 181 */
  "void",
#endif
  /* 182 */
  "tree", // L_TREE
  /* 183 */
  "btree", // L_BTREE
  /* 184 */
  "const", // L_CONST
  /* 185 */
  "top", // L_TOP
  /* 186 */
  "sons", // L_SONS
  /* 187 */
  "prefix", // L_PREFIX
  /* 188 */
  "postfix", // L_POSTFIX
  /* 189 */
  "sizet", // L_SIZET
  /* 190 */
  "mirror", // L_MIRROR
  /* 191 */
  "rank", // L_RANK
  /* 192 */
  "father", // L_FATHER
  /* 193 */
  "son", // L_SON
  /* 194 */
  "subtree", // L_SUBTREE
  /* 195 */
  "arity", // L_ARITY
  /* 196 */
  "bin", // L_BIN
  /* 197 */
  "left", // L_LEFT
  /* 198 */
  "right", // L_RIGHT
  /* 199 */
  "infix", // L_INFIX
  /* 200 */
  "LOCAL_OPERATIONS",
  /* 201 */
  "SYSTEM", // L_SYSTEM
  /* 202 */
  "EVENTS", // L_EVENTS,
  /* 203 */
  "WITNESS", // L_WITNESS
#ifdef BALBULETTE
  /* 201 */
  ":|", // L_BECOMES_SUCH_THAT,
  /* 202 */
  "MODEL", // L_MODEL,
  /* 204 */
  "THEOREMS", // L_THEOREMS,
#endif
  /* 204 */
  "CppComment", // L_CPP_COMMENT
  /* 205 */
  "CppKeptComment", // L_KEPT_CPP_COMMENT
  /* 206 */
  "REAL", //L_REAL_TYPE
  /* 207 */
  "Literal Real", // L_REAL
  /* 208 */
  "FLOAT", //L_FLOAT_TYPE
  /* 29 */
  "JUMPIF", //L_JUMPIF
  /* 210 */
  "LABEL", //L_LABEL,
  /* 211 */
  "TO", //L_TO,
  /* 212 */
  "real", // L_REALOP
  /* 213 */
  "floor", // L_FLOOR
  /* 214 */
  "ceiling", // L_CEILING
  /* 215 */
  "ref", //L_REF
  /* 216 */
  "<->>", //L_SURJ_RELATION,
  /* 217 */
  "<<->", //L_TOTAL_RELATION,
  /* 218 */
  "<<->>", //L_TOTAL_SURJ_RELATION,
} ;

static const char *french_ascii_lex_type[] =
{
  /* 000 */
  "Identificateur", 				// L_IDENT
  /* 001 */
  "Commentaire", // L_COMMENT
  /* 002 */
  "Commentaire traduisible", // L_KEPT_COMMENT
  /* 003 */
  "Cha�ne litt�rale", // L_STRING
  /* 004 */
  "Nombre litt�ral", // L_NUMBER
  /* 005 */
  "(", //L_OPEN_PAREN,
  /* 006 */
  ")", //L_CLOSE_PAREN,
  /* 007 */
  "{", //L_OPEN_BRACES,
  /* 008 */
  "}", //L_CLOSE_BRACES,
  /* 009 */
  "[", //L_OPEN_BRACKET,
  /* 010 */
  "]", //L_CLOSE_BRACKET,
  /* 011 */
  ";", //L_SCOL,
  /* 012 */
  ",", //L_COMMA,
  /* 013 */
  ".", //L_DOT,
  /* 014 */
  "MACHINE", //L_MACHINE,
  /* 015 */
  "REFINEMENT", //L_REFINEMENT,
  /* 016 */
  "IMPLEMENTATION", //L_IMPLEMENTATION,
  /* 017 */
  "BEGIN", //L_BEGIN,
  /* 018 */
  "END", //L_END,
  /* 019 */
  "SEES", //L_SEES,
  /* 020 */
  "INCLUDES", //L_INCLUDES,
  /* 021 */
  "PROMOTES", //L_PROMOTES,
  /* 022 */
  "IMPORTS", //L_IMPORTS,
  /* 023 */
  "REFINES", //L_REFINES,
  /* 024 */
  "EXTENDS", //L_EXTENDS,
  /* 025 */
  "USES", //L_USES,
  /* 026 */
  "CONSTRAINTS", //L_CONSTRAINTS,
  /* 027 */
  "DEFINITIONS", //L_DEFINITIONS,
  /* 028 */
  "INVARIANT", //L_INVARIANT,
  /* 029 */
  "ASSERTIONS", //L_ASSERTIONS,
  /* 030 */
  "VALUES", //L_VALUES,
  /* 031 */
  "SETS", //L_SETS,
  /* 032 */
  "CONCRETE_CONSTANTS", //L_CONSTANTS,
  /* 033 */
  "ABSTRACT_CONSTANTS", //L_HIDDEN_CONSTANTS,
  /* 034 */
  "PROPERTIES", //L_PROPERTIES,
  /* 035 */
  "CONCRETE_VARIABLES", //L_VARIABLES,
  /* 036 */
  "ABSTRACT_VARIABLES", //L_HIDDEN_VARIABLES,
  /* 037 */
  "INITIALISATION", //L_INITIALISATION,
  /* 038 */
  "OPERATIONS", //L_OPERATIONS,
  /* 039 */
  "bool", //L_BOOL,
  /* 040 */
  "mod", //L_MOD,
  /* 041 */
  "succ", //L_SUCC,
  /* 042 */
  "pred", //L_PRED,
  /* 043 */
  "max", //L_MAX,
  /* 044 */
  "min", //L_MIN,
  /* 045 */
  "card", //L_CARD,
  /* 046 */
  "SIGMA", //L_SIGMA,
  /* 047 */
  "PI", //L_PI,
  /* 048 */
  "union", //L_GUNION,
  /* 049 */
  "inter", //L_GINTER,
  /* 050 */
  "UNION", //L_QUNION,
  /* 051 */
  "INTER", //L_QINTER,
  /* 052 */
  "id", //L_ID,
  /* 053 */
  "prj1", //L_PRJ1,
  /* 054 */
  "prj2", //L_PRJ2,
  /* 055 */
  "dom", //L_DOM,
  /* 056 */
  "ran", //L_RAN,
  /* 057 */
  "size", //L_SIZE,
  /* 058 */
  "first", //L_FIRST,
  /* 059 */
  "last", //L_LAST,
  /* 060 */
  "front", //L_FRONT,
  /* 061 */
  "tail", //L_TAIL,
  /* 062 */
  "rev", //L_REV,
  /* 063 */
  "or", //L_OR,
  /* 064 */
  "not", //L_NOT,
  /* 065 */
  "+", //L_PLUS,
  /* 066 */
  "-", //L_MINUS,
  /* 067 */
  "*", //L_TIMES,
  /* 068 */
  "/", //L_DIVIDES,
  /* 069 */
  "**", //L_POWER,
  /* 070 */
  "\\/", //L_UNION,
  /* 071 */
  "/\\", //L_INTERSECT,
  /* 072 */
  "..", //L_INTERVAL,
  /* 073 */
  "=", //L_EQUAL,
  /* 074 */
  "==", //L_REWRITES,
  /* 075 */
  "/=", //L_DIFFERENT,
  /* 076 */
  "<", //L_LESS_THAN,
  /* 077 */
  "<=", //L_LESS_OR_EQUAL_THAN
  /* 078 */
  ">", //L_GREATER
  /* 079 */
  ">=", //L_GREATER_OR_EQUAL_THAN
  /* 080 */
  "&", //L_AND,
  /* 081 */
  "VAR", //L_VAR,
  /* 082 */
  "IN", //L_IN,
  /* 083 */
  "IF", //L_IF,
  /* 084 */
  "THEN", //L_THEN,
  /* 085 */
  "ELSIF", //L_ELSIF,
  /* 086 */
  "ELSE", //L_ELSE,
  /* 087 */
  "CASE", //L_CASE,
  /* 088 */
  "EITHER", //L_EITHER,
  /* 089 */
  "OR", //L_OR,
  /* 090 */
  "skip", //L_SKIP,
  /* 091 */
  "PRE", //L_PRE,
  /* 092 */
  "CHOICE", //L_CHOICE,
  /* 093 */
  "SELECT", //L_SELECT,
  /* 094 */
  "WHEN", //L_WHEN,
  /* 095 */
  "WHILE", //L_WHILE,
  /* 096 */
  "VARIANT", //L_VARIANT,
  /* 097 */
  "LET", //L_LET,
  /* 098 */
  "BE", //L_BE,
  /* 099 */
  "NAT", //L_NAT,
  /* 100 */
  "NAT1", //L_NAT1,
  /* 101 */
  "INT", //L_INT,
  /* 102 */
  "TRUE", //L_TRUE,
  /* 103 */
  "FALSE", //L_FALSE,
  /* 104 */
  "MAXINT", //L_MAXINT,
  /* 105 */
  "MININT", //L_MININT,
  /* 106 */
  "{}", //L_EMPTY_SET,
  /* 107 */
  "INTEGER", //L_INTEGER,
  /* 108 */
  "NATURAL", //L_NATURAL,
  /* 109 */
  "NATURAL1", //L_NATURAL1,
  /* 110 */
  "BOOL", //L_BOOL_TYPE,
  /* 111 */
  "STRING", //L_STRING_TYPE,
  /* 112 */
  "POW", //L_POW,
  /* 113 */
  "POW1", //L_POW1,
  /* 114 */
  "FIN", //L_FIN,
  /* 115 */
  "FIN1", //L_FIN1,
  /* 116 */
  "<--", //L_RETURNS,
  /* 117 */
  "|->", //L_MAPLET,
  /* 118 */
  "!", //L_FORALL,
  /* 119 */
  "#", //L_EXISTS,
  /* 120 */
  "=>", //L_IMPLIES,
  /* 121 */
  "<=>", //L_EQUIVALENT,
  /* 122 */
  ":", //L_BELONGS,
  /* 123 */
  "/:", //L_NOT_BELONGS,
  /* 124 */
  "<:", //L_INCLUDED,
  /* 125 */
  "<<:", //L_STRICT_INCLUDED,
  /* 126 */
  "/<:", //L_NOT_INCLUDED,
  /* 127 */
  "/<<:", //L_NOT_STRICT_INCLUDED,
  /* 128 */
  "><", //L_DIRECT_PRODUCT,
  /* 129 */
  "||", //L_PARALLEL,
  /* 130 */
  "<|", //L_RESTRICTION,
  /* 131 */
  "<<|", //L_ANTIRESTRICTION,
  /* 132 */
  "|>", //L_CORESTRICTION,
  /* 133 */
  "|>>", //L_ANTICORESTRICTION,
  /* 134 */
  "<+", //L_LEFT_OVERLOAD,
  /* 135 */
  "+->",//L_PARTIAL_FUNCTION
  /* 136 */
  "-->",//L_TOTAL_FUNCTION,			/* -->				*/
  /* 137 */
  ">+>",//L_PARTIAL_INJECTION,		/* >+>				*/
  /* 138 */
  ">->",//L_TOTAL_INJECTION,			/* >->				*/
  /* 139 */
  "+->>",//L_PARTIAL_SURJECTION,		/* +->>				*/
  /* 140 */
  "-->>",//L_TOTAL_SURJECTION,			/* -->>				*/
  /* 141 */
  ">->>", //L_BIJECTION,
  /* 142 */
  "%", //L_LAMBDA,
  /* 143 */
  "fnc", //L_TRANS_FUNC,
  /* 144 */
  "rel", //L_TRANS_REL,
  /* 145 */
  "seq", //L_SEQUENCE,
  /* 146 */
  "seq1", //L_NON_EMPTY_SEQUENCE,
  /* 147 */
  "iseq", //L_INJECTIVE_SEQ,
  /* 148 */
  "iseq1", //L_NON_EMPTY_INJECTIVE_SEQ,
  /* 149 */
  "perm", //L_PERMUTATION,
  /* 150 */
  "^", //L_CONCAT,
  /* 151 */
  "->", //L_HEAD_INSERT,
  /* 152 */
  "<-", //L_TAIL_INSERT,				/* <-					*/
  /* 153 */
  "/|\\", //L_HEAD_RESTRICT,
  /* 154 */
  "\\|/", //L_TAIL_RESTRICT,			/* \|/				*/
  /* 155 */
  "conc", //L_GENERALISED_CONCAT,
  /* 156 */
  "~", //L_CONVERSE,
  /* 157 */
  "[]", //L_EMPTY_SEQ,
  /* 158 */
  "<->", //L_SET_RELATION,
  /* 159 */
  "|", //L_PIPE,
  /* 160 */
  "$0", //L_SCOPE,
  /* 161 */
  "ASSERT", //L_ASSERT,
  /* 162 */
  "OF", //L_OF,
  /* 163 */
  "DO", //L_DO,
  /* 164 */
  "ANY", //L_ANY,
  /* 165 */
  "WHERE", //L_WHERE,
  /* 166 */
  ":=", //L_AFFECT,
  /* 167 */
  "::", //L_BECOMES_MEMBER_OF,
  /* 168 */
  "iterate", //L_ITERATE,
  /* 169 */
  "closure", //L_CLOSURE,
  /* 170 */
  "closure1", //L_CLOSURE1,
  /* 171 */
  "",  // L_EVL, genere par ana LR 	*/
  /* 172 */
  "-", // L_UMINUS,	Moins unaire, genere par ana LR 	*/
  /* 173 */
  "\\;", //L_LR_SCOL, "\;" genere par ana LR 	*/
  /* 174 */
  "\\||", // L_LR_PARALLEL, \||" , genere par ana LR 	*/
  /* 175 */
  "identificateur renomm�", // L_RENAMED_IDENT
  /* 176 */
  "rec",
  /* 177 */
  "struct",
  /* 178 */
  "\\:",
  /* 179 */
  "'",
  /* 180 */
  "Nom de fichier d'inclusion en bibliotheque",
#ifdef COMPAT
  /* 181 */
  "void",
#endif
  /* 182 */
  "tree", // L_TREE
  /* 183 */
  "btree", // L_BTREE
  /* 184 */
  "const", // L_CONST
  /* 185 */
  "top", // L_TOP
  /* 186 */
  "sons", // L_SONS
  /* 187 */
  "prefix", // L_PREFIX
  /* 188 */
  "postfix", // L_POSTFIX
  /* 189 */
  "sizet", // L_SIZET
  /* 190 */
  "mirror", // L_MIRROR
  /* 191 */
  "rank", // L_RANK
  /* 192 */
  "father", // L_FATHER
  /* 193 */
  "son", // L_SON
  /* 194 */
  "subtree", // L_SUBTREE
  /* 195 */
  "arity", // L_ARITY
  /* 196 */
  "bin", // L_BIN
  /* 197 */
  "left", // L_LEFT
  /* 198 */
  "right", // L_RIGHT
  /* 199 */
  "infix", // L_INFIX
  /* 200 */
  "LOCAL_OPERATIONS",
  /* 201 */
  "SYSTEM", // LSYSTEM
  /* 202 */
   "EVENTS", // L_EVENTS,
   /* 203 */
   "WITNESS", // L_WITNESS
#ifdef BALBULETTE
  /* 201 */
  ":|", //L_BECOMES_SUCH_THAT,
  /* 202 */
  "MODEL", // L_MODEL,
   /* 204 */
  "THEOREMS", // L_THEOREMS,
#endif
    /* 204 */
  "CppComment", // L_CPP_COMMENT
  /* 205 */
  "CppKeptComment", // L_KEPT_CPP_COMMENT
  /* 206 */
  "REAL", //L_REAL_TYPE,
  /* 207 */
  "Nombre r�el", // L_REAL
  /* 208 */
  "FLOAT", //L_FLOAT_TYPE,
  /* 209 */
  "JUMPIF", //L_JUMPIF
  /* 210 */
  "LABEL", //L_LABEL,
  /* 211 */
  "TO", //L_TO,
    /* 212 */
	"real", // L_REALOP
    /* 213 */
	"floor", // L_FLOOR
    /* 214 */
	"ceiling", // L_CEILING
    /* 215 */
    "ref", //L_REF
    /* 216 */
    "<->>", //L_SURJ_RELATION,
    /* 217 */
    "<<->", //L_TOTAL_RELATION,
    /* 218 */
    "<<->>", //L_TOTAL_SURJ_RELATION,
} ;

static size_t ascii_lex_type_len[] =
{
  /* 000 */
  10,	/* L_IDENT */
  /* 001 */
  7,	/* L_COMMENT */
  /* 002 */
  12,	/* L_KEPT_COMMENT */
  /* 003 */
  15,	/* L_STRING */
  /* 004 */
  15,	/* L_NUMBER */
  /* 005 */
  1,	/* L_OPEN_PAREN */
  /* 006 */
  1,	/* L_CLOSE_PAREN */
  /* 007 */
  1,	/* L_OPEN_BRACES */
  /* 008 */
  1,	/* L_CLOSE_BRACES */
  /* 009 */
  1,	/* L_OPEN_BRACKET */
  /* 010 */
  1,	/* L_CLOSE_BRACKET */
  /* 011 */
  1,	/* L_SCOL */
  /* 012 */
  1,	/* L_COMMA */
  /* 013 */
  1,	/* L_DOT */
  /* 014 */
  7,	/* L_MACHINE */
  /* 015 */
  10,	/* L_REFINEMENT */
  /* 016 */
  14,	/* L_IMPLEMENTATION */
  /* 017 */
  5,	/* L_BEGIN */
  /* 018 */
  3,	/* L_END */
  /* 019 */
  4,	/* L_SEES */
  /* 020 */
  8,	/* L_INCLUDES */
  /* 021 */
  8,	/* L_PROMOTES */
  /* 022 */
  7,	/* L_IMPORTS */
  /* 023 */
  7,	/* L_REFINES */
  /* 024 */
  7,	/* L_EXTENDS */
  /* 025 */
  4,	/* L_USES */
  /* 026 */
  11,	/* L_CONSTRAINTS */
  /* 027 */
  11,	/* L_DEFINITIONS */
  /* 028 */
  9,	/* L_INVARIANT */
  /* 029 */
  10,	/* L_ASSERTIONS */
  /* 030 */
  6,	/* L_VALUES */
  /* 031 */
  4,	/* L_SETS */
  /* 032 */
  18,	/* L_CONCRETE_CONSTANTS */
  /* 033 */
  18,	/* L_ABSTRACT_CONSTANTS */
  /* 034 */
  10,	/* L_PROPERTIES */
  /* 035 */
  18,	/* L_CONCRETE_VARIABLES */
  /* 036 */
  18,	/* L_ABSTRACT_VARIABLES */
  /* 037 */
  14,	/* L_INITIALISATION */
  /* 038 */
  10,	/* L_OPERATIONS */
  /* 039 */
  4,	/* L_BOOL */
  /* 040 */
  3,	/* L_MOD */
  /* 041 */
  4,	/* L_SUCC */
  /* 042 */
  4,	/* L_PRED */
  /* 043 */
  3,	/* L_MAX */
  /* 044 */
  3,	/* L_MIN */
  /* 045 */
  4,	/* L_CARD */
  /* 046 */
  5,	/* L_SIGMA */
  /* 047 */
  2,	/* L_PI */
  /* 048 */
  5,	/* L_GUNION */
  /* 049 */
  5,	/* L_GINTER */
  /* 050 */
  5,	/* L_QUNION */
  /* 051 */
  5,	/* L_QINTER */
  /* 052 */
  2,	/* L_ID */
  /* 053 */
  4,	/* L_PRJ1 */
  /* 054 */
  4,	/* L_PRJ2 */
  /* 055 */
  3,	/* L_DOM */
  /* 056 */
  3,	/* L_RAN */
  /* 057 */
  4,	/* L_SIZE */
  /* 058 */
  5,	/* L_FIRST */
  /* 059 */
  4,	/* L_LAST */
  /* 060 */
  5,	/* L_FRONT */
  /* 061 */
  4,	/* L_TAIL */
  /* 062 */
  3,	/* L_REV */
  /* 063 */
  2,	/* L_L_OR */
  /* 064 */
  3,	/* L_L_NOT */
  /* 065 */
  1,	/* L_PLUS */
  /* 066 */
  1,	/* L_MINUS */
  /* 067 */
  1,	/* L_TIMES */
  /* 068 */
  1,	/* L_DIVIDES */
  /* 069 */
  2,	/* L_POWER */
  /* 070 */
  2,	/* L_UNION */
  /* 071 */
  2,	/* L_INTERSECT */
  /* 072 */
  2,	/* L_INTERVAL */
  /* 073 */
  1,	/* L_EQUAL */
  /* 074 */
  2,	/* L_REWRITES */
  /* 075 */
  2,	/* L_DIFFERENT */
  /* 076 */
  1,	/* L_LESS_THAN */
  /* 077 */
  2,	/* L_LESS_THAN_OR_EQUAL */
  /* 078 */
  1,	/* L_GREATER_THAN */
  /* 079 */
  2,	/* L_GREATER_THAN_OR_EQUAL */
  /* 080 */
  1,	/* L_AND */
  /* 081 */
  3,	/* L_VAR */
  /* 082 */
  2,	/* L_IN */
  /* 083 */
  2,	/* L_IF */
  /* 084 */
  4,	/* L_THEN */
  /* 085 */
  5,	/* L_ELSIF */
  /* 086 */
  4,	/* L_ELSE */
  /* 087 */
  4,	/* L_CASE */
  /* 088 */
  6,	/* L_EITHER */
  /* 089 */
  2,	/* L_OR */
  /* 090 */
  4,	/* L_SKIP */
  /* 091 */
  3,	/* L_PRE */
  /* 092 */
  6,	/* L_CHOICE */
  /* 093 */
  6,	/* L_SELECT */
  /* 094 */
  4,	/* L_WHEN */
  /* 095 */
  5,	/* L_WHILE */
  /* 096 */
  7,	/* L_VARIANT */
  /* 097 */
  3,	/* L_LET */
  /* 098 */
  2,	/* L_BE */
  /* 099 */
  3,	/* L_NAT */
  /* 100 */
  4,	/* L_NAT1 */
  /* 101 */
  3,	/* L_INT */
  /* 102 */
  4,	/* L_TRUE */
  /* 103 */
  5,	/* L_FALSE */
  /* 104 */
  6,	/* L_MAXINT */
  /* 105 */
  6,	/* L_MININT */
  /* 106 */
  2,	/* L_EMPTY_SET */
  /* 107 */
  7,	/* L_INTEGER */
  /* 108 */
  7,	/* L_NATURAL */
  /* 109 */
  8,	/* L_NATURAL1 */
  /* 110 */
  4,	/* L_BOOL_TYPE */
  /* 111 */
  6,	/* L_STRING_TYPE */
  /* 112 */
  3,	/* L_POW */
  /* 113 */
  4,	/* L_POW1 */
  /* 114 */
  3,	/* L_FIN */
  /* 115 */
  4,	/* L_FIN1 */
  /* 116 */
  3,	/* L_RETURNS */
  /* 117 */
  3,	/* L_MAPLET */
  /* 118 */
  1,	/* L_FORALL */
  /* 119 */
  1,	/* L_EXISTS */
  /* 120 */
  2,	/* L_IMPLIES */
  /* 121 */
  3,	/* L_EQUIVALENT */
  /* 122 */
  1,	/* L_BELONGS */
  /* 123 */
  2,	/* L_NOT_BELONGS */
  /* 124 */
  2,	/* L_INCLUDED */
  /* 125 */
  3,	/* L_STRICT_INCLUDED */
  /* 126 */
  3,	/* L_NOT_INCLUDED */
  /* 127 */
  4,	/* L_NOT_STRICT_INCLUDED */
  /* 128 */
  2,	/* L_DIRECT_PRODUCT */
  /* 129 */
  2,	/* L_PARALLEL */
  /* 130 */
  2,	/* L_RESTRICTION */
  /* 131 */
  3,	/* L_ANTIRESTRICTION */
  /* 132 */
  2,	/* L_CORESTRICTION */
  /* 133 */
  3,	/* L_ANTICORESTRICTION */
  /* 134 */
  2,	/* L_LEFT_OVERLOAD */
  /* 135 */
  3,	/* L_PARTIAL_FUNCTION */
  /* 136 */
  3,	/* L_TOTAL_FUNCTION */
  /* 137 */
  3,	/* L_PARTIAL_INJECTION */
  /* 138 */
  3,	/* L_TOTAL_INJECTION */
  /* 139 */
  4,	/* L_PARTIAL_SURJECTION */
  /* 140 */
  4,	/* L_TOTAL_SURJECTION */
  /* 141 */
  4,	/* L_BIJECTION */
  /* 142 */
  1,	/* L_LAMBDA */
  /* 143 */
  3,	/* L_TRANS_FUNC */
  /* 144 */
  3,	/* L_TRANS_REL */
  /* 145 */
  3,	/* L_SEQUENCE */
  /* 146 */
  4,	/* L_NON_EMPTY_SEQUENCE */
  /* 147 */
  4,	/* L_INJECTIVE_SEQ */
  /* 148 */
  5,	/* L_NON_EMPTY_INJECTIVE_SEQ */
  /* 149 */
  4,	/* L_PERMUTATION */
  /* 150 */
  1,	/* L_CONCAT */
  /* 151 */
  2,	/* L_HEAD_INSERT */
  /* 152 */
  2,	/* L_TAIL_INSERT */
  /* 153 */
  3,	/* L_HEAD_RESTRICT */
  /* 154 */
  3,	/* L_TAIL_RESTRICT */
  /* 155 */
  4,	/* L_GENERALISED_CONCAT */
  /* 156 */
  1,	/* L_CONVERSE */
  /* 157 */
  2,	/* L_EMPTY_SEQ */
  /* 158 */
  3,	/* L_SET_RELATION */
  /* 159 */
  1,	/* L_PIPE */
  /* 160 */
  2,	/* L_SCOPE */
  /* 161 */
  6,	/* L_ASSERT */
  /* 162 */
  2,	/* L_OF */
  /* 163 */
  2,	/* L_DO */
  /* 164 */
  3,	/* L_ANY */
  /* 165 */
  5,	/* L_WHERE */
  /* 166 */
  2,	/* L_AFFECT */
  /* 167 */
  2,	/* L_BECOMES_MEMBER_OF */
  /* 168 */
  7,	/* L_ITERATE */
  /* 169 */
  7,	/* L_CLOSURE */
  /* 170 */
  8,	/* L_CLOSURE1 */
  /* 171 */
  0,	/* L_EVL */
  /* 172 */
  1,	/* L_UMINUS */
  /* 173 */
  2,	/* L_LR_SCOL */
  /* 174 */
  3,	/* L_LR_PARALLEL */
  /* 175 */
  3,	/* L_RENAMED_IDENTIFIER */
  /* 176 */
  3, 	/* L_REC */
  /* 177 */
  6,  /* L_REC */
  /* 178 */
  2,  /* L_LABEL_COL */
  /* 179 */
  1,  /* L_RECORD_ACCESS */
  /* 180 */
  10, /* L_LIB_FILE_NAME */
#ifdef COMPAT
  /* 181 */
  0,	/* L_VOID */
#endif
  /* 182 */
  4, // L_TREE
  /* 183 */
  5, // L_BTREE
  /* 184 */
  5, // L_CONST
  /* 185 */
  3, // L_TOP
  /* 186 */
  4, // L_SONS
  /* 187 */
  6, // L_PREFIX
  /* 188 */
  7, // L_POSTFIX
  /* 189 */
  5, // L_SIZET
  /* 190 */
  6, // L_MIRROR
  /* 191 */
  4, // L_RANK
  /* 192 */
  6, // L_FATHER
  /* 193 */
  3, // L_SON
  /* 194 */
  7, // L_SUBTREE
  /* 195 */
  5, // L_ARITY
  /* 196 */
  3, // L_BIN
  /* 197 */
  4, // L_LEFT
  /* 198 */
  5, // L_RIGHT
  /* 199 */
  5, // L_INFIX
  /* 200 */
  16,	/* L_LOCAL_OPERATIONS */
  /* 201 */
  6, /* L_SYSTEM */
  /* 202 */
  6, // L_EVENTS,
  /* 203 */
   6,	/* L_WITNESS */
#ifdef BALBULETTE
  /* 201 */
  2,	/* L_BECOMES_SUCH_THAT */
  /* 202 */
  5, // L_MODEL,
  /* 204 */
  8, // L_THEOREMS,
#endif
  /* 204 */
  10, // L_CPP_COMMENT
  /* 205 */
  14, // L_KEPT_CPP_COMMENT
  /* 206 */
  4,	/* L_REAL_TYPE */
  /* 207 */
  4,	/* L_REAL */
  /* 208 */
  5,	/* L_FLOAT_TYPE */
  /* 209 */
  6,	/* L_JUMPIF */
  /* 210 */
  6,	/* L_LABEL */
  /* 211 */
  4,	/* L_TO */
  /* 212 */
  4, // L_REALOP
  /* 213 */
  5, // L_FLOOR
  /* 214 */
  7, // L_CEILING
  /* 215 */
  3, /* L_REF */
  /* 216 */
  4,	/* L_SURJ_RELATION */
  /* 217 */
  4,	/* L_TOTAL_RELATION */
  /* 218 */
  5,	/* L_TOTAL_SURJ_RELATION */
} ;

static int use_english = TRUE ;
void french_lex_type(void)
{
  ASSERT(sizeof(french_ascii_lex_type) == (sizeof(english_ascii_lex_type))) ;
  use_english = FALSE ;
}

void english_lex_type(void)
{
  ASSERT(sizeof(french_ascii_lex_type) == (sizeof(english_ascii_lex_type))) ;
  use_english = TRUE ;
}

// " (les guillemets sont importants, ils remettent le mode hilight d'emacs
// sur de bonnes bases)
#ifdef BUILD_BCOMP
// Fonction auxiliaire qui calcule la table ascii_lex_type_len
static void compute_ascii_len(void)
{
  int i ;
  for (i = L_IDENT ; i <= L_RENAMED_IDENT ; i ++)
	{
	  printf("\t/* %03d */\t%d,\t/* %s */\n",
			 i,
			 strlen(ascii_lex_type[i]),
			 debug_lex_type[i]) ;
	}
}
#endif

//
//	}{	Constructeur
//

T_lexem::T_lexem(T_lex_type new_lex_type,
						  const char *new_value,
						  size_t new_value_len,
						  int new_file_line,
						  int new_file_column,
						  T_symbol *new_file_name,
						  int auto_chain)
  : T_object(NODE_LEXEM), value(new_value)
{
#ifdef DEBUG_LEX
  TRACE5("T_lexem::T_lexem(%s, %s, %d, %d) -> %x",
		 debug_lex_type[new_lex_type],
		 (new_value == NULL) ? "(NULL)" : new_value,
		 new_file_line,
		 new_file_column,
		 this) ;
#endif

  set_tmp2(FALSE) ;

  // On s'ajoute dans le gestionnaire d'objets
  get_object_manager()->add_lexem(this) ;

#ifdef BUILD_BCOMP
  // Mode particulier, lors de l'ecriture du compilateur
  // on veut la table des longueurs ascii
  compute_ascii_len() ;
  exit(0) ;
#endif

  ENTER_TRACE ;
  lex_type = new_lex_type ;
  file_line = new_file_line ;
  file_column = new_file_column ;
  position = 0 ;

#ifdef DEBUG_LEX
  TRACE3("%s : new_file_name %x:%s",
		 get_lex_ascii(),
		 new_file_name,
		 new_file_name->get_value()) ;
#endif // DEBUG_LEX

  ASSERT(object_test(new_file_name) == TRUE) ;
  ASSERT(new_file_name->is_a_symbol()) ;
  file_name = new_file_name ;
  ASSERT(file_column > 0) ;
  original_lexem = NULL ;
  rewrite_rule_lexem = NULL ;
  expanded = FALSE ;

  if (new_value == NULL)
	{
	  value_len = 0 ;
	  value = NULL ;
	}
  else
	{
	  T_symbol *str = lookup_symbol(new_value, new_value_len) ;
	  value = str->get_value() ;
	  value_len = str->get_len() ;
	}

  if (auto_chain == TRUE)
	{
	  if (lex_get_first_lexem() == NULL)
		{
		  lex_set_first_lexem(this) ;
		}
	  else
		{
		  lex_get_last_lexem()->next_lexem = this ;
		}

	  prev_lexem = lex_get_last_lexem() ;
	  next_lexem = NULL ;
	  lex_set_last_lexem(this) ;

	  next_expanded_lexem = prev_expanded_lexem = NULL ;
	  next_unsolved_comment = prev_unsolved_comment = NULL ;
	}
  if ( (lex_type == L_COMMENT) || (lex_type == L_KEPT_COMMENT) 
	  || (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT)
	  )
	{
	  get_object_manager()->add_unsolved_comment(this) ;
	}
  else
	{
	  // Mise a jour du checksum
	  if (value != NULL)
		{
		  // identificateur
		  MD5Update(lex_get_MD5_context(), (unsigned char *)value, value_len) ;
		}
	  else
		{
		  // Mot-cle
		  MD5Update(lex_get_MD5_context(),
					(unsigned char *)get_lex_ascii(),
					get_lex_ascii_len()) ;
		}
	}

  if (lex_get_push_next_lexem() == TRUE)
	{
	  push_first_lex_stack(this) ;
	  lex_set_push_next_lexem(FALSE) ;
	}

  if (is_a_comment() == FALSE)
	{
	  set_first_compo_code_lexem(this) ;


	  // On verifie que ce n'est pas un parasite en debut de fichier
	  if (    (get_first_component_started() == FALSE)
			  && (get_file_definitions_mode() == FALSE ) )
		{
		  // Parse error !
		  syntax_error(this,
					   CAN_CONTINUE,
					   get_error_msg(E_GARBAGE_BEFORE_COMPONENT)) ;
		}
	}

  EXIT_TRACE ;
#ifdef DEBUG_LEX
  TRACE2("fin de T_lexem(%x)::T_lexem(%s)", this, (value == NULL) ? "" : value) ;
#endif
}

//
//	}{	Constructeur de clonage
//
T_lexem::T_lexem(T_lexem *ref_lexem,
						  T_lexem *new_original_lexem,
						  T_lexem **adr_first,
						  T_lexem **adr_last)
  : T_object(NODE_LEXEM), value(ref_lexem->value)
{
#ifdef DEBUG_LEX
  TRACE4("T_lexem::T_lexem(%x, %x, %x) CLONAGE de %s",
		 ref_lexem, adr_first, adr_last, ref_lexem->get_lex_name()) ;

  ENTER_TRACE ;
#endif
  // On s'ajoute dans le gestionnaire d'objets
  get_object_manager()->add_lexem(this) ;

  lex_type = ref_lexem->lex_type ;
  file_line = ref_lexem->file_line ;
  file_column = ref_lexem->file_column ;
  file_name = ref_lexem->file_name ;
  original_lexem = new_original_lexem ;
  expanded = FALSE ;
  position = ref_lexem->position ;
  set_tmp2(ref_lexem->get_tmp2()) ;

  if (ref_lexem->value == NULL)
	{
	  value = NULL ;
	  value_len = 0 ;
	}
  else
	{
	  T_symbol *str = lookup_symbol(ref_lexem->value, ref_lexem->value_len) ;
	  value = str->get_value() ;
	  value_len = str->get_len() ;
	}

  // Chainage

  if (adr_first != NULL)
	{
	  if (*adr_last == NULL)
		{
		  // Chainage en tete
		  *adr_first = this ;
		  prev_lexem = NULL ;
		}
	  else
		{
		  // Chainage en queue
		  (*adr_last)->next_lexem = this ;
		  prev_lexem = *adr_last ;
		}

	  next_lexem = NULL ;
	  *adr_last = this ;
	}

  next_expanded_lexem = prev_expanded_lexem = NULL ;
  prev_unsolved_comment = next_unsolved_comment = NULL ;

  if ( (lex_type == L_COMMENT) || (lex_type == L_KEPT_COMMENT)
	  || (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT) )
	{
	  get_object_manager()->add_unsolved_comment(this) ;
	}

  if (lex_get_push_next_lexem() == TRUE)
	{
	  push_first_lex_stack(this) ;
	  lex_set_push_next_lexem(FALSE) ;
	}

#ifdef DEBUG_LEX
  EXIT_TRACE ;
  TRACE0("fin de T_lexem::T_lexem CLONAGE") ;
#endif
}

//
// }{ Constructeur de type :: inserer avant
//
T_lexem::T_lexem(T_lexem *before_lexem,
						  T_lex_type new_lex_type)
  : T_object(NODE_LEXEM)
{
#ifdef DEBUG_LEX
  TRACE2("T_lexem::T_lexem INSERT BEFORE(%x, %s)",
		 before_lexem, debug_lex_type[new_lex_type]) ;

  ENTER_TRACE ;
#endif

  // On s'ajoute dans le gestionnaire d'objets
  get_object_manager()->add_lexem(this) ;

  lex_type = new_lex_type ;
  file_line = before_lexem->file_line ;
  file_column = before_lexem->file_column ;
  file_name = before_lexem->file_name ;
  original_lexem = before_lexem->original_lexem ;
  expanded = before_lexem->expanded ;
  value = NULL ;
  value_len = 0 ;
  position = 0 ;
  set_tmp2(FALSE) ;

  // Chainage
  //
  //	Attention convention "inversees" par rapport a l'usage
  //					AFTER <---X----> BEFORE
  //					  ^                ^
  //					  |----> this <----|
  T_lexem *after_lexem = before_lexem->get_prev_lexem() ;

  set_next_lexem(before_lexem) ;
  set_prev_lexem(after_lexem) ;
  before_lexem->set_prev_lexem(this) ;

  if (after_lexem == NULL)
	{
	  lex_set_first_lexem(this) ;
	}
  else
	{
	  after_lexem->set_next_lexem(this) ;
	}

  // Attention, si on insere avant un lexeme qui a un champ prev_expanded_lexem
  // alors il faut detourner ce chainage. Sinon l'algo LR se plante quand
  // il fait get_prev() sur before_lexem : il recoit prev_expanded_lexem
  // et pas this
  if (before_lexem->prev_expanded_lexem != NULL)
	{
	  prev_expanded_lexem = before_lexem->prev_expanded_lexem ;
	  before_lexem->prev_expanded_lexem = NULL ;
	}
  next_expanded_lexem = NULL ;

  if ( (lex_type == L_COMMENT) || (lex_type == L_KEPT_COMMENT) 
	  || (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT) )
	{
	  get_object_manager()->add_unsolved_comment(this) ;
	}

  prev_unsolved_comment = next_unsolved_comment ;

  if (lex_get_push_next_lexem() == TRUE)
	{
	  push_first_lex_stack(this) ;
	  lex_set_push_next_lexem(FALSE) ;
	}

#ifdef DEBUG_LEX
  EXIT_TRACE ;
  TRACE0("fin de T_lexem::T_lexem INSERT BEFORE") ;
#endif
}

//
//	}{	Destructeur
//
T_lexem::~T_lexem(void)
{
#ifdef DEBUG_LEX
  TRACE6("debut T_lexem(%x)::~T_lexem() %s:%d:%d %s <%s>",
		 this,
		 get_file_name()->get_value(),
		 get_file_line(),
		 get_file_column(),
		 get_lex_type_ascii(lex_type),
		 (value == NULL)? "(null)" : value) ;
#endif
  ENTER_TRACE ;

#if 0
  // A FAIRE :: etudier
  (car ca crashe, il faut peaufiner les liens selon moi)
	if (value != NULL)
	  {
		delete [] value ;
	  }
#endif

  EXIT_TRACE ;
#ifdef DEBUG_LEX
  TRACE1("fin T_lexem(%x)::T_lexem()", this) ;
#endif
}

#ifdef DUMP_LEXEMS
//
//	]{	Dump
//
void T_lexem::dump_lexems(void)
{
#ifdef DEBUG_LEX
  TRACE1("dump_lexems(%x)", this) ;
#endif

  if (value == NULL)
	{
	  fprintf(stdout,"%s:%d:%d: Lexem %x type = %03d :: %s\n",
			  get_file_name()->get_value(),
			  file_line,
			  file_column,
			  (size_t)this,
			  lex_type,
			  debug_lex_type[lex_type]) ;
	  TRACE7("%s:%d:%d: Lexem type = %03d :: %s next %x next_expanded %x\n",
			 get_file_name()->get_value(),
			 file_line,
			 file_column,
			 lex_type,
			 debug_lex_type[lex_type],
			 next_lexem,
			 next_expanded_lexem) ;
	}
  else
	{
	  fprintf(stdout,"%s:%d:%d: Lexem %x type = %03d :: %s value = %x \"%s\"\n",
			  get_file_name()->get_value(),
			  file_line,
			  file_column,
			  (size_t)this,
			  lex_type,
			  debug_lex_type[lex_type],
			  (size_t)value,
			  value) ;
	  TRACE9("%s:%d:%d: Lexem %x type = %03d :: %s value = \"%s\"\n next %x next_expanded %x",
			 get_file_name()->get_value(),
			 file_line,
			 file_column,
			 this,
			 lex_type,
			 debug_lex_type[lex_type],
			 value,
			 next_lexem,
			 next_expanded_lexem) ;
	}

  TRACE3("prev_line %x start_of_line %x next_line %x",
		 prev_line, start_of_line, next_line) ;

#if 0
  if (prev_line != NULL)
	{
	  fprintf(stdout,
			  "%s:%d:%d: prev line\n",
			  get_file_name()->get_value(),
			  prev_line->get_file_line(),
			  prev_line->get_file_column()) ;
	  TRACE3("%s:%d:%d: prev line\n",
			 get_file_name(),
			 prev_line->get_file_line(),
			 prev_line->get_file_column()) ;
	}

  if (next_line != NULL)
	{
	  TRACE3("%s:%d:%d: next line\n",
			 get_file_name()->get_value(),
			 next_line->get_file_line(),
			 next_line->get_file_column()) ;
	}
#endif

  T_lexem *after ;
  after = (next_expanded_lexem == NULL) ? next_lexem : next_expanded_lexem ;

  if (after != NULL)
	{
	  after->dump_lexems() ;
	}
}
#endif

const char *T_lexem::get_lex_name(void)
{
  if (  	(lex_type == L_IDENT)
			|| (lex_type == L_COMMENT)
			|| (lex_type == L_KEPT_COMMENT)
			|| (lex_type == L_STRING)
			|| (lex_type == L_LIB_FILE_NAME)
			|| (lex_type == L_NUMBER) 
			|| (lex_type == L_REAL)
			|| (lex_type == L_CPP_COMMENT)
			|| (lex_type == L_CPP_KEPT_COMMENT)
			)
	{
	  return value ;
	}

  // on saute le 'L_'
  const char *res = debug_lex_type[lex_type] ;
#ifdef DEBUG_LEX
  TRACE1("on rend %s sans les 2 premiers caracteres", res) ;
#endif
  return (res + 2) ;
}

// Pour savoir si un lexeme est un nom de clause
int is_a_clause(T_lexem *lexem)
{
#ifdef DEBUG_LEX
  TRACE2("is_a_clause(%s)->%s",
		 lexem->get_lex_ascii(),
		 (lex_is_clause[lexem->get_lex_type()] == TRUE) ? "TRUE" : "FALSE") ;
#endif
  // La reponse est facile a donner, sauf pour "INVARIANT" et "VARIANT" qui
  // sont a la fois un nom de clause et un mot-cle (dans WHILE)
  // pour le savoir il faut retourner en arriere.
  // Si on on trouve un WHILE avant un dep_clause c'etait un
  // invariant de boucle. Sinon c'est une clause

  if (lexem->get_lex_type() != L_INVARIANT && lexem->get_lex_type() != L_VARIANT)
	{
	  return lex_is_clause[lexem->get_lex_type()] ;
	}

  // Ici on a un invariant ou un variant, il faut �tudier !
  TRACE3("ETUDE DE L'INVARIANT ou du VARIANT %s:%d:%d",
		 lexem->get_file_name()->get_value(),
		 lexem->get_file_line(),
		 lexem->get_file_column()) ;
  T_lexem *cur_lexem = lexem->get_prev_lexem() ;

  while (cur_lexem != NULL)
	{
	  if (cur_lexem->get_lex_type() == L_WHILE)
		{
		  // invariant de boucle
		  TRACE0("invariant de boucle") ;
		  return FALSE ;
		}

	  if (is_a_clause(cur_lexem) == TRUE)
		{
		  // clause
		  TRACE0("clause") ;
		  return TRUE ;
		}

	  cur_lexem = cur_lexem->get_prev_lexem() ;
	}

  // Ici on a tout remonter sans rien trouver. C'est donc une
  // clause (cas degenere !!!)
  return TRUE ;
}

// Pour savoir si un lexeme est un nom de clause de debut de composant
int is_a_begin_component_clause(T_lex_type lex_type)
{
  // Le code marche car L_MACHINE, L_REFINEMENT et L_IMPLEMENTATION
  // doivent etre declares successivement et dans cet ordre
#ifdef BALBULETTE
  if ( (lex_type >= L_MACHINE) && (lex_type <= L_IMPLEMENTATION) ||
	   (lex_type == L_MODEL) )
#else
  if ( (lex_type >= L_MACHINE) && (lex_type <= L_IMPLEMENTATION) ||
		  (lex_type == L_SYSTEM)  )
#endif
	{
	  return TRUE ;
	}

  return FALSE ;
}

// Pour savoir si un lexeme est un nom de clause de dependance
int is_a_dep_clause(T_lexem *lexem)
{
  return lex_is_dep_clause[lexem->get_lex_type()] ;
}

//
// }{ Obtention du nom de lexeme a partir d'un lex_type
// exemple : get_lex_type_name(L_IDENT) -> L_IDENT"
const char *get_lex_type_name(T_lex_type lex_type)
{
  return debug_lex_type[lex_type] ;
}

// Obtention de l'origine ASCII d'un lexeme a partir d'un lex_type
// exemple : get_lex_type_name(L_OPERATIONS) -> "OPERATIONS
const char *get_lex_type_ascii(T_lex_type lex_type)
{
  //#ifdef DEBUG_LEX
  //TRACE1("L_IDENT=%s", english_ascii_lex_type[L_IDENT]) ;
  //TRACE1("res = %s", english_ascii_lex_type[lex_type]) ;
  //#endif
  return (use_english == TRUE)
	? english_ascii_lex_type[lex_type]
	: french_ascii_lex_type[lex_type] ;
}

// Obtention de longueur de l'origine ASCII d'un lexeme a partir d'un lex_type
// exemple : get_lex_type_ascii(L_OPERATIONS) -> "OPERATIONS"
//           get_lex_type_ascii_len(L_OPERATIONS) -> 10
// VALIDE SEULEMENT POUR LES MOTS CLES
size_t get_lex_type_ascii_len(T_lex_type lex_type)
{
#ifdef DEBUG_LEX
  TRACE1("L_IDENT=%s", english_ascii_lex_type[L_IDENT]) ;
  TRACE1("res = %s", english_ascii_lex_type[lex_type]) ;
#endif
  return ascii_lex_type_len[lex_type] ;
}

// Renvoie TRUE si le lexeme courant est avant ref_lexem dans le fichier
// Renvoie FALSE sinon
int T_lexem::is_before(T_lexem *ref_lexem)
{
  if (file_line < ref_lexem->file_line)
	{
	  return TRUE ;
	}
  else if (file_line > ref_lexem->file_line)
	{
	  return FALSE ;
	}

  // Ici on sait qu'ils sont sur la meme ligne
  if (file_column < ref_lexem->file_column)
	{
	  return TRUE ;
	}

  return FALSE ;
}

static const char *const open_string_scp = "\"" ;
static const char *const close_string_scp = "\"" ;
const char *get_open_string(void)
{
  return open_string_scp ;
}

const char *get_close_string(void)
{
  return close_string_scp ;
}

void T_lexem::set_next_unsolved_comment(T_lexem *next_unsolved)
{
#ifdef DEBUG_COMMENT
  TRACE2("T_lexem(%x)::set_next_unsolved_comment(%x)", this, next_unsolved) ;
#endif
  next_unsolved_comment = next_unsolved ;
}

void T_lexem::set_prev_unsolved_comment(T_lexem *prev_unsolved)
{
#ifdef DEBUG_COMMENT
  TRACE2("T_lexem(%x)::set_prev_unsolved_comment(%x)", this, prev_unsolved) ;
#endif
  prev_unsolved_comment = prev_unsolved ;
}

T_lexem *T_lexem::get_real_next_lexem(void) { return next_lexem ; }
T_lexem *T_lexem::get_real_prev_lexem(void) { return prev_lexem ; }

#ifdef MATRA
//Ajout CT le 23.07.97 : resolution du probleme ancienne chaine/nouvelle chaine
//anomalie matra 001 .
//les expressions max{a,b} sont remplacees par max({a,b}).
//Il en est de mem pour tous les operateurs predefinis max, min, size, conc....
int is_a_builtin_lexem( T_lexem *lexem )
{
  //seuls les lexems associes aux operateurs unaires builtin max, min...
  //nous interesse.
  // il s'agit des operateurs dans l'enumere T_lex_type
  //depuis L_MAXC jusqu'a L_L_NOT.
  ASSERT( lexem != NULL ) ;

  T_lex_type type = lexem->get_lex_type() ;

  if ( (type >= L_MAX) && (type < L_L_OR) )
	{
	  return TRUE ;
	}//end if

  return FALSE ;

}


// Ajout d'un Lexem L_OPEN(CLOSE)_PAREN AVANTle lexem passe en parametre
void add_L_PAREN( T_lex_type type, T_lexem *before_lexem )
{
  ASSERT( before_lexem != NULL ) ;

  //creation du lexem L_OPEN(CLOSE)_PAREN
  T_lexem *new_lex = new T_lexem( before_lexem, type ) ;

  new_lex->set_value( NULL );
  new_lex->set_value_len(0) ;
  new_lex->set_file_name(before_lexem->get_file_name()) ;
  new_lex->set_file_line(before_lexem->get_file_line()) ;
  new_lex->set_file_column(before_lexem->get_file_column()) ;

#ifdef DEBUG_PAREN
  fprintf( stdout, "creation du lexem : %x\n", new_lex ) ;
#endif
}


//Destruction d'un Lexem L_OPEN(CLOSE)_PAREN passe en parametre
//le chainage entre les lexemes de la liste doit etre mis a jour
void delete_L_PAREN( T_lex_type type, T_lexem *lexem )
{
  ASSERT((lexem != NULL) &&
		 (lexem->get_lex_type() == type) ) ;

  //mise a jour chainage avant et arriere
  T_lexem *next_lexem = lexem->get_next_lexem() ;
  T_lexem *prev_lexem = lexem->get_prev_lexem() ;

  if ( prev_lexem != NULL )
	{

	  prev_lexem->set_next_lexem( next_lexem ) ;
	}//end if ( prev_lexem != NULL )

  if ( next_lexem != NULL )
	{

	  next_lexem->set_prev_lexem( prev_lexem ) ;

	}//end if (next_lexem != NULL )


#ifdef DEBUG_PAREN
  fprintf( stdout, "destruction du lexem : %x\n", lexem ) ;
#endif
}


//Detection du lexeme associe a "}".
//insertion apres ce lexeme d'un L_CLOSE_PAREN
void find_and_add_to_end_of_expr( T_lexem *from_lexem )
{

  ASSERT( from_lexem != NULL ) ;

  T_lex_type type = from_lexem->get_lex_type() ;
  ASSERT( type == L_OPEN_BRACES ) ;

  //close_braces est a TRUE si on a trouve l'accolade fermante
  //correspondante.
  int close_braces = FALSE ;

  //compteur du nombre d'accolade fermant a trouver avant
  //de detecter la bonne (cas max{a, max {b, c}}).
  int nb_other_braces = 0 ;

  //la recherche debut apres le L_OPEN_BRACES courant.
  from_lexem = from_lexem->get_next_lexem() ;


  //tant qu'il y a des lexemes et
  //que le lexeme L_CLOSE_BRACES n'a pas ete trouve
  while ( (from_lexem != NULL) &&
		  (close_braces == FALSE ) )
	{

	  ASSERT( from_lexem != NULL ) ;

	  //type du lexeme
	  type = from_lexem->get_lex_type() ;

	  if ( type == L_OPEN_BRACES )
		{

		  //on a trouve une accolade ouvrante
		  //on increment le compteur.
		  nb_other_braces = nb_other_braces + 1 ;

		}
	  else if ( ( type == L_CLOSE_BRACES ) &&
				( nb_other_braces > 0 ) )
		{
		  //on a trouve une accolade fermante
		  //mais ce n'est pas la bonne....
		  //il faut decremente le compteur
		  nb_other_braces = nb_other_braces - 1 ;

		}
	  else if ( type == L_CLOSE_BRACES )
		{
		  //c'est la bonne
#ifdef DEBUG_PAREN
		  fprintf(stdout," on a trouve l'accolade fermante!!!\n") ;
#endif
		  //on a trouve l'accolade fermante !
		  close_braces = TRUE ;

		  //on cree alors un L_CLOSE_PAREN avant le
		  // lexeme suivant }
		  add_L_PAREN(L_CLOSE_PAREN, from_lexem->get_next_lexem() ) ;

		}//end if ...


	  //on passe au lexeme suivant....
	  from_lexem = from_lexem->get_next_lexem() ;

	}//end while

  ASSERT( close_braces == TRUE ) ;


}


//recherche a partir du lexem fdrom_lexem le lexeme )
//en tenant compte des eventuelles () inserees.
//si la ) est suivie du lexem } : l'expression est du type
//           {(a,b,c,d,e,f)} ; il faut la remplacer par {a,b,c,d,e,f}.
//Dans ce cas, cette fonction retourne TRUE, FALSE sinon
int find_and_del_L_PAREN( T_lexem *from_lexem )
{

  ASSERT( from_lexem != NULL ) ;

  T_lex_type type = from_lexem->get_lex_type() ;
  ASSERT( type == L_OPEN_PAREN ) ;

  //close_paren est a TRUE si on a trouve la parenthese fermante
  //correspondante
  int close_paren = FALSE ;

  //flag d'information pour l'appelant
  int to_delete = FALSE ;

  //compteur du nombre de parenthese fermante a trouver avant
  //de detecter la bonne (cas (a,b,(c,d),e) ).
  int nb_other_paren = 0 ;

  //la recherche debute apres le L_OPEN_PAREN courant.
  from_lexem = from_lexem->get_next_lexem() ;


  //tant qu'il y a des lexemes et
  //que le lexeme L_CLOSE_PAREN n'a pas ete trouve
  while ( (from_lexem != NULL) &&
		  (close_paren == FALSE ) )
	{

	  ASSERT( from_lexem != NULL ) ;

	  //type du lexeme
	  type = from_lexem->get_lex_type() ;

	  if ( type == L_OPEN_PAREN )
		{

		  //on a trouve une parenthese ouvrante
		  //on increment le compteur.
		  nb_other_paren = nb_other_paren + 1 ;

		}
	  else if ( ( type == L_CLOSE_PAREN ) &&
				( nb_other_paren > 0 ) )
		{
		  //on a trouve une parenthese fermante
		  //mais ce n'est pas la bonne....
		  //il faut decremente le compteur
		  nb_other_paren = nb_other_paren - 1 ;

		}
	  else if ( type == L_CLOSE_PAREN )
		{
		  //c'est la bonne
#ifdef DEBUG_PAREN
		  fprintf(stdout," on a trouve la parenthese fermante!!!\n") ;
#endif
		  //on a trouve la parenthese fermante !
		  close_paren = TRUE ;

		}//end if ...


	  //on passe au lexeme suivant....
	  from_lexem = from_lexem->get_next_lexem() ;

	}//end while

  ASSERT( close_paren == TRUE ) ;

  //from_lexem pointe sur le lexem suivant la parenthese fermante.
  if ( from_lexem != NULL &&
	   from_lexem->get_lex_type() == L_CLOSE_BRACES )
	{

	  //expression du type {(a,b,c)}
	  //1ere etape : transformation en {(a,b,c}
	  //la 2nde etape est realisee par l'appelant
	  delete_L_PAREN( L_CLOSE_PAREN, from_lexem->get_prev_lexem() ) ;

	  //flag pour 2nde etape.
	  to_delete = TRUE ;

	}//end if


  return to_delete ;
}


//CETTE OPERATION DOIT ETRE APPELEE APRES EXPANSION DES DEFINITIONS
//cette fonction resoud les pbs du type : max {a, min{c, d}} en max({a, min({c, d})})
//                   et les pbs du type : {(a,b,c,d,e,f)} en {a,b,c,d,e,f}
extern void find_builtin_and_add_L_PAREN( T_lexem *from_lexem )
{

  T_lexem *next_lexem = NULL ;
  T_lex_type next_type = L_IDENT ; //pour warning

  //il faut parcourir la liste des lexemes depuis from_lexem
  //jusqu'a la fin de la liste.

  while ( from_lexem != NULL )
	{

	  next_lexem = from_lexem->get_next_lexem() ;

	  //seuls les operateurs de type builtin nous interesse
	  if ( is_a_builtin_lexem( from_lexem ) == TRUE )
		{
#ifdef DEBUG_PAREN
		  fprintf(stdout," is a builtin ...type : %d, next : %d",
				  from_lexem->get_lex_type(),
				  next_lexem->get_lex_type()) ;
#endif

	      //si le lexeme suivant n'est pas NULL
		  if ( next_lexem != NULL )
			{

			  next_type = next_lexem->get_lex_type() ;

			  //en fonction du type du lexeme suivant
			  switch ( next_type )
				{

				case L_OPEN_BRACES :
				  {
					//si le lexem suivant est une "{" (L_OPEN_BRACES)
#ifdef DEBUG_PAREN
					fprintf(stdout," ADDING L_PAREN before {...") ;
#endif

					//il faut rajouter un L_OPEN_PAREN AVANT next
					add_L_PAREN( L_OPEN_PAREN, next_lexem ) ;

					//il faut detecter la fin de l'expression "}" (L_CLOSE_BRACES)
					//est inserer alors un L_CLOSE_PAREN
					find_and_add_to_end_of_expr( next_lexem ) ;

					break ;

			      }
				case L_IDENT :
				  {
					//sinon si le lexeme suivant est un identificateur

#ifdef DEBUG_PAREN
					fprintf(stdout," ADDING L_PAREN before ident...") ;
#endif
					//il faut rajouter un L_OPEN_PAREN AVANT next
					add_L_PAREN( L_OPEN_PAREN, next_lexem ) ;

					//il faut rajouter un L_CLOSE_PAREN APRES next
					next_lexem = next_lexem->get_next_lexem() ;
					add_L_PAREN( L_CLOSE_PAREN, next_lexem ) ;

					break ;
				  }
				default :
				  {

					//fall into .... on ne fait rien.
#ifdef DEBUG_PAREN
					fprintf(stdout," DEfault : next_type : %d \n", next_type ) ;
#endif

	                break ;
				  }


				}//end switch

			}//end if next_lexem != NULL

		}
	  else if ( from_lexem->get_lex_type() == L_OPEN_BRACES)
		{

		  if ( next_lexem != NULL &&
			   (next_lexem->get_lex_type() == L_OPEN_PAREN) )
			{

			  //l'expression debute par "{("
			  //si l'expression est du type {(a,b,c,d)} , il faut la
			  //remplacer par {a,b,c,d}.

			  //reference le lexeme qu'il faudra peut-etre dechainer
			  T_lexem *paren_to_delete = next_lexem ;

			  //il faut donc detecter la ) fermante associee, si elle
			  int to_delete = find_and_add_to_end_of_exprnd_and_del_L_PAREN( next_lexem ) ;

			  if (to_delete == TRUE)
				{

				  // la parenthese fermante a ete retiree
				  //on enleve la parenthese ouvrante referencee
				  delete_L_PAREN(L_OPEN_PAREN, paren_to_delete) ;

				}//end if (to_delete == TRUE )


			}//end if

		}//end if

#ifdef DEBUG_PAREN
	  fprintf(stdout,"\n") ;
#endif

	  //on passe au lexeme suivant qui est toujours next_lexem
	  //puisqu'on peut avoir un expression de la forme
	  //   max {a, max{b,c}} !!!!
	  from_lexem = next_lexem ;

	}//end while ...


}

#endif // MATRA

// Liberation d'un flux de lexemes
void unlink_lexems(T_lexem *cur_lexem, int inside_expansion)
{
  TRACE1("unlink_lexems(%x)", cur_lexem) ;
  if (cur_lexem == NULL)
	{
	  return ;
	}
  T_lexem *next = cur_lexem->get_next_lexem() ;
  T_lexem *next_unexpanded = cur_lexem->get_real_next_lexem() ;
  T_lexem *next_raw = lex_skip_comments(next_unexpanded) ;

  if (next_raw != next_unexpanded)
	{
	  T_lexem *cur = next_unexpanded ;

	  while (cur != next_raw)
		{
		  T_lexem *cur_next = cur->get_real_next_lexem() ;
		  delete cur ;
		  cur = cur_next ;
		}
	}


  delete cur_lexem ;

  if (inside_expansion == TRUE)
	{
	  // On s'arrete la pour ne pas liberer deux fois le meme chemin
	  // (cas de l'expansion de f(g(x)) ou f et g sont des definitions
	  TRACE0("inside_expansion=TRUE->on sort") ;
	  return ;
	}

  inside_expansion = FALSE ;

  TRACE2("next %x, next_raw %x", next, next_raw) ;
  /*
	// En cas d'expansion des definitions, il faut liberer
	// les deux chemins. Par defaut, on libere le chemin
	// expanse. (F, G, H). Il faut aussi liberer la source
	// (B, C, D, E)
	//          next_raw
	//  ---> A ---> B ---> C ---> D ---> E ---> F
	//          |   /\     /\     /\   /\
	//          |    \orig. \ lexem\  /     |
	//          ----> F ---> G ---> H -------
	//          next
	// Pour cela on met E->next a NULL et on lance recursivement
	// la liberation a partir de B.
	// Comment trouver E ? c'est F->get_prev_lexem(). Et F est
	// le premier lexeme du flux "next" pour lequel original_lexem = NULL
  */
  if (next_raw != next)
	{
	  inside_expansion = TRUE ;
	  // A l'entree de la boucle, next_raw = B et cur = next = F
	  TRACE1("ici next_raw %x", next_raw) ;
	  TRACE1("ici next %x", next) ;
	  TRACE1("ici next_raw %s", next_raw->get_lex_ascii()) ;
	  TRACE1("ici next %s", next->get_lex_ascii()) ;
	  T_lexem *cur = next ;
	  T_lexem *prev = NULL ;
	  while (cur->get_original_lexem() != NULL)
		{
		  TRACE2("dans la boucle cur=%x original_lexem %x",
				 cur,
				 cur->get_original_lexem()) ;
		  prev = cur ;
		  cur = cur->get_next_lexem() ;
		}

	  // Ici cur = F, on coupe le flux apres E
	  cur->get_prev_lexem()->set_next_lexem(NULL) ;
	}

  if (next != NULL)
	{
	  TRACE1(">> on libere le flux a partir de %x", next) ;
	  ENTER_TRACE ; ENTER_TRACE ;
	  unlink_lexems(next, inside_expansion) ;
	  EXIT_TRACE ; EXIT_TRACE ;
	  TRACE1("<< on libere le flux a partir de %x", next) ;
	}

}

#ifdef COMPAT
// Methode qui permet de dire qu'un lexeme doit etre ignore par le
// decompilateur. Si le lexeme provient de l'expansion d'une
// definition, on propage la correction a la definition elle-meme
void T_lexem::mark_as_void(void)
{
  TRACE4("T_lexem(%x: %s:%d:%d)::mark_as_void",
		 this,
		 file_name->get_value(),
		 file_line,
		 file_column) ;

  lex_type = L_VOID ;

  verbose_describe("%s:%d:%d: Ignored lexem marked as void\n",
				   file_name->get_value(),
				   file_line,
				   file_column) ;
  if (rewrite_rule_lexem != NULL)
	{
	  rewrite_rule_lexem->mark_as_void() ;
	}
}
#endif // COMPAT



//T_symbol * T_lexem::get_checksum(void)
//{
//    T_symbol * ret = 0;
//    char MD5_key[33] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};


//    ret = new T_symbol(MD5_key, 33, 0, 0, 0);

//    return ret;
//}
//
// Fin de c_lex.cpp
//
