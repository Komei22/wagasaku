// repirVeiw.hpp
#ifndef repairVeiw_hpp
#define repairVeiw_hpp

#include <stdio.h>
#include <vector>
#include "student.hpp"
#include "teacher.hpp"
#include "convertMachine.hpp"
#include "solution.hpp"

using namespace std;

typedef int ASSIGN_INFOMATION;
const ASSIGN_INFOMATION TEACHER_ID = 0;
const ASSIGN_INFOMATION STUDENT_ID = 1;
const ASSIGN_INFOMATION SUBJECT_ID = 2;
const ASSIGN_INFOMATION DAY = 3;
const ASSIGN_INFOMATION COMA = 4;

//0:先生,1:生徒,2:科目,3:日,4:コマ
typedef vector<int> ASSIGN_INF;

typedef vector<Solution> Solutions;

typedef vector< vector<string> > DECODE_SCHEDULE;

class RepairVeiw {
public:
//    vector<ASSIGN_INF> assign;
    
    vector<string> subject_name;
    
public:
    // 科目名の初期化
    void InitializeSubjectName();
    
    // スケジュール配列の初期化
    void InitializeStudentSchedule(DECODE_SCHEDULE&, int, int, STUDENT);
    
    void InitializeTeacherSchedule(DECODE_SCHEDULE&, int, int, TEACHER);
    
    // 生徒のcsvの初期化
    void InitializeStudentCSV(STUDENTS, int);
    
    // 講師のcsvの初期化
    void InitializeTeacherCSV(TEACHERS, int);
    
    // 出力スケジュールのヘッダ
    void OutputHead(ofstream &, string, int);
    
    // 変数からスケジュールの復元
    void DecodeSchedule(STUDENTS, TEACHERS, ConvertMachine, Solutions);
    
    // 生徒のスケジュールの復元
    void DecodeStudentSchedule(DECODE_SCHEDULE &, TEACHER, STUDENT, int, int, ASSIGN);
    // 講師のスケジュールの復元
    void DecodeTeacherSchedule(DECODE_SCHEDULE &, TEACHER, STUDENT, int, int, ASSIGN);
};


#endif /* repairVeiw_hpp */
