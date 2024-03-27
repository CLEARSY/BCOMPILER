#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H
#include <c_port.h>
#include <c_api.h>

//class T_machine;
//class T_betree;
//class T_symbol;
//class T_lexem;
//class T_op;
//class T_skip;
//class T_begin;
//class T_used_op;
//class T_choice;
//class T_or;
//class T_precond;
//class T_assert;
//class T_if;
//class T_else;
//class T_select;
//class T_when;
//class T_predicate;
//class T_expr;
//class T_case;
//class T_case_item;
//class T_list_link;
//class T_let;
//class T_valuation;
//class T_ident;
//class T_var;
//class T_while;
//class T_any;
//class T_affect;
//class T_becomes_member_of;
//class T_becomes_such_that;
//class T_op_call;
//class T_comment;
//class T_flag;
//class T_definition;
//class T_used_machine;
//class T_literal_integer;
//class T_binary_op;
//class T_expr_with_parenthesis;
//class T_unary_op;
//class T_array_item;
//class T_converse;
//class T_literal_string;
//class T_keyword;
//class T_lambda_expr;
//class T_binary_predicate;
//class T_typing_predicate;
//class T_comprehensive_set;
//class T_image;
//class T_predicate_with_parenthesis;
//class T_extensive_set;
//class T_not_predicate;
//class T_epxpr_predicate;
//class T_universal_predicate;
//class T_existential_predicate;
//class T_empty_seq;
//class T_empty_set;
//class T_sigma;
//class T_sigma_real;
//class T_pi;
//class T_pi_real;
//class T_quantified_union;
//class T_quantified_intersection;
//class T_literal_boolean;
//class T_literal_enumerated_value;
//class T_relation;
//class T_renamed_ident;
//class T_extensive_seq;
//class T_generalised_union;
//class T_generalised_intersection;
//class T_object;
//class T_item;
//class T_substitution;
//class T_integer;
//class T_betree_manager;
//class T_betree_list;
//class T_type;
//class T_product_type;
//class T_setof_type;
//class T_abstract_type;
//class T_enumerated_type;
//class T_predefined_type;
//class T_base_type;
//class T_constructor_type;
//class T_generic_type;
//class T_generic_op;
//class T_bound;
//class T_op_result;
//class T_pragma;
//class T_pragma_lexem;
//class T_B0_type;
//class T_B0_base_type;
//class T_B0_abstract_type;
//class T_B0_enumerated_type;
//class T_B0_interval_type;
//class T_B0_array_type;
//class T_record;
//class T_record_item;
//class T_struct;
//class T_record_access;
//class T_record_type;
//class T_label_type;
//class T_B0_record_type;
//class T_B0_label_type;
//class T_msg_line;
//class T_msg_line_manager;
//class T_comment;
//class T_file_component;
//class T_project;
//class T_project_manager;
//class T_file_definition;
//class T_component_checksums;
//class T_operation_checksums;
//class T_witness;

/**
 * A visitor class simplifying visiting bcomp objects.
 */
class NodeVisitor
{
public:
    virtual ~NodeVisitor();

    /**
     * Visit the given object. Calling visit on a bcomp object will call
     * method of the visitor corresponding to the object, with the object
     * as parameter.
     *
     * For instance, if n is an instance of T_or, calling visit(n) will
     * call the method visitOr(T_or*) with n as parameter
     *
     * @param n the visited object
     */
    void visit(T_object *n);

    virtual void visitMachine(T_machine*);
    virtual void visitBetree(T_betree*);
    virtual void visitSymbol(T_symbol*);
    virtual void visitLexem(T_lexem*);
    virtual void visitOperation(T_op*);
    virtual void visitSkip(T_skip*);
    virtual void visitBegin(T_begin*);
    virtual void visitUsedOp(T_used_op*);
    virtual void visitChoice(T_choice*);
    virtual void visitOr(T_or*);
    virtual void visitPrecond(T_precond*);
    virtual void visitAssert(T_assert*);
    virtual void visitIf(T_if*);
    virtual void visitElse(T_else*);
    virtual void visitSelect(T_select*);
    virtual void visitWhen(T_when*);
    virtual void visitPredicate(T_predicate*);
    virtual void visitExpr(T_expr*);
    virtual void visitCase(T_case*);
    virtual void visitCaseItem(T_case_item*);
    virtual void visitListLink(T_list_link*);
    virtual void visitLet(T_let*);
    virtual void visitValuation(T_valuation*);
    virtual void visitIdent(T_ident*);
    virtual void visitVar(T_var*);
    virtual void visitWhile(T_while*);
    virtual void visitAny(T_any*);
    virtual void visitAffect(T_affect*);
    virtual void visitBecomesMemberOf(T_becomes_member_of*);
    virtual void visitBecomesSuchThat(T_becomes_such_that*);
    virtual void visitOpCall(T_op_call*);
    virtual void visitComment(T_comment*);
    virtual void visitFlag(T_flag*);
    virtual void visitDefinition(T_definition*);
    virtual void visitUsedMachine(T_used_machine*);
    virtual void visitLiteralInteger(T_literal_integer*);
    virtual void visitBinaryOp(T_binary_op*);
    virtual void visitExprWithParenthesis(T_expr_with_parenthesis*);
    virtual void visitUnaryOp(T_unary_op*);
    virtual void visitArrayItem(T_array_item*);
    virtual void visitConverse(T_converse*);
    virtual void visitLiteralString(T_literal_string*);
    virtual void visitKeyword(T_keyword*);
    virtual void visitLambda(T_lambda_expr*);
    virtual void visitBinaryPredicate(T_binary_predicate*);
    virtual void visitTypingPredicate(T_typing_predicate*);
    virtual void visitComprehensiveSet(T_comprehensive_set*);
    virtual void visitImage(T_image*);
    virtual void visitPredicateWithParenthesis(T_predicate_with_parenthesis*);
    virtual void visitExtensiveSet(T_extensive_set*);
    virtual void visitNotPredicate(T_not_predicate*);
    virtual void visitExprPredicate(T_expr_predicate*);
    virtual void visitUniversalpredicate(T_universal_predicate*);
    virtual void visitExistentialPredicate(T_existential_predicate*);
    virtual void visitEmptySeq(T_empty_seq*);
    virtual void visitEmptySet(T_empty_set*);
    virtual void visitSIGMA(T_sigma*);
    virtual void visitPI(T_pi*);
    virtual void visitQuantifiedUnion(T_quantified_union*);
    virtual void visitQuantifiedIntersection(T_quantified_intersection*);
    virtual void visitLiteralBoolean(T_literal_boolean*);
    virtual void visitLiteralEnumeratedValue(T_literal_enumerated_value*);
    virtual void visitLiteralReal(T_literal_real*);
    virtual void visitRelation(T_relation*);
    virtual void visitRenamedIdent(T_renamed_ident*);
    virtual void visitExtensiveSeq(T_extensive_seq*);
    virtual void visitGeneralisedUnion(T_generalised_union*);
    virtual void visitgeneralisedIntersection(T_generalised_intersection*);
    virtual void visitObject(T_object *o);
    virtual void visitItem(T_item*);
    virtual void visitSubstitution(T_substitution*);
    virtual void visitInteger(T_integer*);
    virtual void visitBetreeManager(T_betree_manager*);
    virtual void visitBetreeList(T_betree_list*);
    virtual void visitType(T_type*);
    virtual void visitProductType(T_product_type*);
    virtual void visitSetOfType(T_setof_type*);
    virtual void visitAbstractType(T_abstract_type*);
    virtual void visitEnumeratedType(T_enumerated_type*);
    virtual void visitPredefinedType(T_predefined_type*);
    virtual void visitBaseType(T_base_type*);
    virtual void visitConstructorType(T_constructor_type*);
    virtual void visitGenericType(T_generic_type*);
    virtual void visitGenericOp(T_generic_op*);
    virtual void visitBound(T_bound*);
    virtual void visitOpResult(T_op_result*);
    virtual void visitPragma(T_pragma*);
    virtual void visitPragmaLexem(T_pragma_lexem*);
    virtual void visitB0Type(T_B0_type*);
    virtual void visitB0BaseType(T_B0_base_type*);
    virtual void visitB0AbstractType(T_B0_abstract_type*);
    virtual void visitB0EnumeratedType(T_B0_enumerated_type*);
    virtual void visitB0IntervalType(T_B0_interval_type*);
    virtual void visitB0ArrayType(T_B0_array_type*);
    virtual void visitRecord(T_record*);
    virtual void visitRecordItem(T_record_item*);
    virtual void visitStruct(T_struct*);
    virtual void visitRecordAccess(T_record_access*);
    virtual void visitRecordType(T_record_type*);
    virtual void visitLabelType(T_label_type*);
    virtual void visitB0RecordType(T_B0_record_type*);
    virtual void visitB0LabelType(T_B0_label_type*);
    virtual void visitMsgLine(T_msg_line*);
    virtual void visitMsgLineManager(T_msg_line_manager*);
    virtual void visitComponent(T_comment*);
    virtual void visitFileComponent(T_file_component*);
    virtual void visitProject(T_project*);
    virtual void visitProjectManager(T_project_manager*);
    virtual void visitFileDefinition(T_file_definition*);
    virtual void visitComponentChecksums(T_component_checksums*);
    virtual void visitOperationChecksums(T_operation_checksums*);

    virtual void visitWitness(T_witness*);
};

#endif // NODE_VISITOR_H
