!
! calculation of fibonacci numbers and factorials
!


function fibo(n) result(m)
	integer :: n
	integer :: m

	if(n <= 1) then
		m = 1
	else
		m = fibo(n - 1) + fibo(n - 2)
	end if
end function


function fac(n) result(m)
	integer :: n
	integer :: m

	if(n <= 1) then
		m = 1
		return
	end if

	m = n*fac(n - 1)
end function


program fibo
	do while(.true.)
		integer :: n = getint("n = ")
		if(n < 0) then
			break
		end if

		print(n + "! = " + fac(n))
		print("fib(" + n + ") = " + fibo(n))
	end do
end program
