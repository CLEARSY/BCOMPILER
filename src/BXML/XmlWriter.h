#ifndef _XmlWriter_H
#define _XmlWriter_H
#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <list>
#include <NodeVisitor.h>

class XmlStreamWriter;

class XmlWriter
    : public NodeVisitor
{
public:
    XmlWriter(const std::string& traceability, 
        std::ostream *output_stream = 0);
    ~XmlWriter();

    void setIndentSize(int size);
    void setIsEventB(bool value);

    /*!
     * \brief Tells if for a set SET_1 given in parameter to a machine, the
     * constraints SET_1 : FIN(INTEGER) should be added
     * \param value
     * If value is true, the constraints will be added
     */
    void setAddConstraintOnParamSet(bool value);

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
    virtual void visitComponent(T_component*);
    virtual void visitFileComponent(T_file_component*);
    virtual void visitProject(T_project*);
    virtual void visitProjectManager(T_project_manager*);
    virtual void visitFileDefinition(T_file_definition*);
    virtual void visitComponentChecksums(T_component_checksums*);
    virtual void visitOperationChecksums(T_operation_checksums*);

    virtual void visitWitness(T_witness*);


    /*!
     * Indicates whether positional attributes (line/col/file) should be written
     */
    bool writePositionAttributes() const;
    void setWritePositionAttributes(bool write_attributes);

    void setSemanticAnalysis(bool semanticAnalysis)
    { _semanticAnalysis = semanticAnalysis; }
    void setB0Check(bool b0Check)
    { _b0Check = b0Check; }

    static const int TYPE_REF_BOOL {0};
    static const int TYPE_REF_INTEGER {1};
    static const int TYPE_REF_POW_INTEGER {2};
    static const int TYPE_REF_POW_POW_INTEGER {3};

private:
    static const std::set<std::string> UNARY_OPERATORS;
    static const std::set<std::string> BINARY_OPERATORS;

    bool getPosition(T_item *item, std::string& file_name, int& line, int& column, int &size);

    bool getDefinitionPosition(T_item *item, std::string& file_name, int& line, int& column, int &size);

    void beginAttributes(bool& tag_opened);
    void endAttributes(bool tag_opened);

    bool isSetParam(T_ident *param) const;

    void flattenConjunction(T_predicate *pred, std::list<T_item*> &predicates);
    void flattenDisjunction(T_predicate *pred, std::list<T_item*> &predicates);

    void writeType(T_type *type);
    void writeItemAttributes(T_item *item);

    void writeConstraints(T_machine *mch);


    void writePrevSubstitutions(T_substitution *sub,T_substitution_link_type link_type);
    void writeSubstitutions(T_substitution *sub);
    void writeSubstitutionsElement(const std::string& element, T_substitution *sub, T_item *parent);

    void getItemList(T_item *list, std::vector<T_item*>& items);

    void writeElementItemList(const std::string element, T_item *item, T_item *parent);

    void writeQuantifiedPredicate(const std::string& type, T_ident *variables, T_predicate *predicate, T_item *parent);

    void writeBinaryExpression(const std::string& op, T_expr*left, T_expr *right, T_item *parent);
    void writeUnaryExpression(const std::string& op, T_item *exp, T_item *parent);
    void writeNaryExpression(const std::string &op, T_item *exp_list, T_item *attributes_item);

    void writeWhen(T_predicate *condition, T_substitution *body);

    void writeQuantifiedExp(const std::string& operator_string,
                            T_ident *identifiers,
                            T_predicate *predicate,
                            T_expr *expr,
                            T_item *parent);

    void writeRecordItems(T_record_item *rec_item);

    void writeB0ArrayTypeSource(const std::vector<T_item*>& expressions);
    void writeLeftMapletList(const std::vector<T_item*>& expressions, const T_binary_operator separator);

    void writeIfThen(T_predicate* condition, T_substitution *then_substitution, T_else *else_sub);


    void writeSets(T_ident *sets, T_item *sets_clause);

    void writeValues(T_valuation *values, T_item *parent);

    void writePromotedOpList(T_used_op *list, T_item *parent);

    void writeElementItem(const std::string& element_name, T_item *item, T_item *parent);

    bool isConjunction(T_predicate *predicate) const;
    bool isDisjunction(T_predicate *predicate) const;

    void writeConjunction(T_predicate *predicate);
    void writeDisjunction(T_predicate *predicate);

    void writeBuiltinType(T_builtin_type builtin_type);

    void beginIdentElement(T_ident *ident);
    void endIdentElement();

    void writeTypeInfos();


    T_ident *getIdent(T_expr *exp, const char *error_message) const;

    std::string getIdentValue(T_ident* ident) const;
    std::string getString(T_symbol *symbol) const;


    XmlStreamWriter *writer;
    std::stringstream *typeInfoStream;
    XmlStreamWriter *typeInfoWriter;
    std::vector<std::string> typeInfoDict;

    std::string machineFileName;
    std::string _traceability;
    bool positionAttributes;

    bool writeAttributes;

    bool _semanticAnalysis;
    bool _b0Check;
    bool isEventB;
    bool add_constraint_on_param_set_;
};

#endif /* _XmlWriter_H */
