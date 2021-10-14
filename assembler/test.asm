// Comment


      // One more Comment

@R0
D=M //commnet
@R1
D=D-M                   // comment
@OUTPUT_FIRST // commnet
D;JGT
@R1
D=M                  
@OUTPUT_D   
0;JMP

           (OUTPUT_FIRST)
    @R0
    D=M // commnet

(OUTPUT_D)
    @R2           // comment
    M=D

(INFINITE_LOOP)
    @INFINITE_LOOP
    0;JMP
