.data
prompt1: .asciiz "Digite o número de elementos do vetor: "
prompt2: .asciiz "Digite um valor para o vetor: "
result:  .asciiz "O somatório dos elementos é:"

.text
main:
        # Print message for the number of elements
        li $v0, 4                  
        la $a0, prompt1            
        syscall

        # Read the number of elements (n)
        li $v0, 5                  
        syscall
        move $s0, $v0              # Store n in $s0

        # Allocate space for the vector on the stack
        li $t8, 4                  # Word size
        mul $t0, $s0, $t8            
        sub $sp, $sp, $t0          
        move $s1, $sp              # Save the starting address of the vector in $s1

        # Fill the vector with user inputs
        li $t1, 0                  
fill_vector:
        bge $t1, $s0, end_fill      # If i >= n, stop
        li $v0, 4                  
        la $a0, prompt2            
        syscall

        # Read the value and store it in the vector
        li $v0, 5                  
        syscall
        sw $v0, 0($s1)              # Store value in the vector

        addi $t1, $t1, 1           
        addi $s1, $s1, 4           # Move to the next position
        j fill_vector              

end_fill:

        # Restore $s1 to point to the start of the vector
        sub $s1, $s1, $t0          

        #lw $t0 4($s1)
        #li $v0, 1 
        #move $a0, $t0
        #syscall

        #li $v0, 10
        #syscall


        # Call the sum function
        jal sum_vector             
        
        # Stores the value returned from function
        move $t9, $v0

        # Print result message
        li $v0, 4                  
        la $a0, result             
        syscall

        # Print the sum result
        li $v0, 1                  
        move $a0, $t9              
        syscall

        # Exit program
        li $v0, 10                 
        syscall

# Function to sum vector elements
sum_vector:
        li $t1, 0                  
        li $t2, 0                  
        move $t4, $s1              # Pointer to the vector

sum_loop:
        bge $t1, $s0, end_sum       
        lw $t3, 0($t4)              # Load value from vector
        add $t2, $t2, $t3           
        addi $t1, $t1, 1           
        addi $t4, $t4, 4           # Move pointer
        j sum_loop                 

end_sum:
        move $v0, $t2              
        jr $ra                      