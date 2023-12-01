all: compile

compile:
	iverilog -o test InstructionRAM.v MainMemory.v CPU.v test_CPU.v
	vvp test

clean:
	rm test