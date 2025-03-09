# int factorial(int n) {
#   int f = 1; 
#   while (n > 0) {
#     f = f * n;
#     n--;
#   }
#   return f;
# }
# 
# void main() {
#   int n, fact;
#   scanf("%d", &n);
#   fact = factorial(n);
#   printf("factorial of% d is: %d\n", n, fact);
# }

.data
msg1: .asciiz "factorial of"
msg2: .asciiz " is" 

.text
main:

  # Calls read_int
  li $v0, 5
  syscall

  move $a0, $v0
  jal factorial

  move $t0, $v0
  move $t1, $a0

  # Prints message
  li $v0, 4
  la $a0, msg1
  syscall

  li $v0, 1
  move $a0, $t1
  syscall

  li $v0, 4
  la $a0, msg2
  syscall

  li $v0, 1
  move $a0, $t0
  syscall

  li $v0, 11
  li $a0, 10
  syscall

  # Ends program
  li $v0, 10
  syscall

factorial:
  li $t0, 1 # f = 1
  move $t1, $a0

while:
  beq $t1, $zero, return 
  mul $t0, $t1, $t0
  addi $t1, $t1, -1
  j while

return:
  move $v0, $t0
  jr $ra