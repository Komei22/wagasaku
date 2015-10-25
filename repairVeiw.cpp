// repairVeiw.cpp
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "repairVeiw.hpp"

using namespace boost;

void RepairVeiw::DecodeSchedule(STUDENTS students, TEACHERS teachers, ConvertMachine convert_machine) {
    ofstream csv;
    // 生徒のスケジュールの復元
    BOOST_FOREACH(STUDENT student, students) {
        csv.open("./schedule/student/" + student.name + ".csv");
        csv << "aaa" << endl;
        BOOST_FOREACH(TEACHER teacher, teachers) {
            for (int day = 0; day < convert_machine.piriod.size(); day++) {
                for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                    for (int subject = 0; subject < student.subject.size(); subject++) {
                        
                    }
                }
            }
        }
        csv.close();
    }
    
    // 講師のスケジュールの復元
    
    
}



