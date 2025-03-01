/**
 * performs semantic checks on the syntax tree
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date jul-20
 * @license see 'LICENSE' file
 */

#ifndef __SEMANTIC_CHECKS_H__
#define __SEMANTIC_CHECKS_H__

#include "ast.h"


class Semantics : public ASTVisitor
{
public:
	Semantics();
	virtual ~Semantics() = default;

	virtual t_astret visit(const ASTUMinus* ast) override;
	virtual t_astret visit(const ASTPlus* ast) override;
	virtual t_astret visit(const ASTMult* ast) override;
	virtual t_astret visit(const ASTMod* ast) override;
	virtual t_astret visit(const ASTPow* ast) override;
	virtual t_astret visit(const ASTNorm* ast) override;
	virtual t_astret visit(const ASTVar* ast) override;
	virtual t_astret visit(const ASTCall* ast) override;
	virtual t_astret visit(const ASTStmts* ast) override;
	virtual t_astret visit(const ASTVarDecl* ast) override;
	virtual t_astret visit(const ASTFunc* ast) override;
	virtual t_astret visit(const ASTReturn* ast) override;
	virtual t_astret visit(const ASTAssign* ast) override;
	virtual t_astret visit(const ASTVarRange* ast) override;
	virtual t_astret visit(const ASTArrayAccess* ast) override;
	virtual t_astret visit(const ASTArrayAssign* ast) override;
	virtual t_astret visit(const ASTComp* ast) override;
	virtual t_astret visit(const ASTCond* ast) override;
	virtual t_astret visit(const ASTCases* ast) override;
	virtual t_astret visit(const ASTBool* ast) override;
	virtual t_astret visit(const ASTLoop* ast) override;
	virtual t_astret visit(const ASTRangedLoop* ast) override;
	virtual t_astret visit(const ASTStrConst* ast) override;
	virtual t_astret visit(const ASTExprList* ast) override;
	virtual t_astret visit(const ASTNumConst<t_real>* ast) override;
	virtual t_astret visit(const ASTNumConst<t_int>* ast) override;
	virtual t_astret visit(const ASTNumConst<t_cplx>* ast) override;
	virtual t_astret visit(const ASTNumConst<bool>* ast) override;
	virtual t_astret visit(const ASTNumConstList<t_int>* ast) override;
	virtual t_astret visit(const ASTLabel* ast) override;
	virtual t_astret visit(const ASTJump* ast) override;

	// ------------------------------------------------------------------------
	// internally handled dummy nodes
	// ------------------------------------------------------------------------
	virtual t_astret visit(const ASTArgNames*) override { return nullptr; }
	virtual t_astret visit(const ASTTypeDecl*) override { return nullptr; }
	// ------------------------------------------------------------------------

protected:
};


#endif
