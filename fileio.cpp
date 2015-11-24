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

TEACHER FILEIO::InputTeacherData(string filename, ConvertMachine& convert_machine, bool is_first_teacher) {
    const string inFile = "./data/teacher/"+filename;
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
        int column = 0;
        typedef tokenizer<escaped_list_separator<char> > ESC_TOKENIZER;
        ESC_TOKENIZER tokens(line);
        for(ESC_TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
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
                    if(*token_iter == "") continue;
                    teacher.name = *token_iter;
                    ++token_iter;
                    teacher.id = lexical_cast<int>(*token_iter);
                    break;
                case IS_SUBJECT:
                    if(*token_iter == "") continue;
                    ++token_iter;
                    teacher.teach_subject.push_back(lexical_cast<int>(*token_iter));
                    break;
                case IS_SCHEDULE:
                    column++;
                    if(*token_iter == "コマ") goto LINE_SKIP;
                    if(is_first_teacher && column == 1) convert_machine.piriod.push_back(*token_iter);
                    if(column > 1) {
                        if (*token_iter == "") {
                            schedule_per_day.push_back(0);
                        } else {
                            schedule_per_day.push_back(1);
                        }
                    }
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
    
    // デバッグ用
//    	 cout << "講師名" << endl;
//    	 cout << teacher.name << endl;
//    	 cout << "指導科目" << endl;
//    	 BOOST_FOREACH(int val, teacher.teach_subject) {
//    	 	cout << val;
//    	 }cout << endl;
//    	 cout << "スケジュール" << endl;
//    	 BOOST_FOREACH(vector<int> v, teacher.schedule) {
//    	 	BOOST_FOREACH(int val, v) {
//    	 		cout << val;
//    	 	}
//    	 }cout << endl;
//        // デバッグ用 convert_machine class
//         cout << "講習期間" << endl;
//         BOOST_FOREACH(string str1, convert_machine.piriod) {
//         	cout << str1;
//         }cout << endl;
    return teacher;
}

STUDENT FILEIO::InputStudentData(string filename, vector<TEACHER> teachers) {
    ifstream ifs("./data/student/"+filename);
    if(!ifs) {
        cout << "Error:Input data file not found" << endl;
        exit(1);
    }
    
    STUDENT student;
    string line;
    vector<int> schedule_per_day;
    INPUT_STATE state = FIRST_STATE;
    while(getline(ifs, line)) {
        int column = 0;
        typedef tokenizer<escaped_list_separator<char> > ESC_TOKENIZER;
        ESC_TOKENIZER tokens(line);
        for(ESC_TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
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
                if(*token_iter == "") continue;
                student.name = *token_iter;
                ++token_iter;
                if(*token_iter == "高校生") {
                    student.grade = HIGH;
                } else {
                    student.grade = JUNIOR;
                }
                ++token_iter;
                student.id = lexical_cast<int>(*token_iter);
                break;
            } else if(state == IS_SUBJECT) {
                if(*token_iter == "") continue;
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
                }
                student.nomination_teacher_id.push_back(nominate_teacher);
            } else if(state == IS_SCHEDULE) {
                column++;
                if(*token_iter == "コマ") goto LINE_SKIP;
                if(column > 1) {
                    if (*token_iter == "") {
                        schedule_per_day.push_back(0);
                    } else {
                        schedule_per_day.push_back(1);
                    }
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
//             cout << "生徒名" << endl;
//             cout << student.name << student.grade << endl;
//             cout << "科目" << endl;
//             BOOST_FOREACH(int val, student.subject) {
//             	cout << val;
//             }cout << endl;
//             cout << "担当講師" << endl;
//             BOOST_FOREACH(vector<int> v, student.nomination_teacher_id) {
//             	BOOST_FOREACH(int id, v){
//             		cout << id;
//             	}cout << endl;
//             }
//             cout << "スケジュール" << endl;
//             BOOST_FOREACH(vector<int> v, student.schedule) {
//             	BOOST_FOREACH(int val, v) {
//             		cout << val;
//             	}
//             }cout << endl;
    return student;
}


void FILEIO::CreateSubjectList() {
    subject_list.push_back("国語");
    subject_list.push_back("数学");
    subject_list.push_back("英語");
    subject_list.push_back("理科");
    subject_list.push_back("社会");
}

int FILEIO::CheckInput(STUDENTS students, TEACHERS teachers) {
    CreateSubjectList();
    // 生徒のとっている科目に対して指導する先生がいない
    bool is_teaching_teacher = false;
    int subject_num = teachers[0].teach_subject.size();
    for (int subject = 0; subject < subject_num; subject++) {
        BOOST_FOREACH(STUDENT student, students) {
            BOOST_FOREACH(TEACHER teacher, teachers) {
                if (student.subject[subject] == 0) is_teaching_teacher = true;
                if (student.subject[subject] > 0 && teacher.teach_subject[subject] == 1) { //ある生徒のある科目に対して、教える講師がいるなら
                    is_teaching_teacher = true;
                }
            }
            if(is_teaching_teacher == false) {
                cout << format("%sの%sを指導する講師がいません") % student.name % subject_list[subject] << endl;
                return 1;
            }
        }
    }
    return 0;
}



void FILEIO::OutputString(ofstream& lp, string str) {
    lp << format("%s") % str << endl;
}

void FILEIO::OutputBinaryVariable(ofstream& lp, string str) {
    lp << format("%s ") % str;
}


void FILEIO::OutputVariable(ofstream& lp, string oper, int teacher, int student, int subject, int day, int coma) {
    lp << format("%s x_%s_%s_%s_%s_%s ") %oper % teacher % student % subject % day % coma;
}


void FILEIO::InputSOLfile(RepairVeiw& repair_veiw, ifstream& ifs) {
    string line;
    int input_state = FIRST_STATE;
    int read_state = WAIT_VARIABLE;
    string variable;
    while(getline(ifs, line)) {
        typedef char_separator<char> CHAR_SEP;
        typedef tokenizer<CHAR_SEP> TOKENIZER;
        CHAR_SEP char_sep(" ", "", drop_empty_tokens);
        TOKENIZER tokens(line, char_sep);
        for(TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
            if(input_state == FIRST_STATE && *token_iter == "------") {
                input_state = WAIT;
            } else if(input_state == WAIT && *token_iter == "------") {
                input_state = READ_VARIABLE;
                goto NEXT_LINE;
            } else if(*token_iter == "Integer") {
                input_state = FINISH;
            }
            if(input_state == READ_VARIABLE) {
                if(read_state == WAIT_VARIABLE) {
                    variable.assign(*token_iter);
                    if (variable[0] == 'x') read_state = CHECKING;
                } else if(read_state == CHECKING) {
                    if(*token_iter == "*") { //*が来たら次に0か1が来る。1が来たら読み込みの状態に移行
                        ++token_iter;
                        if(*token_iter == "1") {
                            read_state = READING;
                        } else {
                            read_state = WAIT_VARIABLE;
                        }
                    }
                } else if(read_state == READING) {
                    VariableAnalysis(repair_veiw, variable);
                    read_state = WAIT_VARIABLE;
                }
            }
        }
    NEXT_LINE:;
    }
}


void FILEIO::VariableAnalysis(RepairVeiw& repair_view,string variable) {
    typedef char_separator<char> CHAR_SEP;
    typedef tokenizer<CHAR_SEP> TOKENIZER;
    CHAR_SEP char_sep("_", "", drop_empty_tokens);
    TOKENIZER tokens(variable, char_sep);
    vector<int> assign_inf;
    for(TOKENIZER::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter) {
        ++token_iter;
        assign_inf.push_back(lexical_cast<int>(*token_iter));
        ++token_iter;
        assign_inf.push_back(lexical_cast<int>(*token_iter));
        ++token_iter;
        assign_inf.push_back(lexical_cast<int>(*token_iter));
        ++token_iter;
        assign_inf.push_back(lexical_cast<int>(*token_iter));
        ++token_iter;
        assign_inf.push_back(lexical_cast<int>(*token_iter));
    }
    repair_view.assign.push_back(assign_inf);
}


