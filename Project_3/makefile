all: compile

compile:
	iverilog -o test ALU.v test_ALU.v
	vvp test

clean:
	rm test