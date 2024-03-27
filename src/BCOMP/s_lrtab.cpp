/******************************* CLEARSY **************************************
* Fichier : $Id: s_lrtab.cpp,v 2.0 2005-04-25 10:46:10 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Tables de l'analyseur LR
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
RCS_ID("$Id: s_lrtab.cpp,v 1.18 2005-04-25 10:46:10 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_lrtab.h"

//
//	}{	Familles de lexemes
//
static const char *lex_family_name_sct[] =
	{
	"ATOM",			// atomes : identificateurs, entiers, booleens
	"UNR",			// operateurs unaires
	"BIN",			// operateurs binaires
	"OPN",			// lexemes ouvrants
	"CLO",			// lexemes fermants
	"PST",			// tilde
	"END"			// fin de fichier ou lexemes inconnus pour l'analyseur LR
	} ;

//
//	}{	Table type de lexeme -> famille de lexeme
//
static T_lexem_family lexem_family_set[] =
	{
	/* 000 L_IDENT					*/	FAMILY_ATOM,
	/* 001 L_COMMENT				*/	FAMILY_END,
	/* 002 L_KEPT_COMMENT			*/	FAMILY_END,
	/* 003 L_STRING					*/	FAMILY_ATOM,
	/* 004 L_NUMBER					*/	FAMILY_ATOM,
	/* 005 L_OPEN_PAREN				*/	FAMILY_OPN,
	/* 006 L_CLOSE_PAREN			*/	FAMILY_CLO,
	/* 007 L_OPEN_BRACES			*/	FAMILY_OPN,
	/* 008 L_CLOSE_BRACES			*/	FAMILY_CLO,
	/* 009 L_OPEN_BRACKET			   	*/	FAMILY_OPN,
	/* 010 L_CLOSE_BRACKET			   	*/	FAMILY_CLO,
	/* 011 L_SCOL					*/	FAMILY_END,
	/* 012 L_COMMA					*/	FAMILY_BIN,
	/* 013 L_DOT					*/	FAMILY_BIN,
	/* 014 L_MACHINE		   		*/	FAMILY_END,
	/* 015 L_REFINEMENT		   		*/	FAMILY_END,
	/* 016 L_IMPLEMENTATION	   		*/	FAMILY_END,
	/* 017 L_BEGIN					*/	FAMILY_END,
	/* 018 L_END					*/	FAMILY_END,
	/* 019 L_SEES					*/	FAMILY_END,
	/* 020 L_INCLUDES				*/	FAMILY_END,
	/* 021 L_PROMOTES				*/	FAMILY_END,
	/* 022 L_IMPORTS				*/	FAMILY_END,
	/* 023 L_REFINES				*/	FAMILY_END,
	/* 024 L_EXTENDS				*/	FAMILY_END,
	/* 025 L_USES					*/	FAMILY_END,
	/* 026 L_CONSTRAINTS			*/	FAMILY_END,
	/* 027 L_DEFINITIONS			*/	FAMILY_END,
	/* 028 L_INVARIANT				*/	FAMILY_END,
	/* 029 L_ASSERTIONS				*/	FAMILY_END,
	/* 030 L_VALUES					*/	FAMILY_END,
	/* 031 L_SETS					*/	FAMILY_END,
	/* 032 L_CONSTANTS				*/	FAMILY_END,
	/* 033 L_HIDDEN_CONSTANTS		*/	FAMILY_END,
	/* 034 L_PROPERTIES				*/	FAMILY_END,
	/* 035 L_VARIABLES				*/	FAMILY_END,
	/* 036 L_HIDDEN_VARIABLES		*/	FAMILY_END,
	/* 037 L_INITIALISATION			*/	FAMILY_END,
	/* 038 L_OPERATIONS			   	*/	FAMILY_END,
	/* 039 L_BOOL					*/	FAMILY_UNR,
	/* 040 L_MOD					*/	FAMILY_BIN,
	/* 041 L_SUCC					*/	FAMILY_ATOM,
	/* 042 L_PRED					*/	FAMILY_ATOM,
	/* 043 L_MAX					*/	FAMILY_UNR,
	/* 044 L_MIN					*/	FAMILY_UNR,
	/* 045 L_CARD					*/	FAMILY_UNR,
	/* 046 L_SIGMA					*/	FAMILY_UNR,
	/* 047 L_PI					   	*/	FAMILY_UNR,
	/* 048 L_GUNION					*/	FAMILY_UNR,
	/* 049 L_GINTER					*/	FAMILY_UNR,
	/* 050 L_QUNION					*/	FAMILY_UNR,
	/* 051 L_QINTER					*/	FAMILY_UNR,
	/* 052 L_ID						*/	FAMILY_UNR,
	/* 053 L_PRJ1					*/	FAMILY_UNR,
	/* 054 L_PRJ2					*/	FAMILY_UNR,
	/* 055 L_DOM					*/	FAMILY_UNR,
	/* 056 L_RAN					*/	FAMILY_UNR,
	/* 057 L_SIZE					*/	FAMILY_UNR,
	/* 058 L_FIRST					*/	FAMILY_UNR,
	/* 059 L_LAST					*/	FAMILY_UNR,
	/* 060 L_FRONT					*/	FAMILY_UNR,
	/* 061 L_TAIL					*/	FAMILY_UNR,
	/* 062 L_REV					*/	FAMILY_UNR,
	/* 063 L_L_OR					*/	FAMILY_BIN,
	/* 064 L_L_NOT					*/	FAMILY_UNR,
	/* 065 L_PLUS					*/	FAMILY_BIN,
	/* 066 L_MINUS					*/	FAMILY_BIN,
	/* 067 L_TIMES					*/	FAMILY_BIN,
	/* 068 L_DIVIDES			   	*/	FAMILY_BIN,
	/* 069 L_POWER					*/	FAMILY_BIN,
	/* 070 L_UNION					*/	FAMILY_BIN,
	/* 071 L_INTERSECT			   	*/	FAMILY_BIN,
	/* 072 L_INTERVAL			   	*/	FAMILY_BIN,
	/* 073 L_EQUAL					*/	FAMILY_BIN,
	/* 074 L_REWRITES			   	*/	FAMILY_END,
	/* 075 L_DIFFERENT			   	*/	FAMILY_BIN,
	/* 076 L_LESS_THAN 				*/	FAMILY_BIN,
	/* 077 L_LESS_THAN_OR_EQUAL	   	*/	FAMILY_BIN,
	/* 078 L_GREATER_THAN	   	   	*/	FAMILY_BIN,
	/* 079 L_GREATER_THAN_OR_EQUAL_THAN	*/	FAMILY_BIN,
	/* 080 L_AND					*/	FAMILY_BIN,
   	/* 081 L_VAR					*/	FAMILY_END,
	/* 082 L_IN						*/	FAMILY_END,
	/* 083L_IF						*/	FAMILY_END,
	/* 084 L_THEN					*/	FAMILY_END,
	/* 085 L_ELSIF					*/	FAMILY_END,
	/* 086 L_ELSE					*/	FAMILY_END,
	/* 087 L_CASE					*/	FAMILY_END,
	/* 088 L_EITHER					*/	FAMILY_END,
	/* 089 L_OR						*/	FAMILY_END,
	/* 090 L_SKIP					*/	FAMILY_END,
	/* 091 L_PRE					*/	FAMILY_END,
	/* 092 L_CHOICE					*/	FAMILY_END,
	/* 093 L_SELECT					*/	FAMILY_END,
	/* 094 L_WHEN					*/	FAMILY_END,
	/* 095 L_WHILE					*/	FAMILY_END,
	/* 096 L_VARIANT				*/	FAMILY_END,
	/* 097 L_LET					*/	FAMILY_END,
	/* 098 L_BE						*/	FAMILY_END,
   	/* 099 L_NAT					*/	FAMILY_ATOM,
	/* 100 L_NAT1					*/	FAMILY_ATOM,
	/* 101 L_INT					*/	FAMILY_ATOM,
	/* 102 L_TRUE					*/	FAMILY_ATOM,
	/* 103L_FALSE					*/	FAMILY_ATOM,
	/* 104 L_MAXINT					*/	FAMILY_ATOM,
	/* 105 L_MININT					*/	FAMILY_ATOM,
	/* 106 L_EMPTY_SET				*/	FAMILY_ATOM,
	/* 107 L_INTEGER				*/	FAMILY_ATOM,
	/* 108 L_NATURAL				*/	FAMILY_ATOM,
	/* 109 L_NATURAL1				*/	FAMILY_ATOM,
	/* 110 L_BOOL_TYPE				*/	FAMILY_ATOM,
	/* 111 L_STRING_TYPE			*/	FAMILY_ATOM,
	/* 112 L_POW					*/	FAMILY_UNR,
	/* 113L_POW1					*/	FAMILY_UNR,
	/* 114 L_FIN					*/	FAMILY_UNR,
	/* 115 L_FIN1					*/	FAMILY_UNR,
  	/* 116 L_RETURNS				*/	FAMILY_BIN,
	/* 117 L_MAPLET					*/	FAMILY_BIN,
	/* 118 L_FORALL					*/	FAMILY_UNR,
	/* 119 L_EXISTS					*/	FAMILY_UNR,
	/* 120 L_IMPLIES				*/	FAMILY_BIN,
	/* 121 L_EQUIVALENT					*/	FAMILY_BIN,
	/* 122 L_BELONGS				*/	FAMILY_BIN,
	/* 123 L_NOT_BELONGS			*/	FAMILY_BIN,
	/* 124 L_INCLUDED				*/	FAMILY_BIN,
	/* 125 L_STRICT_INCLUDED		*/	FAMILY_BIN,
	/* 126 L_NOT_INCLUDED			*/	FAMILY_BIN,
	/* 127 L_NOT_STRICT_INCLUDED	*/	FAMILY_BIN,
	/* 128 L_DIRECT_PRODUCT			*/	FAMILY_BIN,
	/* 129 L_PARALLEL				*/	FAMILY_END,
	/* 130 L_RESTRICTION			*/	FAMILY_BIN,
	/* 131 L_ANTIRESTRICTION		*/	FAMILY_BIN,
	/* 132 L_CORESTRICTION			*/	FAMILY_BIN,
	/* 133 L_ANTICORESTRICTION		*/	FAMILY_BIN,
	/* 134 L_LEFT_OVERLOAD			*/	FAMILY_BIN,
	/* 135 L_PARTIAL_FUNCTION		*/	FAMILY_BIN,
	/* 136 L_TOTAL_FUNCTION			*/	FAMILY_BIN,
	/* 137 L_PARTIAL_INJECTION		*/	FAMILY_BIN,
	/* 138 L_TOTAL_INJECTION		*/	FAMILY_BIN,
	/* 139 L_PARTIAL_SURJECTION		*/	FAMILY_BIN,
	/* 140 L_TOTAL_SURJECTION		*/	FAMILY_BIN,
	/* 141 L_BIJECTION				*/	FAMILY_BIN,
	/* 142L_LAMBDA					*/	FAMILY_UNR,
	/* 143 L_TRANS_FUNC				*/	FAMILY_UNR,
	/* 144 L_TRANS_REL				*/	FAMILY_UNR,
	/* 145 L_SEQUENCE				*/	FAMILY_UNR,
	/* 146 L_NON_EMPTY_SEQUENCE	   	*/	FAMILY_UNR,
	/* 147 L_INJECTIVE_SEQ			*/	FAMILY_UNR,
	/* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	FAMILY_UNR,
	/* 149 L_PERMUTATION			*/	FAMILY_UNR,
	/* 150 L_CONCAT					*/	FAMILY_BIN,
	/* 151 L_HEAD_INSERT		   	*/	FAMILY_BIN,
	/* 152 L_TAIL_INSERT			*/	FAMILY_BIN,
	/* 153 L_HEAD_RESTRICT			*/	FAMILY_BIN,
	/* 154 L_TAIL_RESTRICT			*/	FAMILY_BIN,
	/* 155 L_GENERALISED_CONCAT		*/	FAMILY_UNR,
	/* 156 L_CONVERSE				*/	FAMILY_PST,
	/* 157 L_EMPTY_SEQ				*/	FAMILY_ATOM,
	/* 158 L_SET_RELATION			*/	FAMILY_BIN,
	/* 159 L_PIPE					*/	FAMILY_BIN,
	/* 160 L_SCOPE					*/	FAMILY_END,
	/* 161 L_ASSERT					*/	FAMILY_END,
	/* 162L_OF						*/	FAMILY_END,
	/* 163 L_DO						*/	FAMILY_END,
	/* 164 L_ANY					*/	FAMILY_END,
	/* 165 L_WHERE					*/	FAMILY_END,
	/* 166 L_AFFECT					*/	FAMILY_BIN,
	/* 167 L_BECOMES_MEMBER_OF		*/	FAMILY_BIN,
	/* 168 L_ITERATE				*/	FAMILY_UNR,
	/* 169 L_CLOSURE				*/	FAMILY_UNR,
	/* 170 L_CLOSURE1				*/	FAMILY_UNR,
	/* 171 L_EVL					*/	FAMILY_BIN,
	/* 172 L_UMINUS					*/	FAMILY_UNR,
	/* 173 L_LR_SCOL	   			*/	FAMILY_BIN,
	/* 174 L_LR_PARALLEL   			*/	FAMILY_BIN,
	/* 175 L_RENAMED_IDENT 			*/	FAMILY_ATOM,
   	/* 176 L_REC 					*/	FAMILY_UNR,
	/* 177 L_STRUCT					*/	FAMILY_UNR,
	/* 178 L_LABEL_COL 				*/	FAMILY_BIN,
	/* 179 L_RECORD_ACCESS 			*/	FAMILY_BIN,
	/* 180 L_LIB_FILE_NAME 			*/	FAMILY_END,
#ifdef COMPAT
	/* 181 L_VOID                   */	FAMILY_END,
#endif
	/* 182  L_TREE                  */  FAMILY_UNR,
	/* 183  L_BTREE                 */  FAMILY_UNR,
    /* 184  L_CONST                 */  FAMILY_UNR,
    /* 185  L_TOP                   */  FAMILY_UNR,
    /* 186  L_SONS                  */  FAMILY_UNR,
    /* 187  L_PREFIX                */  FAMILY_UNR,
    /* 188  L_POSTFIX               */  FAMILY_UNR,
    /* 189  L_SIZET                 */  FAMILY_UNR,
    /* 190  L_MIRROR                */  FAMILY_UNR,
    /* 191  L_RANK                  */  FAMILY_UNR,
    /* 192  L_FATHER                */  FAMILY_UNR,
    /* 193  L_SON                   */  FAMILY_UNR,
    /* 194  L_SUBTREE               */  FAMILY_UNR,
    /* 195  L_ARITY                 */  FAMILY_UNR,
    /* 196  L_BIN                   */  FAMILY_UNR,
    /* 197  L_LEFT                  */  FAMILY_UNR,
    /* 198  L_RIGHT                 */  FAMILY_UNR,
    /* 199  L_INFIX                 */  FAMILY_UNR,
	/* 200 L_LOCAL_OPERATIONS  	   	*/	FAMILY_END,
	/* 201 L_SYSTEM    	   		    */	FAMILY_END,
	/* 202  L_EVENTS			   	*/	FAMILY_END,
	/* 203  L_WITNESS			   	*/	FAMILY_END,
#ifdef BALBULETTE
	/* 201  L_BECOMES_SUCH_THAT	   	*/	FAMILY_BIN,
	/* 202  L_MODEL	   				*/	FAMILY_END,
	/* 204  L_THEOREMS			   	*/	FAMILY_END,
#endif
	/* 204 L_CPP_COMMENT            */ FAMILY_END,
	/* 205 L_CPP_KEPT_COMMENT       */ FAMILY_END,
        /* 206 L_REAL_TYPE			*/	FAMILY_ATOM,
        /* 207 L_REAL			*/	FAMILY_ATOM,
        /* 208 L_FLOAT_TYPE			*/	FAMILY_ATOM,
        /* 209 L_JUMPIF					*/	FAMILY_END,
        /* 210 L_LABEL					*/	FAMILY_END,
        /* 211 L_TO					*/	FAMILY_END,
        /* 212 L_REALOP					*/	FAMILY_UNR,
        /* 213 L_FLOOR					*/	FAMILY_UNR,
        /* 214 L_CEILING				*/	FAMILY_UNR,
    /* 215 L_REF				*/	FAMILY_BIN,
    /* 216 L_SURJ_RELATION			*/	FAMILY_BIN,
    /* 217 L_TOTAL_RELATION			*/	FAMILY_BIN,
    /* 218 L_TOTAL_SURJ_RELATION	*/	FAMILY_BIN,
} ;

//
//	}{	Table type de lexeme -> priorite de lexeme
//
// Priorite de type : "erreur"
#include <limits.h>
const int ERROR_NO_PRIO = INT_MIN ;

#ifdef PRIO_POUR_BCOMP_IDEAL
// Priorites NG que l'on souhaite avoir a terme
static int lexem_prio_sit[] =
	{
	/* 000 L_IDENT					*/	ERROR_NO_PRIO,
	/* 001 L_COMMENT				*/	ERROR_NO_PRIO,
	/* 002 L_KEPT_COMMENT			*/	ERROR_NO_PRIO,
	/* 003 L_STRING					*/	ERROR_NO_PRIO,
	/* 004 L_NUMBER					*/	ERROR_NO_PRIO,
	/* 005 L_OPEN_PAREN				*/	ERROR_NO_PRIO,
	/* 006 L_CLOSE_PAREN			*/	ERROR_NO_PRIO,
	/* 007 L_OPEN_BRACES			*/	ERROR_NO_PRIO,
	/* 008 L_CLOSE_BRACES			*/	ERROR_NO_PRIO,
	/* 009 L_OPEN_BRACKET			   	*/	ERROR_NO_PRIO,
	/* 010 L_CLOSE_BRACKET			   	*/	ERROR_NO_PRIO,
	/* 011 L_SCOL					*/	ERROR_NO_PRIO,
	/* 012 L_COMMA					*/	70,
	/* 013 L_DOT					*/	220,
	/* 014 L_MACHINE		   		*/	ERROR_NO_PRIO,
	/* 015 L_REFINEMENT		   		*/	ERROR_NO_PRIO,
	/* 016 L_IMPLEMENTATION	   		*/	ERROR_NO_PRIO,
	/* 017 L_BEGIN					*/	ERROR_NO_PRIO,
	/* 018 L_END					*/	ERROR_NO_PRIO,
	/* 019 L_SEES					*/	ERROR_NO_PRIO,
	/* 020 L_INCLUDES				*/	ERROR_NO_PRIO,
	/* 021 L_PROMOTES				*/	ERROR_NO_PRIO,
	/* 022 L_IMPORTS				*/	ERROR_NO_PRIO,
	/* 023 L_REFINES				*/	ERROR_NO_PRIO,
	/* 024 L_EXTENDS				*/	ERROR_NO_PRIO,
	/* 025 L_USES					*/	ERROR_NO_PRIO,
	/* 026 L_CONSTRAINTS			*/	ERROR_NO_PRIO,
	/* 027 L_DEFINITIONS			*/	ERROR_NO_PRIO,
	/* 028 L_INVARIANT				*/	ERROR_NO_PRIO,
	/* 029 L_ASSERTIONS				*/	ERROR_NO_PRIO,
	/* 030 L_VALUES					*/	ERROR_NO_PRIO,
	/* 031 L_SETS					*/	ERROR_NO_PRIO,
	/* 032 L_CONSTANTS				*/	ERROR_NO_PRIO,
	/* 033 L_HIDDEN_CONSTANTS		*/	ERROR_NO_PRIO,
	/* 034 L_PROPERTIES				*/	ERROR_NO_PRIO,
	/* 035 L_VARIABLES				*/	ERROR_NO_PRIO,
	/* 036 L_HIDDEN_VARIABLES		*/	ERROR_NO_PRIO,
	/* 037 L_INITIALISATION			*/	ERROR_NO_PRIO,
	/* 038 L_OPERATIONS			   	*/	ERROR_NO_PRIO,
	/* 039 L_BOOL					*/	250,
	/* 040 L_MOD					*/	190,
	/* 041 L_SUCC					*/	ERROR_NO_PRIO,
	/* 042 L_PRED					*/	ERROR_NO_PRIO,
	/* 043 L_MAX					*/	250,
	/* 044 L_MIN					*/	250,
	/* 045 L_CARD					*/	250,
	/* 046 L_SIGMA					*/	250,
	/* 047 L_PI					   	*/	250,
	/* 048 L_GUNION					*/	250,
	/* 049 L_GINTER					*/	250,
	/* 050 L_QUNION					*/	250,
	/* 051 L_QINTER					*/	250,
	/* 052 L_ID						*/	250,
	/* 053 L_PRJ1					*/	250,
	/* 054 L_PRJ2					*/	250,
	/* 055 L_DOM					*/	250,
	/* 056 L_RAN					*/	250,
	/* 057 L_SIZE					*/	250,
	/* 058 L_FIRST					*/	250,
	/* 059 L_LAST					*/	250,
	/* 060 L_FRONT					*/	250,
	/* 061 L_TAIL					*/	250,
	/* 062 L_REV					*/	250,
	/* 063 L_L_OR					*/	40,
	/* 064 L_L_NOT					*/	250,
	/* 065 L_PLUS					*/	180,
	/* 066 L_MINUS					*/	180,
	/* 067 L_TIMES					*/	190,
	/* 068 L_DIVIDES			   	*/	190,
	/* 069 L_POWER					*/	200,
#ifndef MATRA
	/* 070 L_UNION					*/	140,
	/* 071 L_INTERSECT			   	*/	140,
#else // Pour Matra il faut la meme priorite que corestrict
	/* 070 L_UNION					*/	130,
	/* 071 L_INTERSECT			   	*/	130,
#endif
	/* 072 L_INTERVAL			   	*/	170,
	/* 073 L_EQUAL					*/	50,
	/* 074 L_REWRITES			   	*/	ERROR_NO_PRIO,
	/* 075 L_DIFFERENT			   	*/	50,
	/* 076 L_LESS					*/	50,
	/* 077 L_LESS_OR_EQUAL_THAN	   	*/	50,
	/* 078 L_GREATER			   	*/	50,
	/* 079 L_GREATER_OR_EQUAL_THAN	*/	50,
	/* 080 L_AND					*/	40,
   	/* 081 L_VAR					*/	ERROR_NO_PRIO,
	/* 082 L_IN						*/	ERROR_NO_PRIO,
	/* 083 L_IF						*/	ERROR_NO_PRIO,
	/* 084 L_THEN					*/	ERROR_NO_PRIO,
	/* 085 L_ELSIF					*/	ERROR_NO_PRIO,
	/* 086 L_ELSE					*/	ERROR_NO_PRIO,
	/* 087 L_CASE					*/	ERROR_NO_PRIO,
	/* 088 L_EITHER					*/	ERROR_NO_PRIO,
	/* 089 L_OR						*/	ERROR_NO_PRIO,
	/* 090 L_SKIP					*/	ERROR_NO_PRIO,
	/* 091 L_PRE					*/	ERROR_NO_PRIO,
	/* 092 L_CHOICE					*/	ERROR_NO_PRIO,
	/* 093L_SELECT					*/	ERROR_NO_PRIO,
	/* 094 L_WHEN					*/	ERROR_NO_PRIO,
	/* 095 L_WHILE					*/	ERROR_NO_PRIO,
	/* 096 L_VARIANT				*/	ERROR_NO_PRIO,
	/* 097 L_LET					*/	ERROR_NO_PRIO,
	/* 098 L_BE						*/	ERROR_NO_PRIO,
   	/* 099 L_NAT					*/	ERROR_NO_PRIO,
	/* 100 L_NAT1					*/	ERROR_NO_PRIO,
	/* 101 L_INT					*/	ERROR_NO_PRIO,
	/* 102 L_TRUE					*/	ERROR_NO_PRIO,
	/* 103 L_FALSE					*/	ERROR_NO_PRIO,
	/* 104 L_MAXINT					*/	ERROR_NO_PRIO,
	/* 105 L_MININT					*/	ERROR_NO_PRIO,
	/* 106 L_EMPTY_SET				*/	ERROR_NO_PRIO,
	/* 107 L_INTEGER				*/	ERROR_NO_PRIO,
	/* 108 L_NATURAL				*/	ERROR_NO_PRIO,
	/* 109 L_NATURAL1				*/	ERROR_NO_PRIO,
	/* 110 L_BOOL_TYPE				*/	ERROR_NO_PRIO,
	/* 111 L_STRING_TYPE			*/	ERROR_NO_PRIO,
	/* 112 L_POW					*/	250,
	/* 113L_POW1					*/	250,
	/* 114 L_FIN					*/	250,
	/* 115 L_FIN1					*/	250,
  	/* 116 L_RETURNS				*/	20,
	/* 117 L_MAPLET					*/	160,
	/* 118 L_FORALL					*/	250,
	/* 119 L_EXISTS					*/	250,
	/* 120 L_IMPLIES				*/	30,
	/* 121 L_EQUIVALENT					*/	30,
	/* 122 L_BELONGS				*/	60,
	/* 123 L_NOT_BELONGS			*/	60,
	/* 124 L_INCLUDED				*/	60,
	/* 125 L_STRICT_INCLUDED		*/	60,
	/* 126 L_NOT_INCLUDED			*/	60,
	/* 127 L_NOT_STRICT_INCLUDED	*/	60,
	/* 128 L_DIRECT_PRODUCT			*/	100,
	/* 129 L_PARALLEL				*/	ERROR_NO_PRIO,
	/* 130 L_RESTRICTION			*/	130,
	/* 131 L_ANTIRESTRICTION		*/	130,
	/* 132 L_CORESTRICTION			*/	130,
	/* 133 L_ANTICORESTRICTION		*/	130,
	/* 134 L_LEFT_OVERLOAD			*/	90,
	/* 135 L_PARTIAL_FUNCTION		*/	120,
	/* 136 L_TOTAL_FUNCTION			*/	120,
	/* 137 L_PARTIAL_INJECTION		*/	120,
	/* 138 L_TOTAL_INJECTION		*/	120,
	/* 139 L_PARTIAL_SURJECTION		*/	120,
	/* 140 L_TOTAL_SURJECTION		*/	120,
	/* 141 L_BIJECTION				*/	120,
	/* 142 L_LAMBDA					*/	250,
	/* 143 L_TRANS_FUNC				*/	250,
	/* 144 L_TRANS_REL				*/	250,
	/* 145 L_SEQUENCE				*/	250,
	/* 146 L_NON_EMPTY_SEQUENCE	   	*/	250,
	/* 147 L_INJECTIVE_SEQ			*/	250,
	/* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	250,
	/* 149 L_PERMUTATION			*/	250,
	/* 150 L_CONCAT					*/	110,
	/* 151 L_HEAD_INSERT		   	*/	130,
	/* 152L_TAIL_INSERT			*/	130,
	/* 153 L_HEAD_RESTRICT			*/	150,
	/* 154 L_TAIL_RESTRICT			*/	150,
	/* 155 L_GENERALISED_CONCAT		*/	250,
	/* 156 L_CONVERSE				*/	230,
	/* 157 L_EMPTY_SEQ				*/	ERROR_NO_PRIO,
	/* 158 L_SET_RELATION			*/	120,
	/* 159 L_PIPE					*/	10,
	/* 160 L_SCOPE					*/	ERROR_NO_PRIO,
	/* 161 L_ASSERT					*/	ERROR_NO_PRIO,
	/* 162L_OF						*/	ERROR_NO_PRIO,
	/* 163 L_DO						*/	ERROR_NO_PRIO,
	/* 164 L_ANY					*/	ERROR_NO_PRIO,
	/* 165 L_WHERE					*/	ERROR_NO_PRIO,
	/* 166 L_AFFECT					*/	20,
	/* 167 L_BECOMES_MEMBER_OF		*/	20,
	/* 168 L_ITERATE				*/	250,
	/* 169 L_CLOSURE				*/	250,
	/* 170 L_CLOSURE1				*/	250,
	/* 171 L_EVL					*/	240,
	/* 172 L_UMINUS					*/	210,
	/* 173 L_LR_SCOL	   			*/	80,
	/* 174 L_LR_PARALLEL   			*/	100,
	/* 175 L_RENAMED_IDENT 			*/	ERROR_NO_PRIO,
   	/* 176 L_REC 					*/	260,
	/* 177 L_STRUCT					*/	250,
	/* 178 L_LABEL_COL 				*/	80, // plus que L_COMMA i.e. plus que 70
	/* 179 L_RECORD_ACCESS 			*/	250, // plus que L_COMMA i.e. plus que 70
	/* 180 L_LIB_FILE_NAME 			*/	ERROR_NO_PRIO,
#ifdef COMPAT
	/* 181 L_VOID                   */	ERROR_NO_PRIO,
#endif
	/* 182  L_TREE                  */  250,
    /* 183  L_BTREE                 */  250,
    /* 184  L_CONST                 */  250,
    /* 185  L_TOP                   */  250,
    /* 186  L_SONS                  */  250,
    /* 187  L_PREFIX                */  250,
    /* 188  L_POSTFIX               */  250,
    /* 189  L_SIZET                 */  250,
    /* 190  L_MIRROR                */  250,
    /* 191  L_RANK                  */  250,
    /* 192  L_FATHER                */  250,
    /* 193  L_SON                   */  250,
    /* 194  L_SUBTREE               */  250,
    /* 195  L_ARITY                 */  250,
    /* 196  L_BIN                   */  250,
    /* 197  L_LEFT                  */  250,
    /* 198  L_RIGHT                 */  250,
    /* 199  L_INFIX                 */  250,
	/* 200 L_LOCAL_OPERATIONS  	   	*/	ERROR_NO_PRIO,
#ifdef BALBULETTE
	/* 201  L_BECOMES_SUCH_THAT	   	*/	60,
	/* 202  L_MODEL	   				*/	ERROR_NO_PRIO,
	/* 203  L_EVENTS			   	*/	ERROR_NO_PRIO,
	/* 204  L_THEOREMS			   	*/	ERROR_NO_PRIO,
        /* 205  L_REAL_TYPE			*/	ERROR_NO_PRIO,
#endif
        /* 206 L_NUMBER					*/	ERROR_NO_PRIO,
    /* 207 L_SURJ_RELATION			*/	120,
    /* 208 L_TOTAL_RELATION			*/	120,
    /* 209 L_TOTAL_SURJ_RELATION			*/	120,
	} ;
#else // !PRIO_POUR_BCOMP_IDEAL
// Priorites "compromis", compatibles avec le Kernel
static int lexem_prio_sit[] =
	{
	/* 000 L_IDENT					*/	ERROR_NO_PRIO,
	/* 001 L_COMMENT				*/	ERROR_NO_PRIO,
	/* 002 L_KEPT_COMMENT			*/	ERROR_NO_PRIO,
	/* 003 L_STRING					*/	ERROR_NO_PRIO,
	/* 004 L_NUMBER					*/	ERROR_NO_PRIO,
	/* 005 L_OPEN_PAREN				*/	ERROR_NO_PRIO,
	/* 006 L_CLOSE_PAREN			*/	ERROR_NO_PRIO,
	/* 007 L_OPEN_BRACES			*/	ERROR_NO_PRIO,
	/* 008 L_CLOSE_BRACES			*/	ERROR_NO_PRIO,
	/* 009 L_OPEN_BRACKET			   	*/	ERROR_NO_PRIO,
	/* 010 L_CLOSE_BRACKET			   	*/	ERROR_NO_PRIO,
	/* 011 L_SCOL					*/	ERROR_NO_PRIO,
	/* 012 L_COMMA					*/	115,
	/* 013 L_DOT					*/	220,
	/* 014 L_MACHINE		   		*/	ERROR_NO_PRIO,
	/* 015 L_REFINEMENT		   		*/	ERROR_NO_PRIO,
	/* 016 L_IMPLEMENTATION	   		*/	ERROR_NO_PRIO,
	/* 017 L_BEGIN					*/	ERROR_NO_PRIO,
	/* 018 L_END					*/	ERROR_NO_PRIO,
	/* 019 L_SEES					*/	ERROR_NO_PRIO,
	/* 020 L_INCLUDES				*/	ERROR_NO_PRIO,
	/* 021 L_PROMOTES				*/	ERROR_NO_PRIO,
	/* 022 L_IMPORTS				*/	ERROR_NO_PRIO,
	/* 023 L_REFINES				*/	ERROR_NO_PRIO,
	/* 024 L_EXTENDS				*/	ERROR_NO_PRIO,
	/* 025 L_USES					*/	ERROR_NO_PRIO,
	/* 026 L_CONSTRAINTS			*/	ERROR_NO_PRIO,
	/* 027 L_DEFINITIONS			*/	ERROR_NO_PRIO,
	/* 028 L_INVARIANT				*/	ERROR_NO_PRIO,
	/* 029 L_ASSERTIONS				*/	ERROR_NO_PRIO,
	/* 030 L_VALUES					*/	ERROR_NO_PRIO,
	/* 031 L_SETS					*/	ERROR_NO_PRIO,
	/* 032 L_CONSTANTS				*/	ERROR_NO_PRIO,
	/* 033 L_HIDDEN_CONSTANTS		*/	ERROR_NO_PRIO,
	/* 034 L_PROPERTIES				*/	ERROR_NO_PRIO,
	/* 035 L_VARIABLES				*/	ERROR_NO_PRIO,
	/* 036 L_HIDDEN_VARIABLES		*/	ERROR_NO_PRIO,
	/* 037 L_INITIALISATION			*/	ERROR_NO_PRIO,
	/* 038 L_OPERATIONS			   	*/	ERROR_NO_PRIO,
	/* 039 L_BOOL					*/	250,
	/* 040 L_MOD					*/	190,
	/* 041 L_SUCC					*/	ERROR_NO_PRIO,
	/* 042 L_PRED					*/	ERROR_NO_PRIO,
	/* 043 L_MAX					*/	250,
	/* 044 L_MIN					*/	250,
	/* 045 L_CARD					*/	250,
	/* 046 L_SIGMA					*/	250,
	/* 047 L_PI					   	*/	250,
	/* 048 L_GUNION					*/	250,
	/* 049 L_GINTER					*/	250,
	/* 050 L_QUNION					*/	250,
	/* 051 L_QINTER					*/	250,
	/* 052 L_ID						*/	250,
	/* 053 L_PRJ1					*/	250,
	/* 054 L_PRJ2					*/	250,
	/* 055 L_DOM					*/	250,
	/* 056 L_RAN					*/	250,
	/* 057 L_SIZE					*/	250,
	/* 058 L_FIRST					*/	250,
	/* 059 L_LAST					*/	250,
	/* 060 L_FRONT					*/	250,
	/* 061 L_TAIL					*/	250,
	/* 062 L_REV					*/	250,
	/* 063 L_L_OR					*/	40,
	/* 064 L_L_NOT					*/	250,
	/* 065 L_PLUS					*/	180,
	/* 066 L_MINUS					*/	180,
	/* 067 L_TIMES					*/	190,
	/* 068 L_DIVIDES			   	*/	190,
	/* 069 L_POWER					*/	200,
	/* 070 L_UNION					*/	160,
	/* 071 L_INTERSECT			   	*/	160,
	/* 072 L_INTERVAL			   	*/	170,
	/* 073 L_EQUAL					*/	60,
	/* 074 L_REWRITES			   	*/	ERROR_NO_PRIO,
	/* 075 L_DIFFERENT			   	*/	160,
	/* 076 L_LESS					*/	160,
	/* 077 L_LESS_OR_EQUAL_THAN	   	*/	160,
	/* 078 L_GREATER			   	*/	160,
	/* 079 L_GREATER_OR_EQUAL_THAN	*/	160,
	/* 080 L_AND					*/	40,
   	/* 081 L_VAR					*/	ERROR_NO_PRIO,
	/* 082 L_IN						*/	ERROR_NO_PRIO,
	/* 083 L_IF						*/	ERROR_NO_PRIO,
	/* 084 L_THEN					*/	ERROR_NO_PRIO,
	/* 085 L_ELSIF					*/	ERROR_NO_PRIO,
	/* 086 L_ELSE					*/	ERROR_NO_PRIO,
	/* 087 L_CASE					*/	ERROR_NO_PRIO,
	/* 088 L_EITHER					*/	ERROR_NO_PRIO,
	/* 089 L_OR						*/	ERROR_NO_PRIO,
	/* 090 L_SKIP					*/	ERROR_NO_PRIO,
	/* 091 L_PRE					*/	ERROR_NO_PRIO,
	/* 092 L_CHOICE					*/	ERROR_NO_PRIO,
	/* 093L_SELECT					*/	ERROR_NO_PRIO,
	/* 094 L_WHEN					*/	ERROR_NO_PRIO,
	/* 095 L_WHILE					*/	ERROR_NO_PRIO,
	/* 096 L_VARIANT				*/	ERROR_NO_PRIO,
	/* 097 L_LET					*/	ERROR_NO_PRIO,
	/* 098 L_BE						*/	ERROR_NO_PRIO,
   	/* 099 L_NAT					*/	ERROR_NO_PRIO,
	/* 100 L_NAT1					*/	ERROR_NO_PRIO,
	/* 101 L_INT					*/	ERROR_NO_PRIO,
	/* 102 L_TRUE					*/	ERROR_NO_PRIO,
	/* 103 L_FALSE					*/	ERROR_NO_PRIO,
	/* 104 L_MAXINT					*/	ERROR_NO_PRIO,
	/* 105 L_MININT					*/	ERROR_NO_PRIO,
	/* 106 L_EMPTY_SET				*/	ERROR_NO_PRIO,
	/* 107 L_INTEGER				*/	ERROR_NO_PRIO,
	/* 108 L_NATURAL				*/	ERROR_NO_PRIO,
	/* 109 L_NATURAL1				*/	ERROR_NO_PRIO,
	/* 110 L_BOOL_TYPE				*/	ERROR_NO_PRIO,
	/* 111 L_STRING_TYPE			*/	ERROR_NO_PRIO,
	/* 112 L_POW					*/	250,
	/* 113L_POW1					*/	250,
	/* 114 L_FIN					*/	250,
	/* 115 L_FIN1					*/	250,
  	/* 116 L_RETURNS				*/	20,
	/* 117 L_MAPLET					*/	160,
	/* 118 L_FORALL					*/	250,
	/* 119 L_EXISTS					*/	250,
	/* 120 L_IMPLIES				*/	30,
	/* 121 L_EQUIVALENT					*/	60,
	/* 122 L_BELONGS				*/	60,
	/* 123 L_NOT_BELONGS			*/	160,
	/* 124 L_INCLUDED				*/	110,
	/* 125 L_STRICT_INCLUDED		*/	110,
	/* 126 L_NOT_INCLUDED			*/	110,
	/* 127 L_NOT_STRICT_INCLUDED	*/	110,
	/* 128 L_DIRECT_PRODUCT			*/	160,
	/* 129 L_PARALLEL				*/	ERROR_NO_PRIO,
	/* 130 L_RESTRICTION			*/	160,
	/* 131 L_ANTIRESTRICTION		*/	160,
	/* 132 L_CORESTRICTION			*/	160,
	/* 133 L_ANTICORESTRICTION		*/	160,
	/* 134 L_LEFT_OVERLOAD			*/	160,
	/* 135 L_PARTIAL_FUNCTION		*/	125,
	/* 136 L_TOTAL_FUNCTION			*/	125,
	/* 137 L_PARTIAL_INJECTION		*/	125,
	/* 138 L_TOTAL_INJECTION		*/	125,
	/* 139 L_PARTIAL_SURJECTION		*/	125,
	/* 140 L_TOTAL_SURJECTION		*/	125,
	/* 141 L_BIJECTION				*/	125,
	/* 142 L_LAMBDA					*/	250,
	/* 143 L_TRANS_FUNC				*/	250,
	/* 144 L_TRANS_REL				*/	250,
	/* 145 L_SEQUENCE				*/	250,
	/* 146 L_NON_EMPTY_SEQUENCE	   	*/	250,
	/* 147 L_INJECTIVE_SEQ			*/	250,
	/* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	250,
	/* 149 L_PERMUTATION			*/	250,
	/* 150 L_CONCAT					*/	160,
	/* 151 L_HEAD_INSERT		   	*/	160,
	/* 152L_TAIL_INSERT				*/	160,
	/* 153 L_HEAD_RESTRICT			*/	160,
	/* 154 L_TAIL_RESTRICT			*/	160,
	/* 155 L_GENERALISED_CONCAT		*/	250,
	/* 156 L_CONVERSE				*/	230,
	/* 157 L_EMPTY_SEQ				*/	ERROR_NO_PRIO,
	/* 158 L_SET_RELATION			*/	125,
	/* 159 L_PIPE					*/	10,
	/* 160 L_SCOPE					*/	ERROR_NO_PRIO,
	/* 161 L_ASSERT					*/	ERROR_NO_PRIO,
	/* 162L_OF						*/	ERROR_NO_PRIO,
	/* 163 L_DO						*/	ERROR_NO_PRIO,
	/* 164 L_ANY					*/	ERROR_NO_PRIO,
	/* 165 L_WHERE					*/	ERROR_NO_PRIO,
	/* 166 L_AFFECT					*/	110,
	/* 167 L_BECOMES_MEMBER_OF		*/	60,
	/* 168 L_ITERATE				*/	250,
	/* 169 L_CLOSURE				*/	250,
	/* 170 L_CLOSURE1				*/	250,
	/* 171 L_EVL					*/	240,
	/* 172 L_UMINUS					*/	210,
	/* 173 L_LR_SCOL	   			*/	20,
	/* 174 L_LR_PARALLEL   			*/	20,
	/* 175 L_RENAMED_IDENT 			*/	ERROR_NO_PRIO,
   	/* 176 L_REC 					*/	260,
	/* 177 L_STRUCT					*/	250,
	/* 178 L_LABEL_COL 				*/	120, // plus que L_COMMA i.e. plus que 120
	/* 179 L_RECORD_ACCESS 			*/	250, // plus que L_COMMA i.e. plus que 70
	/* 180 L_LIB_FILE_NAME 			*/	ERROR_NO_PRIO,
#ifdef COMPAT
	/* 181 L_VOID                   */	ERROR_NO_PRIO,
#endif
    /* 182  L_TREE                  */  250,
    /* 183  L_BTREE                 */  250,
    /* 184  L_CONST                 */  250,
    /* 185  L_TOP                   */  250,
    /* 186  L_SONS                  */  250,
    /* 187  L_PREFIX                */  250,
    /* 188  L_POSTFIX               */  250,
    /* 189  L_SIZET                 */  250,
    /* 190  L_MIRROR                */  250,
    /* 191  L_RANK                  */  250,
    /* 192  L_FATHER                */  250,
    /* 193  L_SON                   */  250,
    /* 194  L_SUBTREE               */  250,
    /* 195  L_ARITY                 */  250,
    /* 196  L_BIN                   */  250,
    /* 197  L_LEFT                  */  250,
    /* 198  L_RIGHT                 */  250,
    /* 199  L_INFIX                 */  250,
	/* 200 L_LOCAL_OPERATIONS  	   	*/	ERROR_NO_PRIO,
	/* 201 L_SYSTEM		   		    */	ERROR_NO_PRIO,
	/* 202  L_EVENTS			   	*/	ERROR_NO_PRIO,
	/* 203 L_WITNESS					*/	ERROR_NO_PRIO,
#ifdef BALBULETTE
	/* 201  L_BECOMES_SUCH_THAT	   	*/	60,
	/* 202  L_MODEL	   				*/	ERROR_NO_PRIO,
	/* 204  L_THEOREMS			   	*/	ERROR_NO_PRIO,
#endif
	/* 204 L_CPP_COMMENT            */  ERROR_NO_PRIO,
	/* 205 L_CPP_KEPT_COMMENT       */  ERROR_NO_PRIO,
        /* 206 L_REAL_TYPE			*/	ERROR_NO_PRIO,
        /* 207 L_REAL					*/	ERROR_NO_PRIO,
        /* 208 L_FLOAT_TYPE			*/	ERROR_NO_PRIO,
        /* 209 L_JUMPIF				*/	ERROR_NO_PRIO,
        /* 210 L_LABEL					*/	ERROR_NO_PRIO,
        /* 211 L_TO					*/	ERROR_NO_PRIO,
        /* 212 L_REALOP					*/	250,
        /* 213 L_FLOOR					*/	250,
        /* 214 L_CEILING				*/	250,
    /* 215 L_REF				*/	20,
    /* 216 L_SURJ_RELATION			*/	125,
    /* 217 L_TOTAL_RELATION			*/	125,
    /* 218 L_TOTAL_SURJ_RELATION	*/	125,
	} ;
#endif // PRIO_POUR_BCOMP_IDEAL

//
//	}{	Associativite de lexemes
//
static const char *lex_assos_name_sct[] =
	{
	"LEFT",
	"RIGHT",
	""
	} ;

//
//	}{	Table type de lexeme -> associativite de lexeme
//
static T_lexem_associativity lexem_assos_sit[] =
	{
	/* 000 L_IDENT					*/	ASSOS_ERROR,
	/* 001 L_COMMENT				*/	ASSOS_ERROR,
	/* 002 L_KEPT_COMMENT			*/	ASSOS_ERROR,
	/* 003 L_STRING					*/	ASSOS_ERROR,
	/* 004 L_NUMBER					*/	ASSOS_ERROR,
	/* 005 L_OPEN_PAREN				*/	ASSOS_ERROR,
	/* 006 L_CLOSE_PAREN			*/	ASSOS_ERROR,
	/* 007 L_OPEN_BRACES			*/	ASSOS_ERROR,
	/* 008 L_CLOSE_BRACES			*/	ASSOS_ERROR,
	/* 009 L_OPEN_BRACKET			*/	ASSOS_ERROR,
	/* 010 L_CLOSE_BRACKET			*/	ASSOS_ERROR,
	/* 011 L_SCOL					*/	ASSOS_ERROR,
	/* 012 L_COMMA					*/	ASSOS_LEFT,
	/* 013 L_DOT					*/	ASSOS_RIGHT,
	/* 014 L_MACHINE		   		*/	ASSOS_ERROR,
	/* 015 L_REFINEMENT		   		*/	ASSOS_ERROR,
	/* 016 L_IMPLEMENTATION	   		*/	ASSOS_ERROR,
	/* 017 L_BEGIN					*/	ASSOS_ERROR,
	/* 018 L_END					*/	ASSOS_ERROR,
	/* 019 L_SEES					*/	ASSOS_ERROR,
	/* 020 L_INCLUDES				*/	ASSOS_ERROR,
	/* 021 L_PROMOTES				*/	ASSOS_ERROR,
	/* 022 L_IMPORTS				*/	ASSOS_ERROR,
	/* 023 L_REFINES				*/	ASSOS_ERROR,
	/* 024 L_EXTENDS				*/	ASSOS_ERROR,
	/* 025 L_USES					*/	ASSOS_ERROR,
	/* 026 L_CONSTRAINTS			*/	ASSOS_ERROR,
	/* 027 L_DEFINITIONS			*/	ASSOS_ERROR,
	/* 028 L_INVARIANT				*/	ASSOS_ERROR,
	/* 029 L_ASSERTIONS				*/	ASSOS_ERROR,
	/* 030 L_VALUES					*/	ASSOS_ERROR,
	/* 031 L_SETS					*/	ASSOS_ERROR,
	/* 032 L_CONSTANTS				*/	ASSOS_ERROR,
	/* 033 L_HIDDEN_CONSTANTS		*/	ASSOS_ERROR,
	/* 034 L_PROPERTIES				*/	ASSOS_ERROR,
	/* 035 L_VARIABLES				*/	ASSOS_ERROR,
	/* 036 L_HIDDEN_VARIABLES		*/	ASSOS_ERROR,
	/* 037 L_INITIALISATION			*/	ASSOS_ERROR,
	/* 038 L_OPERATIONS			   	*/	ASSOS_ERROR,
	/* 039 L_BOOL					*/	ASSOS_ERROR,
	/* 040 L_MOD					*/	ASSOS_LEFT,
	/* 041 L_SUCC					*/	ASSOS_ERROR,
	/* 042 L_PRED					*/	ASSOS_ERROR,
	/* 043 L_MAX					*/	ASSOS_ERROR,
	/* 044 L_MIN					*/	ASSOS_ERROR,
	/* 045 L_CARD					*/	ASSOS_ERROR,
	/* 046 L_SIGMA					*/	ASSOS_ERROR,
	/* 047 L_PI					   	*/	ASSOS_ERROR,
	/* 048 L_GUNION					*/	ASSOS_ERROR,
	/* 049 L_GINTER					*/	ASSOS_ERROR,
	/* 050 L_QUNION					*/	ASSOS_ERROR,
	/* 051 L_QINTER					*/	ASSOS_ERROR,
	/* 052 L_ID						*/	ASSOS_ERROR,
	/* 053 L_PRJ1					*/	ASSOS_ERROR,
	/* 054 L_PRJ2					*/	ASSOS_ERROR,
	/* 055 L_DOM					*/	ASSOS_ERROR,
	/* 056 L_RAN					*/	ASSOS_ERROR,
	/* 057 L_SIZE					*/	ASSOS_ERROR,
	/* 058 L_FIRST					*/	ASSOS_ERROR,
	/* 059 L_LAST					*/	ASSOS_ERROR,
	/* 060 L_FRONT					*/	ASSOS_ERROR,
	/* 061 L_TAIL					*/	ASSOS_ERROR,
	/* 062 L_REV					*/	ASSOS_ERROR,
	/* 063 L_L_OR					*/	ASSOS_LEFT,
	/* 064 L_L_NOT					*/	ASSOS_ERROR,
	/* 065 L_PLUS					*/	ASSOS_LEFT,
	/* 066 L_MINUS					*/	ASSOS_LEFT,
	/* 067 L_TIMES					*/	ASSOS_LEFT,
	/* 068 L_DIVIDES			   	*/	ASSOS_LEFT,
	/* 069 L_POWER					*/	ASSOS_RIGHT,
	/* 070 L_UNION					*/	ASSOS_LEFT,
	/* 071 L_INTERSECT			   	*/	ASSOS_LEFT,
	/* 072 L_INTERVAL			   	*/	ASSOS_LEFT,
	/* 073 L_EQUAL					*/	ASSOS_LEFT,
	/* 074 L_REWRITES			   	*/	ASSOS_ERROR,
	/* 075 L_DIFFERENT			   	*/	ASSOS_LEFT,
	/* 076 L_LESS					*/	ASSOS_LEFT,
	/* 077 L_LESS_OR_EQUAL_THAN	   	*/	ASSOS_LEFT,
	/* 078 L_GREATER			   	*/	ASSOS_LEFT,
	/* 079 L_GREATER_OR_EQUAL_THAN	*/	ASSOS_LEFT,
	/* 080 L_AND					*/	ASSOS_LEFT,
   	/* 081 L_VAR					*/	ASSOS_ERROR,
	/* 082 L_IN						*/	ASSOS_ERROR,
	/* 083 L_IF						*/	ASSOS_ERROR,
	/* 084 L_THEN					*/	ASSOS_ERROR,
	/* 085 L_ELSIF					*/	ASSOS_ERROR,
	/* 086 L_ELSE					*/	ASSOS_ERROR,
	/* 087 L_CASE					*/	ASSOS_ERROR,
	/* 088 L_EITHER					*/	ASSOS_ERROR,
	/* 089 L_OR						*/	ASSOS_ERROR,
	/* 090 L_SKIP					*/	ASSOS_ERROR,
	/* 091 L_PRE					*/	ASSOS_ERROR,
	/* 092 L_CHOICE					*/	ASSOS_ERROR,
	/* 093 L_SELECT					*/	ASSOS_ERROR,
	/* 094 L_WHEN					*/	ASSOS_ERROR,
	/* 095 L_WHILE					*/	ASSOS_ERROR,
	/* 096 L_VARIANT				*/	ASSOS_ERROR,
	/* 097 L_LET					*/	ASSOS_ERROR,
	/* 098 L_BE						*/	ASSOS_ERROR,
   	/* 099 L_NAT					*/	ASSOS_ERROR,
	/* 100 L_NAT1					*/	ASSOS_ERROR,
	/* 101 L_INT					*/	ASSOS_ERROR,
	/* 102 L_TRUE					*/	ASSOS_ERROR,
	/* 103 L_FALSE					*/	ASSOS_ERROR,
	/* 104 L_MAXINT					*/	ASSOS_ERROR,
	/* 105 L_MININT					*/	ASSOS_ERROR,
	/* 106 L_EMPTY_SET				*/	ASSOS_ERROR,
	/* 107 L_INTEGER				*/	ASSOS_ERROR,
	/* 108 L_NATURAL				*/	ASSOS_ERROR,
	/* 109 L_NATURAL1				*/	ASSOS_ERROR,
	/* 110 L_BOOL_TYPE				*/	ASSOS_ERROR,
	/* 111 L_STRING_TYPE			*/	ASSOS_ERROR,
	/* 112 L_POW					*/	ASSOS_ERROR,
	/* 113 L_POW1					*/	ASSOS_ERROR,
	/* 114 L_FIN					*/	ASSOS_ERROR,
	/* 115 L_FIN1					*/	ASSOS_ERROR,
  	/* 116 L_RETURNS				*/	ASSOS_LEFT,
	/* 117 L_MAPLET					*/	ASSOS_LEFT,
	/* 118 L_FORALL					*/	ASSOS_ERROR,
	/* 119 L_EXISTS					*/	ASSOS_ERROR,
	/* 120 L_IMPLIES				*/	ASSOS_LEFT,
	/* 121 L_EQUIVALENT				*/	ASSOS_LEFT,
	/* 122 L_BELONGS				*/	ASSOS_LEFT,
	/* 123 L_NOT_BELONGS			*/	ASSOS_LEFT,
	/* 124 L_INCLUDED				*/	ASSOS_LEFT,
	/* 125 L_STRICT_INCLUDED		*/	ASSOS_LEFT,
	/* 126 L_NOT_INCLUDED			*/	ASSOS_LEFT,
	/* 127 L_NOT_STRICT_INCLUDED	*/	ASSOS_LEFT,
	/* 128 L_DIRECT_PRODUCT			*/	ASSOS_LEFT,
	/* 129 L_PARALLEL				*/	ASSOS_ERROR,
	/* 130 L_RESTRICTION			*/	ASSOS_LEFT,
	/* 131 L_ANTIRESTRICTION		*/	ASSOS_LEFT,
	/* 132 L_CORESTRICTION			*/	ASSOS_LEFT,
	/* 133 L_ANTICORESTRICTION		*/	ASSOS_LEFT,
	/* 134 L_LEFT_OVERLOAD			*/	ASSOS_LEFT,
	/* 135 L_PARTIAL_FUNCTION		*/	ASSOS_LEFT,
	/* 136 L_TOTAL_FUNCTION			*/	ASSOS_LEFT,
	/* 137 L_PARTIAL_INJECTION		*/	ASSOS_LEFT,
	/* 138 L_TOTAL_INJECTION		*/	ASSOS_LEFT,
	/* 139 L_PARTIAL_SURJECTION		*/	ASSOS_LEFT,
	/* 140 L_TOTAL_SURJECTION		*/	ASSOS_LEFT,
	/* 141 L_BIJECTION				*/	ASSOS_LEFT,
	/* 142 L_LAMBDA					*/	ASSOS_ERROR,
	/* 143 L_TRANS_FUNC				*/	ASSOS_ERROR,
	/* 144 L_TRANS_REL				*/	ASSOS_ERROR,
	/* 145 L_SEQUENCE				*/	ASSOS_ERROR,
	/* 146 L_NON_EMPTY_SEQUENCE	   	*/	ASSOS_ERROR,
	/* 147 L_INJECTIVE_SEQ			*/	ASSOS_ERROR,
	/* 148 L_NON_EMPTY_INJECTIVE_SEQ*/	ASSOS_ERROR,
	/* 149 L_PERMUTATION			*/	ASSOS_ERROR,
	/* 150 L_CONCAT					*/	ASSOS_LEFT,
	/* 151 L_HEAD_INSERT		   	*/	ASSOS_LEFT,
	/* 152 L_TAIL_INSERT			*/	ASSOS_LEFT,
	/* 153 L_HEAD_RESTRICT			*/	ASSOS_LEFT,
	/* 154 L_TAIL_RESTRICT			*/	ASSOS_LEFT,
	/* 155 L_GENERALISED_CONCAT		*/	ASSOS_ERROR,
	/* 156 L_CONVERSE				*/	ASSOS_ERROR,
	/* 157 L_EMPTY_SEQ				*/	ASSOS_ERROR,
	/* 158 L_SET_RELATION			*/	ASSOS_LEFT,
	/* 159 L_PIPE					*/	ASSOS_LEFT,
	/* 160 L_SCOPE					*/	ASSOS_ERROR,
	/* 161 L_ASSERT					*/	ASSOS_ERROR,
	/* 162 L_OF						*/	ASSOS_ERROR,
	/* 163 L_DO						*/	ASSOS_ERROR,
	/* 164 L_ANY					*/	ASSOS_ERROR,
	/* 165 L_WHERE					*/	ASSOS_ERROR,
	/* 166 L_AFFECT					*/	ASSOS_LEFT,
	/* 167 L_BECOMES_MEMBER_OF		*/	ASSOS_LEFT,
	/* 168 L_ITERATE				*/	ASSOS_ERROR,
	/* 169 L_CLOSURE				*/	ASSOS_ERROR,
	/* 170 L_CLOSURE1				*/	ASSOS_ERROR,
	/* 171 L_EVL					*/	ASSOS_LEFT,
	/* 172 L_UMINUS					*/	ASSOS_ERROR,
	/* 173 L_LR_SCOL	   			*/	ASSOS_LEFT,
	/* 174 L_LR_PARALLEL   			*/	ASSOS_LEFT,
	/* 174 L_RENAMED_IDENT 			*/	ASSOS_ERROR,
	/* 176 L_REC 					*/	ASSOS_ERROR,
	/* 177 L_STRUCT					*/	ASSOS_ERROR,
	/* 178 L_LABEL_COL		   		*/	ASSOS_LEFT,
	/* 179 L_RECORD_ACCESS	   		*/	ASSOS_LEFT,
	/* 180 L_LIB_FILE_NAME 			*/	ASSOS_ERROR,
#ifdef COMPAT
	/* 181 L_VOID                   */	ASSOS_ERROR,
#endif
	/* 182  L_TREE                  */  ASSOS_ERROR,
    /* 183  L_BTREE                 */  ASSOS_ERROR,
    /* 184  L_CONST                 */  ASSOS_ERROR,
    /* 185  L_TOP                   */  ASSOS_ERROR,
    /* 186  L_SONS                  */  ASSOS_ERROR,
    /* 187  L_PREFIX                */  ASSOS_ERROR,
    /* 188  L_POSTFIX               */  ASSOS_ERROR,
    /* 189  L_SIZET                 */  ASSOS_ERROR,
    /* 190  L_MIRROR                */  ASSOS_ERROR,
    /* 191  L_RANK                  */  ASSOS_ERROR,
    /* 192  L_FATHER                */  ASSOS_ERROR,
    /* 193  L_SON                   */  ASSOS_ERROR,
    /* 194  L_SUBTREE               */  ASSOS_ERROR,
    /* 195  L_ARITY                 */  ASSOS_ERROR,
    /* 196  L_BIN                   */  ASSOS_ERROR,
    /* 197  L_LEFT                  */  ASSOS_ERROR,
    /* 198  L_RIGHT                 */  ASSOS_ERROR,
    /* 199  L_INFIX                 */  ASSOS_ERROR,
	/* 200 L_LOCAL_OPERATIONS	   	*/	ASSOS_ERROR,
	/* 201 L_SYSTEM		   		    */	ASSOS_ERROR,
	/* 202  L_EVENTS			   	*/	ASSOS_ERROR,
	/* 203 L_WITNESS				*/	ASSOS_ERROR,
#ifdef BALBULETTE
	/* 201  L_BECOMES_SUCH_THAT		*/	ASSOS_LEFT,
	/* 202  L_MODEL	   				*/	ASSOS_ERROR,
	/* 204  L_THEOREMS			   	*/	ASSOS_ERROR,
#endif
	/* 204 L_CPP_COMMENT            */  ASSOS_ERROR,
	/* 205 L_CPP_KEPT_COMMENT       */  ASSOS_ERROR,
        /* 206 L_REAL_TYPE			*/	ASSOS_ERROR,
        /* 207 L_REAL					*/	ASSOS_ERROR,
        /* 208 L_FLOAT_TYPE			*/	ASSOS_ERROR,
        /* 209 L_JUMPIF					*/	ASSOS_ERROR,
        /* 210 L_LABEL					*/	ASSOS_ERROR,
        /* 211 L_TO					*/	ASSOS_ERROR,
        /* 212 L_REALOP					*/	ASSOS_ERROR,
        /* 213 L_FLOOR					*/	ASSOS_ERROR,
        /* 214 L_CEILING				*/	ASSOS_ERROR,
    /* 215 L_REF				*/	ASSOS_LEFT,
    /* 216 L_SURJ_RELATION				*/	ASSOS_LEFT,
    /* 217 L_TOTAL_RELATION				*/	ASSOS_LEFT,
    /* 218 L_TOTAL_SURJ_RELATION		*/	ASSOS_LEFT,
		} ;

//
//	}{	Obtention de la famille de lexemes d'un lexem
//
T_lexem_family get_lexem_family(T_lexem *cur_lexem)
{
#ifdef FULL_TRACE
TRACE2("get_lexem_family(cur_lexem :: %x \"%s\")",
	   cur_lexem,
	   cur_lexem->get_lex_name()) ;
TRACE3("get_lexem_family(%x::%s)->%d",
	   cur_lexem,
	   (cur_lexem == NULL) ? "NULL" : cur_lexem->get_lex_name(),
	   (cur_lexem == NULL)
	   	? FAMILY_END : lexem_family_set[cur_lexem->get_lex_type()]) ;
#endif

return (cur_lexem == NULL)
	? FAMILY_END : lexem_family_set[cur_lexem->get_lex_type()] ;
}

//
//	}{	Obtention de la priorite d'un lexeme
//
int get_lexem_prio(T_lexem *cur_lexem)
{
#ifndef NO_CHECK
int table_size = sizeof(lexem_prio_sit) /sizeof(int) ;
ASSERT(cur_lexem->get_lex_type() < table_size) ;
#endif
return lexem_prio_sit[cur_lexem->get_lex_type()] ;
}

//
//	}{	Obtention de l'associativite d'un lexeme
//
T_lexem_associativity get_lexem_assoc(T_lexem *cur_lexem)
{
#ifndef NO_CHECK
int table_size = sizeof(lexem_assos_sit) /sizeof(int) ;
ASSERT(cur_lexem->get_lex_type() < table_size) ;
#endif
return lexem_assos_sit[cur_lexem->get_lex_type()] ;
}

//
//	}{	Dump des tables au format LaTeX 2e
//
void latex2e_dump_LR_tables(const char *file_name,
									 const char *output_path)
{
TRACE2("latex2e_dump_LR_tables(\"%s\", \"%s\")", file_name, output_path) ;
print("Generating LateX2e LR table \"%s/%s\"\n",
				   output_path,
				   file_name) ;

char *path = new char[strlen(file_name) + strlen(output_path) + 2] ;
#ifdef STATS_ALLOC
add_alloc(path,  strlen(file_name) + strlen(output_path) + 2, __FILE__, __LINE__) ;
#endif

sprintf(path, "%s/%s", output_path, file_name) ;

FILE *fd = fopen(path, "w") ;

if (fd == NULL)
	{
	toplevel_error(FATAL_ERROR,
								get_error_msg(E_PROBLEM_WRITING_FILE),
								file_name,
								strerror(errno)) ;
	}

s_fprintf(fd, "%% Document generated by bcomp version %s\n", get_bcomp_version()) ;
s_fprintf(fd, "\\documentclass[a4paper,11pt]{report}\n") ;
s_fprintf(fd, "\\usepackage{supertabular}\n") ;
s_fprintf(fd, "\\usepackage{fancyheadings}\n") ;

s_fprintf(fd, "\
\\setlength{\\parindent}{0mm}\n\
\\setlength{\\parskip}{3.5pt}\n\
\\setlength{\\textwidth}{16.5cm}\n\
\\setlength{\\textheight}{25cm}\n\
\\setlength{\\oddsidemargin}{-0,5cm}\n\
\\setlength{\\evensidemargin}{-0,5cm}\n\
\\setlength{\\headheight}{1cm}\n\
\\setlength{\\topmargin}{-2cm}\n") ;



s_fprintf(fd, "\\begin{document}\n") ;
s_fprintf(fd, "\\lhead{Bcomp version %s\n}", get_bcomp_version()) ;
s_fprintf(fd, "\\chead{}\n") ;
s_fprintf(fd, "\\rhead{Internal lexems table}\n") ;
s_fprintf(fd, "\\lfoot{\\copyright 2008 CLEARSY\n}", get_bcomp_version()) ;
s_fprintf(fd, "\\cfoot{}\n") ;
s_fprintf(fd, "\\rfoot{\\thepage}\n") ;
s_fprintf(fd, "\\pagestyle{fancy}\n") ;

s_fprintf(fd,
		  "\\tablehead{\\hline Nb&ASCII&LEXEM&FAMILY&PRIO&ASSOC\\\\\\hline}\n");
s_fprintf(fd, "\\tabletail{\\hline \\multicolumn{6}{|r|}\
{Followed on next page}\\\\\\hline}\n") ;
s_fprintf(fd, "\\tablelasttail{\\hline}\n") ;
s_fprintf(fd,
		  "\\begin{supertabular}{|p{0.8cm}|p{4.1cm}|p{5.1cm}|l|c|c|c|}\\hline\n") ;

T_lex_type i ;
size_t table_size = sizeof(lexem_family_set) / sizeof(int) ;
for (i = L_IDENT ; (size_t)i < table_size ; i = (T_lex_type)((size_t)i + 1))
	{
	TRACE2("get_lex_type_ascii(%d) = \"%s\"", i, get_lex_type_ascii(i)) ;

	s_fprintf(fd,
			  "%d &\\verb@%s@ & \\verb@%s@ & {\\bf %s} &",
			  i,
			  get_lex_type_ascii(i),
			  get_lex_type_name(i),
			  lex_family_name_sct[lexem_family_set[i]]) ;

	if (lexem_prio_sit[i] == ERROR_NO_PRIO)
		{
		s_fprintf(fd, " &") ;
		}
	else
		{
		s_fprintf(fd, "%d &", lexem_prio_sit[i]) ;
		}

	s_fprintf(fd, "{\\em %s}\\\\\\hline\n", lex_assos_name_sct[lexem_assos_sit[i]]);
	}

s_fprintf(fd, "\\end{supertabular}\n") ;

s_fprintf(fd, "\\end{document}\n") ;
fclose(fd) ;
print("LateX2e LR table \"%s/%s\" generated\n",
				   output_path,
				   file_name) ;

}

//
//	}{	Dump des tables au format HTML
//
void html_dump_LR_tables(const char *file_name,
								  const char *output_path)
{
TRACE2("html_dump_LR_tables(\"%s\", \"%s\")", file_name, output_path) ;
print("Generating HTML LR table \"%s/%s\"\n",
				   output_path,
				   file_name) ;

char *path = new char[strlen(file_name) + strlen(output_path) + 2] ;
#ifdef STATS_ALLOC
add_alloc(path,  strlen(file_name) + strlen(output_path) + 2, __FILE__, __LINE__) ;
#endif
sprintf(path, "%s/%s", output_path, file_name) ;

FILE *fd = fopen(path, "w") ;

if (fd == NULL)
	{
	toplevel_error(FATAL_ERROR,
								get_error_msg(E_PROBLEM_WRITING_FILE),
								file_name,
								strerror(errno)) ;
	}

s_fprintf(fd,
		  "<HTML>\n<TITLE>B Compiler version %s - Internal Lexems Table</TITLE>\n",
		  get_bcomp_version()) ;

s_fprintf(fd, "<TABLE BORDER=4 CELLPADDING=4>") ;
s_fprintf(fd, "<TR><TD>Nb</TD><TD>ASCII</TD><TD>LEXEM</TD><TD>FAMILY</TD>") ;
s_fprintf(fd, "<TD>PRIO</TD><TD>ASSOC</TD></TR>\n") ;


T_lex_type i ;
size_t table_size = sizeof(lexem_family_set) / sizeof(int) ;
for (i = L_IDENT ; (size_t)i < table_size ; i = (T_lex_type)((size_t)i + 1))
	{
	TRACE2("get_lex_type_ascii(%d) = \"%s\"", i, get_lex_type_ascii(i)) ;

	s_fprintf(fd,
			  "<TR><TD>%d</TD><TD>%s</TD><TD>%s</TD><TD><BOLD>%s</BOLD></TD>",
			  i,
			  get_lex_type_ascii(i),
			  get_lex_type_name(i),
			  lex_family_name_sct[lexem_family_set[i]]) ;

	if (lexem_prio_sit[i] == ERROR_NO_PRIO)
		{
		s_fprintf(fd, "<TD></TD>") ;
		}
	else
		{
		s_fprintf(fd, "<TD>%d</TD>", lexem_prio_sit[i]) ;
		}

	s_fprintf(fd,
			  "<TD><em>%s</em></TD></TR>\n",
			  lex_assos_name_sct[lexem_assos_sit[i]]);
	}

s_fprintf(fd, "</TABLE>\n") ;
s_fprintf(fd, "</HTML>\n") ;
fclose(fd) ;
print("HTML LR table \"%s/%s\" generated\n",
				   output_path,
				   file_name) ;

}

//
//	}{	Fin du fichier
//
