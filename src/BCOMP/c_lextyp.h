/******************************* CLEARSY **************************************
* Fichier : $Id: c_lextyp.h,v 2.0 2005-04-25 10:43:56 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type enumere T_lex_type (types de lexemes)
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
#ifndef _C_LEXTYP_H_
#define _C_LEXTYP_H_

//
//	}{	Types de lexemes
//
typedef enum
	{
	/* 000 */	L_IDENT = 0,   				/* Identifier  			*/
	/* 001 */	L_COMMENT,					/* Comment				*/
	/* 002 */	L_KEPT_COMMENT,	   			/* Kept Comment	   		*/
	/* 003 */	L_STRING,	   				/* Literal string		*/
	/* 004 */	L_NUMBER,	   				/* Literal integer		*/

	/* 005 */  	L_OPEN_PAREN,		   		/* '(' 					*/
	/* 006 */	L_CLOSE_PAREN,				/* ')' 					*/
	/* 007 */	L_OPEN_BRACES,				/* '{' 					*/
	/* 008 */	L_CLOSE_BRACES,   			/* '}' 					*/
	/* 009 */	L_OPEN_BRACKET,				/* '['					*/
	/* 010 */	L_CLOSE_BRACKET,	  		/* ']'					*/
	/* 011 */	L_SCOL,						/* ';' 					*/
	/* 012 */	L_COMMA,   					/* ','					*/
	/* 013 */	L_DOT,						/* '.'					*/

				// ATTENTION L_MACHINE, L_REFINEMENT et L_IMPLEMENTATION
				// doivent etre declares successivement et dans cet ordre
	/* 014 */	L_MACHINE,					/* 'MACHINE'			*/
	/* 015 */	L_REFINEMENT,				/* 'REFINEMENT'			*/
	/* 016 */	L_IMPLEMENTATION,			/* 'IMPLEMENTATION'		*/

	/* 017 */	L_BEGIN,					/* 'BEGIN'				*/
	/* 018 */	L_END,						/* 'END'				*/
	/* 019 */	L_SEES,						/* 'SEES'				*/
	/* 020 */	L_INCLUDES,					/* 'INCLUDES'			*/
	/* 021 */	L_PROMOTES,					/* 'PROMOTES'			*/
	/* 022 */	L_IMPORTS,					/* 'IMPORTS'			*/
	/* 023 */	L_REFINES,					/* 'REFINES'			*/
	/* 024 */	L_EXTENDS,					/* 'EXTENDS'			*/
	/* 025 */	L_USES,						/* 'USES'				*/

	/* 026 */	L_CONSTRAINTS,				/* 'CONSTRAINTS'		*/
	/* 027 */	L_DEFINITIONS,				/* 'DEFINITIONS'		*/
	/* 028 */	L_INVARIANT,				/* 'INVARIANT'			*/
	/* 029 */	L_ASSERTIONS,				/* 'ASSERTIONS'			*/
	/* 030 */	L_VALUES,					/* 'VALUES'				*/

	/* 031 */	L_SETS,						/* 'SETS'				*/
	/* 032 */	L_CONCRETE_CONSTANTS,	   	/* 'CONSTANTS'			*/
											/*  'VISIBLE_CONSTANTS'	*/
											/*  'CONCRETE_CONSTANTS'*/
	/* 033 */	L_ABSTRACT_CONSTANTS,	   	/*  'HIDDEN_CONSTANTS'	*/
											/*	'ABSTRACT_CONSTANTS'*/
	/* 034 */	L_PROPERTIES,				/*  'PROPERTIES'	   	*/
	/* 035 */	L_CONCRETE_VARIABLES,		/*  'VARIABLES'			*/
											/* 'VISIBLE_VARIABLES'	*/
											/* 'CONCRETE_VARIABLES'	*/
	/* 036 */	L_ABSTRACT_VARIABLES,		/* 'HIDDEN_VARIABLES'	*/
											/* 'ABSTRACT_VARIABLES'	*/
	/* 037 */	L_INITIALISATION,			/* 'INITIALISATION'		*/
	/* 038 */	L_OPERATIONS,				/* 'OPERATIONS'			*/
	/* 039 */	L_BOOL,						/* 'bool'				*/
	/* 040 */	L_MOD,						/* 'mod'				*/
	/* 041 */	L_SUCC,						/* 'succ'				*/
	/* 042 */	L_PRED,						/* 'pred'				*/
	/* 043 */	L_MAX,						/* 'max'				*/
	/* 044 */	L_MIN,						/* 'min'				*/
	/* 045 */	L_CARD,						/* 'card'				*/
	/* 046 */	L_SIGMA,					/* 'SIGMA'				*/
	/* 047 */	L_PI,						/* 'PI'					*/
	/* 048 */	L_GUNION,					/* 'union'				*/
	/* 049 */	L_GINTER,					/* 'inter'				*/
	/* 050 */	L_QUNION,					/* 'UNION'				*/
	/* 051 */	L_QINTER,					/* 'INTER'				*/
	/* 052 */	L_ID,						/* 'id'					*/
	/* 053 */	L_PRJ1,						/* 'prj1'				*/
	/* 054 */	L_PRJ2,						/* 'prj2'				*/
	/* 055 */	L_DOM,						/* 'dom'				*/
	/* 056 */	L_RAN,						/* 'ran'				*/
	/* 057 */	L_SIZE,						/* 'size'				*/
	/* 058 */	L_FIRST,					/* 'first'				*/
	/* 059 */	L_LAST,						/* 'last'				*/
	/* 060 */	L_FRONT,					/* 'front'				*/
	/* 061 */	L_TAIL,						/* 'tail'				*/
	/* 062 */	L_REV,						/* 'rev'				*/
	/* 063 */	L_L_OR,						/* 'or'					*/
	/* 064 */	L_L_NOT,		   			/* 'not'				*/

	/* 065 */  	L_PLUS,						/* '+'					*/
	/* 066 */	L_MINUS,					/* '-'					*/
	/* 067 */	L_TIMES,					/* '*'					*/
	/* 068 */	L_DIVIDES,					/* '/'					*/
	/* 069 */	L_POWER,					/* '**'					*/
	/* 070 */	L_UNION,					/* '\/'					*/
	/* 071 */	L_INTERSECT,				/* '/\'					*/
	/* 072 */	L_INTERVAL,					/* '..'					*/
	/* 073 */	L_EQUAL,					/* '=' 					*/
	/* 074 */	L_REWRITES,					/* '=='					*/
	/* 075 */	L_DIFFERENT,				/* '/='    				*/
	/* 076 */	L_LESS_THAN,   				/* '<' 					*/
	/* 077 */	L_LESS_THAN_OR_EQUAL,  		/* '<=' 				*/
	/* 078 */	L_GREATER_THAN,				/* '>' 					*/
	/* 079 */	L_GREATER_THAN_OR_EQUAL,   	/* '>=' 				*/
	/* 080 */	L_AND,			   			/* '&' 					*/

   	/* 081 */  	L_VAR,						/* 'VAR'				*/
	/* 082 */	L_IN,						/* 'IN'					*/
	/* 083 */	L_IF,						/* 'IF'					*/
	/* 084 */	L_THEN,						/* 'THEN'				*/
	/* 085 */	L_ELSIF,					/* 'ELSIF'				*/
	/* 086 */	L_ELSE,						/* 'ELSE'				*/
	/* 087 */	L_CASE,						/* 'CASE'				*/
	/* 088 */	L_EITHER,					/* 'EITHER'				*/
	/* 089 */	L_OR,						/* 'OR'					*/
	/* 090 */	L_SKIP,						/* 'skip'				*/
	/* 091 */	L_PRE,						/* 'PRE'				*/
	/* 092 */	L_CHOICE,					/* 'CHOICE'				*/
	/* 093 */	L_SELECT,					/* 'SELECT'				*/
	/* 094 */	L_WHEN,						/* 'WHEN'				*/
	/* 095 */	L_WHILE,					/* 'WHILE'				*/
	/* 096 */	L_VARIANT,					/* 'VARIANT'			*/
	/* 097 */	L_LET,						/* 'LET'				*/
	/* 098 */	L_BE,						/* 'BE'					*/

   	/* 099 */	L_NAT,						/* 'NAT'				*/
	/* 100 */	L_NAT1,						/* 'NAT_1'				*/
	/* 101 */	L_INT,						/* 'INT'				*/
	/* 102 */	L_TRUE,						/* 'TRUE'				*/
	/* 103 */	L_FALSE,					/* 'FALSE'				*/
	/* 104 */	L_MAXINT,					/* 'MAXINT'				*/
	/* 105 */	L_MININT,					/* 'MININT'				*/
	/* 106 */	L_EMPTY_SET,				/* '{}'					*/
	/* 117 */	L_INTEGER,					/* 'INTEGER'			*/
	/* 118 */	L_NATURAL,					/* 'NATURAL'			*/
	/* 119 */	L_NATURAL1,					/* 'NATURAL1'			*/
	/* 110 */	L_BOOL_TYPE,   				/* 'BOOL'				*/
	/* 111 */	L_STRING_TYPE, 				/* 'STRING'				*/
	/* 112 */	L_POW,						/* 'POW'				*/
	/* 113 */	L_POW1,						/* 'POW1'				*/
	/* 114 */	L_FIN,						/* 'FIN'				*/
	/* 115 */	L_FIN1,						/* 'FIN1'				*/

  	/* 116 */  	L_RETURNS,					/* '<--'				*/
	/* 117 */	L_MAPLET,					/* '|->'				*/
	/* 118 */	L_FORALL,					/* '!'					*/
	/* 119 */	L_EXISTS,					/* '#'					*/
	/* 120 */	L_IMPLIES,					/* '=>'					*/
	/* 121 */	L_EQUIVALENT,  				/* '<=>'				*/
	/* 122 */	L_BELONGS,					/* ':'					*/
	/* 123 */	L_NOT_BELONGS,				/* '/:'					*/
	/* 124 */	L_INCLUDED,					/* '<:'					*/
	/* 125 */	L_STRICT_INCLUDED,			/* '<<:'				*/
	/* 126 */	L_NOT_INCLUDED,		   		/* '/<:'   				*/
	/* 127 */	L_NOT_STRICT_INCLUDED, 		/* '/<<:'				*/
	/* 128 */	L_DIRECT_PRODUCT,			/* '><'					*/
	/* 129 */	L_PARALLEL,					/* '||'					*/
	/* 130 */	L_RESTRICTION,				/* '<|'					*/
	/* 131 */	L_ANTIRESTRICTION,			/* '<<|'				*/
	/* 132 */	L_CORESTRICTION,			/* '|>'					*/
	/* 133 */	L_ANTICORESTRICTION,		/* '|>>'				*/
	/* 134 */	L_LEFT_OVERLOAD,			/* '<+'					*/
	/* 135 */	L_PARTIAL_FUNCTION,			/* '+->'				*/
	/* 136 */	L_TOTAL_FUNCTION,			/* '-->'				*/
	/* 137 */	L_PARTIAL_INJECTION,		/* '>+>'				*/
	/* 138 */	L_TOTAL_INJECTION,			/* '>->'				*/
	/* 139 */	L_PARTIAL_SURJECTION,		/* '+->>'				*/
	/* 140 */	L_TOTAL_SURJECTION,			/* '-->>'				*/
	/* 141 */	L_BIJECTION,				/* '>->>'				*/
	/* 142 */	L_LAMBDA,					/* '%'					*/
	/* 143 */	L_TRANS_FUNC,				/* 'fnc'				*/
	/* 144 */	L_TRANS_REL,				/* 'rel'				*/
	/* 145 */	L_SEQUENCE,					/* 'seq'				*/
	/* 146 */	L_NON_EMPTY_SEQUENCE,		/* 'seq1'				*/
	/* 147 */	L_INJECTIVE_SEQ,			/* 'iseq'				*/
	/* 148 */	L_NON_EMPTY_INJECTIVE_SEQ,	/* 'iseq1'				*/
	/* 149 */	L_PERMUTATION,				/* 'perm'				*/
	/* 150 */	L_CONCAT,					/* '^'					*/
	/* 151 */	L_HEAD_INSERT,				/* '->'					*/
	/* 152 */	L_TAIL_INSERT,				/* '<-'					*/
	/* 153 */	L_HEAD_RESTRICT,			/* '/|\'				*/
	/* 154 */	L_TAIL_RESTRICT,			/* '\|/'				*/
	/* 155 */	L_GENERALISED_CONCAT,		/* 'conc'				*/
	/* 156 */	L_CONVERSE,					/* '~'					*/
	/* 157 */	L_EMPTY_SEQ,		   		/* '[]'					*/
	/* 158 */	L_SET_RELATION,		   		/* '<->'   				*/
	/* 159 */	L_PIPE,			 	  		/* '|'   				*/
	/* 160 */	L_SCOPE,			   		/* '$0'   				*/
	/* 161 */	L_ASSERT,			   		/* 'ASSERT'				*/
	/* 162 */	L_OF,			   			/* 'OF'					*/
	/* 163 */	L_DO,			   			/* 'DO'					*/
	/* 164 */	L_ANY,			   			/* 'ANY'				*/
	/* 165 */	L_WHERE,			   		/* 'WHERE'				*/
	/* 166 */	L_AFFECT,			   		/* ':='					*/
	/* 167 */	L_BECOMES_MEMBER_OF,   		/* '::'					*/
	/* 168 */	L_ITERATE,					/* 'iterate'			*/
	/* 169 */	L_CLOSURE,   				/* 'closure'	   		*/
	/* 170 */	L_CLOSURE1,			   		/* 'closure1'  	   		*/
	/* 171 */	L_EVL,   					/* EVL, genere par ana LR 	*/
	/* 172 */	L_UMINUS,			   		/* Moins unaire, genere par ana LR 	*/
	/* 173 */	L_LR_SCOL,			   		/* "\;" genere par ana LR 	*/
	/* 174 */	L_LR_PARALLEL,	   	   		/* "\||" , genere par ana LR 	*/
	/* 175 */	L_RENAMED_IDENT,			/* identifiant renomme */
	/* 176 */	L_REC,						/* 'rec' */
	/* 177 */	L_STRUCT,					/* 'struct' */
	/* 178 */	L_LABEL_COL, 				/* '\:' */
	/* 179 */	L_RECORD_ACCESS,   		    /* ''' */
	/* 180 */	L_LIB_FILE_NAME,			/* <nom_fichier> */
#ifdef COMPAT
	/* 181 */	L_VOID,						/* lexeme fantome */
#endif
	/* 182 */ L_TREE,                       /* 'tree'      */
	/* 183 */ L_BTREE,                      /* 'btree'     */
	/* 184 */ L_CONST,                      /* 'const'     */
	/* 185 */ L_TOP,                        /* 'top'       */
	/* 186 */ L_SONS,                       /* 'sons'      */
	/* 187 */ L_PREFIX,                     /* 'prefix'    */
	/* 188 */ L_POSTFIX,                    /* 'postfix'   */
	/* 189 */ L_SIZET,                      /* 'sizet'     */
	/* 190 */ L_MIRROR,                     /* 'mirror'     */
	/* 191 */ L_RANK,                       /* 'rank'      */
	/* 192 */ L_FATHER,                     /* 'father'    */
	/* 193 */ L_SON,                        /* 'son'       */
	/* 194 */ L_SUBTREE,                    /* 'subtree'   */
	/* 195 */ L_ARITY,                      /* 'arity'     */
	/* 196 */ L_BIN,                        /* 'bin'       */
	/* 197 */ L_LEFT,                       /* 'left'      */
	/* 198 */ L_RIGHT,                      /* 'right'     */
	/* 199 */ L_INFIX,                      /* 'infix'     */
#ifdef BALBULETTE
	/* 201 */ L_BECOMES_SUCH_THAT,   		/* ':|'		   */
	/* 202 */ L_MODEL,				   		/* 'MODEL'	   */
	/* 204 */ L_THEOREMS					/* 'THEOREMS'  */
#else // ! BALBULETTE
	/* 200 */ L_LOCAL_OPERATIONS,			/* 'LOCAL_OPERATIONS' */
	/* 201 */ L_SYSTEM,                     /* 'SYSTEM' */
	/* 202 */ L_EVENTS,						/* 'EVENTS'	   */
    /* 203 */ L_WITNESS,					/* 'WITNESS'	   */
#endif
	/* 204 */ L_CPP_COMMENT,                /* '//' */
	/* 205 */ L_CPP_KEPT_COMMENT,           /* '//?' */
	/* 206 */ L_REAL_TYPE,                  /* 'REAL' */
	/* 207 */ L_REAL,               		/* Literal real	*/
	/* 208 */ L_FLOAT_TYPE,             	/* 'FLOAT' */
	/* 209 */ L_JUMPIF,                 	/* 'JUMPIF' */
	/* 210 */ L_LABEL,			   			/* 'LABEL'				*/
	/* 211 */ L_TO,							/* 'TO'				*/
	/* 212 */ L_REALOP,                 	/* real */
	/* 213 */ L_FLOOR,                  	/* floor */
	/* 214 */ L_CEILING,                	/* ceiling */
	/* 215 */  	L_REF,						/* 'ref'				*/
	/* 216 */   L_SURJ_RELATION,        	/* '<->> */
	/* 217 */   L_TOTAL_RELATION,       	/* '<<-> */
	/* 218 */   L_TOTAL_SURJ_RELATION   	/* '<<->> */
        } T_lex_type ;

//
//	}{	Types de lexemes
//
typedef enum
	{
	/* 000 */	PL_IDENT = 0,   			/* Identifier  			*/
	/* 001 */  	PL_PRAGMA,		  	 		/* 'pragma'				*/
	/* 002 */  	PL_OPEN_PAREN,		   		/* '(' 					*/
	/* 003 */	PL_CLOSE_PAREN,				/* ')' 					*/
	/* 004 */	PL_COMMA,   				/* ','					*/
	/* 005 */	PL_STRING,   				/* "string"		   		*/
	/* 006 */	PL_GARBAGE   				/* poubelle	   			*/
	} T_pragma_lex_type ;

#endif /* _C_LEXTYP_H_ */


