# int factorial(int n) {
#   if (n == 0) {
#     return 1;
#   }
# 
#   return n * factorial(n - 1);
# }
# 
# int main() {
#   int n, fact;
#   scanf("%d", &n);
#   fact = factorial(n);
#   printf("factorial of %d is %d\n", n, fact);
# }

.data
msg1: .asciiz "factorial of"
msg2: .asciiz " is" 

.text
main:

  # Calls read_int
  li $v0, 5
  syscall

  # Store the input value
  move $t0, $v0

  # Jumps into procedure
  move $a0, $t0
  jal factorial

  # Stores the returned value from function
  move $t1, $v0 

  # Prints message
  li $v0, 4
  la $a0, msg1
  syscall

  li $v0, 1
  move $a0, $t0
  syscall

  li $v0, 4
  la $a0, msg2
  syscall

  li $v0, 1
  move $a0, $t1
  syscall

  li $v0, 11
  li $a0, 10
  syscall

  # Ends program
  li $v0, 10
  syscall

factorial:
  # Store return address ($ra) and function parameter ($a0) in stack
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $a0, 4($sp)

  # Decrement $a0
  addi $a0, $a0, -1 

  # If it's equals to zero return 1
  beq $a0, $zero, return

  # Call factorial again
  jal factorial

  # Returning from call stack, retrieve return address and function parameter from stack
  lw $ra, 0($sp)
  lw $a0, 4($sp)
  addi $sp, $sp, 8

  # Multiply the returned value with the parameter
  mul $a0, $a0, $v0

  # Return from function call
  move $v0, $a0
  jr $ra
  
return:
  li $v0, 1  
  jr $ra