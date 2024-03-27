/******************************* CLEARSY **************************************
* Fichier : $Id: c_nodtyp.h,v 2.0 1999-10-08 13:36:54 jfm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type enumere T_node_type
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
#ifndef _C_NODTYP_H_
#define _C_NODTYP_H_

//
//	}{	Type de sous-classe de T_object
//
typedef enum
	{
	/* 00 */	NODE_MACHINE = 0,
	/* 01 */	NODE_BETREE,
	/* 02 */	NODE_SYMBOL,
	/* 03 */	NODE_LEXEM,
	/* 04 */	NODE_OPERATION,
	/* 05 */	NODE_SKIP,
	/* 06 */	NODE_BEGIN,
	/* 07 */	NODE_USED_OP,
	/* 08 */	NODE_CHOICE,
	/* 09 */	NODE_OR,
	/* 10 */	NODE_PRECOND,
	/* 11 */	NODE_ASSERT,
	/* 12 */	NODE_IF,
	/* 13 */	NODE_ELSE,
	/* 14 */	NODE_SELECT,
	/* 15 */	NODE_WHEN,
	/* 16 */	NODE_PREDICATE,
	/* 17 */	NODE_EXPR,
	/* 18 */	NODE_CASE,
	/* 19 */	NODE_CASE_ITEM,
	/* 20 */	NODE_LIST_LINK,
	/* 21 */	NODE_LET,
	/* 22 */	NODE_VALUATION,
	/* 23 */	NODE_IDENT,
	/* 24 */	NODE_VAR,
	/* 25 */	NODE_WHILE,
	/* 26 */	NODE_ANY,
	/* 27 */	NODE_AFFECT,
	/* 28 */	NODE_BECOMES_MEMBER_OF,
	/* 29 */	NODE_BECOMES_SUCH_THAT,
	/* 30 */	NODE_OP_CALL,
	/* 31 */	NODE_COMMENT,
	/* 32 */	NODE_FLAG,
	/* 33 */	NODE_DEFINITION,
	/* 34 */	NODE_USED_MACHINE,
	/* 35 */	NODE_LITERAL_INTEGER,
	/* 36 */   	NODE_BINARY_OP,
	/* 37 */	NODE_EXPR_WITH_PARENTHESIS,
	/* 38 */	NODE_UNARY_OP,
	/* 39 */	NODE_ARRAY_ITEM,
	/* 40 */	NODE_CONVERSE,
	/* 41 */	NODE_LITERAL_STRING,
	/* 42 */	NODE_KEYWORD,
	/* 43 */	NODE_LAMBDA_EXPR,
	/* 44 */	NODE_BINARY_PREDICATE,
	/* 45 */	NODE_TYPING_PREDICATE,
	/* 46 */	NODE_COMPREHENSIVE_SET,
	/* 47 */	NODE_IMAGE,
	/* 48 */	NODE_PREDICATE_WITH_PARENTHESIS,
	/* 49 */	NODE_EXTENSIVE_SET,
	/* 50 */	NODE_NOT_PREDICATE,
	/* 51 */	NODE_EXPR_PREDICATE,
	/* 52 */	NODE_UNIVERSAL_PREDICATE,
	/* 53 */	NODE_EXISTENTIAL_PREDICATE,
	/* 54 */	NODE_EMPTY_SEQ,
	/* 55 */	NODE_EMPTY_SET,
	/* 56 */	NODE_SIGMA,
	/* 57 */	NODE_PI,
	/* 58 */	NODE_QUANTIFIED_UNION,
	/* 59 */	NODE_QUANTIFIED_INTERSECTION,
	/* 60 */	NODE_LITERAL_BOOLEAN,
	/* 61 */	NODE_LITERAL_ENUMERATED_VALUE,
	/* 62 */	NODE_RELATION,
	/* 63 */	NODE_RENAMED_IDENT,
	/* 64 */	NODE_EXTENSIVE_SEQ,
	/* 65 */	NODE_GENERALISED_UNION,
	/* 66 */	NODE_GENERALISED_INTERSECTION,
	/* 67 */	NODE_OBJECT,
	/* 68 */	NODE_ITEM,
	/* 69 */	NODE_SUBSTITUTION,
	/* 70 */	NODE_INTEGER,
	/* 71 */	NODE_BETREE_MANAGER,
	/* 72 */	NODE_BETREE_LIST,
	/* 73 */	NODE_TYPE,
	/* 74 */	NODE_PRODUCT_TYPE,
	/* 75 */	NODE_SETOF_TYPE,
	/* 76 */	NODE_ABSTRACT_TYPE,
	/* 77 */	NODE_ENUMERATED_TYPE,
	/* 78 */	NODE_PREDEFINED_TYPE,
	/* 79 */	NODE_BASE_TYPE,
	/* 80 */	NODE_CONSTRUCTOR_TYPE,
	/* 81 */	NODE_GENERIC_TYPE,
	/* 82 */	NODE_GENERIC_OP,
	/* 83 */	NODE_BOUND,
	/* 84 */	NODE_OP_RESULT,
	/* 85 */	NODE_PRAGMA,
	/* 86 */	NODE_PRAGMA_LEXEM,
	/* 87 */	NODE_B0_TYPE,
	/* 88 */	NODE_B0_BASE_TYPE,
	/* 89 */	NODE_B0_ABSTRACT_TYPE,
	/* 90 */	NODE_B0_ENUMERATED_TYPE,
	/* 91 */	NODE_B0_INTERVAL_TYPE,
	/* 92 */	NODE_B0_ARRAY_TYPE,
	/* 93 */	NODE_RECORD,
	/* 94 */	NODE_RECORD_ITEM,
	/* 95 */	NODE_STRUCT,
	/* 96 */	NODE_RECORD_ACCESS,
	/* 97 */	NODE_RECORD_TYPE,
	/* 98 */	NODE_LABEL_TYPE,
	/* 99 */	NODE_B0_RECORD_TYPE,
	/* 100 */	NODE_B0_LABEL_TYPE,
   	/* 101 */	NODE_MSG_LINE,
   	/* 102 */	NODE_MSG_LINE_MANAGER,
   	/* 103 */	NODE_COMPONENT,
   	/* 104 */	NODE_FILE_COMPONENT,
   	/* 105 */	NODE_PROJECT,
   	/* 106 */	NODE_PROJECT_MANAGER,
   	/* 107 */	NODE_FILE_DEFINITION,
	/* 108 */   NODE_COMPONENT_CHECKSUMS,
	/* 109 */   NODE_OPERATION_CHECKSUMS,
	/* 110 */   NODE_OP_CALL_TREE,
	/* 111 */   NODE_WITNESS,
        /* 112 */	NODE_LITERAL_REAL,
        /* 113 */	NODE_LABEL,
        /* 114 */	NODE_JUMPIF,
				// Ces valeurs ci-apres doivent etre les dernieres
				// de la table :
    /* 115*/	NODE_USER_DEFINED1, // Doit etre le premier u.defined !
    /* 116*/	NODE_USER_DEFINED2,
    /* 117*/	NODE_USER_DEFINED3,
    /* 118*/	NODE_USER_DEFINED4,
    /* 119*/	NODE_USER_DEFINED5,
    /* 120*/	NODE_USER_DEFINED6,
    /* 121*/	NODE_USER_DEFINED7,
    /* 122*/	NODE_USER_DEFINED8,
    /* 123*/	NODE_USER_DEFINED9,
    /* 124*/	NODE_USER_DEFINED10,
    /* 125*/	NODE_USER_DEFINED11,
    /* 126*/	NODE_USER_DEFINED12,
    /* 127*/	NODE_USER_DEFINED13,
    /* 128*/	NODE_USER_DEFINED14,
    /* 129*/	NODE_USER_DEFINED15,
    /* 130*/	NODE_USER_DEFINED16,
    /* 131*/	NODE_USER_DEFINED17,
    /* 132*/	NODE_USER_DEFINED18,
    /* 133*/	NODE_USER_DEFINED19,
    /* 134*/	NODE_USER_DEFINED20 // DOIT ETRE LA DERNIERE VALEUR !
	} T_node_type ;			// Reporter toute modification dans c_meta.cpp
							// et dans c_msg.cpp (2 tables)
							// La derniere valeur doit etre NODE_USER_DEFINED

// Macros pour compatibilite ascendante
#define	NODE_LITTERAL_INTEGER NODE_LITERAL_INTEGER
#define	NODE_LITTERAL_BOOLEAN NODE_LITERAL_BOOLEAN
#define	NODE_LITTERAL_STRING NODE_LITERAL_STRING
#define	NODE_LITTERAL_ENUMERATED_VALUE NODE_LITERAL_ENUMERATED_VALUE

//
//	}{	Type de fichier binaire
//
typedef enum
{
  // 00 Betree syntaxique
  FILE_SYNTAXIC_BETREE = 0,
  // 01 Betree semantique
  FILE_SEMANTIC_BETREE,
  // 02 Betree passe au verificateur de B0
  FILE_B0_CHECKED_BETREE,
  // 03 Betree passe a l'analyseur de dependances
  FILE_DEP_BETREE,
  // 04 Liste de composants (MS)
  FILE_COMPONENT_LIST,
  // 05 Liste de composants (MS)
  FILE_COMPONENT_CHECKSUMS,
  // 06 Joker (pour la lecture de n'importe quel type de fichier)
  FILE_READ_JOKER
} T_betree_file_type ;

//
//	}{	Type d'utilisation du fichier
//
typedef enum
{
  FILE_READ_BETREE,
  FILE_WRITE_BETREE
} T_betree_file_mode ;

//
// }{ Diagnostic Entrees/Sorties
//
typedef enum
{
  IO_OK,
  IO_ERROR
} T_io_diagnostic ;

#endif /* _C_NODTYP_H_ */


