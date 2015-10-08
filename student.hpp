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
const GRADE HIGHT = 0;
const GRADE JUNIOR = 1;

// 生徒名
typedef string STUDENT_NAME;

// 各科目のコマ数  
typedef vector<int> SUBJECT_CLASS_NUM;

// スケジュール vecter[コマ][日にち]
typedef vector< vector<int> > SCHEDULE;

// 科目ごとの指定講師 vector[教科][教師名]
typedef vector< vector<string> > TEACHER_NOMINATION;

// 科目ごとの講師指定を
typedef vector< vector<int> > NOMINATION_TEACHER_ID;

class STUDENT {
public:
	STUDENT_NAME name;

	int id;

	GRADE grade;

	SUBJECT_CLASS_NUM subject;

	SCHEDULE schedule;

	TEACHER_NOMINATION teacher_nomination;

	NOMINATION_TEACHER_ID nomination_teacher_id;

};

#endif //__STUDENT_HPP
