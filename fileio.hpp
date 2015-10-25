// fileio.hpp

#ifndef __FILEIO_HPP
#define __FILEIO_HPP

#include "student.hpp"
#include "teacher.hpp"
#include "convertMachine.hpp"
#include "repairVeiw.hpp"

typedef int INPUT_STATE;
const INPUT_STATE FIRST_STATE = -1;
const INPUT_STATE IS_NAME = 0;
const INPUT_STATE IS_SUBJECT = 1;
const INPUT_STATE IS_SCHEDULE = 2;

const INPUT_STATE WAIT = 3;
const INPUT_STATE READ_VARIABLE = 4;
const INPUT_STATE FINISH = 5;

class FILEIO {
public:
    // 講師情報の読み込み
    TEACHER InputTeacherData(string, ConvertMachine &);
    
	// 生徒情報の読み込み
	STUDENT InputStudentData(string, vector<TEACHER>);
    
    // 文字列の出力 空白を渡すと改行
    void OutputString(ofstream &, string);
    
    // 制約式の出力
    void OutputVariable(ofstream &, string, int, int, int, int, int);
    
    // 結果の変数の読み込み
    void InputSOLfile(RepairVeiw &);
    
    // 変数解析
    void VariableAnalysis(RepairVeiw &, string);
};


#endif // __FILEIO_HPP