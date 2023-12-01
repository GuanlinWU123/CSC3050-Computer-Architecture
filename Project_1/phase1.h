//
// Created by 吴冠霖 on 2022/2/19.
//

#ifndef ASS1_PHASE1_H
#define ASS1_PHASE1_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "LabelTable.h"

void StringSplit1(std::string str, const char split, std::vector<std::string>& res);

std::vector<std::string> openFile(std::string path);

LabelTable createLabelTable(std::string filePath);

#endif //ASS1_PHASE1_H
