#
# MicroF compiler and VM
# @author Tobias Weber (orcid: 0000-0002-7230-1932)
# @date February 2025
# @license: see 'LICENSE.GPL' file
#

cmake_minimum_required(VERSION 3.5)

project(muF)
list(APPEND CMAKE_MODULE_PATH
	${PROJECT_SOURCE_DIR}
	/usr/local/lib/cmake
	/usr/local/lib64/cmake
)

option(USE_BOOST_GIL "use boost.gil" FALSE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-Wall -Wextra)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

add_definitions(${Boost_CXX_FLAGS})

if(USE_BOOST_GIL)
	find_package(PNG REQUIRED)
	add_definitions(${PNG_DEFINITIONS} -DUSE_BOOST_GIL)
	include_directories(${PNG_INCLUDE_DIRS})
endif()


find_package(LibLalr1 REQUIRED)
find_package(Mathlibs REQUIRED)


include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"${Mathlibs_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}/.."
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_BINARY_DIR}"
)


add_executable(parsergen
	src/parser/parsergen.cpp src/parser/grammar.cpp
	src/parser/func.cpp src/parser/loops.cpp
	src/parser/ops.cpp src/parser/var.cpp
	src/parser/arr.cpp
)

target_compile_definitions(parsergen
	PUBLIC -DCREATE_PRODUCTION_RULES
)

target_include_directories(parsergen
	PUBLIC ${LibLalr1_INCLUDE_DIRECTORIES}
)

target_link_libraries(parsergen
	${LibLalr1_LIBRARIES}
	${Boost_LIBRARIES}
)


if(EXISTS "${CMAKE_BINARY_DIR}/parser.cpp")
	message("Building recursive-ascent compiler.")

	add_executable(compile
		src/codegen/main.cpp
		src/codegen/codegen.cpp src/codegen/codegen.h
		src/codegen/var.cpp src/codegen/arr.cpp
		src/codegen/func.cpp src/codegen/ops.cpp
		src/codegen/loops.cpp
		src/codegen/consttab.cpp src/codegen/consttab.h

		src/parser/lexer.cpp src/parser/lexer.h
		${CMAKE_BINARY_DIR}/parser.cpp ${CMAKE_BINARY_DIR}/parser.h
		src/parser/grammar.cpp src/parser/grammar.h
		src/parser/func.cpp src/parser/loops.cpp
		src/parser/ops.cpp src/parser/var.cpp
		src/parser/arr.cpp

		src/common/sym.cpp src/common/sym.h
		src/ast/ast.h
		src/ast/printast.cpp src/ast/printast.h
	)

	target_compile_definitions(compile
		PUBLIC -DCREATE_SEMANTIC_RULES -DUSE_RECASC=1
	)

	target_include_directories(compile
		PUBLIC ${LibLalr1_INCLUDE_DIRECTORIES}
	)

	target_link_libraries(compile
		#${LibLalr1_LIBRARIES}
		${Boost_LIBRARIES}
	)
elseif(EXISTS "${CMAKE_BINARY_DIR}/parser.tab")
	message("Building table-based compiler.")

	add_executable(compile
		src/codegen/main.cpp
		src/codegen/codegen.cpp src/codegen/codegen.h
		src/codegen/var.cpp src/codegen/arr.cpp
		src/codegen/func.cpp src/codegen/ops.cpp
		src/codegen/consttab.cpp src/codegen/consttab.h

		src/parser/lexer.cpp src/parser/lexer.h
		src/parser/grammar.cpp src/parser/grammar.h
		src/parser/func.cpp src/parser/loops.cpp
		src/parser/ops.cpp src/parser/var.cpp
		src/parser/arr.cpp

		src/common/sym.cpp src/common/sym.h
		src/ast/ast.h
		src/ast/printast.cpp src/ast/printast.h
	)

	target_compile_definitions(compile
		PUBLIC -DCREATE_SEMANTIC_RULES -DUSE_RECASC=0
	)

	target_include_directories(compile
		PUBLIC ${LibLalr1_INCLUDE_DIRECTORIES}
	)

	target_link_libraries(compile
		${LibLalr1Parser_LIBRARIES} #${LibLalr1_LIBRARIES}
		${Boost_LIBRARIES}
	)
endif()



# -----------------------------------------------------------------------------
# vm
# -----------------------------------------------------------------------------
add_executable(vm
	src/common/types.h
	src/vm/main.cpp src/vm/types.h
	src/vm/opcodes.h src/vm/vm.h
	src/vm/ops.h src/vm/mem.h
	src/vm/vm.cpp src/vm/run.cpp
	src/vm/extfuncs.cpp
	src/vm/memdump.cpp
)

target_link_libraries(vm ${Boost_LIBRARIES}
	$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>
	$<$<TARGET_EXISTS:PNG::PNG>:PNG::PNG>
)
# -----------------------------------------------------------------------------
