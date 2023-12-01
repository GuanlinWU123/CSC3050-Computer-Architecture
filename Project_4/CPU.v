module pcPlus4Mod(pcF, pcPlus4F); //add 4 to pc and output a new pc pcPlus4F
    input[31:0] pcF;
    output[31:0] pcPlus4F;
    reg[31:0] pcPlus4F_r;

    always @(pcF)
        begin
            pcPlus4F_r = pcF + 4; 
        end
    assign pcPlus4F = pcPlus4F_r;
endmodule

module mux_2to1_pcP(PCSrcM, pcPlus4F, pcBranchM, pcP, RESET); //Used to output RESET signal, deal with branch & jump instruction
    input PCSrcM;
    input[31:0] pcPlus4F, pcBranchM;
    output RESET;
    reg RESET_r;
    output[31:0] pcP;
    reg[31:0] pcP_r;

    always @(PCSrcM or pcPlus4F or pcBranchM)
        begin
            if (PCSrcM) //branch&jump signal
                begin
                    pcP_r = pcBranchM;
                    RESET_r = 1'b1;
                end
            else
                begin
                    pcP_r = pcPlus4F;
                    RESET_r = 1'b0;
                end
        end
    assign RESET = RESET_r;
    assign pcP = pcP_r;
endmodule

module bufferPC2IF(CLOCK, RESET, pcP, pcF);
    input CLOCK, RESET;
    input[31:0] pcP;
    output[31:0] pcF;
    reg[31:0] pcF;

    initial
        begin
            pcF <= 32'b0; //set up the pcF at the start of the program
        end
    always @(posedge CLOCK)
        begin
            pcF <= pcP; 
        end
endmodule

module bufferIF2ID(CLOCK, RESET, RD, pcPlus4F, instrD, pcPlus4D);
    input CLOCK, RESET;
    input[31:0] RD, pcPlus4F;
    output[31:0] instrD, pcPlus4D;
    reg[31:0] instrD, pcPlus4D;

    always @(posedge CLOCK)
        if(RESET)
            begin
            // deal with the RESET case
            instrD <= 32'b0000_0000_0000_0000_0000_0000_0000_0000; 
            pcPlus4D <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
            end
        else
            begin 
            instrD <= RD;
            pcPlus4D <= pcPlus4F; 
            end
endmodule

module instructionFetch(instrD, opcode, shamt, func, A1, A2, rtD, rdD, immediateInput, jumpInput); 
    //Fecth the instruction into several parts: opcode, shamt, func, rs, rt, rd, immediate
    input[31:0] instrD;
    
    output[4:0] A1, A2, rtD, rdD, shamt;
    output[5:0] opcode, func;
    output[15:0] immediateInput;
    output[25:0] jumpInput;

    reg[4:0] A1_r, A2_r, rtD_r, rdD_r, shamt_r;
    reg[5:0] opcode_r, func_r;
    reg[15:0] immediateInput_r;
    reg[25:0] jumpInput_r;

    always @(*)
        begin
            opcode_r = instrD[31:26];
            if (opcode_r == 6'b000011) //deal with jump instruction
                begin
                    rtD_r = 5'b11111; //deal with the jal instruction
                end
            else
                begin
                    rtD_r = instrD[20:16];
                end
            func_r = instrD[5:0];
            A1_r = instrD[25:21];
            A2_r = instrD[20:16];
            rdD_r = instrD[15:11];
            shamt_r = instrD[10:6];
            immediateInput_r = instrD[15:0];
            jumpInput_r = instrD[25:0];
        end

    assign opcode = opcode_r;
    assign func = func_r;
    assign A1 = A1_r;
    assign A2 = A2_r;
    assign rtD = rtD_r;
    assign rdD = rdD_r;
    assign shamt = shamt_r;
    assign immediateInput = immediateInput_r;
    assign jumpInput = jumpInput_r;

endmodule

module controlUnit(instrD, opcode, func, shamt, regWriteD, memToRegD, memWriteD, branchD, aluControlD, 
                    aluSrcD, regDstD, signExtSrcD, add4SrcD);
    input[5:0] opcode, func;
    input[4:0] shamt;
    input[31:0] instrD;
    output regWriteD, memToRegD, memWriteD, branchD, aluSrcD, regDstD, signExtSrcD;
    output[1:0] add4SrcD; //00: signImmE<<2 + 4; 01: signImmE<<2; 10:aluOut
    output[18:0] aluControlD;

    reg regWriteD_r, memToRegD_r, memWriteD_r, branchD_r, aluSrcD_r, regDstD_r, signExtSrcD_r;
    reg[1:0] add4SrcD_r;
    reg[5:0] func_r;
    reg[4:0] shamt_r;
    reg[18:0] aluControlD_r;//[18]use&not use [17]sign&unsign [16:11]opcode [10:5]func [4:0]shamt

    always @(instrD)
        begin
        aluControlD_r[18:17] = 2'b10;
        regWriteD_r = 1'b0;
        memToRegD_r = 1'b0;
        memWriteD_r = 1'b0;
        branchD_r = 1'b0;
        aluSrcD_r = 1'b0;
        regDstD_r = 1'b0;
        signExtSrcD_r = 1'b0;
        add4SrcD_r = 2'b00;
        func_r = 6'b000000;
        shamt_r = 5'b00000;

        case(opcode)
            6'b000000: //R type
                begin
                func_r = func;
                aluSrcD_r = 1'b0;
                    case(func)
                        6'b100000: //add
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                            end
                        
                        6'b100001: //addu
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                                aluControlD_r[17] = 1'b1;

                            end

                        6'b100010: //sub
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                            end
                        
                        6'b100011: //subu
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                                aluControlD_r[17] = 1'b1;
                            end

                        6'b100100: //and
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                            end
                        
                        6'b100111: //nor
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;
                            end

                        6'b100101: //or
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;   
                            end

                        6'b100110: //xor
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;   
                                // $display("controlUnit: xor");
                            end
                        
                        6'b101010: //slt
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;   
                            end


                        6'b000000: //sll
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1; 
                            end
                        
                        6'b000100: //sllv
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1; 
                            end

                        6'b000010: //srl
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1; 
                            end

                        6'b000110: //srlv
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1; 
                            end

                        6'b000011: //sra
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1;             
                            end

                        6'b000111: //srav
                            begin
                                regWriteD_r = 1'b1;
                                regDstD_r = 1'b1; 
                            end

                        6'b001000: //jr N
                            begin
                                branchD_r = 1'b1;
                                add4SrcD_r = 2'b10;
                            end
                    endcase
                end

            6'b001000:  //addi
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                end

            6'b001001:  //addiu
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                end
            
            6'b001100:  //andi
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                end
            
            6'b001101: //ori
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                end
            
            6'b001110: //xori
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                    aluControlD_r[17] = 1'b1;
                end
            
            6'b000100: //beq 
                begin
                    branchD_r = 1'b1;
                end

            6'b000101: //bne 
                begin
                    branchD_r = 1'b1;
                end
            
            6'b100011: //lw
                begin
                    regWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1; 
                    memToRegD_r = 1'b1;
                end
            
            6'b101011: //sw
                begin
                    memWriteD_r = 1'b1;
                    aluSrcD_r = 1'b1; 
                    memToRegD_r = 1'b1;
                end
            
            6'b000010: //j N
                begin
                    branchD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                    signExtSrcD_r = 1'b1;
                    add4SrcD_r = 2'b01;
                end
            
            6'b000011: //jal N
                begin
                    regWriteD_r = 1'b1;
                    branchD_r = 1'b1;
                    aluSrcD_r = 1'b1;
                    signExtSrcD_r = 1'b1;
                    add4SrcD_r = 2'b01;
                end
        endcase
        aluControlD_r[16:11] = opcode;
        aluControlD_r[10:5] = func_r;
        aluControlD_r[4:0] = shamt;
    end
    assign regWriteD = regWriteD_r;
    assign memToRegD = memToRegD_r;
    assign memWriteD = memWriteD_r;
    assign branchD = branchD_r;
    assign aluSrcD = aluSrcD_r;
    assign regDstD = regDstD_r;
    assign signExtSrcD = signExtSrcD_r;
    assign add4SrcD = add4SrcD_r;
    assign aluControlD = aluControlD_r;
endmodule

module registerFile(A1, A2, A3, WD3, WE3, RD1, RD2);
    // deal with the content in 32 registers, read and write
    input WE3, CLOCK;
    input[4:0] A1, A2, A3; 
    input[31:0] WD3;
    output[31:0] RD1, RD2;
    reg[31:0] registers [0:31];

    integer k;
    integer q;
    initial begin
        for (q=0; q < 32; q = q + 1) begin
            registers[q] = 32'b0;
            end
    end

    assign RD1 = registers[A1];
    assign RD2 = registers[A2];
    always @(*) 
        begin
        if ((A3 != 0) && (WE3 == 1)) //deal with the situation that the write register is 0, which should always be 0.
            begin
            registers[A3] = WD3;
            // $display("registers:");
            // for (k=0; k < 32; k = k + 1) begin
            //     $display("%b", registers[k]);
            //     end
            end
        end
endmodule

module signExtention(immediateInput, jumpInput, aluControlD, signExtSrcD, signImmD);
    //deal with the sign extension of 26bit number in jump instruction and 16bit number in I type instruction. 
    input signExtSrcD;
    input[15:0] immediateInput;
    input[18:0] aluControlD;
    input[25:0] jumpInput;
    output[31:0] signImmD;
    reg signControl;
    reg[31:0] signImmD_r;

    always @(*)
        begin
            signControl = aluControlD[17];
            case(signExtSrcD)
                1'b0:
                    begin 
                        case(signControl)
                            1'b0: //sign extension immediate
                                begin
                                    signImmD_r = {{16{immediateInput[15]}}, immediateInput[15:0]};
                                end
                            1'b1: //unsign extension immediate
                                begin
                                    signImmD_r = {{16{1'b0}}, immediateInput[15:0]};
                                end
                        endcase
                    end

                1'b1: //sign extension jump address
                    begin
                        signImmD_r = {{6{jumpInput[25]}}, jumpInput[25:0]};
                    end
            endcase
        end
    assign signImmD = signImmD_r;
endmodule

module bufferID2EX(CLOCK, RESET, regWriteD, memToRegD, memWriteD, branchD, aluSrcD, regDstD, 
    add4SrcD, aluControlD, RD1, RD2, rtD, rdD, signImmD, pcPlus4D, instrD, regWriteE, memToRegE, 
    memWriteE, branchE, aluSrcE, regDstE, add4SrcE, aluControlE, srcAE, writeDataE, rtE, rdE, signImmE, 
    pcPlus4E, instrE);

    input CLOCK, RESET, regWriteD, memToRegD, memWriteD, branchD, aluSrcD, regDstD;
    input[1:0] add4SrcD;
    input[4:0] rtD, rdD;
    input[18:0] aluControlD;
    input[31:0] RD1, RD2, signImmD, pcPlus4D, instrD;

    output regWriteE, memToRegE, memWriteE, branchE, aluSrcE, regDstE;
    output[1:0] add4SrcE;
    output[4:0] rdE, rtE;
    output[18:0] aluControlE; 
    output[31:0] srcAE, writeDataE, signImmE, pcPlus4E, instrE;

    reg regWriteE, memToRegE, memWriteE, branchE, aluSrcE, regDstE, add4SrcE;
    reg[4:0] rtE, rdE;
    reg[18:0] aluControlE; 
    reg[31:0] srcAE, writeDataE, signImmE, pcPlus4E, instrE;

    always @(posedge CLOCK)
        if(RESET)
            begin
            regWriteE <= 1'b0;
            memToRegE <= 1'b0;
            memWriteE <= 1'b0;
            branchE <= 1'b0;
            aluSrcE <= 1'b0;
            regDstE <= 1'b0;
            add4SrcE <= 1'b0;
            aluControlE <= 19'b0000_0000_0000_0000_000;
            srcAE <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
            writeDataE <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
            rtE <= 5'b00000;
            rdE <= 5'b00000;
            signImmE <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
            pcPlus4E <= 32'b0000_0000_0000_0000_0000_0000_0000_0000; 
            instrE <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
            end
        else
            begin
            regWriteE <= regWriteD;
            memToRegE <= memToRegD;
            memWriteE <= memWriteD;
            branchE <= branchD;
            aluSrcE <= aluSrcD;
            regDstE <= regDstD;
            add4SrcE <= add4SrcD;
            aluControlE <= aluControlD;
            srcAE <= RD1;
            writeDataE <= RD2;
            rtE <= rtD;
            rdE <= rdD;
            signImmE <= signImmD;
            pcPlus4E <= pcPlus4D;
            instrE <= instrD;
            end
endmodule

module mux_2to1_srcBE(writeDataE, signImmE, aluSrcE, srcBE); //deal with which should be the input of srcBE of alu
    input[31:0] writeDataE, signImmE;
    input aluSrcE;
    output[31:0] srcBE;
    reg[31:0] srcBE_r;
    always @(*)
        begin
            srcBE_r = (aluSrcE == 1'b0 ? writeDataE : signImmE);
        end
    assign srcBE = srcBE_r;
endmodule

module mux_2to1_writeReg(rtE, rdE, regDstE, writeRegE); 
    //deal with which register will be the register to write data in: R type of I type
    input[4:0] rtE, rdE;
    input regDstE;
    output[4:0] writeRegE;
    reg[4:0] writeRegE_r;
  always @(*)
        begin
            writeRegE_r = (regDstE == 1'b0 ? rtE : rdE);
        end
  assign writeRegE = writeRegE_r;
endmodule

module alu(instrE, aluControlE, srcAE, srcBE, pcPlus4E, ENABLE, aluOutE, flags, ENABLEE);
    input ENABLE;
    input[18:0] aluControlE;
    input[31:0] srcAE, srcBE, pcPlus4E, instrE;

    output ENABLEE;
    output[31:0] aluOutE;
    output[2:0] flags;

    reg signControlAlu, ENABLEE_r;
    reg[2:0] tempFlag; //[2] zeroflag [1]negative [0]overflow
    reg[5:0] opcode, func;
    reg[4:0] sa;
    reg[31:0] srcAE_reg_u, srcBE_reg_u;
    reg signed [31:0] srcAE_reg, srcBE_reg, temp_reg;

    always @(instrE)
        begin
            if (instrE == 32'b11111111111111111111111111111111)
                begin
                    ENABLEE_r = 1'b0;
                end
            else
                begin
                    ENABLEE_r = ENABLE;
                end

            opcode = aluControlE[16:11];
            tempFlag = 3'b000;
            signControlAlu = aluControlE[17];
            
            case(signControlAlu) //deal with the sign or unsign number for input
                1'b0:
                    begin
                        srcAE_reg = srcAE;
                        srcBE_reg = srcBE;
                    end
                1'b1:
                    begin
                        srcAE_reg_u = srcAE;
                        srcBE_reg_u = srcBE;
                    end
            endcase

            // if (opcode == 6'b001110)
            //     begin
            //         srcAE_reg = srcAE;
            //         srcBE_reg = srcBE;
            //     end

            case(opcode)
                6'b000000: //R type
                    begin
                    sa = aluControlE[4:0];
                    func = aluControlE[10:5];

                        case(func)
                            6'b100000: //add
                                begin
                                    temp_reg = srcAE_reg + srcBE_reg;
                                    if ((srcAE_reg[31] == srcBE_reg[31]) && (~temp_reg[31] == srcAE_reg[31]))
                                        tempFlag[0] = 1'b1;
                                end
                            
                            6'b100001: //addu
                                begin
                                    temp_reg = srcAE_reg_u + srcBE_reg_u;
                                end

                            6'b100010: //sub
                                begin
                                    temp_reg = srcAE_reg - srcBE_reg;
                                    if ((srcAE_reg[31] == ~srcBE_reg[31]) && (~temp_reg[31] == srcAE_reg[31]))
                                        tempFlag[0] = 1'b1;
                                end
                            
                            6'b100011: //subu
                                begin
                                    temp_reg = srcAE_reg_u - srcBE_reg_u;
                                end

                            6'b100100: //and
                                begin
                                    temp_reg = (srcAE_reg & srcBE_reg);
                                end
                            
                            6'b100111: //nor
                                begin
                                    temp_reg = ~(srcAE_reg | srcBE_reg);
                                end

                            6'b100101: //or
                                begin
                                    temp_reg = (srcAE_reg | srcBE_reg);
                                end

                            6'b100110: //xor
                                begin
                                    temp_reg = (srcAE_reg ^ srcBE_reg);
                                end

                            6'b101010: //slt
                                begin
                                if (srcAE_reg < srcBE_reg) begin
                                    tempFlag[1] = 1'b1;
                                    temp_reg = 32'b1;
                                end else begin  
                                    temp_reg = 32'b0;
                                    end
                                end

                            6'b000000: //sll
                                begin
                                    temp_reg = srcBE_reg << sa;
                                end
                            
                            6'b000100: //sllv
                                begin
                                    temp_reg = srcBE_reg << srcAE_reg;
                                end

                            6'b000010: //srl
                                begin
                                    temp_reg = srcBE_reg >> sa;
                                end

                            6'b000110: //srlv
                                begin
                                    temp_reg = srcBE_reg >> srcAE_reg;
                                end

                            6'b000011: //sra
                                begin
                                    temp_reg = srcBE_reg >>> sa;
                                    
                                end

                            6'b000111: //srav
                                begin
                                    temp_reg = srcBE_reg >>> srcAE_reg;
                                end
                            
                            6'b001000: //jr N
                                begin
                                    tempFlag[2] = 1'b1;
                                    temp_reg = srcAE_reg;
                                end
                        endcase
                    end

                6'b001000:  //addi
                    begin
                        temp_reg = srcAE_reg + srcBE_reg;
                        if ((srcAE_reg[31] == srcBE_reg[15]) && (~temp_reg[31] == srcAE_reg[31]))
                            tempFlag[0] = 1'b1;
                    end

                6'b001001:  //addiu
                    begin
                        temp_reg = srcAE_reg + srcBE_reg;
                    end
                
                6'b001100:  //andi
                    begin
                        temp_reg = srcAE_reg & srcBE_reg;
                    end
                
                6'b001101: //ori
                    begin
                        temp_reg = srcAE_reg | srcBE_reg;
                    end
                
                6'b001110: //xori
                    begin
                        temp_reg = srcAE_reg_u ^ srcBE_reg_u;
                    end
                
                6'b000100: //beq
                    begin
                        if ((srcAE_reg - srcBE_reg) == 0) begin
                            temp_reg = 0;
                            tempFlag[2] = 1'b1;
                        end else begin
                            temp_reg = srcAE_reg - srcBE_reg;
                            end
                    end

                6'b000101: //bne
                    begin
                        if ((srcAE_reg - srcBE_reg) != 0) begin
                            temp_reg = 0;
                            tempFlag[2] = 1'b1;
                        end else begin
                            temp_reg = srcAE_reg - srcBE_reg;
                            end
                    end
                
                6'b100011: //lw
                    begin
                    temp_reg = srcAE_reg + srcBE_reg;
                    end
                
                6'b101011: //sw
                    begin
                    temp_reg = srcAE_reg + srcBE_reg;

                    end
                
                6'b000010: //j 
                    begin  
                        temp_reg = 0;
                        tempFlag[2] = 1'b1;
                    end
                
                6'b000011: //jal 
                    begin
                        temp_reg = pcPlus4E;
                        tempFlag[2] = 1'b1;
                    end
            endcase
        end

    assign flags = tempFlag;
    assign aluOutE = temp_reg;
    assign ENABLEE = ENABLEE_r;
endmodule

module adder_pcBranchE(signImmE, pcPlus4E, add4SrcE, aluOutE, pcBranchE);
    //this is used to deal with three situation of branch: bne, beq; j, jal; jr
    input[1:0] add4SrcE;
    input[31:0] signImmE, pcPlus4E, aluOutE;
    output[31:0] pcBranchE;

    reg[31:0] signImmE_s, pcBranchE_r;

    always @(*)
        begin
            case(add4SrcE)
                2'b00: //bne, beq
                    begin
                        signImmE_s = signImmE << 2;
                        pcBranchE_r = (signImmE_s + pcPlus4E);
                    end
                2'b01: //j, jal
                    begin
                        pcBranchE_r = signImmE << 2;
                    end
                2'b10: //jr(as the content contained register will be the address)
                    begin
                        pcBranchE_r = aluOutE;
                    end
            endcase
        end
    assign pcBranchE = pcBranchE_r;
endmodule

module bufferEX2MEM(CLOCK, RESET, regWriteE, memToRegE, memWriteE, branchE, aluOutE, flags, 
                writeDataE, writeRegE, pcBranchE, instrE, ENABLEE, regWriteM, memToRegM, branchM, aluOutM, 
                flagsM, writeRegM, EDIT_SERIAL, pcBranchM, instrM, ENABLEM);

    input CLOCK, RESET, regWriteE, memToRegE, memWriteE, branchE, ENABLEE;
    input[2:0] flags;
    input[4:0] writeRegE;
    input[31:0] aluOutE, writeDataE, pcBranchE, instrE;

    output regWriteM, memToRegM, branchM, PCSrcM, ENABLEM;
    output[2:0] flagsM;
    output[4:0] writeRegM;
    output[31:0] aluOutM, pcBranchM, instrM;
    output[64:0] EDIT_SERIAL;

    reg regWriteM, memToRegM, branchM, PCSrcM, ENABLEM;
    reg[2:0] flagsM;
    reg[4:0] writeRegM;
    reg[31:0] aluOutM, pcBranchM, instrM;
    reg[64:0] EDIT_SERIAL;

    always @(posedge CLOCK)
        if(RESET)
            begin  
            regWriteM <= 1'b0;
            memToRegM <= 1'b0;
            branchM <= 1'b0;
            aluOutM <= 32'b0;
            flagsM <= 3'b000;
            writeRegM <= 5'b00000;
            EDIT_SERIAL <= 64'b0;
            pcBranchM <= 32'b0; 
            instrM <= 32'b0;
            ENABLEM <= 1'b1;
            end
        else
            begin  
            regWriteM <= regWriteE;
            memToRegM <= memToRegE;
            branchM <= branchE;
            aluOutM <= aluOutE;
            flagsM <= flags;
            writeRegM <= writeRegE;
            EDIT_SERIAL[64] <= memWriteE;
            EDIT_SERIAL[31:0] <= aluOutE;
            EDIT_SERIAL[63:32] <= writeDataE;
            pcBranchM <= pcBranchE;
            instrM <= instrE;
            ENABLEM <= ENABLEE;
            end
endmodule

module PCSrcMGate(branchM, RESET, flagsM, PCSrcM);
    //this is used to deal with the branch and jump signal
    input branchM, RESET;
    input[2:0] flagsM;

    output PCSrcM;
    reg PCSrcM_r;


    always @(branchM or flagsM) 
    //the reason use these two variables is that RESET might be changed while branchM and flagsM are not change
    //thus, it will cause the RESET and PCSrcM alternate with each other again and again
        begin
            if (RESET)
                begin
                    PCSrcM_r = 1'b0;
                end
            else
                begin
                    PCSrcM_r = branchM & flagsM[2];
                end;
        end
    assign PCSrcM = PCSrcM_r;
endmodule

module bufferMEM2WB(CLOCK, RESET, regWriteM, memToRegM, aluOutM, readDataM, writeRegM, instrM, 
        regWriteW, memToRegW, aluOutW, readDataW, writeRegW, instrW);

    input CLOCK, RESET, regWriteM, memToRegM;
    input[4:0] writeRegM;
    input[31:0] aluOutM, readDataM, writeDataE, instrM;

    output regWriteW, memToRegW;
    output[4:0] writeRegW;
    output[31:0] aluOutW, readDataW, instrW;

    reg regWriteW, memToRegW;
    reg[4:0] writeRegW;
    reg[31:0] aluOutW, readDataW, instrW;

    always @(posedge CLOCK)
            if(RESET)
                begin
                // the reason do not clear regWriteW, writeRegW and aluOutW is that the branch signal will be 
                //given back at stage 4, but the instruction executed before the branch instrution should also
                //be donw for writing back into register. 
                regWriteW <= regWriteM;
                memToRegW <= 1'b0;
                writeRegW <= writeRegM;
                aluOutW <= aluOutM;
                readDataW <= 32'b0;
                instrW <= 32'b0;
                end
            else
                begin
                regWriteW <= regWriteM;
                memToRegW <= memToRegM;
                aluOutW <= aluOutM;
                writeRegW <= writeRegM;
                readDataW <= readDataM;
                instrW <= instrM;
                end
endmodule

module mux_2to1_resultW(aluOutW, readDataW, memToRegW, resultW);
    //deal with whether the data from alu or from data memory will be the data to write back in register file. 
    input[31:0] aluOutW, readDataW;
    input memToRegW;
    output[31:0] resultW;
    reg[31:0] resultW_r;
    always @(aluOutW or readDataW or memToRegW)
        begin
            resultW_r = (memToRegW == 1'b0 ? aluOutW : readDataW);
        end
    assign resultW = resultW_r;
endmodule

module writeBack(regWriteW, writeRegW, resultW, WE3, A3, WD3);
    input regWriteW;
    input[4:0] writeRegW; 
    input[31:0] resultW;

    output WE3;
    output[4:0] A3;
    output[31:0] WD3;

    reg WE3_r;
    reg[4:0] A3_r;
    reg[31:0] WD3_r;

    always @(*) 
        begin
            WE3_r = regWriteW;
            WD3_r = resultW;
            A3_r = writeRegW;
        end
    assign WE3 = WE3_r;
    assign WD3 = WD3_r;
    assign A3 = A3_r;
endmodule