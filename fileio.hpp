// fileio.hpp

#ifndef __FILEIO_HPP
#define __FILEIO_HPP

#include <vector>
#include <fstream>

#include "student.hpp"
#include "teacher.hpp"
#include "convertMachine.hpp"
#include "repairVeiw.hpp"
#include "solution.hpp"

typedef int INPUT_STATE;
const INPUT_STATE FIRST_STATE = -1;
const INPUT_STATE IS_NAME = 0;
const INPUT_STATE IS_SUBJECT = 1;
const INPUT_STATE IS_SCHEDULE = 2;

const INPUT_STATE WAIT = 3;
const INPUT_STATE READ_VARIABLE = 4;
const INPUT_STATE FINISH = 5;

typedef int READ_VARIABLE_STATE;
const READ_VARIABLE_STATE WAIT_VARIABLE = 6;
const READ_VARIABLE_STATE READING = 7;
const READ_VARIABLE_STATE CHECKING = 8;

typedef vector<string> SUBJECT_LIST;

class FILEIO {
public:
    SUBJECT_LIST subject_list;
    
    // 講師情報の読み込み
    TEACHER InputTeacherData(string, ConvertMachine &, bool);
    
	// 生徒情報の読み込み
	STUDENT InputStudentData(string, vector<TEACHER>);
    
    // 教科名のリスト作成
    void CreateSubjectList();
    
    // 入力のチェック 違反1 正常0
    int CheckInput(STUDENTS, TEACHERS);
    
    // 文字列の出力 空白を渡すと改行
    void OutputString(ofstream &, string);
    
    // 変数のバイナリ宣言出力
    void OutputBinaryVariable(ofstream&, string);
    
    // 整数変数の出力
    void OutputObjectiveVariable(ofstream&, int);
    
    // 制約式の出力
    void OutputJuniorVariable(ofstream &, string, int, int, int, int, int);
    
    void OutputHighVariable(ofstream&, string, int, int, int, int, int, int);
    
    // 結果の変数の読み込み
    void InputSOLfile(Solution &, ifstream&);
    
    // 中学生の変数解析
    void JuniorVariableAnalysis(Solution &, string);
    
    // 高校生の変数解析
    void HighVariableAnalysis(Solution &, string);

};


#endif // __FILEIO_HPP