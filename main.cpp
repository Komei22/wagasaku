// main.cpp

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include "fileio.hpp"
#include "convertMachine.hpp"

using namespace std;

FILEIO filein;

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
        teachers.push_back(filein.InputTeacherData(filename, convert_machine));
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
        students.push_back(filein.InputStudentData(filename, teachers));
    }
    
    // 出力用ファイル
    ofstream lp("./lp/netz.lp");
    // 先生の生徒に対する割り当て可能性
    convert_machine.GenerateTeacherAssignFomula(lp, students, teachers);
    // 生徒の先生に対する割り当て可能性
    convert_machine.GenerateStudentAssignFomula(lp, students, teachers);
    // 高校生に対するコマの制限
    convert_machine.GenerateHSFomula(lp, students, teachers);
    
    return 0;
}