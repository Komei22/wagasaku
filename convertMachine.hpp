// convertMachine.hpp

#ifndef __CONVERTMACHINE_HPP
#define __CONVERTMACHINE_HPP

#include <string>
#include <fstream>
#include <queue>
#include <set>

#include "teacher.hpp"
#include "student.hpp"

using namespace std;

typedef vector<STUDENT> STUDENTS;

typedef vector<TEACHER> TEACHERS;

typedef vector<string> DEVIDE_PIRIOD;

typedef vector<pair<int, int> > EMPTY_RATE;

typedef priority_queue<pair<int, int> > SUBJECT_PRIORITY;

typedef priority_queue<pair<int, int> > PHASE_PRIORITY;

typedef pair<int, int> STUDENT_PRIORITY;
typedef priority_queue<STUDENT_PRIORITY> STUDENT_PRIORITY_SET;

typedef priority_queue<pair<int, int> > TEACHER_PRIORITY;

typedef priority_queue<pair<int, int> > SUBJECT_PRIORITY;

class ConvertMachine {
public:
	// 講習期間
	DEVIDE_PIRIOD piriod;
    
    // 分割した講習期間のリスト
    vector<DEVIDE_PIRIOD> devide_piriod_list;
    
    // 書き込んだ制約の変数を保管 01変数
    set<string> writed_binary_variable;
    
    // 書き込んだ制約の変数を保管 整数
    set<string> writed_integer_variable;
    
    // 分割する必要がなかった場合、情報をセットする
    void SetConvertInfomation(STUDENTS&);
    
    // 講習期間の分割
    void DevideDay();
    
    // 分割期間中の科目のコマ数に分割
    void DevideSubject(STUDENTS&, TEACHERS&);
    
    // 仮想のスケジュールの作成
    void CreateVirtualSchedule(TEACHERS&, STUDENTS&);
    
    // 生徒の優先度を設定
    void SetStudentPriority(STUDENT_PRIORITY_SET&, STUDENTS);
    
    // コマをphase毎に分配する
    void DistributeComaForFhase(STUDENT&, TEACHERS&);
    
    // 各教科毎の1phaseのコマ数の閾値を計算
    void CalculateComaThreSubject(vector<int>&, STUDENT, int);
    
    // phaseでの平均指導科目数
    int CalcurateComaThreAll(STUDENT, int);
    
    // 分割期間に閾値数分のコマを割り当て、空きコマが足りない場合できるだけ割り当てる 中学生用
    int AssignComaInFhaseJunior(STUDENT&, TEACHERS&, int, int, vector<int>&, vector<int>, int);
    
    //　分割期間に閾値数分のコマを割り当て、空きコマが足りない場合できるだけ割り当てる 高校生用
    int AssignComaInFhaseHigh(STUDENT&, TEACHERS&, int, int, vector<int>&, vector<int>, int);
    
    void SetSubjectPriority(SUBJECT_PRIORITY&, vector<int>, STUDENT, TEACHERS);

    void AssignRemainingComaJunior(STUDENT&, TEACHERS&, EMPTY_RATE&, vector<int>, int);
    
    void AssignRemainingComaHigh(STUDENT&, TEACHERS&, EMPTY_RATE&, vector<int>, int);
    
    void UpdatePhaseEmptyRate(PHASE_PRIORITY&, EMPTY_RATE);
    
    // あるphaseのスタートは全体でどこのインデックスかに変換
    int PhaseToWholeDayIndex(int);
    
    // 科目の優先度の更新
    void UpdateSubjectPriority(SUBJECT_PRIORITY& , vector<int>, int);
    
    // 余りのコマがあるか
    int CheckRemainigComa(vector<int>);
    
    // 問題の出力
    void GenerateLPProbrem(ofstream&, STUDENTS, TEACHERS, int);
    
    void SaveWritedJuniorVariable(int, int, int, int, int);
    
    void SaveWritedHighVariable(int, int, int, int, int, int);
    
    void SaveWritedIntegerVariable(int);
    
    // 先生たちの指導回数の平均化目的関数
    void GenerateObjectiveFunction(ofstream&, STUDENTS, TEACHERS);
    
    // 講師の割り当て可能性の制約式
	void GenerateTeacherAssignFomula(ofstream&,STUDENTS, TEACHERS, int);
    
    // 生徒の割り当て可能性の制約式
    void GenerateStudentAssignFomula(ofstream&,STUDENTS, TEACHERS, int);
    
    // 生徒のコマ数の制約式
    void GenerateComaFomula(ofstream& ,STUDENTS, TEACHERS, int);
    
    // 高校生のコマに対する制約式
    void GenerateHSFomula(ofstream& ,STUDENTS, TEACHERS, int);
    
    // 先生たちの指導回数の平均化制約
    void GenerateTeachAverageingFomula(ofstream&,STUDENTS, TEACHERS, int);
    
    // 生徒の1日のコマの平均化
    void GenerateAveragingComaNumFomula(ofstream&, STUDENTS, TEACHERS, int);
    
    // 変数の01変数宣言
    void GenerateBinaryVariable(ofstream&);
    
    // 変数の整数宣言
    void GenerateIntegerVariable(ofstream&);
    
    // 変換コマンドの実行
    void ExecuteConvertCommand();

};

#endif //__CONVERTMACHINE_HPP