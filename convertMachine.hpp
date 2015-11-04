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
	//講習期間
	vector<string> piriod;
    
    vector<DEVIDE_PIRIOD> devide_piriod_list;
    
    
    //講習期間,期間中の教科の数の分割 返り値:講習何日
    void DevideDay();
    
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