/**
 * llvm zero-address code generator
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#ifndef __ZEROACASM_H__
#define __ZEROACASM_H__

#include "ast/ast.h"
#include "consttab.h"
#include "vm/opcodes.h"

#include <stack>
#include <unordered_map>


/**
 * zero-address code generation
 * (the return value is only used for type information for casting)
 */
class ZeroACAsm : public ASTVisitor
{
public:
	ZeroACAsm(SymTab* syms, std::ostream* ostr = &std::cout);
	virtual ~ZeroACAsm();

	ZeroACAsm(const ZeroACAsm&) = delete;
	const ZeroACAsm& operator=(const ZeroACAsm&) = delete;

	virtual t_astret visit(const ASTUMinus* ast) override;
	virtual t_astret visit(const ASTPlus* ast) override;
	virtual t_astret visit(const ASTMult* ast) override;
	virtual t_astret visit(const ASTMod* ast) override;
	virtual t_astret visit(const ASTPow* ast) override;
	virtual t_astret visit(const ASTNorm* ast) override;

	virtual t_astret visit(const ASTVarDecl* ast) override;
	virtual t_astret visit(const ASTVar* ast) override;
	virtual t_astret visit(const ASTAssign* ast) override;
	virtual t_astret visit(const ASTVarRange* ast) override;

	virtual t_astret visit(const ASTArrayAccess* ast) override;
	virtual t_astret visit(const ASTArrayAssign* ast) override;

	virtual t_astret visit(const ASTNumConst<t_real>* ast) override;
	virtual t_astret visit(const ASTNumConst<t_int>* ast) override;
	virtual t_astret visit(const ASTNumConst<t_cplx>* ast) override;
	virtual t_astret visit(const ASTNumConst<bool>* ast) override;
	virtual t_astret visit(const ASTStrConst* ast) override;

	virtual t_astret visit(const ASTFunc* ast) override;
	virtual t_astret visit(const ASTCall* ast) override;
	virtual t_astret visit(const ASTReturn* ast) override;
	virtual t_astret visit(const ASTStmts* ast) override;

	virtual t_astret visit(const ASTCond* ast) override;
	virtual t_astret visit(const ASTLoop* ast) override;
	virtual t_astret visit(const ASTRangedLoop* ast) override;
	virtual t_astret visit(const ASTLoopBreak* ast) override;
	virtual t_astret visit(const ASTLoopNext* ast) override;

	virtual t_astret visit(const ASTComp* ast) override;
	virtual t_astret visit(const ASTBool* ast) override;
	virtual t_astret visit(const ASTExprList* ast) override;

	virtual t_astret visit(const ASTLabel* ast) override;
	virtual t_astret visit(const ASTJump* ast) override;

	// ------------------------------------------------------------------------
	// internally handled dummy nodes
	// ------------------------------------------------------------------------
	virtual t_astret visit(const ASTArgNames*) override { return nullptr; }
	virtual t_astret visit(const ASTTypeDecl*) override { return nullptr; }
	// ------------------------------------------------------------------------

	void Start();
	void Finish();

	void SetDebug(bool b) { m_debug = b; }


protected:
	// finds the symbol with a specific name in the symbol table
	t_astret GetSym(const t_str& name) const;

	// finds the size of the symbol for the stack frame
	std::size_t GetSymSize(const Symbol* sym) const;

	// finds the size of the local function variables for the stack frame
	std::size_t GetStackFrameSize(const Symbol* func) const;

	// returns common type of a binary operation
	std::tuple<t_astret, t_astret, t_astret>
	GetCastSymType(t_astret term1, t_astret term2);

	// emits code to cast to given type
	void CastTo(t_astret ty_to,
		const std::optional<std::streampos>& pos = std::nullopt,
		bool allow_array_cast = false);

	// push constants
	void PushRealConst(t_vm_real);
	void PushIntConst(t_vm_int);
	void PushCplxConst(const t_vm_cplx&);
	void PushBoolConst(t_vm_bool);
	void PushStrConst(const t_vm_str& str);
	void PushVecSize(std::size_t size);
	void PushRealVecConst(const std::vector<t_vm_real>& vec);
	void PushIntVecConst(const std::vector<t_vm_int>& vec);
	void PushCplxVecConst(const std::vector<t_vm_cplx>& vec);

	void AssignVar(t_astret sym);
	void CallExternal(const t_str& funcname);

	Symbol* GetTypeConst(SymbolType ty) const;
	std::pair<Symbol*, Symbol*> GetArrayTypeConst(SymbolType ty) const;


private:
	// symbol table
	SymTab* m_syms{nullptr};

	// constants table
	ConstTab m_consttab{};

	// code output
	std::ostream* m_ostr{&std::cout};

	// currently active function scope
	std::vector<t_str> m_curscope{};
	// current address on stack for local variables
	std::unordered_map<t_str, t_vm_addr> m_local_stack{};
	// current address on stack for global variables
	t_vm_addr m_global_stack{};

	// stream positions where addresses need to be patched in
	std::vector<std::tuple<t_str, std::streampos, t_vm_addr, const AST*>>
		m_func_comefroms{};
	std::vector<std::streampos> m_endfunc_comefroms{};
	std::vector<std::tuple<std::streampos, std::streampos>> m_const_addrs{};

	// currently active loops in function
	std::size_t m_loop_ident{0};  // loop unique ident counter
	std::vector<std::size_t> m_cur_loop{};
	std::unordered_multimap<std::size_t, std::streampos>
		m_loop_begin_comefroms{}, m_loop_end_comefroms{};

	// addresses of labels
	std::unordered_map<t_str, std::streampos> m_labels{};
	std::vector<std::pair<t_str, std::streampos>> m_goto_comefroms{};

	// dummy symbols for constants
	Symbol *m_real_const{}, *m_int_const{}, *m_cplx_const{};
	Symbol *m_real_array_const{}, *m_int_array_const{}, *m_cplx_array_const{};
	Symbol *m_bool_const{}, *m_str_const{};

	bool m_debug{false};
};


#endif
