// convertMachine.cpp
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "convertMachine.hpp"
#include "fileio.hpp"

using namespace std;
using namespace boost;

FILEIO fileout;

#define DEVIDE_SIZE 7

void ConvertMachine::SetConvertInfomation(STUDENTS& students) {
    vector<string> piriod_list;
    copy(piriod.begin(), piriod.end(), back_inserter(piriod_list));
    devide_piriod_list.push_back(piriod_list);
    
    BOOST_FOREACH(STUDENT& student, students) {
        vector<int> subject_list;
        copy(student.subject.begin(), student.subject.end(), back_inserter(subject_list));
        student.coma_of_subject_phase.push_back(subject_list);
    }
}

void ConvertMachine::DevideDay() {
    int phase_num = ceil((double)piriod.size()/DEVIDE_SIZE);
    double phase_threshold = (double)piriod.size()/(double)phase_num;
    DEVIDE_PIRIOD devide_piriod;
    int start_day = 0;
    double decimal = phase_threshold - floor(phase_threshold);
    int phase_size = floor(phase_threshold);
    for (int phase = 0; phase < phase_num-1; phase++) {
        int start_day_tmp = 0;
        for (int day = start_day; day < start_day+phase_size; day++) {
            devide_piriod.push_back(piriod[day]);
            start_day_tmp = day;
        }
        start_day = start_day_tmp+1;
        devide_piriod_list.push_back(devide_piriod);
        devide_piriod.clear();
        double sum = phase_threshold + decimal;
        phase_size = floor(sum);
        decimal = sum - (double)phase_size;
    }
    for (int day = start_day; day < piriod.size(); day++) {
        devide_piriod.push_back(piriod[day]);
    }
    devide_piriod_list.push_back(devide_piriod);
}


void ConvertMachine::DevideSubject(STUDENTS& students, TEACHERS& teachers) {
    CreateVirtualSchedule(teachers, students);
    // 生徒の優先度を設定
    STUDENT_PRIORITY_SET student_priority;
    SetStudentPriority(student_priority, students);
    
    while (!student_priority.empty()) {
        int student_idx = student_priority.top().second;
        student_priority.pop();
        STUDENT student = students[student_idx];
        DistributeComaForFhase(student, teachers);
        students[student_idx] = student;
    }
}

void ConvertMachine::CreateVirtualSchedule(TEACHERS& teachers, STUDENTS& students) {
    BOOST_FOREACH(TEACHER& teacher, teachers) {
        copy(teacher.schedule.begin(), teacher.schedule.end(), back_inserter(teacher.virtual_schedule));
    }
    BOOST_FOREACH(STUDENT& student, students) {
        copy(student.schedule.begin(), student.schedule.end(), back_inserter(student.virtual_schedule));
    }
}

void ConvertMachine::SetStudentPriority(STUDENT_PRIORITY_SET& student_priority, STUDENTS students) {
    int student_idx = 0;
    BOOST_FOREACH(STUDENT student, students) {
        int evalution = 100000;
        // スケジューリングの可能性が少ない人ほど優先度　高
        for (int day_idx = 0; day_idx < piriod.size(); day_idx++) {
            for (int coma_idx = 0; coma_idx < student.schedule[day_idx].size(); coma_idx++) {
                if (student.schedule[day_idx][coma_idx] == 1) {
                    evalution--;
                }
            }
        }
        if (student.grade == HIGH) { //高校生は優先度　高
            evalution = evalution + 1000;
        }
        student_priority.push(make_pair(evalution, student_idx));
        student_idx++;
    }
}


void ConvertMachine::DistributeComaForFhase(STUDENT& student, TEACHERS& teachers) {
    int phase_num = devide_piriod_list.size();
    int subject_num = student.subject.size();
    
    // ある生徒の各教科毎の1フェイズのコマ数の閾値を計算
    vector<int> coma_thre_of_subject(subject_num, 0);
    CalculateComaThreSubject(coma_thre_of_subject, student, phase_num);
    
    // ある生徒のphaseでの平均指導科目数
    int coma_thre_of_all = CalcurateComaThreAll(student, phase_num);
    
    // 生徒のphaseごとの科目のコマ数の初期化
    student.coma_of_subject_phase.resize(phase_num);
    for (int phase = 0; phase < phase_num; phase++) {
        student.coma_of_subject_phase[phase].assign(subject_num, 0);
    }
    
    vector<int> coma_num_of_subject;
    copy(student.subject.begin(), student.subject.end(), back_inserter(coma_num_of_subject));
    int phase_start_idx = 0;
    EMPTY_RATE empty_rate;
    // あるphaseに割り当てができるかをチェックして割り当て可能なら割り当てていく、空きコマが足りない場合は出来るだけ割り当てておく
    for (int phase = 0; phase < phase_num; phase++) {
        int empty_num = 0;
        if (student.grade == JUNIOR) {
            empty_num = AssignComaInFhaseJunior(student, teachers, phase_start_idx, phase, coma_num_of_subject, coma_thre_of_subject, coma_thre_of_all);
        } else {
            empty_num = AssignComaInFhaseHigh(student, teachers, phase_start_idx, phase, coma_num_of_subject, coma_thre_of_subject, coma_thre_of_all);
        }
        empty_rate.push_back(make_pair(empty_num, phase));
        phase_start_idx += devide_piriod_list[phase].size();
    }
    
    // 割り当てなければならないコマが残っていたとき、余ってる分を余裕のあるphaseに割り当てる
    bool is_rest = false;
    for (int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
        if(coma_num_of_subject[subject_idx] > 0) is_rest = true;
    }
    if (is_rest) {
        if (student.grade == JUNIOR) {
            AssignRemainingComaJunior(student, teachers, empty_rate, coma_num_of_subject, coma_thre_of_all);
        } else {
            AssignRemainingComaHigh(student, teachers, empty_rate, coma_num_of_subject, coma_thre_of_all);
        }
    }
    
    //    デバッグ用　phaseごとの各科目のコマ数
    cout << student.name << endl;
    BOOST_FOREACH(vector<int> subject, student.coma_of_subject_phase) {
        BOOST_FOREACH(int coma, subject) {
            cout << coma;
        }cout << endl;
    }
}


void ConvertMachine::CalculateComaThreSubject(vector<int>& coma_thre_of_subject, STUDENT student, int phase_num) {
    for (int subject_idx = 0; subject_idx < student.subject.size(); subject_idx++) {
        if (student.subject[subject_idx] != 0) {
            double subject_thre_tmp = (double)student.subject[subject_idx]/(double)phase_num;
            int subject_thre = 0;
            if (subject_thre_tmp > 1) {
                subject_thre = floor(subject_thre_tmp);
            } else {
                subject_thre = ceil(subject_thre_tmp);
            }
            coma_thre_of_subject[subject_idx] = subject_thre;
        }
    }
}


int ConvertMachine::CalcurateComaThreAll(STUDENT student, int phase_num) {
    double coma_thre_of_all = 0;
    for (int subject_idx = 0; subject_idx < student.subject.size(); subject_idx++) {
        coma_thre_of_all += student.subject[subject_idx];
    }
    coma_thre_of_all = floor(coma_thre_of_all/(double)phase_num);
    if(coma_thre_of_all == 0) coma_thre_of_all++;
    return coma_thre_of_all;
}


int ConvertMachine::AssignComaInFhaseJunior(STUDENT& student, TEACHERS& teachers, int phase_start_idx, int phase, vector<int> &coma_num_of_subject, vector<int> coma_thre_of_subject, int coma_thre_of_all) {
    int coma_num = student.schedule[0].size();
    // 科目の優先度の設定
    SUBJECT_PRIORITY subject_priority;
    SetSubjectPriority(subject_priority, coma_num_of_subject, student, teachers);
    
    // コマの割り当て
    int empty_shcedule_num = 0;
    int total_assign_num = 0;
    while (!subject_priority.empty()) {
        int subject_idx = subject_priority.top().second;
        subject_priority.pop();
        if (coma_num_of_subject[subject_idx] <= 0) continue;
        for (int day_idx = phase_start_idx; day_idx < phase_start_idx+devide_piriod_list[phase].size(); day_idx++) {
            if (!student.nomination_teacher_id[subject_idx].empty()) {
                BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                    BOOST_FOREACH(TEACHER& teacher, teachers) {
                        if (nominate_teacher_id == teacher.id) {
                            for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                                if (student.schedule[day_idx][coma_idx] ==1 && teacher.virtual_schedule[day_idx][coma_idx] == 1) {
                                    empty_shcedule_num++;
                                    if (total_assign_num == coma_thre_of_all) break;
                                    if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                        student.coma_of_subject_phase[phase][subject_idx]++;
                                        teacher.virtual_schedule[day_idx][coma_idx] = 0;
                                        total_assign_num++;
                                        empty_shcedule_num--;
                                        coma_num_of_subject[subject_idx]--;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                BOOST_FOREACH(TEACHER& teacher, teachers) {
                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                        if (student.schedule[day_idx][coma_idx] ==1 && teacher.virtual_schedule[day_idx][coma_idx] == 1) {
                            empty_shcedule_num++;
                            if (total_assign_num == coma_thre_of_all) break;
                            if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                student.coma_of_subject_phase[phase][subject_idx]++;
                                teacher.virtual_schedule[day_idx][coma_idx] = 0;
                                total_assign_num++;
                                empty_shcedule_num--;
                                coma_num_of_subject[subject_idx]--;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return empty_shcedule_num;
}


int ConvertMachine::AssignComaInFhaseHigh(STUDENT& student, TEACHERS& teachers, int phase_start_idx, int phase, vector<int> &coma_num_of_subject, vector<int> coma_thre_of_subject, int coma_thre_of_all) {
    int coma_num = student.schedule[0].size();
    // 科目の優先度の設定
    SUBJECT_PRIORITY subject_priority;
    SetSubjectPriority(subject_priority, coma_num_of_subject, student, teachers);
    
    // コマの割り当て
    int empty_shcedule_num = 0;
    int total_assign_num = 0;
    while (!subject_priority.empty()) {
        int subject_idx = subject_priority.top().second;
        subject_priority.pop();
        if (coma_num_of_subject[subject_idx] <= 0) continue;
        for (int day_idx = phase_start_idx; day_idx < phase_start_idx+devide_piriod_list[phase].size(); day_idx++) {
            if (!student.nomination_teacher_id[subject_idx].empty()) {
                BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                    BOOST_FOREACH(TEACHER& teacher, teachers) {
                        if (nominate_teacher_id == teacher.id) {
                            for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                                if (coma_idx == coma_num-1) break;
                                if (student.virtual_schedule[day_idx][coma_idx] ==1 && teacher.virtual_schedule[day_idx][coma_idx] == 1 && student.virtual_schedule[day_idx][coma_idx+1] == 1 && teacher.virtual_schedule[day_idx][coma_idx+1] == 1) {
                                    empty_shcedule_num++;
                                    if (total_assign_num == coma_thre_of_all) break;
                                    if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                        student.coma_of_subject_phase[phase][subject_idx]++;
                                        teacher.virtual_schedule[day_idx][coma_idx] = 0;
                                        teacher.virtual_schedule[day_idx][coma_idx+1] = 0;
                                        student.virtual_schedule[day_idx][coma_idx] = 0;
                                        student.virtual_schedule[day_idx][coma_idx+1] = 0;
                                        total_assign_num++;
                                        empty_shcedule_num--;
                                        coma_num_of_subject[subject_idx]--;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                BOOST_FOREACH(TEACHER& teacher, teachers) {
                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                        if (coma_idx == coma_num-1) break;
                        if (student.schedule[day_idx][coma_idx] ==1 && teacher.virtual_schedule[day_idx][coma_idx] == 1 && student.schedule[day_idx][coma_idx+1] == 1 && teacher.virtual_schedule[day_idx][coma_idx+1] == 1) {
                            empty_shcedule_num++;
                            if (total_assign_num == coma_thre_of_all) break;
                            if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                student.coma_of_subject_phase[phase][subject_idx]++;
                                teacher.virtual_schedule[day_idx][coma_idx] = 0;
                                teacher.virtual_schedule[day_idx][coma_idx+1] = 0;
                                student.virtual_schedule[day_idx][coma_idx] = 0;
                                student.virtual_schedule[day_idx][coma_idx+1] = 0;
                                total_assign_num++;
                                empty_shcedule_num--;
                                coma_num_of_subject[subject_idx]--;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return empty_shcedule_num;
}


void ConvertMachine::SetSubjectPriority(SUBJECT_PRIORITY& subject_priority, vector<int> coma_num_of_subject, STUDENT student, TEACHERS teachers) {
    int subject_num = student.subject.size();
    int evalution = 1000;
    for (int subject = 0; subject < subject_num; subject++) {
        evalution = coma_num_of_subject[subject];
        int empty_num_subject = 0;
        if (!student.nomination_teacher_id.empty()) {
            BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                BOOST_FOREACH(TEACHER teacher, teachers) {
                    if (teacher.id == nominated_teacher_id) {
                        if (student.grade == HIGH) {
                            for (int day = 0; day < piriod.size(); day++) {
                                for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                                    if (teacher.virtual_schedule[day][coma] == 1 && student.virtual_schedule[day][coma] == 1 && teacher.virtual_schedule[day][coma+1] == 1 && student.virtual_schedule[day][coma+1] == 1) {
                                        empty_num_subject++;
                                    }
                                }
                            }
                        } else {
                            for (int day = 0; day < piriod.size(); day++) {
                                for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                                    if (teacher.virtual_schedule[day][coma] == 1 && student.virtual_schedule[day][coma] == 1 && teacher.virtual_schedule[day][coma+1] == 1) {
                                        empty_num_subject++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        evalution -= empty_num_subject;
        subject_priority.push(make_pair(evalution, subject));
    }
}


void ConvertMachine::AssignRemainingComaJunior(STUDENT& student, TEACHERS& teachers, EMPTY_RATE& empty_rate, vector<int> coma_num_of_subject, int coma_thre_of_all) {
    // phaseの指導可能コマの空き具合(empty_rate)からphaseの優先度を決定
    PHASE_PRIORITY phase_priority;
    
    // 科目の優先度の設定
    int subject_num = coma_num_of_subject.size();
    SUBJECT_PRIORITY subject_priority;
    UpdateSubjectPriority(subject_priority, coma_num_of_subject, subject_num);
    
    // 各phase合計のコマ数を保管
    int phase_num = devide_piriod_list.size();
    vector<int> total_coma_num_phase;
    for (int phase_idx = 0; phase_idx < phase_num; phase_idx++) {
        int total_coma = 0;
        for (int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
            total_coma += student.coma_of_subject_phase[phase_idx][subject_idx];
        }
        total_coma_num_phase.push_back(total_coma);
    }
    
    
    // デバッグ用
    //            cout << student.name << endl;
    //            BOOST_FOREACH(vector<int> subject, student.coma_of_subject_phase) {
    //                BOOST_FOREACH(int coma, subject) {
    //                    cout << coma;
    //                }cout << endl;
    //            }
    //
    //        cout << "rest" << endl;
    //        BOOST_FOREACH(int num, coma_num_of_subject) {
    //            cout << num;
    //        }cout << endl;
    
    // 余りのコマを割り当てる
    int coma_num = student.schedule[0].size();
    while (!CheckRemainigComa(coma_num_of_subject)) {
        // 各phaseの平均の割り当て可能コマ数を1引き上げる
        coma_thre_of_all++;
        UpdatePhaseEmptyRate(phase_priority, empty_rate);
        while (!phase_priority.empty()) {
            int phase = phase_priority.top().second;
            phase_priority.pop();
            while (!subject_priority.empty()) {
                int subject_idx = subject_priority.top().second;
                subject_priority.pop();
                int whole_day_idx_start = PhaseToWholeDayIndex(phase);
                for (int whole_day_idx = whole_day_idx_start; whole_day_idx < whole_day_idx_start+devide_piriod_list[phase].size(); whole_day_idx++) {
                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                        if (!student.nomination_teacher_id[subject_idx].empty()) {
                            BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                                BOOST_FOREACH(TEACHER& teacher, teachers) {
                                    if (teacher.id == nominate_teacher_id) {
                                        if (student.schedule[whole_day_idx][coma_idx] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx] == 1) {
                                            if (total_coma_num_phase[phase] >= coma_thre_of_all) break;
                                            if (coma_num_of_subject[subject_idx] == 0) break;
                                            student.coma_of_subject_phase[phase][subject_idx]++;
                                            teacher.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                            coma_num_of_subject[subject_idx]--;
                                            empty_rate[phase].first--;
                                            total_coma_num_phase[phase]++;
                                        }
                                    }
                                }
                            }
                        } else {
                            BOOST_FOREACH(TEACHER& teacher, teachers) {
                                if (student.schedule[whole_day_idx][coma_idx] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx] == 1) {
                                    if (total_coma_num_phase[phase] >= coma_thre_of_all) break;
                                    if (coma_num_of_subject[subject_idx] == 0) break;
                                    student.coma_of_subject_phase[phase][subject_idx]++;
                                    teacher.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                    coma_num_of_subject[subject_idx]--;
                                    empty_rate[phase].first--;
                                    total_coma_num_phase[phase]++;
                                }
                            }
                        }
                    }
                }
            }
            //            BOOST_FOREACH(int num, coma_num_of_subject) {
            //                cout << num;
            //            }cout << endl;
            if(CheckRemainigComa(coma_num_of_subject)) break;
            UpdateSubjectPriority(subject_priority, coma_num_of_subject, subject_num);
        }
    }
}

void ConvertMachine::AssignRemainingComaHigh(STUDENT& student, TEACHERS& teachers, EMPTY_RATE& empty_rate, vector<int> coma_num_of_subject, int coma_thre_of_all) {
    // phaseの指導可能コマの空き具合(empty_rate)からphaseの優先度を決定
    PHASE_PRIORITY phase_priority;
    
    // 科目の優先度の設定
    int subject_num = coma_num_of_subject.size();
    SUBJECT_PRIORITY subject_priority;
    UpdateSubjectPriority(subject_priority, coma_num_of_subject, subject_num);
    
    // 各phase合計のコマ数を保管
    int phase_num = devide_piriod_list.size();
    vector<int> total_coma_num_phase;
    for (int phase_idx = 0; phase_idx < phase_num; phase_idx++) {
        int total_coma = 0;
        for (int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
            total_coma += student.coma_of_subject_phase[phase_idx][subject_idx];
        }
        total_coma_num_phase.push_back(total_coma);
    }
    
    
    // デバッグ用
    //            cout << student.name << endl;
    //            BOOST_FOREACH(vector<int> subject, student.coma_of_subject_phase) {
    //                BOOST_FOREACH(int coma, subject) {
    //                    cout << coma;
    //                }cout << endl;
    //            }
    //
    //        cout << "rest" << endl;
    //        BOOST_FOREACH(int num, coma_num_of_subject) {
    //            cout << num << ",";
    //        }cout << endl;
    
    // 余りのコマを割り当てる
    int coma_num = student.schedule[0].size();
    while (!CheckRemainigComa(coma_num_of_subject)) {
        // 各phaseの平均の割り当て可能コマ数を1引き上げる
        coma_thre_of_all++;
        UpdatePhaseEmptyRate(phase_priority, empty_rate);
        while (!phase_priority.empty()) {
            int phase = phase_priority.top().second;
            phase_priority.pop();
            while (!subject_priority.empty()) {
                int subject_idx = subject_priority.top().second;
                subject_priority.pop();
                int whole_day_idx_start = PhaseToWholeDayIndex(phase);
                for (int whole_day_idx = whole_day_idx_start; whole_day_idx < whole_day_idx_start+devide_piriod_list[phase].size(); whole_day_idx++) {
                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                        if (coma_idx == coma_num-1) break;
                        if (!student.nomination_teacher_id[subject_idx].empty()) {
                            BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                                BOOST_FOREACH(TEACHER& teacher, teachers) {
                                    if (teacher.id == nominate_teacher_id) {
                                        if (student.virtual_schedule[whole_day_idx][coma_idx] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx] == 1 && student.virtual_schedule[whole_day_idx][coma_idx+1] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx+1] == 1) {
                                            if (total_coma_num_phase[phase] >= coma_thre_of_all) break;
                                            if (coma_num_of_subject[subject_idx] == 0) break;
                                            student.coma_of_subject_phase[phase][subject_idx]++;
                                            teacher.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                            teacher.virtual_schedule[whole_day_idx][coma_idx+1] = 0;
                                            student.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                            student.virtual_schedule[whole_day_idx][coma_idx+1] = 0;
                                            coma_num_of_subject[subject_idx]--;
                                            empty_rate[phase].first--;
                                            total_coma_num_phase[phase]++;
                                        }
                                    }
                                }
                            }
                        } else {
                            BOOST_FOREACH(TEACHER& teacher, teachers) {
                                if (student.virtual_schedule[whole_day_idx][coma_idx] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx] == 1 && student.virtual_schedule[whole_day_idx][coma_idx+1] == 1 && teacher.virtual_schedule[whole_day_idx][coma_idx+1] == 1) {
                                    if (total_coma_num_phase[phase] >= coma_thre_of_all) break;
                                    if (coma_num_of_subject[subject_idx] == 0) break;
                                    student.coma_of_subject_phase[phase][subject_idx]++;
                                    teacher.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                    teacher.virtual_schedule[whole_day_idx][coma_idx+1] = 0;
                                    student.virtual_schedule[whole_day_idx][coma_idx] = 0;
                                    student.virtual_schedule[whole_day_idx][coma_idx+1] = 0;
                                    coma_num_of_subject[subject_idx]--;
                                    empty_rate[phase].first--;
                                    total_coma_num_phase[phase]++;
                                }
                            }
                        }
                    }
                }
            }
            //                        BOOST_FOREACH(int num, coma_num_of_subject) {
            //                            cout << num;
            //                        }cout << endl;
            if(CheckRemainigComa(coma_num_of_subject)) break;
            UpdateSubjectPriority(subject_priority, coma_num_of_subject, subject_num);
        }
        //            cout << "virtual" << endl;
        //            BOOST_FOREACH(TEACHER teacher, teachers) {
        //                for (int day_idx = 0; day_idx < piriod.size(); day_idx++) {
        //                    cout << piriod[day_idx] << endl;
        //                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
        //                        cout << student.virtual_schedule[day_idx][coma_idx];
        //                    }cout << endl;
        //                }
        //            }
    }
}


void ConvertMachine::UpdatePhaseEmptyRate(PHASE_PRIORITY& phase_priority, EMPTY_RATE empty_rate) {
    for (EMPTY_RATE::iterator empty_rate_it = empty_rate.begin(); empty_rate_it != empty_rate.end(); empty_rate_it++) {
        phase_priority.push(*empty_rate_it);
    }
}


int ConvertMachine::PhaseToWholeDayIndex(int phase) {
    int whole_day_idx = 0;
    for (int phase_idx = 0; phase_idx < phase; phase_idx++) {
        whole_day_idx += devide_piriod_list[phase_idx].size();
    }
    return whole_day_idx;
}


void ConvertMachine::UpdateSubjectPriority(SUBJECT_PRIORITY& subject_priority, vector<int> coma_num_of_subject, int subject_num) {
    for (int subject = 0; subject < subject_num; subject++) {
        subject_priority.push(make_pair(coma_num_of_subject[subject], subject));
    }
}


int ConvertMachine::CheckRemainigComa(vector<int> coma_num_of_subject) {
    int zero_num = 0;
    BOOST_FOREACH(int num, coma_num_of_subject) {
        if (num == 0) zero_num++;
    }
    if (zero_num == coma_num_of_subject.size()) return 1;
    return 0;
}


void ConvertMachine::GenerateLPProbrem(ofstream& lp, STUDENTS students, TEACHERS teachers, int phase) {
    fileout.OutputString(lp, "minimize");
    // 先生たちの指導回数の平均化（目的関数）
    GenerateTeachAverageingFunction(lp);
    fileout.OutputString(lp, "\nsubject to");
    // 先生の生徒に対する割り当て可能性
    GenerateTeacherAssignFomula(lp, students, teachers, phase);
    // 生徒の先生に対する割り当て可能性
    GenerateStudentAssignFomula(lp, students, teachers, phase);
    // 生徒のコマ数の制限
    GenerateComaFomula(lp, students, teachers, phase);
    // 高校生に対するコマの制限
    GenerateHSFomula(lp, students, teachers, phase);
    // 先生の指導回数の平均化（制約）
    GenerateTeachAverageingFomula(lp, students, teachers, phase);
    //     01変数であることを宣言
    fileout.OutputString(lp, "\nbinary");
    GenerateBinaryVariable(lp, students, teachers, phase);
    fileout.OutputString(lp, "\ngeneral");
    fileout.OutputString(lp, "y_M y_m");
    fileout.OutputString(lp, "\nend");
}


void ConvertMachine::GenerateTeachAverageingFunction(ofstream& lp) {
    fileout.OutputString(lp, "y_M - y_m");
}


void ConvertMachine::SaveWritedJuniorVariable(int teacher_id, int student_id, int subject, int day, int coma) {
    ostringstream oss;
    oss << "x_" << teacher_id << "_" << student_id << "_" << subject << "_" << day << "_" << coma;
    string variable = oss.str();
    if (writed_variable.find(variable) == writed_variable.end()) {
        writed_variable.insert(variable);
    }
}


void ConvertMachine::SaveWritedHighVariable(int teacher_id, int student_id, int subject, int day, int coma1, int coma2) {
    ostringstream oss;
    oss << "h_" << teacher_id << "_" << student_id << "_" << subject << "_" << day << "_" << coma1 << "_" << coma2;
    string variable = oss.str();
    if (writed_variable.find(variable) == writed_variable.end()) {
        writed_variable.insert(variable);
    }
}


void ConvertMachine::GenerateTeacherAssignFomula(ofstream& lp, STUDENTS students, TEACHERS teachers, int phase) {
    // ある講師のあるコマに生徒の科目を割り当てられるかどうか
    BOOST_FOREACH(TEACHER teacher, teachers) {
        int whole_day_idx_start = PhaseToWholeDayIndex(phase);
        for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                bool isOutput = false;
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.grade == JUNIOR) {  //中学生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                                SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                                isOutput = true;
                                            }
                                        }
                                    }
                                }
                            } else {  //高校生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma > 0 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma-1] == 1 && student.schedule[day][coma-1] == 1) {
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma-1, coma);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma-1, coma);
                                        isOutput = true;
                                    }
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma > 0 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma-1] == 1 && student.schedule[day][coma-1] == 1) {
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma-1, coma);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma-1, coma);
                                                isOutput = true;
                                            }
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                isOutput = true;
                                            }
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

void ConvertMachine::GenerateStudentAssignFomula(ofstream& lp, STUDENTS students, TEACHERS teachers, int phase) {
    // ある講師のあるコマに生徒の科目を割り当てられるかどうか
    BOOST_FOREACH(STUDENT student, students) {
        int whole_day_idx_start = PhaseToWholeDayIndex(phase);
        for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
            for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                bool isOutput = false;
                BOOST_FOREACH(TEACHER teacher, teachers) {
                    for (int subject = 0; subject < student.subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.grade == JUNIOR) {  //中学生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                                SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                                isOutput = true;
                                            }
                                        }
                                    }
                                }
                            } else {
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma > 0 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma-1] == 1 && student.schedule[day][coma-1] == 1) {
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma-1, coma);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma-1, coma);
                                        isOutput = true;
                                    }
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma > 0 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma-1] == 1 && student.schedule[day][coma-1] == 1) {
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma-1, coma);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma-1, coma);
                                                isOutput = true;
                                            }
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                isOutput = true;
                                            }
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

void ConvertMachine::GenerateComaFomula(ofstream& lp,STUDENTS students, TEACHERS teachers, int phase) {
    BOOST_FOREACH(STUDENT student, students) {
        for (int subject = 0; subject < student.subject.size(); subject++) {
            bool isOutput = false;
            BOOST_FOREACH(TEACHER teacher, teachers) {
                int whole_day_idx_start = PhaseToWholeDayIndex(phase);
                for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
                    for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                        if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.grade == JUNIOR) {  //中学生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                                SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                                isOutput = true;
                                            }
                                        }
                                    }
                                }
                            }else {
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                isOutput = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(isOutput) {
                int subject_num = student.coma_of_subject_phase[phase][subject];
                string subject_num_str = lexical_cast<string>(subject_num);
                fileout.OutputString(lp, "= " + subject_num_str);
            }
        }
    }
}


void ConvertMachine::GenerateHSFomula(ofstream& lp, STUDENTS students, TEACHERS teachers, int phase) {
    BOOST_FOREACH(STUDENT student, students) {
        if (student.grade == HIGH) {
            BOOST_FOREACH(TEACHER teacher, teachers) {
                for (int subject = 0; subject < student.subject.size(); subject++) {
                    if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {
                        int whole_day_idx_start = PhaseToWholeDayIndex(phase);
                        for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
                            int output_num = 0;
                            for (int coma = 0; coma < student.schedule[day].size(); coma++) {
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        output_num++;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                output_num++;
                                            }
                                        }
                                    }
                                }
                                if (output_num == 2) {
                                    fileout.OutputString(lp, "<= 1");
                                    output_num = 0;
                                    coma--;
                                }
                            }
                            if (output_num == 1) {
                                fileout.OutputString(lp, "<= 1");
                            }
                        }
                    }
                }
            }
        }
    }
}


void ConvertMachine::GenerateTeachAverageingFomula(ofstream& lp,STUDENTS students, TEACHERS teachers, int phase) {
    BOOST_FOREACH(TEACHER teacher, teachers) {
        bool isOutput = false;
        int whole_day_idx_start = PhaseToWholeDayIndex(phase);
        for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.grade == JUNIOR) {  //中学生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        isOutput = true;
                                        SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                                isOutput = true;
                                                SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                            }
                                        }
                                    }
                                }
                            } else {
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+ 2", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+ 2", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                isOutput = true;
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
        if(isOutput) fileout.OutputString(lp, "- y_M <= 0");
    }
    BOOST_FOREACH(TEACHER teacher, teachers) {
        bool isOutput = false;
        int whole_day_idx_start = PhaseToWholeDayIndex(phase);
        for (int day = whole_day_idx_start; day < whole_day_idx_start+devide_piriod_list[phase].size(); day++) {
            for (int coma = 0; coma < teacher.schedule[day].size(); coma++) {
                BOOST_FOREACH(STUDENT student, students) {
                    for (int subject = 0; subject < teacher.teach_subject.size(); subject++) {
                        if (teacher.teach_subject[subject] == 1 && student.coma_of_subject_phase[phase][subject] > 0) {  //ある科目が指導できるか、その生徒がその教科を取っているか
                            if (student.grade == JUNIOR) {  //中学生の場合
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                        //x_teacherid_studentid_subject_day_comaを生成
                                        fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                        isOutput = true;
                                        SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1) {  //どちらのスケジュールも空いていたら
                                                //x_teacherid_studentid_subject_day_comaを生成
                                                fileout.OutputJuniorVariable(lp, "+", teacher.id, student.id, subject, day, coma);
                                                isOutput = true;
                                                SaveWritedJuniorVariable(teacher.id, student.id, subject, day, coma);
                                            }
                                        }
                                    }
                                }
                            } else {
                                if (student.nomination_teacher_id[subject].empty()) {  //講師の指定がなかった場合
                                    if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                        fileout.OutputHighVariable(lp, "+ 2", teacher.id, student.id, subject, day, coma, coma+1);
                                        SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                        isOutput = true;
                                    }
                                } else {  //講師の指定があった場合
                                    BOOST_FOREACH(int nominated_teacher_id, student.nomination_teacher_id[subject]) {
                                        if (nominated_teacher_id == teacher.id) {  //ある生徒のある科目の指定講師だった場合
                                            if (coma < student.schedule[day].size()-1 && teacher.schedule[day][coma] == 1 && student.schedule[day][coma] == 1 && teacher.schedule[day][coma+1] == 1 && student.schedule[day][coma+1] == 1) {
                                                fileout.OutputHighVariable(lp, "+ 2", teacher.id, student.id, subject, day, coma, coma+1);
                                                SaveWritedHighVariable(teacher.id, student.id, subject, day, coma, coma+1);
                                                isOutput = true;
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
        if(isOutput) fileout.OutputString(lp, "- y_m >= 0");
    }
}

void ConvertMachine::GenerateBinaryVariable(ofstream& lp, STUDENTS students, TEACHERS teachers, int phase) {
    for (set<string>::iterator wv_it = writed_variable.begin(); wv_it != writed_variable.end(); ++wv_it) {
        //        cout << *wv_it << endl;
        fileout.OutputBinaryVariable(lp, *wv_it);
    }
    fileout.OutputString(lp, "");
}

void ConvertMachine::ExecuteConvertCommand() {
    vector<string> command_set;
    for (int phase_idx = 0; phase_idx < devide_piriod_list.size(); phase_idx++) {
        ostringstream oss;
        oss << "./solver/glpsol --cpxlp ./lp/netz" << phase_idx << ".lp -o ./sol/netz" << phase_idx << ".sol";
        string command = oss.str();
        command_set.push_back(command);
    }
    BOOST_FOREACH(string command, command_set) {
        //        cout << command << endl;
        system(command.c_str());
        cout << endl;
    }
}







