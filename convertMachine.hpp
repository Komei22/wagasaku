// convertMachine.hpp

#ifndef __CONVERTMACHINE_HPP
#define __CONVERTMACHINE_HPP

#include <string>
#include <fstream>

#include "teacher.hpp"
#include "student.hpp"

using namespace std;

typedef vector<STUDENT> STUDENTS;

typedef vector<TEACHER> TEACHERS;

typedef vector<string> DEVIDE_PIRIOD;

class ConvertMachine {
public:
	// 講習期間
	vector<string> piriod;
    
    // 分割した講習期間のリスト
    vector<DEVIDE_PIRIOD> devide_piriod_list;
    
    // 講習期間の分割
    void DevideDay();
    
    // 分割期間中の科目のコマ数に分割
    void DevideSubject(STUDENTS, TEACHERS);
    
    // コマをphase毎に分配する
    void DistributeComaForFhase(STUDENT, TEACHERS, vector<int>);
    
    // 分割期間に閾値数分のコマを割り当て、空きコマが足りない場合できるだけ割り当てる
    void AssignComaInFhase(STUDENT&, TEACHERS, int, int, vector<int>&, vector<int>);
    
    // 先生たちの指導回数の平均化目的関数
    void GenerateTeachAverageingFunction(ofstream&);
    
    // 講師の割り当て可能性の制約式
	void GenerateTeacherAssignFomula(ofstream&,STUDENTS, TEACHERS);
    
    // 生徒の割り当て可能性の制約式
    void GenerateStudentAssignFomula(ofstream&,STUDENTS, TEACHERS);
    
    // 生徒のコマ数の制約式
    void GenerateComaFomula(ofstream& ,STUDENTS, TEACHERS);
    
    // 高校生のコマに対する制約式
    void GenerateHSFomula(ofstream& ,STUDENTS, TEACHERS);
    
    // 先生たちの指導回数の平均化制約
    void GenerateTeachAverageingFomula(ofstream&,STUDENTS, TEACHERS);
    
    // 変数の01変数宣言
    void GenerateBinaryVariable(ofstream& ,STUDENTS, TEACHERS);

};

#endif //__CONVERTMACHINE_HPP