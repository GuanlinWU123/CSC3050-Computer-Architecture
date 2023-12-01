.text
A:
    addi $t0, $t0, 0
    addi $t0, $t0, 0
    addi $t0, $t0, 0
B: 
    j D
    addi $t0, $t0, 0
    addi $t0, $t0, 0
    bne $s0, $t0, D
    addi $t0, $t0, 0
    addi $t0, $t0, 0
C: 
    addi $t0, $t0, 0
    addi $t0, $t0, 0
D: 
    j A
    beq $s0, $t0, D
    j C