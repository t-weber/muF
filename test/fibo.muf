!
! calculation of fibonacci numbers and factorials
!


function integer fibo(integer n)
	if n <= 1 then
		return 1
	else
		return fibo(n - 1) + fibo(n - 2)
	end if
end function


function integer fac(integer n)
	if n <= 1 then
		return 1
	end if

	return n*fac(n - 1)
end function


program fibo
	do while(1 == 1)
		integer :: n = getint("n = ")
		if n < 0 then
			break
		end if

		print(n + "! = " + fac(n))
		print("fib(" + n + ") = " + fibo(n))
	end do
end program
