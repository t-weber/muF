!
! calculation of fibonacci numbers and factorials
!


function fibo(integer :: n) result(integer)
	if n <= 1 then
		return 1
	else
		return fibo(n - 1) + fibo(n - 2)
	end if
end function


function fac(integer :: n) result(integer)
	if n <= 1 then
		return 1
	end if

	return n*fac(n - 1)
end function


program fibo
	do while(.true.)
		integer :: n = getint("n = ")
		if n < 0 then
			break
		end if

		print(n + "! = " + fac(n))
		print("fib(" + n + ") = " + fibo(n))
	end do
end program
