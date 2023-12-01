module alu(instruction, regA, regB, result, flags);

    input[31:0] instruction, regA, regB;

    output[31:0] result;
    output[2:0] flags;

    reg[2:0] tempFlag;
    reg[5:0] opcode, func;
    reg[4:0] rd, rs, rt, sa;
    reg[15:0] immediate_u;
    reg[31:0] reg_s_u, reg_t_u, reg_d;
    reg signed [15:0] immediate;
    reg signed [31:0] reg_s, reg_t, temp_reg;

    always @(instruction or regA or regB)
        begin
            opcode = instruction[31:26];
            rs = instruction[25:21];
            rt = instruction[20:16];
            immediate_u = instruction[15:0];
            immediate = instruction[15:0];
            tempFlag = 3'b000;

            case(rs)
                5'b00000:
                    begin
                        reg_s_u = regA;
                        reg_s = regA;
                    end
                5'b00001:
                    begin
                        reg_s_u = regB;
                        reg_s = regB;
                    end
            endcase

            case(rt)
                5'b00000:
                    begin
                        reg_t_u = regA;
                        reg_t = regA;
                    end
                5'b00001:
                    begin
                        reg_t_u = regB;
                        reg_t = regB;
                    end
            endcase

            case(opcode)
                6'b000000: //R type
                    begin
                    rd = instruction[15:11];
                    sa = instruction[10:6];
                    func = instruction[5:0];
                        case(func)
                            6'b100000: //add
                                begin
                                    reg_d = reg_s + reg_t;
                                    if ((reg_s[31] == reg_t[31]) && (~reg_d[31] == reg_s[31]))
                                        tempFlag[0] = 1'b1;
                                end
                            
                            6'b100001: //addu
                                begin
                                    reg_d = reg_s_u + reg_t_u;
                                end

                            6'b100010: //sub
                                begin
                                    reg_d = reg_s - reg_t;
                                    if ((reg_s[31] == ~reg_t[31]) && (~reg_d[31] == reg_s[31]))
                                        tempFlag[0] = 1'b1;
                                end
                            
                            6'b100011: //subu
                                begin
                                    reg_d = reg_s_u - reg_t_u;
                                end

                            6'b100100: //and
                                begin
                                    reg_d = reg_s_u & reg_t_u;
                                end
                            
                            6'b100111: //nor
                                begin
                                    reg_d = ~(reg_s_u | reg_t_u);
                                end

                            6'b100101: //or
                                begin
                                    reg_d = reg_s_u | reg_t_u;
                                end

                            6'b100110: //xor
                                begin
                                    reg_d = reg_s_u ^ reg_t_u;
                                end
                            
                            6'b101010: //slt
                                begin
                                if (reg_s < reg_t) begin
                                    tempFlag[1] = 1'b1;
                                    reg_d = 1;
                                end else begin  
                                    reg_d = 0;
                                    end
                                end
                            
                            6'b101011: //sltu
                                begin
                                if (reg_s_u < reg_t_u) begin
                                    tempFlag[1] = 1'b1;
                                    reg_d = 1;
                                end else begin
                                    reg_d = 0;
                                    end
                                end

                            6'b000000: //sll
                                begin
                                    reg_d = reg_t << sa;
                                end
                            
                            6'b000100: //sllv
                                begin
                                    reg_d = reg_t << reg_s_u;
                                end

                            6'b000010: //srl
                                begin
                                    reg_d = reg_t >> sa;
                                end

                            6'b000110: //srlv
                                begin
                                    reg_d = reg_t >> reg_s_u;
                                end

                            6'b000011: //sra
                                begin
                                    reg_d = reg_t >>> sa;
                                    
                                end

                            6'b000111: //srav
                                begin
                                    reg_d = reg_t >>> reg_s_u;
                                end
                        endcase
                    end

                6'b001000:  //addi
                    begin
                        reg_d = reg_s + {{16{immediate[15]}}, immediate};
                        if ((reg_s[31] == immediate[15]) && (~reg_d[31] == reg_s[31]))
                            tempFlag[0] = 1'b1;
                    end

                6'b001001:  //addiu
                    begin
                        reg_d = reg_s + {{16{immediate_u[15]}}, immediate};
                    end
                
                6'b001100:  //andi
                    begin
                        reg_d = reg_s_u & {{16{immediate_u[15]}}, immediate};
                    end
                
                6'b001101: //ori
                    begin
                        reg_d = reg_s_u | {{16{immediate_u[15]}}, immediate};
                    end
                
                6'b001110: //xori
                    begin
                        reg_d = reg_s_u ^ {{16{immediate_u[15]}}, immediate};
                    end
                
                6'b000100: //beq
                    begin
                        if ((reg_s - reg_t) == 0) begin
                            reg_d = 0;
                            tempFlag[2] = 1'b1;
                        end else begin
                            reg_d = reg_s - reg_t;
                            end
                    end

                6'b000101: //bne
                    begin
                        if ((reg_s - reg_t) == 0) begin
                            reg_d = 0;
                            tempFlag[2] = 1'b1;
                        end else begin
                            reg_d = reg_s - reg_t;
                            end
                    end
                
                6'b100011: //lw
                    begin
                    reg_d = reg_s + immediate;
                    end
                
                6'b101011: //sw
                    begin
                    reg_d = reg_s + immediate;
                    end
                
                6'b001010: //slti
                    begin
                    temp_reg = {{16{immediate[15]}}, immediate};
                    if (reg_s < temp_reg) begin
                        tempFlag[1] = 1'b1;
                        reg_d = 1;
                    end else begin   
                        reg_d = 0;
                        end
                    end
                
                6'b001011: //sltiu
                    begin
                    if (reg_s_u < {{16'b0}, immediate_u}) begin
                        tempFlag[1] = 1'b1;
                        reg_d = 1;
                    end else begin
                        reg_d = 0;
                        end
                    end
                

            endcase

        end

    assign flags = tempFlag;
    assign result = reg_d;

endmodule