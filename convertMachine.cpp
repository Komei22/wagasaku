// convertMachine.cpp
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "convertMachine.hpp"
#include "fileio.hpp"

using namespace std;
using namespace boost;

FILEIO fileout;

void ConvertMachine::GenerateTeachAverageingFunction(ofstream& lp) {
    fileout.OutputString(lp, "y_M - y_m");
}

void ConvertMachine::GenerateTeacherAssignFomula(ofstream& lp, STUDENTS students, TEACHERS teachers) {
    // ある講師のあるコマに生徒の科目を割り当てられるかどうか
    BOOST_FOREACH(TEACHER teacher, teachers) {
        for (int day = 0; day < piriod.size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                bool isOutput = false;
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                    isOutput = true;
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                            isOutput = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(isOutput) fileout.OutputString(lp, "<= 1");
            }
        }
    }
}

void ConvertMachine::GenerateStudentAssignFomula(ofstream& lp, STUDENTS students, TEACHERS teachers) {
    // ある講師のあるコマに生徒の科目を割り当てられるかどうか
    BOOST_FOREACH(STUDENT student, students) {
        for (int day = 0; day < piriod.size(); day++) {
            for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                bool isOutput = false;
                BOOST_FOREACH(TEACHER teacher, teachers) {
                    for (int subject = 0; subject < student.subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                    isOutput = true;
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                            isOutput = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(isOutput) fileout.OutputString(lp, "<= 1");
            }
        }
    }
}

void ConvertMachine::GenerateComaFomula(ofstream& lp,STUDENTS students, TEACHERS teachers) {
    BOOST_FOREACH(STUDENT student, students) {
        for (int subject = 0; subject < student.subject.size(); subject++) {
            bool isOutput = false;
            BOOST_FOREACH(TEACHER teacher, teachers) {
                for (int day = 0; day < piriod.size(); day++) {
                    for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                    isOutput = true;
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                            isOutput = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(isOutput) {
                string subject_num = lexical_cast<string>(student.subject[subject]);
                fileout.OutputString(lp, "= " + subject_num);
            }
        }
        
    }
    
    
}


void ConvertMachine::GenerateHSFomula(ofstream& lp, STUDENTS students, TEACHERS teachers) {
    // 高校生は2コマで1コマとする制約式
    BOOST_FOREACH(STUDENT student, students) {
        if (student.grade == HIGHT) {
            BOOST_FOREACH(TEACHER teacher, teachers) {
                for (int subject = 0; subject < student.subject.size(); subject++) {
                    for (int day = 0; day < piriod.size(); day++) {
                        for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                            if (student.schedule[day][coma] == 1 && coma != student.schedule[day].size()-1) {  //最終コマでなく、生徒のスケジュールが空いていた場合
                                if (student.schedule[day][coma+1] == 1) {
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                    fileout.OutputVariable(lp, "-", teacher.id, student.id, subject, day, coma+1);
                                    fileout.OutputString(lp, ">= 0");
                                    fileout.OutputVariable(lp, "-", teacher.id, student.id, subject, day, coma);
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma+1);
                                    fileout.OutputString(lp, ">= 0");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ConvertMachine::GenerateTeachAverageingFomula(ofstream& lp,STUDENTS students, TEACHERS teachers) {
    BOOST_FOREACH(TEACHER teacher, teachers) {
        for (int day = 0; day < piriod.size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        fileout.OutputString(lp, "- y_M <= 0");
    }
    BOOST_FOREACH(TEACHER teacher, teachers) {
        for (int day = 0; day < piriod.size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        fileout.OutputString(lp, "- y_m >= 0");
    }
}

void ConvertMachine::GenerateBinaryVariable(ofstream& lp, STUDENTS students, TEACHERS teachers) {
    BOOST_FOREACH(TEACHER teacher, teachers) {
        for (int day = 0; day < piriod.size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.subject[subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                    //x_teacherid_studentid_subject_day_comaを生成
                                    fileout.OutputVariable(lp, "", teacher.id, student.id, subject, day, coma);
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputVariable(lp, "", teacher.id, student.id, subject, day, coma);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    BOOST_FOREACH(STUDENT student, students) {
        if (student.grade == HIGHT) {
            BOOST_FOREACH(TEACHER teacher, teachers) {
                for (int subject = 0; subject < student.subject.size(); subject++) {
                    for (int day = 0; day < piriod.size(); day++) {
                        for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                            if (student.schedule[day][coma] == 1) {
                                //x_teacherid_studentid_subject_day_comaを生成
                                fileout.OutputVariable(lp, "", teacher.id, student.id, subject, day, coma);
                            }
                        }
                    }
                }
            }
        }
    }
    fileout.OutputString(lp, "");
}












