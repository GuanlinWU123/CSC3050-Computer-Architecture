#include <iostream>
#include "LabelTable.h"
#include "phase1.h"
#include "phase2.h"

using namespace std;

int compare_files(FILE* fp1, FILE* fp2){
    char char1 = fgetc(fp1);
    char char2 = fgetc(fp2);

    while(char1 != EOF && char2 != EOF){
        if(char1 != char2){
            return -1;
        }
        char1 = fgetc(fp1);
        char2 = fgetc(fp2);
    }
    return 0;
}

//int main(int argc, char* argv[]) {
//    if (argc < 4){
//        printf("Please enter an input file, an output file, and expected output file \n");
//    }
//
//    LabelTable lableTable = createLabelTable(argv[1]);
//    vector<string> tokens = openFile(argv[1]);
//    phase2(argv[2], lableTable, tokens);
//
//    FILE* fp1;
//    FILE* fp2;
//
//    fp1 = fopen(argv[3], "r");
//    fp2 = fopen(argv[2], "r");
//
//    if(fp1 == NULL || fp2 == NULL){
//        printf("Error: Files are not open correctly \n");
//    }
//
//    int res = compare_files(fp1, fp2);
//
//    if(res == 0){
//        printf("ALL PASSED! CONGRATS :) \n");
//    }else{
//        printf("YOU DID SOMETHING WRONG :( \n");
//    }
//
//    fclose(fp1);
//    fclose(fp2);
int main(){
    vector<string> filePaths {"testfile1.asm", //Absolute address of test files
                              "testfile2.asm",
                              "testfile3.asm",
                              "testfile4.asm",
                              "testfile5.asm"};

    for (int j = 0; j < filePaths.size(); j++){
        string fileOutPath = "/Users/wuguanlin/Desktop/CSC3050/Assignment_1/Ass1/realOutput" + to_string(j+1) + ".txt";
        string path = filePaths.at(j);
        cout << path.substr(51, 13) << endl;
        if(path.substr(51, 13) == "testfile2.asm"){
            cout << 2 << endl;
        }
        LabelTable lableTable = createLabelTable(path);
        vector<string> tokens = openFile(path);
        phase2(fileOutPath, lableTable, tokens);
    }
    FILE* fp1;
    FILE* fp2;
    FILE* fp3;
    FILE* fp4;
    FILE* fp5;
    FILE* fp6;
    FILE* fp7;
    FILE* fp8;
    FILE* fp9;
    FILE* fp10;


    fp1 = fopen("expectedoutput.txt", "r");
    fp3 = fopen("expectedoutput2.txt", "r");
    fp5 = fopen("expectedoutput3.txt", "r");
    fp7 = fopen("expectedoutput4.txt", "r");
    fp9 = fopen("expectedoutput5.txt", "r");

    fp2 = fopen("realOutput1.txt", "r"); //Will be generated after the former instruction
    fp4 = fopen("realOutput2.txt", "r");
    fp6 = fopen("realOutput3.txt", "r");
    fp8 = fopen("realOutput4.txt", "r");
    fp10 = fopen("realOutput5.txt", "r");

    if(fp1 == NULL || fp2 == NULL || fp3 == NULL || fp4 == NULL || fp5 == NULL || fp6 == NULL){
        printf("Error: Files are not open correctly \n");
    }

    int counter = 0;
    while (counter < 5){
        int res;
        if (counter == 0){
            cout << "First comparison between realOutput1.txt and expectedoutput.txt, result: " << endl;
            res = compare_files(fp1, fp2);
        }else if (counter == 1){
            cout << "Second comparison between realOutput2.txt and expectedoutput2.txt, result: " << endl;
            res = compare_files(fp3, fp4);
        }else if (counter == 2){
            cout << "Third comparison between realOutput3.txt and expectedoutput3.txt, result: " << endl;
            res = compare_files(fp5, fp6);
        }else if (counter == 3){
            cout << "Fourth comparison between realOutput3.txt and expectedoutput3.txt, result: " << endl;
            res = compare_files(fp7, fp8);
        }else {
            cout << "Fifth comparison between realOutput3.txt and expectedoutput3.txt, result: " << endl;
            res = compare_files(fp9, fp10);
        }

        if(res == 0){
            printf("ALL PASSED! CONGRATS :) \n");
        }else{
            printf("YOU DID SOMETHING WRONG :( \n");
        }
        counter ++;
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    fclose(fp6);
    fclose(fp7);
    fclose(fp8);
    fclose(fp9);
    fclose(fp10);
    return 0;
}
