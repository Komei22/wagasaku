// repairVeiw.cpp
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "repairVeiw.hpp"
#include "fileio.hpp"

using namespace boost;

FILEIO csvout;

void RepairVeiw::InitializeSubjectName() {
    subject_name.push_back("国語");
    subject_name.push_back("数学");
    subject_name.push_back("英語");
    subject_name.push_back("理科");
    subject_name.push_back("社会");
}


void RepairVeiw::InitializeStudentSchedule(DECODE_SCHEDULE& schedule, int coma_num, int piriod_num, STUDENT student) {
    schedule.resize(piriod_num);
    for (int idx = 0; idx < piriod_num; idx++) {
        schedule[idx].resize(coma_num);
    }
    for (int day = 0; day < piriod_num; day++) {
        for (int coma = 0; coma < coma_num; coma++) {
            if (student.schedule[day][coma] == 1) {
                schedule[day][coma] = '1';
            }
        }
    }
}


void RepairVeiw::InitializeTeacherSchedule(DECODE_SCHEDULE& schedule, int coma_num, int piriod_num, TEACHER teacher) {
    schedule.resize(piriod_num);
    for (int idx = 0; idx < piriod_num; idx++) {
        schedule[idx].resize(coma_num);
    }
    for (int day = 0; day < piriod_num; day++) {
        for (int coma = 0; coma < coma_num; coma++) {
            if (teacher.schedule[day][coma] == 1) {
                schedule[day][coma] = '1';
            }
        }
    }
}

void RepairVeiw::InitializeStudentCSV(STUDENTS students, int coma_num) {
    BOOST_FOREACH(STUDENT student, students) {
        ofstream csv("./schedule/student/" + student.name + ".csv", ios::out);
        OutputHead(csv, student.name, coma_num);
        csv.close();
    }
}


void RepairVeiw::InitializeTeacherCSV(TEACHERS teachers, int coma_num) {
    BOOST_FOREACH(TEACHER teacher, teachers) {
        ofstream csv("./schedule/teacher/" + teacher.name + ".csv", ios::out);
        OutputHead(csv, teacher.name, coma_num);
        csv.close();
    }
}


void RepairVeiw::OutputHead(ofstream& csv, string name, int coma_num) {
    csvout.OutputString(csv, name);
    csvout.OutputString(csv, "");
    csv << "スケジュール,";
    for (int idx = 0; idx < coma_num; idx++) {
        csv << format("%s,") % (idx+1);
    }csv << endl;
}


void RepairVeiw::DecodeSchedule(STUDENTS students, TEACHERS teachers, ConvertMachine convert_machine, Solutions solutions) {
    InitializeSubjectName();
    ofstream csv;
    
    // スケジュール保管用の初期化
    int coma_num = students[0].schedule[0].size();
    int piriod_num = convert_machine.piriod.size();
    DECODE_SCHEDULE schedule;
    
    // 生徒のスケジュールの復元
    BOOST_FOREACH(STUDENT student, students) {
        InitializeStudentSchedule(schedule, coma_num, piriod_num, student);
        csv.open("./schedule/student/" + student.name + ".csv", ios::app);
//        OutputHead(csv, student.name, coma_num);
        for (int phase = 0; phase < convert_machine.devide_piriod_list.size(); phase++) {
            int phase_start = convert_machine.PhaseToWholeDayIndex(phase);
            for (int day = phase_start; day < phase_start + convert_machine.devide_piriod_list[phase].size(); day++) {
                for (int coma = 0; coma < coma_num; coma++) {
                    BOOST_FOREACH(TEACHER teacher, teachers) {
                        DecodeStudentSchedule(schedule, teacher, student, day, coma, solutions[phase].assign);
                    }
                }
            }
        }
        for (int day = 0; day < piriod_num; day++) {
            csv << format("%s,") % convert_machine.piriod[day];
            for (int coma = 0; coma < coma_num; coma++) {
                csv << format("%s,") % schedule[day][coma];
            }
            csv << endl;
        }
        schedule.clear();
        csv.close();
    }
    
    // 講師のスケジュールの復元
    BOOST_FOREACH(TEACHER teacher, teachers) {
        InitializeTeacherSchedule(schedule, coma_num, piriod_num, teacher);
        csv.open("./schedule/teacher/" + teacher.name + ".csv", ios::app);
//        OutputHead(csv, teacher.name, coma_num);
        for (int phase = 0; phase < convert_machine.devide_piriod_list.size(); phase++) {
            int phase_start = convert_machine.PhaseToWholeDayIndex(phase);
            for (int day = phase_start; day < phase_start + convert_machine.devide_piriod_list[phase].size(); day++) {
                for (int coma = 0; coma < coma_num; coma++) {
                    BOOST_FOREACH(STUDENT student, students) {
                        DecodeTeacherSchedule(schedule, teacher, student, day, coma, solutions[phase].assign);
                    }
                }
            }
        }
        for (int day = 0; day < piriod_num; day++) {
            csv << format("%s,") % convert_machine.piriod[day];
            for (int coma = 0; coma < coma_num; coma++) {
                csv << format("%s,") % schedule[day][coma];
            }
            csv << endl;
        }
        schedule.clear();
        csv.close();
    }
}


void RepairVeiw::DecodeStudentSchedule(DECODE_SCHEDULE& schedule, TEACHER teacher, STUDENT student, int day, int coma, ASSIGN assign) {
    BOOST_FOREACH(ASSIGN_INF assign_inf, assign) {
        if (assign_inf[TEACHER_ID] == teacher.id && assign_inf[STUDENT_ID] == student.id && assign_inf[DAY] == day && assign_inf[COMA] == coma) {
            string output_str;
            output_str = subject_name[assign_inf[SUBJECT_ID]] + ":" + teacher.name;
            schedule[day][coma] = output_str;
        }
    }
}


void RepairVeiw::DecodeTeacherSchedule(DECODE_SCHEDULE& schedule, TEACHER teacher, STUDENT student, int day, int coma, ASSIGN assign) {
    BOOST_FOREACH(ASSIGN_INF assign_inf, assign) {
        if (assign_inf[TEACHER_ID] == teacher.id && assign_inf[STUDENT_ID] == student.id && assign_inf[DAY] == day && assign_inf[COMA] == coma) {
            string output_str;
            output_str = subject_name[assign_inf[SUBJECT_ID]] + ":" + student.name;
            schedule[day][coma] = output_str;
        }
    }
}


