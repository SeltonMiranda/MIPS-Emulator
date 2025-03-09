.text
main:
  # Reads the first number
  li $v0, 5
  syscall # Calls read_int

  move $t0, $v0

  # Reads the second number
  li $v0, 5
  syscall # Calls read_int

  move $t1, $v0

  # The sum of them is in $t2
  add $t2, $t1, $t0

  # Divide by 2, the result is stored in $t3
  srl $t3, $t2, 1

  li $v0, 1
  move $a0, $t3 
  syscall # Calls print_int

  jal print_new_line # Jumps to procedure

  # Exit the program
  li $v0, 10
  syscall

print_new_line:
  li $v0, 11
  li $a0, 10 # Ascii "\n"
  syscall # Calls print_char
  jr $ra # returns from procedure