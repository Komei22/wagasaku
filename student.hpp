// student.hpp

#ifndef __STUDENT_HPP
#define __STUDENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// 科目
typedef int SUBJECT;
const SUBJECT LANGUAGE = 0;
const SUBJECT MATH = 1;
const SUBJECT ENGLISH = 2;
const SUBJECT SIENCE = 3;
const SUBJECT SOCIETY = 4;

// 学年
typedef int GRADE;
const GRADE HIGH = 0;
const GRADE JUNIOR = 1;

// 生徒名
typedef string STUDENT_NAME;

// 各科目のコマ数  
typedef vector<int> SUBJECT_CLASS_NUM;

// スケジュール vecter[コマ][日にち]
typedef vector< vector<int> > SCHEDULE;

// 科目ごとの講師指定
typedef vector< vector<int> > NOMINATION_TEACHER_ID;

class STUDENT {    
public:
	STUDENT_NAME name;

	int id;

	GRADE grade;

	SUBJECT_CLASS_NUM subject;
    
    // 生徒のphase毎の指導科目別コマ数
    vector<SUBJECT_CLASS_NUM> coma_of_subject_phase;

	SCHEDULE schedule;

	NOMINATION_TEACHER_ID nomination_teacher_id;

};

#endif //__STUDENT_HPP
