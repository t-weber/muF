!
! calculation of fibonacci numbers and factorials
!


recursive function fibo(n) result(m)
	integer, intent(in) :: n
	integer :: m

	if(n <= 1) then
		m = 1
	else
		m = fibo(n - 1) + fibo(n - 2)
	end if
end function


recursive function fac(n) result(m)
	integer, intent(in) :: n
	integer :: m

	if(n <= 1) then
		m = 1
		return
	end if

	m = n*fac(n - 1)
end function


program fibotest
	integer :: n
	integer :: fibo, fac  ! optional re-declaration of functions

	do while(.true.)
		n = getint("n = ")
		if(n < 0) then
			exit
		end if

		print(n + "! = " + fac(n))
		print("fib(" + n + ") = " + fibo(n))
	end do
end program
