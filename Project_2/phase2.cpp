//
// Created by 吴冠霖 on 2022/3/5.
//
#include "phase2.h"
#include "memory.h"

using namespace std;

typedef map<string, string> Map ;

Map registerTable ( {{"00000", "$zero"}, {"00001", "$at"}, {"00010", "$v0"}, {"00011", "$v1"},
                    {"00100",   "$a0"}, {"00101", "$a1"}, {"00110", "$a2"}, {"00111", "$a3"},
                    {"01000",   "$t0"}, {"01001", "$t1"}, {"01010", "$t2"}, {"01011", "$t3"},
                    {"01100",   "$t4"}, {"01101", "$t5"}, {"01110", "$t6"}, {"01111", "$t7"},
                    {"10000",   "$s0"}, {"10001", "$s1"}, {"10010", "$s2"}, {"10011", "$s3"},
                    {"10100",   "$s4"}, {"10101", "$s5"}, {"10110", "$s6"}, {"10111", "$s7"},
                    {"11000",   "$t8"}, {"11001", "$t9"}, {"11010", "$k0"}, {"11011", "$k1"},
                    {"11100",   "$gp"}, {"11101", "$sp"}, {"11110", "$s8"}, {"11110", "$fp"},
                    {"11111",   "$ra"}} );

Map RFuncTable ( {{"100000","add"}, {"100001","addu"}, {"100100","and"}, {"011010","div"},
                 {"011011","divu"}, {"001001","jalr"}, {"001000","jr"}, {"010000","mfhi"},
                 {"010010","mflo"}, {"010001","mthi"}, {"010011","mtlo"}, {"011000","mult"},
                 {"011001","multu"}, {"100111","nor"}, {"100101","or"}, {"000000","sll"},
                 {"000100","sllv"}, {"101010","slt"}, {"101011","sltu"}, {"000011","sra"},
                 {"000111","srav"}, {"000010","srl"}, {"000110","srlv"}, {"100010","sub"},
                 {"100011","subu"}, {"001100","syscall"}, {"100110","xor"}} );

Map IFuncTable ( {{"001000", "addi"}, {"001001", "addiu"}, {"001100", "andi"}, {"000100", "beq"},
                  {"000001", "bgez"}, {"000111", "bgtz"}, {"000110", "blez"}, {"000001", "bltz"},
                  {"000101", "bne"}, {"100000", "lb"}, {"100100", "lbu"}, {"100001", "lh"},
                  {"100101", "lhu"}, {"001111", "lui"}, {"100011", "lw"}, {"001101", "ori"},
                  {"101000", "sb"}, {"001010", "slti"}, {"001011", "sltiu"}, {"101001", "sh"},
                  {"101011", "sw"}, {"001110", "xori"}, {"100010", "lwl"}, {"100110", "lwr"},
                  {"101010", "swl"}, {"101110", "swr"}} );

Map JFuncTable( {{"000010","j"}, {"000011","jal"}} );

void advance_pc(int* pcPointer, int* nPcPointer, int offset){
    pcPointer = nPcPointer;
    nPcPointer += offset;
}

string JudgeType(string text){
    string opcode = text.substr(0, 6);
    if (opcode == "000000"){
        return "r";
    }else if (opcode == "000010" || opcode == "000011"){
        return "j";
    }else{
        return "i";
    }
}

string registerTrans(string regisNum){
    string regisName = registerTable.at(regisNum);
    return regisName;
}

vector<string> textEditing(string text, string type){
    vector<string> strList;
    if (type == "r"){
        string opcode = text.substr(0, 6);
        string rs = text.substr(6, 5);
        string rt = text.substr(11, 5);
        string rd = text.substr(16, 5);
        string shamt = text.substr(21, 5);
        string func = text.substr(26, 6);
        strList = {opcode, rs, rt, rd, shamt, func};

    } else if (type == "i"){
        string opcode = text.substr(0, 6);
        string rs = text.substr(6, 5);
        string rt = text.substr(11, 5);
        string immediate = text.substr(16, 16);
        strList = {opcode, rs, rt, immediate};

    } else {
        string opcode = text.substr(0, 6);
        string target = text.substr(6, 26);
        strList = {opcode, target};
    }
    return strList;
}

string generateString(char* pointer){
    char* charPointer = pointer;
    string newStr;
    while (*charPointer != '\0'){
        newStr.push_back(*charPointer);
        charPointer++;
    }
    return newStr;
}

queue<string> syscallFunction(int number, int* pointer, queue<string> fileInQueue, FILE* ofile, map<string, int*> registerMemory, map<int, int*> virtualMemory){

    switch (number) {
        case 1:{ //print_int
            fprintf(ofile, "%d", *pointer);
            fflush(ofile);
            break;
        }
        case 4:{ //print_string
            int tempNum = *registerMemory.at("$a0");
            char* tempPointer = (char*) virtualMemory.at(tempNum);
            while (*tempPointer != '\0'){
                fprintf(ofile, "%c", *tempPointer);
                tempPointer++;
            }
            fflush(ofile);
            break;
        }
        case 5:{ //read_int
            int tempNum = stoi(fileInQueue.front());
            *registerMemory.at("$v0") = tempNum;
            fileInQueue.pop();
            break;
        }
        case 8:{ //read_string
            int strLength;
            string tempStr = fileInQueue.front();
            int tempNum = *registerMemory.at("$a0");
            char* charPointer = (char*) virtualMemory.at(tempNum);
            if (*registerMemory.at("$a1") <= tempStr.size()){
                strLength = *registerMemory.at("$a1");
            }else {
                strLength = tempStr.size();
            }
            for (int i = 0; i < strLength; i++){
                *charPointer = tempStr.at(i);
                charPointer++;
            }
            fileInQueue.pop();
            break;
        }
        case 10:{ //exit
            exit(0);
        }
        case 11:{ //print_char
            char* tempPointer = (char*) registerMemory.at("$a0");
            fprintf(ofile, "%c", *tempPointer);
            fflush(ofile);
            break;
        }
        case 12:{ //read_char
            string tempStr = fileInQueue.front();
            char tempChar = tempStr.at(0);
            char* v0CharPointer = (char*) registerMemory.at("$v0");
            *v0CharPointer = tempChar;
            fileInQueue.pop();
            break;
        }
        case 13:{ //open
            cout << "open" << endl;
            int tempNum = *registerMemory.at("$a0");
            char* charPointer = (char*) virtualMemory.at(tempNum);
            string fileName = generateString(charPointer);
            int fd = open(charPointer, *registerMemory.at("$a1"), *registerMemory.at("$a2"));
            *registerMemory.at("$a0") = fd;
            break;
        }
        case 14:{ //read
            int fd = *registerMemory.at("$a0");
            read(fd, registerMemory.at("$a1"), *registerMemory.at("$a2"));
            break;
        }
        case 15:{ //write
            int fd = *registerMemory.at("$a0");
            write(fd, registerMemory.at("$a1"), *registerMemory.at("$a2"));
            break;
        }
        case 16:{ //close
            int fd = *registerMemory.at("$a0");
            close(fd);
            break;
        }
        case 17:{ //exit2
            int result = *registerMemory.at("$a0");
            _exit(result);
        }
    }
    return fileInQueue;
}

void rTypeFunction(vector<string> strList, map<string, int*> registerMemory, map<int, int*> virtualMemory, queue<string> fileInQueue){ //return queue
    string rs = registerTable.at(strList.at(1));
    string rt = registerTable.at(strList.at(2));
    string rd = registerTable.at(strList.at(3));
    string shamt = strList.at(4);
    string func = strList.at(5);
    string funcName = RFuncTable.at(func);
    int* rsPointer = registerMemory.at(rs);
    int* rtPointer = registerMemory.at(rt);
    int* rdPointer = registerMemory.at(rd);
    int* HIPointer = registerMemory.at("$hi");
    int* LOPointer = registerMemory.at("$lo");

    if (funcName == "add"){
        *rdPointer = *rsPointer + *rtPointer;

    }else if (funcName == "addu"){
        *rdPointer = *rsPointer + *rtPointer;

    }else if (funcName == "and"){
        *rdPointer = (*rsPointer & *rtPointer);

    }else if (funcName == "div"){
        *HIPointer = *rsPointer % *rtPointer;
        *LOPointer = *rsPointer / *rtPointer;

    }else if (funcName == "divu"){
        *HIPointer = *rsPointer % *rtPointer;
        *LOPointer = *rsPointer / *rtPointer;

    }else if (funcName == "mfhi"){
        *rdPointer = *HIPointer;

    }else if (funcName == "mflo"){
        *rdPointer = *LOPointer;

    }else if (funcName == "mthi"){
        *HIPointer = *rsPointer;

    }else if (funcName == "mtlo"){
        *LOPointer = *rsPointer;

    }else if (funcName == "mult"){
        int tempNum = *rsPointer * *rtPointer;
        string binNumStr = decToBin64(tempNum);
        *HIPointer = binToDec(binNumStr.substr(0, 32));
        *LOPointer = binToDec(binNumStr.substr(32, 32));

    }else if (funcName == "multu"){
        int tempNum = *rsPointer * *rtPointer;
        string binNumStr = decToBin64(tempNum);
        *HIPointer = binToDec(binNumStr.substr(0, 32));
        *LOPointer = binToDec(binNumStr.substr(32, 32));

    }else if (funcName == "nor"){
        *rdPointer = (~(*rsPointer) & ~(*rtPointer));

    }else if (funcName == "or"){
        *rdPointer = *rsPointer | *rtPointer;

    }else if (funcName == "sll"){
        *rdPointer = *rtPointer << binToDec(shamt);

    }else if (funcName == "sllv"){
        string binNum = decToBin64(*rsPointer);
        string tempStr = binNum.substr(binNum.length()-5, 5);
        *rdPointer = *rtPointer << binToDec(tempStr);

    }else if (funcName == "slt"){
        if (*rsPointer < *rtPointer){
            *rdPointer = 1;
        }else{
            *rdPointer = 0;
        }

    }else if (funcName == "sltu"){
        if (*rsPointer < *rtPointer){
            *rdPointer = 1;
        }else{
            *rdPointer = 0;
        }

    }else if (funcName == "sra"){
        *rdPointer = *rtPointer >> binToDec(shamt);

    }else if (funcName == "srav"){
        string binNum = decToBin64(*rsPointer);
        string tempStr = binNum.substr(binNum.length()-5, 5);
        *rdPointer = *rtPointer >> binToDec(tempStr);

    }else if (funcName == "srl"){
        *rdPointer = (unsigned)*rtPointer >> binToDec(shamt);

    }else if (funcName == "srlv"){
        string binNum = decToBin64(*rsPointer);
        string tempStr = binNum.substr(binNum.length()-5, 5);
        *rdPointer = (unsigned)*rtPointer >> binToDec(tempStr);

    }else if (funcName == "sub"){
        *rdPointer = *rsPointer - *rtPointer;

    }else if (funcName == "subu"){
        *rdPointer = *rsPointer - *rtPointer;

    }else if (funcName == "xor"){
        *rdPointer = (*rsPointer ^ *rtPointer);
    }
}

int iTypeFunction(vector<string> strList, int* nPcPointer, map<string, int*> registerMemory, map<int, int*> virtualMemory){
    string opcode = strList.at(0);
    string rs = registerTable.at(strList.at(1));
    string rt = registerTable.at(strList.at(2));
    string immediate = strList.at(3);
    string funcName = IFuncTable.at(opcode);
    int* rsPointer;

    rsPointer = registerMemory.at(rs);
    int* rtPointer = registerMemory.at(rt);

    if (funcName == "addi"){
        *rtPointer = *rsPointer + signedBinToDec(immediate);

    }else if (funcName == "addiu"){
        *rtPointer = *rsPointer + binToDec(immediate);

    }else if (funcName == "andi"){
        *rtPointer = (*rsPointer & binToDec(immediate));

    }else if (funcName == "beq"){
        if (*rsPointer == *rtPointer){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;


    }else if (funcName == "bgez"){
        if (*rsPointer >= 0){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;

    }else if (funcName == "bgtz"){
        if (*rsPointer > 0){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;

    }else if (funcName == "blez"){
        if (*rsPointer <= 0){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;

    }else if (funcName == "bltz"){
        if (*rsPointer < 0){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;

    }else if (funcName == "bne"){
        if (*rsPointer != *rtPointer){
            int offset = signedBinToDec(immediate);
            *nPcPointer += (offset << 2);
        }
        return 0;

    }else if (funcName == "lb"){
        int addressNum = *rsPointer + signedBinToDec(immediate);
        int8_t* int8Pointer = (int8_t*) virtualMemory.at(addressNum);
        *rtPointer = *int8Pointer;

    }else if (funcName == "lbu"){
        int addressNum = *rsPointer + binToDec(immediate);
        int8_t* int8Pointer = (int8_t*) virtualMemory.at(addressNum);
        *rtPointer = *int8Pointer;


    }else if (funcName == "lh"){
        int addressNum = *rsPointer + signedBinToDec(immediate);
        int16_t* int16Pointer = (int16_t*) virtualMemory.at(addressNum);
        *rtPointer = *int16Pointer;

    }else if (funcName == "lhu"){
        int addressNum = *rsPointer + binToDec(immediate);
        int16_t* int16Pointer = (int16_t*) virtualMemory.at(addressNum);
        *rtPointer = *int16Pointer;

    }else if (funcName == "lui"){
        int newInt = signedBinToDec(immediate) << 16;
        *rtPointer = newInt;

    }else if (funcName == "lw"){
        int addressNum = *rsPointer + signedBinToDec(immediate);
        int* intPointer = virtualMemory.at(addressNum);
        *rtPointer = *intPointer;

    }else if (funcName == "ori"){
        if (signedBinToDec(immediate) == 220){
            cout << 1 << endl;
        }
        *rtPointer = (*rsPointer | signedBinToDec(immediate));

    }else if (funcName == "sb"){
        int8_t* tempRtPointer = (int8_t*)rtPointer;
        int8_t int8 = *tempRtPointer;
        int tempNum = *rsPointer + signedBinToDec(immediate);
        int8_t* tempPointer = (int8_t*) virtualMemory.at(tempNum);
        *tempPointer = int8;

    }else if (funcName == "slti"){
        if (*rsPointer < signedBinToDec(immediate)){
            *rtPointer = 1;
        }else {
            *rtPointer = 0;
        }

    }else if (funcName == "sltiu"){
        if (*rsPointer < binToDec(immediate)){
            *rtPointer = 1;
        }else {
            *rtPointer = 0;
        }

    }else if (funcName == "sh"){
        int16_t* tempRtPointer = (int16_t*)rtPointer;
        int16_t int16 = *tempRtPointer;
        int tempNum = *rsPointer + signedBinToDec(immediate);
        int16_t* tempPointer = (int16_t*) virtualMemory.at(tempNum);
        *tempPointer = int16;

    }else if (funcName == "sw"){
        int tempNum = *rsPointer + signedBinToDec(immediate);
        int* tempPointer = virtualMemory.at(tempNum);
        *tempPointer = *rtPointer;

    }else if (funcName == "xori"){
        *rtPointer = (*rsPointer ^ signedBinToDec(immediate));

    }else if (funcName == "lwl"){
        char* tempPointer = (char*) rsPointer;
        char* tempRtPointer = (char*) rtPointer;
        tempPointer += signedBinToDec(immediate);
        for (int i = binToDec(immediate); i < 4; i++){
            *tempRtPointer = *tempPointer;
            tempPointer++;
            tempRtPointer++;
        }

    }else if (funcName == "lwr"){
        char* tempPointer = (char*) rsPointer;
        char* tempRtPointer = (char*) rtPointer;
        tempRtPointer += (signedBinToDec(immediate)-1);
        for (int i = 0; i <= binToDec(immediate); i++){
            *tempRtPointer = *tempPointer;
            tempPointer++;
            tempRtPointer++;
        }

    }else if (funcName == "swl"){
        int tempNum = signedBinToDec(immediate) % 4;
        char* tempPointer = (char*) rsPointer;
        char* tempRtPointer = (char*) rtPointer;
        tempPointer += tempNum;
        for (int i = tempNum; i < 4; i++){
            *tempPointer = *tempRtPointer;
            tempPointer++;
            tempRtPointer++;
        }

    }else if (funcName == "swr"){
        int tempNum = signedBinToDec(immediate) % 4;
        char* tempPointer = (char*) rsPointer;
        char* tempRtPointer = (char*) rtPointer;
        tempRtPointer += 2;
        for (int i = 0; i <= tempNum; i++){
            *tempPointer = *tempRtPointer;
            tempPointer++;
            tempRtPointer++;
        }
    }
    return 0;
}

void jTypeFunction(vector<string> strList, int* nPcPointer, map<string, int*> registerMemory, map<int, int*> virtualMemory){
    string funcName = JFuncTable.at(strList.at(0));
    string target = strList.at(1);

    if (funcName == "j"){
        int targetAddressNum = signedBinToDec(target) << 2;
        *nPcPointer = targetAddressNum;

    }else if (funcName == "jal"){
        *registerMemory.at("$ra") = (*nPcPointer + 4);
        int targetAddressNum = signedBinToDec(target) << 2;
        *nPcPointer = targetAddressNum;
    }
}

int compare_files(FILE* fp1, FILE* fp2){
    char char1 = fgetc(fp1);
    char char2 = fgetc(fp2);

    while(char1 != EOF && char2 != EOF){
        if(char1 != char2){
            return -1;
        }
        char1 = fgetc(fp1);
        char2 = fgetc(fp2);
    }
    return 0;
}



