//
// Created by 吴冠霖 on 2022/2/21.
//

#ifndef ASS1_PHASE2_H
#define ASS1_PHASE2_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <string>
#include <sstream>
#include <bitset>
#include "LabelTable.h"


std::string DecToBin(int value);

std::string JTypeDecToBin(int value);

void StringSplit(std::string str, const char split, std::vector<std::string>& res);

std::vector<std::string> TextEditing(std::string text);

std::string JudgeType(std::string type);

std::string RTypeInstruction(std::string func, std::string rs, std::string rt, std::string rd, std::string shamt);

std::string CallRType(std::string func, std::vector<std::string> strList);

std::string ITypeInstruction(std::string func, std::string rt, std::string rs, std::string imm);

std::string CallIType(std::string func, std::vector<std::string> strList);

int CheckIfBranch(std::string ins);

std::string BranchFunction(std::string func, std::string ins, std::vector<std::string> strList, LabelTable labelTable, int startPos);

int phase2(std::string fileOutPath, LabelTable labelTable, std::vector<std::string> tokens);

#endif //ASS1_PHASE2_H
