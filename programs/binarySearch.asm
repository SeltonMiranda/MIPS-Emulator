# array[5] = {24, 42, 58, 455, 1024}
# 
# int binarySearch(int n) {
#   int start = 0;
#   int end = 4;
#   while (start <= end) {
#     int mid = (start + end) / 2;
#     
#     if (array[mid] == n) {
#       return mid;
#     }
# 
#     if (array[mid] < n) {
#       start = mid + 1;
#     } else {
#       end = mid - 1;
#     }
#   }
# 
#   return -1;
# }
# 
# int main() {
#   int number_to_found, pos;
#   scanf("%d", &number_to_found);
#   pos = binarySearch(number_to_found);
#   printf("position %d\n", pos); 
#   return 0;
# }

.data
array: .word -1, 2, 58, 233, 1024  # Sorted array
msg: .asciiz "pos"                # Output message

.text
main:
  # Read an integer from user input
  li $v0, 5
  syscall

  # Pass input value to binarySearch
  move $a0, $v0
  jal binarySearch
  move $t0, $v0  # Store the result (position)

  # Print message "pos"
  li $v0, 4
  la $a0, msg
  syscall

  # Print the position
  li $v0, 1
  move $a0, $t0
  syscall

  # Print a newline
  li $v0, 11
  li $a0, 10
  syscall

  # Exit the program
  li $v0, 10
  syscall

# Binary search function
# Input: $a0 = target value
# Output: $v0 = index of the target value, or -1 if not found
# Registers:
#   $t0 = start index
#   $t1 = end index
#   $t3 = array base address
#   $t4 = mid index
#   $t5 = offset (mid index * 4)
#   $t6 = address of array[mid]
#   $t7 = array[mid]
#   $t8 = target value

binarySearch:
  li $t0, 0     # Start index = 0
  li $t1, 4     # End index = 4 (array has 5 elements)
  la $t3, array # Load array base address
  move $t8, $a0 # Store target value

while:
  # If end < start, element not found
  blt $t1, $t0, not_found

  # Compute mid index: (start + end) / 2
  add $t4, $t0, $t1
  srl $t4, $t4, 1 # Equivalent to dividing by 2

  # Compute address of array[mid]
  sll $t5, $t4, 2    # Multiply mid index by 4 (word size)
  add $t6, $t3, $t5  # Compute address of array[mid]
  lw $t7, $t6, 0     # Load array[mid] value

  # If array[mid] == target, return mid index
  beq $t7, $t8, found

  # If array[mid] > target, search in left half
  bge $t7, $t8, searchToLeft

  # Otherwise, search in right half: start = mid + 1
  addi $t0, $t4, 1
  j while

searchToLeft:
  # Search in left half: end = mid - 1
  addi $t1, $t4, -1
  j while
  
found:
  move $v0, $t4 # Return found index
  jr $ra        # Return to caller

not_found:
  li $v0, -1 # Return -1 if not found
  jr $ra     # Return to caller