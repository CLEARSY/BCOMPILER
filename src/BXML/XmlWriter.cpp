#include <algorithm>
#include <exception>
#include <sstream>
#include <set>
#include <locale>
#include "XmlWriter.h"
#include "XmlStreamWriter.h"

#define NOT_IMPLEMENTED(x) throw XmlException("NOT IMPLEMENTED: XmlWriter::" x)

#define NEVER_CALLED(x) throw XmlException("XmlWriter::" x " should never be called")

const std::set<std::string> XmlWriter::UNARY_OPERATORS = {
    "max",
    "min",
    "card",
    "dom",
    "ran",
    "POW",
    "POW1",
    "FIN",
    "FIN1",
    "union",
    "inter",
    "seq",
    "seq1",
    "iseq",
    "iseq1",
    "size",
    "conc",
    "perm",

    "first",
    "last",
    "id",
    "closure",
    "closure1",

    "tail",
    "front",
    "rev",

    "struct",
    "rec",

    "succ",
    "pred",
    "rel",
    "fnc",

    "rmax",
    "rmin",
    "real",
    "floor",
    "ceiling",

    "tree",
    "btree",
    "top",
    "sons",
    "prefix",
    "postfix",
    "sizet",
    "mirror",
    "left",
    "right",
    "infix",
    //"bin", bin is treated elsewhere as it is also a ternary operator
};

const std::set<std::string> XmlWriter::BINARY_OPERATORS = {
    "prj1",
    "prj2",
    "iterate",
    "const",
    "rank",
    "father",
    "subtree",
    "arity",
};

bool XmlWriter::writePositionAttributes() const
{
    return positionAttributes;
}

void XmlWriter::setWritePositionAttributes(bool write_attributes)
{
    positionAttributes = write_attributes;
}

XmlWriter::XmlWriter(const std::string& traceability, std::ostream *output_stream)
    : writer(new XmlStreamWriter(output_stream)),
    _traceability(traceability),
    positionAttributes(true),
    writeAttributes(true),
    typeInfoStream(new std::stringstream()),
    typeInfoWriter(new XmlStreamWriter(typeInfoStream)),
    _semanticAnalysis(false),
    _b0Check(false)
{
    writer->setIndentString("");

    typeInfoWriter->setIndentString("");
    typeInfoWriter->beginElement("TypeInfos");
    typeInfoDict.push_back("<Id value='BOOL'/>");
    typeInfoDict.push_back("<Id value='INTEGER'/>");
    typeInfoDict.push_back("<Unary_Exp op='POW'>\n<Id value='INTEGER'/>\n</Unary_Exp>");
    typeInfoDict.push_back("<Unary_Exp op='POW'>\n<Unary_Exp op='POW'>\n<Id value='INTEGER'/>\n</Unary_Exp>\n</Unary_Exp>");

    typeInfoWriter->beginElement("Type");
    typeInfoWriter->writeAttribute("id", TYPE_REF_BOOL);
    typeInfoWriter->writeXml("<Id value='BOOL'/>");
    typeInfoWriter->endElement("Type");

    typeInfoWriter->beginElement("Type");
    typeInfoWriter->writeAttribute("id",TYPE_REF_INTEGER);
    typeInfoWriter->writeXml("<Id value='INTEGER'/>");
    typeInfoWriter->endElement("Type");

    typeInfoWriter->beginElement("Type");
    typeInfoWriter->writeAttribute("id",TYPE_REF_POW_INTEGER);
    typeInfoWriter->beginElement("Unary_Exp");
    typeInfoWriter->writeAttribute("op","POW");
    typeInfoWriter->writeXml("<Id value='INTEGER'/>");
    typeInfoWriter->endElement("Unary_Exp");
    typeInfoWriter->endElement("Type");

    typeInfoWriter->beginElement("Type");
    typeInfoWriter->writeAttribute("id",TYPE_REF_POW_POW_INTEGER);
    typeInfoWriter->beginElement("Unary_Exp");
    typeInfoWriter->writeAttribute("op","POW");
    typeInfoWriter->beginElement("Unary_Exp");
    typeInfoWriter->writeAttribute("op","POW");
    typeInfoWriter->writeXml("<Id value='INTEGER'/>");
    typeInfoWriter->endElement("Unary_Exp");
    typeInfoWriter->endElement("Unary_Exp");
    typeInfoWriter->endElement("Type");

}

XmlWriter::~XmlWriter()
{ }

void XmlWriter::setIndentSize(int size)
{
    std::string indent(size, ' ');
    writer->setIndentString(indent);
    typeInfoWriter->setIndentString(indent);
}

void XmlWriter::setIsEventB(bool value)
{
    isEventB = value;
}

void XmlWriter::setAddConstraintOnParamSet(bool value) {
  add_constraint_on_param_set_ = value;
}

std::string XmlWriter::getString(T_symbol *symbol) const
{
    std::string result;
    if(symbol)
    {
        result = symbol->get_value();
    }

    return result;
}

std::string XmlWriter::getIdentValue(T_ident *ident) const
{
    std::string result;
    if(ident)
    {
        return getString(ident->get_name());
    }

    return result;
}

T_ident *XmlWriter::getIdent(T_expr *exp, const char *error_message) const
{
    T_ident *result = dynamic_cast<T_ident*>(exp);

    if(result)
    {
        return result;
    }
    else
    { // LCOV_EXCL_START
        throw XmlException(error_message);
    } // LCOV_EXCL_STOP
}

void XmlWriter::getItemList(T_item *list, std::vector<T_item *> &items)
{
    while(list)
    {
        items.push_back(list);

        list = list->get_next();
    }
}

void XmlWriter::beginAttributes(bool &tag_opened)
{
    if(!tag_opened)
    {
        tag_opened = true;
        writer->beginElement("Attr");
    }
}

void XmlWriter::endAttributes(bool tag_opened)
{
    if(tag_opened)
    {
        writer->endElement("Attr");
    }
}


bool XmlWriter::getDefinitionPosition(T_item *item, std::string& file_name, int& line, int& column, int &size)
{
    file_name ="";
    line = -1;
    column = -1;

    if(item)
    {
        T_lexem *lexem = item->get_ref_lexem();
        if(lexem)
        {

            // It comes from a defintion
            if(lexem->get_original_lexem())
            {
                T_lexem * def  = lexem;
                while(def->get_original_lexem())
                {
                    def = def->get_original_lexem();
                }


                // get file name
                T_symbol *file_name_symb = def->get_file_name();
                if(file_name_symb)
                {
                    file_name = file_name_symb->get_value();
                }

                line = def->get_file_line();
                column = def->get_file_column();
                size = def->get_value_len();
                if(size == 0)
                {
                    size = def->get_lex_ascii_len();
                }
                return true;
            }
        }
    }

    return false;
}

bool XmlWriter::getPosition(T_item *item, std::string &file_name, int &line, int &column, int& size)
{
    file_name = "";
    line = -1;
    column = -1;
    if(item)
    {
        T_lexem *lexem = item->get_ref_lexem();
        if(lexem)
        {
            // get file name
            T_symbol *file_name_symb = lexem->get_file_name();
            if(file_name_symb)
            {
                file_name = file_name_symb->get_value();
            }

            line = lexem->get_file_line();
            column = lexem->get_file_column();
            size = lexem->get_value_len();
            if(size == 0)
            {
                size = lexem->get_lex_ascii_len();
            }
            return true;
        }
    }

    return false;
}

void XmlWriter::writeType(T_type *type){
    if(_semanticAnalysis){
        std::stringstream ss;
        XmlStreamWriter xsw(&ss);
        xsw.setIndentString("");
        int pos = -1; //can use long instead
        int i = 0;

        //Write the type info in a stream
        auto backup = writer;
        writer = &xsw;
        visit(type);
        writer = backup;
        std::string str = ss.str();
        auto itr = std::find(typeInfoDict.begin(),
                typeInfoDict.end(),
                str);
        if(itr == typeInfoDict.end()) {
            pos = typeInfoDict.size();
            typeInfoDict.push_back(std::move(str));
            typeInfoWriter->beginElement("Type");
            typeInfoWriter->writeAttribute("id", pos);
            typeInfoWriter->writeXml(ss.str());
            typeInfoWriter->endElement("Type");
        }
        else {
            pos = std::distance(typeInfoDict.begin(), itr);
        }
        writer->writeAttribute("typref", pos);
    }
}

void XmlWriter::writeItemAttributes(T_item *item)
{
    if(item && writeAttributes)
    {
        // inhibits writing of other attributes to prevent loops
        writeAttributes = false;

        //***** Part where real tag's attributes are written *****//
        T_type *type = item->get_B_type();
        if(type)
            writeType(type);

        //***** Part where the Attr tag is wrote *****//
        bool tag_opened = false;

        if(writePositionAttributes())
        {
            int line,defLine;
            int column,defColumn;
            int size,defSize;



            std::string file_name,def_file_name;

            bool defPos = getDefinitionPosition(item,def_file_name,defLine,defColumn,defSize);

            if(getPosition(item, file_name, line, column, size))
            {
                beginAttributes(tag_opened);

                writer->beginElement("Pos");
                if(file_name != machineFileName)
                {
                    writer->writeAttribute("f", file_name);
                }

                writer->writeAttribute("l", line);
                writer->writeAttribute("c", column);
                writer->writeAttribute("s", size);

                T_op * oper = dynamic_cast<T_op *>(item);
                if(oper)
                {
                    if(oper->get_body()->hasEndKeyword())
                    {
                        T_item * endWord = oper->get_body()->getFlagForEndKeyword();
                        if(endWord)
                        {
                            int endLine,endColumn,endSize;
                            std::string end_file_name;
                            getPosition(endWord,end_file_name,endLine,endColumn,endSize);

                            writer->writeAttribute("endLine",endLine);
                        }
                    }
                }

                if(defPos)
                {
                    writer->writeAttribute("expanded","yes");
                }

                writer->endElement();

                if(defPos)
                {
                    writer->beginElement("Pos");
                    if(file_name != machineFileName)
                    {
                        writer->writeAttribute("f", def_file_name);
                    }

                    writer->writeAttribute("l", defLine);
                    writer->writeAttribute("c", defColumn);
                    writer->writeAttribute("s", defSize);

                    writer->endElement();
                }
            }
        }

        T_ident *id = dynamic_cast<T_ident*>(item);
        if(id)
        {
            T_B0_type *b0_type = id->get_B0_type();
            if(b0_type)
            {
                beginAttributes(tag_opened);

                writer->beginElement("B0Type");
                visit(b0_type);
                writer->endElement("B0Type");
            }
        }

        endAttributes(tag_opened);

        // Restores the writing of attributes
        writeAttributes = true;
    }
}

void XmlWriter::writeSets(T_ident *sets, T_item *sets_clause)
{
    if(sets) {
        //Place the cursor on the first ident not inherited
        while(sets != NULL)
        {
            if(sets->get_inherited() == FALSE)
                break;
            sets = static_cast<T_ident*>(sets->get_next());
        }
    }
    if(sets)
    {
        writer->beginElement("Sets");
        writeItemAttributes(sets_clause);

        while(sets != NULL)
        {
            if(sets->get_inherited() == FALSE)
            {
                T_expr *expr = sets->make_expr();
                T_ident *set_ident = getIdent(expr, "XmlWriter: writeSets: unhandled expression type");

                T_expr * enumerated_values = set_ident->get_values();

                writer->beginElement("Set");
                //writeItemAttributes(sets);
                visit(set_ident);

                if(enumerated_values)
                {
                    writer->beginElement("Enumerated_Values");

                    while(enumerated_values)
                    {
                        T_ident *enum_ident = getIdent(enumerated_values, "XmlWriter: writeSets: unhandled enum type");
                        visit(enum_ident);

                        enumerated_values = static_cast<T_expr*>(enumerated_values->get_next());
                    }

                    writer->endElement("Enumerated_Values");
                }

                writer->endElement("Set");
            }

            sets = static_cast<T_ident*>(sets->get_next());
        }

        writer->endElement("Sets");
    }
}

void XmlWriter::writePromotedOpList(
        T_used_op *list,
        T_item *parent)
{
    if(list)
    {
        writer->beginElement("Promotes");
        writeItemAttributes(parent);

        while(list)
        {
            std::string operation_name = getIdentValue(list->get_name());

            writer->beginElement("Promoted_Operation");
            writeItemAttributes(list);

            writer->writeText(operation_name);
            writer->endElement("Promoted_Operation");

            list = static_cast<T_used_op*>(list->get_next());
        }

        writer->endElement("Promotes");
    }
}

void XmlWriter::writeElementItem(const std::string &element_name, T_item *item, T_item *parent)
{
    if(item)
    {
        writer->beginElement(element_name);
        writeItemAttributes(parent);
        visit(item);
        writer->endElement(element_name);
    }
}

void XmlWriter::writePrevSubstitutions(T_substitution *sub,T_substitution_link_type link_type)
{
    T_substitution* current = sub;
    if(sub->get_prev())
    {
        sub = static_cast<T_substitution*>(sub->get_prev());
        T_substitution_link_type prev_link_type = sub->get_link_type();
        if (link_type == prev_link_type) {
            writePrevSubstitutions(sub,link_type);
            visit(current);
        } else {
            writer->beginElement("Nary_Sub");
            writer->writeAttribute("op", prev_link_type == LINK_PARALLEL ? "||" : ";");
            writePrevSubstitutions(sub,prev_link_type);
            visit(current);
            writer->endElement("Nary_Sub");
        }
    } else {
        visit(current);
    }
}

void XmlWriter::writeSubstitutions(T_substitution *sub)
{
    if(sub)
    {
        if(sub->get_next())
        {
            // Several subtitutions
            while (sub->get_next()) {
                sub = static_cast<T_substitution*>(sub->get_next());
            }
            T_substitution* current = sub;
            sub = static_cast<T_substitution*>(sub->get_prev());
            T_substitution_link_type link_type = sub->get_link_type();
            writer->beginElement("Nary_Sub");
            writer->writeAttribute("op", link_type == LINK_PARALLEL ? "||" : ";");
            writePrevSubstitutions(sub,link_type);
            visit(current);
            writer->endElement("Nary_Sub");
        }
        else
        {
            visit(sub);
        }
    }
}

void XmlWriter::writeSubstitutionsElement(const std::string &element, T_substitution *sub, T_item *parent)
{
    if(sub)
    {
        writer->beginElement(element);
        writeItemAttributes(parent);

        writeSubstitutions(sub);
        writer->endElement(element);
    }
}

void XmlWriter::writeValues(T_valuation *values, T_item *parent)
{
    if(values)
    {
        writer->beginElement("Values");
        writeItemAttributes(parent);

        while(values)
        {
            visit(values);

            values = static_cast<T_valuation*>(values->get_next());
        }

        writer->endElement("Values");
    }
}

void XmlWriter::writeElementItemList(const std::string element, T_item *item, T_item *parent)
{
    bool listOfIdent = true;
    if(item)
    {
        //Explore the list of item to know if it contains only idents.
        T_item *explore_item = item;
        while(explore_item)
        {
            if(explore_item->get_node_type() != NODE_IDENT && explore_item->get_node_type() != NODE_RENAMED_IDENT)
                listOfIdent = false;
            explore_item = explore_item->get_next();
        }
        //If it's a ident list, go to the first not inherited ident.
        if(listOfIdent) {
            while(item)
            {
                if(((T_ident*)item)->get_inherited() == FALSE)
                    break;
                item = item->get_next();
            }
        }
    }
    //Write items if they exist.
    if(item)
    {
        writer->beginElement(element);
        writeItemAttributes(parent);

        while(item)
        {
            visit(item);

            item = item->get_next();
        }
        writer->endElement(element);
    }
}

bool XmlWriter::isSetParam(T_ident *param) const
{
    std::string param_name = getIdentValue(param);
    std::string::iterator e = param_name.end();
    for(std::string::iterator it = param_name.begin(); it != e; ++it)
    {
        if(std::islower(*it))
        {
            return false;
        }
    }

    return true;
}

void XmlWriter::writeConstraints(T_machine *mch)
{
    T_predicate *constraints = mch->get_constraints();
    std::list<T_ident*> sets_parameters;
    T_ident *current_param = mch->get_params();
    while(current_param)
    {
        if(isSetParam(current_param))
        {
            sets_parameters.push_back(current_param);
        }

        current_param = (T_ident*)current_param->get_next();
    }

    if(constraints || !sets_parameters.empty())
    {
        writer->beginElement("Constraints");
        writeItemAttributes(mch->get_constraints_clause());

        std::list<T_item*> constraint_items;
        flattenConjunction(constraints, constraint_items);

        writer->beginElement("Nary_Pred");
        writer->writeAttribute("op", "&");
        for(std::list<T_ident*>::iterator it = sets_parameters.begin(); it != sets_parameters.end(); ++it)
        {
            T_type * param_type = (*it)->get_B_type();
            // set : FIN(set)
            if (add_constraint_on_param_set_) {
                T_setof_type * set_type = new T_setof_type(param_type, nullptr, nullptr, nullptr);
                writer->beginElement("Exp_Comparison");
                writer->writeAttribute("op", ":");
                visit(*it);
                writer->beginElement("Unary_Exp");
                writer->writeAttribute("op", "FIN");
                writeType(set_type);
                visit(*it);
                writer->endElement("Unary_Exp");
                writer->endElement("Exp_Comparison");
            }
            // not(set = {})
            writer->beginElement("Unary_Pred");
            writer->writeAttribute("op", "not");
            writer->beginElement("Exp_Comparison");
            writer->writeAttribute("op", "=");
            visit(*it);
            writer->beginElement("EmptySet");
            writeType(param_type);
            writer->endElement("EmptySet");
            writer->endElement("Exp_Comparison");
            writer->endElement("Unary_Pred");
        }

        for(std::list<T_item*>::iterator it = constraint_items.begin(); it != constraint_items.end(); ++it)
        {
            visit(*it);
        }

        writer->endElement("Nary_Pred");
        writer->endElement("Constraints");
    }
}

void XmlWriter::visitMachine(T_machine*mch)
{
    if(mch)
    {
        // initialize machineFileName
        int l,c,s;
        getPosition(mch, machineFileName, l, c, s);

        writer->beginDocument();
        if (!_traceability.empty()) {
            writer->writeComment(_traceability);
        }
        writer->beginElement("Machine");
        writer->writeAttribute("xmlns", "https://www.atelierb.eu/Formats/bxml");
        writer->writeAttribute("version", "1.0");

        writer->writeAttribute("name", getIdentValue(mch->get_machine_name()));

        std::string machine_type;
        T_machine_type type = mch->get_machine_type();
        switch(type)
        {
            case MTYPE_SPECIFICATION:
            case MTYPE_SYSTEM:
                machine_type = "abstraction";
                break;
            case MTYPE_REFINEMENT:
                machine_type = "refinement";
                break;
            case MTYPE_IMPLEMENTATION:
                machine_type = "implementation";
                break;
        }
        writer->writeAttribute("type", machine_type);
        writer->writeAttribute("position",
                positionAttributes ? "true" : "false");
        writer->writeAttribute("semantic",
                _semanticAnalysis ? "true" : "false");
        writer->writeAttribute("b0check",
                _b0Check ? "true" : "false");

        T_ident *abstraction_name = mch->get_abstraction_name();
        if(abstraction_name)
        {
            writer->beginElement("Abstraction");
            writer->writeText(getIdentValue(abstraction_name));
            writer->endElement("Abstraction");
        }

        // Parameters
        writeElementItemList("Parameters", mch->get_params(), 0);

        writeConstraints(mch);

        // Architecture clauses
        writeElementItemList("Includes", mch->get_includes(), mch->get_includes_clause());
        writeElementItemList("Imports", mch->get_imports(), mch->get_imports_clause());
        writeElementItemList("Sees", mch->get_sees(), mch->get_sees_clause());
        writeElementItemList("Extends", mch->get_extends(), mch->get_extends_clause());
        writeElementItemList("Uses", mch->get_uses(), mch->get_uses_clause());

        writePromotedOpList(mch->get_promotes(), mch->get_promotes_clause());

        writeValues(mch->get_values(), mch->get_values_clause());
        writeSets(mch->get_sets(), mch->get_sets_clause());

        writeElementItemList("Abstract_Constants", mch->get_abstract_constants(), mch->get_abstract_constants_clause());
        writeElementItemList("Concrete_Constants", mch->get_concrete_constants(), mch->get_concrete_constants_clause());

        writeElementItemList("Abstract_Variables", mch->get_abstract_variables(), mch->get_abstract_variables_clause());
        writeElementItemList("Concrete_Variables", mch->get_concrete_variables(), mch->get_concrete_variables_clause());

        writeElementItem("Properties", mch->get_properties(), mch->get_properties_clause());
        writeElementItem("Invariant", mch->get_invariant(), mch->get_invariant_clause());
        writeElementItem("Variant", mch->get_variant(), mch->get_variant_clause());

        writeSubstitutionsElement("Initialisation", mch->get_initialisation(), mch->get_initialisation_clause());
        writeElementItemList("Assertions", mch->get_assertions(), mch->get_assertions_clause());
        writeElementItemList("Local_Operations", mch->get_local_operations(), mch->get_local_operations_clause());
        writeElementItemList("Operations", mch->get_operations(), mch->get_local_operations_clause());

        writeTypeInfos();

        writer->endElement("Machine");
        writer->endDocument();
    }
}

void XmlWriter::visitBetree(T_betree* betree)
{
    T_machine *machine = betree->get_root();

    visitMachine(machine);
}

// LCOV_EXCL_START
void XmlWriter::visitSymbol(T_symbol*)
{
    NEVER_CALLED("visitSymbol");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitLexem(T_lexem*)
{
    NEVER_CALLED("visitLexem");
}
// LCOV_EXCL_STOP

void XmlWriter::visitOperation(T_op*operation)
{
    std::string op_name = getIdentValue(operation->get_name());
    writer->beginElement("Operation");
    writer->writeAttribute("name", op_name);
    writeItemAttributes(operation);

    writeElementItemList("Refines", operation->get_ref_operation(), 0);

    writeElementItemList("Output_Parameters", operation->get_out_params(), 0);
    writeElementItemList("Input_Parameters", operation->get_in_params(), 0);

    T_predicate    *precond = 0;
    T_substitution *body = operation->get_body();
    T_node_type body_type = body->get_node_type();
    T_begin *begin_node = 0;
    switch(body_type)
    {
        case NODE_PRECOND:
            {
                T_precond *precond_node = static_cast<T_precond*>(body);
                precond = precond_node->get_predicate();
                body = precond_node->get_body();
                break;
            }
        case NODE_BEGIN:
            {
                begin_node = static_cast<T_begin*>(body);
                body = begin_node->get_body();
                break;
            }
        default:
            // Keep things unchanged
            break;
    }

    writeElementItem("Precondition", precond, 0);
    if (body_type == NODE_BEGIN && isEventB) {
        writer->beginElement("Body");
        writeItemAttributes(begin_node);
        writeSubstitutionsElement("Bloc_Sub", body, 0);
        writer->endElement("Body");
    } else {
        writeSubstitutionsElement("Body", body, 0);
    }

    writer->endElement();
}

void XmlWriter::visitSkip(T_skip *skip)
{
    writer->beginElement("Skip");
    writeItemAttributes(skip);
    writer->endElement();
}

void XmlWriter::visitBegin(T_begin *begin)
{
    writeSubstitutionsElement("Bloc_Sub", begin->get_body(), begin);
}

// LCOV_EXCL_START
void XmlWriter::visitUsedOp(T_used_op*)
{
    NEVER_CALLED("visitUsedOp");
}
// LCOV_EXCL_STOP

void XmlWriter::writeB0ArrayTypeSource(const std::vector<T_item *> &expressions)
{

    int count = expressions.size();
    --count;

    for(int i=0; i<count; ++i)
    {
        writer->beginElement("Binary_Exp");
        writer->writeAttribute("op", "*");

        visit(expressions[i]);
    }
    visit(expressions[count]);
    for(int i=0; i<count; ++i)
    {
        // Closes Binary_Substitution
        writer->endElement("Binary_Exp");
    }
}

void XmlWriter::writeLeftMapletList(const std::vector<T_item *> &expressions, const T_binary_operator separator)
{
    using std::vector;

    assert(expressions.size() > 0);
    //int count = expressions.size() - 1; // number of maplet expressions to build
    //int i, j;
    T_type *type = expressions[0]->get_B_type();

    vector<T_type*> types;
    int i = 1;
    while(type && i<expressions.size()) {
        T_type *type2 = expressions[i]->get_B_type();
        if(type2){
            type = new T_product_type(type,type2, nullptr, nullptr, nullptr);
            types.push_back(type);
        } else {
            type = nullptr;
        }
        ++i;
    }
    for(int j = expressions.size()-2; j>=0; --j) {
        writer->beginElement("Binary_Exp");
        writer->writeAttribute("op", "|->");
        if (separator == BOP_MAPLET)
            writer->writeAttribute("normalized", "false");
        else
            writer->writeAttribute("normalized", "true");
        if(type){
            writeType(types[j]);
        }
    }

    visit(expressions[0]);
    for(int i = 1; i < expressions.size(); ++i)
    {
        visit(expressions[i]);
        writer->endElement("Binary_Exp");
    }

    for(T_type *type : types)
        delete type;
}


void XmlWriter::visitChoice(T_choice*choice_substitution)
{
    writer->beginElement("Nary_Sub");
    writer->writeAttribute("op", "CHOICE");
    writeItemAttributes(choice_substitution);

    writeSubstitutions(choice_substitution->get_body());

    T_or *or_substitution = choice_substitution->get_or();

    while(or_substitution)
    {
        writeSubstitutions(or_substitution->get_body());

        or_substitution = static_cast<T_or*>(or_substitution->get_next());
    }

    writer->endElement("Nary_Sub");
}

// LCOV_EXCL_START
void XmlWriter::visitOr(T_or*)
{
    NEVER_CALLED("visitOr");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitPrecond(T_precond *precondition)
{
    NEVER_CALLED("visitPrecond");
}
// LCOV_EXCL_STOP

void XmlWriter::visitAssert(T_assert* assert_sub)
{
    T_predicate *pred = assert_sub->get_predicate();
    T_substitution *body = assert_sub->get_body();

    writer->beginElement("Assert_Sub");
    writeItemAttributes(assert_sub);

    writeElementItem("Guard", pred, 0);
    writeSubstitutionsElement("Body", body, 0);

    writer->endElement("Assert_Sub");
}

void XmlWriter::writeIfThen(T_predicate *condition, T_substitution *then_substitution, T_else *current_else)
{
    writeElementItem("Condition", condition, 0);
    writeSubstitutionsElement("Then", then_substitution, 0);
    if(current_else)
    {
        T_else *next_else = static_cast<T_else*>(current_else->get_next());
        T_predicate *else_cond = current_else->get_cond();
        T_substitution *else_sub = current_else->get_body();

        writer->beginElement("Else");
        if(else_cond)
        {
            writer->beginElement("If_Sub");
            writer->writeAttribute("elseif", "yes");

            writeIfThen(else_cond, else_sub, next_else);
            writer->endElement("If_Sub");
        }
        else
        {
            writeSubstitutions(else_sub);
        }
        writer->endElement("Else");
    }
}

void XmlWriter::visitIf(T_if *if_substitution)
{
    writer->beginElement("If_Sub");
    writer->writeAttribute("elseif", "no");
    writeItemAttributes(if_substitution);

    writeIfThen(if_substitution->get_cond(), if_substitution->get_then_body(), if_substitution->get_else());

    writer->endElement("If_Sub");
}

// LCOV_EXCL_START
void XmlWriter::visitElse(T_else*)
{
    NEVER_CALLED("visitElse");
}
// LCOV_EXCL_STOP

void XmlWriter::writeWhen(T_predicate *condition, T_substitution *body)
{
    writer->beginElement("When");
    writeElementItem("Condition", condition, 0);
    writeSubstitutionsElement("Then", body, 0);
    writer->endElement();
}

void XmlWriter::visitSelect(T_select*select)
{
    writer->beginElement("Select");
    writeItemAttributes(select);

    writer->beginElement("When_Clauses");
    writeWhen(select->get_cond(), select->get_then_body());

    T_when *else_when = 0;

    T_when *current = select->get_when();
    while(current)
    {
        if(current->get_cond())
        {
            writeWhen(current->get_cond(), current->get_body());
        }
        else
        {
            if(else_when)
            { // LCOV_EXCL_START
                throw XmlException("Two ELSE clauses encountered in SELECT substitution");
            } // LCOV_EXCL_STOP
            else_when = current;
        }
        current = static_cast<T_when*>(current->get_next());
    }
    writer->endElement("When_Clauses");


    if(else_when)
    {
        writeSubstitutionsElement("Else", else_when->get_body(), 0);
    }

    writer->endElement("Select");
}

// LCOV_EXCL_START
void XmlWriter::visitWhen(T_when*)
{
    NEVER_CALLED("visitWhen");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitPredicate(T_predicate*)
{
    NEVER_CALLED("visitPredicate");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitExpr(T_expr*)
{
    NEVER_CALLED("visitExpr");
}
// LCOV_EXCL_STOP

void XmlWriter::visitCase(T_case *case_sub)
{
    writer->beginElement("Case_Sub");
    writeItemAttributes(case_sub);

    writeElementItem("Value", case_sub->get_case_select(), 0);
    writer->beginElement("Choices");
    T_case_item *case_item = case_sub->get_case_items();
    T_case_item *else_case = 0;

    while(case_item)
    {
        if(case_item->get_literal_values())
        {
            visit(case_item);
        }
        else
        {
            else_case = case_item;
        }

        case_item = static_cast<T_case_item*>(case_item->get_next());
    }
    writer->endElement("Choices");

    if(else_case)
    {
        writeSubstitutionsElement("Else", else_case->get_body(), 0);
    }

    writer->endElement("Case_Sub");
}

void XmlWriter::visitCaseItem(T_case_item *case_item)
{
    writer->beginElement("Choice");
    writeItemAttributes(case_item);

    T_item * values = case_item->get_literal_values();
    while (values) {
        writeElementItem("Value", values, 0);
        values = static_cast<T_item*>(values->get_next());
    }
    writeSubstitutionsElement("Then", case_item->get_body(), 0);
    writer->endElement();
}

void XmlWriter::visitListLink(T_list_link *link)
{
    visit(link->follow_indirection());
}

void XmlWriter::visitLet(T_let *let_substitution)
{
    writer->beginElement("LET_Sub");
    writeItemAttributes(let_substitution);

    writeElementItemList("Variables", let_substitution->get_lvalues(), 0);

    writeValues(let_substitution->get_valuations(), 0);

    writeSubstitutionsElement("Then", let_substitution->get_body(), 0);
    writer->endElement();
}

void XmlWriter::visitValuation(T_valuation *valuation)
{
    writer->beginElement("Valuation");
    std::string constant_name = getIdentValue(valuation->get_ident());
    writer->writeAttribute("ident", constant_name);
    writeItemAttributes(valuation->get_ident());

    visit(valuation->get_value());

    writer->endElement();
}

void XmlWriter::beginIdentElement(T_ident *ident)
{
    writer->beginElement("Id");
    T_symbol *value = ident->get_base_name();
    writer->writeAttribute("value", getString(value));
    if(ident->has_suffix_number())
    {
        std::stringstream stream;
        stream << ident->get_suffix_number();
        std::string suffix_number = stream.str();
        writer->writeAttribute("suffix" , suffix_number);
    }
}

void XmlWriter::endIdentElement()
{
    writer->endElement("Id");
}

void XmlWriter::visitIdent(T_ident *ident)
{
    if(ident->get_inherited() == FALSE) {
        beginIdentElement(ident);
        writeItemAttributes(ident);
        endIdentElement();
    }
}

void XmlWriter::visitRenamedIdent(T_renamed_ident *rident)
{
    if(rident->get_inherited() == FALSE) {
        beginIdentElement(rident);
        writer->writeAttribute("instance", getString(rident->get_instance_name()));
        writer->writeAttribute("component", getString(rident->get_component_name()));
        writeItemAttributes(rident);
        endIdentElement();
    }
}

void XmlWriter::visitVar(T_var *var_substitution)
{
    writer->beginElement("VAR_IN");
    writeItemAttributes(var_substitution);

    writeElementItemList("Variables", var_substitution->get_identifiers(), 0);
    writeSubstitutionsElement("Body", var_substitution->get_body(), 0);
    writer->endElement();
}

void XmlWriter::visitWhile(T_while*while_sub)
{
    writer->beginElement("While");
    writeItemAttributes(while_sub);

    writeElementItem("Condition", while_sub->get_cond(), 0);
    writeSubstitutionsElement("Body", while_sub->get_body(), 0);
    writeElementItem("Invariant", while_sub->get_invariant(), 0);
    writeElementItem("Variant", while_sub->get_variant(), 0);
    writer->endElement();

}

void XmlWriter::visitAny(T_any*any_substitution)
{
    writer->beginElement("ANY_Sub");
    writeItemAttributes(any_substitution);

    writeElementItemList("Variables", any_substitution->get_identifiers(), 0);

    writer->beginElement("Pred");
    visit(any_substitution->get_where());
    writer->endElement("Pred");

    writeSubstitutionsElement("Then", any_substitution->get_body(), 0);

    writer->endElement();
}

void XmlWriter::visitAffect(T_affect *affect_substitution)
{
    writer->beginElement("Assignement_Sub");
    writeItemAttributes(affect_substitution);

    writer->beginElement("Variables");
    T_item *current = affect_substitution->get_lvalues();
    while(current)
    {
        visit(current);

        current = current->get_next();
    }
    writer->endElement("Variables");


    T_expr *current_value = affect_substitution->get_values();
    writer->beginElement("Values");
    while(current_value)
    {
        visit(current_value);

        current_value = static_cast<T_expr*>(current_value->get_next());
    }
    writer->endElement("Values");
    writer->endElement("Assignement_Sub");
}

void XmlWriter::visitBecomesMemberOf(T_becomes_member_of *bof)
{
    writer->beginElement("Becomes_In");
    writeItemAttributes(bof);

    writeElementItemList("Variables", bof->get_lvalues(), 0);

    writer->beginElement("Value");
    visit(bof->get_set());
    writer->endElement();

    writer->endElement();
}

void XmlWriter::visitBecomesSuchThat(T_becomes_such_that *bst)
{
    writer->beginElement("Becomes_Such_That");
    writeItemAttributes(bst);
    writeElementItemList("Variables", bst->get_lvalues(), 0);

    writeElementItem("Pred", bst->get_predicate(), 0);

    writer->endElement();
}

void XmlWriter::visitOpCall(T_op_call *op_call)
{
    writer->beginElement("Operation_Call");
    writeItemAttributes(op_call);

    writer->beginElement("Name");
    visit(op_call->get_op_name());
    writer->endElement("Name");

    T_item *in_param = op_call->get_in_params();
    if(in_param)
    {
        writer->beginElement("Input_Parameters");
        while(in_param)
        {
            visit(in_param);

            in_param = in_param->get_next();
        }
        writer->endElement("Input_Parameters");
    }

    T_item *out_param = op_call->get_out_params();
    if(out_param)
    {
        writer->beginElement("Output_Parameters");

        while(out_param)
        {
            visit(out_param);

            out_param = out_param->get_next();
        }
        writer->endElement("Output_Parameters");
    }

    writer->endElement("Operation_Call");
}

// LCOV_EXCL_START
void XmlWriter::visitComment(T_comment*comment)
{
    NEVER_CALLED("visitComment");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitFlag(T_flag*)
{
    NEVER_CALLED("visitFlag");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitDefinition(T_definition*)
{
    NEVER_CALLED("visitDefinition");
}
// LCOV_EXCL_STOP

void XmlWriter::visitUsedMachine(T_used_machine *used_machine)
{
    writer->beginElement("Referenced_Machine");
    writeItemAttributes(used_machine);

    writer->beginElement("Name");
    writer->writeText(getString(used_machine->get_component_name()));
    writer->endElement("Name");

    T_symbol *instance_name = used_machine->get_instance_name();
    if(instance_name)
    {
        writer->beginElement("Instance");
        writer->writeText(getString(instance_name));
        writer->endElement("Instance");
    }

    writeElementItemList("Parameters", used_machine->get_use_params(), 0);
    writer->endElement("Referenced_Machine");
}

void XmlWriter::visitLiteralInteger(T_literal_integer*integer)
{
    writer->beginElement("Integer_Literal");
    T_integer *value = integer->get_value();
    std::string string_value = value->get_is_positive() ? "" : "-";
    string_value += value->get_value()->get_value();
    writer->writeAttribute("value", string_value);
    writeItemAttributes(integer);

    writer->endElement();
}

void XmlWriter::visitBinaryOp(T_binary_op *binary_op)
{
    T_binary_operator op_type = binary_op->get_operator();
    std::string op_type_string = "";
    switch(op_type)
    {
        case BOP_PLUS:
            {
                T_type *type = binary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type_string = "+i";
                    else if(type->is_a_real())
                        op_type_string = "+r";
                    else if(type->is_a_float())
                        op_type_string = "+f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type_string = "+";
                }
                break;
            }
        case BOP_MINUS:
            {
                T_type *type = binary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type_string = "-i";
                    else if(type->is_a_real())
                        op_type_string = "-r";
                    else if(type->is_a_float())
                        op_type_string = "-f";
                    else
                        op_type_string = "-s";
                }
                else {
                    op_type_string = "-";
                }
                break;
            }
        case BOP_TIMES:
            {
                T_type *type = binary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type_string = "*i";
                    else if(type->is_a_real())
                        op_type_string = "*r";
                    else if(type->is_a_float())
                        op_type_string = "*f";
                    else
                        op_type_string = "*s";
                }
                else {
                    op_type_string = "*";
                }
                break;
            }
        case BOP_DIVIDES:
            {
                T_type *type = binary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type_string = "/i";
                    else if(type->is_a_real())
                        op_type_string = "/r";
                    else if(type->is_a_float())
                        op_type_string = "/f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type_string = "/";
                }
                break;
            }
        case BOP_MOD:
            op_type_string = "mod";
            break;
        case BOP_POWER:
            {
                T_type *type = binary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type_string = "**i";
                    else if(type->is_a_real())
                        op_type_string = "**r";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type_string = "**";
                }
                break;
            }
        case BOP_RESTRICT:
            op_type_string = "<|";
            break;
        case BOP_ANTIRESTRICT:
            op_type_string = "<<|";
            break;
        case BOP_CORESTRICT:
            op_type_string = "|>";
            break;
        case BOP_ANTICORESTRICT:
            op_type_string = "|>>";
            break;
        case BOP_LEFT_OVERLOAD:
            op_type_string = "<+";
            break;
        case BOP_CONCAT:
            op_type_string = "^";
            break;
        case BOP_HEAD_INSERT:
            op_type_string = "->";
            break;
        case BOP_TAIL_INSERT:
            op_type_string = "<-";
            break;
        case BOP_HEAD_RESTRICT:
            op_type_string = "/|\\";
            break;
        case BOP_TAIL_RESTRICT:
            op_type_string = "\\|/";
            break;
        case BOP_INTERVAL:
            op_type_string = "..";
            break;
        case BOP_INTERSECTION:
            op_type_string = "/\\";
            break;
        case BOP_UNION:
            op_type_string = "\\/";
            break;
        case BOP_MAPLET:
            op_type_string = "|->";
            break;
        case BOP_SET_RELATION:
            op_type_string = "<->";
            break;
        case BOP_SURJ_RELATION:
            op_type_string = "<->>";
            break;
        case BOP_TOTAL_RELATION:
            op_type_string = "<<->";
            break;
        case BOP_TOTAL_SURJ_RELATION:
            op_type_string = "<<->>";
            break;
            //case BOP_CONSTANT_FUNCTION: // ????? cartesian product ?????
            //    op_type_string = "*";
            //    break;
        case BOP_COMPOSITION:
            op_type_string = ";";
            break;
        case BOP_DIRECT_PRODUCT:
            op_type_string = "><";
            break;
        case BOP_PARALLEL_PRODUCT:
            op_type_string = "||";
            break;
        case BOP_COMMA:
            op_type_string = ",";
            break;
            // LCOV_EXCL_START
        default:
            throw XmlException("visitBinaryOp: unknown operator type");
            // LCOV_EXCL_STOP
    }

    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", op_type_string);
    writeItemAttributes(binary_op);
    visit(binary_op->get_op1());
    visit(binary_op->get_op2());
    writer->endElement();
}

void XmlWriter::visitExprWithParenthesis(T_expr_with_parenthesis*exp)
{
    visit(exp->get_expr());
}

void XmlWriter::writeNaryExpression(const std::string &op, T_item *exp_list, T_item *attributes_item)
{
    writer->beginElement("Nary_Exp");
    writer->writeAttribute("op", op);
    writeItemAttributes(attributes_item);

    while(exp_list)
    {
        visit(exp_list);
        exp_list = exp_list->get_next();
    }

    writer->endElement("Nary_Exp");
}

void XmlWriter::writeUnaryExpression(const std::string &op, T_item *exp, T_item *parent)
{
    writer->beginElement("Unary_Exp");
    writer->writeAttribute("op", op);
    writeItemAttributes(parent);

    visit(exp);
    writer->endElement();
}

void XmlWriter::visitUnaryOp(T_unary_op *unary_op)
{
    T_unary_operator op;
    op = unary_op->get_operator();
    std::string op_string;
    switch(op)
    {
        case UOP_MINUS:
            {
                T_type *type = unary_op->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_string = "-i";
                    else if(type->is_a_real())
                        op_string = "-r";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_string = "-";
                }
                break;
            }
            // LCOV_EXCL_START
        default:
            throw XmlException("Unknown unary operator");
            // LCOV_EXCL_STOP
    }

    writeUnaryExpression(op_string, unary_op->get_operand(), unary_op);
}



void XmlWriter::visitArrayItem(T_array_item *array_item)
{
    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", "(");
    writeItemAttributes(array_item);

    visit(array_item->get_array_name());

    std::vector<T_item*> indexes;
    getItemList(array_item->get_indexes(), indexes);

    writeLeftMapletList(indexes, array_item->get_separator());

    writer->endElement("Binary_Exp");
}

void XmlWriter::visitConverse(T_converse*converse_expression)
{
    writeUnaryExpression("~", converse_expression->get_expr(), converse_expression);
}

void XmlWriter::visitLiteralString(T_literal_string*string_literal)
{
    std::string value = string_literal->get_value()->get_value();

    writer->beginElement("STRING_Literal");
    writer->writeAttribute("value", value);
    writeItemAttributes(string_literal);
    writer->endElement();
}

// LCOV_EXCL_START
void XmlWriter::visitKeyword(T_keyword*)
{
    NEVER_CALLED("visitKeyword");
}
// LCOV_EXCL_STOP

void XmlWriter::visitLambda(T_lambda_expr*lambda)
{
    writeQuantifiedExp("%", lambda->get_variables(), lambda->get_predicate(), lambda->get_expr(), lambda);
}

bool XmlWriter::isConjunction(T_predicate *predicate) const
{
    if(predicate && predicate->get_node_type() == NODE_BINARY_PREDICATE)
    {
        T_binary_predicate *binary = static_cast<T_binary_predicate*>(predicate);

        return binary->get_predicate_type() == BPRED_CONJONCTION;
    }
    else
    {
        return false;
    }
}

void XmlWriter::flattenConjunction(T_predicate *pred, std::list<T_item *> &predicates)
{
    T_predicate *predicate = pred;

    while(predicate &&
            isConjunction(predicate))
    {
        T_binary_predicate *binary_pred = static_cast<T_binary_predicate*>(predicate);
        predicates.push_front(binary_pred->get_pred2());

        predicate = binary_pred->get_pred1();
    }

    if(predicate)
    {
        predicates.push_front(predicate);
    }

}

bool XmlWriter::isDisjunction(T_predicate *predicate) const
{
    if(predicate && predicate->get_node_type() == NODE_BINARY_PREDICATE)
    {
        T_binary_predicate *binary = static_cast<T_binary_predicate*>(predicate);

        return binary->get_predicate_type() == BPRED_OR;
    }
    else
    {
        return false;
    }
}

void XmlWriter::flattenDisjunction(T_predicate *pred, std::list<T_item *> &predicates)
{
    T_predicate *predicate = pred;

    while(predicate &&
            isDisjunction(predicate))
    {
        T_binary_predicate *binary_pred = static_cast<T_binary_predicate*>(predicate);
        predicates.push_front(binary_pred->get_pred2());

        predicate = binary_pred->get_pred1();
    }

    if(predicate)
    {
        predicates.push_front(predicate);
    }

}
void XmlWriter::writeConjunction(T_predicate *predicate)
{
    std::list<T_item*> predicates;
    flattenConjunction(predicate, predicates);

    writer->beginElement("Nary_Pred");
    writer->writeAttribute("op", "&");

    for(std::list<T_item*>::iterator it = predicates.begin(); it != predicates.end(); ++it)
    {
        visit(*it);
    }
    writer->endElement("Nary_Pred");
}

void XmlWriter::writeDisjunction(T_predicate *predicate)
{
    std::list<T_item*> predicates;
    flattenDisjunction(predicate, predicates);

    writer->beginElement("Nary_Pred");
    writer->writeAttribute("op", "or");

    for(std::list<T_item*>::iterator it = predicates.begin(); it != predicates.end(); ++it)
    {
        visit(*it);
    }
    writer->endElement("Nary_Pred");
}

void XmlWriter::writeBuiltinType(T_builtin_type builtin_type)
{
    std::string type_name;

    switch (builtin_type)
    {
        case BTYPE_INTEGER:
            type_name = "INTEGER";
            break;
        case BTYPE_BOOL:
            type_name = "BOOL";
            break;
        case BTYPE_STRING:
            type_name = "STRING";
            break;
        case BTYPE_REAL:
            type_name = "REAL";
            break;
        case BTYPE_FLOAT:
            type_name = "FLOAT";
            break;
            // LCOV_EXCL_START
        default:
            throw XmlException("Unknown predefined type");
            // LCOV_EXCL_STOP
    }

    writer->beginElement("Id");
    writer->writeAttribute("value", type_name);

    writer->endElement();
}

void XmlWriter::visitBinaryPredicate(T_binary_predicate *binary_predicate)
{
    std::string predicate_operator;
    switch(binary_predicate->get_predicate_type())
    {
        case BPRED_OR:
            writeDisjunction(binary_predicate);
            return;
        case BPRED_IMPLIES:
            predicate_operator = "=>";
            break;
        case BPRED_EQUIVALENT:
            predicate_operator = "<=>";
            break;
        case BPRED_CONJONCTION:
            writeConjunction(binary_predicate);
            return;
            // LCOV_EXCL_START
        default:
            throw XmlException("Unknown binary predicate type");
            // LCOV_EXCL_STOP
    }

    writer->beginElement("Binary_Pred");
    writer->writeAttribute("op", predicate_operator);
    writeItemAttributes(binary_predicate);

    visit(binary_predicate->get_pred1());
    visit(binary_predicate->get_pred2());
    writer->endElement();
}

void XmlWriter::visitTypingPredicate(T_typing_predicate*typing_predicate)
{
    std::string operator_value;
    T_typing_predicate_type t = typing_predicate->get_typing_predicate_type();
    switch(t)
    {
        case TPRED_EQUAL:
            operator_value = "=";
            break;
        case TPRED_BELONGS:
            operator_value  = ":";
            break;
        case TPRED_INCLUDED:
            operator_value = "<:";
            break;
        case TPRED_STRICT_INCLUDED:
            operator_value = "<<:";
            break;
            // LCOV_EXCL_START
        default:
            throw XmlException("Unknown typing predicate");
            // LCOV_EXCL_STOP
    }

    writer->beginElement("Exp_Comparison");
    writer->writeAttribute("op", operator_value);
    writeItemAttributes(typing_predicate);

    std::vector<T_item*> identifiers;
    T_expr *current = typing_predicate->get_identifiers();
    while(current)
    {
        identifiers.push_back(current);
        current = static_cast<T_expr*>(current->get_next());
    }
    writeLeftMapletList(identifiers, typing_predicate->get_separator());

    visit(typing_predicate->get_type()->make_expr());

    writer->endElement("Exp_Comparison");
}

void XmlWriter::visitComprehensiveSet(T_comprehensive_set*set)
{
    writer->beginElement("Quantified_Set");
    writeItemAttributes(set);

    writeElementItemList("Variables", set->get_identifiers(), 0);

    writer->beginElement("Body");
    visit(set->get_predicate());
    writer->endElement("Body");
    writer->endElement("Quantified_Set");
}

void XmlWriter::visitImage(T_image *img)
{
    writeBinaryExpression("[", img->get_relation(), img->get_expr(), img);
}

void XmlWriter::visitPredicateWithParenthesis(T_predicate_with_parenthesis*pred)
{
    visit(pred->get_predicate());
}

void XmlWriter::visitExtensiveSet(T_extensive_set *ext_set)
{
    writeNaryExpression("{", ext_set->get_items(), ext_set);
}

void XmlWriter::visitNotPredicate(T_not_predicate *not_predicate)
{
    writer->beginElement("Unary_Pred");
    writer->writeAttribute("op", "not");
    writeItemAttributes(not_predicate);

    visit(not_predicate->get_predicate());
    writer->endElement("Unary_Pred");
}

void XmlWriter::visitExprPredicate(T_expr_predicate *exp_predicate)
{
    std::string op_type;
    T_expr_predicate_type type = exp_predicate->get_predicate_type();

    switch(type)
    {
        case EPRED_DIFFERENT:
            op_type = "/=";
            break;
        case EPRED_NOT_BELONGS:
            op_type = "/:";
            break;
        case EPRED_NOT_INCLUDED:
            op_type = "/<:";
            break;
        case EPRED_NOT_STRICT_INCLUDED:
            op_type = "/<<:";
            break;
        case EPRED_LESS_THAN:
            {
                T_type *type = exp_predicate->get_expr1()->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type = "<i";
                    else if(type->is_a_real())
                        op_type = "<r";
                    else if(type->is_a_float())
                        op_type = "<f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type = "<";
                }
                break;
            }
        case EPRED_GREATER_THAN:
            {
                T_type *type = exp_predicate->get_expr1()->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type = ">i";
                    else if(type->is_a_real())
                        op_type = ">r";
                    else if(type->is_a_float())
                        op_type = ">f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type = ">";
                }
                break;
            }
        case EPRED_LESS_THAN_OR_EQUAL:
            {
                T_type *type = exp_predicate->get_expr1()->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type = "<=i";
                    else if(type->is_a_real())
                        op_type = "<=r";
                    else if(type->is_a_float())
                        op_type = "<=f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type = "<=";
                }
                break;
            }
        case EPRED_GREATER_THAN_OR_EQUAL:
            {
                T_type *type = exp_predicate->get_expr1()->get_B_type();
                if(type){
                    if(type->is_an_integer())
                        op_type = ">=i";
                    else if(type->is_a_real())
                        op_type = ">=r";
                    else if(type->is_a_float())
                        op_type = ">=f";
                    // LCOV_EXCL_START
                    else
                        assert(false);
                    // LCOV_EXCL_STOP
                }
                else {
                    op_type = ">=";
                }
                break;
            }
            // LCOV_EXCL_START
        default:
            throw XmlException("Unexpected expression predicate type");
            // LCOV_EXCL_STOP
    }

    writer->beginElement("Exp_Comparison");
    writer->writeAttribute("op", op_type);
    writeItemAttributes(exp_predicate);

    visit(exp_predicate->get_expr1());
    visit(exp_predicate->get_expr2());

    writer->endElement("Exp_Comparison");
}

void XmlWriter::writeQuantifiedPredicate(const std::string &type, T_ident *variables, T_predicate *predicate, T_item *parent)
{
    writer->beginElement("Quantified_Pred");
    writer->writeAttribute("type", type);
    writeItemAttributes(parent);

    writeElementItemList("Variables", variables, 0);
    writeElementItem("Body", predicate, 0);
    writer->endElement("Quantified_Pred");
}

void XmlWriter::visitUniversalpredicate(T_universal_predicate *universal_predicate)
{
    writeQuantifiedPredicate("!", universal_predicate->get_variables(), universal_predicate->get_predicate(), universal_predicate);
}

void XmlWriter::visitExistentialPredicate(T_existential_predicate*exists)
{
    writeQuantifiedPredicate("#", exists->get_variables(), exists->get_predicate(), exists);

}

void XmlWriter::visitEmptySeq(T_empty_seq*eseq)
{
    writer->beginElement("EmptySeq");
    writeItemAttributes(eseq);
    writer->endElement();
}

void XmlWriter::visitEmptySet(T_empty_set*eset)
{
    writer->beginElement("EmptySet");
    writeItemAttributes(eset);
    writer->endElement();
}

void XmlWriter::writeQuantifiedExp(const std::string &operator_string,
        T_ident *identifiers,
        T_predicate *predicate,
        T_expr *expr,
        T_item *parent)
{
    writer->beginElement("Quantified_Exp");
    writer->writeAttribute("type", operator_string);
    writeItemAttributes(parent);

    writeElementItemList("Variables", identifiers, 0);

    writer->beginElement("Pred");
    visit(predicate);
    writer->endElement();

    writer->beginElement("Body");
    visit(expr);
    writer->endElement();

    writer->endElement();
}

void XmlWriter::visitSIGMA(T_sigma*sigma_exp)
{
    T_type *type = sigma_exp->get_B_type();
    std::string op = "SIGMA";
    if(type){
        if(type->is_an_integer())
            op = "iSIGMA";
        else if(type->is_a_real())
            op = "rSIGMA";
        // LCOV_EXCL_START
        else
            assert(false);
        // LCOV_EXCL_STOP
    }
    writeQuantifiedExp(op,
            sigma_exp->get_variables(),
            sigma_exp->get_predicate(),
            sigma_exp->get_expr(),
            sigma_exp);
}

void XmlWriter::visitPI(T_pi*pi_exp)
{
    T_type *type = pi_exp->get_B_type();
    std::string op = "PI";
    if(type){
        if(type->is_an_integer())
            op = "iPI";
        else if(type->is_a_real())
            op = "rPI";
        // LCOV_EXCL_START
        else
            assert(false);
        // LCOV_EXCL_STOP
    }
    writeQuantifiedExp(op,
            pi_exp->get_variables(),
            pi_exp->get_predicate(),
            pi_exp->get_expr(),
            pi_exp);
}

void XmlWriter::visitQuantifiedUnion(T_quantified_union*quantified_union)
{
    writeQuantifiedExp("UNION",
            quantified_union->get_variables(),
            quantified_union->get_predicate(),
            quantified_union->get_expression(),
            quantified_union);
}

void XmlWriter::visitQuantifiedIntersection(T_quantified_intersection*quantified_inter)
{
    writeQuantifiedExp("INTER",
            quantified_inter->get_variables(),
            quantified_inter->get_predicate(),
            quantified_inter->get_expression(),
            quantified_inter);
}

void XmlWriter::visitLiteralBoolean(T_literal_boolean *boolean_literal)
{
    writer->beginElement("Boolean_Literal");
    std::string value_string = boolean_literal->get_value() ? "TRUE" : "FALSE";

    writer->writeAttribute("value", value_string);
    writeItemAttributes(boolean_literal);

    writer->endElement();
}

void XmlWriter::visitLiteralEnumeratedValue(T_literal_enumerated_value *enumerated_value)
{
    visitIdent(enumerated_value);
}

void XmlWriter::visitLiteralReal(T_literal_real *real)
{
    writer->beginElement("Real_Literal");
    T_symbol *value = real->get_value();
    std::string string_value = value->get_value();
    writer->writeAttribute("value", string_value);
    writeItemAttributes(real);

    writer->endElement();
}

void XmlWriter::writeBinaryExpression(const std::string &op, T_expr *left, T_expr *right, T_item *parent)
{
    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", op);
    writeItemAttributes(parent);

    visit(left);
    visit(right);
    writer->endElement("Binary_Exp");

}

void XmlWriter::visitRelation(T_relation *rel)
{
    T_relation_type t = rel->get_relation_type();
    std::string operator_string;

    switch(t)
    {
        case RTYPE_TOTAL_FUNCTION:
            operator_string = "-->";
            break;
        case RTYPE_TOTAL_INJECTION:
            operator_string = ">->";
            break;
        case RTYPE_TOTAL_SURJECTION:
            operator_string = "-->>";
            break;
        case RTYPE_PARTIAL_FUNCTION:
            operator_string = "+->";
            break;
        case RTYPE_PARTIAL_INJECTION:
            operator_string = ">+>";
            break;
        case RTYPE_PARTIAL_SURJECTION:
            operator_string = "+->>";
            break;
        case RTYPE_BIJECTION:
            operator_string = ">->>";
            break;
    }

    writeBinaryExpression(operator_string, rel->get_src_set(), rel->get_dst_set(), rel);
}

void XmlWriter::visitExtensiveSeq(T_extensive_seq *ext_seq)
{
    writeNaryExpression("[", ext_seq->get_items(), ext_seq);
}

void XmlWriter::visitGeneralisedUnion(T_generalised_union*generalised_union)
{
    writeUnaryExpression("union", generalised_union->get_expressions(), generalised_union);
}

void XmlWriter::visitgeneralisedIntersection(T_generalised_intersection*generalised_inter)
{
    writeUnaryExpression("inter", generalised_inter->get_expressions(), generalised_inter);
}

// LCOV_EXCL_START
void XmlWriter::visitObject(T_object *o)
{
    NEVER_CALLED("visitObject");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitItem(T_item*)
{
    NEVER_CALLED("visitItem");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitSubstitution(T_substitution*)
{
    NEVER_CALLED("visitSubstitution");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitInteger(T_integer*)
{
    NEVER_CALLED("visitInteger");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitBetreeManager(T_betree_manager*)
{
    NEVER_CALLED("visitBetreeManager");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitBetreeList(T_betree_list*)
{
    NEVER_CALLED("visitBetreeList");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitType(T_type*)
{
    NEVER_CALLED("visitType");
}
// LCOV_EXCL_STOP

void XmlWriter::visitProductType(T_product_type *product_type)
{
    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", "*");
    visit(product_type->get_type1());
    visit(product_type->get_type2());
    writer->endElement();
}

void XmlWriter::visitSetOfType(T_setof_type *set_type)
{
    writer->beginElement("Unary_Exp");
    writer->writeAttribute("op", "POW");
    visit(set_type->get_base_type());
    writer->endElement();
}

void XmlWriter::visitAbstractType(T_abstract_type *abstract_type)
{
    T_ident *set_ident = abstract_type->get_set();
    beginIdentElement(set_ident);
    endIdentElement();
}

void XmlWriter::visitEnumeratedType(T_enumerated_type *enum_type)
{
    T_ident *type_definition = enum_type->get_type_definition();
    beginIdentElement(type_definition);
    endIdentElement();
}

void XmlWriter::visitPredefinedType(T_predefined_type *predefined_type)
{
    T_builtin_type builtin_type = predefined_type->get_type();

    writeBuiltinType(builtin_type);
}

// LCOV_EXCL_START
void XmlWriter::visitBaseType(T_base_type *)
{
    NEVER_CALLED("visitBaseType");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitConstructorType(T_constructor_type*)
{
    NEVER_CALLED("visitConstructorType");
}
// LCOV_EXCL_STOP

void XmlWriter::visitGenericType(T_generic_type* generic_type)
{
    T_type* binding = generic_type->get_type();
    if(binding != nullptr) {
        visit(binding);
    }
    else {
        writer->beginElement("Generic_Type");
        writer->endElement();
    }
}

// LCOV_EXCL_START
void XmlWriter::visitGenericOp(T_generic_op*)
{
    NEVER_CALLED("visitGenericOp");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitBound(T_bound *)
{
    // ??
    NEVER_CALLED("visitBound");
}
// LCOV_EXCL_STOP

void XmlWriter::visitOpResult(T_op_result *op_result)
{
    T_item *op_name = op_result->get_op_name();
    T_item *first_param = op_result->get_in_params();
    T_node_type node_type = op_name->get_node_type();

    std::vector<T_item*> params;
    getItemList(first_param, params);

    switch(node_type) {
        case NODE_IDENT:
            {
                std::string ident = getIdentValue(static_cast<T_ident*>(op_name));
                if(ident == "bool")
                {
                    assert(params.size() == 1);
                    writeElementItem("Boolean_Exp", first_param, op_result);
                    return;
                }

                if(BINARY_OPERATORS.find(ident) != BINARY_OPERATORS.end()){
                    assert(params.size() == 2);

                    writer->beginElement("Binary_Exp");
                    writer->writeAttribute("op", ident);
                    writeItemAttributes(op_result);
                    visit(params[0]);
                    visit(params[1]);
                    writer->endElement("Binary_Exp");
                    return;
                }

                if(UNARY_OPERATORS.find(ident) != UNARY_OPERATORS.end()) {
                    assert(params.size() == 1);

                    if(ident == "min"){
                        T_type *type = op_result->get_B_type();
                        if(type){
                            if(type->is_an_integer())
                                ident = "imin";
                            else if(type->is_a_real())
                                ident = "rmin";
                            // LCOV_EXCL_START
                            else
                                assert(false);
                            // LCOV_EXCL_STOP
                        }
                    }
                    else if(ident == "max"){
                        T_type *type = op_result->get_B_type();
                        if(type){
                            if(type->is_an_integer())
                                ident = "imax";
                            else if(type->is_a_real())
                                ident = "rmax";
                            // LCOV_EXCL_START
                            else
                                assert(false);
                            // LCOV_EXCL_STOP
                        }
                    }
                    writeUnaryExpression(ident, first_param, op_result);
                    return;
                }

                if(ident == "bin" && params.size() == 1){
                    writeUnaryExpression(ident, first_param, op_result);
                    return;
                }

                if(ident == "son" or ident == "bin"){
                    assert(params.size() == 3);

                    writer->beginElement("Ternary_Exp");
                    writer->writeAttribute("op", ident);
                    writeItemAttributes(op_result);
                    visit(params[0]);
                    visit(params[1]);
                    visit(params[2]);
                    writer->endElement("Ternary_Exp");
                    return;
                }
                break;
            } // NODE_IDENT
        default:
            // Nothing
            break;
    } // switch

    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", "(");
    writeItemAttributes(op_result);
    visit(op_name);
    writeLeftMapletList(params, BOP_COMMA);
    writer->endElement("Binary_Exp");
}

// LCOV_EXCL_START
void XmlWriter::visitPragma(T_pragma*)
{
    NEVER_CALLED("visitPragma");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitPragmaLexem(T_pragma_lexem*)
{
    NEVER_CALLED("visitPragmaLexem");
}
// LCOV_EXCL_STOP

void XmlWriter::visitB0Type(T_B0_type *b0_type)
{
    visit(b0_type->get_B_type());
}

void XmlWriter::visitB0BaseType(T_B0_base_type*B0_type)
{
    writeBuiltinType(B0_type->get_type());
}

void XmlWriter::visitB0AbstractType(T_B0_abstract_type*B0_type)
{
    T_ident *name = B0_type->get_name();
    beginIdentElement(name);
    endIdentElement();
}

void XmlWriter::visitB0EnumeratedType(T_B0_enumerated_type*B0_enum_type)
{
    T_ident *name = B0_enum_type->get_name();
    beginIdentElement(name);
    endIdentElement();
}

void XmlWriter::visitB0IntervalType(T_B0_interval_type *interval_type)
{
    if(interval_type->get_min() != nullptr && interval_type->get_max() != nullptr){
        writer->beginElement("Binary_Exp");
        writer->writeAttribute("op", "..");
        visit(interval_type->get_min());
        visit(interval_type->get_max());
        writer->endElement();
    } else {
        writer->beginElement("Id");
        writer->writeAttribute("value", "INTEGER");
        writer->endElement();
    }
}

void XmlWriter::visitB0ArrayType(T_B0_array_type *array_type)
{
    writer->beginElement("Binary_Exp");
    writer->writeAttribute("op", "-->");
    std::vector<T_item*> src_types;
    getItemList(array_type->get_first_src_type(), src_types);

    writeB0ArrayTypeSource(src_types);
    visit(array_type->get_dst_type());

    writer->endElement();
}

void XmlWriter::writeRecordItems(T_record_item *rec_item)
{
    /* Get the type of the record to add missing record label*/
    T_item* record_type = rec_item->get_father()->get_B_type();
    T_label_type *type_rec_label = NULL;
    if(record_type)
        if(record_type->get_node_type() == NODE_RECORD_TYPE) {
            type_rec_label = ((T_record_type*)record_type)->get_labels();
        }

    while(rec_item)
    {
        /*add missing record label if needed*/
        if(type_rec_label && rec_item->get_label() == NULL) {
            std::string name = getIdentValue(type_rec_label->get_name());
            T_ident* label = new T_ident();
            label->set_name(new T_symbol(name.c_str(), name.length(), 0, NULL, NULL));
            rec_item->set_label(label);
        }
        visitRecordItem(rec_item);

        if(type_rec_label)
            type_rec_label = static_cast<T_label_type*>(type_rec_label->get_next());
        rec_item = static_cast<T_record_item*>(rec_item->get_next());
    }
}

void XmlWriter::visitRecord(T_record *rec)
{
    writer->beginElement("Record");
    writeItemAttributes(rec);

    writeRecordItems(rec->get_record_items());
    writer->endElement("Record");
}

void XmlWriter::visitRecordItem(T_record_item *rec_item)
{
    writer->beginElement("Record_Item");
    writer->writeAttribute("label", getIdentValue(rec_item->get_label()));
    writeItemAttributes(rec_item);

    visit(rec_item->get_value());

    writer->endElement("Record_Item");
}

void XmlWriter::visitStruct(T_struct*stru)
{
    writer->beginElement("Struct");
    writeItemAttributes(stru);

    writeRecordItems(stru->get_record_items());
    writer->endElement("Struct");
}

void XmlWriter::visitRecordAccess(T_record_access *rec_access)
{
    writer->beginElement("Record_Field_Access");
    writer->writeAttribute("label", getIdentValue(rec_access->get_label()));
    writeItemAttributes(rec_access);
    visit(rec_access->get_record());
    writer->endElement("Record_Field_Access");
}

void XmlWriter::visitRecordType(T_record_type*rec)
{
    writeElementItemList("Struct", rec->get_labels(), rec);
}

void XmlWriter::visitLabelType(T_label_type*label_type)
{
    writer->beginElement("Record_Item");
    writer->writeAttribute("label", getIdentValue(label_type->get_name()));
    writeItemAttributes(label_type);

    visit(label_type->get_type());
    writer->endElement("Record_Item");
}

void XmlWriter::visitB0RecordType(T_B0_record_type*b0_record)
{
    writeElementItemList("Struct", b0_record->get_labels(), b0_record);
}

void XmlWriter::visitB0LabelType(T_B0_label_type*b0_label)
{
    writer->beginElement("Record_Item");
    writer->writeAttribute("label", getIdentValue(b0_label->get_name()));
    writeItemAttributes(b0_label);

    visit(b0_label->get_type());
    writer->endElement("Record_Item");
}

// LCOV_EXCL_START
void XmlWriter::visitMsgLine(T_msg_line*)
{
    NEVER_CALLED("visitMsgLine");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitMsgLineManager(T_msg_line_manager*)
{
    NEVER_CALLED("visitMsgLineManager");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitComponent(T_component*)
{
    NEVER_CALLED("visitComponent");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitFileComponent(T_file_component*)
{
    NEVER_CALLED("visitFileComponent");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitProject(T_project*)
{
    NEVER_CALLED("visitProject");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitProjectManager(T_project_manager*)
{
    NEVER_CALLED("visitProjectManager");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitFileDefinition(T_file_definition*)
{
    NEVER_CALLED("visitFileDefinition");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitComponentChecksums(T_component_checksums*)
{
    NEVER_CALLED("visitComponentChecksums");
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void XmlWriter::visitOperationChecksums(T_operation_checksums*)
{
    NEVER_CALLED("visitOperationChecksums");
}

void XmlWriter::visitWitness(T_witness *witness)
{
    writer->beginElement("Witness");
    writeItemAttributes(witness);
    writer->beginElement("Witnesses");
    visit(witness->get_predicate());
    writer->endElement("Witnesses");
    writer->beginElement("Body");
    writeSubstitutions(witness->get_body());
    writer->endElement("Body");
    writer->endElement("Witness");
}

void XmlWriter::writeTypeInfos() {
    if(_semanticAnalysis){
        typeInfoStream->seekg(0);

        typeInfoWriter->endElement("TypeInfos");
        writer->writeXml(typeInfoStream->str());
    }
}
