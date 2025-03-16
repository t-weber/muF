/**
 * zero-address code vm -- operators
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 15-march-2025
 * @license see 'LICENSE' file
 */

#include "vm.h"
#include "ops.h"


void VM::OpMatrixMultiplication()
{
	t_int M2_cols = std::get<m_intidx>(PopData());
	t_int M2_rows = std::get<m_intidx>(PopData());
	t_int M1_cols = std::get<m_intidx>(PopData());
	t_int M1_rows = std::get<m_intidx>(PopData());

	t_data M2 = PopData();
	t_data M1 = PopData();

	auto mat_mult = [&M1, &M2, M1_cols, M1_rows, M2_cols, M2_rows]<class t_vec>()
		-> std::pair<bool, t_data>
	{
		using t_elem = typename t_vec::value_type;
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
		//constexpr const std::size_t elem_type_idx = GetDataTypeIndex<t_elem>();

		if(M1.index() != vec_idx || M2.index() != vec_idx)
			return std::make_pair(false, t_data{});

		// matrices as flat vectors
		const t_vec& M1_vec = std::get<vec_idx>(M1);
		const t_vec& M2_vec = std::get<vec_idx>(M2);

		// multiply matrices
		//using t_mat = m::MatrixDyn<t_elem>;
		using t_mat = m::mat<t_elem, std::vector>;
		t_mat M1 = m::create<t_mat>(M1_rows, M1_cols, M1_vec);
		t_mat M2 = m::create<t_mat>(M2_rows, M2_cols, M2_vec);
		t_mat prod = M1 * M2;

		// return product matrix as flat vector
		t_vec prod_vec{prod.data(), prod.size1()*prod.size2()};
		return std::make_pair(true,
			t_data{std::in_place_index<vec_idx>, std::move(prod_vec)});
	};

	t_data result;

	// real matrix multiplication
	if(auto [ ok, res ] = mat_mult.template operator()<t_vec_real>(); ok)
		result = std::move(res);

	// int matrix multiplication
	else if(auto [ ok, res ] = mat_mult.template operator()<t_vec_int>(); ok)
		result = std::move(res);

	// complex matrix multiplication
	else if(auto [ ok, res ] = mat_mult.template operator()<t_vec_cplx>(); ok)
		result = std::move(res);

	PushData(result);
}
