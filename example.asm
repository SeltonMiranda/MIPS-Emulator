    addi $1, zero, 5
    jal test
    addi $2, zero, 543
    ebreak
test:
    addi $3, zero, 0
    addi $4, zero, 11
for:   
    bge $3, $4, fora_for
    addi $3, $3, 1
    j for
fora_for:
    addi $5, zero, 69
    jr ra