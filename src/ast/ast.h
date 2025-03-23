/**
 * syntax tree
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 20-dec-19
 * @license see 'LICENSE' file
 */

#ifndef __AST_H__
#define __AST_H__

#include <memory>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdint>

#include "common/sym.h"
#include "lalr1/ast.h"


class AST;
class ASTStmts; class ASTExprList;
class ASTUMinus; class ASTPlus; class ASTMult; class ASTMod;
class ASTPow; class ASTNorm;
class ASTVarDecl; class ASTVar;
class ASTFunc; class ASTReturn; class ASTCall;
class ASTVarRange; class ASTAssign;
class ASTArrayAssign; class ASTArrayAccess;
class ASTCond; class ASTCases; class ASTComp; class ASTBool;
class ASTLoop; class ASTRangedLoop; class ASTLoopBreak; class ASTLoopNext;
class ASTJump; class ASTLabel;
class ASTStrConst;
template<class> class ASTNumConst;
template<class> class ASTNumConstList;
class ASTInternalArgNames; class ASTInternalMisc;


enum class ASTType
{
	Stmts, ExprList,
	UMinus, Plus, Mult, Mod,
	Pow, Norm,
	VarDecl, Var,
	Func, Return, Call,
	VarRange, Assign,
	ArrayAssign, ArrayAccess,
	Cond, Cases, Comp, Bool,
	Loop, RangedLoop, LoopBreak, LoopNext,
	Jump, Label,
	StrConst, NumConst, NumConstList,
	InternalArgNames, InternalMisc,
};


using ASTPtr = std::shared_ptr<AST>;
using t_astret = std::shared_ptr<Symbol>;


/**
 * constant ast visitor
 */
class ASTVisitor
{
public:
	virtual ~ASTVisitor() {}

	virtual t_astret visit(const ASTUMinus* ast) = 0;
	virtual t_astret visit(const ASTPlus* ast) = 0;
	virtual t_astret visit(const ASTMult* ast) = 0;
	virtual t_astret visit(const ASTMod* ast) = 0;
	virtual t_astret visit(const ASTPow* ast) = 0;
	virtual t_astret visit(const ASTNorm* ast) = 0;

	virtual t_astret visit(const ASTVarDecl* ast) = 0;
	virtual t_astret visit(const ASTVar* ast) = 0;
	virtual t_astret visit(const ASTAssign* ast) = 0;
	virtual t_astret visit(const ASTVarRange* ast) = 0;

	virtual t_astret visit(const ASTArrayAssign* ast) = 0;
	virtual t_astret visit(const ASTArrayAccess* ast) = 0;

	virtual t_astret visit(const ASTNumConst<t_real>* ast) = 0;
	virtual t_astret visit(const ASTNumConst<t_int>* ast) = 0;
	virtual t_astret visit(const ASTNumConst<t_cplx>* ast) = 0;
	virtual t_astret visit(const ASTNumConst<t_quat>* ast) = 0;
	virtual t_astret visit(const ASTNumConst<bool>* ast) = 0;

	virtual t_astret visit(const ASTNumConstList<t_int>* ast) = 0;

	virtual t_astret visit(const ASTStrConst* ast) = 0;

	virtual t_astret visit(const ASTFunc* ast) = 0;
	virtual t_astret visit(const ASTCall* ast) = 0;
	virtual t_astret visit(const ASTReturn* ast) = 0;
	virtual t_astret visit(const ASTStmts* ast) = 0;

	virtual t_astret visit(const ASTCond* ast) = 0;
	virtual t_astret visit(const ASTCases* ast) = 0;
	virtual t_astret visit(const ASTLoop* ast) = 0;
	virtual t_astret visit(const ASTRangedLoop* ast) = 0;
	virtual t_astret visit(const ASTLoopBreak* ast) = 0;
	virtual t_astret visit(const ASTLoopNext* ast) = 0;

	virtual t_astret visit(const ASTComp* ast) = 0;
	virtual t_astret visit(const ASTBool* ast) = 0;
	virtual t_astret visit(const ASTExprList* ast) = 0;

	virtual t_astret visit(const ASTLabel* ast) = 0;
	virtual t_astret visit(const ASTJump* ast) = 0;

	virtual t_astret visit(const ASTInternalArgNames* ast) = 0;
	virtual t_astret visit(const ASTInternalMisc* ast) = 0;
};


/**
 * mutable ast visitor
 */
class ASTMutableVisitor
{
public:
	virtual ~ASTMutableVisitor() {}

	virtual t_astret visit(ASTUMinus* ast) = 0;
	virtual t_astret visit(ASTPlus* ast) = 0;
	virtual t_astret visit(ASTMult* ast) = 0;
	virtual t_astret visit(ASTMod* ast) = 0;
	virtual t_astret visit(ASTPow* ast) = 0;
	virtual t_astret visit(ASTNorm* ast) = 0;

	virtual t_astret visit(ASTVarDecl* ast) = 0;
	virtual t_astret visit(ASTVar* ast) = 0;
	virtual t_astret visit(ASTAssign* ast) = 0;
	virtual t_astret visit(ASTVarRange* ast) = 0;

	virtual t_astret visit(ASTArrayAssign* ast) = 0;
	virtual t_astret visit(ASTArrayAccess* ast) = 0;

	virtual t_astret visit(ASTNumConst<t_real>* ast) = 0;
	virtual t_astret visit(ASTNumConst<t_int>* ast) = 0;
	virtual t_astret visit(ASTNumConst<t_cplx>* ast) = 0;
	virtual t_astret visit(ASTNumConst<t_quat>* ast) = 0;
	virtual t_astret visit(ASTNumConst<bool>* ast) = 0;

	virtual t_astret visit(ASTNumConstList<t_int>* ast) = 0;

	virtual t_astret visit(ASTStrConst* ast) = 0;

	virtual t_astret visit(ASTFunc* ast) = 0;
	virtual t_astret visit(ASTCall* ast) = 0;
	virtual t_astret visit(ASTReturn* ast) = 0;
	virtual t_astret visit(ASTStmts* ast) = 0;

	virtual t_astret visit(ASTCond* ast) = 0;
	virtual t_astret visit(ASTCases* ast) = 0;
	virtual t_astret visit(ASTLoop* ast) = 0;
	virtual t_astret visit(ASTRangedLoop* ast) = 0;
	virtual t_astret visit(ASTLoopBreak* ast) = 0;
	virtual t_astret visit(ASTLoopNext* ast) = 0;

	virtual t_astret visit(ASTComp* ast) = 0;
	virtual t_astret visit(ASTBool* ast) = 0;
	virtual t_astret visit(ASTExprList* ast) = 0;

	virtual t_astret visit(ASTLabel* ast) = 0;
	virtual t_astret visit(ASTJump* ast) = 0;

	virtual t_astret visit(ASTInternalArgNames* ast) = 0;
	virtual t_astret visit(ASTInternalMisc* ast) = 0;
};


/**
 * ast node base
 */
class AST : public lalr1::ASTBase
{
public:
	virtual ~AST() = default;

	virtual t_astret accept(ASTVisitor* visitor) const = 0;
	virtual t_astret accept(ASTMutableVisitor* visitor) = 0;
	virtual ASTType type() = 0;

	// TODO: either also add this to derived ast classes or use terminal override value in lexer
	//virtual bool IsTerminal() const override { return false; }
};


/**
 * ast visitor acceptor
 */
template<class t_ast_sub>
class ASTAcceptor : public AST
{
public:
	virtual t_astret accept(ASTVisitor* visitor) const override
	{
		const t_ast_sub *sub = static_cast<const t_ast_sub*>(this);
		return visitor->visit(sub);
	}

	virtual t_astret accept(ASTMutableVisitor* visitor) override
	{
		t_ast_sub *sub = static_cast<t_ast_sub*>(this);
		return visitor->visit(sub);
	}
};


class ASTUMinus : public ASTAcceptor<ASTUMinus>
{
public:
	ASTUMinus(ASTPtr term) : term{term}
	{}

	const ASTPtr GetTerm() const { return term; }
	void SetTerm(const ASTPtr term) { this->term = term; }

	virtual ASTType type() override { return ASTType::UMinus; }

private:
	ASTPtr term{};
};


class ASTPlus : public ASTAcceptor<ASTPlus>
{
public:
	ASTPlus(ASTPtr term1, ASTPtr term2, bool invert = false)
		: term1{term1}, term2{term2}, inverted{invert}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }
	bool IsInverted() const { return inverted; }

	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	virtual ASTType type() override { return ASTType::Plus; }

private:
	ASTPtr term1{}, term2{};
	bool inverted{ false };
};


class ASTMult : public ASTAcceptor<ASTMult>
{
public:
	ASTMult(ASTPtr term1, ASTPtr term2, bool invert = false)
		: term1{term1}, term2{term2}, inverted{invert}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }
	bool IsInverted() const { return inverted; }

	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	virtual ASTType type() override { return ASTType::Mult; }

private:
	ASTPtr term1{}, term2{};
	bool inverted{ false };
};


class ASTMod : public ASTAcceptor<ASTMod>
{
public:
	ASTMod(ASTPtr term1, ASTPtr term2)
		: term1{term1}, term2{term2}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }

	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	virtual ASTType type() override { return ASTType::Mod; }

private:
	ASTPtr term1{}, term2{};
};


class ASTPow : public ASTAcceptor<ASTPow>
{
public:
	ASTPow(ASTPtr term1, ASTPtr term2)
		: term1{term1}, term2{term2}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }

	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	virtual ASTType type() override { return ASTType::Pow; }

private:
	ASTPtr term1{}, term2{};
};


class ASTNorm : public ASTAcceptor<ASTNorm>
{
public:
	ASTNorm(ASTPtr term) : term{term}
	{}

	const ASTPtr GetTerm() const { return term; }
	void SetTerm(const ASTPtr term) { this->term = term; }

	virtual ASTType type() override { return ASTType::Norm; }

private:
	ASTPtr term{};
};


class ASTVar : public ASTAcceptor<ASTVar>
{
public:
	ASTVar(const t_str& ident)
		: ident{ident}
	{}

	const t_str& GetIdent() const { return ident; }

	virtual ASTType type() override { return ASTType::Var; }

private:
	t_str ident{};
};


class ASTLabel : public ASTAcceptor<ASTLabel>
{
public:
	ASTLabel(const t_str& ident)
		: ident{ident}
	{}

	const t_str& GetIdent() const { return ident; }

	virtual ASTType type() override { return ASTType::Label; }

private:
	t_str ident{};
};


class ASTJump : public ASTAcceptor<ASTJump>
{
public:
	ASTJump(const t_str& label, bool is_comefrom = false)
		: label{label}, is_comefrom{is_comefrom}
	{}

	const t_str& GetLabel() const { return label; }
	bool IsComefrom() const { return is_comefrom; }

	virtual ASTType type() override { return ASTType::Jump; }

private:
	// label to jump to (or come from)
	t_str label{};
	bool is_comefrom{ false };
};


class ASTStmts : public ASTAcceptor<ASTStmts>
{
public:
	ASTStmts() : stmts{}
	{}

	void AddStatement(ASTPtr stmt) { stmts.push_front(stmt); }
	const std::list<ASTPtr>& GetStatementList() const { return stmts; }

	virtual ASTType type() override { return ASTType::Stmts; }

private:
	std::list<ASTPtr> stmts{};
};


class ASTVarDecl : public ASTAcceptor<ASTVarDecl>
{
public:
	ASTVarDecl()
		: vars{}
	{}

	ASTVarDecl(std::shared_ptr<ASTAssign> optAssign)
		: vars{}, optAssign{optAssign}
	{}

	void AddVariable(const t_str& var) { vars.push_front(var); }
	const std::list<t_str>& GetVariables() const { return vars; }

	const std::shared_ptr<ASTAssign> GetAssignment() const { return optAssign; }
	void SetAssignment(const std::shared_ptr<ASTAssign> term) { this->optAssign = term; }

	bool GetIntentIn() const { return intent_in; }
	void SetIntentIn(bool b) { intent_in = b; }

	bool GetIntentOut() const { return intent_out; }
	void SetIntentOut(bool b) { intent_out = b; }

	virtual ASTType type() override { return ASTType::VarDecl; }

private:
	std::list<t_str> vars{};

	// optional assignment
	std::shared_ptr<ASTAssign> optAssign{};

	// for function arguments or returns
	bool intent_in{ false };
	bool intent_out{ false };
};


class ASTInternalArgNames : public ASTAcceptor<ASTInternalArgNames>
{
public:
	ASTInternalArgNames() : argnames{}
	{}

	void AddArg(const t_str& argname, SymbolType ty=SymbolType::UNKNOWN,
		const std::vector<std::size_t>& dims = { 1 })
	{
		argnames.push_front(std::make_tuple(argname, ty, dims));
	}

	const std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>>& GetArgs() const
	{
		return argnames;
	}

	std::size_t GetNumArgs() const { return argnames.size(); }

	std::vector<t_str> GetArgIdents() const
	{
		std::vector<t_str> idents;
		for(const auto& arg : argnames)
			idents.push_back(std::get<0>(arg));
		return idents;
	}

	std::vector<SymbolType> GetArgTypes() const
	{
		std::vector<SymbolType> ty;
		for(const auto& arg : argnames)
			ty.push_back(std::get<1>(arg));
		return ty;
	}

	const t_str& GetArgIdent(std::size_t idx) const
	{
		return std::get<0>(*std::next(argnames.begin(), idx));
	}

	void SetArgType(std::size_t idx, SymbolType ty)
	{
		std::get<1>(*std::next(argnames.begin(), idx)) = ty;
	}

	void SetArgDims(std::size_t idx, const std::vector<std::size_t>& dims)
	{
		std::get<2>(*std::next(argnames.begin(), idx)) = dims;
	}

	virtual ASTType type() override { return ASTType::InternalArgNames; }

private:
	std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>> argnames{};
};


class ASTFunc : public ASTAcceptor<ASTFunc>
{
public:
	ASTFunc(const t_str& ident,
		std::shared_ptr<ASTInternalArgNames> args, std::shared_ptr<ASTStmts> stmts,
		std::shared_ptr<ASTInternalArgNames> rets = nullptr)
		: ident{ident}, args{args->GetArgs()}, rets{}, stmts{stmts}
	{
		if(rets)
			this->rets = rets->GetArgs();
	}

	const t_str& GetIdent() const { return ident; }

	const std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>>&
	GetArgs() const { return args; }

	const std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>>&
	GetRets() const { return rets; }

	const std::shared_ptr<ASTStmts> GetStatements() const { return stmts; }

	bool GetRecursive() const { return recursive; }
	void SetRecursive(bool b) { recursive = b; }

	virtual ASTType type() override { return ASTType::Func; }

private:
	t_str ident{};
	bool recursive{ true };
	std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>> args{};
	std::list<std::tuple<t_str, SymbolType, std::vector<std::size_t>>> rets{};
	std::shared_ptr<ASTStmts> stmts{};
};


class ASTReturn : public ASTAcceptor<ASTReturn>
{
public:
	ASTReturn(const std::shared_ptr<ASTExprList>& rets = nullptr,
		bool only_jump_to_func_end = true)
		: rets{rets}, only_jump_to_func_end{only_jump_to_func_end}
	{}

	bool OnlyJumpToFuncEnd() const { return only_jump_to_func_end; }
	const std::shared_ptr<ASTExprList> GetRets() const { return rets; }

	virtual ASTType type() override { return ASTType::Return; }

private:
	std::shared_ptr<ASTExprList> rets{};
	bool only_jump_to_func_end{ true };
};


class ASTExprList : public ASTAcceptor<ASTExprList>
{
public:
	ASTExprList()
	{}

	void AddExpr(ASTPtr expr) { exprs.push_front(expr); }

	const std::list<ASTPtr>& GetList() const { return exprs; }
	std::list<ASTPtr>& GetList() { return exprs; }

	/**
	 * specialised use as an array
	 */
	void SetArrayType(SymbolType ty = SymbolType::REAL_ARRAY) { m_array_type = ty; }
	SymbolType GetArrayType() const { return m_array_type; }
	bool IsArray() const { return m_array_type != SymbolType::VOID; }

	virtual ASTType type() override { return ASTType::ExprList; }

private:
	std::list<ASTPtr> exprs{};
	SymbolType m_array_type = SymbolType::VOID;  /* void: no array */
};


class ASTCall : public ASTAcceptor<ASTCall>
{
public:
	ASTCall(const t_str& ident)
		: ident{ident}, args{std::make_shared<ASTExprList>()}
	{}

	ASTCall(const t_str& ident, std::shared_ptr<ASTExprList> args)
		: ident{ident}, args{args}
	{}

	const t_str& GetIdent() const { return ident; }
	const std::list<ASTPtr>& GetArgumentList() const { return args->GetList(); }
	std::list<ASTPtr>& GetArgumentList() { return args->GetList(); }

	virtual ASTType type() override { return ASTType::Call; }

private:
	t_str ident{};
	std::shared_ptr<ASTExprList> args{};
};


class ASTAssign : public ASTAcceptor<ASTAssign>
{
public:
	ASTAssign()
	{}

	ASTAssign(const t_str& ident, ASTPtr _expr)
		: idents{{ident}}, expr{_expr}
	{}

	ASTAssign(const std::vector<t_str>& idents, ASTPtr expr)
		: idents{idents}, expr{expr}
	{}

	const std::vector<t_str>& GetIdents() const { return idents; }
	const t_str& GetIdent() const { return GetIdents()[0]; }

	const ASTPtr GetExpr() const { return expr; }
	void SetExpr(const ASTPtr expr) { this->expr = expr; }

	bool IsMultiAssign() const { return idents.size() > 1; }
	bool IsNullAssign() const { return idents.size() == 0; }

	virtual ASTType type() override { return ASTType::Assign; }

private:
	std::vector<t_str> idents{};
	ASTPtr expr{};
};


class ASTVarRange : public ASTAcceptor<ASTVarRange>
{
public:
	ASTVarRange()
	{}

	ASTVarRange(const t_str& ident, ASTPtr begin, ASTPtr end, ASTPtr inc = nullptr)
		: ident{ident}, begin{begin}, end{end}, inc{inc}
	{}

	const t_str& GetIdent() const { return ident; }
	const ASTPtr GetBegin() const { return begin; }
	const ASTPtr GetEnd() const { return end; }
	const ASTPtr GetInc() const { return inc; }

	virtual ASTType type() override { return ASTType::VarRange; }

private:
	t_str ident{};
	ASTPtr begin{}, end{}, inc{};
};


class ASTComp : public ASTAcceptor<ASTComp>
{
public:
	enum CompOp
	{
		EQU, NEQ,
		GT, LT, GEQ, LEQ
	};

public:
	ASTComp(ASTPtr term1, ASTPtr term2, CompOp op)
		: term1{term1}, term2{term2}, op{op}
	{}

	ASTComp(ASTPtr term1, CompOp op)
		: term1{term1}, term2{nullptr}, op{op}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }
	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	CompOp GetOp() const { return op; }

	virtual ASTType type() override { return ASTType::Comp; }

private:
	ASTPtr term1{}, term2{};
	CompOp op{};
};


class ASTBool : public ASTAcceptor<ASTBool>
{
public:
	enum BoolOp
	{
		NOT,
		AND, OR, XOR
	};

public:
	ASTBool(ASTPtr term1, ASTPtr term2, BoolOp op)
		: term1{term1}, term2{term2}, op{op}
	{}

	ASTBool(ASTPtr term1, BoolOp op)
		: term1{term1}, term2{nullptr}, op{op}
	{}

	const ASTPtr GetTerm1() const { return term1; }
	const ASTPtr GetTerm2() const { return term2; }
	void SetTerm1(const ASTPtr term) { this->term1 = term; }
	void SetTerm2(const ASTPtr term) { this->term2 = term; }

	BoolOp GetOp() const { return op; }

	virtual ASTType type() override { return ASTType::Bool; }

private:
	ASTPtr term1{}, term2{};
	BoolOp op{};
};


class ASTCond : public ASTAcceptor<ASTCond>
{
public:
	ASTCond(const ASTPtr cond, ASTPtr if_stmt)
		: cond{cond}, if_stmt{if_stmt}
	{}
	ASTCond(const ASTPtr cond, ASTPtr if_stmt, ASTPtr else_stmt)
		: cond{cond}, if_stmt{if_stmt}, else_stmt{else_stmt}
	{}

	const ASTPtr GetCond() const { return cond; }
	void SetCond(const ASTPtr term) { this->cond = term; }

	const ASTPtr GetIf() const { return if_stmt; }
	const ASTPtr GetElse() const { return else_stmt; }
	bool HasElse() const { return else_stmt != nullptr; }

	virtual ASTType type() override { return ASTType::Cond; }

private:
	ASTPtr cond{};
	ASTPtr if_stmt{}, else_stmt{};
};


class ASTCases : public ASTAcceptor<ASTCases>
{
public:
	using t_case = std::pair<ASTPtr /*cond*/, ASTPtr /*stmts*/>;
	using t_cases = std::list<t_case>;

public:
	ASTCases()
	{}

	void SetExpr(ASTPtr expr) { this->expr = expr; }
	const ASTPtr GetExpr() const { return expr; }

	void AddCase(ASTPtr cond, ASTPtr stmts)
	{
		cases.push_front(std::make_pair(cond, stmts));
	}

	const t_cases& GetCases() const { return cases; }
	t_cases& GetCases() { return cases; }

	void SetDefaultCase(ASTPtr stmts) { default_stmts = stmts; }
	const ASTPtr GetDefaultCase() const { return default_stmts; }

	virtual ASTType type() override { return ASTType::Cases; }

private:
	ASTPtr expr{};           // expression to select
	t_cases cases{};         // cases to handle
	ASTPtr default_stmts{};  // default case
};


class ASTLoop : public ASTAcceptor<ASTLoop>
{
public:
	ASTLoop(const ASTPtr cond, ASTPtr stmt)
		: cond{cond}, stmt{stmt}
	{}

	const ASTPtr GetCond() const { return cond; }
	void SetCond(const ASTPtr term) { this->cond = term; }

	const ASTPtr GetLoopStmt() const { return stmt; }

	virtual ASTType type() override { return ASTType::Loop; }

private:
	ASTPtr cond{}, stmt{};
};


class ASTRangedLoop : public ASTAcceptor<ASTRangedLoop>
{
public:
	ASTRangedLoop(std::shared_ptr<ASTVarRange> range, ASTPtr stmt)
		: range{range}, stmt{stmt}
	{}

	const std::shared_ptr<ASTVarRange> GetRange() const { return range; }
	const ASTPtr GetLoopStmt() const { return stmt; }

	virtual ASTType type() override { return ASTType::RangedLoop; }

private:
	std::shared_ptr<ASTVarRange> range{};
	ASTPtr stmt{};
};


class ASTLoopBreak : public ASTAcceptor<ASTLoopBreak>
{
public:
	ASTLoopBreak(t_int num_loops = 0) : num_loops{num_loops}
	{}

	virtual ASTType type() override { return ASTType::LoopBreak; }

	t_int GetNumLoops() const { return num_loops; }

private:
	t_int num_loops{ 0 };
};


class ASTLoopNext : public ASTAcceptor<ASTLoopNext>
{
public:
	ASTLoopNext(t_int num_loops = 0) : num_loops{num_loops}
	{}

	virtual ASTType type() override { return ASTType::LoopNext; }

	t_int GetNumLoops() const { return num_loops; }

private:
	t_int num_loops{ 0 };
};


class ASTArrayAccess : public ASTAcceptor<ASTArrayAccess>
{
public:
	ASTArrayAccess(ASTPtr term,
		ASTPtr num1, ASTPtr num2 = nullptr,
		bool ranged12 = false)
		: term{term}, num1{num1}, num2{num2}, ranged12{ranged12}
	{}

	const ASTPtr GetTerm() const { return term; }
	void SetTerm(const ASTPtr term) { this->term = term; }

	const ASTPtr GetNum1() const { return num1; }
	const ASTPtr GetNum2() const { return num2; }
	void SetNum1(const ASTPtr term) { this->num1 = term; }
	void SetNum2(const ASTPtr term) { this->num2 = term; }

	bool IsRanged12() const { return ranged12; }

	virtual ASTType type() override { return ASTType::ArrayAccess; }

private:
	ASTPtr term;

	ASTPtr num1{}, num2{};
	bool ranged12{ false };
};


class ASTArrayAssign : public ASTAcceptor<ASTArrayAssign>
{
public:
	ASTArrayAssign(const t_str& ident, ASTPtr expr,
		ASTPtr num1, ASTPtr num2 = nullptr, bool ranged12 = false)
		: ident{ident}, expr{expr}, num1{num1}, num2{num2}, ranged12{ranged12}
	{}

	const t_str& GetIdent() const { return ident; }

	const ASTPtr GetExpr() const { return expr; }
	void SetExpr(const ASTPtr expr) { this->expr = expr; }

	const ASTPtr GetNum1() const { return num1; }
	const ASTPtr GetNum2() const { return num2; }
	void SetNum1(const ASTPtr term) { this->num1 = term; }
	void SetNum2(const ASTPtr term) { this->num2 = term; }

	bool IsRanged12() const { return ranged12; }

	virtual ASTType type() override { return ASTType::ArrayAssign; }

private:
	t_str ident{};
	ASTPtr expr{};

	ASTPtr num1{}, num2{};
	bool ranged12{ false };
};


template<class t_num>
class ASTNumConst : public ASTAcceptor<ASTNumConst<t_num>>
{
public:
	ASTNumConst(const t_num& val) : val{val}
	{}

	const t_num& GetVal() const { return val; }
	void SetVal(const t_num& val) { this->val = val; }

	virtual ASTType type() override { return ASTType::NumConst; }

private:
	t_num val{};
};


template<class t_num>
class ASTNumConstList : public ASTAcceptor<ASTNumConstList<t_num>>
{
public:
	ASTNumConstList()
	{}

	const std::list<t_num>& GetValues() const { return vals; }
	std::size_t GetSize() const { return vals.size(); }
	void AddValue(t_num val) { vals.push_front(val); }
	t_num GetValue(std::size_t idx) const { return *std::next(vals.begin(), idx); }

	virtual ASTType type() override { return ASTType::NumConstList; }

private:
	std::list<t_num> vals{};
};


class ASTStrConst : public ASTAcceptor<ASTStrConst>
{
public:
	ASTStrConst(const t_str& str) : val{str}
	{}

	const t_str& GetVal() const { return val; }
	void SetVal(const t_str& val) { this->val = val; }

	virtual ASTType type() override { return ASTType::StrConst; }

private:
	t_str val{};
};


/**
 * miscellaneous options
 */
class ASTInternalMisc : public ASTAcceptor<ASTInternalMisc>
{
public:
	ASTInternalMisc()
	{}

	bool GetRecursive() const { return recursive; }
	void SetRecursive(bool b) { recursive = b; }

	bool GetIntentIn() const { return intent_in; }
	void SetIntentIn(bool b) { intent_in = b; }

	bool GetIntentOut() const { return intent_out; }
	void SetIntentOut(bool b) { intent_out = b; }

	virtual ASTType type() override { return ASTType::InternalMisc; }

private:
	bool recursive{ true };
	bool intent_in{ false };
	bool intent_out{ false };
};


#endif
