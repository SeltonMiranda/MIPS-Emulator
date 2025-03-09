#   int rfibonacci(int n) {
#     if (n == 0)
#       return 0;
#     if (n == 1)
#       return 1;
#     return rfibonacci(n-1) + rfibonacci(n-2);
#   }
#   
#   void main() {
#     int n, fibo;
#     scanf("%d", &n);
#     fibo = rfibonacci(n);
#     printf("O número %d da sequencia eh: %d", n, fibo);
#   }

.data
msg1: .asciiz: "o numero"
msg2: .asciiz: "da sequencia eh:"
new_line: .word 10 # '\n'

.text
main:

  # Reads an int
  li $v0, 5
  syscall

  # Stores the input
  move $a0, $v0

  # Calls function 
  jal rfibonacci

  # Stores value returned from function
  move $t0, $v0

  # Prints msg1
  li $v0, 4
  la $a0, msg1
  syscall

  # Prints variable "n"
  li $v0, 1
  move $a0, $a0
  syscall

  # Prints msg2
  li $v0, 4
  la $a0, msg2
  syscall

  li $v0, 1
  move $a0, $t0
  syscall

  # Prints new line
  li $v0, 11
  la $a0, new_line
  syscall

  # Ends program
  li $v0, 10
  syscall

# Recursive fibonacci function
# Takes an parameter "n"
# Returns the "nth" term
rfibonacci:

  # if n == 0 => return 0
  beq $a0, $zero, base_case_1

  # if n == 1 => return 1
  li $t1, 1
  beq $a0, $t1, base_case_2

  # Allocates space in stack and store $ra and $a0 in stack
  addi $sp, $sp, -12
  sw $ra, 0($sp)
  sw $a0, 4($sp)

  addi $a0, $a0, -1       # n = n - 1

  # Calls rfibonacci(n-1)
  jal rfibonacci

  # Stores value from rfibonacci(n-1)
  sw $v0, 8($sp)

  # Retrieve the value returned before call rfibonacci(n-1)
  lw $a0, 4($sp)
  addi $a0, $a0, -2       # n = n - 2

  # Calls rfibonacci(n-2)
  jal rfibonacci

  # Retrieve the value returned from rfibonacci(n-1)
  lw $t1, 8($sp)

  # Sum the returned value from rfibonacci(n-2) plus the value from rfibonacci(n-1) which is $t1
  add $v0, $v0, $t1

  # Retrive return address from stack
  lw $ra, 0($sp)

  # Deallocates stack
  addi $sp, $sp, 12

  # Return from function
  jr $ra
  
base_case_1:
  li $v0, 0
  jr $ra 

base_case_2:
  li $v0, 1
  jr $ra


