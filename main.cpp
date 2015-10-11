// main.cpp

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include "fileio.hpp"
#include "convertMachine.hpp"

using namespace std;

FILEIO fileio;

ConvertMachine convert_machine;

int main() {
    // teacherオブジェクト生成
    vector<TEACHER> teachers;
    // 講師情報の読み込み
    const char* path_teacher = "./teacher";
    DIR *dp;       // ディレクトリへのポインタ
    dirent* entry; // readdir() で返されるエントリーポイント
    dp = opendir(path_teacher);
    if (dp == NULL) return 1;
    while ((entry = readdir(dp)) != NULL) {
        string filename;
        filename = (string)entry->d_name;
        if (filename[0] == '.') continue;
        teachers.push_back(fileio.InputTeacherData(filename, convert_machine));
    }
    
    // studentオブジェクトを生成
    vector<STUDENT> students;
    // 生徒情報の読み込み
    const char* path_student = "./student";
    dp = opendir(path_student);
    if (dp == NULL) return 1;
    while ((entry = readdir(dp)) != NULL) {
        string filename;
        filename = (string)entry->d_name;
        if (filename[0] == '.') continue;
        students.push_back(fileio.InputStudentData(filename, teachers));
    }
    
    // 出力用ファイル
    ofstream lp("./lp/netz.lp");
    fileio.OutputString(lp, "minimize");
    // 先生たちの指導回数の平均化（目的関数）
    convert_machine.GenerateTeachAverageingFunction(lp);
    fileio.OutputString(lp, "\nsubject to");
    // 先生の生徒に対する割り当て可能性
    convert_machine.GenerateTeacherAssignFomula(lp, students, teachers);
    // 生徒の先生に対する割り当て可能性
    convert_machine.GenerateStudentAssignFomula(lp, students, teachers);
    // 生徒のコマ数の制限
    convert_machine.GenerateComaFomula(lp, students, teachers);
    // 高校生に対するコマの制限
    convert_machine.GenerateHSFomula(lp, students, teachers);
    // 先生の指導回数の平均化（制約）
    convert_machine.GenerateTeachAverageingFomula(lp, students, teachers);
    // 01変数であることを宣言
    fileio.OutputString(lp, "\nbinary");
    convert_machine.GenerateBinaryVariable(lp, students, teachers);
    fileio.OutputString(lp, "\ngeneral");
    fileio.OutputString(lp, "y_M y_m");
    fileio.OutputString(lp, "\nend");
    
    // ソルバーで問題を解く
    system("./solver/glpsol --cpxlp ./lp/netz.lp -o ./sol/netz.sol");
    
    
    
    return 0;
}