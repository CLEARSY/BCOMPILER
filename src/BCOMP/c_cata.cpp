/******************************* CLEARSY **************************************
* Fichier : $Id: c_cata.cpp,v 2.0 2002-07-17 10:07:25 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Catalogue de messages
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
RCS_ID("$Id: c_cata.cpp,v 1.41 2002-07-17 10:07:25 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

/* Includes Composant Local */
#include "c_api.h"

extern void init_tables(void) ;

#ifndef NO_CHECKS
// On memorise la taille du catalogue courant
static size_t size_si = 0 ;
#endif

static const char *const english_catalog_sct[] =
	{
	/* 00 C_EXPR_OR_PRED */
	"Expression or Predicate",
	/* 01 C_MACHINE_NAME */
	"machine name (identifier)",
	/* 02 C_IO_ERROR */
	"I/O error",
	/* 03 C_ABORTING */
	" : fatal error - aborting\n",
	/* 04 C_OPN_MIGHT_NOT_BE_CLOSED %s opn name */
	"Warning : %s may not be properly closed\n",
	/* 05 C_BEGINNING_OF %s opn name */
	"Beginning of %s\n",
	/* 06 C_END_OF %s opn name */
	"End of %s\n",
	/* 07 C_UNEXPECTED */
	"Unexpected",
	/* 08 C_UNEXPECTED_EOF_AFTER_THIS_TOKEN */
	"Unexpected end of file after this token",
	/* 09 C_UNEXPECTED_IDENT_VALUE %s ident name */
	"Unexpected identifier : \"%s\"",
	/* 10 C_UNEXPECTED_STRING_VALUE %s value */
	"Unexpected string : \"%s\"",
	/* 11 C_UNEXPECTED_LITERAL_NUMBER_VALUE %s value */
	"Unexpected literal number : %s",
	/* 12 C_UNEXPECTED_TOKEN_VALUE %s ascii */
	"Unexpected token : %s",
	/* 13 C_WHERE */
	"'WHERE'",
	/* 14 C_EXPECTED_VALUE	%s expected value */
	"Expected value(s) : %s\n",
	/* 15 C_IS_A_MACRO_DEF_HERE %s macro name */
	"(\"%s\" is a macro defined here).\n",
	/* 16 C_REDEFINITION_OF	%s redefined */
	"Redefinition of %s\n",
	/* 17 C_LOCATION_OF_PREVIOUS_OCCURENCE */
	"Location of previous occurence\n",
	/* 18 C_EXPR */
	"expression",
	/* 19 C_PRED */
	"predicate",
	/* 20 C_IDENT */
	"identifier",
	/* 21 C_SUBST */
	"substitution",
	/* 22 C_INTERNAL_ERROR */
	"Internal error at %s:%d\n",
	/* 23 C_EOF */
	"end of file",
        /* 24 C_THEN */
	"'THEN'",
	/* 25 C_VAR_OF_LAMBDA_EXPR */
	"Variable of Lambda-expression",
	/* 26 C_PAREXPR */
	"(expression)",
	/* 27 C_PREDICATE_PIPE_EXPR */
	"predicate '|' expression",
	/* 28 C_VAR_OF_SIGMA */
	"variable of SIGMA",
	/* 29 C_VAR_OF_PI */
	"variable of PI",
	/* 30 C_VAR_OF_QUNION */
	"variable of quantified union",
	/* 31 C_VAR_OF_QINTER */
	"variable of quantified intersection",
 	/* 32 C_QUANT_VAR */
	"Quantified variable",
	/* 33 C_PARPRED */
	"(predicate)",
	/* 34 C_IDENT_OR_FCALL */
	"identifier or identifer(Expr)",
	/* 35 C_OP_NAME */
	"operation name",
	/* 36 C_UNABLE_TO_OPEN_TRACE_FILE */
	"unable to open trace file :",
	/* 37 C_DEF_ARG */
	"a definition argument (ident)",
	/* 38 C_CLOSE_PAREN */
	"')'",
	/* 39 C_OPEN_PAREN */
	"'('",
	/* 40 C_CLOSE_BRACES */
	"'}'",
	/* 41 C_OPEN_BRACES */
	"'{'",
	/* 42 C_REWRITES */
	"'=='",
	/* 43 C_DEF_CLAUSE */
	"DEFINITIONS clause",
	/* 44 C_SET_ITEM */
	"set item",
	/* 45 C_ATOM_UN_BINOP_OPN */
	"Atom, unary or binary operator, '(', '{' or '['",
	/* 46 C_ATOM_UNOP_OPN */
	"Atom, unary operator, '(', '{' or '['",
	/* 47 C_OPN_PAREN_OR_BRACKET */
	"'(' or '['",
	/* 48 C_BINOP_OR_OPN_OR_TILDA */
	"Binary operator, '(', '[' or '{', '~'",
	/* 49 C_ATOM_UN_BINOP_OPN_CLO */
	"Atom, unary or binary operator, '(', ')', '{', '}', '[' or ']'",
	/* 50 C_SEEN_MACH_NAME */
	"seen machine name",
	/* 51 C_OP_OUTPUT_PARAM */
	"operation output parameter",
	/* 52 C_RETURNS */
	"'<--'",
	/* 53 C_EQU_RET_OPPAR_COMMA */
	"'=', '<--', '(' or ','",
	/* 54 C_AT_TOPLEVEL */
	"At toplevel: ",
	/* 55 C_EQUALS */
	"'='",
	/* 56 C_OR_END */
	"'OR', 'END'",
	/* 57 ELSEIF_ELSE_END */
	"'ELSIF', 'ELSE', 'END'",
	/* 58 WHEN_ELSE_END */
	"'WHEN', 'ELSE', 'END'",
	/* 59 OF */
	"'OF'",
	/* 60 EITHER */
	"'EITHER'",
	/* 61 BE */
	"'BE'",
	/* 62 IDENT_IN */
	"Identifier, 'IN'",
	/* 63 LET_VALUATION */
	"let valuation (Ident = Expression +&)",
	/* 64 DO */
	"'DO'",
	/* 65 INVARIANT */
	"'INVARIANT'",
	/* 66 C_OP_INPUT_PARAM */
	"operation input parameter",
	/* 67 C_END */
	"'END'",
	/* 68 C_IN */
	"'IN'",
	/* 69 C_VARIANT */
	"'VARIANT'",
 	/* 70 C_MACH_PARAM */
	"machine parameter (identifier)",
 	/* 71 C_CONSTANTS_CL */
	"CONCRETE_CONSTANTS clause",
 	/* 72 C_HCONSTANTS_CL */
	"ABSTRACT_CONSTANTS clause",
 	/* 73 C_VARIABLES_CL */
	"CONCRETE_VARIABLES clause",
 	/* 74 C_HVARIABLES_CL */
	"ABSTRACT_VARIABLES clause",
	/* 75 C_SETS_CL */
	"SETS clause",
	/* 76 C_SET_NAME */
	"SET name",
    /* 77 C_CONSTRAINTS_CL */
	"CONSTRAINTS clause",
    /* 78 C_PROPERTIES_CL */
	"PROPERTIES clause",
    /* 79 C_INVARIANT_CL */
	"INVARIANT clause",
    /* 80 C_ASSERTION_CL */
	"ASSERTIONS clause",
    /* 81 C_INITIALISATION_CL */
	"INITIALISATION clause",
    /* 82 C_VALUES_CL */
	"VALUES clause",
    /* 83 C_IMPORTS_CL */
	"IMPORTS clause",
    /* 84 C_OPERATIONS_CL */
	"OPERATIONS clause",
    /* 85 C_SEES_CL */
	"SEES clause",
    /* 86 C_PROMOTES_CL */
	"PROMOTES clause",
    /* 87 C_EXTENDS_CL */
	"EXTENDS clause",
    /* 88 C_USES_CL */
	"USES clause",
    /* 89 C_INCLUDES_CL */
	"INCLUDES clause",
	/* 90 C_ABSTR_NAME */
	"abstraction name",
	/* 91 C_BCOMP_SUCCESS */
	"bcomp: Compilation successful\n",
	/* 92 C_LOCALISATION_OF_CLAUSE */
	"location of %s clause\n",
    /* 93 C_REFINES_CL */
	"REFINES clause",
	/* 94 C_LOADING_BETREE %s betree name */
	"Loading Betree from file %s\n",
	/* 95 C_SAVING_BETREE %s betree name */
	"Saving Betree into file %s\n",
	/* 96 C_DUMPING_HTML_BETREE */
	"Dumping Betree into HTML in %s\n",
	/* 97 C_SYNTAX_ANALYSIS */
	"Doing syntax analysis for component %s\n",
	/* 98 C_SEMANTIC_ANALYSIS */
	"Doing semantic analysis for component %s\n",
	/* 99 C_IT_IS_AN_IMPLEMENTATION */
	"(it is an implementation)\n",
	/* 100 C_IT_IS_A_REFINEMENT */
	"(it is a refinement)\n",
	/* 101 C_NO_TYPE_GIVEN */
	"no type given",
	/* 102 C_USING_PATH_XX_FOR_COMPONENT %s path %s component %s component */
	"Using path \"%s/%s\" for component %s\n",
	/* 103 C_B0_CHECK */
	"Doing B0 Check for component %s\n",
	/* 104 C_REMOVING_PREV_HTML */
	"Removing previous HTML files ...\n",
	/* 105 C_CREATING_HTML */
	"Creating HTML files\n",
	/* 106 C_DEP_ANALYSIS */
	"Doing dependencies analysis for component %s\n",
	/* 107 C_USING_PATH_XX_FOR_FILE_DEF %s path %s component %s component */
	"Using path \"%s/%s\" for definitions file %s\n",
	/* 108 C_LOCAL_QUANTIFIER */
	"local quantifier",
	/* 109 C_NO_VISIBILITY */
	"no visibility",
  	/* 110 C_ABSTRACT_SET_LOST %s set name */
	"Specification abstract set %s is not present in implementation",
	/* 111 C_CREATING_REFINEMENT %s ref name %s abs name */
	"Creating refinement %s of component %s\n",
	/* 112 C_CREATING_IMPLEMENTATION %s imp name %s abs name */
	"Creating implementation %s of component %s\n",
	/* 113 C_COMPONENT_CHECKSUM %s file name %s checksum %s checksum */
	"Checksum for component %s are %s (raw checksum) and %s (expanded checksum)\n",
	/* 114 C_CLAUSE_CHECKSUM %s file name %s checksum */
	"%s is %s\n",
	/* 115 C_FORMULA */
	"formula",
   	/* 116 C_OP_PRECOND_CHECKSUM %s op_name %s checksum %s checksum */
	"Checksum for precondition of operation %s is %s\n",
	/* 117 C_OP_BODY_CHECKSUM %s op_name %s checksum %s checksum */
	"Checksum for body of operation %s is %s\n",
    /* 118 C_LOCAL_OPERATIONS_CL */
	"LOCAL_OPERATIONS clause",
   	/* 119 C_NO_SUCH_FILE_IN_PROJECT */
	"This file is not part of the current project",
	/* 120	C_IDENT_IN_MULTIPLE_AFFECT */
	"identifier (or a renamed identifier), because this is a multiple affectation",
	/* 121 C_REDEFINITION_OF_MININT_VALUE value */
	"Using user-specified value %s for MININT\n",
	/* 122 C_REDEFINITION_OF_MAXINT_VALUE  value */
	"Using user-specified value %s for MAXINT\n",
	/* 123 C_DEFINITION_NAME */
	"a definition name (ident)",
	/* 124 C_OR_ELSE_END */
	"'OR', 'ELSE', 'END'",
	/* 125 C_INV_TOKEN_IN_DEF */
	"invalid token '%s' in definition '%s'",
	/* 126 C_EQUALS_or_BELONGS*/
	"':' or '='",
	/* 127 C_BELONGS*/
	":",
	/* 128 C_TYPING_PREDICAT */
	"Typing predicate",
	/* 129 WITNESS_VALUATION */
	"witness (Ident = Expression +&)",
    /* 130 C_TO */
    "'TO'",
    /* 131 C_PRECOND */
    "PRE should not appear inside a substitution. Maybe you meant to use an ASSERT here.",
	} ;

static const char *french_catalog_sct[] =
	{
	/* 00 C_EXPR_OR_PRED */
	"Expression ou Predicate",
	/* 01 C_MACHINE_NAME */
	"nom de machine (identificateur)",
	/* 02 C_IO_ERROR */
	"erreur d'entr�e/sortie",
	/* 03 C_ABORTING */
	" : erreur fatale - abandon de la compilation\n",
	/* 04 C_OPN_MIGHT_NOT_BE_CLOSED %s opn name */
	"Attention : %s n'est peut �tre pas ferm� correctement\n",
	/* 05 C_BEGINNING_OF %s opn name */
	"D�but de %s\n",
	/* 06 C_END_OF %s opn name */
	"Fin de %s\n",
	/* 07 C_UNEXPECTED */
	"non attendu",
	/* 08 C_UNEXPECTED_EOF_AFTER_THIS_TOKEN */
	"Fin de fichier non attendue apr�s ce lex�me",
	/* 09 C_UNEXPECTED_IDENT_VALUE %s ident name */
	"Identificateur non attendu : \"%s\"",
	/* 10 C_UNEXPECTED_STRING_VALUE %s value */
	"Cha�ne non attendue : \"%s\"",
	/* 11 C_UNEXPECTED_LITERAL_NUMBER_VALUE %s value */
	"Nombre litt�ral non attendu %s",
	/* 12 C_UNEXPECTED_TOKEN_VALUE %s ascii */
	"Lex�me non attendu : %s",
	/* 13 C_WHERE */
	"'WHERE'",
	/* 14 C_EXPECTED_VALUE	%s expected value */
	"Valeur(s) attendue(s) : %s\n",
	/* 15 C_IS_A_MACRO_DEF_HERE %s macro name */
	"(\"%s\" est une macro d�finie ici).\n",
	/* 16 C_REDEFINITION_OF	%s redefined */
	"Redefinition de %s\n",
	/* 17 C_LOCATION_OF_PREVIOUS_OCCURENCE */
	"Localisation de la pr�c�dente occurence\n",
	/* 18 C_EXPR */
	"expression",
	/* 19 C_PRED */
	"pr�dicat",
	/* 20 C_IDENT */
	"identificateur",
	/* 21 C_SUBST */
	"substitution",
	/* 22 C_INTERNAL_ERROR */
	"B Compiler : erreur interne � la position %s:%d\n",
	/* 23 C_EOF */
	"fin de fichier",
	/* 24 THEN */
	"'THEN'",
	/* 25 C_VAR_OF_LAMBDA_EXPR */
	"Variable de la lambda-expression",
	/* 26 C_PAREXPR */
	"(expression)",
	/* 27 C_PREDICATE_PIPE_EXPR */
	"pr�dicat '|' expression",
	/* 28 C_VAR_OF_SIGMA */
	"variable de SIGMA",
	/* 29 C_VAR_OF_PI */
	"variable de PI",
	/* 30 C_VAR_OF_QUNION */
	"variable de l'union quantifi�e",
	/* 31 C_VAR_OF_QINTER */
	"variable de l'intersection quantifi�e",
 	/* 32 C_QUANT_VAR */
	"variable quantifi�e",
	/* 33 C_PARPRED */
	"(pr�dicat)",
	/* 34 C_IDENT_OR_FCALL */
	"identificateur ou identificateur(Expr)",
	/* 35 C_OP_NAME */
	"nom d'op�ration",
	/* 36 C_UNABLE_TO_OPEN_TRACE_FILE */
	"impossible d'ouvrir le fichier de trace :",
	/* 37 C_DEF_ARG */
	"un param�tre de d�finition (identificateur)",
	/* 38 C_CLOSE_PAREN */
	"')'",
	/* 39 C_OPEN_PAREN */
	"'('",
	/* 40 C_CLOSE_BRACES */
	"'}'",
	/* 41 C_OPEN_BRACES */
	"'{'",
	/* 42 C_REWRITES */
	"'=='",
	/* 43 C_DEF_CLAUSE */
	"clause DEFINITIONS",
	/* 44 C_SET_ITEM */
	"�l�ment d'ensemble",
	/* 45 C_ATOM_UN_BINOP_OPN */
	"Atome, op�rateur unaire ou binaire, '(', '{' ou '['",
	/* 46 C_ATOM_UNOP_OPN */
	"Atome, op�rateur unaire , '(', '{' ou '['",
	/* 47 C_OPN_PAREN_OR_BRACKET */
	"'(' ou '['",
	/* 48 C_BINOP_OR_OPN_OR_TILDA */
	"Op�rateur binaire, '(', '[' or '{', '~'",
	/* 49 C_ATOM_UN_BINOP_OPN_CLO */
	"Atome, op�rateur unaire ou binaire, '(', ')', '{', '}', '[' or ']'",
	/* 50 C_SEEN_MACH_NAME */
	"nom de machine vue",
	/* 51 C_OP_OUTPUT_PARAM */
	"param�tre de sortie d'op�ration",
	/* 52 C_RETURNS */
	"'<--'",
	/* 53 C_EQU_RET_OPPAR_COMMA */
	"'=', '<--', '(' or ','",
	/* 54 C_AT_TOPLEVEL */
	"Au niveau principal: ",
	/* 55 C_EQUALS */
	"'='",
	/* 56 C_OR_END */
	"'OR', 'END'",
	/* 57 ELSEIF_ELSE_END */
	"'ELSIF', 'ELSE', 'END'",
	/* 58 WHEN_ELSE_END */
	"'WHEN', 'ELSE', 'END'",
	/* 59 OF */
	"'OF'",
	/* 60 EITHER */
	"'EITHER'",
	/* 61 BE */
	"'BE'",
	/* 62 IDENT_IN */
	"Identifier, 'IN'",
	/* 63 LET_VALUATION */
	"valuation de LET (Ident = Expression +&)",
	/* 64 DO */
	"'DO'",
	/* 65 INVARIANT */
	"'INVARIANT'",
	/* 66 C_OP_INPUT_PARAM */
	"param�tre d'entr�e d'op�ration",
	/* 67 C_END */
	"'END'",
	/* 68 C_IN */
	"'IN'",
	/* 69 C_VARIANT */
	"'VARIANT'",
 	/* 70 C_MACH_PARAM */
	"param�tre formel de machine (identificateur)",
 	/* 71 C_CONSTANTS_CL */
	"clause CONCRETE_CONSTANTS",
 	/* 72 C_HCONSTANTS_CL */
	"clause ABSTRACT_CONSTANTS",
 	/* 73 C_VARIABLES_CL */
	"clause CONCRETE_VARIABLES",
 	/* 74 C_HVARIABLES_CL */
	"clause ABSTRACT_VARIABLES",
	/* 75 C_SETS_CL */
	"clause SETS",
	/* 76 C_SET_NAME */
	"clause SET name",
    /* 77 C_CONSTRAINTS_CL */
	"clause CONSTRAINTS",
    /* 78 C_PROPERTIES_CL */
	"clause PROPERTIES",
    /* 79 C_INVARIANT_CL */
	"clause INVARIANT",
    /* 80 C_ASSERTION_CL */
	"clause ASSERTIONS",
    /* 81 C_INITIALISATION_CL */
	"clause INITIALISATION",
    /* 82 C_VALUES_CL */
	"clause VALUES",
    /* 83 C_IMPORTS_CL */
	"clause IMPORTS",
    /* 84 C_OPERATIONS_CL */
	"clause OPERATIONS",
    /* 85 C_SEES_CL */
	"clause SEES",
    /* 86 C_PROMOTES_CL */
	"clause PROMOTES",
    /* 87 C_EXTENDS_CL */
	"clause EXTENDS",
    /* 88 C_USES_CL */
	"clause USES",
    /* 89 C_INCLUDES_CL */
	"clause INCLUDES",
	/* 90 C_ABSTR_NAME */
	"nom de l'abstraction",
	/* 91 C_BCOMP_SUCCESS */
	"bcomp: Compilation termin�e\n",
	/* 92 C_LOCALISATION_OF_CLAUSE */
	"localisation de la clause %s\n",
    /* 93 C_REFINES_CL */
	"clause REFINES",
	/* 94 C_LOADING_BETREE %s betree name */
	"Chargement du Betree depuis de fichier %s\n",
	/* 95 C_SAVING_BETREE %s betree name */
	"Sauvegarde du Betree dans le fichier %s\n",
	/* 96 C_DUMPING_HTML_BETREE */
	"Production de la repr�sentation HTML du Betree dans le r�pertoire %s\n",
	/* 97 C_SYNTAX_ANALYSIS */
	"Analyse syntaxique du composant %s\n",
	/* 98 C_SEMANTIC_ANALYSIS */
	"Analyse s�mantique du composant %s\n",
	/* 99 C_IT_IS_AN_IMPLEMENTATION */
	"(c'est une impl�mentation)\n",
	/* 100 C_IT_IS_A_REFINEMENT */
	"(c'est un raffinement)\n",
	/* 101 C_NO_TYPE_GIVEN */
	"pas de type fourni",
	/* 102 C_USING_PATH_XX_FOR_COMPONENT %s path %s component %s component */
	"Utilisation du chemin d'acc�s \"%s/%s\" pour le composant %s\n",
	/* 103 C_B0_CHECK */
	"B0 Check du composant %s\n",
	/* 104 C_REMOVING_PREV_HTML */
	"Suppression des fichiers HTML pr�c�dents ...\n",
	/* 105 C_CREATING_HTML */
	"Cr�ation des fichiers HTML\n",
	/* 106 C_DEP_ANALYSIS */
	"Analyse des d�pendances du composant %s\n",
	/* 107 C_USING_PATH_XX_FOR_FILE_DEF %s path %s component %s component */
	"Utilisation du chemin d'acc�s \"%s/%s\" pour le fichier de d�finitions %s\n",
	/* 108 C_LOCAL_QUANTIFIER */
	"quantificateur local",
	/* 109 C_NO_VISIBILITY */
	"pas de visibilit�",
  	/* 110 C_ABSTRACT_SET_LOST %s set name */
	"L'ensemble abstrait %s de la sp�cification n'est pas pr�sent dans l'impl�mentation",
	/* 111 C_CREATING_REFINEMENT %s ref name %s abs name */
	"Cr�ation du raffinement %s du composant %s\n",
	/* 112 C_CREATING_IMPLEMENTATION %s imp name %s abs name */
	"Cr�ation de l'impl�mentation %s du composant %s\n",
	/* 113 C_COMPONENT_CHECKSUM %s file name %s checksum %s checksum*/
	"Les checksums du composant %s sont %s (brut) et %s (d�finitions expans�es)\n",
	/* 114 C_CLAUSE_CHECKSUM %s file name %s checksum %s checksum*/
	"%s vaut %s\n",
	/* 115 C_FORMULA */
	"formule",
   	/* 116 C_OP_PRECOND_CHECKSUM %s op_name %s checksum %s checksum */
	"Le checksum de la pr�condition de l'op�ration %s est %s\n",
	/* 117 C_OP_BODY_CHECKSUM %s op_name %s checksum %s checksum */
	"Le checksum du corps de l'op�ration %s est %s\n",
    /* 118 C_LOCAL_OPERATIONS_CL */
	"clause OPERATIONS",
   	/* 119 C_NO_SUCH_FILE_IN_PROJECT */
	"Ce fichier ne fait pas partie du projet courant",
	/* 120	C_IDENT_IN_MULTIPLE_AFFECT */
	"identificateur (�ventuellement renomm�), car ceci est une affectation multiple",
	/* 121 C_REDEFINITION_OF_MININT_VALUE value */
	"Utilisation de la valeur %s pour MININT\n",
	/* 122 C_REDEFINITION_OF_MAXINT_VALUE value */
	"Utilisation de la valeur %s pour MAXINT\n",
	/* 123 C_DEFINITION_NAME */
	"un nom de d�finition (identificateur)",
	/* 124 C_OR_ELSE_END */
	"'OR', 'ELSE', 'END'",
	/* 125 C_INV_TOKEN_IN_DEF */
	"Lex�me '%s' invalide dans la definition '%s'",
	/* 126 C_EQUALS_or_BELONGS*/
	"':' or '='",
	/* 127 C_BELONGS*/
	"':'",
	/* 128 C_TYPING_PREDICAT */
	"predicat typant",
	/* 129 WITNESS_VALUATION */
	"WITNESS (Ident = Expression +&)",
    /* 130 C_TO */
    "'TO'",
    /* 131 C_PRECOND */
    "PRE ne devrait pas apparaître dans une substitution. Peut-être devriez-vous utiliser un ASSERT.",
	} ;

static const char *const *catalog_sop = NULL ;

// Ne pas mettre de traces dans cette fonction car sinon on
// a une boucle si on appelle get_catalog depuis le code de TRACE
// (c'est le cas si l'on ne peut pas ecrire dans le fichier de traces)
// Initilalisation des tables mulilingues
void french_catalog()
{
  TRACE0("french_catalog") ;
catalog_sop = french_catalog_sct ;
#ifndef NO_CHECKS
size_si = sizeof(french_catalog_sct) ;
#endif
}

// Ne pas mettre de traces dans cette fonction car sinon on
// a une boucle si on appelle get_catalog depuis le code de TRACE
// (c'est le cas si l'on ne peut pas ecrire dans le fichier de traces)
void english_catalog()
{
  TRACE0("english_catalog") ;
catalog_sop = english_catalog_sct ;
#ifndef NO_CHECKS
size_si = sizeof(english_catalog_sct) ;
#endif
}

// Ne pas mettre de traces dans cette fonction car sinon on
// a une boucle si on appelle get_catalog depuis le code de TRACE
// (c'est le cas si l'on ne peut pas ecrire dans le fichier de traces)
const char *get_catalog(T_catalog_msg catalog_msg)
{
#ifndef NO_CHECKS
  TRACE2("get_catalog(%d), size_si(%d)", catalog_msg, size_si) ;
  TRACE2("ASSERT %d < %d",
		 (size_t)catalog_msg,
		 (size_si / sizeof(char *))) ;
#endif // NO_CHECKS
  init_tables() ;
  ASSERT((size_t)catalog_msg < (size_si / sizeof(char *))) ;
  TRACE2("get_catalog(%d)->%s", catalog_msg, catalog_sop[catalog_msg]) ;
  return catalog_sop[catalog_msg] ;
}

//
//	}{	Fin du fichier
//

