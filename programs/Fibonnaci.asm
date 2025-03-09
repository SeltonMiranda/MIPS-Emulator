# Calculates "n" iterations of fibonacci sequence
# int fibonacci(int n) {
#   int first = 1;
#   int second = 1;
#   for (int i = 0; i < n; i++) {
#     int next = first + second; 
#     first = second;
#     second = next;
#   }
#   return second;
# }
# 
# 
# void main() {
#   int iter, fibo;
#   scanf("%d", &iter);
#   fibo = fibonacci(n);
#   printf("%d\n", fibo);
# }

.text
main:

  # Calls read_int
  li $v0, 5
  syscall

  # Prepares argument for function call
  move $a0, $v0
  jal fibonacci

  # Prepare arguments for syscall
  # in $v0 is the returned value from fibonacci procedure
  move $a0, $v0 
  li $v0, 1
  syscall 

  li $v0, 11
  li $a0, 10
  syscall

  # Ends program
  li $v0, 10
  syscall

fibonacci:
  li $t0, 1 # first number
  li $t1, 1 # second number

  li $t2, 0 # iterator

for:
  bge $t2, $a0, return
  add $t3, $t0, $t1 # Calculates next
  move $t0, $t1
  move $t1, $t3
  addi $t2, $t2, 1
  j for

return:
  move $v0, $t1
  jr $ra