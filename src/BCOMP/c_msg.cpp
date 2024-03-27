/******************************* CLEARSY **************************************
    You should have received a copy of the GNU General Public License
    along with B_COMPILER; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_msg.cpp,v 1.101 2002-11-25 16:58:43 lv Exp $") ;
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#define gettext(S) S

/* Includes Composant Local */
#include "c_api.h"

// Catalogues multilingues
extern void french_catalog(void) ;
extern void english_catalog(void) ;

// Types de lexemes multilingues
extern void french_lex_type(void) ;
extern void english_lex_type(void) ;

// Obtention des natures des identificateurs, initialisation des
// tables multilingues
extern void french_ident_type_name(void) ;
extern void english_ident_type_name(void) ;

// Langue utilisee pour dialoguer
static T_language language_sop = ENGLISH ;
T_language get_language(void)
{
    return language_sop ;
}

#define gettext_noop(String) String

static const char *const english_error_table[] =
{
    /* 00 E_PARSE_ERROR */
    gettext_noop("Parse error : illegal character '%c'"),
    /* 01 E_NO_SUCH_FILE */
    gettext_noop("Unable to open input file \"%s\" (%s)"),
    /* 02 E_EOF_INSIDE_STRING */
    gettext_noop("End of file found inside string"),
    /* 03 E_START_OF_STRING */
    gettext_noop("String starts here"),
    /* 04 E_NEW_LINE_INSIDE_STRING */
    gettext_noop("New line found inside string"),
    /* 05 E_TOKEN_PARSE_ERROR */
    gettext_noop("Parse error : illegal token %s"),
    /* 06 E_NO_INPUT_NAME_GIVEN */
    gettext_noop("No input file name given (usage : bcomp -V)"),
    /* 07 E_INVALID_COMPONENT_TYPE */
    gettext_noop("Invalid component type %s (MACHINE, REFINEMENT or IMPLEMENTATION expected)"),
    /* 08 E_BAD_COMPONENT_NAME */
    gettext_noop("Disparity between file name \"%s\" and component name \"%s\""),
    /* 09 E_COMPONENT_WITHOUT_END %s (component type) */
    gettext_noop("No \"END\" keyword matching component's %s keyword"),
    /* 10 E_MACH_INVALID_CLAUSE %s (lex name) */
    gettext_noop("Invalid token : '%s' is not a valid machine clause"),
    /* 11 E_OP_NAME_OR_PARAM_EXPECTED %s (lex name) */
    gettext_noop("Operation name or output parameters expected, got %s"),
    /* 12 E_UNEXPECTED_EOF */
    gettext_noop("Unexpected end of file"),
    /* 13 E_EMPTY_FILE */
    gettext_noop("Source file is empty"),
    /* 14 E_CONFUSED */
    gettext_noop("There were %d error(s). Stopping compilation."),
    /* 15 E_BAD_COMPONENT_SUFFIX, %s (machine type) %s (suffix) */
    gettext_noop("Disparity between component type (%s) and file suffix (%s)"),
    /* 16 E_UNUSED_MESSAGE_16 */
    "",
    /* 17 E_MACRO_DEFINITION %s (macro name) %d (macro args) */
    gettext_noop("%s is a definition with %d parameter(s) defined here"),
    /* 18 E_MACRO_INVALID_ARGS %d (macro args) %s (macro name) */
    gettext_noop("Invalid number of arguments (%d) for definition %s"),
    /* 19 E_ERROR_PARSING_ATOMIC_SUBST %s (ident name) */
    gettext_noop("Error while parsing atomic substitution starting with identifier \"%s\""),
    /* 20 E_UNUSED_MESSAGE_20 */
    "",
    /* 21 E_UNUSED_MESSAGE_21 */
    "",
    /* 22 E_REFINES_FORBIDDEN_IN_SPEC */
    gettext_noop("REFINES clause is not allowed in a specification"),
    /* 23 E_INCLUDES_FORBIDDEN_IN_IMP */
    gettext_noop("INCLUDES clause is forbidden in an implementation"),
    /* 24 E_IMPORTS_FORBIDDEN_IN_SPEC */
    gettext_noop("IMPORTS clause is not allowed in a specification"),
    /* 25 E_IMPORTS_FORBIDDEN_IN_REF */
    gettext_noop("IMPORTS clause is not allowed in a refinement"),
    /* 26 E_NO_OUTPUT_NAME_GIVEN */
    gettext_noop("No output file name given (usage : bcomp -V)"),
    /* 27 E_BAD_MAGIC */
    gettext_noop("Bad magic number (expected 0x%08x, got 0x%08x).\n\"%s\" is not a Betree dump"),
    /* 28 E_CYCLE_IN_DEFINITIONS */
    gettext_noop("Cycle found in DEFINITIONS clause"),
    /* 29 E_OUT_OF_MEMORY %d nb bytes */
    gettext_noop("Virtual memory exhausted while trying to allocate %d bytes of memory"),
    /* 30 E_IO_ERROR (%s errno) */
    gettext_noop("Fatal IO error (\"%s\")"),
    /* 31 E_PART_OF_DEF_CYCLE %s (def name) */
    gettext_noop("Definition \"%s\" is part of the cycle"),
    /* 32 E_NAME_CLASH_IN_DEF_FPARAMS %s (def name) %s (param name) */
    gettext_noop("In definition %s : redefinition of formal parameter %s"),
    /* 33 E_PROBLEM_READING_FILE %s (file name) %s (errno) */
    gettext_noop("Error while reading file \"%s\" : %s"),
    /* 34 E_PROBLEM_WRITING_FILE %s (file name) %s (errno) */
    gettext_noop("Error while creating file \"%s\" : %s"),
    /* 35 E_EITHER_EXPECTED */
    gettext_noop("Error : OR keyword illegal in first argument of choice (EITHER expected)"),
    /* 36 E_OR_EXPECTED */
    gettext_noop("Error : EITHER keyword after first argument of choice (OR expected)"),
    /* 37 E_PROBLEM_CLOSING_FILE %s (file name) %s (errno) */
    gettext_noop("Error while closing file \"%s\" : %s"),
    /* 38 E_FILE_NAME_HAS_NO_SUFFIX %s (file name) */
    gettext_noop("File name \"%s\" does not have a suffix"),
    /* 39 E_CLO_DOES_NOT_MATCH_OPN %s (clo name) */
    gettext_noop("Closing lexem \"%s\" does not match opening lexem"),
    /* 40 E_OPN_LOCALISATION %s (opn name) */
    gettext_noop("(Localisation of opening lexem \"%s\")"),
    /* 41 E_UNEXPECTED_BINOP (binop name) */
    gettext_noop("Unexpected binary operator %s"),
    /* 42 E_NOT_AN_EXPRESSION %s (formula type) */
    gettext_noop("Formula is not an expression (it is a(n) %s)"),
    /* 43 E_MISALIGNED_ADDRESS (%d (address) */
    gettext_noop("Misaligned address 0x%x"),
    /* 44 E_BAD_DOT_CONSTRUCT */
    gettext_noop("Bad syntaxic construction with '.'. Lambda-expression, Universal or Existential Predicate, SIGMA, PI, UNION or INTER expected"),
    /* 45 E_BAD_DOT_EXPLAIN */
    gettext_noop("(Hint : quantifiers must be declared as '(xx,yy..)' and not 'xx,yy..')"),
    /* 46 E_BPRED_PRED1_NOT_A_PRED %s*/
    gettext_noop("Left hand side of binary predicate is not a predicate : it is a(n) %s"),
    /* 47 E_BPRED_PRED2_NOT_A_PRED %s */
    gettext_noop("Right hand side of binary predicate is not a predicate : it is a(n) %s"),
    /* 48 E_VAR_FORBIDDEN_IN_SPEC */
    gettext_noop("VAR .. IN .. END substitution is not allowed in a specification"),
    /* 49 E_WHILE_FORBIDDEN_IN_SPEC */
    gettext_noop("WHILE substitution is not allowed in a specification"),
    /* 50 E_LET_FORBIDDEN_IN_IMP */
    gettext_noop("LET substitution is not allowed in an implementation"),
    /* 51 E_SELECT_FORBIDDEN_IN_IMP */
    gettext_noop("SELECT substitution is not allowed in an implementation"),
    /* 52 E_ANY_FORBIDDEN_IN_IMP */
    gettext_noop("ANY substitution is not allowed in an implementation"),
    /* 53 E_CHOICE_FORBIDDEN_IN_IMP */
    gettext_noop("CHOICE substitution is not allowed in an implementation"),
    /* 54 E_PRE_FORBIDDEN_IN_IMP */
    gettext_noop("PRE .. THEN .. END substitution is not allowed in an implementation"),
    /* 55 E_SEQ_FORBIDDEN_IN_SPEC */
    gettext_noop("Sequential (';') substitution is not allowed in a specification"),
    /* 56 E_WHILE_FORBIDDEN_IN_SPEC */
    gettext_noop("WHILE substitution is not allowed in a refinement"),
    /* 57 E_LR_STATE_DOES_NOT_HAVE_A_SUCCESSOR, %d lr_state */
    gettext_noop("LR state %d does not have a successor"),
    /* 58 E_LEXEM_WITHOUT_ASSOC */
    gettext_noop("Lexem does not have an associativity"),
    /* 59 E_ATOM_TYPE_NOT_YET_IMPLEMENTED %s atom type %s atom ascii */
    gettext_noop("Atom of type %s (%s) is not yet implemented"),
    /* 60 E_BINOP_TYPE_NOT_YET_IMPLEMENTED %s binop type %s binop ascii */
    gettext_noop("Binop of type %s (%s) is not yet implemented"),
    /* 61 E_UNOP_TYPE_NOT_YET_IMPLEMENTED %s unop ascii */
    gettext_noop("Unop of type %s is not yet implemented"),
    /* 62 E_PARENTH_COMPULSORY_IN_FUNCTION_CALL, %s op name */
    gettext_noop("Use of '()' is compulsory for function call"),
    /* 63 E_WHILE_PARSING_OPERATION_HEADER */
    gettext_noop("While parsing operation header"),
    /* 64 E_CL1_IMPLIES_CL2 */
    gettext_noop("Component has a %s clause but not a(n) %s clause"),
    /* 65 E_IDENT_CLASH */
    gettext_noop("Definition of identifier \"%s\" as a %s clashes with previous definition as a %s"),
    /* 66 E_IDENT_CLASH_OTHER_LOCATION */
    gettext_noop("Location of conflicting definition"),
    /* 67 E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP */
    gettext_noop("ABSTRACT_CONSTANTS clause is forbidden in an implementation"),
    /* 68 E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP */
    gettext_noop("ABSTRACT_VARIABLES clause is forbidden in an implementation"),
    /* 69 E_CONSTRAINTS_FORBIDDEN_IN_REF */
    gettext_noop("CONSTRAINTS clause is forbidden in a refinement"),
    /* 70 E_CONSTRAINTS_FORBIDDEN_IN_IMP */
    gettext_noop("CONSTRAINTS clause is forbidden in an implementation"),
    /* 71 E_COMPONENT_HAS_NO_REFINES_CLAUSE */
    gettext_noop("Component does not have a REFINES clause"),
    /* 72 E_USES_FORBIDDEN_IN_REF */
    gettext_noop("USES clause is forbidden in a refinement"),
    /* 73 E_USES_FORBIDDEN_IN_IMP */
    gettext_noop("USES clause is forbidden in an implementation"),
    /* 74 E_VALUES_FORBIDDEN_IN_REF */
    gettext_noop("VALUES clause is forbidden in a refinement"),
    /* 75 E_VALUES_FORBIDDEN_IN_SPEC */
    gettext_noop("VALUES clause is forbidden in a specification"),
    /* 76 E_REQUIRED_COMPONENT_MISSING %s component name */
    gettext_noop("Required component %s is missing"),
    /* 77 E_REF_DOES_NOT_DUPLICATE_SEES %s %s seen name, %s abstract name */
    gettext_noop("No SEES %s clause for component %s seen by abstraction %s"),
    /* 78 E_SEES_LOCATION %s seen name */
    gettext_noop("Location of SEES %s clause in abstraction"),
    /* 79 E_REQ_COMP_NOT_A_SPEC */
    gettext_noop("Required component is not a specification"),
    /* 80 E_REF_COMP_NOT_A_SPEC */
    gettext_noop("Refined component is not a specification"),
    /* 81 E_REF_COMP_NEITHER_SPEC_NOR_REF */
    gettext_noop("Refined component is neither a specification, nor a refinement"),
    /* 82 E_PARAM_N_IS_NOT_REFINED %d param number, %s param name */
    gettext_noop("Formal parameter #%d of abstraction (\"%s\") is missing"),
    /* 83 E_PARAM_N_IS_NOT_SPECIFIED %d param number, %s param name */
    gettext_noop("Formal parameter #%d (\"%s\") is not a parameter of the abstraction"),
    /* 84 E_LOCATION_OF_ABSTRACT_PARAMS */
    gettext_noop("Definition of the formal parameters of the abstraction"),
    /* 85 E_LOCATION_OF_PARAM_N_IN_ABSTRACTION param number */
    gettext_noop("Definition of the formal parameter #%d of the abstraction"),
    /* 86 E_PARAM_N_DIFFERS %d par number, %s par name %d par number, %s par name */
    gettext_noop("Formal parameter #%d (\"%s\") differs from formal parameter #%d (\"%s\") of the abstraction"),
    /* 87 E_CAN_NOT_IMPORT_A_MACHINE_THAT_USES machine name */
    gettext_noop("Machine %s can not be imported since it contains a USES clause"),
    /* 88 E_CAN_NOT_EXTEND_A_MACHINE_THAT_USES machine name */
    gettext_noop("Machine %s can not be extended since it contains a USES clause"),
    /* 89 E_CAN_NOT_SEE_A_MACHINE_THAT_USES machine name */
    gettext_noop("Machine %s can not be seen since it contains a USES clause"),
    /* 89 E_CAN_NOT_REFINE_A_MACHINE_THAT_USES machine name */
    gettext_noop("Machine %s can not be refined since it contains a USES clause"),
    /* 91 E_OPERATION_IS_NOT_SPECIFIED %s op name %s spec name */
    gettext_noop("Operation %s is not an operation of abstraction %s"),
    /* 92 E_OPERATION_IS_NOT_REFINED %s op name %s comp name */
    gettext_noop("Operation %s is not refined in component %s"),
    /* 93 E_PARALLEL_FORBIDDEN_IN_SPEC */
    gettext_noop("Parallel ('||') substitution is not allowed in an implementation"),
    /* 94 E_BECOMES_SUCH_THAT_FORBIDDEN_IN_IMP */
    gettext_noop("Becomes such that (':') substitution is not allowed in an implementation"),
    /* 95 E_BECOMES_MEMBER_OF_FORBIDDEN_IN_IMP */
    gettext_noop("Becomes member of ('::') substitution is not allowed in an implementation"),
    /* 96 E_SPEC_SHOULD_HAVE_A_CONSTRAINTS_CL */
    gettext_noop("Specification has formal scalar parameters and no CONSTRAINTS clause"),
    /* 97 E_SPEC_SHOULD_NOT_HAVE_A_CONSTRAINTS_CL */
    gettext_noop("CONSTRAINTS clause is forbidden in a specification without formal scalar parameters"),
    /* 98 E_BEFORE_ONLY_IN_WHILE_AND_BEC_SUCH_THAT */
    gettext_noop("'$0' expression is only allowed in WHILE and in 'becomes such that' substitutions"),
    /* 99 E_COMPONENT_CAN_NOT_REFINE_ITSELF */
    gettext_noop("Component can not refine itself"),
    /* 100 E_COMPONENT_MULTIPLY_REFERENCED %s component name */
    gettext_noop("Component %s is multiply referenced"),
    /* 101 E_MULTIPLE_IMPORTATION_OF_COMPONENT component name */
    gettext_noop("Component %s is multiply imported"),
    /* 102 E_LOCATION_OF_COMPONENT_IMPORT component name */
    gettext_noop("Component %s is imported here"),
    /* 103  E_UNDECLARED_IDENT %s ident name */
    gettext_noop("Undeclared identifier \"%s\""),
    /* 104 E_NO_TYPE_FOR_IDENT %s ident type %s ident name */
    gettext_noop("No type given for %s %s"),
    /* 105 E_LOCATION_OF_OP_SPEC */
    gettext_noop("Location of operation specification"),
    /* 106 E_ILLEGAL_TYPE_FOR_IDENT %s type %s ident name */
    gettext_noop("Illegal type %s for %s %s"),
    /* 107 E_LOCATION_OF_IDENT_TYPE,  ident name */
    gettext_noop("Location of type %s definition for %s %s"),
    /* 108 E_UNEXPECTED_ARGUMENT_IN_EQPRED */
    gettext_noop("Unexpected expression in equality predicate (should be of type Exp = Exp or Exp /= Exp)"),
    /* 109 RHS_OF_BELONGS_MUST_BE_A_SET %s type */
    gettext_noop("Right hand side of ':' predicate should be a Set (it is %s)"),
    /* 110 E_UNBALANCED_AFFECTATION % d nb_ident %d nb_values */
    gettext_noop("Unbalanced affectation : there are %d identifiers on the lhs but %d expressions on the rhs"),
    /* 111 E_WRONG_NUMBER_OF_TYPES_GIVEN %s type name %d nb ident */
    gettext_noop("Given type %s does not type the %d identifiers on the left hand side"),
    /* 112 E_OP_PARAM_NOT_SPECIFIED %s ident_type %s ident_type */
    gettext_noop("%s %s not specified"),
    /* 113 E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM */
    gettext_noop("STRING type can only be used to type operation input parameters, with the belong predicate"),
    /* 114 E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM2 %s id name %s id type */
    gettext_noop("STRING type can only be used to type operation input parameters (%s is a %s)"),
    /* 115 E_NON_VISIBLE_ENTITY %s name %s rights */
    gettext_noop("Illegal access to non-visible entity \"%s\" (rights are \"%s\" for %s \"%s\" declared in \"%s\", %s, from %s of \"%s\")"),
    /* 116 E_NON_VISIBLE_ENTITY_DECLARATION %s name */
    gettext_noop("Location of non-visible entity \"%s\" declaration"),
    /* 117 E_INCOMPATIBLE_TYPES %s prev_type %s new_type */
    gettext_noop("New type %s is incompatible with previous type %s"),
    /* 118 E_WRONG_NUMBER_OF_EFFECTIVE_IN_PARAMS %s op %d eff %d formal */
    gettext_noop("In operation %s call: wrong number of effective in parameters (%d given parameters, %d expected)"),
    /* 119 E_WRONG_NUMBER_OF_EFFECTIVE_OUT_PARAMS %s op %d eff %d formal */
    gettext_noop("In operation %s call: wrong number of effective out parameters (%d parameters given, %d expected)"),
    /* 120 E_LOCATION_OF_ITEM_DEF %s name */
    gettext_noop("Location of \"%s\" definition"),
    /* 121 E_CALLED_ITEM_IS_NOT_AN_OPERATION %s op_name */
    gettext_noop("Impossible to call \"%s\" since it is not an operation"),
    /* 122 E_ACCESS_UNTYPED_IDENT %s ident name */
    gettext_noop("Identifier \"%s\" can not be accessed since it has not been typed"),
    /* 123 RHS_OF_BECOMES_MEMBER_OF_THAT_MUST_BE_A_SET %s type */
    gettext_noop("Right hand side of '::' substitution should be a Set (it is %s)"),
    /* 124 E_UNBALANCED_BECOMES_MEMBER_OF % d nb_ident %d nb_values */
    gettext_noop("Unbalanced '::' substitution : there are %d identifiers on the lhs but %d sets on the rhs"),
    /* 125 E_ITEM_OF_CARTESIAN_PROD_IS_NOT_A_SET */
    gettext_noop("Item of cartesian product is not a set"),
    /* 126 E_UNCOMPATIBLE_EXTENSIVE_ITEM_TYPE %s cur_type %s ref_type */
    gettext_noop("In extensive set : item type %s is not compatible with the type of the items of the set (%s)"),
    /* 127 E_INVALID_INTERVAL */
    gettext_noop("Invalid interval"),
    /* 128 E_OPERAND_SHOULD_BE_AN_INTEGER %s type_name */
    gettext_noop("Operand is not an integer (its type is %s)"),
    /* 129 E_OPERAND_SHOULD_BE_AN_INTEGER_OR_A_SET %s type_name */
    gettext_noop("Operand is not an integer neither a set (its type is %s)"),
    /* 130 E_OPERAND_SHOULD_BE_A_SET %s type_name */
    gettext_noop("Operand is not a set (given operand type is %s)"),
    /* 131 E_INVALID_OP_CALL %s op_name */
    gettext_noop("While calling operation %s : invalid use of ':=' (use '<--' instead)"),
    /* 132 E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND  %s op name %d nb_oper */
    gettext_noop("Builtin operator %s has only one operand (%d operands given)"),
    /* 133 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a set operand (given operand type is %s)"),
    /* 134 E_UNCOMPATIBLE_SET_OPERAND %s type %s ref_type */
    gettext_noop("Set type %s is not compatible with first operand type %s"),
    /* 135 E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_SET %s op name %s oper type */
    gettext_noop("Builtin operator %s requires an integer set operand (given operand type is %s)"),
    /* 136 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a sequence operand (given operand type is %s)"),
    /* 137 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a non empty sequence operand (given operand type is %s)"),
    /* 138 E_OPERAND_OF_SET_RELATION_IS_NOT_A_SET %s type name */
    gettext_noop("Operand of set relation '<->' should be a set (given operand type is %s)"),
    /* 139 E_OPERAND_OF_CONVERSE_IS_NOT_A_RELATION %s type name */
    gettext_noop("Operand of converse '~' should be a relation (given operand type is %s)"),
    /* 140 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a relation operand (given operand type is %s)"),
    /* 141 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A %s op name %s oper type */
    gettext_noop("Builtin operator %s requires an operand of type 'relation from a A to A' (given operand type is %s)"),
    /* 142 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION %s op name %s oper type */
    gettext_noop("Builtin operator %s requires its first operand to be of type relation (given operand type is %s)"),
    /* 143 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A %s op name %s oper type */
    gettext_noop("Builtin operator %s requires its first operand to be of type 'relation from a A to A' (given operand type is %s)"),
    /* 144 E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER %s op name %s oper type */
    gettext_noop("Builtin operator %s requires is second operand to be an integer (given operand type is %s)"),
    /* 145 E_BUILTIN_OP_NEEDS_TWO_OPERANDS %s op name %d nb oper */
    gettext_noop("Builtin operator %s needs two operands (%d operands given)"),
    /* 146 E_OPERAND_OF_COMPOSITION_SHOULD_BE_A_RELATION %s oper type */
    gettext_noop("Operand of composition operator ';' should be a relation (given operand type is %s)"),
    /* 147 E_OPERANDS_OF_COMPOSITION_SHOULD_HAVE_MATCHING_TYPES %s t1 %s t2 */
    gettext_noop("Operands of compositor operator ';' should have compatible types (given types are %s and %s"),
    /* 148 E_LOCATION_OF_BOGUS_COMPOSITION_OPERAND */
    gettext_noop("Location of bad composition operator ';' operand"),
    /* 149 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER %s type */
    gettext_noop("First operand of predicate should be an integer (given operand type is %s)"),
    /* 150 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER %s type */
    gettext_noop("Second operand of predicate should be an integer (given operand type is %s)"),
    /* 151 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_SET %s type */
    gettext_noop("First operand of predicate should be a set (given operand type is %s)"),
    /* 152 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_SET %s type */
    gettext_noop("Second operand of predicate should be a set (given operand type is %s)"),
    /* 153 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
    gettext_noop("First operand of builtin operator %s is not a set (given operand type is %s)"),
    /* 154 E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
    gettext_noop("Second operand of builtin operator %s is not a set (given operand type is %s)"),
    /* 155 E_VARIANT_IS_NOT_BE_AN_INTEGER %s variant type */
    gettext_noop("WHILE loop variant is not an integer (given variant type is %s)"),
    /* 156 E_IDENT_NOT_TYPED_IN_BECOMES_SUCH_THAT_PRED %s ident_name */
    gettext_noop("Identifier %s is not typed in the predicate of 'becomes such that'"),
    /* 157 E_IDENT_IS_NOT_AN_ARRAY %s type */
    gettext_noop("Identifier %s is not an array (given type is %s)"),
    /* 158 E_ITEM_IS_NOT_AN_ARRAY %s type */
    gettext_noop("Item is not an array (given type is %s)"),
    /* 159 E_BAD_INDEXES_FOR_ARRAY %s idx type %s array type */
    gettext_noop("The type %s of the indexes is not compatible with the type %s of the array"),
    /* 160 E_UNABLE_TO_FIND_CONTEXT_FOR_IDENT %s ident_name %s ident_type */
    gettext_noop("Unable to find context for identifier \"%s\" (which is a %s)"),
    /* 161 E_EXPR_INCOMPATIBLE_TYPES %s prev_type %s new_type */
    gettext_noop("Type %s is incompatible with type %s"),
    /* 162 E_VARIABLE_IS_ASSIGNED_IN_PARRALLEL %s variable */
    gettext_noop("Variable %s is modified in two parallel substitutions"),
    /* 163 E_LOCATION_OF_PREVIOUS_ASSIGN variable */
    gettext_noop("Location of previous modification of variable %s"),
    /* 164 E_LHS_OF_LET_VALUATION_MUST_BE_A_LOC_VAR %s variable */
    gettext_noop("%s can not be valued here since is not a local variable of the LET substitution"),
    /* 165 E_MULTIPLE_TYPE_IN_LET_VALUATION %s variable */
    gettext_noop("%s can not be valued here since it has already been valued in this LET substitution"),
    /* 166 E_INCOMPATIBLE_TYPES_IN_PRED %s type %s type */
    gettext_noop("Type %s is incompatible with type %s"),
    /* 167 E_INCOMPATIBLE_TYPES_IN_EXPR %s type %s first_type */
    gettext_noop("Type %s is incompatible with first type %s"),
    /* 168 E_LHS_OF_BINOP_SHOULD_BE_A_SEQ %s type */
    gettext_noop("Lhs of binary operator should be a sequence (given type is %s)"),
    /* 169 E_RHS_OF_BINOP_SHOULD_BE_A_SEQ %s type */
    gettext_noop("Rhs of binary operator should be a sequence (given type is %s)"),
    /* 170 E_ILLEGAL_TYPE_FOR_ITEM_TO_ADD_IN_SEQ %s itype %s stype */
    gettext_noop("Can not add an item of type %s to a sequence of type %s"),
    /* 171 E_OPERAND_SHOULD_BE_A_SEQ %s type */
    gettext_noop("Operand should be a sequence (given type is %s)"),
    /* 172 E_UNKNOWN_OPERATION_CAN_NOT_BE_PROMOTED %s op_name */
    gettext_noop("Unknown or non-visible operation %s can not be promoted"),
    /* 173 E_OP_PARAM_NOT_REFINED, %s ident_type %s ident_type */
    gettext_noop("%s %s has not been refined"),
    /* 174 E_OPERAND_OF_IMAGE_SHOULD_BE_A_RELATION %s type */
    gettext_noop("First operand of image should be a relation (given type is %s)"),
    /* 175 E_SECOND_OPERAND_OF_IMAGE_SHOULD_BE_A_SUBSET_OF  %s set %s type */
    gettext_noop("Second operand of image should be a subset of %s (given type is %s)"),
    /* 176 E_UNABLE_TO_FIND_LINK_BETWEEN_COMPONENTS %s name1 %s name2 */
    gettext_noop("Unable to find the visibility link between components %s and %s"),
    /* 177 E_INVALID_VALUATION_OF_ABSTRACT_SET %s abs set name */
    gettext_noop("Invalid valuation of abstract set %s (given type is neither an abstract set, nor an finite and non-empty interval of integers)"),
    /* 178 E_ANY_DATA_CAN_NOT_BE_MODIFIED %s name */
    gettext_noop("Abstract data %s of ANY clause can not be modified"),
    /* 179 E_NO_CLOSE_PAREN_MATCHING_OPEN_PAREN */
    gettext_noop("No ')' matching '(' around previous substitution"),
    /* 180 E_OPEN_PAREN_LOCATION */
    gettext_noop("Location of unmatched '('"),
    /* 181 E_VAR_OF_LAMBDA_EXPR_NOT_TYPED %s var name */
    gettext_noop("Variable %s of lambda expression is not typed in the typing predicate"),
    /* 182 E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER %s op name %s oper type */
    gettext_noop("Builtin operator %s requires an integer operand (given operand type is %s)"),
    /* 183 E_FIRST_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_SET %s type */
    gettext_noop("First operand of (anti)restriction is not a set (given operand type is %s)"),
    /* 184 E_SECOND_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_RELATION %s type */
    gettext_noop("Second operand of (anti)restriction is not a relation (given operand type is %s)"),
    /* 185 E_FIRST_OPERAND_OF_CORESTRICT_IS_NOT_A_RELATION %s type */
    gettext_noop("First operand of corestriction is not a relation (given operand type is %s)"),
    /* 186 E_SECOND_OPERAND_OF_CORESTRICT_IS_NOT_A_SET %s type */
    gettext_noop("Second operand of corestriction is not a set (given operand type is %s)"),
    /* 187 E_PRAGMA_WITHOUT_OPEN_PAREN */
    gettext_noop("Invalid pragma \"%s\" has no '(' : parameters will be ignored"),
    /* 188 E_EOF_INSIDE_PRAGMA */
    gettext_noop("Invalid pragma \"%s\" : missing parameters : parameters will be ignored"),
    /* 189 E_PRAGMA_WITH_NO_NAME */
    gettext_noop("Invalid pragma \"%s\" has no identifier : it will be ignored"),
    /* 190 E_SECOND_OPERAND_OF_DPROD_IS_NOT_A_RELATION %s type */
    gettext_noop("Second operand of direct product is not a relation (given operand type is %s)"),
    /* 191 E_FIRST_OPERAND_OF_DPROD_IS_NOT_A_RELATION %s type */
    gettext_noop("First operand of direct product is not a relation (given operand type is %s)"),
    /* 192 E_TRAILING_GARBAGE_AFTER_PRAGMA */
    gettext_noop("Trailing garbage after pragma"),
    /* 193 E_WRONG_NUMBER_OF_EFFECTIVE_MACHINE_PARAMS %d eff %d formal %s mach name*/
    gettext_noop("Wrong number of effective parameters for machine %s (%d given parameter(s), %d expected)"),
    /* 194 E_INCOMPATIBLE_TYPES_FOR_MACHINE_PARAMS %s etype %s ftype %s pname */
    gettext_noop("Effective parameter type %s is incompatible with type %s of formal parameter %s"),
    /* 195 E_OP1RELDST_INCOMPATIBLE_TYPES_OP2BASE %s lhs type name %s rhs type name */
    gettext_noop("Lhs relation destination type %s is incompatible with rhs base type %s"),
    /* 196 E_IMAGE_EXPR_TYPES_GEQ_REL_TYPES %d nb_expr_types %d nb_rel_types */
    gettext_noop("In [] : expression has %d types, which is greater or equal than the number of types in the relation (%d)"),
    /* 197 E_OP2RELSRC_INCOMPATIBLE_TYPES_OP1BASE %s lhs type name %s rhs type name */
    gettext_noop("Rhs relation source type %s is incompatible with lhs base type %s"),
    /* 198 E_IN_EXTENSIVE_REC_PAREN_EXPR_EXPECTED %s class_name */
    gettext_noop("In extensive record : expression with parenthesis expected, got %s"),
    /* 199 E_RECORD_LABEL_EXPECTED %s class name */
    gettext_noop("Record label expected, got %s"),
    /* 200 E_FIRST_OPERAND_OF_L_OVERLOAD_IS_NOT_A_RELATION %s type */
    gettext_noop("First operand of left overload is not a relation (given operand type is %s)"),
    /* 201 E_SECOND_OPERAND_OF_L_OVERLOAD_IS_NOT_A_SET %s type */
    gettext_noop("Second operand of left overload is not a relation (given operand type is %s)"),
    /* 202 E_INCOMPATIBLE_INDEX_TYPE %s idx type %d idx nb %s fct type */
    gettext_noop("Type %s of index number %d is incompatible with expected type %s"),
    /* 203 E_LABEL_COMPULSORY */
    gettext_noop("A record label is compulsory here"),
    /* 204 E_EOF_FOUND_WHILE_PARSING_DEFINITION %s def name */
    gettext_noop("End of file or component found while parsing definition %s"),
    /* 205 E_TYPE_SHOULD_BE_A_SET_OF_SET type name */
    gettext_noop("Type should be a set of set i.e. POW(POW(t)) (given type is %s)"),
    /* 206 E_SECOND_OPERAND_OF_PPROD_IS_NOT_A_RELATION %s type */
    gettext_noop("Second operand of parallel product is not a relation (given operand type is %s)"),
    /* 207 E_FIRST_OPERAND_OF_PPROD_IS_NOT_A_RELATION %s type */
    gettext_noop("First operand of parallel product is not a relation (given operand type is %s)"),
    /* 208 E_NO_TYPE_GIVEN_FOR_EFFECTIVE_PARAMETER */
    gettext_noop("No type given for effective parameter"),
    /* 209 E_CALLED_OP_NAME_SHOULD_BE_AN_IDENT %s class name */
    gettext_noop("Called operation name should be an identifier (given name is a(n) %s)"),
    /* 210 E_RHS_OF_VALUATION_HAS_NO_TYPE */
    gettext_noop("Right hand side of valuation does not have a type"),
    /* 211 E_RECORD_ITEM_WITHOUT_LABEL */
    gettext_noop("The label of this record item can not be retrieved from the other record item since it has no label either"),
    /* 212 E_LOCATION_OF_OTHER_JOKER */
    gettext_noop("Location of other record item without label"),
    /* 213 E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_RECORD %s field %s type name */
    gettext_noop("Can not access field %s of this expression since it is not a record (given type is %s)"),
    /* 214 E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_PART_OF_RECORD %s field %s type name */
    gettext_noop("Can not access field %s of this expression since it is not a label of this record type (given record type is %s)"),
    /* 215 E_CAN_NOT_TYPE_IDENT_WITH_JOKER_RECORD_TYPE %s type name */
    gettext_noop("Identifier can not be typed with this record type since it contains jokers (given type is: %s)"),
    /* 216 E_FILE_DEF_NAME_WITH_PATH %s file_name */
    gettext_noop("Definitions file name can not include a path. Use \"file\" to fetch a local file or <file> to fetch a library file."),
    /* 217 E_UNABLE_TO_FIND_OBJECT_AT_RANK %d rank */
    gettext_noop("Unable to find object at rank %d - File is corrupted"),
    /* 218 E_IDENT_INVALID_FOR_KERNEL %s (ident name) */
    gettext_noop("Identifier \"%s\" has less than 2 characters : it will be rejected by Atelier-B's Logic Solver"),
    /* 219 E_NO_COMPONENT_MATCH_IN_MULTI_COMPO_FILE %s compo_name %s file_name */
    gettext_noop("There should be a component named %s in this file (\"%s\")"),
    /* 220 E_COMPONENT_CAN_NOT_BE_IN_THIS_FILE %s compo name */
    gettext_noop("Component %s can not be part of this file"),
    /* 221 E_ONLY_ONE_COMPONENT_CAN_BE_STORED_IN_FILE %s compo name %s file_name */
    gettext_noop("Component %s can not be stored in file %s since it is not a multi-component file"),
    /* 222 E_INVALID_EXPR_WITH_EQUAL */
    gettext_noop("Invalid expression : <expr \"=\" expr> is not an expression, but a predicate."),
    /* 223 E_NO_TYPE_FOR_IDENT_LOC_EXPECTED %s ident_type %s ident_name %s ident_name %s clause name %s compo name */
    gettext_noop("No type given for %s %s. %s should have been typed in the %s clause of %s"),
    /* 224 E_IDENT_CAN_NOT_BE_TYPED_HERE %s ident_type %s ident_name %s ident_name %s clause name %s clause name */
    gettext_noop("%s %s can not be typed in the %s clause. It should be typed in the %s clause"),
    /* 225 E_CAN_NOT_WRITE_OP %s op_name */
    gettext_noop("%s is an operation : it can not be on the left-hand side of an affectation"),
    /* 226 E_IDENT_HAS_NOT_BEEN_VALUATED %s ident_type %s ident_name %s mach name*/
    gettext_noop("%s %s has not been valuated in the VALUES clause of %s"),
    /* 227 E_LOCATION_OF_VALUES_CLAUSE */
    gettext_noop("Location of VALUES clause"),
    /* 228 E_IDENT_HAS_NOT_BEEN_INITIALISED %s ident_type %s ident_name %s mach name*/
    gettext_noop("%s %s has not been initialised in the INITIALISATION clause of %s"),
    /* 229 E_LOCATION_OF_INITIALISATION_CLAUSE */
    gettext_noop("Location of INITIALISATION clause"),
    /* 230 E_INVALID_IDENTIFIER %s ident_name */
    gettext_noop("Invalid identifier \"%s\" : '$' can only be used inside the \"$0\" suffix for an identifier"),
    /* 231	E_GARBAGE_BEFORE_COMPONENT */
    gettext_noop( gettext_noop("Garbage lexem before component beginning")),
    /* 232 E_MISSING_DOT %s expr name */
    gettext_noop("Missing '.' in %s expression construction should be here"),
    /* 233 E_SET_FORMAL_PARAMETERS_CAN_NOT_BE_AN_EMPTY_INTERVAL */
    gettext_noop("A Set formal parameter can not be valuated with an empty interval"),
    /* 234 E_DEFINITION_CAN_NOT_REWRITE_ITSELF %s def_name */
    gettext_noop("Definition %s can not be part of its own rewrite rule"),
    /*  235 E_UNEXPECTED_NODE_TYPE %d type %s name */
    gettext_noop("Unexpected node type %d (%s)"),
    /* 236 E_CAN_NOT_CREATE_SYNTAXIC_SKEL_REF_PROMOTES %s name */
    gettext_noop("Can not create syntaxic refinement of component %s since it has a PROMOTES clause"),
    /* 237 E_CONSTANTS_SHOULD_NOT_BE_RENAMED %s ident_name %s ident_type %s machine def name */
    gettext_noop("The \"%s\" identifier, defined as a(n) %s in %s, should not be renamed"),
    /* 238 E_VARIABLES_SHOULD_BE_RENAMED %s ident_name %s ident_type %s machine def name*/
    gettext_noop("The \"%s\" identifier, defined as a(n) %s in %s, is not renamed or is renamed with a bad prefix"),
    /* 239 E_CAN_NOT_REFINES, %s ident_type %s ident_name %s ident_type*/
    gettext_noop("The %s \"%s\" can not refine a(n) %s"),
    /* 240 E_ENUMERATED_IDENT_DIFFER %s SET's name %s glued val %s original val*/
    gettext_noop("In the redefinition of \"%s\", the enumerated value \"%s\" differs from \"%s\""),
    /* 241 E_ENUMERATED_IDENT_EXCESS %s SET's name %s execess val */
    gettext_noop("In the redefinition of \"%s\", enumerated value \"%s\" is exceeding"),
    /* 242 E_ENUMERATED_IDENT_MISSING %s SET's name %s missing value*/
    gettext_noop("In the redefinition of \"%s\", enumerated value \"%s\" is missing"),
    /* 243 E_ITEM_IS_NOT_ALLOWED_IN_AN_IMPLEMENTATION %s class_name */
    gettext_noop("%s is not allowed in an implementation"),
    /* 244 E_RECORD_IS_NOT_A_B0_RECORD,  %s class_name */
    gettext_noop("%s is not implementable"),
    /* 245 E_B0_INVALID_ARRAY_EXPRESSION,  %s class_name (table_access) */
    gettext_noop("Invalid array expression %s"),
    /* 246 E_B0_INDEX_MUST_BE_A_TERM,  table_access */
    gettext_noop("In an array access, index must be a term"),
    /* 247 E_B0_INVALID_BUILTIN_FOR_RANGE,  %s class_name  */
    gettext_noop("Invalid range %s "),
    /* 248 E_B0_INTERVAL_BOUND_MUST_BE_ARITH_EXPRESSION,  (Intervalle) */
    gettext_noop("Invalid interval bound %s is not an arithmetic expression"),
    /* 249 E_B0_INVALID_EFFECTIVE_PARAMETERS_IN_OP_CALL, %s op_name %s class_name */
    gettext_noop("In %s call : %s is not a valid effective parameter (term or string expected)"),
    /* 250 E_B0_INVALID_RANGE_FOR_ARRAY_AGGREGATE,  %s class_name  */
    gettext_noop("Invalid index set %s for array aggregate"),
    /* 251 E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE %s class_name */
    gettext_noop("Invalid value %s for array aggregate"),
    /* 252 E_B0_INVALID_INDEX_FOR_MAPLET,  %s class_name  */
    gettext_noop("Invalid index %s for maplet (literal scalar expected)"),
    /* 253 E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION,  %s class_name */
    gettext_noop("Invalid operand %s of %s (arithmetic expression expected)"),
    /* 254 E_B0_PARAMETER_MUST_BE_A_BOOL_EXPRESSION,  %s class_name  */
    gettext_noop("Invalid operand %s of bool (boolean expression expected)"),
    /* 255 E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION,* %s class_name  */
    gettext_noop("%s is not a condition"),
    /* 256 E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM,  %s class_name  */
    gettext_noop("%s is not a simple term"),
    /* 257 E_B0_INVALID_VALUE_FOR_MAPLET %s class_name */
    gettext_noop("Invalid value %s for maplet"),
    /* 258 E_B0_INVALID_MAPLET,   */
    gettext_noop("Maplet is not implementable"),
    /* 259 E_B0_INVALID_TERM %s class_name */
    gettext_noop("%s is not a term"),
    /* 260 E_B0_INVALID_SIMPLE_TERM %s class_name */
    gettext_noop("%s is not a simple term"),
    /* 261 E_B0_INVALID_EFFECTIVE_PARAMETER,   */
    gettext_noop("Effective parameter is not implementable"),
    /* 262 E_B0_INVALID_RECORD_ITEM,    */
    gettext_noop("Record item is not implementable"),
    /* 263 E_B0_INVALID_CASE_SELECTOR  %s class_name   */
    gettext_noop("Invalid case selector %s (simple term expected)"),
    /* 264 E_B0_INVALID_SIMULTANEOUS_AFFECTATION */
    gettext_noop("Simultaneous affectation is not implementable"),
    /* 265 E_B0_INVALID_LHS_IN_AFFECTATION   */
    gettext_noop("Invalid lhs in affectation"),
    /* 266 E_B0_INVALID_RHS_IN_AFFECTATION    */
    gettext_noop("Invalid rhs in affectation"),
    /* 267 E_B0_INVALID_IDENT_TYPE_FOR_FORMAL_PARAMETER  %s ident_type_name  , %s name */
    gettext_noop("Invalid identifier type %s for formal scalar parameter %s"),
    /* 268 E_B0_INVALID_DEST_SET_FOR_TOTAL_FUNCTION   %s class_name   */
    gettext_noop("Invalid destination set %s for a total function"),
    /* 269 E_B0_INVALID_RELATION_FOR_CONCRETE_TYPE    */
    gettext_noop("Invalid relation type for concrete type (total function expected)"),
    /* 270 E_B0_INVALID_SRC_SET_FOR_TOTAL_FUNCTION   %s class_name   */
    gettext_noop("Invalid source set %s for a total function"),
    /* 271 E_B0_INVALID_SIMPLE_SET	 %s class_name   */
    gettext_noop("Invalid simple set %s"),
    /* 272 E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT  %s class_name, %s name  */
    gettext_noop("Invalid typing expression %s for concrete constant %s"),
    /* 273 E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE	 %s class_name, %s name  */
    gettext_noop("Invalid typing expression %s for concrete variable %s"),
    /* 274 E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER	 %s class_name, %s name */
    gettext_noop("Invalid typing expression %s for formal scalar parameter %s"),
    /* 275 E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER	 %s class_name, %s name */
    gettext_noop("Invalid typing expression %s for in operation parameter %s"),
    /* 276 E_B0_INVALID_TYPING_PREDICATE     */
    gettext_noop("Invalid typing predicate ( = or : expected)"),
    /* 277 E_B0_INVALID_IDENT_TYPE_FOR_ARRAY_INDEX   %s ident_type_name  , %s name */
    gettext_noop("Invalid identifier type %s for array index %s"),
    /* 278 E_B0_SCALAR_EXPECTED  %s class_name */
    gettext_noop("Invalid type for expression %s : scalar expected" ),
    /* 279 E_B0_INVALID_TYPING_EXPR_FOR_RECORD_ITEM	 %s class_name   */
    gettext_noop("Invalid typing expression %s for record_item"),
    /* 280 E_B0_INVALID_TYPE	 %s ident_type_name  , %s name */
    gettext_noop("Type of %s %s not implementable"),
    /* 281 E_B0_INVALID_TYPE_FOR_RECORD_LABEL	    */
    gettext_noop("Type of record label not implementable"),
    /* 282 E_B0_INVALID_EXPR_FOR_INSTANCIATION  %s class_name   */
    gettext_noop("Invalid expression %s for formal parameter instanciation"),
    /* 283 E_B0_INVALID_INDEX_FOR_ARRAY_VALUATION	%s class_name (literal scalar expected)  */
    gettext_noop("Invalid index %s for array constant valuation (literal scalar expected)"),
    /* 284 E_B0_INVALID_IDENT_TYPE_FOR_INSTANCIATION	%s ident_type_name  , %s name */
    gettext_noop("Invalid ident type %s for identifier %s in an instanciation context"),
    /* 285 E_B0_INVALID_IDENT_TYPE_FOR_VALUATION	%s ident_type_name  , %s name */
    gettext_noop("Invalid ident type %s for identifier %s in a valuation context"),
    /* 286 E_B0_INVALID_EXPR_FOR_VALUATION	%s class_name , %s ident_type_name, %s ident_name  */
    gettext_noop("Invalid expression %s for valuation of %s %s"),
    /* 287 E_B0_INTERVAL_BOUND_MUST_BE_LITERAL_INTEGER  %s class_name */
    gettext_noop("Invalid interval bound (literal integer expected)"),
    /* 288 E_B0_INVALID_IDENT_TYPE_FOR_CONCRETE_VALUE 	%s ident_type_name  , %s name */
    gettext_noop("Invalid ident type %s for identifier %s (concrete data expected)"),
    /* 289 E_B0_IDENTIFIER_ALREADY_VALUED  %s ident_type_name  , %s name */
    gettext_noop("%s %s is already valued"),
    /* 290 E_COMPO_CLASH %s compo_name */
    gettext_noop("Illegal redefinition of component \"%s\""),
    /* 291 E_UNEXPECTED_TYPE_CASE_BRANCH %s class_name*/
    gettext_noop("Invalid expression \"%s\" in the case branch (literal integer, literal boolean or literal enumerated value expected)"),
    /* 292 E_BRANCH_TYPE_DIFFER_FROM_SELECTOR_TYPE %s branch type, %s selector type*/
    gettext_noop("The branch type %s differs from selector type (%s)"),
    /* 293 E_BRANCH_VALUE_ALREADY_EXIST */
    gettext_noop("This case branch value already exist"),
    /* 294 E_INVALID_RENAMED_OP_IN_SPEC %s renop name */
    gettext_noop("Invalid operation name %s : an operation name in a specification can not be renamed"),
    /* 295 E_LHS_OF_LET_SUBSTITUTION_CAN_NOT_BE_A_LOC_VAR %s variable */
    gettext_noop("%s can not be valued here since is a local variable of the LET substitution"),
    /* 296 E_INVALID_UNIVERSAL_PREDICATE */
    gettext_noop("Invalid universal predicate. Valid universal predicates are of type !x.(P1=>P2) or !(x1,..xn).(P1=>P2)"),
    /* 297 E_BEFORE_ONLY_IF_VAR_PART_OF_BECOMES_SUCH_THAT %s ident name */
    gettext_noop("$0 can not be used since %s is not a local quantifier of becomes such that"),
    /* 298 E_BEFORE_ONLY_IF_VAR_OF_WHILE_SPECIFIED_IN_ABSTRACTION %s ident_name %s mach name %s abstraction_name */
    gettext_noop("$0 can not be used since %s is not specified in %s's abstraction %s"),
    /* 299 E_B0_FORMAL_SET_PARAMETERS_CAN_NOT_BE_TYPED %s ident name*/
    gettext_noop("Formal set parameter \"%s\" can not be typed"),
    /* 300 E_BEFORE_ONLY_INVARIANT_OF_WHILE, %s ident name */
    gettext_noop("$0 can not be used since %s is not in a predicate of the INVARIANT of a WHILE loop"),
    /* 301 E_B0_MAPLET_ALREADY_EXISTS */
    gettext_noop("There is already a maplet with this index"),
    /* 302 E_B0_LACKS_OF_MAPLETS_FOR_CURRENT_ARRAY */
    gettext_noop("There are some lacks in maplet set for a total coverage of the array"),
    /* 303 E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS */
    gettext_noop("LHS and RHS arrays are incompatible"),
    /* 304 E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_FORMAL_AND_EFFECTIVE_PARAM */
    gettext_noop("Effective array parameter is incompatible with formal parameter"),
    /* 305 E_MACHINE_MUST_INCLUDE_USED_MACHINE (%s M1) (%s M2) (%s M3) (%s M1) (%s M3)*/
    gettext_noop("\"%s\" INCLUDES \"%s\" which USES \"%s\", therefore \"%s\" must include \"%s\""),
    /* 306 E_ILLEGAL_CALL_OF_INCLUDED_OPERATIONS_IN_PARALLEL_SUBST %s used_machine name*/
    gettext_noop("Two operations from the same included machine \"%s\" can not be called in two parallel substitutions"),
    /* 307 E_BEFORE_ONLY_IF_VAR_OF_WHILE_GLUED_OR_FROM_ABSTRACTION %s var name */
    gettext_noop("$0 can not be used since %s is neither a glued variable, nor a concrete variable refining a variable from the abstraction"),
    /* 308 E_EOF_FOUND_WHILE_PARSING_DEF_FILE %s def file name */
    gettext_noop("End of file found while parsing definition file %s"),
    /* 309 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a tree operand (given operand type is %s)"),
    /* 310 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_TREE %s op name %s oper type*/
    gettext_noop("Builtin operator %s requires a sequence of tree operand (given operand type is %s)"),
    /* 311 E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE %s ntype %s ttype */
    gettext_noop("Can not add a node of type %s to a tree of type %s"),
    /* 312 E_OPERAND_OF_BUILTIN_OP_IS_NOT_SEQ_OF_INT %s op name %s oper type */
    gettext_noop("Builtin operator %s requires a sequence of integers (given operand type is %s)"),
    /* 313 E_BUILTIN_OP_NEEDS_THREE_OPERANDS %s op name %d nb oper */
    gettext_noop("Builtin operator %s needs three operands (%d operands given)"),
    /* 314 E_BUILTIN_OP_NEEDS_ONE_OR_THREE_OPERANDS  %s op name %d nb oper */
    gettext_noop("Builtin operator %s needs one or three operands (%d operands given)"),
    /* 315 E_B0_DATA_IS_TYPED_WITH_WRONG_IDENT_TYPE   %s ident_type_name  , %s name , %s ident_type_name, %s name  */
    gettext_noop("%s \"%s\" is not implementable (typed with %s \"%s\")"),
    /* 316 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_SEQ %s op_name %s oper_type */
    gettext_noop("Builtin operator %s requires a sequence of sequence operand (given operand type is %s)"),
    /* 317 E_INVALID_IMAGE_INDEX_TYPE %s type_name %s expect_type*/
    gettext_noop("Invalid Image index type %s is not a subset of %s"),
    /* 318 E_IDENT_CLASHES_WITH_KERNEL_BUILTIN %s ident_name */
    gettext_noop("Identifier %s can not be used since it clashes with a builtin identifier of Atelier B's Logic Solver"),
    /* 319  E_KERNEL_HIDDEN_CLASH %s ident_name %s other_name %s other_component %s*/
    gettext_noop("Identifier %s and %s from component %s clash according to Atelier B's Logic Solver and Prover rules"),
    /* 320  E_CAN_NOT_TYPE_WITH_A_GENERIC_TYPE %s ident_name %s type_name */
    gettext_noop("Untyped identifier %s can not be typed with generic type %s"),
    /* 321 E_LOCAL_OPERATIONS_FORBIDDEN_IN_SPEC_OR_REF */
    gettext_noop("LOCAL_OPERATIONS clause is forbidden in a specification or in a refinement"),
    /* 322 E_LOCAL_OPERATION_IS_NOT_IMPLEMENTED %s local_op name %s compo name*/
    gettext_noop("Local operation %s is not implemented in component %s"),
    /* 323 E_LITERAL_INTEGER_GREATER_THAN_MAXINT */
    gettext_noop("Literal value %s%s is greater than MAXINT=%s%s"),
    /* 324 E_LITERAL_INTEGER_LESS_THAN_MININT */
    gettext_noop("Literal value %s%s is lower than MININT=%s%s"),
    /* 325 E_MININT_IS_GREATER_THAN_MAXINT */
    gettext_noop("MININT=%s is greater than MAXINT=%s"),
    /* 326 E_B0_TOOL_RESTRICTS_INSTANCIATION_TO_LITERALS  %s tool name */
    gettext_noop("Tool %s restricts B0 instanciation to literals (integer or boolean)"),
    /* 327 E_MS_LEXICAL_STATE_FILE */
    gettext_noop("Lexem %s is not accepted in state file %s"),
    /* 328 E_MS_STATE_FILE_LEXEM_WITH_NO_VALUE */
    gettext_noop("No value associated to the lexem %d in state file %s"),
    /* 329 E_MS_SYNTAXE_STATE_FILE */
    gettext_noop("Syntax error in state file %s line %d : lexem of type %d instead of type %d"),
    /* 330 E_MS_ACTION_TODO_INEXISTANT */
    gettext_noop("Action to do of index [%d,%d] doesn't exist in the tabular of the actions to do"),
    /* 331 E_MS_ACTION_TOUPDATE_INEXISTANT */
    gettext_noop("Action to update of index [%d,%d] doesn't exist in the tabular of the actions to update"),
    /* 332 E_MS_UPDATE_ACTION_WITH_NULL_SIGNATURE */
    gettext_noop("Update of action state %x with a NULL signature"),
    /* 333 E_MS_TYPE_OF_ACTION_INEXISTANT */
    gettext_noop("Type of action %d doesn't exist"),
    /* 334 E_MS_STATE_OF_ACTION_HAS_EVER_BEEN_MEMORIZED */
    gettext_noop("State of action %d has ever been memorized in %x"),
    /* 335  E_MS_CAN_NOT_OPEN_STATE_FILE */
    gettext_noop("Can't open state file %s"),
    /* 336 E_CYCLE_IN_IMPLEMENTATION_OF_LOCAL_OP %s local_op_name */
    gettext_noop("Illegal cycle in the operation call graph of the implementation of local operation %s"),
    /* 337 E_LOCAL_OP_IS_PART_OF_THE_CYCLE %s local_op_name */
    gettext_noop("Local operation %s is part of the cycle"),
    /* 338 E_MULTIPLE_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP %s machine name %s oploc name %s machine_name */
    gettext_noop("Illegal call to an operation of %s : in the specification of local operation %s, there is a simultaneous call to another operation of %s"),
    /* 339 E_PREVIOUS_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP %s macihne_name %s oploc name*/
    gettext_noop("Location of simultaneous call of an operation of %s in the specification of local operation %s"),
    /* 340   E_INVALID_OUTPUT_PARAM_CONC_VAR_IS_MODIFIED_IN_LOCAL_OP_SPEC %s op name %s var name %s var name %s op_name */
    gettext_noop("Illegal call to local operation %s with output parameter %s : %s is a concrete variable modified in the body of the specification of %s"),
    /* 341 E_LOC_OF_MODIF_OF_CONC_VAR_IN_LOCAL_OP_SPEC %s var_name %s op_name */
    gettext_noop("Location of the modification of the concrete variable %s in the specification of %s"),
    /* 342   E_IN_OP_LOCAL_OP_CALL_INVALID_IMPORTED_OUTPUT_PARAM_IS_MODIFIED_IN_IMPORTED_OP %s op name %s var name %s var name %s op_name */
    gettext_noop("In local operation specification : illegal call to imported operation %s with output parameter %s : %s is an imported variable modified in the body of the specification of %s"),
    /* 343 E_LOC_OF_IMPORTED_VAR_VAR_IN_IMPORTED_OP %s var_name %s op_name */
    gettext_noop("Location of the modification of the imported variable %s in the specification of imported operation %s"),
    /* 344 E_TOO_MANY_PARAM_IN_REF_OP %s op_name */
    gettext_noop("Too many parameters in refined operation \"%s\""),
    /* 345 E_TOO_FEW_PARAM_IN_REF_OP %s op_name */
    gettext_noop("Too few parameters in refined operation \"%s\""),
    /* 346 E_PARAM_NAME_DIFF_1  %s arg_name_in_spec %s arg_name_in_ref */
    gettext_noop("Different parameter names (\"%s\" / \"%s\")"),
    /* 347 E_PARAM_NAME_DIFF_2  %s op_name */
    gettext_noop("between specification and refinement of operation \"%s\""),
    /* 348 E_DOUBLE_REF  %s ident_name  */
    gettext_noop("Identifier \"%s\" appears twice in left hand side"),
    /* 349 E_IDENT_INIT_VAR_LOCAL %s invalid_ident  %s expected_ident */
    gettext_noop("Invalid identifier \"%s\" ( \"%s\" expected )"),
    /* 350 E_B0_INVALID_TYPING_EXPR %s ident_name %s ident_type*/
    gettext_noop("Invalid typing expression for %s %s (identifier, 'BOOL', set or 'STRING' expected)"),
    /* 351 E_B0_ABST_MACH_AND_CONC_INCL %s machine name */
    gettext_noop("Component %s is abstract and has a concrete included component"),
    /* 352 E_B0_INTERVAL_TYPE_ONLY_IN_BASIC_TRANS_MACH !inutilisé! */
    gettext_noop("Interval type is allowed only in basic translator machine (basic_translator_machine pragma)"),
    /* 353 E_B0_INVALID_RANGE_FOR_ARRAY_TYPE %s type name !inutilisé! */
    gettext_noop("Invalid range for array type %s (interval, 'BOOL' or enumerated set expected)"),
    /* 354 E_B0_ARRAY_MINIMAL_BOUND !inutilisé! */
    gettext_noop("Invalid interval minimal bound (literal integer 0 expected)"),
    /* 355 E_B0_ARRAY_MAX_BOUND !inutilisé! */
    gettext_noop("Invalid interval maximal bound (static expression expected)"),
    /* 356 E_B0_INVALID_ARRAY_TYPE !inutilisé!  */
    gettext_noop("Invalid array type" ),
    /* 357 E_LOCATION_OF_VALUATION !inutilisé! */
    gettext_noop("Location of valuation"),
    /* 358 E_B0_OPERATION_MUST_BE_ABSTRACT %s operation name */
    gettext_noop("Operation %s must be declared abstract"),
    /* 359 E_B0_INDICES_ORDERED */
    gettext_noop("The indices of an array must be ordered in asceding order"),
    /* 360 E_B0_INVALID_VALUATION_OF_ABSTRACT_SET %s abs set name */
    gettext_noop("Invalid valuation of abstract set %s (abstract set or a constant interval of integers expected)"),
    /* 361 E_INVALID_OP_REFINEMENT %s op_name */
    gettext_noop("Invalid operation refinement, %s has not the same signature in specification"),
    /* 362 E_B0_OP_MUST_CONCRETE_IN_IMPL %s op_name*/
    gettext_noop("Operation %s must be concrete in implementation"),
    /* 363 E_B0_INVALID_EXPR */
    gettext_noop("Invalid expression"),
    /* 364 E_B0_NO_LITERAL_INTEGER_TYPE %s machine_name*/
    gettext_noop("No literal integer type given for machine %s (literal_integer_type pragma)"),
    /* 365 E_B0_TYPE_DATA_EXPECTED */
    gettext_noop("Concrete constant, set or BOOL expected"),
    /* 366 E_B0_TYPE_INPUT_PARAM_EXPECTED */
    gettext_noop("Concrete constant, set, BOOL or STRING expected"),
    /* 367 E_B0_LOC_VAR_MUST_INITIALISED %s var_loc_name */
    gettext_noop("Local variable %s must be initialised"),
    /* 368 E_B0_INVALID_TYPING_EXPR_FOR_LOCAL_VARIABLE %s class_name, %s name  */
    gettext_noop("Invalid typing expression %s for local variable %s"),
    /* 369 E_B0_INVALID_TYPING_EXPR_FOR_OP_OUT_PARAM */
    gettext_noop("Invalid typing expression %s for output param %s"),
    /* 370 E_B0_INVALID_TYPE_DECL  %s ident_name*/
    gettext_noop("Invalid declaration for type %s "),
    /* 371 E_B0_PARAM_FORBI_BOM */
    gettext_noop("Machine parameter is forbidden"),
    /* 372 E_B0_OPERATION_MUST_BE_CONCRETE %s operation name*/
    gettext_noop("Operation %s must be declared concrete"),
    /* 373 E_B0_INVALID_TYPE_VALUATION type_name*/
    gettext_noop("%s valuation and declaration must be exactly the same formula"),
    /* 374 E_B0_INCOMPATIBLE_DECLARATIONS */
    gettext_noop("Declaration of %s is incompatible with previous definition"),
    /* 375 E_B0_LOCATION_OF_FIRST_DECLARATION */
    gettext_noop("Location of previous declaration"),
    /* 376 E_B0_AGREGATE_ONLY_IN_VAR_VALUES */
    gettext_noop("Array agreggate expressions are allowed only for initialised a constant or a local variable"),
    /* 377 E_B0_INCOMPATIBLE_TYPE_FOR_PARAMS %s effective_type %s formal_type %sformal_parameter_name*/
    gettext_noop("Effective parameter type %s is incompatible with formal parameter type %s ( formal parameter %s)"),
    /* 378 E_B0_IDENT_CANT_BE_REFERED %s ident_name */
    gettext_noop("%s can not be refered in effective parameter"),
    /* 379 E_B0_REDEFINITION_OF_INLINE */
    gettext_noop("Illegal redefinition of INLINE pragma"),
    /* 380 E_B0_REDEFINITION_OF_INLINE_ALL */
    gettext_noop("Illegal redefinition of INLINE_ALL pragma"),
    /* 381 E_B0_INLINE_AND_INLINE_ALL */
    gettext_noop("INLINE pragma can not be used with INLINE_ALL pragma"),
    /* 382 E_B0_IS_NOT_OP_FROM_IMPL %s op_name %s implemenation_name*/
    gettext_noop("%s operation is not an operation from implementation %s"),
    /* 383 E_B0_INLINE_ONLY_IN_IMPL_OR_MACH */
    gettext_noop("INLINE pragma and INLINE_ALL pragma are allowed only in implementation or specification"),
    /* 384 E_B0_INLINE_SHOULD_PARAM */
    gettext_noop("INLINE pragma should have parameter"),
    /* 385 E_REDEF_PRAG_LIT_INT_TYPE */
    gettext_noop("Illegal redefinition of literal_integer_type pragma"),
    /* 386 E_LIT_INT_TYPE_TAKES_ONE_PARAM */
    gettext_noop("literal_integer_type pragma takes one and only one parameter"),
    /* 387 E_B0_ARRAY_EXPR_FORBIDDEN */
    gettext_noop("Array expression forbidden"),
    /* 388 E_B0_INVALID_IDENT */
    gettext_noop("Invalid identifier name %s"),
    /* 389 E_B0_INVALID_TYPE_FOR_BECOMES_MEMBER %s type_name */
    gettext_noop("Type %s should be a complete type"),
    /* 390 E_B0_INVALID_BECOMES_MEMBER */
    gettext_noop("Invalid substitution '::' (expected 'identifier :: type')"),
    /* 391 E_B0_IS_NOT_OP_FROM_MCH %s op_name %s spec_name*/
    gettext_noop("Operation %s is not an operation from the specification %s"),
    /* 392 E_B0_SET_VALUED_WITH_CONSTANTS_FROM_SAME_MODULE %s set_nam %s constant_name*/
    gettext_noop("Set %s can not be valued with constant %s from the same module"),
    /* 393 E_B0_OPERATION_ONLY_ONE_OUT_PARAM %s operation_name */
    gettext_noop("Operation %s should have only one out parameter"),
    /* 394 E_B0_EXPR_STATIC */
    gettext_noop("Expression must be static"),
    /* 395 E_B0_OM_INVALID_RANGE_FOR_ARRAY_AGGREGATE */
    gettext_noop("Invalid index set for array aggregate"),
    /* 396 E_B0_INVALID_TYPE_FOR_PARAM %s new_type_name , %s parameter_name, operation_name previous_type_name */
    gettext_noop("The type %s for paramter %s of operation %s is incompatible with previous type %s"),
    /* 397 E_B0_LIT_PRAGMA_PARAM_CONCRETE_CONSTANT */
    gettext_noop("The parameter of the literal_integer_type pragma must be a concrete constant"),
    /* 398 E_B0_LIT_PRAGMA_PARAM_INTEGER_TYPE */
    gettext_noop("The parameter of the literal_integer_type pragma must denote an integer type"),
    /* 399 E_HEXADECIMAL_LITERAL_OVERFLOW */
    gettext_noop("Hexadecimal number too large"),
    /* 400 E_OPERAND_SHOULD_BE_A_REAL %s type_name */
    gettext_noop("Operand is not a real (its type is %s)"),
    /* 401 E_OPERAND_SHOULD_BE_A_FLOAT %s type_name*/
    gettext_noop("Operand is not a float (its type is %s)"),
    /* 402 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_FLOAT %s type_name*/
    gettext_noop("First operand is not a float (its type is %s)"),
    /* 403 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_FLOAT */
     gettext_noop("Second operand is not a float (its type is %s)"),
    /* 404 E_FIRST_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL %s type_name */
    gettext_noop("First operand is not a real (its type is %s)"),
    /* 405 E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_OR_REAL_SET %s op name %s oper type */
    gettext_noop("Builtin operator %s requires an integer or real set operand (given operand type is %s)"),
    /* 406 E_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL %s type_name */
    gettext_noop("Operand is not an integer or a real (its type is %s)"),
    /* 407 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_REAL %s type */
   gettext_noop("First operand is not a real (its type is %s)"),
    /* 408 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_REAL %s type */
   gettext_noop("Second operand is not a real (its type is %s)"),
   /* 409 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_REAL_SET %s op name %s oper type */
   gettext_noop("Builtin operator %s requires a real set operand (given operand type is %s)"),
   /* E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_REAL, %s op name %s oper type */
   gettext_noop("Builtin operator %s requires a real operand (given operand type is %s)"),
   /* E_OPERAND_SHOULD_BE_AN_INTEGER_A_REAL_OR_A_FLOAT %s type */
   gettext_noop("Operand is not an integer, a real nor a float (its type is %s)"),
    /* E_INVALID_EVENT_REF %s event */
    gettext_noop("Unknown refined event %s"),
    /* E_DOUBLE_EVENT_REF %s event */
    gettext_noop("Refined event %s is duplicated"),
    /* E_REFINED_EVENT_CANNOT_REF %s event */
    gettext_noop("Event %s already refines itself. It cannot refine another event"),
    /* E_REFINE_AND_MERGE_EVENT %s event */
    gettext_noop("%s refines multiple events and almost one of them (%s) is refined by multiple events"),
    /* E_EVENTB_INVALID_HEAD_SUBSTITUTION */
    gettext_noop("Event should start with skip, assignement, BEGIN, SELECT or ANY subtitution"),
    /* E_EVENTB_AMBIGUOUS_WITNESS */
    gettext_noop("Ambiguous identifier in WITNESS clause"),
    /* E_VARIANT_CLAUSE_MANDATORY */
    gettext_noop("VARIANT clause is mandatory when new events are defined"),
    /* E_PRAGMA_IN_DEF */
    gettext_noop("Pragma in definitions are not handled by compiler"),
    /* E_BINARY_LITERAL_OVERFLOW */
    gettext_noop("Binary number too large"),
    /* E_TOTAL_RELATIONS_NOT_ALLOWED */
    gettext_noop("Total and surjective relation operators are not allowed"),
} ;

//static const char *const french_error_table[] =
//{
//    /* 00 E_PARSE ERROR */
//    "Erreur d'analyse lexicale : caractère illegal '%c'",
//    /* 01 E_NO_SUCH_FILE */
//    "Impossible d'ouvrir le fichier \"%s\" en lecture (%s)",
//    /* 02 E_EOF_INSIDE_STRING */
//    "Fin de fichier détectée dans une chaine",
//    /* 03 E_START_OF_STRING */
//    "Début de la chaine",
//    /* 04 E_NEW_LINE_INSIDE_STRING */
//    "Retour chariot détecté dans une chaine",
//    /* 05 E_TOKEN_PARSE_ERROR */
//    "Erreur de parsing : lexeme illegal %s",
//    /* 06 E_NO_INPUT_NAME_GIVEN */
//    "Nom de fichier a analyser manquant (usage : bcomp -V)",
//    /* 07 E_INVALID_COMPONENT_TYPE */
//    "Type de composant %s invalide (MACHINE, REFINEMENT ou IMPLEMENTATION attendu)",
//    /* 08 E_BAD_COMPONENT_NAME */
//    "Le nom du fichier \"%s\" ne correspond pas au nom du composant \"%s\"",
//    /* 09 E_COMPONENT_WITHOUT_END %s (component type) */
//    "Le mot clé %S du composant n'a pas de \"END\" associé",
//    /* 10 E_MACH_INVALID_CLAUSE %s (lex name) */
//    "Mot clé invalide : '%s' n'est pas une clause valide de machine",
//    /* 11 E_OP_NAME_OR_PARAM_EXPECTED %s (lex name) */
//    "Nom d'opération ou de paramètre d'entrée attendu, %s obtenu",
//    /* 12 E_UNEXPECTED_EOF */
//    "Fin de fichier prématurée",
//    /* 13 E_EMPTY_FILE */
//    "Le fichier source est vide",
//    /* 14 E_CONFUSED */
//    "Il y a eu %d erreur(s). La compilation s'arrête.",
//    /* 15 E_BAD_COMPONENT_SUFFIX, %s (machine type) %s (suffix) */
//    "Le type du composant (%s) ne correspond pas au suffixe du fichier (%s)",
//    /* 16 E_UNUSED_MESSAGE_16 */
//    "",
//    /* 17 E_MACRO_DEFINITION %s (macro name) %d (macro args) */
//    "%s est une définition à %d paramètre(s) définie ici",
//    /* 18 E_MACRO_INVALID_ARGS %d (macro args) %s (macro name) */
//    "Nomde de paramètre invalide (%d) pour l'utilisation de la définition %s",
//    /* 19 E_ERROR_PARSING_ATOMIC_SUBST %s (ident name) */
//    "Erreur lors de l'analyse syntaxique de la substitution atomique commençant par l'identificateur \"%s\"",
//    /* 20 E_UNUSED_MESSAGE_20 */
//    "",
//    /* 21 E_UNUSED_MESSAGE_21 */
//    "",
//    /* 22 E_REFINES_FORBIDDEN_IN_SPEC */
//    "La clause REFINES est interdite dans une spécification",
//    /* 23 E_INCLUDES_FORBIDDEN_IN_IMP */
//    "La clause INCLUDES est interdite dans une implémentation",
//    /* 24 E_IMPORTS_FORBIDDEN_IN_SPEC */
//    "La clause IMPORTS est interdit dans une spécification",
//    /* 25 E_IMPORTS_FORBIDDEN_IN_REF */
//    "La clause IMPORTS est interdit dans un raffinement",
//    /* 26 E_NO_OUTPUT_NAME_GIVEN */
//    "Vous n'avez pas donné de nom de fichier à produire (options : bcomp -V)",
//    /* 27 E_BAD_MAGIC */
//    "Mauvais magic number (attendu 0x%08x, obtenu 0x%08x).\n\"%s\" is n'est pas un Betree binaire",
//    /* 28 E_CYCLE_IN_DEFINITIONS */
//    "Dépendances circulaires dans la clause DEFINITIONS",
//    /* 29 E_OUT_OF_MEMORY %d nb bytes */
//    "Mémoire virtuelle épuisée lors de la tentative d'allocation de %d octets de mémoire",
//    /* 30 E_IO_ERROR (%s errno) */
//    "Erreur d'entrée/sortie fatale (\"%s\")",
//    /* 31 E_PART_OF_DEF_CYCLE %s (def name) */
//    "La définition \"%s\" fait partie du cycle",
//    /* 32 E_NAME_CLASH_IN_DEF_FPARAMS %s (def name) %s (param name) */
//    "Dans la definition %s : redéfinition du paramètre formel %s",
//    /* 33 E_PROBLEM_READING_FILE %s (file name) %s (errno) */
//    "Erreur lors de la lecture du fichier \"%s\" : %s",
//    /* 34 E_PROBLEM_WRITING_FILE %s (file name) %s (errno) */
//    "Erreur lors de la création du fichier \"%s\" : %s",
//    /* 35 E_EITHER_EXPECTED */
//    "Erreur : Le mot-clé OR est ne peut pas être la première alternative du choix (il faut utiliser EITHER)",
//    /* 36 E_OR_EXPECTED */
//    "Erreur : Le mot-clé EITHER ne peut être que la première alternative du choix (il faut utiliser OR)",
//    /* 37 E_PROBLEM_CLOSING_FILE %s (file name) %s (errno) */
//    "Erreur lors de la fermeture du fichier \"%s\" : %s",
//    /* 38 E_FILE_NAME_HAS_NO_SUFFIX %s (file name) */
//    "Le nom de fichier \"%s\" ne comporte pas de suffixe",
//    /* 39 E_CLO_DOES_NOT_MATCH_OPN %s (clo name) */
//    "Le lexème fermant \"%s\" ne correspond pas au lexème ouvrant associé",
//    /* 40 E_OPN_LOCALISATION %s (opn name) */
//    "(Localisation du lexème ouvrant associé \"%s\")",
//    /* 41 E_UNEXPECTED_BINOP (binop name) */
//    "L'opérateur binaire %s ne peut pas être utilisé ici",
//    /* 42 E_NOT_AN_EXPRESSION %s (formula type) */
//    "La formule n'est pas une expression (c'est un(e) %s)",
//    /* 43 E_MISALIGNED_ADDRESS (%d (address) */
//    "L'adresse 0x%x n'est pas alignée correctement",
//    /* 44 E_BAD_DOT_CONSTRUCT */
//    "Mauvaise construction syntaxique avec '.'. Les possibilités valides sonts~: une Lambda-expression, un prédicat universel ou existentiel, , SIGMA, PI, UNION ou INTER",
//    /* 45 E_BAD_DOT_EXPLAIN */
//    "(Remarque : les quantificateurs doivent être déclarés sous la forme '(xx,yy..)' et pas sous la forme 'xx,yy..')",
//    /* 46 E_BPRED_PRED1_NOT_A_PRED %s*/
//    "Le côté gauche du prédicat binaire n'est pas un prédicat : c'est un(e) %s",
//    /* 47 E_BPRED_PRED2_NOT_A_PRED %s */
//    "Le côté droit du prédicat binaire n'est pas un prédicat : c'est un(e) %s",
//    /* 48 E_VAR_FORBIDDEN_IN_SPEC */
//    "La substitution VAR .. IN .. END n'est pas autorisée dans une spécification",
//    /* 49 E_WHILE_FORBIDDEN_IN_SPEC */
//    "La substitution WHILE n'est pas autorisée dans une spécification",
//    /* 50 E_LET_FORBIDDEN_IN_IMP */
//    "La substitution LET n'est pas autorisée dans une implémentation",
//    /* 51 E_SELECT_FORBIDDEN_IN_IMP */
//    "La substitution SELECT n'est pas autorisée dans une implémentation",
//    /* 52 E_ANY_FORBIDDEN_IN_IMP */
//    "La substitution ANY n'est pas autorisée dans une implémentation",
//    /* 53 E_CHOICE_FORBIDDEN_IN_IMP */
//    "La substitution CHOICE n'est pas autorisée dans une implémentation",
//    /* 54 E_PRE_FORBIDDEN_IN_IMP */
//    "La substitution PRE..THEN..END n'est pas autorisée dans une implémentation",
//    /* 55 E_SEQ_FORBIDDEN_IN_SPEC */
//    "La substitution séquentielle (';') n'est pas autorisée dans une spécification",
//    /* 56 E_WHILE_FORBIDDEN_IN_REF */
//    "La substitution WHILE n'est pas autorisée dans un raffinement",
//    /* 57 E_LR_STATE_DOES_NOT_HAVE_A_SUCCESSOR, %d lr_state */
//    "L'état %d de l'automate LR n'a pas de successeur",
//    /* 58 E_LEXEM_WITHOUT_ASSOC */
//    "Le lexème n'a pas d'associativité",
//    /* 59 E_ATOM_TYPE_NOT_YET_IMPLEMENTED %s atom type %s atom ascii */
//    "Les constructions atomiques de type %s (%s) ne sont pas implémentés dans cette version",
//    /* 60 E_BINOP_TYPE_NOT_YET_IMPLEMENTED %s binop type %s binop ascii */
//    "Les constructions binaires de type %s (%s) ne sont pas implémentés dans cette version",
//    /* 61 E_UNOP_TYPE_NOT_YET_IMPLEMENTED %s unop ascii */
//    "Les opérateurs unaires de type %s ne sont pas implémentés dans cette version",
//    /* 62 E_PARENTH_COMPULSORY_IN_FUNCTION_CALL, %s op name */
//    "L'utilisation de '()' est obligatoire lors de l'appel d'une fonction",
//    /* 63 E_WHILE_PARSING_OPERATION_HEADER */
//    "Lors de l'analyse syntaxique de l'entête de l'opération",
//    /* 64 E_CL1_IMPLIES_CL2 */
//    "Le composant a une clause %s mais pas de clause %s",
//    /* 65 E_IDENT_CLASH */
//    "La définition de l'identificateur \"%s\" en tant que %s est en conflit avec sa définition précédente en tant que %s",
//    /* 66 E_IDENT_CLASH_OTHER_LOCATION */
//    "Localisation de la définition en conflit\n",
//    /* 67 E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP */
//    "La clause ABSTRACT_CONSTANTS n'est pas autorisée dans une implémentation",
//    /* 68 E_ABSTRACT_CONSTANTS_FORBIDDEN_IN_IMP */
//    "La clause ABSTRACT_VARIABLES n'est pas autorisée dans une implémentation",
//    /* 69 E_CONSTRAINTS_FORBIDDEN_IN_REF */
//    "La clause CONSTRAINTS n'est pas autorisée dans un raffinement",
//    /* 70 E_CONSTRAINTS_FORBIDDEN_IN_IMP */
//    "La clause CONSTRAINTS n'est pas autorisée dans une implémentation",
//    /* 71 E_COMPONENT_HAS_NO_REFINES_CLAUSE */
//    "Le composant n'a pas de clause REFINES",
//    /* 72 E_USES_FORBIDDEN_IN_REF */
//    "La clause USES n'est pas autorisée dans un raffinement",
//    /* 73 E_USES_FORBIDDEN_IN_IMP */
//    "La clause USES n'est pas autorisée dans une implémentation",
//    /* 74 E_VALUES_FORBIDDEN_IN_REF */
//    "La clause VALUES n'est pas autorisée dans un raffinement",
//    /* 75 E_VALUES_FORBIDDEN_IN_SPEC */
//    "La clause VALUES n'est pas autorisée dans une implémentation",
//    /* 76 E_REQUIRED_COMPONENT_MISSING %s component name */
//    "Le composant requis %s n'a pas pu être trouvé",
//    /* 77 E_REF_DOES_NOT_DUPLICATE_SEES %s %s seen name, %s abstract name */
//    "La clause SEES %s correspondant au composant %s vu par l'abstraction %s n'est pas présente",
//    /* 78 E_SEES_LOCATION %s seen name */
//    "Localisation de la clause SEES %s clause dans l'abstraction",
//    /* 79 E_REQ_COMP_NOT_A_SPEC */
//    "Le composant requis n'est pas une spécification",
//    /* 80 E_REF_COMP_NOT_A_SPEC */
//    "Le composant raffiné n'est pas une spécification",
//    /* 81 E_REF_COMP_NEITHER_SPEC_NOR_REF */
//    "Le composant raffiné n'est ni une spécification, ni un raffinement",
//    /* 82 E_PARAM_N_IS_NOT_REFINED %d param number, %s param name */
//    "Le paramètre formel #%d de l'abstraction (\"%s\") a été omis",
//    /* 83 E_PARAM_N_IS_NOT_SPECIFIED %d param number, %s param name */
//    "Le paramètre formel #%d (\"%s\") n'est pas un paramètre formel de l'abstraction",
//    /* 84 E_LOCATION_OF_ABSTRACT_PARAMS */
//    "Localisation des paramètres formels de l'abstraction",
//    /* 85 E_LOCATION_OF_PARAM_N_IN_ABSTRACTION param number */
//    "Définition du paramètre formel #%d de l'abstraction",
//    /* 86 E_PARAM_N_DIFFERS %d par number, %s par name %d par number, %s par name */
//    "Le paramètre formel #%d (\"%s\") diffère du paramètre formel #%d (\"%s\") de l'abstraction",
//    /* 87 E_CAN_NOT_IMPORT_A_MACHINE_THAT_USES machine name */
//    "La machine %s ne peut pas être importée car elle contient une clause USES",
//    /* 88 E_CAN_NOT_EXTEND_A_MACHINE_THAT_USES machine name */
//    "La machine %s ne peut pas être étendue car elle contient une clause USES",
//    /* 89 E_CAN_NOT_SEE_A_MACHINE_THAT_USES machine name */
//    "La machine %s ne peut pas être vue car elle contient une clause USES",
//    /* 89 E_CAN_NOT_REFINE_A_MACHINE_THAT_USES machine name */
//    "La machine %s ne peut pas être raffinée car elle contient une clause USES",
//    /* 91 E_OPERATION_IS_NOT_SPECIFIED %s op name %s spec name */
//    "L'opération %s n'est pas une opération de l'abstraction %s",
//    /* 92 E_OPERATION_IS_NOT_REFINED %s op name %s comp name */
//    "L'opération %s is n'est pas raffinée dans le composant %s",
//    /* 93 E_PARALLEL_FORBIDDEN_IN_SPEC */
//    "La substitution parallèle ('||') n'est pas autorisée dans une implémentation",
//    /* 94 E_BECOMES_SUCH_THAT_FORBIDDEN_IN_IMP */
//    "La substitution devient tel que (':') n'est pas autorisée dans une implémentation",
//    /* 95 E_BECOMES_MEMBER_OF_FORBIDDEN_IN_IMP */
//    "La substitution devient membre de ('::') n'est pas autorisée dans une implémentation",
//    /* 96 E_SPEC_SHOULD_HAVE_A_CONSTRAINTS_CL */
//    "La spécification a des paramètres formels scalaires et pas de clause CONSTRAINTS",
//    /* 97 E_SPEC_SHOULD_NOT_HAVE_A_CONSTRAINTS_CL */
//    "La clause CONSTRAINTS n'est pas autorisés dans une spécification sans paramètre formel scalaire",
//    /* 98 E_BEFORE_ONLY_IN_WHILE_AND_BEC_SUCH_THAT */
//    "L'expression '$0' n'est autorisée que dans les substitution WHILE and et 'devient tel que'",
//    /* 99 E_COMPONENT_CAN_NOT_REFINE_ITSELF */
//    "Le composant ne peut pas se raffiner lui-même",
//    /* 100 E_COMPONENT_MULTIPLY_REFERENCED %s component name */
//    "Le composant %s est référencé plusieurs fois",
//    /* 101 E_MULTIPLE_IMPORTATION_OF_COMPONENT component name */
//    "Le composant %s est importé plusieurs fois",
//    /* 102 E_LOCATION_OF_COMPONENT_IMPORT component name */
//    "Le composant %s est importé ici",
//    /* 103  E_UNDECLARED_IDENT %s ident name */
//    "L'identificateur \"%s\" est inconnu",
//    /* 104 E_NO_TYPE_FOR_IDENT %s ident type %s ident name */
//    "%s %s n'a pas été typé",
//    /* 105 E_LOCATION_OF_OP_SPEC */
//    "Localisation de la spéficiation de l'opération",
//    /* 106 E_ILLEGAL_TYPE_FOR_IDENT %s type %s ident name */
//    "Type illégal %s pour %s %s",
//    /* 107 E_LOCATION_OF_IDENT_TYPE,  ident name */
//    "Localisation de la définition du type %s pour %s %s",
//    /* 108 E_UNEXPECTED_ARGUMENT_IN_EQPRED */
//    "Présence illégale d'une expression dans un prédicat d'égalité (qui devrait être du type Exp = Exp ou du type Exp /= Exp)",
//    /* 109 RHS_OF_BELONGS_MUST_BE_A_SET %s type */
//    "La partie droite du prédicat  ':' devrait être un ensebmle (c'est un(e))",
//    /* 110 E_UNBALANCED_AFFECTATION % d nb_ident %d nb_values */
//    "Affectation non symétrique : il y a %d identificateurs dans la partie gauche, mais %d expressions dans la partie droite",
//    /* 111 E_WRONG_NUMBER_OF_TYPES_GIVEN %s type name %d nb ident */
//    "Le type donné %s ne type pas les %d identificateurs de la partie gauche",
//    /* 112 E_OPERATION_PARAM_NOT_SPECIFIED %s ident_type %s ident_type */
//    "%s %s n'est pas spécifié",
//    /* 113 E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM */
//    "Le type STRING ne peut être utilisé que pour typer des paramètres d'entrée d'opérations, avec le prédicat d'appartenance",
//    /* 114 E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM2 %s id name %s id type */
//    "Le type STRING ne peut être utilisé que pour typer des paramètres d'entrée d'opérations (%s est un(e) %s)",
//    /* 115 E_NON_VISIBLE_ENTITY %s name %s rights */
//    "Accés illégal à l'entité \"%s\" qui n'est pas visible (les droits sont \"%s\" pour %s \"%s\" déclaré dans \"%s\", %s, depuis %s de \"%s\")",
//    /* 116 E_NON_VISIBLE_ENTITY_DECLARATION %s name */
//    "Localisation de la déclaration de l'entité non visible \"%s\"",
//    /* 117 E_INCOMPATIBLE_TYPES %s prev_type %s new_type */
//    "Le nouveau type %s n'est pas compatible avec le type précédent %s",
//    /* 118 E_WRONG_NUMBER_OF_EFFECTIVE_IN_PARAMS %s op %d eff %d formal */
//    "Lors de l'appel de l'opération %s : mauvais nombre de paramètres effectifs d'entrée (%d paramètres fournis, %d paramètres attendus)",
//    /* 119 E_WRONG_NUMBER_OF_EFFECTIVE_OUT_PARAMS %s op %d eff %d formal */
//    "Lors de l'appel de l'opération %s : mauvais nombre de paramètres effectifs de sortie (%d paramètres fournis, %d paramètres attendus)",
//    /* 120 E_LOCATION_OF_ITEM_DEF %s name */
//    "Localisation de la définition de  \"%s\"",
//    /* 121 E_CALLED_ITEM_IS_NOT_AN_OPERATION %s op_name */
//    "\"%s\" ne peut pas être appelée car ce n'est pas une opération",
//    /* 122 E_ACCESS_UNTYPED_IDENT %s ident name */
//    "L'identificateur \"%s\" ne peut pas être accédé car il n'a pas (encore) été typé(e)",
//    /* 123 RHS_OF_BECOMES_MEMBER_OF_THAT_MUST_BE_A_SET %s type */
//    "La partie droite de la substitution '::' doit être un ensemble (et pas un %s)",
//    /* 124 E_UNBALANCED_BECOMES_MEMBER_OF % d nb_ident %d nb_values */
//    "Substitution '::' non symétrique : il y a %d identificateurs en partie gauche, et %d ensembles en partie droite",
//    /* 125 E_ITEM_OF_CARTESIAN_PROD_IS_NOT_A_SET */
//    "L'élément du produit cartésien n'est pas un ensemble",
//    /* 126 E_UNCOMPATIBLE_EXTENSIVE_ITEM_TYPE %s cur_type %s ref_type */
//    "Dans l'ensemble en extension : le type %s de l'élément est incompatible avec le type des autres éléments de l'ensemble (%s)",
//    /* 127 E_INVALID_INTERVAL */
//    "Intervalle invalide",
//    /* 128 E_OPERAND_SHOULD_BE_AN_INTEGER %s type_name */
//    "L'opérande n'est pas un entier (son type est %s)",
//    /* 129 E_OPERAND_SHOULD_BE_AN_INTEGER_OR_A_SET %s type_name */
//    "L'opérande n'est ni un entier, ni un ensemble (son type est %s)",
//    /* 130 E_OPERAND_SHOULD_BE_A_SET %s type_name */
//    "L'opérande n'est pas un ensemble (son type est %s)",
//    /* 131 E_INVALID_OP_CALL %s op_name */
//    "Lors de l'appel de l'opération %s : utilisation illégale de ':=' (utiliser '<--' à la place)",
//    /* 132 E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND  %s op name %d nb_oper */
//    "L'opérateur prédéfini %s n'a qu'une opérande (%d opérandes fournies)",
//    /* 133 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type ensemble (type fourni: %s)",
//    /* 134 E_UNCOMPATIBLE_SET_OPERAND %s type %s ref_type */
//    "Le type de l'ensemlbe %s est incompatible avec le type de la première opérande %s",
//    /* 135 E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_SET %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type ensemble d'entiers (type fourni: %s)",
//    /* 136 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type séquence (type fourni: %s)",
//    /* 137 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type séquence non vide (type fourni: %s)",
//    /* 138 E_OPERAND_OF_SET_RELATION_IS_NOT_A_SET %s type name */
//    "L'opérande de la relation ensemblise '<->' doit être un ensemble (et pas %s)",
//    /* 139 E_OPERAND_OF_CONVERSE_IS_NOT_A_RELATION %s type name */
//    "L'opérande de '~' doit être une relation (et pas %s)",
//    /* 140 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type relation (type fourni: %s)",
//    /* 141 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une opérande de type relation de A vers A (type fourni: %s)",
//    /* 142 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une première opérande de type relation (type fourni: %s)",
//    /* 143 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une première opérande de type relation de A vers A (type fourni: %s)",
//    /* 144 E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER %s op name %s oper type */
//    "L'opérateur prédéfini %s nécessite une seconde opérande de type entier (type fourni: %s)",
//    /* 145 E_BUILTIN_OP_NEEDS_TWO_OPERANDS %s op name %d nb oper */
//    "L'opérateur prédéfini %s requiert deux opérandes (et pas %s opérandes)",
//    /* 146 E_OPERAND_OF_COMPOSITION_SHOULD_BE_A_RELATION %s oper type */
//    "L'opérande de l'opérateur de composition ';' doit être une relation (type fourni: %s)",
//    /* 147 E_OPERANDS_OF_COMPOSITION_SHOULD_HAVE_MATCHING_TYPES %s t1 %s t2 */
//    "Les types des opérandes de l'opérandes de composition ';' doivent être compatibles (les types %s et %s ne le sont pas)",
//    /* 148 E_LOCATION_OF_BOGUS_COMPOSITION_OPERAND */
//    "Localisation de l'opérateur de composition ';' incriminé",
//    /* 149 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER %s type */
//    "La première opérande du prédicat doit être un entier (type fourni: %s)",
//    /* 150 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER %s type */
//    "La deuxième opérande du prédicat doit être un entier (type fourni: %s)",
//    /* 151 E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_SET %s type */
//    "La première opérande du prédicat doit être un ensemble (type fourni: %s)",
//    /* 152 E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_SET %s type */
//    "La deuxième opérande du prédicat doit être un ensemble (type fourni: %s)",
//    /* 153 E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
//    "La première opérande de l'opérateur prédéfini doit être un ensemble (type fourni: %s)",
//    /* 154 E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET %s op name %s oper type */
//    "La deuxième opérande de l'opérateur prédéfini doit être un ensemble (type fourni: %s)",
//    /* 155 E_VARIANT_IS_NOT_BE_AN_INTEGER %s variant type */
//    "Le variant de la boucle WHILE doit être un entier (type fourni: %s)",
//    /* 156 E_IDENT_NOT_TYPED_IN_BECOMES_SUCH_THAT_PRED %s ident_name */
//    "L'identificateur %s n'est pas typé dans le prédicat de 'devient tel que'",
//    /* 157 E_IDENT_IS_NOT_AN_ARRAY %s type */
//    "L'identificateur %s n'est pas un tableau (type fourni: %s)",
//    /* 158 E_ITEM_IS_NOT_AN_ARRAY %s type */
//    "L'entité n'est pas un tableau (type fourni: %s)",
//    /* 159 E_BAD_INDEXES_FOR_ARRAY %s idx type %s array type */
//    "Le type %s du (des) index(es) est incompatible avec le type %s du tableau",
//    /* 160 E_UNABLE_TO_FIND_CONTEXT_FOR_IDENT %s ident_name %s ident_type */
//    "Impossible de trouver le contexte d'utilisation de l'identificateur \"%s\" (qui est un(e) %s)",
//    /* 161 E_EXPR_INCOMPATIBLE_TYPES %s prev_type %s new_type */
//    "Le type %s de la partie gauche est incompatible avec le type %s de la partie droite",
//    /* 162 E_VARIABLE_IS_ASSIGNED_IN_PARRALLEL %s variable */
//    "La variable %s est modifiée dans deux substitutions parallèles",
//    /* 163 E_LOCATION_OF_PREVIOUS_ASSIGN variable */
//    "Localisation de la modification précédente de la variable %s",
//    /* 164 E_LHS_OF_LET_VALUATION_MUST_BE_A_LOC_VAR %s variable */
//    "%s ne peut pas être valué(e) ici car ce n'est pas une variable locale de la substitution LET",
//    /* 165 E_MULTIPLE_TYPE_IN_LET_VALUATION %s variable */
//    "%s ne peut pas être valué(e) ici car elle a déjà été valuée dans cette substitution LET",
//    /* 166 E_INCOMPATIBLE_TYPES_IN_PRED %s lhs type %s rhs type */
//    "Le type %s de la partie gauche est incompatible avec le type %s de la partie droite",
//    /* 167 E_INCOMPATIBLE_TYPES_IN_EXPR %s type %s first_type */
//    "Le type %s est incompatible avec le premier type %s",
//    /* 168 E_LHS_OF_BINOP_SHOULD_BE_A_SEQ %s type */
//    "La partie gauche de l'opérateur binaire doit être une séquence (type fourni: %s)",
//    /* 169 E_RHS_OF_BINOP_SHOULD_BE_A_SEQ %s type */
//    "La partie droite de l'opérateur binaire doit être une séquence (type fourni: %s)",
//    /* 170 E_ILLEGAL_TYPE_FOR_ITEM_TO_ADD_IN_SEQ %s itype %s stype */
//    "Il est impossible d'ajouter un élément de type %s a une séquence de type %s",
//    /* 171 E_OPERAND_SHOULD_BE_A_SEQ %s type */
//    "L'opérande n'est pas une séquence (type fourni: %s)",
//    /* 172 E_UNKNOWN_OPERATION_CAN_NOT_BE_PROMOTED %s op_name */
//    "L'opération %s ne peut pas être promue car elle n'est pas visibile ou elle n'existe pas",
//    /* 173 E_OP_PARAM_NOT_REFINED, %s ident_type %s ident_type */
//    "%s %s n'a pas été raffiné",
//    /* 174 E_OPERAND_OF_IMAGE_SHOULD_BE_A_RELATION %s type */
//    "La première opérande de image doit être une relation (type fourni: %s)",
//    /* 175 E_SECOND_OPERAND_OF_IMAGE_SHOULD_BE_A_SUBSET_OF  %s set %s type */
//    "La deuxième opérande de image doit être un sous-type de %s (type fourni: %s)",
//    /* 176 E_UNABLE_TO_FIND_LINK_BETWEEN_COMPONENTS %s name1 %s name2 */
//    "Impossible de trouver le lien entre les composants %s et %s",
//    /* 177 E_INVALID_VALUATION_OF_ABSTRACT_SET %s abs set name */
//    "Valuation illégale de l'ensemble abstrait %s (type fourni: %s, qui n'est ni un ensemble abstrait, ni un intervalle fini et non vide d'entiers)",
//    /* 178 E_ANY_DATA_CAN_NOT_BE_MODIFIED %s name */
//    "La donnée abstraite %s de la clause ANY ne peut pas être modifiée",
//    /* 179 E_NO_CLOSE_PAREN_MATCHING_OPEN_PAREN */
//    "Pas de ')' correspondant au '(' délimitant la substitution précédente",
//    /* 180 E_OPEN_PAREN_LOCATION */
//    "Localisation de la '(' non terminée",
//    /* 181 E_VAR_OF_LAMBDA_EXPR_NOT_TYPED %s var name */
//    "La variable %s de la lambda expression n'est pas typée dans le prédicat de typage",
//    /* 182 E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER %s op name %s oper type */
//    "L'opérateur prédicat %s requiert une opérande entière (type fourni: %s)",
//    /* 183 E_FIRST_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_SET %s type */
//    "La première opérande de l'opérateur d'(anti)restriction doit être un ensemble (type fourni: %s)",
//    /* 184 E_SECOND_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_RELATION %s type */
//    "La seconde opérande de l'opérateur d'(anti)restriction doit être une relation (type fourni: %s)",
//    /* 185 E_FIRST_OPERAND_OF_CORESTRICT_IS_NOT_A_RELATION %s type */
//    "La première opérande de l'opérateur de corestriction doit être une relation (type fourni: %s)",
//    /* 186 E_SECOND_OPERAND_OF_CORESTRICT_IS_NOT_A_SET %s type */
//    "La première opérande de l'opérateur de corestriction doit être un ensemble (type fourni: %s)",
//    /* 187 E_PRAGMA_WITHOUT_OPEN_PAREN */
//    "Le pragma invalide \"%s\" ne comporte pas de  '(' : ses paramètres seront ignorés",
//    /* 188 E_EOF_INSIDE_PRAGMA */
//    "Le pragma invalide \"%s\" n'a pas de paramètre",
//    /* 189 E_PRAGMA_WITH_NO_NAME */
//    "Le pragma invalide \"%s\" n'a pas de nom : il sera ignoré",
//    /* 190 E_SECOND_OPERAND_OF_DPROD_IS_NOT_A_RELATION %s type */
//    "La seconde opérande de l'opérateur de produit direct doit être une relation (type fourni: %s",
//    /* 191 E_FIRST_OPERAND_OF_DPROD_IS_NOT_A_RELATION %s type */
//    "La première opérande de l'opérateur de produit direct doit être une relation (type fourni: %s",
//    /* 192 E_TRAILING_GARBAGE_AFTER_PRAGMA */
//    "Caractères parasites en fin de pragma",
//    /* 193 E_WRONG_NUMBER_OF_EFFECTIVE_MACHINE_PARAMS %d eff %d formal %s mach name*/
//    "Mauvais nombre de paramètres effectifs formels pour la machine %s (%d paramètre(s) fourni(s), %d paramètre(s) attendu(s))",
//    /* 194 E_INCOMPATIBLE_TYPES_FOR_MACHINE_PARAMS %s etype %s ftype %s pname*/
//    "Le type %s Type du paramètre effectif est incompatible avec le type %s du paramètre formel %s",
//    /* 195 E_OP1RELDST_INCOMPATIBLE_TYPES_OP2BASE %s lhs type name %s rhs type name */
//    "Le type cible %s de la relation en partie gauche est incompatible avec le type de base %s de la partie droite",
//    /* 196 E_IMAGE_EXPR_TYPES_GEQ_REL_TYPES %d nb_expr_types %d nb_rel_types */
//    "Dans [] : l'expression a %d types, ce qui est supérieur ou égal au nombre de types %d de la relation",
//    /* 197 E_OP2RELSRC_INCOMPATIBLE_TYPES_OP1BASE %s lhs type name %s rhs type name */
//    "Le type source %s de la relation en partie droite est incompatible avec le type de base %s de la partie gauche",
//    /* 198 E_IN_EXTENSIVE_REC_PAREN_EXPR_EXPECTED %s class_name */
//    "Record en extension : expression parenthésée attendue, %s fourni(e)",
//    /* 199 E_RECORD_LABEL_EXPECTED %s class name */
//    "Label de record attendu, %s fourni(e)",
//    /* 200 E_FIRST_OPERAND_OF_L_OVERLOAD_IS_NOT_A_RELATION %s type */
//    "La première opérande de la surcharche à gauche n'est pas une relation (type fourni : %s)",
//    /* 201 E_SECOND_OPERAND_OF_L_OVERLOAD_IS_NOT_A_SET %s type */
//    "La deuxième opérande de la surcharche à gauche n'est pas une relation (type fourni : %s)",
//    /* 202 E_INCOMPATIBLE_INDEX_TYPE %s idx type %d idx nb %s fct type */
//    "Le type %s de l'index numéro %d est incompatible avec le type attendu %s",
//    /* 203 E_LABEL_COMPULSORY */
//    "Un label de record doit obligatoirement être fourni ici",
//    /* 204 E_EOF_FOUND_WHILE_PARSING_DEFINITION %s def name */
//    "Fin de ficher ou de composant détectée lors de l'analyse de la définition %s",
//    /* 205 E_TYPE_SHOULD_BE_A_SET_OF_SET type name */
//    "Le type n'est pas un ensemble d'ensebmle i.e. Setof(Setof(t)) (type fourni: %s)",
//    /* 206 E_SECOND_OPERAND_OF_PPROD_IS_NOT_A_RELATION %s type */
//    "La seconde opérande de l'opérateur de produit parallèle doit être une relation (type fourni: %s",
//    /* 207 E_FIRST_OPERAND_OF_PPROD_IS_NOT_A_RELATION %s type */
//    "La première opérande de l'opérateur de produit parallèle doit être une relation (type fourni: %s",
//    /* 208 E_NO_TYPE_GIVEN_FOR_EFFECTIVE_PARAMETER */
//    "Le paramètre effectif n'est pas typé",
//    /* 209 E_CALLED_OP_NAME_SHOULD_BE_AN_IDENT %s class name */
//    "Le nom de l'opération appelée doit être un identificateur (le nom fourni est un(e) %s)",
//    /* 210 E_RHS_OF_VALUATION_HAS_NO_TYPE */
//    "La partie droite de la valuation n'a pas de type",
//    /* 211 E_RECORD_ITEM_WITHOUT_LABEL */
//    "Le label de ce champ de record ne peut pas être inféré depuis l'autre champ de record car ce dernier n'a pas de label non plus",
//    /* 212 E_LOCATION_OF_OTHER_JOKER */
//    "Localisation de l'autre champ de record sans label",
//    /* 213 E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_RECORD %s field %s type name */
//    "Impossible d'accéder au champ %s de cette expression car ce n'est pas un record (type fourni: %s)",
//    /* 214 E_CAN_NOT_ACCESS_FIELD_SINCE_NOT_PART_OF_RECORD %s field %s type name */
//    "Impossible d'accéder au champ %s de cette expression car ce n'est pas un label de ce type record (type fourni: %s)",
//    /* 215 E_CAN_NOT_TYPE_IDENT_WITH_JOKER_RECORD_TYPE %s type name */
//    "L'identificateur ne peut pas être typé avec ce type record car il contient des jokers (type fourni: %s)",
//    /* 216 E_FILE_DEF_NAME_WITH_PATH %s file_name */
//    "Le nom d'un fichier de définitions ne peut pas contenir de chemin d'accés. Utilisez \"fichier\" pour accéder à un fichier local, ou <fichier> pour accéder à un fichier dans une bibliothèque",
//    /* 217 E_UNABLE_TO_FIND_OBJECT_AT_RANK %d rank */
//    "Impossible de trouver l'objet au rang %d - Fichier corrompu",
//    /* 218 E_IDENT_INVALID_FOR_KERNEL %s (ident name) */
//    "L'identificateur \"%s\" a moins de 2 caractères : il sera rejetté par le Logic Solver de l'Atelier B",
//    /* 219 E_NO_COMPONENT_MATCH_IN_MULTI_COMPO_FILE %s compo_name %s file_name */
//    "Il devrait y avoir un composant nommé %s dans ce fichier (\"%s\")",
//    /* 220 E_COMPONENT_CAN_NOT_BE_IN_THIS_FILE %s compo name */
//    "Le composant %s ne peut pas être présent dans ce fichier",
//    /* 221 E_ONLY_ONE_COMPONENT_CAN_BE_STORED_IN_FILE %s compo name %s file_name */
//    "Le composant %s ne peut pas être décrit dans le fichier %s car ce n'est pas un foichier multi-composants",
//    /* 222 E_INVALID_EXPR_WITH_EQUAL */
//    "L'expression est invalide :  <expr \"=\" expr> n'est pas un expression, mais un prédicat.",
//    /* 223 E_NO_TYPE_FOR_IDENT_LOC_EXPECTED %s ident_type %s ident_name %s ident_name %s clause name %s compo name */
//    "Le (la) %s %s n'a pas de type. %s doit être typé(e) dans la clause %s de %s",
//    /* 224 E_IDENT_CAN_NOT_BE_TYPED_HERE %s ident_type %s ident_name %s ident_name %s clause name %s clause name */
//    "Le (la) %s %s ne peut pas être typé(e) dans la clause %s. Il (elle) doit être typé(e) dans la clause %s",
//    /* 225 E_CAN_NOT_WRITE_OP %s op_name */
//    "%s est une opération : elle ne peut pas se trouver en partie gauche d'une affectation",
//    /* 226 E_IDENT_HAS_NOT_BEEN_VALUATED %s ident_type %s ident_name %s mach name*/
//    "Le (la) %s %s n'a pas été valué(e) dans la clause VALUES de %s",
//    /* 227 E_LOCATION_OF_VALUES_CLAUSE */
//    "Localisation de la clause VALUES",
//    /* 228 E_IDENT_HAS_NOT_BEEN_INITIALISED %s ident_type %s ident_name %s mach name*/
//    "La %s %s n'a pas été initialisée dans la clause INITIALISATION de %s",
//    /* 229 E_LOCATION_OF_INITIALISATION_CLAUSE */
//    "Localisation de la clause INITIALISATION",
//    /* 230 E_INVALID_IDENTIFIER %s ident_name */
//    "L'identificateur \"%s\" est invalide : Le caractère '$' n'est utilisable dans un identificateur que dans le suffixe \"$0\"",
//    /* 231	E_GARBAGE_BEFORE_COMPONENT */
//    "Lexème parasite avant le début du composant",
//    /* 232 E_MISSING_DOT %s expr name */
//    "Le '.' dans la construction de l'expression %s devrait se trouver ici",
//    /* 233 E_SET_FORMAL_PARAMETERS_CAN_NOT_BE_AN_EMPTY_INTERVAL */
//    "Un paramètre formel ensemblise ne peut pas être valué avec un intervalle vide",
//    /* 234 E_DEFINITION_CAN_NOT_REWRITE_ITSELF %s def_name */
//    "La definition %s ne peut pas apparaître dans sa propre règle de réécriture",
//    /*  235 E_UNEXPECTED_NODE_TYPE %d type %s name */
//    "Type de noeud %d (%s) non attendu",
//    /* 236 E_CAN_NOT_CREATE_SYNTAXIC_SKEL_REF_PROMOTES %s name */
//    "Impossible de créer un raffinement syntaxique du composant %s car il contient une clause PROMOTES",
//    /* 237 E_CONSTANTS_SHOULD_NOT_BE_RENAMED %s ident_name %s ident_type %s machine def name */
//    "L'identifiant \"%s\" defini comme un(e) %s dans %s, ne doit pas etre renommé",
//    /* 238 E_VARIABLES_SHOULD_BE_RENAMED %s ident_name %s ident_type %s machine def name*/
//    "L'identifiant \"%s\" defini comme un(e) %s dans %s, n'est pas renommé, ou est renommé avec un mauvais préfixe",
//    /* 239 E_CAN_NOT_REFINES, %s ident_type %s ident_name %s ident_type*/
//    "%s \"%s\" ne peut raffiner un(e) %s",
//    /* 240 E_ENUMERATED_IDENT_DIFFER %s SET's name %s glued val %s original val*/
//    "Dans la redéfinition de \"%s\", la valeur enumérée\"%s\" est différente de  \"%s\"",
//    /* 241 E_ENUMERATED_IDENT_EXCESS %s SET's name %s execess val */
//    "Dans la redéfinition \"%s\", la valeur énumérée \"%s\" est en trop",
//    /* 242 E_ENUMERATED_IDENT_MISSING %s SET's name %s missing value*/
//    "Dans la redéfinition de \"%s\", la valeur énumérée \"%s\" manque",
//    /* 243 E_ITEM_IS_NOT_ALLOWED_IN_AN_IMPLEMENTATION %s class_name */
//    "%s interdit(e) dans une implémentation",
//    /* 244 E_RECORD_IS_NOT_A_B0_RECORD,  %s class_name */
//    "%s non implémentable",
//    /* 245 E_B0_INVALID_ARRAY_EXPRESSION,  %s class_name (table_access) */
//    "Expression tableau %s invalide",
//    /* 246 E_B0_INDEX_MUST_BE_A_TERM,  table_access */
//    "L'index du tableau doit être un terme",
//    /* 247 E_B0_INVALID_BUILTIN_FOR_RANGE,  %s class_name  */
//    "Ensemble prédéfini %s invalide ",
//    /* 248 E_B0_INTERVAL_BOUND_MUST_BE_ARITH_EXPRESSION,  (Intervalle) */
//    "Borne d'intervalle %s invalide  (une expression arithmétique est attendue)",
//    /* 249 E_B0_INVALID_EFFECTIVE_PARAMETERS_IN_OP_CALL, %s op_name %s class_name */
//    "Lors de l'appel de l'opération %s : %s n'est pas un paramètre effectif valide (un terme ou une expression arithmétique est attendue)",
//    /* 250 E_B0_INVALID_RANGE_FOR_ARRAY_AGGREGATE,  %s class_name  */
//    "Ensemble indice %s invalide pour un tableau constant",
//    /* 251 E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE %s class_name */
//    "valeur %s invalide pour un tableau constant",
//    /* 252 E_B0_INVALID_INDEX_FOR_MAPLET,  %s class_name  */
//    "Index %s invalide pour un maplet (un scalaire littéral est attendu)",
//    /* 253 E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION,  %s class_name */
//    "Opérande %s de %s invalide (une expression arithmétique est attendue)",
//    /* 254 E_B0_PARAMETER_MUST_BE_A_BOOL_EXPRESSION,  %s class_name  */
//    "Opérande %s de bool invalide (une expression booléenne est attendue)",
//    /* 255 E_B0_PREDICATE_IS_NOT_A_CONDITION,* %s class_name  */
//    "%s n'est pas une condition",
//    /* 256 E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM,  %s class_name  */
//    "%s n'est pas un terme simple",
//    /* 257 E_B0_INVALID_VALUE_FOR_MAPLET %s class_name */
//    "Valeur %s invalide pour un maplet",
//    /* 258 E_B0_INVALID_MAPLET,   */
//    "Maplet non implémentable",
//    /* 259 E_B0_INVALID_TERM %s class_name */
//    "%s n'est pas un terme",
//    /* 260 E_B0_INVALID_SIMPLE_TERM %s class_name */
//    "%s n'est pas un terme simple",
//    /* 261 E_B0_INVALID_EFFECTIVE_PARAMETER,   */
//    "Paramètre effectif non implémentable",
//    /* 262 E_B0_INVALID_RECORD_ITEM,    */
//    "Elément de record non implémentable",
//    /* 263 E_B0_INVALID_CASE_SELECTOR  %s class_name   */
//    "Sélecteur de case %s invalide (un terme simple est attendu)",
//    /* 264 E_B0_INVALID_SIMULTANEOUS_AFFECTATION */
//    "Affectation simultanée non implémentable",
//    /* 265 E_B0_INVALID_LHS_IN_AFFECTATION  %s class_name   */
//    "Partie gauche d'affectation non valide",
//    /* 266 E_B0_INVALID_RHS_IN_AFFECTATION	,  %s class_name   */
//    "Partie droite d'affectation non valide",
//    /* 267 E_B0_INVALID_IDENT_TYPE_FOR_FORMAL_PARAMETER  %s ident_type_name  , %s name */
//    "Nature d'identificateur %s invalide pour le parametre formel scalaire %s",
//    /* 268 E_B0_INVALID_DEST_SET_FOR_TOTAL_FUNCTION   %s class_name   */
//    "Ensemble de destination %s invalide pour une fonction totale",
//    /* 269 E_B0_INVALID_RELATION_FOR_CONCRETE_TYPE    */
//    "Nature de relation invalide pour une donnée concrète (fonction totale attendue)",
//    /* 270 E_B0_INVALID_SRC_SET_FOR_TOTAL_FUNCTION   %s class_name   */
//    "Ensemble source %s invalide pour une fonction totale",
//    /* 271 E_B0_INVALID_SIMPLE_SET	 %s class_name   */
//    "Ensemble simple %s invalide ",
//    /* 272 E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT  %s class_name, %s name  */
//    "Expression de typage %s invalide pour la constante concrète %s",
//    /* 273 E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE	 %s class_name, %s name  */
//    "Expression de typage %s invalide pour la variable concrète %s",
//    /* 274 E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER	 %s class_name, %s name */
//    "Expression de typage %s invalide pour le paramètre formel scalaire %s",
//    /* 275 E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER	 %s class_name, %s name */
//    "Expression de typage %s invalide pour le paramètre d'entrée d'opération %s",
//    /* 276 E_B0_INVALID_TYPING_PREDICATE     */
//    "Prédicat de typage invalide ( = ou : attendu)",
//    /* 277 E_B0_INVALID_IDENT_TYPE_FOR_ARRAY_INDEX   %s ident_type_name  , %s name */
//    "Nature d'identificateur %s invalide pour l'index %s",
//    /* 278 E_B0_SCALAR_EXPECTED  %s class_name */
//    "Type invalide pour l'expression %s : scalaire attendu" ,
//    /* 279 E_B0_INVALID_TYPING_EXPR_FOR_RECORD_ITEM	 %s class_name   */
//    "Expression de typage %s invalide pour un élément de record",
//    /* 280 E_B0_INVALID_TYPE	 %s ident_type_name  , %s name */
//    "Type de %s %s non implémentable",
//    /* 281 E_B0_INVALID_TYPE_FOR_RECORD_LABEL	    */
//    "Type de label de record non implémentable",
//    /* 282 E_B0_INVALID_EXPR_FOR_INSTANCIATION  %s class_name   */
//    "Expression %s invalide pour une instanciation de paramètre formels",
//    /* 283 E_B0_INVALID_INDEX_FOR_ARRAY_VALUATION	%s class_name (literal scalar expected)  */
//    "Index %s invalide pour une valuation de tableau constant (scalaire littéral attendu)",
//    /* 284 E_B0_INVALID_IDENT_TYPE_FOR_INSTANCIATION	%s ident_type_name  , %s name */
//    "Type d'identificateur %s de l'identificateur %s invalide (dans un contexte d'instanciation de paramètres formels)",
//    /* 285 E_B0_INVALID_IDENT_TYPE_FOR_VALUATION	%s ident_type_name  , %s name */
//    "Type d'identificateur %s de l'identificateur %s invalide (dans un contexte de valuation des constantes concrètes et des ensembles abstraits)",
//    /* 286 E_B0_INVALID_EXPR_FOR_VALUATION	%s class_name , %s ident_type_name, %s ident_name  */
//    "Expression %s invalide pour la valuation de %s %s",
//    /* 287 E_B0_INTERVAL_BOUND_MUST_BE_LITERAL_INTEGER  %s class_name */
//    "Borne d'intervalle invalide (entier littéral attendu)",
//    /* 288 E_B0_INVALID_IDENT_TYPE_FOR_CONCRETE_VALUE 	%s ident_type_name  , %s name */
//    "Type d'identificateur %s de l'identificateur %s invalide (donnée concrète attendue)",
//    /* 289 E_B0_IDENTIFIER_ALREADY_VALUED  %s ident_type_name  , %s name */
//    "%s %s est déjà valué",
//    /* 290 E_COMPO_CLASH %s compo_name */
//    "Redéfinition illégale du composant \"%s\"",
//    /* 291 E_UNEXPECTED_TYPE_CASE_BRANCH %s class_name*/
//    "L'expression \"%s\" est  interdite dans la branche du case, seul les entiers littéraux, les booléens littéraux et les valeurs énumérées littérales sont autorisés)",
//    /* 292 E_BRANCH_TYPE_DIFFER_FROM_SELECTOR_TYPE %s branch type, %s selector type*/
//    "Le type %s dans la branche du case, est différent du type du sélecteur: %s",
//    /* 293 E_BRANCH_VALUE_ALREADY_EXIST */
//    "Cette valeur est deja utilisée dans un branche du CASE",
//    /* 294 E_INVALID_RENAMED_OP_IN_SPEC %s renop name */
//    "Nom d'opération invalide : un nom d'opération ne peut pas être renommé dans une spécification",
//    /* 295 E_LHS_OF_LET_SUBSTITUTION_CAN_NOT_BE_A_LOC_VAR %s variable */
//    "%s ne peut pas être valuée ici car c'est une variable locale de la substitiution LET",
//    /* 296 E_INVALID_UNIVERSAL_PREDICATE */
//    "Prédicat universel invalide. Les seules constructions autorisées sont  !x.(P1=>P2) ou !(x1,..xn).(P1=>P2)",
//    /* 297 E_BEFORE_ONLY_IF_VAR_PART_OF_BECOMES_SUCH_THAT %s ident name */
//    "$0 ne peut pas être utilisé car %s n'est pas un quantificateur local de la substitution devient tel que",
//    /* 298 E_BEFORE_ONLY_IF_VAR_OF_WHILE_SPECIFIED_IN_ABSTRACTION %s ident_name %s mach name %s abstraction_name */
//    "$0 ne peut pas être utilisé car %s n'est pas spécifié dans l'abstraction de %s (%s)",
//    /* 299 E_B0_FORMAL_SET_PARAMETERS_CAN_NOT_BE_TYPED %s ident name*/
//    "Le paramètre formel \"%s\" ne peut pas être typé",
//    /* 300 E_BEFORE_ONLY_IN_INVARIANT_OF_WHILE, %s ident name */
//    "$0 ne peut pas être utilisé car %s n'est pas présent dans dans un prédicat de l'INVARIANT d'une boucle WHILE",
//    /* 301 E_B0_MAPLET_ALREADY_EXISTS */
//    "L'indice du maplet est déjà utilisé dans l'ensemble de maplets",
//    /* 302 E_B0_LACKS_OF_MAPLETS_FOR_CURRENT_ARRAY */
//    "Il manque des indices pour couvrir la totalité du tableau",
//    /* 303 E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS */
//    "Les tableaux en partie gauche et droite sont incompatibles",
//    /* 304 E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_FORMAL_AND_EFFECTIVE_PARAM */
//    "Le paramètre effectif de type tableau est incompatible avec le paramètre formel",
//    /* 305 E_MACHINE_MUST_INCLUDE_USED_MACHINE (%s M1) (%s M2) (%s M3) (%s M1) (%s M3)*/
//    "\"%s\" INCLUDES \"%s\" qui USES \"%s\", donc \"%s\" doit inclure \"%s\"",
//    /* 306 E_ILLEGAL_CALL_OF_INCLUDED_OPERATIONS_IN_PARALLEL_SUBST %s used_machine name*/
//    "Il est interdit d'appeler en parallèle deux opérations de la même instance de machine incluse \"%s\"",
//    /* 307 E_BEFORE_ONLY_IF_VAR_OF_WHILE_GLUED_OR_FROM_ABSTRACTION %s var name */
//    "$0 ne peut pas être utilisé car %s n'est ni une variable collée, ni une variable concrète raffinant une variable de l'abstraction",
//    /* 308 E_EOF_FOUND_WHILE_PARSING_DEF_FILE %s def file name */
//    "Fin de ficher lors de l'analyse du fichier de définition %s",
//    /* 309 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE %s op name %s oper type */
//    "L'opérateur %s requiert une opérande du type arbre (type fourni: %s)",
//    /* 310 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_TREE %s op name %s oper type*/
//    "L'opérateur %s requiert une opérande du type sequence d'arbre (type fourni: %s)",
//    /* 311 E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE %s ntype %s ttype */
//    "Il est impossible d'ajouter un noeud de type %s a un arbre de type %s",
//    /* 312 E_OPERAND_OF_BUILTIN_OP_IS_NOT_SEQ_OF_INT %s op name %s oper type */
//    "L'opérateur %s requiert une opérande du type séquence d'entiers (type fourni: %s)",
//    /* 313 E_BUILTIN_OP_NEEDS_THREE_OPERANDS %s op name %d nb oper */
//    "L'opérateur prédéfini %s requiert trois opérandes (et pas %s opérandes)",
//    /* 314 E_BUILTIN_OP_NEEDS_ONE_OR_THREE_OPERANDS  %s op name %d nb oper */
//    "L'opérateur prédéfini %s requiert un ou trois opérandes (et pas %s opérandes)",
//    /* 315 E_B0_DATA_IS_TYPED_WITH_WRONG_IDENT_TYPE   %s ident_type_name  , %s name , %s ident_type_name, %s name  */
//    "%s \"%s\" n'est pas implémentable (typé avec %s \"%s\")",
//    /* 316 E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_SEQ %s op_name %s oper_type */
//    "L'opérateur %s requiert une opérande du type séquence de séquences (type fourni: %s)",
//    /* 317 E_INVALID_IMAGE_INDEX_TYPE %s type_name %s expect_type*/
//    "Type %s de l'index de l'image incorrect (ce n'est pas un sous-ensemble de %s)",
//    /* 318 E_IDENT_CLASHES_WITH_KERNEL_BUILTIN %s ident_name */
//    "L'identificateur %s ne peut pas être utilisé car c'est un mot réservé du Logic Solver de l'Atelier B",
//    /* 319  E_KERNEL_HIDDEN_CLASH %s ident_name %s other_name %s other_component %s*/
//    "L'identificateur %s, et l'identificateur %s du composant %s sont en collision selon les règles du Logic Solver et du Prouveur de l'Atelier B's",
//    /* 320  E_CAN_NOT_TYPE_WITH_A_GENERIC_TYPE %s ident_name %s type_name */
//    "L'identificateur non typé %s ne peut pas être typé avec le type générique %s",
//    /* 321 E_LOCAL_OPERATIONS_FORBIDDEN_IN_SPEC_OR_REF */
//    "La clause LOCAL_OPERATIONS est interdite dans une spécification et dans un raffinement",
//    /* 322 E_LOCAL_OPERATION_IS_NOT_IMPLEMENTED %s local_op name %s compo name*/
//    "L'opération locale %s n'est pas implémentée dans le composant %s",
//    /* 323 E_LITERAL_INTEGER_GREATER_THAN_MAXINT */
//    "Valeur littérale %s%s plus grande que MAXINT=%s%s",
//    /* 324 E_LITERAL_INTEGER_LESS_THAN_MININT */
//    "Valeur littérale %s%s plus petite que MININIT=%s%s",
//    /* 325 E_MININT_IS_GREATER_THAN_MAXINT */
//    "La valeur de MININT=%s est plus petite que la valeur de MAXINT=%s",
//    /* 326 E_B0_TOOL_RESTRICTS_INSTANCIATION_TO_LITERALS  %s tool name %s */
//    "L'outil %s impose un contrôle supplémentaire sur les instanciations B0 qui ne peuvent pas utiliser que des littéraux (entiers ou booléens)",
//    /* 327 E_MS_LEXICAL_STATE_FILE */
//    "Lexeme %s non accepté dans le fichier d'état %s",
//    /* 328 E_MS_STATE_FILE_LEXEM_WITH_NO_VALUE */
//    "Pas de valeur associée au lexeme de type %d dans le fichier d'état %s",
//    /* 329 E_MS_SYNTAXE_STATE_FILE */
//    "Erreur de syntaxe dans le fichier d'état %s ligne %d : lexeme de type %d à la place d'un lexeme de type %d",
//    /* 330 E_MS_ACTION_TODO_INEXISTANT */
//    "L'action d'indice [%d,%d] n'existe pas dans le tableau des actions a executer",
//    /* 331 E_MS_ACTION_TODO_INEXISTANT */
//    "L'action d'indice [%d,%d] n'existe pas dans le tableau des actions a mettre a jour",
//    /* 332 E_MS_UPDATE_ACTION_WITH_NULL_SIGNATURE */
//    "Mise a jour de l'etat de l'action %x avec une signature nulle",
//    /* 333 E_MS_TYPE_OF_ACTION_INEXISTANT */
//    "Le type d'action %d n'existe pas",
//    /* 334 E_MS_STATE_OF_ACTION_HAS_EVER_BEEN_MEMORIZED */
//    "L'etat de l'action %d a deja ete stockee dans la structure %x",
//    /* 335 E_MS_CAN_NOT_OPEN_STATE_FILE */
//    "Le fichier d'etat %s ne peut pas etre ouvert",
//    /* 336 E_CYCLE_IN_IMPLEMENTATION_OF_LOCAL_OP %s local_op_name */
//    "Dépendances circulaires dans le graphe d'appel d'opérations de l'implémentation de l'opération locale %s",
//    /* 337 E_LOCAL_OP_IS_PART_OF_THE_CYCLE %s local_op_name */
//    "L'opération locale %s fait partie du cycle",
//    /* 338 E_MULTIPLE_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP %s machine name %s oploc name %s machine_name */
//    "Appel illégal à une opération de %s : dans la spécification de l'opération locale %s, il y a un appel simultané à une autre opération de %s",
//    /* 339 E_PREVIOUS_MACHINE_CALL_IN_SPEC_OF_LOCAL_OP %s macihne_name %s oploc name*/
//    "Localisation de l'appel simultané à une opération de %s dans la spécification de l'opération locale %s",
//    /* 340   E_INVALID_OUTPUT_PARAM_CONC_VAR_IS_MODIFIED_IN_LOCAL_OP_SPEC %s op name %s var name %s var name %s op_name */
//    "Appel invalide de l'opération locale %s avec le paramètre de sortie %s : %s est une variable concrète modifiée dans le corps de la specification de %s",
//    /* 341 E_LOC_OF_MODIF_OF_CONC_VAR_IN_LOCAL_OP_SPEC %s var_name %s op_name */
//    "Localisation de la modification de la variable concrète %s dans la spécification de %s",
//    /* 342   E_IN_OP_LOCAL_OP_CALL_INVALID_IMPORTED_OUTPUT_PARAM_IS_MODIFIED_IN_IMPORTED_OP %s op name %s var name %s var name %s op_name */
//    "Dans la spécification de l'opération locale : appel invalide à l'opération importée %s avec le paramètre de sortie %s : %s est une variable importée modifiée dans le corps de la spécification de %s",
//    /* 343 E_LOC_OF_IMPORTED_VAR_VAR_IN_IMPORTED_OP %s var_name %s op_name */
//    "Localisation de la modification de la variable importée %s dans la spécification de l'opération importée %s",
//    /* 344 E_TOO_MANY_PARAM_IN_REF_OP %s op_name */
//    "Trop de paramètres dans l'opération raffinée \"%s\"",
//    /* 345 E_TOO_FEW_PARAM_IN_REF_OP %s op_name */
//    "Pas assez de paramètres dans l'opération raffinée \"%s\"",
//    /* 346 E_PARAM_NAME_DIFF_1  %s arg_name_in_spec %s arg_name_in_ref */
//    "Noms de paramètre différents (\"%s\" / \"%s\") ",
//    /* 347 E_PARAM_NAME_DIFF_2  %s op_name */
//    "entre la specification et le raffinement de l'opération \"%s\"",
//    /* 348 E_DOUBLE_REF  %s ident_name  */
//    "L'identificateur \"%s\" apparaît deux fois en partie gauche",
//    /* 349 E_IDENT_INIT_VAR_LOCAL %s invalide_ident  %s expected_ident */
//    "Invalide identificateur \"%s\" ( \"%s\" attendu )",
//    /* 350 E_B0_INVALID_TYPING_EXPR %s ident_name %s ident_type*/
//    "Invalide expression typante pour la %s %s (identificateur, 'BOOL' ou ensemble attendu)",
//    /* 351 E_B0_ABST_MACH_AND_CONC_INCL  %s machine name*/
//    "Le composant %s est abstrait et inclu un composant concret",
//    /* 352 E_B0_INTERVAL_TYPE_ONLY_IN_BASIC_TRANS_MACH */
//    "Les types intervalle ne sont alloués que dans les machines de base du traducteur (pragma basic_translator_machine)",
//    /* 353 E_B0_INVALID_RANGE_FOR_ARRAY_TYPE %s type_name*/
//    "Invalide rang pour le type tableau %s (intervalle, 'BOOL' ou ensemble enuméré attenu)",
//    /* 354 E_B0_ARRAY_MINIMAL_BOUND */
//    "Invalide borne minimale (literal integer 0 expected)",
//    /* 355 E_B0_ARRAY_MAX_BOUND */
//    "Invalide borne maximale d'intervalle (expression évaluable statiquement attendue)",
//    /* 356 E_B0_INVALID_ARRAY_TYPE */
//    "Type tableau invalide" ,
//    /* 357 E_LOCATION_OF_VALUATION */
//    "Localisation de la valuation",
//    /* 358 E_B0_OPERATION_MUST_BE_ABSTRACT %s operation name */
//    "L'operation %s doit être déclarée abstraite",
//    /* 359 E_B0_INDICES_ORDERED */
//    "Les indices d'un tableau doivent etre odrdonnées par ordre croissant",
//    /* 360 E_B0_INVALID_VALUATION_OF_ABSTRACT_SET %s abs set name */
//    "Valuation illégale de l'ensemble abstrait %s (ensemble abstrait ou constante intervalle d'entiers attendu)",
//    /* 361 E_INVALID_OP_REFINEMENT %s op_name */
//    "Invalide raffinement d'opération, %s n'a pas la meme signature dans l'absraction",
//    /* 362 *E_B0_OP_MUST_CONCRETE_IN_IMPL %s op_name*/
//    "L'opération %s doit etre concrete en implantation",
//    /* 363 E_B0_INVALID_EXPR */
//    "Invalide expression",
//    /* 364 E_B0_NO_LITERAL_INTEGER_TYPE %s machine name*/
//    "Pas de type donné pour les littéraux entier de la machine %s (pragma literal_integer_type)",
//    /* 365 E_B0_TYPE_DATA_EXPECTED */
//    "Constante concrète, ensemble ou BOOL attendu",
//    /* 366 E_B0_TYPE_INPUT_PARAM_EXPECTED */
//    "Constante concrète, ensemble, BOOL ou STRING attendu",
//    /* 367 E_B0_LOC_VAR_MUST_INITIALISED %s var_loc_name*/
//    "La variable mocale %s doit etre initialisée",
//    /* 368 E_B0_INVALID_TYPING_EXPR_FOR_LOCAL_VARIABLE %s class_name, %s name  */
//    "Expression de typage %s invalide pour la variable locale %s",
//    /* 369 E_B0_INVALID_TYPING_EXPR_FOR_OP_OUT_PARAM %s class_name, %s name  */
//    "Expression de typage %s invalide pour le paramètre de retour %s",
//    /* 370 E_B0_INVALID_TYPE_DECL %s ident_name*/
//    "Invalide déclaration pour le type %s",
//    /* 371 E_B0_PARAM_FORBI_BOM */
//    "Paramètre de machine interdit",
//    /* 372 E_B0_OPERATION_MUST_BE_CONCRETE %s operation name*/
//    "L'operation %s doit etre déclarée concrète",
//    /* 373 E_B0_INVALID_TYPE_VALUATION type_name*/
//    "La valuation et la déclaration de %s doit etre à l'identique",
//    /* 374 E_B0_INCOMPATIBLE_DECLARATIONS %s data_name*/
//    "La déclaration de %s est incompatible avec la précédente",
//    /* 375 E_B0_LOCATION_OF_FIRST_DECLARATION */
//    "Localisation de la précédente déclaration",
//    /* 376 E_B0_AGREGATE_ONLY_IN_VAR_VALUES */
//    "Les expressions agregat tableau ne sont alloué que pour intialiser une constante ou une varirable locale",
//    /* 377 E_B0_INCOMPATIBLE_TYPE_FOR_PARAMS %s effective_type %s formal_type %sformal_parameter_name*/
//    "Le type %s du paramètre effectif est incompatible avec le type %s du paramètre formel (paramètre formel %s)",
//    /* 378 E_B0_IDENT_CANT_BE_REFERED %s ident_name */
//    "%s ne peut être référencée en paramètre effectif",
//    /* 379 E_B0_REDEFINITION_OF_INLINE */
//    "Redéfinition illégale du pragma INLINE",
//    /* 380 E_B0_REDEFINITION_OF_INLINE_ALL */
//    "Redéfinition illégale du pragma INLINE_ALL",
//    /* 381 E_B0_INLINE_AND_INLINE_ALL */
//    "Le pragam INLINE ne peut etre utilisé avec le pragma INLINE_ALL",
//    /* 382 E_B0_IS_NOT_OP_FROM_IMPL %s op_name %s implemenation_name*/
//    "L'operation %s n'est pas une opération de l'implantation %s",
//    /* 383 E_B0_INLINE_ONLY_IN_IMPL_OR_MACH */
//    "Les pragmas INLINE et INLINE_ALL sont alloués uniquement en implantation ou spécification",
//    /* 384 E_B0_INLINE_SHOULD_PARAM */
//    "Le pragma INLINE doit avoir un paramètre",
//    /* 385 E_REDEF_PRAG_LIT_INT_TYPE */
//    "Redéfinition illégale du pragma literal_integer_type",
//    /* 386 E_LIT_INT_TYPE_TAKES_ONE_PARAM */
//    "Le pragma literal_integer_type prend un et un seul paramètre",
//    /* 387 E_B0_ARRAY_EXPR_FORBIDDEN */
//    "Expression de tableau interdite",
//    /* 388 E_B0_INVALID_IDENT */
//    "%s nom d'identificateur invalide",
//    /* 389 E_B0_INVALID_TYPE_FOR_BECOMES_MEMBER %s type_name */
//    "Type %s doit être un type complet complet",
//    /* 390 E_B0_INVALID_BECOMES_MEMBER */
//    "Invalide substitution '::' ('identificateur :: type' attendue)",
//    /* 391 E_B0_IS_NOT_OP_FROM_MCH %s op_name %s spec_name*/
//    "L'operation %s n'est pas une opération de la machine %s",
//    /* 392 E_B0_SET_VALUED_WITH_CONSTANTS_FROM_SAME_MODULE %s set_nam %s constant_name*/
//    "L'ensemble %s ne peut pas être valuée par la constante %s du même module",
//    /* 393 E_B0_OPERATION_ONLY_ONE_OUT_PARAM %s operation_name */
//    "L'operation %s ne doit avoir qu'un seul paramètre",
//    /* 394 E_B0_EXPR_STATIC */
//    "L'expression doit être statique",
//    /* 395 E_B0_OM_INVALID_RANGE_FOR_ARRAY_AGGREGATE */
//    "Ensemble indice %s invalide pour un agrégat tableau",
//    /* 396 E_B0_INVALID_TYPE_FOR_PARAM %s new_type_name , %s parameter_name, operation_name previous_type_name */
//    "Le type %s du paramètre %s de l'opérationn %s est incompatible avec le type précédent %s",
//    /* 397 E_B0_LIT_PRAGMA_PARAM_CONCRETE_CONSTANT */
//    "Le paramètre du pragma literal_integer_type doit être une constante concrète",
//    /* 398 E_B0_LIT_PRAGMA_PARAM_INTEGER_TYPE */
//    "Le paramètre du pragma literal_integer_type doit dénoter un type entier",
//    /* 399 E_HEXADECIMAL_LITERAL_OVERFLOW */
//    "Nombre hexadécimal trop grand",
//    /* 400 E_OPERAND_SHOULD_BE_A_ REAL %s type_name */
//    "L'opérande n'est pas un réel (son type est %s)"
//} ;

static const char *const english_warning_table[] =
{
    /* 00 W_NESTED_COMMENT */
    gettext_noop("Warning: Nested comment detected."),
    /* 01 W_START_OF_COMMENT */
    gettext_noop("Comment starts here."),
    /* 02 W_EOF_INSIDE_COMMENT */
    gettext_noop("End of file found inside comment."),
    /* 03 W_TRAILING_GARBAGE */
    gettext_noop("Warning : Trailing garbage ignored"),
    /* 04 W_EMPTY_CLAUSE %s (clause name) */
    gettext_noop("Warning : empty %s will be ignored"),
    /* 05 W_IDENT_INVALID_FOR_KERNEL %s (ident name) */
    gettext_noop("Warning : identifier \"%s\" has less than 2 characters : it will be rejected by Atelier-B's Logic Solver"),
    /* 06 W_OPEN_LEXEM_NOT_CLOSED */
    gettext_noop("Warning : opening lexem does not have a matching closing lexem"),
    /* 07 W_AMBIGOUS_MINUS */
    gettext_noop("Warning : ambiguous minus will be treated as unary minus"),
    /* 08 W_AMBIGOUS_MINUS_HINT %s (bin ascii) %s (bin ascii) */
    gettext_noop("(Hint : write '- op1 %s op2' instead of 'op1 %s - op2' to avoid this warning)"),
    /* 09 W_EXTRA_SCOL_NOT_STRICT_B */
    gettext_noop("Warning : strict B syntax forbids use of useless ';'"),
    /* 10 W_UNIMPLEMENTABLE_TYPE_FOR_IDENT %s type %s ident type %s ident name */
    gettext_noop("Warning : non-implementable type %s given for %s %s"),
    /* 11 W_EXTRA_PAREN_IN_TPRED_IGNORED */
    gettext_noop("Warning : extra '()' around identifiers in typing predicate are forbidden in strict B syntax and will be ignored"),
    /* 12 W_EXTRA_PAREN_IN_EXPR_IGNORED */
    gettext_noop("Warning : extra '()' around this expression are useless and will be ignored"),
    /* 13 W_B0_DETECTED_CYCLE_IN_INITIALISATION  %s ident_type_name  , %s name */
    gettext_noop("%s %s may have not been initialised yet"),
    /* 14 W_B0_DETECTED_CYCLE_IN_VALUATION  %s ident_type_name  , %s name */
    gettext_noop("%s %s has not been valued yet"),
    /* 15 W_USE_BEFORE_ON_VAR_NOT_MODIFIED_IN_WHILE %s var name */
    gettext_noop("$0 is useless since %s is not modified in the body of the WHILE loop"),
    /* 16 W_ENVIRONMENT_VARIABLE_IS_OUT_OF_DATE %s var name, %s res name*/
    gettext_noop("%s environment variable is out of date, use parameter resource %s instead"),
    /* 17 W_COMMA_WILL_BE_TREATED_AS_A_MAPLET */
    gettext_noop("Lexem ',' will be treated as a maplet ('|->')"),
    /* 18 W_ASSERTIONS_CLAUSE_WITHOUT_INVARIANT_CLAUSE */
    gettext_noop("Warning : ASSERTIONS clause without INVARIANT, only trivial predicates will be provable"),
    /* 19 W_VARIANT_WITHOUT_NON_DIVERGENCE_RESSOURCE */
    gettext_noop("Warning : VARIANT clause without divergence POs generation"),
    /* 20 W_REFINED_EVENT_NOT_CLOSED */
    gettext_noop("Warning : Event %s is refined by %s but not closed"),
    /* 21 W_PRAGMA_IN_DEF */
    gettext_noop("Warning : Pragma in definitions are not handled by compiler"),
} ;

//static const char *const french_warning_table[] =
//{
//    /* 00 W_NESTED_COMMENT */
//    "Attention: Commentaire imbriqué détecté.",
//    /* 01 W_START_OF_COMMENT */
//    "Début du commentaire.",
//    /* 02 W_EOF_INSIDE_COMMENT */
//    "Fin de fichier détectée dans un commentaire.",
//    /* 03 W_TRAILING_GARBAGE */
//    "Attention : caractères supplémentaires ignorés",
//    /* 04 W_EMPTY_CLAUSE %s (clause name) */
//    "Attention : la clause vide %s sera ignorée",
//    /* 05 W_IDENT_INVALID_FOR_KERNEL %s (ident name) */
//    "Attention : l'identificateur \"%s\" a moins de 2 caractères : il sera rejetté par le Logic Solver de l'Atelier B",
//    /* 06 W_OPEN_LEXEM_NOT_CLOSED */
//    "Attention : le lexème ouvrant n'a pas de lexème fermant associé",
//    /* 07 W_AMBIGOUS_MINUS */
//    "Attention : le moins ambigu sera traité comme un moins unaire",
//    /* 08 W_AMBIGOUS_MINUS_HINT %s (bin ascii) %s (bin ascii) */
//    "(Conseil : écrire '- op1 %s op2' à la place de de 'op1 %s - op2' pour éviter ce message)",
//    /* 09 W_EXTRA_SCOL_NOT_STRICT_B */
//    "Attention : la syntaxe B appliquée strictement interdit l'utilisation de ';' superflus",
//    /* 10 W_UNIMPLEMENTABLE_TYPE_FOR_IDENT %s type %s ident type %s ident name */
//    "Attention : le type %s fourni pour %s %s n'est pas implémentable",
//    /* 11 W_EXTRA_PAREN_IN_TPRED_IGNORED */
//    "Attention : les parenthèses autour des identificateurs du prédicat de typage sont interdites dans la syntaxe B stricte. Elles seront ignorées.",
//    /* 12 W_EXTRA_PAREN_IN_EXPR_IGNORED */
//    "Attention : les parenthèses autour de cette expression sont inutiles. Elles seront ignorées.",
//    /* 13 W_B0_DETECTED_CYCLE_IN_INITIALISATION  %s ident_type_name  , %s name */
//    "%s %s peut ne pas avoir encore été initialisée",
//    /* 14 W_B0_DETECTED_CYCLE_IN_VALUATION  %s ident_type_name  , %s name */
//    "%s %s n'a pas encore été valuée",
//    /* 15 W_USE_BEFORE_ON_VAR_NOT_MODIFIED_IN_WHILE %s var name */
//    "$0 n'a pas d'intérêt car %s n'est pas modifié dans le corps de la boucle WHILE",
//    /* 16 W_ENVIRONMENT_VARIABLE_IS_OUT_OF_DATE %s var name, %s res name*/
//    "La variable d'environnement %s est obselete, utilisez la resource %s à la place",
//    /* 17 W_COMMA_WILL_BE_TREATED_AS_A_MAPLET */
//    "Le lexème ',' sera traité comme un maplet ('|->')",
//    /* 18 W_ASSERTIONS_CLAUSE_WITHOUT_INVARIANT_CLAUSE */
//    "Attention : existance d'une clause ASSERTIONS sans clause INVARIANT, seuls les prédicats triviaux seront prouvable"
//} ;

static const char *const english_class_name_table[] =
{
    /* 00 NODE_MACHINE = 0, */
    gettext_noop("machine"),
    /* 01 NODE_BETREE, */
    gettext_noop("betree"),
    /* 02 NODE_SYMBOL, */
    gettext_noop("symbol"),
    /* 03 NODE_LEXEM, */
    gettext_noop("lexem"),
    /* 04 NODE_OPERATION, */
    gettext_noop("operation"),
    /* 05 NODE_SKIP, */
    gettext_noop("skip"),
    /* 06 NODE_BEGIN, */
    gettext_noop("begin"),
    /* 07 NODE_SEQUENCE, */
    gettext_noop("used_operation"),
    /* 08 NODE_CHOICE, */
    gettext_noop("choice"),
    /* 09 NODE_OR, */
    gettext_noop("or"),
    /* 10 NODE_PRECOND, */
    gettext_noop("precond"),
    /* 11 NODE_ASSERT, */
    gettext_noop("assert"),
    /* 12 NODE_IF, */
    gettext_noop("if"),
    /* 13 NODE_ELSE, */
    gettext_noop("else"),
    /* 14 NODE_SELECT, */
    gettext_noop("select"),
    /* 15 NODE_WHEN, */
    gettext_noop("when"),
    /* 16 NODE_PREDICATE */
    gettext_noop("predicate"),
    /* 17 NODE_EXPR */
    gettext_noop("expr"),
    /* 18 NODE_CASE, */
    gettext_noop("case"),
    /* 19 NODE_CASE_ITEM, */
    gettext_noop("case item"),
    /* 20 NODE_LIST_LINK, */
    gettext_noop("list link"),
    /* 21 NODE_LET, */
    gettext_noop("let"),
    /* 22 NODE_VALUATION, */
    gettext_noop("valuation"),
    /* 23 NODE_IDENT, */
    gettext_noop("identifier"),
    /* 24 NODE_VAR, */
    gettext_noop("var"),
    /* 25 NODE_WHILE, */
    gettext_noop("while"),
    /* 26 NODE_ANY, */
    gettext_noop("any"),
    /* 27 NODE_AFFECT, */
    gettext_noop("affect"),
    /* 28 NODE_BECOMES_MEMBER_OF, */
    gettext_noop("becomes member of"),
    /* 29 NODE_BECOMES_SUCH_THAT, */
    gettext_noop("becomes such that"),
    /* 30 NODE_OP_CALL, */
    gettext_noop("operation call"),
    /* 31 NODE_COMMENT, */
    gettext_noop("comment"),
    /* 32 NODE_FLAG, */
    gettext_noop("flag"),
    /* 33 NODE_DEFINITION, */
    gettext_noop("definition"),
    /* 34 NODE_USED_MACHINE, */
    gettext_noop("used machine"),
    /* 35 NODE_LITERAL_INTEGER, */
    gettext_noop("literal integer"),
    /* 36 NODE_BINARY_OP, */
    gettext_noop("binary expression"),
    /* 37 NODE_EXPR_WITH_PARENTHESIS, */
    gettext_noop("expression with parenthesis"),
    /* 38 NODE_UNARY_OP, */
    gettext_noop("unary op"),
    /* 39 NODE_ARRAY_ITEM,			 */
    gettext_noop("array item"),
    /* 40 NODE_CONVERSE,			 */
    gettext_noop("converse"),
    /* 41 NODE_LITERAL_STRING, */
    gettext_noop("literal string"),
    /* 42 NODE_KEYWORD, */
    gettext_noop("keyword"),
    /* 43 NODE_LAMBDA_EXPR, */
    gettext_noop("lambda expression,"),
    /* 44 NODE_BINARY_PREDICATE, */
    gettext_noop("binary predicate"),
    /* 45 NODE_TYPING_PREDICATE, */
    gettext_noop("typing predicate"),
    /* 46 NODE_COMPREHENSIVE_SET, */
    gettext_noop("comprehensive set"),
    /* 47 NODE_IMAGE, */
    gettext_noop("image"),
    /* 48 NODE_PREDICATE_WITH_PARENTHESIS, */
    gettext_noop("predicate with parenthesis"),
    /* 49 NODE_EXTENSIVE_SET, */
    gettext_noop("extensive set"),
    /* 50 NODE_NOT_PREDICATE, */
    gettext_noop("not predicate"),
    /* 51 NODE_EXPR_PREDICATE, */
    gettext_noop("predicate involving two expressions"),
    /* 52 NODE_UNIVERSAL_PREDICATE, */
    gettext_noop("universal predicate"),
    /* 53 NODE_EXISTENTIAL_PREDICATE, */
    gettext_noop("existential predicate"),
    /* 54 NODE_EMPTY_SEQ, */
    gettext_noop("empty seq"),
    /* 55 NODE_EMPTY_SET, */
    gettext_noop("empty set"),
    /* 56 NODE_SIGMA, */
    gettext_noop("sigma"),
    /* 57 NODE_PI, */
    gettext_noop("pi"),
    /* 58 NODE_QUANTIFIED_UNION, */
    gettext_noop("quantified union"),
    /* 59 NODE_QUANTIFIED_INTERSECTION, */
    gettext_noop("quantified intersection"),
    /* 60 NODE_LITERAL_BOOLEAN, */
    gettext_noop("literal boolean"),
    /* 61 NODE_LITERAL_ENUMERATED_VALUE, */
    gettext_noop("literal enumerated value"),
    /* 62 NODE_RELATION, */
    gettext_noop("relation"),
    /* 63 NODE_RENAMED_IDENT, */
    gettext_noop("renamed ident"),
    /* 64 NODE_EXTENSIVE_SEQ, */
    gettext_noop("extensive sequence"),
    /* 65 NODE_GENERALISED_UNION */
    gettext_noop("generalised union"),
    /* 66 NODE_GENERALISED_INTERSECTION */
    gettext_noop("generalised intersection"),
    /* 67 NODE_OBJECT */
    gettext_noop("generic object"),
    /* 68 NODE_ITEM */
    gettext_noop("generic item"),
    /* 69 NODE_SUBSTITUTION */
    gettext_noop("generic substitution"),
    /* 70 NODE_INTEGER */
    gettext_noop("integer"),
    /* 71 NODE_BETREE_MANAGER */
    gettext_noop("betree manager"),
    /* 72 NODE_BETREE_LIST */
    gettext_noop("betree list"),
    /* 73 NODE_TYPE */
    gettext_noop("type"),
    /* 74 NODE_PRODUCT_TYPE */
    gettext_noop("product type"),
    /* 75 NODE_SETOF_TYPE */
    gettext_noop("setof type"),
    /* 76 NODE_ABSTRACT_TYPE */
    gettext_noop("abstract type"),
    /* 77 NODE_ENUMERATED_TYPE */
    gettext_noop("enumerated type"),
    /* 78 NODE_PREDEFINED_TYPE */
    gettext_noop("predefined type"),
    /* 79 NODE_BASE_TYPE */
    gettext_noop("base type"),
    /* 80 NODE_CONSTRUCTOR_TYPE */
    gettext_noop("constructor type"),
    /* 81 NODE_GENERIC_TYPE */
    gettext_noop("generic type"),
    /* 82 NODE_GENERIC_OP */
    gettext_noop("generic operation"),
    /* 83 NODE_BOUND */
    gettext_noop("bound"),
    /* 84 NODE_OP_RESULT */
    gettext_noop("operation result"),
    /* 85 NODE_PRAGMA */
    gettext_noop("pragma"),
    /* 86 NODE_PRAGMA_LEXEM */
    gettext_noop("pragma lexem"),
    /* 87 NODE_B0_TYPE */
    gettext_noop("B0 type"),
    /* 88 NODE_B0_BASE_TYPE */
    gettext_noop("B0 base type"),
    /* 89 NODE_B0_ABSTRACT_TYPE */
    gettext_noop("B0 abstract type"),
    /* 90 NODE_B0_ENUMERATED_TYPE */
    gettext_noop("B0 enumerated type"),
    /* 91 NODE_B0_INTERVAL_TYPE */
    gettext_noop("B0 interval type"),
    /* 92 NODE_B0_ARRAY_TYPE */
    gettext_noop("B0 array type"),
    /* 93 NODE_RECORD */
    gettext_noop("record"),
    /* 94 NODE_RECORD_ITEM */
    gettext_noop("record item"),
    /* 95 NODE_STRUCT */
    gettext_noop("struct"),
    /* 96 NODE_RECORD_ACCESS */
    gettext_noop("record access"),
    /* 97 NODE_RECORD_TYPE */
    gettext_noop("record type"),
    /* 98 NODE_LABEL_TYPE */
    gettext_noop("label type"),
    /* 99 NODE_B0_RECORD_TYPE */
    gettext_noop("B0 record type"),
    /* 100 NODE_B0_LABEL_TYPE */
    gettext_noop("B0 label type"),
    /* 101 NODE_MSG_LINE */
    gettext_noop("message line"),
    /* 102 NODE_MSG_LINE_MANAGER */
    gettext_noop("message line manager"),
    /* 103 NODE_COMPONENT */
    gettext_noop("component"),
    /* 104 NODE_FILE_COMPONENT */
    gettext_noop("file component"),
    /* 105 NODE_PROJECT */
    gettext_noop("project"),
    /* 106 NODE_PROJECT_MANAGER */
    gettext_noop("project manager"),
    /* 107 NODE_FILE_DEFINITION */
    gettext_noop("file definition"),
    /* 108 NODE_COMPONENT_CHECKSUMS */
    gettext_noop("component checksums"),
    /* 109 NODE_OPERATION_CHECKSUMS */
    gettext_noop("operation checksums"),
    /* 110 NODE_OP_CALL_TREE */
    gettext_noop("local operation calling tree"),
    /* 111 NODE_WITNESS, */
    gettext_noop("witness"),
    /* 112 NODE_LITERAL_REAL, */
    gettext_noop("literal real"),
    /* 113 NODE_LABEL, */
    gettext_noop("label"),
    /* 114 NODE_JUMPIF, */
    gettext_noop("jumpif"),
    /* 115 NODE_USER_DEFINED1 */
    gettext_noop("user-defined class 1"),
    /* 116 NODE_USER_DEFINED2 */
    gettext_noop("user-defined class 2"),
    /* 117 NODE_USER_DEFINED3 */
    gettext_noop("user-defined class 3"),
    /* 118 NODE_USER_DEFINED4 */
    gettext_noop("user-defined class 4"),
    /* 119 NODE_USER_DEFINED5 */
    gettext_noop("user-defined class 5"),
    /* 120 NODE_USER_DEFINED6 */
    gettext_noop("user-defined class 6"),
    /* 121 NODE_USER_DEFINED7 */
    gettext_noop("user-defined class 7"),
    /* 122 NODE_USER_DEFINED8 */
    gettext_noop("user-defined class 8"),
    /* 123 NODE_USER_DEFINED9 */
    gettext_noop("user-defined class 9"),
    /* 124 NODE_USER_DEFINED10 */
    gettext_noop("user-defined class 10"),
    /* 125 NODE_USER_DEFINED11 */
    gettext_noop("user-defined class 11"),
    /* 126 NODE_USER_DEFINED12 */
    gettext_noop("user-defined class 12"),
    /* 127 NODE_USER_DEFINED13 */
    gettext_noop("user-defined class 13"),
    /* 128 NODE_USER_DEFINED14 */
    gettext_noop("user-defined class 14"),
    /* 129 NODE_USER_DEFINED15 */
    gettext_noop("user-defined class 15"),
    /* 130 NODE_USER_DEFINED16 */
    gettext_noop("user-defined class 16"),
    /* 131 NODE_USER_DEFINED17 */
    gettext_noop("user-defined class 17"),
    /* 132 NODE_USER_DEFINED18 */
    gettext_noop("user-defined class 18"),
    /* 133 NODE_USER_DEFINED19 */
    gettext_noop("user-defined class 19"),
    /* 134 NODE_USER_DEFINED20 */
    gettext_noop("user-defined class 20"),
} ;

//static const char *const french_class_name_table[] =
//{
//    /* 00 NODE_MACHINE = 0 */
//    "machine",
//    /* 01 NODE_BETREE */
//    "betree",
//    /* 02 NODE_SYMBOL */
//    "symbole",
//    /* 03 NODE_LEXEM */
//    "lexème",
//    /* 04 NODE_OPERATION */
//    "opération",
//    /* 05 NODE_SKIP */
//    "skip",
//    /* 06 NODE_BEGIN */
//    "BEGIN",
//    /* 07 NODE_SEQUENCE */
//    "opération requise",
//    /* 08 NODE_CHOICE */
//    "CHOICE",
//    /* 09 NODE_OR */
//    "OR",
//    /* 10 NODE_PRECOND */
//    "précondition",
//    /* 11 NODE_ASSERT */
//    "ASSERT",
//    /* 12 NODE_IF */
//    "IF",
//    /* 13 NODE_ELSE */
//    "ELSE",
//    /* 14 NODE_SELECT */
//    "SELECT",
//    /* 15 NODE_WHEN */
//    "WHEN",
//    /* 16 NODE_PREDICATE */
//    "prédicat",
//    /* 17 NODE_EXPR */
//    "expression",
//    /* 18 NODE_CASE */
//    "CASE",
//    /* 19 NODE_CASE_ITEM */
//    "élément de case",
//    /* 20 NODE_LIST_LINK */
//    "élément auxiliaire de liste",
//    /* 21 NODE_LET */
//    "LET",
//    /* 22 NODE_VALUATION */
//    "valuation",
//    /* 23 NODE_IDENT */
//    "identificateur",
//    /* 24 NODE_VAR */
//    "VAR",
//    /* 25 NODE_WHILE */
//    "WHILE",
//    /* 26 NODE_ANY */
//    "ANY",
//    /* 27 NODE_AFFECT */
//    "affectation",
//    /* 28 NODE_BECOMES_MEMBER_OF */
//    "substitution 'devient membre de'",
//    /* 29 NODE_BECOMES_SUCH_THAT */
//    "substitution 'devient tel que'",
//    /* 30 NODE_OP_CALL */
//    "appel d'opération",
//    /* 31 NODE_COMMENT */
//    "commentaire",
//    /* 32 NODE_FLAG */
//    "drapeau",
//    /* 33 NODE_DEFINITION */
//    "définition",
//    /* 34 NODE_USED_MACHINE */
//    "machine requise",
//    /* 35 NODE_LITERAL_INTEGER */
//    "entier littéral",
//    /* 36 NODE_BINARY_OP */
//    "expression binaire",
//    /* 37 NODE_EXPR_WITH_PARENTHESIS */
//    "expression parenthésée",
//    /* 38 NODE_UNARY_OP */
//    "opération unaire",
//    /* 39 NODE_ARRAY_ITEM			 */
//    "élément de tableau",
//    /* 40 NODE_CONVERSE			 */
//    "réciproque",
//    /* 41 NODE_LITERAL_STRING */
//    "chaîne littérale",
//    /* 42 NODE_KEYWORD */
//    "mot-clé",
//    /* 43 NODE_LAMBDA_EXPR */
//    "lambda expression",
//    /* 44 NODE_BINARY_PREDICATE */
//    "prédicat binaire",
//    /* 45 NODE_TYPING_PREDICATE */
//    "prédicat de typage",
//    /* 46 NODE_COMPREHENSIVE_SET */
//    "ensemble en compréhension",
//    /* 47 NODE_IMAGE */
//    "image",
//    /* 48 NODE_PREDICATE_WITH_PARENTHESIS, */
//    "précicat parenthésé",
//    /* 49 NODE_EXTENSIVE_SET, */
//    "ensemble en extension",
//    /* 50 NODE_NOT_PREDICATE, */
//    "prédicat de négation",
//    /* 51 NODE_EXPR_PREDICAT, */
//    "prédicat d'expressions",
//    /* 52 NODE_UNIVERSAL_PREDICAT, */
//    "prédicat universel",
//    /* 53 NODE_EXISTENTIAL_PREDICATE, */
//    "prédicat existentiel",
//    /* 54 NODE_EMPTY_SEQ, */
//    "séquence vide",
//    /* 55 NODE_EMPTY_SET, */
//    "ensemble vide",
//    /* 56 NODE_SIGMA, */
//    "SIGMA",
//    /* 57 NODE_PI, */
//    "PI",
//    /* 58 NODE_QUANTIFIED_UNION, */
//    "union quantifiée",
//    /* 59 NODE_QUANTIFIED_INTERSECTION, */
//    "intersection quantifiée",
//    /* 60 NODE_LITERAL_BOOLEAN, */
//    "booléen littéral",
//    /* 61 NODE_LITERAL_ENUMERATED_VALUE, */
//    "valeur énumérée littérale",
//    /* 62 NODE_RELATION, */
//    "relation",
//    /* 63 NODE_RENAMED_IDENT, */
//    "identificateur renommé",
//    /* 64 NODE_EXTENSIVE_SEQ, */
//    "séquence en extension",
//    /* 65 NODE_GENERALISED_UNION */
//    "union généralisée",
//    /* 66 NODE_GENERALISED_INTERSECTION */
//    "intersection généralisée",
//    /* 67 NODE_OBJECT */
//    "objet générique",
//    /* 68 NODE_ITEM */
//    "élément de liste générique",
//    /* 69 NODE_SUBSTITUTION */
//    "substitution",
//    /* 70 NODE_INTEGER */
//    "integer",
//    /* 71 NODE_BETREE_MANAGER */
//    "gestionnaire de betree",
//    /* 72 NODE_BETREE_LIST */
//    "liste de betree",
//    /* 73 NODE_TYPE */
//    "type",
//    /* 74 NODE_PRODUCT_TYPE */
//    "type produit cartésien",
//    /* 75 NODE_SETOF_TYPE */
//    "type setof",
//    /* 76 NODE_ABSTRACT_TYPE */
//    "type abstrait",
//    /* 77 NODE_ENUMERATED_TYPE */
//    "type énuméré",
//    /* 78 NODE_PREDEFINED_TYPE */
//    "type prédéfini",
//    /* 79 NODE_BASE_TYPE */
//    "type de base",
//    /* 80 NODE_CONSTRUCTOR_TYPE */
//    "type constructeur",
//    /* 81 NODE_GENERIC_TYPE */
//    "type générique",
//    /* 82 NODE_GENERIC_OP */
//    "opération generique",
//    /* 83 NODE_BOUND */
//    "borne",
//    /* 84 NODE_OP_RESULT */
//    "résultat d'opération",
//    /* 85 NODE_PRAGMA */
//    "pragma",
//    /* 86 NODE_PRAGMA_LEXEM */
//    "lexème de pragma",
//    /* 87 NODE_B0_TYPE */
//    "type B0",
//    /* 88 NODE_B0_BASE_TYPE */
//    "type B0 de base",
//    /* 89 NODE_B0_ABSTRACT_TYPE */
//    "type B0 abstrait",
//    /* 90 NODE_B0_ENUMERATED_TYPE */
//    "type B0 énuméré",
//    /* 91 NODE_B0_INTERVAL_TYPE */
//    "type B0 intervalle",
//    /* 92 NODE_B0_ARRAY_TYPE */
//    "type B0 tableau",
//    /* 93 NODE_RECORD */
//    "record",
//    /* 94 NODE_RECORD_ITEM */
//    "élément de record",
//    /* 95 NODE_STRUCT */
//    "struct",
//    /* 96 NODE_RECORD_ACCESS */
//    "accès à un élément de record",
//    /* 97 NODE_RECORD_TYPE */
//    "type record",
//    /* 98 NODE_LABEL_TYPE */
//    "type label",
//    /* 99 NODE_B0_RECORD_TYPE */
//    "type B0 record",
//    /* 100 NODE_B0_LABEL_TYPE */
//    "type B0 label",
//    /* 101 NODE_MSG_LINE */
//    "ligne de message",
//    /* 102 NODE_MSG_LINE_MANAGER */
//    "gestionnaire de lignes de message"
//    /* 101 NODE_MSG_LINE */
//    "message line",
//    /* 102 NODE_MSG_LINE_MANAGER */
//    "message line manager",
//    /* 103 NODE_COMPONENT */
//    "composant",
//    /* 104 NODE_FILE_COMPONENT */
//    "fichier composant",
//    /* 105 NODE_PROJECT */
//    "projet",
//    /* 106 NODE_PROJECT_MANAGER */
//    "gestionnaire de projet",
//    /* 107 NODE_FILE_DEFINITION */
//    "fichier de definitions",
//    /* 108 NODE_COMPONENT_CHECKSUMS */
//    "checksums d'un composant",
//    /* 109 NODE_OPERATION_CHECKSUMS */
//    "checksums d'une operation",
//    /* 110 NODE_OP_CALL_TREE */
//    "arbre d'appel d'operations locales"
//    /* 111 NODE_WITNESS */
//    "WITNESS",
//    /* 112 NODE_LITERAL_REAL */
//    "reel littéral",
//    /* 113 NODE_LABEL*/
//    "LABEL",
//    /* 111 NODE_USER_DEFINED1 */
//    "classe utilisateur 1",
//    /* 112 NODE_USER_DEFINED2 */
//    "classe utilisateur 2",
//    /* 113 NODE_USER_DEFINED3 */
//    "classe utilisateur 3",
//    /* 114 NODE_USER_DEFINED4 */
//    "classe utilisateur 4",
//    /* 115 NODE_USER_DEFINED5 */
//    "classe utilisateur 5",
//    /* 116 NODE_USER_DEFINED6 */
//    "classe utilisateur 6",
//    /* 117 NODE_USER_DEFINED7 */
//    "classe utilisateur 7",
//    /* 118 NODE_USER_DEFINED8 */
//    "classe utilisateur 8",
//    /* 119 NODE_USER_DEFINED9 */
//    "classe utilisateur 9",
//    /* 120 NODE_USER_DEFINED10 */
//    "classe utilisateur 10",
//    /* 121 NODE_USER_DEFINED11 */
//    "classe utilisateur 11",
//    /* 122 NODE_USER_DEFINED12 */
//    "classe utilisateur 12",
//    /* 123 NODE_USER_DEFINED13 */
//    "classe utilisateur 13",
//    /* 124 NODE_USER_DEFINED14 */
//    "classe utilisateur 14",
//    /* 125 NODE_USER_DEFINED15 */
//    "classe utilisateur 15",
//    /* 126 NODE_USER_DEFINED16 */
//    "classe utilisateur 16",
//    /* 127 NODE_USER_DEFINED17 */
//    "classe utilisateur 17",
//    /* 128 NODE_USER_DEFINED18 */
//    "classe utilisateur 18",
//    /* 129 NODE_USER_DEFINED19 */
//    "classe utilisateur 19",
//    /* 130 NODE_USER_DEFINED20 */
//    "classe utilisateur 20",
//} ;

static const char *const *error_table_sop = NULL ;
static const char *const *warning_table_sop = NULL ;
static const char *const *class_name_table_sop = NULL ;

static char * nls_localedir;

void set_localedir(char *l) {
}

// Initilalisation des tables mulilingues
// Fonction "cachee", connue que en local et dans c_cat.cpp
// Ne pas mettre de traces dans cette fonction car sinon on
// a une boucle si on appelle get_catalog depuis le code de TRACE
// (c'est le cas si l'on ne peut pas ecrire dans le fichier de traces)
void init_tables()
{

    TRACE1("init_tables error_table_sop=%x", error_table_sop) ;
    if (error_table_sop == NULL)
    {
        //        char *language = getenv("LANGUAGE") ;
        //        if ( (language != NULL) && (strcmp(language, "FRENCH") == 0) )
        //        {
        //            error_table_sop = french_error_table ;
        //            warning_table_sop = french_warning_table ;
        //            class_name_table_sop = french_class_name_table ;
        //            french_catalog() ;
        //            french_ident_type_name() ;
        //            french_lex_type() ;
        //            language_sop = FRENCH ;
        //        }
        //        else
        //        {
        error_table_sop = english_error_table ;
        warning_table_sop = english_warning_table ;
        class_name_table_sop = english_class_name_table ;
        english_catalog() ;
        english_ident_type_name() ;
        english_lex_type() ;
        language_sop = ENGLISH ;
        //        }

        //#ifndef NO_CHECKS
        //        TRACE2("error table : french %d, english %d",
        //               sizeof(french_error_table),
        //               sizeof(english_error_table)) ;
        //        ASSERT(sizeof(french_error_table) == sizeof(english_error_table)) ;
        //        TRACE2("warning table : french %d, english %d",
        //               sizeof(french_warning_table),
        //               sizeof(english_warning_table)) ;
        //        ASSERT(sizeof(french_warning_table) == sizeof(english_warning_table)) ;
        //#endif

    }
}


#ifdef TRACE
const char *_get_error_msg(const char *file,
                                    int line,
                                    T_error_code error_code)
#else
const char *get_error_msg(T_error_code error_code)
#endif
{
    TRACE2("get_error_msg appele depuis %s:%d", file, line) ;
    init_tables() ;

    return gettext(error_table_sop[error_code]) ;
}

#ifdef TRACE
const char *_get_warning_msg(const char *file,
                                      int line,
                                      T_warning_code warning_code)
#else
const char *get_warning_msg(T_warning_code warning_code)
#endif
{
    TRACE2("get_warning_msg appele depuis %s:%d", file, line) ;
    init_tables() ;

    return gettext(warning_table_sop[warning_code]) ;
}

const char *make_class_name(T_object *object)
{
    init_tables() ;
    if (object == NULL)
    {
        return "null" ;
    }
    T_node_type node_type = object->get_node_type() ;

    // On saute les indirections
    while (node_type == NODE_LIST_LINK)
    {
        object = ((T_list_link *)object)->get_object() ;
        node_type = object->get_node_type() ;
    }

    // Cas particulier des binops temporaires
    if (    (object->get_node_type() == NODE_BINARY_OP)
        && (((T_item *)object)->is_an_expr() == FALSE) )
	{
        return get_catalog(C_FORMULA) ;
    }

    return class_name_table_sop[node_type] ;
}

const char *make_class_name(T_node_type node_type)
{
    return class_name_table_sop[node_type] ;
}

//
// }{ Interface de production des messages
//
// Fonction "cachee", connue que de c_usedef.cpp
size_t msg_get_number_of_nodes(void)
{
    TRACE1("msg_get_number_of_nodes() -> %d",
           sizeof(english_class_name_table)/sizeof(char *)) ;
    return sizeof(english_class_name_table)/sizeof(char *) ;
}

//
//	}{	Fin du fichier
//
