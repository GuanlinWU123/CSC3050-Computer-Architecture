//
// Created by 吴冠霖 on 2022/3/9.
//

#ifndef ASS2_PHASE1_H
#define ASS2_PHASE1_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <queue>
#include <set>

struct intCharPair{
    int dataPos;
    char* realAddress;
    std::string variableName;
};

void stringSplit(std::string str, const char split, std::vector<std::string>& res);

std::vector<std::string> openTxtFile(std::string path);

std::vector<intCharPair> openAsmFile(std::string path, std::map<int, int*> virtualMemory);

std::queue<std::string> openInFile(std::string path);

std::map<std::string, char*>createLableTable(std::vector<intCharPair> dataVector);

#endif //ASS2_PHASE1_H
