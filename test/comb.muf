!
! combinatorics
!


!
! recursive factorial
!
function fac_rec(n) result(integer)
	integer :: n

	if(n <= 1) then
		return 1
	end if

	return n*fac(n - 1)
end function


!
! iterative factorial
!
function fac(n) result(integer)
	integer :: n

	if(n <= 1) then
		return 1
	end if

	integer :: res = 1
	integer :: i = 1

	do i = 1, n
		res = res * i
	end do

	return res
end function


!
! binomial coefficient
! see: https://en.wikipedia.org/wiki/Binomial_coefficient
!
function binom(n, k) result(integer)
	integer :: n, k

	return fac(n) / (fac(n - k) * fac(k))
end function


!
! variation of a tuple without repetition
! see: https://de.wikipedia.org/wiki/Variation_(Kombinatorik)
!
function var_norep(n, k) result(integer)
	integer :: n, k

	if(k > n) then
		return 0
	end if

	return fac(n) / fac(n - k)
end function


!
! variation of a tuple with repetition
! see: https://de.wikipedia.org/wiki/Variation_(Kombinatorik)
!
function var_rep(n, k) result(integer)
	integer :: n, k
	return n**k
end function


!
! combination of a set without repetition
! see: https://en.wikipedia.org/wiki/Combination
!
function comb_norep(n, k) result(integer)
	integer :: n, k
	if(k > n) then
		return 0
	end if

	return binom(n, k)
end function


!
! combination of a set with repetition
! see: https://en.wikipedia.org/wiki/Combination
!
function comb_rep(n, k) result(integer)
	integer :: n, k
	return binom(n + k - 1, k)
end function


program comb
	do while(.true.)
		integer :: n = getint("n = ")
		integer :: k = getint("k = ")
		if(n < 0 .or. k < 0) then
			break
		end if

		!print("B(" + n + ", " + k + ") = " + binom(n, k))
		print("Number of ways to select: ")
		print("\t- a " + k + "-tuple from a " + n + "-set without repetitions: " + var_norep(n, k))
		print("\t- a " + k + "-tuple from a " + n + "-set with    repetitions: " + var_rep(n, k))
		print("\t- a " + k + "-set   from a " + n + "-set without repetitions: " + comb_norep(n, k))
		print("\t- a " + k + "-set   from a " + n + "-set with    repetitions: " + comb_rep(n, k))
		print("")
	end do
end program
