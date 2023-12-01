#include "memory.h"
#include "phase1.h"
#include "phase2.h"

using namespace std;



//int main() {
int main(int argc, char* argv[]) {
    string asmPath = argv[1];
    string txtPath = argv[2];
    string checkPtsPath = argv[3];
    string inFilePath = argv[4];
    string outFileName = argv[5];

//    string asmPath = "/Users/wuguanlin/Desktop/CSC3050/Assignment_2/Ass2/many.asm";
//    string txtPath = "/Users/wuguanlin/Desktop/CSC3050/Assignment_2/Ass2/many.txt";
//    string inFilePath = "/Users/wuguanlin/Desktop/CSC3050/Assignment_2/Ass2/many.in";
//    string checkPtsPath = "/Users/wuguanlin/Desktop/CSC3050/Assignment_2/Ass2/many_checkpts.txt";
//    string outFileName = "many.out";

    map<int, int *> virtualMemory = createMemory();
    vector<intCharPair> pairVector = openAsmFile(asmPath, virtualMemory);
    vector<string> tokens = openTxtFile(txtPath);
    queue<string> fileInQueue = openInFile(inFilePath);
    set<int> checkPoints = createCheckPointsSet(checkPtsPath);

    for (int i = 0; i < pairVector.size(); i++) {
        intCharPair pair = pairVector.at(i);
        if (pair.variableName == "1") {
            virtualMemory.emplace(pair.dataPos, (int *) pair.realAddress);
        }
    }

    int endOfTextInt = putText(virtualMemory, tokens);
    map<string, int *> registerMemory = createRegister(virtualMemory);
    checkPoints.emplace(0);

    int counter = 0;
    int *nPcPointer = registerMemory.find("$pc")->second;
    int *pcPointer;

    int startOfDynamic = pairVector.back().dataPos;

    FILE *ofile = fopen(outFileName.c_str(), "wb");
    setbuf(ofile, NULL);
    while (*nPcPointer <= endOfTextInt) {
        pcPointer = virtualMemory.at(*nPcPointer);
        checkpoint_memory(checkPoints, counter, virtualMemory.at(4194304));
        checkpoint_register(registerMemory, registerMemory.at("$zero"), checkPoints, counter);
        string text = decToBin32(*pcPointer);
        string type = JudgeType(text);
        vector<string> editedText = textEditing(text, type);
        if (type == "r") {
            if (editedText.back() == "001100") {
                int syscallNum = *(registerMemory.at("$v0"));
                if (syscallNum == 9) { //sbrk
                    *registerMemory.at("$v0") = startOfDynamic;
                    int tempInt = *registerMemory.at("$a0");
                    startOfDynamic += tempInt;
                } else {
                    fileInQueue = syscallFunction(syscallNum, registerMemory.at("$a0"), fileInQueue, ofile,
                                                  registerMemory, virtualMemory);
                }
            } else if (editedText.back() == "001000") { //jr
                string rs = registerTrans(text.substr(6, 5));
                int *rsPointer = registerMemory.at(rs);
                *nPcPointer = *rsPointer;
                continue;
            } else if (editedText.back() == "001001") { //jalr
                string rs = registerTrans(text.substr(6, 5));
                string rd = registerTrans(text.substr(16, 5));
                int *rsPointer = registerMemory.at(rs);
                int *rdPointer = registerMemory.at(rd);
                *nPcPointer += 4;
                *rdPointer = *nPcPointer;
                *nPcPointer = *rsPointer;
            } else {
                rTypeFunction(editedText, registerMemory, virtualMemory, fileInQueue);
            }
        } else if (type == "i") {
            int flag = iTypeFunction(editedText, nPcPointer, registerMemory, virtualMemory);
            if (flag == 1) {
                continue;
            }
        } else if (type == "j") {
            jTypeFunction(editedText, nPcPointer, registerMemory, virtualMemory);
            continue;
        }

        *nPcPointer += 4;
        counter++;
    }
    fclose(ofile);

//    FILE* fp1 = fopen("/Users/wuguanlin/Desktop/CSC3050/Assignment_2/Ass2/cmake-build-debug/register_97.bin", "r");
//    FILE* fp2 = fopen("/Users/wuguanlin/Desktop/tests/many/correct_dump/register_97.bin", "r");
//    int result = compare_files(fp1, fp2);
//    cout << result << endl;
}