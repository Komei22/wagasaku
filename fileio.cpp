// fileio.cpp
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <dirent.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "fileio.hpp"

using namespace std;
using namespace boost;

TEACHER FILEIO::InputTeacherData(string filename, ConvertMachine& convert_machine) {
    const string inFile = "./teacher/"+filename;
	ifstream ifs(inFile.c_str());
	if(!ifs) {
		cout << "Error:Input data file not found" << endl;
		exit(1);
	}
	
	TEACHER teacher;
	string line;
	vector<int> schedule_per_day;
	INPUT_STATE state = FIRST_STATE;
	while(getline(ifs, line)) {
		line = line+',';
		int column = 0;
		typedef tokenizer<escaped_list_separator<char> > ESC_TOKENIZER;
		ESC_TOKENIZER tokens(line);
		for(ESC_TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
			if(*token_iter == "") continue;
			if(*token_iter == "講師名") {
				state = IS_NAME;
				break;
			} else if(*token_iter == "指導科目") {
				state = IS_SUBJECT;
				break;
			} else if(*token_iter == "スケジュール") {
				state = IS_SCHEDULE;
				goto LINE_SKIP;
			}

            switch (state) {
                case IS_NAME:
                    teacher.name = *token_iter;
                    ++token_iter;
                    teacher.id = lexical_cast<int>(*token_iter);
                    break;
                case IS_SUBJECT:
                    ++token_iter;
                    teacher.teach_subject.push_back(lexical_cast<int>(*token_iter));
                    break;
                case IS_SCHEDULE:
                    column++;
                    if(*token_iter == "コマ") goto LINE_SKIP;
                    if(teacher.id == 0 && column == 1) convert_machine.piriod.push_back(*token_iter);
                    if(column > 1) schedule_per_day.push_back(lexical_cast<int>(*token_iter));
                    break;
                default:
                    break;
            }
		}
		if(state == IS_SCHEDULE) {
			teacher.schedule.push_back(schedule_per_day);
			schedule_per_day.clear();
		}
    LINE_SKIP: ;
	}

// 	// デバッグ用
//	 cout << "講師名" << endl;
//	 cout << teacher.name << endl;
//	 cout << "指導科目" << endl;
//	 BOOST_FOREACH(int val, teacher.teach_subject) {
//	 	cout << val;
//	 }cout << endl;
//	 cout << "スケジュール" << endl;
//	 BOOST_FOREACH(vector<int> v, teacher.schedule) {
//	 	BOOST_FOREACH(int val, v) {
//	 		cout << val;
//	 	}
//	 }cout << endl;
//    // デバッグ用 convert_machine class
//     cout << "講習期間" << endl;
//     BOOST_FOREACH(string str1, convert_machine.piriod) {
//     	cout << str1;
//     }cout << endl;
	return teacher;
}

STUDENT FILEIO::InputStudentData(string filename, vector<TEACHER> teachers) {
    ifstream ifs("./student/"+filename);
    if(!ifs) {
        cout << "Error:Input data file not found" << endl;
        exit(1);
    }
    
    STUDENT student;
    string line;
    vector<int> schedule_per_day;
    INPUT_STATE state = FIRST_STATE;
    while(getline(ifs, line)) {
        line = line+',';
        int column = 0;
        typedef tokenizer<escaped_list_separator<char> > ESC_TOKENIZER;
        ESC_TOKENIZER tokens(line);
        for(ESC_TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
            if(*token_iter == "") continue;
            if(*token_iter == "生徒名") {
                state = IS_NAME;
                break;
            } else if(*token_iter == "科目") {
                state = IS_SUBJECT;
                break;
            } else if(*token_iter == "スケジュール") {
                state = IS_SCHEDULE;
                goto LINE_SKIP;
            }
            
            if(state == IS_NAME) {
                student.name = *token_iter;
                ++token_iter;
                if(*token_iter == "高校生") {
                    student.grade = HIGHT;
                } else {
                    student.grade = JUNIOR;
                }
                ++token_iter;
                student.id = lexical_cast<int>(*token_iter);
                break;
            } else if(state == IS_SUBJECT) {
                ++token_iter;
                student.subject.push_back(lexical_cast<int>(*token_iter));
                ++token_iter;
                //指定講師の読み込み
                vector<int> nominate_teacher;
                string teacher_string = *token_iter;
                typedef tokenizer<char_separator<char> > TEACHER_TOKENIZER;
                char_separator<char> TEACHER_SEP(",");
                TEACHER_TOKENIZER teacher_tokens(teacher_string, TEACHER_SEP);
                for(TEACHER_TOKENIZER::iterator teacher_token_iter = teacher_tokens.begin(); teacher_token_iter != teacher_tokens.end(); ++teacher_token_iter) {
                    BOOST_FOREACH(TEACHER teacher, teachers) {
                        if (*teacher_token_iter == teacher.name) {
                            nominate_teacher.push_back(teacher.id);
                        }
                    }
//                    nominate_teacher.push_back(*teacher_token_iter);
                }
//                student.teacher_nomination.push_back(nominate_teacher);
                student.nomination_teacher_id.push_back(nominate_teacher);
            } else if(state == IS_SCHEDULE) {
                column++;
                if(*token_iter == "コマ") goto LINE_SKIP;
                if(column > 1) {
                    schedule_per_day.push_back(lexical_cast<int>(*token_iter));
                }
            }
        }
        if(state == IS_SCHEDULE) {
            student.schedule.push_back(schedule_per_day);
            schedule_per_day.clear();
        }
        LINE_SKIP: ;
    }
    // デバッグ用 student class
//     cout << "生徒名" << endl;
//     cout << student.name << student.grade << endl;
//     cout << "科目" << endl;
//     BOOST_FOREACH(int val, student.subject) {
//     	cout << val;
//     }cout << endl;
//     cout << "担当講師" << endl;
//     BOOST_FOREACH(vector<int> v, student.nomination_teacher_id) {
//     	BOOST_FOREACH(int id, v){
//     		cout << id;
//     	}cout << endl;
//     }
//     cout << "スケジュール" << endl;
//     BOOST_FOREACH(vector<int> v, student.schedule) {
//     	BOOST_FOREACH(int val, v) {
//     		cout << val;
//     	}
//     }cout << endl;
    return student;
}


void FILEIO::OutputString(string str) {
    ofstream lp("./lp/netz.lp");
    lp << format("%s") % str << endl;
}

void FILEIO::OutputSubjectTo(int teacher, int student, int subject, int day, int coma) {
    ofstream lp("./lp/netz.lp");
    lp << format("+ x_%s_%s_%s_%s_%s ") % teacher % student % subject % day % coma;
}


	







