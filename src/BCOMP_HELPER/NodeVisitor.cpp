#include <c_port.h>
#include <c_api.h>
#include "NodeVisitor.h"

NodeVisitor::~NodeVisitor()
{ }

void NodeVisitor::visit(T_object *o)
{
    T_node_type t = o->get_node_type();
    switch(t)
    {
    case NODE_MACHINE:
        visitMachine((T_machine*)o);
        break;
    case NODE_BETREE:
        visitBetree((T_betree*)o);
        break;
    case NODE_SYMBOL:
        visitSymbol((T_symbol*)o);
        break;
    case NODE_LEXEM:
        visitLexem((T_lexem*)o);
        break;
    case NODE_OPERATION:
        visitOperation((T_op*)o);
        break;
    case NODE_SKIP:
        visitSkip((T_skip*)o);
        break;
    case NODE_BEGIN:
        visitBegin((T_begin*)o);
        break;
    case NODE_USED_OP:
        visitUsedOp((T_used_op*)o);
        break;
    case NODE_CHOICE:
        visitChoice((T_choice*)o);
        break;
    case NODE_OR:
        visitOr((T_or*)o);
        break;

    case NODE_PRECOND:
        visitPrecond((T_precond*)o);
        break;
    case NODE_ASSERT:
        visitAssert((T_assert*)o);
        break;
    case NODE_IF:
        visitIf((T_if*)o);
        break;
    case NODE_ELSE:
        visitElse((T_else*)o);
        break;
    case NODE_SELECT:
        visitSelect((T_select*)o);
        break;
    case NODE_WHEN:
        visitWhen((T_when*)o);
        break;
    case NODE_PREDICATE:
        visitPredicate((T_predicate*)o);
        break;
    case NODE_EXPR:
        visitExpr((T_expr*)o);
        break;
    case NODE_CASE:
        visitCase((T_case*)o);
        break;
    case NODE_CASE_ITEM:
        visitCaseItem((T_case_item*)o);
        break;
    case NODE_LIST_LINK:
        visitListLink((T_list_link*)o);
        break;
    case NODE_LET:
        visitLet((T_let*)o);
        break;
    case NODE_VALUATION:
        visitValuation((T_valuation*)o);
        break;
    case NODE_IDENT:
        visitIdent((T_ident*)o);
        break;
    case NODE_VAR:
        visitVar((T_var*)o);
        break;
    case NODE_WHILE:
        visitWhile((T_while*)o);
        break;
    case NODE_ANY:
        visitAny((T_any*)o);
        break;
    case NODE_AFFECT:
        visitAffect((T_affect*)o);
        break;
    case NODE_BECOMES_MEMBER_OF:
        visitBecomesMemberOf((T_becomes_member_of*)o);
        break;
    case NODE_BECOMES_SUCH_THAT:
        visitBecomesSuchThat((T_becomes_such_that*)o);
        break;
    case NODE_OP_CALL:
        visitOpCall((T_op_call*)o);
        break;
    case NODE_COMMENT:
        visitComment((T_comment*)o);
        break;
    case NODE_FLAG:
        visitFlag((T_flag*)o);
        break;
    case NODE_DEFINITION:
        visitDefinition((T_definition*)o);
        break;
    case NODE_USED_MACHINE:
        visitUsedMachine((T_used_machine*)o);
        break;
    case NODE_LITERAL_INTEGER:
        visitLiteralInteger((T_literal_integer*)o);
        break;
    case NODE_BINARY_OP:
        visitBinaryOp((T_binary_op*)o);
        break;
    case NODE_EXPR_WITH_PARENTHESIS:
        visitExprWithParenthesis((T_expr_with_parenthesis*)o);
        break;
    case NODE_UNARY_OP:
        visitUnaryOp((T_unary_op*)o);
        break;
    case NODE_ARRAY_ITEM:
        visitArrayItem((T_array_item*)o);
        break;
    case NODE_CONVERSE:
        visitConverse((T_converse*)o);
        break;
    case NODE_LITERAL_STRING:
        visitLiteralString((T_literal_string*)o);
        break;
    case NODE_KEYWORD:
        visitKeyword((T_keyword*)o);
        break;
    case NODE_LAMBDA_EXPR:
        visitLambda((T_lambda_expr*)o);
        break;
    case NODE_BINARY_PREDICATE:
        visitBinaryPredicate((T_binary_predicate*)o);
        break;
    case NODE_TYPING_PREDICATE:
        visitTypingPredicate((T_typing_predicate*)o);
        break;
    case NODE_COMPREHENSIVE_SET:
        visitComprehensiveSet((T_comprehensive_set*)o);
        break;
    case NODE_IMAGE:
        visitImage((T_image*)o);
        break;
    case NODE_PREDICATE_WITH_PARENTHESIS:
        visitPredicateWithParenthesis((T_predicate_with_parenthesis*)o);
        break;
    case NODE_EXTENSIVE_SET:
        visitExtensiveSet((T_extensive_set*)o);
        break;
    case NODE_NOT_PREDICATE:
        visitNotPredicate((T_not_predicate*)o);
        break;
    case NODE_EXPR_PREDICATE:
        visitExprPredicate((T_expr_predicate*)o);
        break;
    case NODE_UNIVERSAL_PREDICATE:
        visitUniversalpredicate((T_universal_predicate*)o);
        break;
    case NODE_EXISTENTIAL_PREDICATE:
        visitExistentialPredicate((T_existential_predicate*)o);
        break;
    case NODE_EMPTY_SEQ:
        visitEmptySeq((T_empty_seq*)o);
        break;
    case NODE_EMPTY_SET:
        visitEmptySet((T_empty_set*)o);
        break;
    case NODE_SIGMA:
        visitSIGMA((T_sigma*)o);
        break;
    case NODE_PI:
        visitPI((T_pi*)o);
        break;
    case NODE_QUANTIFIED_UNION:
        visitQuantifiedUnion((T_quantified_union*)o);
        break;
    case NODE_QUANTIFIED_INTERSECTION:
        visitQuantifiedIntersection((T_quantified_intersection*)o);
        break;
    case NODE_LITERAL_BOOLEAN:
        visitLiteralBoolean((T_literal_boolean*)o);
        break;
    case NODE_LITERAL_ENUMERATED_VALUE:
        visitLiteralEnumeratedValue((T_literal_enumerated_value*)o);
        break;
    case NODE_LITERAL_REAL:
        visitLiteralReal((T_literal_real*)o);
        break;
    case NODE_RELATION:
        visitRelation((T_relation*)o);
        break;
    case NODE_RENAMED_IDENT:
        visitRenamedIdent((T_renamed_ident*)o);
        break;
    case NODE_EXTENSIVE_SEQ:
        visitExtensiveSeq((T_extensive_seq*)o);
        break;
    case NODE_GENERALISED_UNION:
        visitGeneralisedUnion((T_generalised_union*)o);
        break;
    case NODE_GENERALISED_INTERSECTION:
        visitgeneralisedIntersection((T_generalised_intersection*)o);
        break;
    case NODE_OBJECT:
        visitObject(o);
        break;
    case NODE_ITEM:
        visitItem((T_item*)o);
        break;
    case NODE_SUBSTITUTION:
        visitSubstitution((T_substitution*)o);
        break;
    case NODE_INTEGER:
        visitInteger((T_integer*)o);
        break;
    case NODE_BETREE_MANAGER:
        visitBetreeManager((T_betree_manager*)o);
        break;
    case NODE_BETREE_LIST:
        visitBetreeList((T_betree_list*)o);
        break;
    case NODE_TYPE:
        visitType((T_type*)o);
        break;
    case NODE_PRODUCT_TYPE:
        visitProductType((T_product_type*)o);
        break;
    case NODE_SETOF_TYPE:
        visitSetOfType((T_setof_type*)o);
        break;
    case NODE_ABSTRACT_TYPE:
        visitAbstractType((T_abstract_type*)o);
        break;
    case NODE_ENUMERATED_TYPE:
        visitEnumeratedType((T_enumerated_type*)o);
        break;
    case NODE_PREDEFINED_TYPE:
        visitPredefinedType((T_predefined_type*)o);
        break;
    case NODE_BASE_TYPE:
        visitBaseType((T_base_type*)o);
        break;
    case NODE_CONSTRUCTOR_TYPE:
        visitConstructorType((T_constructor_type*)o);
        break;
    case NODE_GENERIC_TYPE:
        visitGenericType((T_generic_type*)o);
        break;
    case NODE_GENERIC_OP:
        visitGenericOp((T_generic_op*)o);
        break;
    case NODE_BOUND:
        visitBound((T_bound*)o);
        break;
    case NODE_OP_RESULT:
        visitOpResult((T_op_result*)o);
        break;
    case NODE_PRAGMA:
        visitPragma((T_pragma*)o);
        break;
    case NODE_PRAGMA_LEXEM:
        visitPragmaLexem((T_pragma_lexem*)o);
        break;
    case NODE_B0_TYPE:
        visitB0Type((T_B0_type*)o);
        break;
    case NODE_B0_BASE_TYPE:
        visitB0BaseType((T_B0_base_type*)o);
        break;
    case NODE_B0_ABSTRACT_TYPE:
        visitB0AbstractType((T_B0_abstract_type*)o);
        break;
    case NODE_B0_ENUMERATED_TYPE:
        visitB0EnumeratedType((T_B0_enumerated_type*)o);
        break;
    case NODE_B0_INTERVAL_TYPE:
        visitB0IntervalType((T_B0_interval_type*)o);
        break;
    case NODE_B0_ARRAY_TYPE:
        visitB0ArrayType((T_B0_array_type*)o);
        break;
    case NODE_RECORD:
        visitRecord((T_record*)o);
        break;
    case NODE_RECORD_ITEM:
        visitRecordItem((T_record_item*)o);
        break;
    case NODE_STRUCT:
        visitStruct((T_struct*)o);
        break;
    case NODE_RECORD_ACCESS:
        visitRecordAccess((T_record_access*)o);
        break;
    case NODE_RECORD_TYPE:
        visitRecordType((T_record_type*)o);
        break;
    case NODE_LABEL_TYPE:
        visitLabelType((T_label_type*)o);
        break;
    case NODE_B0_RECORD_TYPE:
        visitB0RecordType((T_B0_record_type*)o);
        break;
    case NODE_B0_LABEL_TYPE:
        visitB0LabelType((T_B0_label_type*)o);
        break;
    case NODE_MSG_LINE:
        visitMsgLine((T_msg_line*)o);
        break;
    case NODE_MSG_LINE_MANAGER:
        visitMsgLineManager((T_msg_line_manager*)o);
        break;
    case NODE_COMPONENT:
        visitComponent((T_comment*)o);
        break;
    case NODE_FILE_COMPONENT:
        visitFileComponent((T_file_component*)o);
        break;
    case NODE_PROJECT:
        visitProject((T_project*)o);
        break;
    case NODE_PROJECT_MANAGER:
        visitProjectManager((T_project_manager*)o);
        break;
    case NODE_FILE_DEFINITION:
        visitFileDefinition((T_file_definition*)o);
        break;
    case NODE_COMPONENT_CHECKSUMS:
        visitComponentChecksums((T_component_checksums*)o);
        break;
    case NODE_OPERATION_CHECKSUMS:
        visitOperationChecksums((T_operation_checksums*)o);
        break;
    case NODE_WITNESS:
        visitWitness((T_witness*)o);
        break;
    default:
        visitObject(o);
    }
}

void NodeVisitor::visitMachine(T_machine*o)
{ visitItem(o); }
void NodeVisitor::visitBetree(T_betree*o)
{ visitItem(o); }
void NodeVisitor::visitSymbol(T_symbol*o)
{ visitObject(o); }
void NodeVisitor::visitLexem(T_lexem*o)
{ visitObject(o); }
void NodeVisitor::visitOperation(T_op*o)
{ visitGenericOp(o); }
void NodeVisitor::visitSkip(T_skip*o)
{ visitSubstitution(o); }
void NodeVisitor::visitBegin(T_begin*o)
{ visitSubstitution(o); }
void NodeVisitor::visitUsedOp(T_used_op*o)
{ visitGenericOp(o); }
void NodeVisitor::visitChoice(T_choice*o)
{ visitSubstitution(o); }
void NodeVisitor::visitOr(T_or*o)
{ visitItem(o); }
void NodeVisitor::visitPrecond(T_precond*o)
{ visitSubstitution(o); }
void NodeVisitor::visitAssert(T_assert*o)
{ visitSubstitution(o); }

void NodeVisitor::visitIf(T_if*o)
{ visitSubstitution(o); }
void NodeVisitor::visitElse(T_else*o)
{ visitItem(o); }
void NodeVisitor::visitSelect(T_select*o)
{ visitSubstitution(o); }
void NodeVisitor::visitWhen(T_when*o)
{ visitItem(o); }
void NodeVisitor::visitPredicate(T_predicate*o)
{ visitItem(o); }
void NodeVisitor::visitExpr(T_expr*o)
{ visitItem(o); }
void NodeVisitor::visitCase(T_case*o)
{ visitSubstitution(o); }

void NodeVisitor::visitCaseItem(T_case_item*o)
{ visitItem(o); }
void NodeVisitor::visitListLink(T_list_link*o)
{ visitItem(o); }

void NodeVisitor::visitLet(T_let*o) { visitSubstitution(o); }
void NodeVisitor::visitValuation(T_valuation*o) { visitItem(o); }
void NodeVisitor::visitIdent(T_ident*o) { visitExpr(o); }

void NodeVisitor::visitVar(T_var*o) { visitSubstitution(o); }
void NodeVisitor::visitWhile(T_while*o) { visitSubstitution(o); }
void NodeVisitor::visitAny(T_any*o) { visitSubstitution(o); }
void NodeVisitor::visitAffect(T_affect*o) { visitSubstitution(o); }
void NodeVisitor::visitBecomesMemberOf(T_becomes_member_of*o) { visitSubstitution(o); }
void NodeVisitor::visitBecomesSuchThat(T_becomes_such_that*o) { visitSubstitution(o); }
void NodeVisitor::visitOpCall(T_op_call*o) { visitSubstitution(o); }
void NodeVisitor::visitComment(T_comment*o) { visitItem(o); }
void NodeVisitor::visitFlag(T_flag*o) { visitItem(o); }
void NodeVisitor::visitDefinition(T_definition*o) { visitItem(o); }
void NodeVisitor::visitUsedMachine(T_used_machine*o) { visitItem(o); }
void NodeVisitor::visitLiteralInteger(T_literal_integer*o) { visitExpr(o); }
void NodeVisitor::visitBinaryOp(T_binary_op*o) { visitExpr(o); }
void NodeVisitor::visitExprWithParenthesis(T_expr_with_parenthesis*o) { visitExpr(o); }
void NodeVisitor::visitUnaryOp(T_unary_op*o) { visitExpr(o); }
void NodeVisitor::visitArrayItem(T_array_item*o) { visitExpr(o); }
void NodeVisitor::visitConverse(T_converse*o) { visitExpr(o); }
void NodeVisitor::visitLiteralString(T_literal_string*o) { visitExpr(o); }
void NodeVisitor::visitKeyword(T_keyword*o) { visitSymbol(o); }
void NodeVisitor::visitLambda(T_lambda_expr*o) { visitExpr(o); }
void NodeVisitor::visitBinaryPredicate(T_binary_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitTypingPredicate(T_typing_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitComprehensiveSet(T_comprehensive_set*o) { visitExpr(o); }
void NodeVisitor::visitImage(T_image*o) { visitExpr(o); }
void NodeVisitor::visitPredicateWithParenthesis(T_predicate_with_parenthesis*o) { visitPredicate(o); }
void NodeVisitor::visitExtensiveSet(T_extensive_set*o) { visitExpr(o); }
void NodeVisitor::visitNotPredicate(T_not_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitExprPredicate(T_expr_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitUniversalpredicate(T_universal_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitExistentialPredicate(T_existential_predicate*o) { visitPredicate(o); }
void NodeVisitor::visitEmptySeq(T_empty_seq*o) { visitExpr(o); }
void NodeVisitor::visitEmptySet(T_empty_set*o) { visitExpr(o); }
void NodeVisitor::visitSIGMA(T_sigma*o) { visitExpr(o); }
void NodeVisitor::visitPI(T_pi*o) { visitExpr(o); }
void NodeVisitor::visitQuantifiedUnion(T_quantified_union*o) { visitExpr(o); }
void NodeVisitor::visitQuantifiedIntersection(T_quantified_intersection*o) { visitExpr(o); }
void NodeVisitor::visitLiteralBoolean(T_literal_boolean*o) { visitExpr(o); }
void NodeVisitor::visitLiteralEnumeratedValue(T_literal_enumerated_value*o) { visitIdent(o); }
void NodeVisitor::visitLiteralReal(T_literal_real*o) { visitExpr(o); }
void NodeVisitor::visitRelation(T_relation*o) { visitExpr(o); }
void NodeVisitor::visitRenamedIdent(T_renamed_ident*o) { visitIdent(o); }
void NodeVisitor::visitExtensiveSeq(T_extensive_seq*o) { visitExpr(o); }
void NodeVisitor::visitGeneralisedUnion(T_generalised_union*o) { visitExpr(o); }
void NodeVisitor::visitgeneralisedIntersection(T_generalised_intersection*o) { visitExpr(o); }

void NodeVisitor::visitObject(T_object *o)
{ }

void NodeVisitor::visitItem(T_item*o)
{
    visitObject(o);
}

void NodeVisitor::visitSubstitution(T_substitution*o)
{
    visitItem(o);
}

void NodeVisitor::visitInteger(T_integer*o)
{
    visitObject(o);
}

void NodeVisitor::visitBetreeManager(T_betree_manager*o) { visitObject(o); }
void NodeVisitor::visitBetreeList(T_betree_list*o) { visitObject(o); }
void NodeVisitor::visitType(T_type*o) { visitItem(o); }
void NodeVisitor::visitProductType(T_product_type*o) { visitConstructorType(o); }
void NodeVisitor::visitSetOfType(T_setof_type*o) { visitConstructorType(o); }
void NodeVisitor::visitAbstractType(T_abstract_type*o) { visitBaseType(o); }

void NodeVisitor::visitEnumeratedType(T_enumerated_type*o) { visitBaseType(o); }
void NodeVisitor::visitPredefinedType(T_predefined_type*o) { visitBaseType(o); }
void NodeVisitor::visitBaseType(T_base_type*o) { visitType(o); }
void NodeVisitor::visitConstructorType(T_constructor_type*o) { visitType(o); }
void NodeVisitor::visitGenericType(T_generic_type*o) { visitType(o); }
void NodeVisitor::visitGenericOp(T_generic_op*o) { visitItem(o); }
void NodeVisitor::visitBound(T_bound*o) { visitExpr(o); }
void NodeVisitor::visitOpResult(T_op_result*o) { visitExpr(o); }
void NodeVisitor::visitPragma(T_pragma*o) { visitItem(o); }
void NodeVisitor::visitPragmaLexem(T_pragma_lexem*o) { visitObject(o); }
void NodeVisitor::visitB0Type(T_B0_type*o) { visitItem(o); }
void NodeVisitor::visitB0BaseType(T_B0_base_type*o) { visitB0Type(o); }
void NodeVisitor::visitB0AbstractType(T_B0_abstract_type*o) { visitB0Type(o); }
void NodeVisitor::visitB0EnumeratedType(T_B0_enumerated_type*o) { visitB0Type(o); }
void NodeVisitor::visitB0IntervalType(T_B0_interval_type*o) { visitB0Type(o); }
void NodeVisitor::visitB0ArrayType(T_B0_array_type*o) {visitB0Type(o);  }
void NodeVisitor::visitRecord(T_record*o) { visitExpr(o); }
void NodeVisitor::visitRecordItem(T_record_item*o) { visitExpr(o); }
void NodeVisitor::visitStruct(T_struct*o) { visitExpr(o); }
void NodeVisitor::visitRecordAccess(T_record_access*o) { visitExpr(o); }
void NodeVisitor::visitRecordType(T_record_type*o) { visitConstructorType(o); }
void NodeVisitor::visitLabelType(T_label_type*o) { visitType(o); }
void NodeVisitor::visitB0RecordType(T_B0_record_type*o)
{ visitB0Type(o); }
void NodeVisitor::visitB0LabelType(T_B0_label_type*o)
{ visitB0Type(o); }
void NodeVisitor::visitMsgLine(T_msg_line*o)
{
    visitItem(o);
}

void NodeVisitor::visitMsgLineManager(T_msg_line_manager*o)
{
    visitItem(o);
}

void NodeVisitor::visitComponent(T_comment*o)
{
    visitItem(o);
}

void NodeVisitor::visitFileComponent(T_file_component*o)
{
    visitItem(o);
}

void NodeVisitor::visitProject(T_project*o)
{
    visitItem(o);
}

void NodeVisitor::visitProjectManager(T_project_manager*o)
{
    visitObject(o);
}

void NodeVisitor::visitFileDefinition(T_file_definition*o)
{
    visitItem(o);
}

void NodeVisitor::visitComponentChecksums(T_component_checksums*o)
{
    visitItem(o);
}

void NodeVisitor::visitOperationChecksums(T_operation_checksums*o)
{
    visitItem(o);
}

void NodeVisitor::visitWitness(T_witness*o)
{
    visitSubstitution(o);
}
