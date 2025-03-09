# void main() {
#   int number;
#   scanf("%d", &number);
#   if (number % 2 == 0) {
#     printf("is even\n");
#   } else {
#     printf("is odd\n");
#   }
# }

.data
number: .space 4
even: .asciiz "is even\n"
odd: .asciiz "is odd\n"

.text
main:

  # Calls read_int
  # The input is returned in $v0 register
  li $v0, 5
  syscall

  # The function's parameters should be in $a0, $a1, ...
  move $a0, $v0 
  jal compute_remainder  

  beq $v0, $zero, isEven 

  # is odd
  li $v0, 4
  la $a0, odd
  syscall # Calls print_string

  j out_if

isEven:
  li $v0, 4 
  la $a0, even
  syscall # Calls print_string

out_if:

  # Ends program
  li $v0, 10
  syscall

# Using the subtract method to calculate the remainder o
compute_remainder:
  move $t0, $a0 # dividend
  li $t1, 2 # divider
  li $t2, 0 # remainder

while:
  blt $t0, $t1, return
  sub $t0, $t0, $t1
  move $t2, $t0
  j while
return:
  # Remainder from division is in $t2
  # We move it to $v0 because is where returned function values should be
  move $v0, $t2 
  jr $ra
