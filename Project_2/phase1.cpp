//
// Created by 吴冠霖 on 2022/3/9.
//

#include "phase1.h"
#include "memory.h"

using namespace std;

void stringSplit(string str, const char split, vector<string>& res){
    istringstream iss(str);	// 输入流
    string token;			// 接收缓冲区
    while (getline(iss, token, split)){	// 以split为分隔符
        if (token != "" && token.find("#") == -1) {
            res.push_back(token);
        }else{
            continue;
        }
    }
}

string eliminateTab(string line){
    vector<string> tempList;
    string outString;
    stringSplit(line, '\t', tempList);
    for (int i = 0; i < tempList.size(); i++){
        if (i == 0){
            outString = tempList.at(i);
        }else{
            outString + " " + tempList.at(i);
        }
    }
    return outString;
}

vector<string> openTxtFile(string path){
    ifstream infile(path);
    string line;
    string templine;
    vector<string> tokens;
    while(infile.eof() != true){
        getline(infile, line);
        tokens.push_back(line);
    }

    infile.close();
    return tokens;
}

vector<intCharPair> openAsmFile(string path, map<int, int*> virtualMemory){
    ifstream infile(path);
    string templine;
    string line;
    int counter = 0;
    int dataStartPos = 5242880;
    vector<intCharPair> staticDataMemory;
    while(infile.eof() != true){
        vector<string> tokens;
        getline(infile, templine);

        if (templine.find("#") != -1){
            int endPos = templine.find("#");
            templine = templine.substr(0, endPos);
        }

        if (templine.find(".text") != -1){
            break;
        }

        if (templine.find('\t') != -1){
            line = eliminateTab(templine);
        }else {
            line = templine;
        }

        int variablePos = line.find(":");
        string variableName = line.substr(0, variablePos);

        int *pointer = virtualMemory.at(dataStartPos);
        set<int> charPosSet;
        if (line.find("asciiz") != -1) {
            int charPos;
            int tempNum1 = line.find_first_of("\"")+1;
            int tempNum2 = line.find_last_of("\"")-tempNum1;
            string tempStr = line.substr(tempNum1, tempNum2);

            if (tempStr.find("\\") != -1){
                charPos = tempStr.find("\\");
                charPosSet.emplace(charPos);
                while (tempStr.find("\\", charPos+1) != -1){
                    charPos = tempStr.find("\\", charPos+1);
                    charPosSet.emplace(charPos);
                }

            } else{
                charPos = 1000;
            }
            char *charPointer = (char *) pointer;
            int strLength = tempStr.length();
            for (int i = 0; i < strLength; i++) {
                intCharPair pair;
                if (charPosSet.count(i) != 0){
                    char tempChar;
                    string charString = tempStr.substr(i, 2);
                    if (charString == "\\n"){
                        tempChar = '\n';
                    }
                    *charPointer = tempChar;
                    pair.dataPos = dataStartPos;
                    pair.realAddress = charPointer;
                    if (i == 0){
                        pair.variableName = variableName;
                    }else{
                        pair.variableName = "1";
                    }
                    staticDataMemory.push_back(pair);
                    i += 1;
                    charPointer++;
                    dataStartPos++;
                    continue;
                }else {
                    *charPointer = tempStr.at(i);
                    pair.dataPos = dataStartPos;
                    pair.realAddress = charPointer;
                }

                if (i==0){
                    pair.variableName = variableName;
                }else{
                    pair.variableName = "1";
                }
                staticDataMemory.push_back(pair);
                charPointer++;
                dataStartPos++;
            }
            *charPointer = '\0';
            intCharPair tempPair;
            tempPair.dataPos = dataStartPos;
            tempPair.realAddress = charPointer;
            tempPair.variableName = "1";
            staticDataMemory.push_back(tempPair);
            dataStartPos++;
            int ans = dataStartPos % 4 ? 4- (dataStartPos % 4) : dataStartPos % 4;
            dataStartPos += ans;

        } else if (line.find("ascii") != -1) {
            int charPos;
            int tempNum1 = line.find_first_of("\"")+1;
            int tempNum2 = line.find_last_of("\"")-tempNum1;
            string tempStr = line.substr(tempNum1, tempNum2);
            if (tempStr.find("\\") != -1){
                charPos = tempStr.find("\\");
                charPosSet.emplace(charPos);
                while (tempStr.find("\\", charPos+1) != -1){
                    charPos = tempStr.find("\\", charPos+1);
                    charPosSet.emplace(charPos);
                }
            }
            char *charPointer = (char *) pointer;
            int strLength = tempStr.length();
            for (int i = 0; i < strLength; i++) {
                intCharPair pair;
                if (charPosSet.count(i) != 0){
                    char tempChar;
                    string charString = tempStr.substr(i, 2);
                    if (charString == "\\n"){
                        tempChar = '\n';
                    }
                    *charPointer = tempChar;
                    pair.dataPos = dataStartPos;
                    pair.realAddress = charPointer;
                    pair.variableName = variableName;
                    staticDataMemory.push_back(pair);
                    i += 1;
                    charPointer++;
                    dataStartPos++;
                    continue;
                }else {
                    *charPointer = tempStr.at(i);
                    pair.dataPos = dataStartPos;
                    pair.realAddress = charPointer;
                }

                if (i==0){
                    pair.variableName = variableName;
                }else{
                    pair.variableName = "1";
                }

                staticDataMemory.push_back(pair);
                charPointer++;
                dataStartPos++;
            }
            int ans = dataStartPos % 4 ? 4- (dataStartPos % 4) : dataStartPos % 4;
            dataStartPos += ans;

        } else if (line.find("word") != -1) {
            stringSplit(line, ' ', tokens);
            string tempStr = tokens.back();
            vector<string> tempVec;
            stringSplit(tempStr, ',', tempVec);
            for (int i = 0; i < tempVec.size(); i++) {
                intCharPair pair;
                *pointer = stoi(tempVec.at(i));
                char *charPointer = (char *) pointer;
                pair.dataPos = dataStartPos;
                pair.realAddress = charPointer;
                pointer++;
                dataStartPos+=4;
                if (i == 0) {
                    pair.variableName = variableName;
                } else {
                    pair.variableName = "1";
                }
                staticDataMemory.push_back(pair);
            }


        } else if (line.find("byte") != -1) {
            stringSplit(line, ' ', tokens);
            string tempStr = tokens.back();
            vector<string> tempVec;
            stringSplit(tempStr, ',', tempVec);
            string finalStr;
            int8_t* int8Pointer = (int8_t*) pointer;
            for (int i = 0; i < tempVec.size(); i++){
                int8_t int8 = stoi(tempVec.at(i));
                intCharPair pair;
                char* charPointer = (char*) int8Pointer;
                pair.dataPos = dataStartPos;
                pair.realAddress = charPointer;
                *int8Pointer = int8;
                int8Pointer++;
                dataStartPos++;
                if (i==0){
                    pair.variableName = variableName;
                }else{
                    pair.variableName = "1";
                }
                staticDataMemory.push_back(pair);
            }
            int ans = dataStartPos % 4 ? 4- (dataStartPos % 4) : dataStartPos % 4;
            dataStartPos += ans;

        } else if (line.find("half") != -1) {
            stringSplit(line, ' ', tokens);
            string tempStr = tokens.back();
            vector<string> tempVec;
            stringSplit(tempStr, ',', tempVec);
            int16_t* int16Pointer = (int16_t*) pointer;
            for (int i = 0; i < tempVec.size(); i++) {
                intCharPair pair;
                *int16Pointer = stoi(tempVec.at(i));
                char *charPointer = (char *) int16Pointer;
                pair.dataPos = dataStartPos;
                pair.realAddress = charPointer;
                int16Pointer++;
                dataStartPos+=2;
                if (i == 0) {
                    pair.variableName = variableName;
                } else {
                    pair.variableName = "1";
                }
                staticDataMemory.push_back(pair);
            }
            int ans = dataStartPos % 4 ? 4- (dataStartPos % 4) : dataStartPos % 4;
            dataStartPos += ans;
        }
    }
    intCharPair finalPair;
    finalPair.dataPos = dataStartPos;
    finalPair.realAddress = (char*) virtualMemory.at(dataStartPos);
    finalPair.variableName = "1";
    staticDataMemory.push_back(finalPair);
    infile.close();
    return staticDataMemory;
}

map<string, char*> createLableTable(vector<intCharPair> dataVector){
    map<string, char*> LableTable;
    int strLength = dataVector.size();
    for (int i = 0; i < strLength; i++){
        intCharPair tempPair = dataVector.at(i);
        if (tempPair.variableName != "1"){
            LableTable.emplace(tempPair.variableName, tempPair.realAddress);
        }
    }
    return LableTable;
}

queue<string> openInFile(string path){
    ifstream infile(path);
    string templine;
    string line;
    queue<string> inFileQueue;
    while(infile.eof() != true) {
        getline(infile, templine);
        if (templine.find("\n") != -1){
            int pos = templine.find("\n");
            line = templine.substr(0, templine.length()-pos);
        }else{
            line = templine;
        }
        inFileQueue.push(line);
    }
    return inFileQueue;
}