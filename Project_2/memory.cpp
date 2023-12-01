//
// Created by 吴冠霖 on 2022/3/8.
//
#include "memory.h"

using namespace std;

string hexToBin(string hexString){
    stringstream temp;
    temp << hex << hexString;
    unsigned n;
    temp >> n;
    bitset<32> b(n);

    return b.to_string();
}

string decToHex(int n){
    stringstream ss;
    ss << setfill('0') << std::setw(8) << hex << n;
    string res = ss.str();
    return res;
}

string decToBin64(int value){
    stringstream ht;
    ht << hex << value;
    stringstream bt;
    bt << hex << ht.str();
    unsigned n;
    bt >> n;
    bitset<64> b(n);
    string tempStr = b.to_string();
    if (value > 0){
        return tempStr;
    } else{
        string finalStr;
        int tempPos = tempStr.find_last_of('1');
        for (int i = 0; i < tempPos; i++){
            if (tempStr.at(i) == '0'){
                finalStr += "1";
            } else {
                finalStr += "0";
            }
        }
        finalStr += tempStr.substr(tempPos, tempStr.length()-tempPos-1);
        return finalStr;
    }
}

string decToBin8(int value){
    stringstream ht;
    ht << hex << value;
    stringstream bt;
    bt << hex << ht.str();
    unsigned n;
    bt >> n;
    bitset<6> b(n);
    string tempStr = b.to_string();
    return tempStr;
}

string decToBin32(int value){
    stringstream ht;
    ht << hex << value;
    stringstream bt;
    bt << hex << ht.str();
    unsigned n;
    bt >> n;
    bitset<32> b(n);
    string tempStr = b.to_string();
    return tempStr;
}

int binToDec(string n) {
    bitset<32> bit(n);
    int decNumber = bit.to_ullong();
    return decNumber;
}

int signedBinToDec(string binStr){ //bin to dec
    int decNumber;
    int size = binStr.length();
    if (binStr.at(0) == '0'){
        decNumber = binToDec(binStr);
    } else {
        decNumber = -32768 + binToDec(binStr.substr(1, size-1));
    }
    return decNumber;
}


map<int, int*> createMemory(){
    int startOfStack;
    int sizeOfMemoryInt = 1572864;
    int startOfStatic = 5242880;
    int* p = (int *)malloc(sizeOfMemoryInt * sizeof(int));
//    int* p = (int *)malloc(1536000*sizeof(int)); //6MB
    int virtualPos = 4194304;
    map<int, int*> virtualMemory;
    for (int i = 0; i < sizeOfMemoryInt; i++){
        virtualMemory.emplace(virtualPos, &p[i]);
        if (virtualPos == 5242880){
            startOfStatic = virtualPos;
        } else if (i == 1535999){
            startOfStack = virtualPos;
        }
        virtualPos += 4;
    }

    return virtualMemory;
}

int putText(map<int, int*> virtualMemory, vector<string> tokens){
    int PC = 4194304;
    for (int i = 0; i < tokens.size(); i++){
        int number = binToDec(tokens[i]);
        *virtualMemory.at(PC) = number;
        PC += 4;
    }
    return PC;
}

map<string, int*> createRegister(map<int, int*> virtualMemory){
    string registerName[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
                             "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5",
                             "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra", "$pc",
                             "$hi", "$lo"};
    int* p = (int *)malloc(35*sizeof(int));
    map<string, int*> registerMemory;
    for (int i = 0; i < 35; i++){
        registerMemory.emplace(registerName[i], &p[i]);
    }
    *registerMemory.find("$pc")->second = 4194304;
    *registerMemory.find("$sp")->second = 10485760;
    *registerMemory.find("$fp")->second = 10485760;
    *registerMemory.find("$gp")->second = 5275648;
    return registerMemory;
}



void displayMemory(map<string, int*> virtualMemory){
    cout << "$pc : " << *(virtualMemory.at("$pc")) << endl;
    for (map<string, int*>::iterator it=virtualMemory.begin(); it!=virtualMemory.end(); ++it) {
        string temp = it->first;
        cout << temp << ":" << *(it->second) << endl;
//        printf("%d\n", *(it->second));
    }
}

void init_checkpoints(set<int> checkpoints, char* checkpoint_file){
    FILE* fp = fopen(checkpoint_file, "r");
    int tmp, i = 0;
    while (fscanf(fp, "%d", &tmp) != EOF){
        checkpoints.insert(tmp);
    }
}

set<int> createCheckPointsSet(string path){
    ifstream infile(path);
    string line;
    set<int> checkPoints;
    while(infile.eof() != true) {
        getline(infile, line);
        if (line != ""){
            checkPoints.emplace(stoi(line));
        }else{
            continue;
        }
    }
    return checkPoints;
}

void checkpoint_memory(set<int> checkpoints, int ins_count, int* memory_pointer){
    if (!checkpoints.count(ins_count)){
        return;
    }
    string name = "memory_" + to_string(ins_count) + ".bin";
    FILE* fp = fopen(name.c_str(), "wb");
    fwrite(memory_pointer, 1, 0x600000, fp);
    fclose(fp);
}

void checkpoint_register(map<string, int*> registerMemory, int* register_pointer, set<int> checkpoints, int ins_count){
    if (!checkpoints.count(ins_count)){
        return;
    }
    string name = "register_" + to_string(ins_count) + ".bin";
    FILE* fp = fopen(name.c_str(), "wb");
    for (int i = 0; i < 35; i++){
        fwrite(register_pointer, 4, 1, fp);
        register_pointer++;
    }
    fclose(fp);
}




