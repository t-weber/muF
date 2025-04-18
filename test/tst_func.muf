function func1() result()
	write("Test 1")
end function


function func2(a) result(x)
	integer :: a, x

	write("\nTest 2")
	x = 2 * a
end function


function func3(a, b) result(x, y)
	integer :: a, b
	integer :: x, y

	write("\nTest 3")
	x = 3 * a
	y = 2 * b
end function


function func4(a, b) result(x, y)
	integer :: a, x
	real :: b, y

	write("\nTest 4")
	x = 3 * a
	y = 2.5 * b
end function


function func5(a) result(x)
	integer, dimension(4) :: a, x

	write("\nTest 5")
	x = 2 * a
end function


function func6(a, b) result(x)
	integer, dimension(4) :: a, b, x

	write("\nTest 6")
	x = a + b
end function


program func_test
	func1()

	integer :: i, j
	i = func2(123)
	write("j = " + i)

	assign i, j = func3(123, 444)
	write("j = " + i + ", j = " + j)

	real :: k
	assign i, k = func4(123, 555.)
	write("j = " + i + ", k = " + k)

	integer, dimension(4) :: a = [ 1, 2, 3, 4 ]
	a = func5(a)
	write("a = " + a)

	integer, dimension(4) :: b = [ 9, 8, 7, 6 ]
	a = func6(a, b)
	write("a = " + a)
end program
