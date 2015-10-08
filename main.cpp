// main.cpp

#include <iostream>
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
    
    


    
    // 先生の生徒に対する割り当て可能性
    convert_machine.GenerateTeacherAssignVariable(students, teachers);
    
    return 0;
}