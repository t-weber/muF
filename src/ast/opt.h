/**
 * syntax tree optimisations
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 9-mar-2025
 * @license see 'LICENSE' file
 */

#ifndef __AST_OPT_H__
#define __AST_OPT_H__

#include "ast.h"


class ASTOpt : public ASTMutableVisitor
{
public:
	ASTOpt();
	virtual ~ASTOpt() = default;

	virtual t_astret visit(ASTUMinus* ast) override;
	virtual t_astret visit(ASTPlus* ast) override;
	virtual t_astret visit(ASTMult* ast) override;
	virtual t_astret visit(ASTMod* ast) override;
	virtual t_astret visit(ASTPow* ast) override;
	virtual t_astret visit(ASTNorm* ast) override;

	virtual t_astret visit(ASTComp* ast) override;

	virtual t_astret visit(ASTVarDecl* ast) override;
	virtual t_astret visit(ASTVar* ast) override;
	virtual t_astret visit(ASTAssign* ast) override;
	virtual t_astret visit(ASTVarRange* ast) override;

	virtual t_astret visit(ASTStmts* ast) override;
	virtual t_astret visit(ASTExprList* ast) override;

	virtual t_astret visit(ASTLoop* ast) override;
	virtual t_astret visit(ASTRangedLoop* ast) override;
        virtual t_astret visit(ASTLoopBreak* ast) override;
        virtual t_astret visit(ASTLoopNext* ast) override;

	virtual t_astret visit(ASTCond* ast) override;
	virtual t_astret visit(ASTCases* ast) override;
	virtual t_astret visit(ASTLabel* ast) override;
	virtual t_astret visit(ASTJump* ast) override;

	virtual t_astret visit(ASTArrayAccess* ast) override;
	virtual t_astret visit(ASTArrayAssign* ast) override;

	virtual t_astret visit(ASTFunc* ast) override;
	virtual t_astret visit(ASTCall* ast) override;
	virtual t_astret visit(ASTReturn* ast) override;

	virtual t_astret visit(ASTBool* ast) override;
	virtual t_astret visit(ASTStrConst* ast) override;
	virtual t_astret visit(ASTNumConst<t_real>* ast) override;
	virtual t_astret visit(ASTNumConst<t_int>* ast) override;
	virtual t_astret visit(ASTNumConst<t_cplx>* ast) override;
	virtual t_astret visit(ASTNumConst<bool>* ast) override;
	virtual t_astret visit(ASTNumConstList<t_int>* ast) override;

	// ------------------------------------------------------------------------
	// internally handled dummy nodes
	// ------------------------------------------------------------------------
	virtual t_astret visit(ASTArgNames*) override { return nullptr; }
	virtual t_astret visit(ASTTypeDecl*) override { return nullptr; }
	// ------------------------------------------------------------------------


	std::tuple<std::size_t, std::size_t> GetConstOpts() const
	{
		return std::make_tuple(m_arith_opts, m_logic_opts);
	}


protected:
	ASTPtr OptConsts(ASTPtr ast);


private:
	std::size_t m_arith_opts{0};  // number of constant arithmetic expression optimisations performed
	std::size_t m_logic_opts{0};   // number of logical arithmetic expression optimisations performed
};


#endif
