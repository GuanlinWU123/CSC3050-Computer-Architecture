//
// Created by 吴冠霖 on 2022/3/5.
//

#ifndef ASS2_PHASE2_H
#define ASS2_PHASE2_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <string>
#include <queue>
#include <sstream>
#include <bitset>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <set>

std::string JudgeType(std::string opcode);

std::string registerTrans(std::string regisNum);

std::vector<std::string> textEditing(std::string text, std::string type);

std::queue<std::string> syscallFunction(int number, int* pointer, std::queue<std::string> fileInQueue, FILE* ofile, std::map<std::string, int*> registerMemory, std::map<int, int*> virtualMemory);

void rTypeFunction(std::vector<std::string> strList, std::map<std::string, int*> registerMemory, std::map<int, int*> virtualMemory, std::queue<std::string> fileInQueue);

int iTypeFunction(std::vector<std::string> strList, int* nPcPointer, std::map<std::string, int*> registerMemory, std::map<int, int*> virtualMemory);

void jTypeFunction(std::vector<std::string> strList, int* nPcPointer, std::map<std::string, int*> registerMemory, std::map<int, int*> virtualMemory);

int compare_files(FILE* fp1, FILE* fp2);

#endif //ASS2_PHASE2_H
