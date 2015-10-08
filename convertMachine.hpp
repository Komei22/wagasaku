// convertMachine.hpp

#ifndef __CONVERTMACHINE_HPP
#define __CONVERTMACHINE_HPP

#include <string>

#include "teacher.hpp"
#include "student.hpp"

using namespace std;

typedef vector<STUDENT> STUDENTS;

typedef vector<TEACHER> TEACHERS;

class ConvertMachine {
public:
	//講習期間
	vector<string> piriod;

	void GenerateTeacherAssignVariable(STUDENTS, TEACHERS);

};

#endif //__CONVERTMACHINE_HPP