#ifndef SRC_LINT_DETECTORS_METHOD_RETURN_LEXICAL_SCOPE_HPP_
#define SRC_LINT_DETECTORS_METHOD_RETURN_LEXICAL_SCOPE_HPP_

#include "detectors/Detector.hpp"
#include "Linter.hpp"

#include <string>

namespace lint {

class MethodReturnLexicalScope : public Detector {
public:
    MethodReturnLexicalScope() = delete;
    MethodReturnLexicalScope(Linter* linter, antlr4::TokenStreamRewriter* rewriter):
        Detector(linter, rewriter), m_inBlockCount(0), m_inNamedAssignCount(0), m_inArgsCount(0) { }
    virtual ~MethodReturnLexicalScope() = default;

    void enterBlock(sprklr::SCParser::BlockContext*) override { ++m_inBlockCount; }
    void exitBlock(sprklr::SCParser::BlockContext*) override { --m_inBlockCount; }

    void enterBody(sprklr::SCParser::BodyContext* ctx) override {
        if (!ctx->returnExpr() || m_inArgsCount > 0 || m_inBlockCount == 0 || m_inNamedAssignCount == 0)
            return;

        m_linter->addIssue({ IssueNumber::kMethodReturnInLexicalScope, IssueSeverity::kError,
                             static_cast<int32_t>(ctx->returnExpr()->start->getLine()),
                             static_cast<int32_t>(ctx->returnExpr()->start->getCharPositionInLine()) });
    }

    void enterExprAssignDotName(sprklr::SCParser::ExprAssignDotNameContext*) override { ++m_inNamedAssignCount; }
    void exitExprAssignDotName(sprklr::SCParser::ExprAssignDotNameContext*) override { --m_inNamedAssignCount; }

    void enterExprAssignGlobal(sprklr::SCParser::ExprAssignGlobalContext*) override { ++m_inNamedAssignCount; }
    void exitExprAssignGlobal(sprklr::SCParser::ExprAssignGlobalContext*) override { --m_inNamedAssignCount; }

    void enterExprAssign(sprklr::SCParser::ExprAssignContext*) override { ++m_inNamedAssignCount; }
    void exitExprAssign(sprklr::SCParser::ExprAssignContext*) override { --m_inNamedAssignCount; }

    void enterVarDef(sprklr::SCParser::VarDefContext*) override { ++m_inNamedAssignCount; }
    void exitVarDef(sprklr::SCParser::VarDefContext*) override { --m_inNamedAssignCount; }

    // Block arguments to messages are safe to contain method returns because the method return is guaranteed to
    // respect the caller frame.
    void enterBlockArgList(sprklr::SCParser::BlockArgListContext*) override { ++m_inArgsCount; }
    void exitBlockArgList(sprklr::SCParser::BlockArgListContext*) override { --m_inArgsCount; }

    void enterArgList(sprklr::SCParser::ArgListContext*) override { ++m_inArgsCount; }
    void exitArgList(sprklr::SCParser::ArgListContext*) override { --m_inArgsCount; }

    void enterKeyArgList(sprklr::SCParser::KeyArgListContext*) override { ++m_inArgsCount; }
    void exitKeyArgList(sprklr::SCParser::KeyArgListContext*) override { --m_inArgsCount; }

    void enterExprBinop(sprklr::SCParser::ExprBinopContext*) override { ++m_inArgsCount; }
    void exitExprBinop(sprklr::SCParser::ExprBinopContext*) override { --m_inArgsCount; }

private:
    int m_inBlockCount;
    int m_inNamedAssignCount;
    int m_inArgsCount;
};

} // namespace lint

#endif // SRC_LINT_DETECTORS_METHOD_RETURN_LEXICAL_SCOPE_HPP_
