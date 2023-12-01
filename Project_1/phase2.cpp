//
// Created by 吴冠霖 on 2022/2/20.
//
#include "phase2.h"

using namespace std;

typedef map<string, string> Map ;

Map registerTable ({{"$zero", "00000"}, {"$at", "00001"}, {"$v0", "00010"}, {"$v1", "00011"},
                    {"$a0",   "00100"}, {"$a1", "00101"}, {"$a2", "00110"}, {"$a3", "00111"},
                    {"$t0",   "01000"}, {"$t1", "01001"}, {"$t2", "01010"}, {"$t3", "01011"},
                    {"$t4",   "01100"}, {"$t5", "01101"}, {"$t6", "01110"}, {"$t7", "01111"},
                    {"$s0",   "10000"}, {"$s1", "10001"}, {"$s2", "10010"}, {"$s3", "10011"},
                    {"$s4",   "10100"}, {"$s5", "10101"}, {"$s6", "10110"}, {"$s7", "10111"},
                    {"$t8",   "11000"}, {"$t9", "11001"}, {"$k0", "11010"}, {"$k1", "11011"},
                    {"$gp",   "11100"}, {"$sp", "11101"}, {"$s8", "11110"}, {"$fp", "11110"},
                    {"$ra",   "11111"}} );

Map RFuncTable ( {{"add","100000"}, {"addu","100001"}, {"and","100100"}, {"div","011010"},
                  {"divu","011011"}, {"jalr","001001"}, {"jr","001000"}, {"mfhi","010000"},
                  {"mflo","010010"}, {"mthi","010001"}, {"mtlo","010011"}, {"mult","011000"},
                  {"multu","011001"}, {"nor","100111"}, {"or","100101"}, {"sll","000000"},
                  {"sllv","000100"}, {"slt","101010"}, {"sltu","101011"}, {"sra","000011"},
                  {"srav","000111"}, {"srl","000010"}, {"srlv","000110"}, {"sub","100010"},
                  {"subu","100011"}, {"syscall","001100"}, {"xor","100110"}} );

Map IFuncTable ( {{"addi", "001000"}, {"addiu", "001001"}, {"andi", "001100"}, {"beq", "000100"},
                  {"bgez", "000001"}, {"bgtz", "000111"}, {"blez", "000110"}, {"bltz", "000001"},
                  {"bne", "000101"}, {"lb", "100000"}, {"lbu", "100100"}, {"lh", "100001"},
                  {"lhu", "100101"}, {"lui", "001111"}, {"lw", "100011"}, {"ori", "001101"},
                  {"sb", "101000"}, {"slti", "001010"}, {"sltiu", "001011"}, {"sh", "101001"},
                  {"sw", "101011"}, {"xori", "001110"}, {"lwl", "100010"}, {"lwr", "100110"},
                  {"swl", "101010"}, {"swr", "101110"}} );

Map JFuncTable( {{"j","000010"}, {"jal","000011"}} );

/* Converts decimal values to hex values (Outputs a string) */
string DecToBin(int value){
    stringstream ht;
    ht << hex << value;
    stringstream bt;
    bt << hex << ht.str();
    unsigned n;
    bt >> n;
    bitset<16> b(n);
    return b.to_string();
}

string JTypeDecToBin(int value){
    stringstream ht;
    ht << hex << value;
    stringstream bt;
    bt << hex << ht.str();
    unsigned n;
    bt >> n;
    bitset<26> b(n);
    return b.to_string();
}

void StringSplit(string str, const char split, vector<string>& res){
    istringstream iss(str);	// 输入流
    string token;			// 接收缓冲区
    while (getline(iss, token, split)){	// 以split为分隔符
        if (token != "") {
            res.push_back(token);
        }else{
            continue;
        }
    }
}

vector<string> TextEditing(string text){
    vector<string> strList;
    StringSplit(text, ' ', strList);
    for (int i = 0; i < strList.size(); i++){
        int pos = strList[i].size() - 1;
        if (strList[i].at(pos) == ','){
            int strLength = strList[i].length() - 1;
            string tempStr = strList[i].substr(0, strLength);
            strList[i] = tempStr;
        }else{
            continue;
        }
    }

    string func = strList.at(0); //I type
    if (func == "lb" || func == "lbu" || func == "lh" || func == "lhu" || func == "lw" ||
        func == "sb" || func == "sh" || func == "sw" || func == "lwl" || func == "lwr" ||
        func == "swl" || func == "swr"){
        string originalStr = strList.at(2);
        int posStart = originalStr.find("(");
        int posEnd = originalStr.size() - 1;
        string rs = originalStr.substr(posStart+1, posEnd - posStart-1);
        string imm = originalStr.substr(0, posStart);
        strList[2] = rs;
        strList.push_back(imm);
    }
    return strList;
}

string JudgeType(string type){
    if (RFuncTable.find(type) != RFuncTable.end()){
        return "r";
    }else if (JFuncTable.find(type) != JFuncTable.end()){
        return "j";
    }else{
        return "i";
    }
}

string RTypeInstruction(string func, string rd, string rs, string rt, string shamt){
    string rsOut;
    string rtOut;
    string rdOut;
    string shamtOut;

    if (rs != "00000"){
        rsOut = registerTable[rs];
    }else{
        rsOut = rs;
    }

    if (rt != "00000"){
        rtOut = registerTable[rt];
    }else{
        rtOut = rt;
    }

    if (rd != "00000"){
        rdOut = registerTable[rd];
    }else{
        rdOut = rd;
    }

    if (shamt != "00000"){
        string tempStr = DecToBin(stoi(shamt));
        shamtOut = tempStr.substr(11,5);
    }else{
        shamtOut = shamt;
    }

    string funcOut = RFuncTable[func];
    string output;
    output = "000000" + rsOut + rtOut + rdOut + shamtOut + funcOut;
    return output;
}

string CallRType(string func, vector<string> strList){
    string output;
    if (func == "jr" || func == "mthi" || func == "mtlo") { //rs
        output =RTypeInstruction(func, "00000", strList.at(1), "00000", "00000");

    } else if (func == "mfhi" || func == "mflo"){ //rd
        output =RTypeInstruction(func, strList.at(1), "00000", "00000", "00000");

    } else if (func == "div" || func == "divu" || func == "mult" || func == "multu") { //rsrt
        output =RTypeInstruction(func, "00000", strList.at(1), strList.at(2), "00000");

    } else if (func == "jalr"){ //rsrd
        output =RTypeInstruction(func, strList.at(1), strList.at(2), "00000", "00000");

    } else if (func == "syscall"){ //null
        output =RTypeInstruction(func, "00000", "00000", "00000", "00000");

    } else if (func == "sll" || func == "sra" || func == "srl") { //rdrtsa
        output =RTypeInstruction(func, strList.at(1), "00000", strList.at(2), strList.at(3));

    }else if (func == "sllv" || func == "srav" || func == "srlv"){ //rdrtrs
        output =RTypeInstruction(func, strList.at(1), strList.at(3), strList.at(2), "00000");

    } else { //rdrtrs
        output =RTypeInstruction(func, strList.at(1), strList.at(2), strList.at(3), "00000");
    }
    return output;
}


string ITypeInstruction(string func, string rt, string rs, string imm) {
    string rsOut;
    string rtOut;

    if (rs == "00000"){
        rsOut = rs;
    }else{
        rsOut = registerTable[rs];
    }

    if (rt == "00000" || rt == "00001"){
        rtOut = rt;
    }else{
        rtOut = registerTable[rt];
    }
    string immOut = DecToBin(stoi(imm));
    string output = IFuncTable[func] + rsOut + rtOut + immOut;
    return output;
}

string CallIType(string func, vector<string> strList) {
    string output;
    if (func == "lui") {
        output = ITypeInstruction(func, strList.at(1), "00000", strList.at(2));

    } else {
        output = ITypeInstruction(func, strList.at(1), strList.at(2), strList.at(3));
    }
    return output;
}

int CheckIfBranch(string ins) {
    if (ins.substr(0, 3) == "beq" || ins.substr(0, 3) == "bne") {
        return 1;

    }else if (ins.substr(0, 4) == "bgez" || ins.substr(0, 4) == "bgtz" ||
              ins.substr(0, 4) == "blez" || ins.substr(0, 4) == "bltz") {
        return 1;
    }else {
        return 0;
    }
}

string JTypeInstruction(string func, string address){
    string funcOut = JFuncTable[func];
    string addressOut = JTypeDecToBin(stoi(address));
    string output = funcOut + addressOut;
    return output;
}

string BranchFunction(string func, string ins, vector<string> strList, LabelTable labelTable, int startPos){
    string output;
    string rt;
    int operationLabel;
    int currentLabel = stoi(labelTable.get(ins, startPos)) + 4;

    if (func == "j" || func == "jal") {
        operationLabel = stoi(labelTable.get(strList.at(1), 0));
        string address = to_string(operationLabel / 4);
        output = JTypeInstruction(func, address);
        return output;
    }

    if (func == "beq" || func == "bne") {
        operationLabel = stoi(labelTable.get(strList.at(3), 0));
        rt = strList.at(2);

    } else if (func == "bgez") {
        operationLabel = stoi(labelTable.get(strList.at(2), 0));
        rt = "00001";

    } else {
        operationLabel = stoi(labelTable.get(strList.at(2), 0));
        rt = "00000";
    }
    string label = to_string((operationLabel - currentLabel) / 4);
    output = ITypeInstruction(func, rt, strList.at(1), label);
    return output;
}

int phase2(string fileOutPath, LabelTable labelTable, vector<string> tokens){
    ofstream fileOut(fileOutPath);
    string output;
    for (int i = 0; i < tokens.size(); i++){
        vector<string> strList = TextEditing(tokens.at(i));
        if (CheckIfBranch(tokens.at(i)) == 1){
            output = BranchFunction(strList.at(0), tokens.at(i), strList, labelTable, i);

        } else{
            if (JudgeType(strList.at(0)) == "r"){
                output = CallRType(strList.at(0), strList);

            }else if (JudgeType(strList.at(0)) == "j"){
                output = BranchFunction(strList.at(0), tokens.at(i), strList, labelTable, 0);

            }else{
                output = CallIType(strList.at(0), strList);
            }
        }
        fileOut << output << endl;
        cout << output << endl;
    }
    fileOut.close();
    return 0;
}


