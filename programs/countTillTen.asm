# void main() {
#   for (int i = 1; i <= 10; i++) {
#     printf("%d", i);
#   }
# }

.text
main:
  li $t0, 10 # 1..10
  li $t1, 1  # iterator

for:
  blt $t0, $t1, done

  # Syscall print_int
  li $v0, 1
  move $a0, $t1
  syscall

  jal print_new_line

  addi $t1, $t1, 1 # Increments iterator
  j for

done:
  #Ends program
  li $v0, 10
  syscall

print_new_line:
  li $v0, 11
  li $a0, 10 # Ascii "\n"
  syscall # Calls print_char
  jr $ra # returns from procedure