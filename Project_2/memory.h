//
// Created by 吴冠霖 on 2022/3/8.
//

#ifndef ASS2_MEMORY_H
#define ASS2_MEMORY_H
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <bitset>
#include <set>
#include <map>

std::string hexToBin(std::string hexString);

std::string decToHex(int n);

std::string decToBin64(int value);

std::string decToBin8(int value);

std::string decToBin32(int value);

int binToDec(std::string n);

int signedBinToDec(std::string binStr);

std::map<int, int*> createMemory();

int putText(std::map<int, int*> virtualMemory, std::vector<std::string> tokens);

std::map<std::string, int*> createRegister(std::map<int, int*> virtualMemory);

void init_checkpoints(std::set<int> checkpoints, char* checkpoint_file);

std::set<int> createCheckPointsSet(std::string path);

void checkpoint_memory(std::set<int> checkpoints, int ins_count, int* memory_pointer);

void checkpoint_register(std::map<std::string, int*> registerMemory, int* register_pointer, std::set<int> checkpoints, int ins_count);

void displayMemory(std::map<std::string, int*> virtualMemory);
#endif //ASS2_MEMORY_H
