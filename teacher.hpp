// teacher.hpp

#ifndef __TEACHER_HPP
#define __TEACHER_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// 生徒名
typedef string TEACHER_NAME;

// 指導科目
typedef vector<int> TEACH_SUBJECT;

// スケジュール
typedef vector< vector<int> > SCHEDULE;

// phaseごとの空きコマ数
typedef vector<int> EMPTY_SCHEDULE_OF_PHASE;

class TEACHER {
public:
	TEACHER_NAME name;

	int id;

	TEACH_SUBJECT teach_subject;

	SCHEDULE schedule;
    
    SCHEDULE virtual_schedule;
    
//    EMPTY_SCHEDULE_OF_PHASE empty_of_phase;

};


#endif //__TEACHER_HPP