// main.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <dirent.h>
#include <boost/foreach.hpp>
#include "fileio.hpp"
#include "convertMachine.hpp"
#include "repairVeiw.hpp"

using namespace std;

using namespace boost;

FILEIO fileio;

ConvertMachine convert_machine;

int main() {
    // teacherオブジェクト生成
    vector<TEACHER> teachers;
    // 講師情報の読み込み
    const char* path_teacher = "./data/teacher";
    DIR *dp;       // ディレクトリへのポインタ
    dirent* entry; // readdir() で返されるエントリーポイント
    dp = opendir(path_teacher);
    if (dp == NULL) return 1;
    bool is_first_teacher = true;
    while ((entry = readdir(dp)) != NULL) {
        string filename;
        filename = (string)entry->d_name;
        if (filename[0] == '.') continue;
        teachers.push_back(fileio.InputTeacherData(filename, convert_machine, is_first_teacher));
        is_first_teacher = false;
    }
    
    // studentオブジェクトを生成
    vector<STUDENT> students;
    // 生徒情報の読み込み
    const char* path_student = "./data/student";
    dp = opendir(path_student);
    if (dp == NULL) return 1;
    while ((entry = readdir(dp)) != NULL) {
        string filename;
        filename = (string)entry->d_name;
        if (filename[0] == '.') continue;
        students.push_back(fileio.InputStudentData(filename, teachers));
    }
    
    // 入力ファイルのチェック
    if(fileio.CheckInput(students, teachers) == 1) return 1;
    
    // 入力ファイルの分割
    convert_machine.DevideDay();
    
    convert_machine.DevideSubject(students, teachers);
    
    // 出力用ファイル
    ofstream lp;
    vector<string> file_path_set;
    for (int phase_idx = 0; phase_idx < convert_machine.devide_piriod_list.size(); phase_idx++) {
        ostringstream oss;
        oss << "./lp/netz" << phase_idx << ".lp";
        string file_path = oss.str();
        file_path_set.push_back(file_path);
        lp.open(file_path);
        // 各ファイルに分割された問題を記述
        convert_machine.GenerateLPProbrem(lp, students, teachers, phase_idx);
        lp.close();
    }
    
    // ソルバーで問題を解く
//    system("./solver/glpsol --cpxlp ./lp/netz.lp -o ./sol/netz.sol");
    
//    vector<RepairVeiw> repair_veiws;
//    repair_veiws.resize(convert_machine.devide_piriod_list.size());
//    for (int phase_idx = 0; phase_idx < convert_machine.devide_piriod_list.size(); phase_idx++) {
//        ostringstream oss;
//        oss << "./sol/netz" << phase_idx << ".sol";
//        string file_path = oss.str();
//        ifstream ifs;
//        ifs.open(file_path);
//        if(!ifs) {
//            cout << "Error:Input data file not found :: sol" << endl;
//            exit(1);
//        }
//        RepairVeiw repair_veiw;
//        fileio.InputSOLfile(repair_veiw, ifs); //InputSOLfileがrepair_veiwを返すようにする
//        repair_veiws.push_back(repair_veiw);
//        ifs.close();
//    }
    
    RepairVeiw repair_veiw;
    ifstream ifs;
    ifs.open("./sol/netz0.sol");
    fileio.InputSOLfile(repair_veiw, ifs);
    
    // 解の読み込み
//    fileio.InputSOLfile(repair_veiw);
    //解の解析
    repair_veiw.DecodeSchedule(students, teachers, convert_machine);
    
    return 0;
}