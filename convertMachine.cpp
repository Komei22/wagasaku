// convertMachine.cpp
#include <boost/foreach.hpp>
#include "convertMachine.hpp"
#include "fileio.hpp"

using namespace std;

FILEIO fileout;

void ConvertMachine::GenerateTeacherAssignFomula(ofstream& lp, STUDENTS students, TEACHERS teachers) {
    // ある講師のあるコマに生徒の科目を割り当てられるかどうか
    fileout.OutputString(lp, "subject to");
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
                                    fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma);
                                    isOutput = true;
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma);
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
                                    fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma);
                                    isOutput = true;
                                }
                            } else {  //講師の指定があった場合
                                BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                    if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                        if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                            //x_teacherid_studentid_subject_day_comaを生成
                                            fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma);
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
                                    fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma);
                                    fileout.OutputSubjectTo(lp, "-", teacher.id, student.id, subject, day, coma+1);
                                    fileout.OutputString(lp, ">= 0");
                                    fileout.OutputSubjectTo(lp, "-", teacher.id, student.id, subject, day, coma);
                                    fileout.OutputSubjectTo(lp, "+", teacher.id, student.id, subject, day, coma+1);
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












