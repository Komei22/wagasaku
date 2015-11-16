// convertMachine.hpp

#ifndef __CONVERTMACHINE_HPP
#define __CONVERTMACHINE_HPP

#include <string>
#include <fstream>
#include <queue>

#include "teacher.hpp"
#include "student.hpp"

using namespace std;

typedef vector<STUDENT> STUDENTS;

typedef vector<TEACHER> TEACHERS;

typedef vector<string> DEVIDE_PIRIOD;

typedef vector<pair<int, int> > EMPTY_RATE;

typedef priority_queue<pair<int, int> > SUBJECT_PRIORITY;

typedef priority_queue<pair<int, int> > PHASE_PRIORITY;

class ConvertMachine {
public:
	// 講習期間
	DEVIDE_PIRIOD piriod;
    
    // 分割した講習期間のリスト
    vector<DEVIDE_PIRIOD> devide_piriod_list;
    
    // 講習期間の分割
    void DevideDay();
    
    // 分割期間中の科目のコマ数に分割
    void DevideSubject(STUDENTS&, TEACHERS&);
    
    // 講師のphaseごとの空きコマ数をカウント
    void CountTeacherEmptySchedule(TEACHERS&);
    
    // コマをphase毎に分配する
    void DistributeComaForFhase(STUDENT&, TEACHERS&);
    
    // 各教科毎の1phaseのコマ数の閾値を計算
    void CalculateComaThreSubject(vector<int>&, STUDENT, int);
    
    // phaseでの平均指導科目数
    int CalcurateComaThreAll(STUDENT, int);
    
    // 分割期間に閾値数分のコマを割り当て、空きコマが足りない場合できるだけ割り当てる
    int AssignComaInFhase(STUDENT&, TEACHERS&, int, int, vector<int>&, vector<int>, int);
    
    void AssignRemainingComa(STUDENT&, TEACHERS&, EMPTY_RATE&, vector<int>, int);
    
    void UpdatePhaseEmptyRate(PHASE_PRIORITY&, EMPTY_RATE);
    
    // あるphaseのスタートは全体でどこのインデックスかに変換
    int PhaseToWholeDayIndex(int);
    
    // 科目の優先度の更新
    void UpdateSubjectPriority(SUBJECT_PRIORITY& , vector<int>, int);
    
    // 余りのコマがあるか
    int CheckRemainigComa(vector<int>);
    
    // 問題の出力
    void GenerateLPProbrem(ofstream&, STUDENTS, TEACHERS);
    
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