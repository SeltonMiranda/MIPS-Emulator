.text
main:
  li $t0, 10 # 1..10
  li $t1, 1  # iterator
  li $t2, 0  # sum

while:
  blt $t0, $t1, out
  add $t2, $t2, $t1 
  addi $t1, $t1, 1
  j while
out:
  li $v0, 1
  move $a0, $t2
  syscall

  li $v0, 10
  syscall