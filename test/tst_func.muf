function func1() result()
	print("Test 1")
end function


function func2(a) result(x)
	integer :: a, x

	print("\nTest 2")
	x = 2 * a
end function


function func3(a, b) result(x, y)
	integer :: a, b
	integer :: x, y

	print("\nTest 3")
	x = 3 * a
	y = 2 * b
end function


function func4(a, b) result(x, y)
	integer :: a, x
	real :: b, y

	print("\nTest 4")
	x = 3 * a
	y = 2.5 * b
end function


function func5(a) result(x)
	integer, dimension(4) :: a, x

	print("\nTest 5")
	x = 2 * a
end function


function func6(a, b) result(x)
	integer, dimension(4) :: a, b, x

	print("\nTest 6")
	x = a + b
end function


program func_test
	func1()

	integer :: i, j
	i = func2(123)
	print("j = " + i)

	assign i, j = func3(123, 444)
	print("j = " + i + ", j = " + j)

	real :: k
	assign i, k = func4(123, 555.)
	print("j = " + i + ", k = " + k)

	integer, dimension(4) :: a = [ 1, 2, 3, 4 ]
	a = func5(a)
	print("a = " + a)

	integer, dimension(4) :: b = [ 9, 8, 7, 6 ]
	a = func6(a, b)
	print("a = " + a)
end program
