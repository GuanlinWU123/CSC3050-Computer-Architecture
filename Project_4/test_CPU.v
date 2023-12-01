`timescale 1ns/1ps

module cpu_test;
reg CLOCK, ENABLE;

reg[31:0] a, b;
reg[64:0] EDIT_SERIAL_r;

wire RESET, ENABLEE, ENABLEM;
wire regWriteD, memToRegD, memWriteD, branchD, aluSrcD, regDstD, signExtSrcD;
wire regWriteE, memToRegE, memWriteE, branchE, aluSrcE, regDstE;
wire regWriteM, memToRegM, branchM, PCSrcM;
wire regWriteW, memToRegW;
wire WE3;
wire[1:0] add4SrcD, add4SrcE;
wire[2:0] flags, flagsM; 
wire[4:0] A1, A2, A3, shamt;
wire[4:0] rtD, rdD, rtE, rdE;
wire[4:0] writeRegE, writeRegM, writeRegW;
wire[5:0] opcode, func;
wire[15:0] immediateInput;
wire[18:0] aluControlD, aluControlE;
wire[25:0] jumpInput;
wire[31:0] RD; 
wire[31:0] pcF;
wire[31:0] pcP, pcPlus4F, pcPlus4D, pcPlus4E;
wire[31:0] instrD, instrE, instrM, instrW;
wire[31:0] pcBranchE, pcBranchM;
wire[31:0] RD1, RD2, srcAE, writeDataE;
wire[31:0] signImmD, signImmE;
wire[31:0] srcBE, aluOutE, aluOutM;
wire[31:0] readDataM;
wire[31:0] aluOutW, readDataW;
wire[31:0] resultW, WD3;
wire[64:0] EDIT_SERIAL;


pcPlus4Mod pcPlus4ModTest(pcF, pcPlus4F);

mux_2to1_pcP mux_2to1_pcPTest(PCSrcM, pcPlus4F, pcBranchM, pcP, RESET);

bufferPC2IF bufferPC2IFTest(CLOCK, RESET, pcP, pcF);

InstructionRAM InstructionRam(CLOCK, RESET, ENABLE, pcF, RD);

bufferIF2ID bufferIF2ID(CLOCK, RESET, RD, pcPlus4F, instrD, pcPlus4D);

instructionFetch instructionFetch(instrD, opcode, shamt, func, A1, A2, rtD, rdD, immediateInput, jumpInput);

controlUnit controlUnit(instrD, opcode, func, shamt, regWriteD, memToRegD, memWriteD, branchD, aluControlD, 
                    aluSrcD, regDstD, signExtSrcD, add4SrcD);

registerFile registerFile(A1, A2, A3, WD3, WE3, RD1, RD2);

signExtention signExtention(immediateInput, jumpInput, aluControlD, signExtSrcD, signImmD);

bufferID2EX bufferID2EX(CLOCK, RESET, regWriteD, memToRegD, memWriteD, branchD, aluSrcD, regDstD, 
    add4SrcD, aluControlD, RD1, RD2, rtD, rdD, signImmD, pcPlus4D, instrD, regWriteE, memToRegE, 
    memWriteE, branchE, aluSrcE, regDstE, add4SrcE, aluControlE, srcAE, writeDataE, rtE, rdE, 
    signImmE, pcPlus4E, instrE);

mux_2to1_srcBE mux_2to1_srcBE(writeDataE, signImmE, aluSrcE, srcBE);

alu alu(instrE, aluControlE, srcAE, srcBE, pcPlus4E, ENABLE, aluOutE, flags, ENABLEE);

mux_2to1_writeReg mux_2to1_writeReg(rtE, rdE, regDstE, writeRegE);

adder_pcBranchE adder_pcBranchE(signImmE, pcPlus4E, add4SrcE, aluOutE, pcBranchE);

bufferEX2MEM bufferEX2MEM(CLOCK, RESET, regWriteE, memToRegE, memWriteE, branchE, aluOutE, flags, 
                writeDataE, writeRegE, pcBranchE, instrE, ENABLEE, regWriteM, memToRegM, branchM, aluOutM, flagsM, 
                writeRegM, EDIT_SERIAL, pcBranchM, instrM, ENABLEM);

PCSrcMGate PCSrcMgate(branchM, RESET, flagsM, PCSrcM);

MainMemory MainMemory(CLOCK, RESET, ENABLEM, aluOutM, EDIT_SERIAL, readDataM);

bufferMEM2WB bufferMEM2WB(CLOCK, RESET, regWriteM, memToRegM, aluOutM, readDataM, writeRegM, instrM, regWriteW, 
                            memToRegW, aluOutW, readDataW, writeRegW, instrW);

mux_2to1_resultW mux_2to1_resultW(aluOutW, readDataW, memToRegW, resultW);          

writeBack writeBack(regWriteW, writeRegW, resultW, WE3, A3, WD3);
initial
    begin
    CLOCK = 0;
    ENABLE = 1'b1;

    $display("result: ");
    
    if (instrW == 32'b11111111111111111111111111111111)
            begin
                #50 $finish;
            end
    
    end
    always #15 CLOCK = ~CLOCK;

endmodule