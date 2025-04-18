!
! combinatorics
!


!
! recursive factorial
!
recursive function fac_rec(n) result(res)
	integer, intent(in) :: n
	integer :: res

	if(n <= 1) then
		res = 1
		return
	end if

	res = n * fac_rec(n - 1)
end function


!
! iterative factorial
!
function fac(n) result(res)
	integer, intent(in) :: n
	integer :: res
	integer :: i = 1

	res = 1
	if(n <= 1) then
		return
	end if

	do i = 2, n
		res = res * i
	end do
end function


!
! binomial coefficient
! see: https://en.wikipedia.org/wiki/Binomial_coefficient
!
function binom(n, k) result(res)
	integer, intent(in) :: n, k
	integer :: res
	integer :: fac

	res = fac(n) / (fac(n - k) * fac(k))
end function


!
! variation (partial permutation) of a tuple without repetition
! see: https://de.wikipedia.org/wiki/Variation_(Kombinatorik)
!
function var_norep(n, k) result(res)
	integer, intent(in) :: n, k
	integer :: res
	integer :: fac

	if(k > n) then
		res = 0
		return
	end if

	res = fac(n) / fac(n - k)
end function


!
! variation of a tuple with repetition
! see: https://de.wikipedia.org/wiki/Variation_(Kombinatorik)
!
function var_rep(n, k) result(res)
	integer, intent(in) :: n, k
	integer :: res

	res = n**k
end function


!
! combination of a set without repetition
! see: https://en.wikipedia.org/wiki/Combination
!
function comb_norep(n, k) result(res)
	integer, intent(in) :: n, k
	integer :: res
	integer :: binom

	if(k > n) then
		res = 0
		return
	end if

	res = binom(n, k)
end function


!
! combination of a set with repetition
! see: https://en.wikipedia.org/wiki/Combination
!
function comb_rep(n, k) result(res)
	integer, intent(in) :: n, k
	integer :: res
	integer :: binom

	res = binom(n + k - 1, k)
end function


program comb
	integer :: n, k
	integer :: binom, var_norep, var_rep, comb_norep, comb_rep

	do while(.true.)
		!n = read_integer("n = ")
		!k = read_integer("k = ")
		print*, "n, k = "
		read*, n, k
		if(n < 0 .or. k < 0) then
			exit
		end if

		!print*, "B(", n, ", ", k, ") = ", binom(n, k)
		print*, "Number of ways to select: "
		print*, "\t- a ", k, "-tuple from a ", n, "-set without repetitions: ", var_norep(n, k)
		print*, "\t- a ", k, "-tuple from a ", n, "-set with    repetitions: ", var_rep(n, k)
		print*, "\t- a ", k, "-set   from a ", n, "-set without repetitions: ", comb_norep(n, k)
		print*, "\t- a ", k, "-set   from a ", n, "-set with    repetitions: ", comb_rep(n, k)
		print*, ""
	end do
end program
