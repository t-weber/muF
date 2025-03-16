/**
 * outputs the syntax tree
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date jun-20
 * @license see 'LICENSE' file
 */

#include "print.h"

#include <iostream>
#include <iomanip>


ASTPrinter::ASTPrinter(std::ostream* ostr)
	: m_ostr{ostr}
{
}


t_astret ASTPrinter::visit(const ASTUMinus* ast)
{
	(*m_ostr) << "<UMinus>\n";
	ast->GetTerm()->accept(this);
	(*m_ostr) << "</UMinus>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTPlus* ast)
{
	(*m_ostr) << "<Plus>\n";
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);
	(*m_ostr) << "</Plus>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTMult* ast)
{
	(*m_ostr) << "<Mult>\n";
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);
	(*m_ostr) << "</Mult>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTMod* ast)
{
	(*m_ostr) << "<Mod>\n";
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);
	(*m_ostr) << "</Mod>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTPow* ast)
{
	(*m_ostr) << "<Pow>\n";
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);
	(*m_ostr) << "</Pow>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNorm* ast)
{
	(*m_ostr) << "<Norm>\n";
	ast->GetTerm()->accept(this);
	(*m_ostr) << "</Norm>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTVar* ast)
{
	(*m_ostr) << "<Var ident=\"" << ast->GetIdent() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTCall* ast)
{
	(*m_ostr) << "<Call ident=\"" << ast->GetIdent() << "\">\n";

	std::size_t argidx = 0;
	for(const auto& arg : ast->GetArgumentList())
	{
		(*m_ostr) << "<arg_" << argidx << ">\n";
		arg->accept(this);
		(*m_ostr) << "</arg_" << argidx << ">\n";
		++argidx;
	}

	(*m_ostr) << "</Call>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTStmts* ast)
{
	(*m_ostr) << "<Stmts>\n";

	std::size_t stmtidx = 0;
	for(const auto& stmt : ast->GetStatementList())
	{
		(*m_ostr) << "<stmt_" << stmtidx << ">\n";
		stmt->accept(this);
		(*m_ostr) << "</stmt_" << stmtidx << ">\n";
		++stmtidx;
	}

	(*m_ostr) << "</Stmts>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTVarDecl* ast)
{
	(*m_ostr) << "<VarDecl>\n";

	std::size_t varidx = 0;
	for(const auto& var : ast->GetVariables())
	{
		(*m_ostr) << "<var_" << varidx << " ident=\"" << var << "\" />\n";
		++varidx;
	}

	if(ast->GetAssignment())
		ast->GetAssignment()->accept(this);

	(*m_ostr) << "</VarDecl>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTFunc* ast)
{
	(*m_ostr) << "<Func ident=\"" << ast->GetIdent() << "\"" << ">\n";

	std::size_t argidx = 0;
	for(const auto& arg : ast->GetArgs())
	{
		t_str argTypeName = Symbol::get_type_name(std::get<1>(arg));

		(*m_ostr) << "<arg_" << argidx << " name=\"" << std::get<0>(arg) << "\"";
		(*m_ostr) << " type=\"" << argTypeName;
		for(std::size_t i = 0; i < std::get<2>(arg).size(); ++i)
			(*m_ostr) << "\" dim" << i << "=\"" << std::get<2>(arg)[i] << "\"";
		(*m_ostr) << " />\n";
		++argidx;
	}

	std::size_t retidx = 0;
	for(const auto& arg : ast->GetRets())
	{
		t_str argTypeName = Symbol::get_type_name(std::get<1>(arg));

		(*m_ostr) << "<ret_" << retidx << " name=\"" << std::get<0>(arg) << "\"";
		(*m_ostr) << " type=\"" << argTypeName;
		for(std::size_t i = 0; i < std::get<2>(arg).size(); ++i)
			(*m_ostr) << "\" dim" << i << "=\"" << std::get<2>(arg)[i] << "\"";
		(*m_ostr) << " />\n";
		++retidx;
	}

	ast->GetStatements()->accept(this);
	(*m_ostr) << "</Func>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTReturn* ast)
{
	if(ast->OnlyJumpToFuncEnd())
	{
		(*m_ostr) << "<Return />\n";
	}
	else
	{
		const auto& retvals = ast->GetRets()->GetList();
		std::size_t numRets = retvals.size();

		if(numRets == 0)
		{
			(*m_ostr) << "<Return />\n";
		}
		else if(numRets == 1)
		{
			(*m_ostr) << "<Return>\n";
			(*retvals.begin())->accept(this);
			(*m_ostr) << "</Return>\n";
		}
		else if(numRets > 1)
		{
			(*m_ostr) << "<MultiReturn>\n";
			std::size_t elemnr = 0;
			for(const auto& elem : retvals)
			{
				(*m_ostr) << "<val_" << elemnr << ">\n";
				elem->accept(this);
				(*m_ostr) << "</val_" << elemnr << ">\n";

				++elemnr;
			}
			(*m_ostr) << "</MultiReturn>\n";
		}
	}

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTAssign* ast)
{
	// empty optional assignment
	if(ast->IsNullAssign())
		return nullptr;

	// multiple assignments
	if(ast->IsMultiAssign())
	{
		(*m_ostr) << "<MultiAssign>\n";

		const auto& idents = ast->GetIdents();
		std::size_t identidx = 0;
		for(const auto& ident : idents)
		{
			(*m_ostr) << "<ident_" << identidx << ">";
			(*m_ostr) << ident;
			(*m_ostr) << "</ident_" << identidx << ">\n";
			++identidx;
		}

		(*m_ostr) << "<rhs>\n";
		ast->GetExpr()->accept(this);
		(*m_ostr) << "</rhs>\n";

		(*m_ostr) << "</MultiAssign>\n";
	}

	// single assignment
	else
	{
		(*m_ostr) << "<Assign ident=\"" << ast->GetIdent() << "\">\n";
		ast->GetExpr()->accept(this);
		(*m_ostr) << "</Assign>\n";
	}

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTVarRange* ast)
{
	(*m_ostr) << "<VarRange ident=\"" << ast->GetIdent() << "\">\n";
	if(ast->GetBegin())
	{
		(*m_ostr) << "<begin>\n";
		ast->GetBegin()->accept(this);
		(*m_ostr) << "</begin>\n";
	}
	if(ast->GetEnd())
	{
		(*m_ostr) << "<end>\n";
		ast->GetEnd()->accept(this);
		(*m_ostr) << "</end>\n";
	}
	if(ast->GetInc())
	{
		(*m_ostr) << "<inc>\n";
		ast->GetInc()->accept(this);
		(*m_ostr) << "</inc>\n";
	}
	(*m_ostr) << "</VarRange>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTArrayAccess* ast)
{
	(*m_ostr) << "<ArrayAccess"
		<< " is_range_12=\"" << ast->IsRanged12() << "\""
		<< ">\n";

	(*m_ostr) << "<idx1>\n";
	ast->GetNum1()->accept(this);
	(*m_ostr) << "</idx1>\n";

	if(ast->GetNum2())
	{
		(*m_ostr) << "<idx2>\n";
		ast->GetNum2()->accept(this);
		(*m_ostr) << "</idx2>\n";
	}

	(*m_ostr) << "<term>\n";
	ast->GetTerm()->accept(this);
	(*m_ostr) << "</term>\n";

	(*m_ostr) << "</ArrayAccess>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTArrayAssign* ast)
{
	(*m_ostr) << "<ArrayAssign ident=\"" << ast->GetIdent() << "\""
		<< " is_range_12=\"" << ast->IsRanged12() << "\""
		<< ">\n";

	(*m_ostr) << "<idx1>\n";
	ast->GetNum1()->accept(this);
	(*m_ostr) << "</idx1>\n";

	if(ast->GetNum2())
	{
		(*m_ostr) << "<idx2>\n";
		ast->GetNum2()->accept(this);
		(*m_ostr) << "</idx2>\n";
	}

	(*m_ostr) << "<expr>\n";
	ast->GetExpr()->accept(this);
	(*m_ostr) << "</expr>\n";

	(*m_ostr) << "</ArrayAssign>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTComp* ast)
{
	(*m_ostr) << "<Comp op=\"";
	switch(ast->GetOp())
	{
		case ASTComp::EQU: (*m_ostr) << "equ"; break;
		case ASTComp::NEQ: (*m_ostr) << "neq"; break;
		case ASTComp::GT: (*m_ostr) << "gt"; break;
		case ASTComp::LT: (*m_ostr) << "lt"; break;
		case ASTComp::GEQ: (*m_ostr) << "geq"; break;
		case ASTComp::LEQ: (*m_ostr) << "leq"; break;
		default: (*m_ostr) << "unknown"; break;
	}
	(*m_ostr) << "\">\n";

	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);
	(*m_ostr) << "</Comp>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTCond* ast)
{
	(*m_ostr) << "<Cond>\n";

	(*m_ostr) << "<cond>\n";
	ast->GetCond()->accept(this);
	(*m_ostr) << "</cond>\n";

	(*m_ostr) << "<if>\n";
	ast->GetIf()->accept(this);
	(*m_ostr) << "</if>\n";

	if(ast->GetElse())
	{
		(*m_ostr) << "<else>\n";
		ast->GetElse()->accept(this);
		(*m_ostr) << "</else>\n";
	}

	(*m_ostr) << "</Cond>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTCases* ast)
{
	(*m_ostr) << "<Cases>\n";

	(*m_ostr) << "<expr>\n";
	ast->GetExpr()->accept(this);
	(*m_ostr) << "</expr>\n";

	for(auto& [ cond, stmts ] : ast->GetCases())
	{
		(*m_ostr) << "<case>\n";
		(*m_ostr) << "<cond>\n";
		cond->accept(this);
		(*m_ostr) << "</cond>\n";

		(*m_ostr) << "<stmts>\n";
		stmts->accept(this);
		(*m_ostr) << "</stmts>\n";
		(*m_ostr) << "</case>\n";
	}

	if(ast->GetDefaultCase())
	{
		(*m_ostr) << "<default>\n";
		ast->GetDefaultCase()->accept(this);
		(*m_ostr) << "</default>\n";
	}

	(*m_ostr) << "</Cases>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTBool* ast)
{
	(*m_ostr) << "<Bool op=\"";
	switch(ast->GetOp())
	{
		case ASTBool::NOT: (*m_ostr) << "not"; break;
		case ASTBool::AND: (*m_ostr) << "and"; break;
		case ASTBool::OR: (*m_ostr) << "or"; break;
		case ASTBool::XOR: (*m_ostr) << "xor"; break;
		default: (*m_ostr) << "unknown"; break;
	}
	(*m_ostr) << "\">\n";

	ast->GetTerm1()->accept(this);
	if(ast->GetTerm2())
		ast->GetTerm2()->accept(this);

	(*m_ostr) << "</Bool>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTLoop* ast)
{
	(*m_ostr) << "<Loop>\n";

	(*m_ostr) << "<cond>\n";
	ast->GetCond()->accept(this);
	(*m_ostr) << "</cond>\n";

	(*m_ostr) << "<stmt>\n";
	ast->GetLoopStmt()->accept(this);
	(*m_ostr) << "</stmt>\n";

	(*m_ostr) << "</Loop>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTRangedLoop* ast)
{
	(*m_ostr) << "<RangedLoop>\n";

	(*m_ostr) << "<range>\n";
	ast->GetRange()->accept(this);
	(*m_ostr) << "</range>\n";

	(*m_ostr) << "<stmt>\n";
	ast->GetLoopStmt()->accept(this);
	(*m_ostr) << "</stmt>\n";

	(*m_ostr) << "</RangedLoop>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTLoopBreak* ast)
{
	(*m_ostr) << "<Break num=\"" << ast->GetNumLoops() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTLoopNext* ast)
{
	(*m_ostr) << "<Next num=\"" << ast->GetNumLoops() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTStrConst* ast)
{
	(*m_ostr) << "<Const type=\"str\" val=\"" << ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTExprList* ast)
{
	(*m_ostr) << "<ExprList>\n";

	std::size_t expridx = 0;
	for(const auto& expr : ast->GetList())
	{
		(*m_ostr) << "<expr_" << expridx << ">\n";
		expr->accept(this);
		(*m_ostr) << "</expr_" << expridx << ">\n";
		++expridx;
	}

	(*m_ostr) << "</ExprList>\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConst<t_real>* ast)
{
	(*m_ostr) << "<Const type=\"scalar\" val=\"" << ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConst<t_int>* ast)
{
	(*m_ostr) << "<Const type=\"integer\" val=\"" << ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConstList<t_int>* ast)
{
	(*m_ostr) << "<Const type=\"integers\" val=\"";
	for(t_int val : ast->GetValues())
		(*m_ostr) << val << ", ";
	(*m_ostr) << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConst<t_cplx>* ast)
{
	(*m_ostr) << "<Const type=\"complex\" val=\"" << ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConst<t_quat>* ast)
{
	using namespace m_ops;
	(*m_ostr) << "<Const type=\"quaternion\" val=\"" << ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTNumConst<bool>* ast)
{
	(*m_ostr) << "<Const type=\"bool\" val=\"" << std::boolalpha
		<< ast->GetVal() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTLabel* ast)
{
	(*m_ostr) << "<Label ident=\"" << ast->GetIdent() << "\" />\n";

	return nullptr;
}


t_astret ASTPrinter::visit(const ASTJump* ast)
{
	if(ast->IsComefrom())
		(*m_ostr) << "<Comefrom label=\"" << ast->GetLabel() << "\" />\n";
	else
		(*m_ostr) << "<Jump label=\"" << ast->GetLabel() << "\" />\n";

	return nullptr;
}
