!
! calculation of fibonacci numbers and factorials
!

program fibo


function integer fibo(integer n)
	if n <= 1 then
		ret 1;
	else
		ret fibo(n-1) + fibo(n-2);
	end if
end function


function integer fac(integer n)
	if n <= 1 then
		ret 1;
	end if

	ret n*fac(n-1);
end function


do while(1==1)
	integer :: n = getint("n = ");
	if n < 0 then
		break;
	end if

	print(n + "! = " + fac(n));
	print("fib(" + n + ") = " + fibo(n));
end do


end program
