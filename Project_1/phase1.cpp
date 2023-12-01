//
// Created by 吴冠霖 on 2022/2/19.
//

#include "phase1.h"

using namespace std;

void StringSplit1(string str, const char split, vector<string>& res){
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

string eliminateTab(string line){
    vector<string> tempList;
    string outString;
    StringSplit1(line, '\t', tempList);
    for (int i = 0; i < tempList.size(); i++){
        if (i == 0){
            outString = tempList.at(i);
        }else{
            outString + " " + tempList.at(i);
        }
    }
    return outString;
}

LabelTable createLabelTable(string filePath){
    ifstream infile(filePath);
    string templine;
    string line;
    LabelTable labelTable;
    int PC = 4194304; //0x400000 in decimal number


    while(infile.eof() != true){
        getline(infile, templine);
        if (templine == "" || templine.at(0) == '#') {
            continue;
        }
        while (templine.at(0) == ' '){
            string aLine;
            aLine = templine.substr(1, templine.length()-1);
            templine = aLine;
        }

        if (templine.find('\r') != -1){
            templine.erase(remove(templine.begin(), templine.end(), '\r'), templine.end());
        }

        if (templine.find('\t') != -1){
            line = eliminateTab(templine);
        }else{
            line = templine;
        }
        if (line.find('.') != -1 ){
            continue;
        } else if (line.find(':') != -1){
            int pos = line.find(':');
            if (pos == line.length() - 1){
                labelTable.put(line.substr(0, pos), to_string(PC));
            } else {
                int numOfLength;
                if (line.find('#') != -1){
                    int tempPos = line.find('#');
                    numOfLength = tempPos - (pos + 1);
                }else{
                    numOfLength = line.length() - (pos + 1);
                }
                labelTable.put(line.substr(0, pos), to_string(PC));
                labelTable.put(line.substr(pos + 1, numOfLength), to_string(PC));
                PC = PC + 4;
            }
        } else{
            int numOfLength;
            if (line.find('#') != -1) {
                int tempPos = line.find('#');
                numOfLength = tempPos - 1;
            } else {
                numOfLength = line.length();
            }
            labelTable.put(line.substr(0, numOfLength), to_string(PC));
            PC = PC + 4;
        }
    }
    infile.close();
    return labelTable;
}

vector<string> openFile(string path){
    ifstream infile(path);
    string line;
    string templine;
    vector<string> tokens;
    while(infile.eof() != true){
        getline(infile, templine);
        if (templine == ""|| templine.at(0) == '.' || templine.find(':') != -1) {
            continue;
        }
        while (templine.at(0) == ' '){
            string aLine;
            aLine = templine.substr(1, templine.length()-1);
            templine = aLine;
        }
        line = eliminateTab(templine);

        if(line.find('#') != -1) {
            if (line.find('#' == 0)){
                continue;
            }else{
                int pos = line.find('#');
                line = line.substr(0, pos);
            }
        }
        tokens.push_back(line);
    }

    infile.close();
    return tokens;
}



