# char *string = "Computer Science"
#
# void main() {
#  int count = 0;
#  for (int i = 0; string[i] != '\0'; i++) {
#   count++;
#  } 
#  printf("%d", count);
# }

.data
string: .asciiz "Computer Science"

.text
main:
  la $t0, string  # start address
  li $t1, 0       # iterator
  
  add $t2, $t0, $t1 # first char
  lbu $t3, $t2, 0    # Loads (&string + $t0)
for:
  beq $t3, $zero, out 
  addi $t1, $t1, 1
  add $t2, $t0, $t1
  lbu $t3, 0($t2)
  j for
out:
  # Removes '\0' from counting
  addi $t1, $t1, -1 

  # Prints the string's length
  li $v0, 1
  move $a0, $t1
  syscall

  # Exit program
  li $v0, 10
  syscall