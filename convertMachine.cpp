// convertMachine.cpp
#include <math.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "convertMachine.hpp"
#include "fileio.hpp"

using namespace std;
using namespace boost;

FILEIO fileout;

#define DEVIDE_SIZE 7

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
    // 講師のphaseごとの空きコマ数
    CountTeacherEmptySchedule(teachers);
    BOOST_FOREACH(STUDENT& student, students) {
        DistributeComaForFhase(student, teachers);
    }
}


void ConvertMachine::CountTeacherEmptySchedule(TEACHERS& teachers) {
    int phase_num = devide_piriod_list.size();
    BOOST_FOREACH(TEACHER& teacher, teachers) {
        teacher.empty_of_phase.resize(phase_num, 0);
        int coma_num = teacher.schedule[0].size();
        int phase_start_idx = 0;
        for (int phase = 0; phase < phase_num; phase++) {
            for (int day_idx = phase_start_idx; day_idx < phase_start_idx+devide_piriod_list[phase].size(); day_idx++) {
                for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                    if (teacher.schedule[day_idx][coma_idx] == 1) {
                        teacher.empty_of_phase[phase]++;
                    }
                }
            }
            phase_start_idx += devide_piriod_list[phase].size();
        }
    }
    // デバッグ用
    //    BOOST_FOREACH(TEACHER teacher, teachers) {
    //        cout << teacher.name << endl;
    //        BOOST_FOREACH(int num, teacher.empty_of_phase) {
    //            cout << num << ",";
    //        }cout << endl;
    //    }
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
        int empty_num = AssignComaInFhase(student, teachers, phase_start_idx, phase, coma_num_of_subject, coma_thre_of_subject, coma_thre_of_all);
        empty_rate.push_back(make_pair(empty_num, phase));
        phase_start_idx += devide_piriod_list[phase].size();
    }
    
    // 割り当てなければならないコマが残っていたとき、余ってる分を余裕のあるphaseに割り当てる
    bool is_rest = false;
    for (int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
        if(coma_num_of_subject[subject_idx] > 0) is_rest = true;
    }
    if (is_rest) AssignRemainingComa(student, teachers, empty_rate, coma_num_of_subject, coma_thre_of_all);
    
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


int ConvertMachine::AssignComaInFhase(STUDENT& student, TEACHERS& teachers, int phase_start_idx, int phase, vector<int> &coma_num_of_subject, vector<int> coma_thre_of_subject, int coma_thre_of_all) {
    int subject_num = student.subject.size();
    int coma_num = student.schedule[0].size();
    // 生徒の最初に探索される教科
    int first_search_subject;
    for (int subject = 0; subject < subject_num; subject++) {
        if(student.subject[subject] != 0){
            first_search_subject = subject;
            break;
        }
    }
    // 科目の優先度の設定
    typedef priority_queue<pair<int, int> > SUBJECT_PRIORITY;
    SUBJECT_PRIORITY subject_priority;
    for (int subject = 0; subject < subject_num; subject++) {
        subject_priority.push(make_pair(coma_num_of_subject[subject], subject));
    }
    
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
                                if (student.schedule[day_idx][coma_idx] ==1 && teacher.schedule[day_idx][coma_idx] == 1) {
                                    if(subject_idx == first_search_subject) empty_shcedule_num++;
                                    if (teacher.empty_of_phase[phase] == 0 || total_assign_num == coma_thre_of_all) break;
                                    if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                        student.coma_of_subject_phase[phase][subject_idx]++;
                                        total_assign_num++;
                                        empty_shcedule_num--;
                                        teacher.empty_of_phase[phase]--;
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
                        if (student.schedule[day_idx][coma_idx] ==1 && teacher.schedule[day_idx][coma_idx] == 1) {
                            if(subject_idx == first_search_subject) empty_shcedule_num++;
                            if (teacher.empty_of_phase[phase] == 0 || total_assign_num == coma_thre_of_all) break;
                            if (student.coma_of_subject_phase[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                student.coma_of_subject_phase[phase][subject_idx]++;
                                total_assign_num++;
                                empty_shcedule_num--;
                                teacher.empty_of_phase[phase]--;
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


void ConvertMachine::AssignRemainingComa(STUDENT& student, TEACHERS& teachers, EMPTY_RATE& empty_rate, vector<int> coma_num_of_subject, int coma_thre_of_all) {
    // phaseの指導可能コマの空き具合(empty_rate)からphaseの優先度を決定
    PHASE_PRIORITY phase_priority;
//    UpdatePhaseEmptyRate(phase_priority, empty_rate);
    
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
//        cout << student.name << endl;
//        BOOST_FOREACH(vector<int> subject, student.coma_of_subject_phase) {
//            BOOST_FOREACH(int coma, subject) {
//                cout << coma;
//            }cout << endl;
//        }
//    
//    cout << "rest" << endl;
//    BOOST_FOREACH(int num, coma_num_of_subject) {
//        cout << num;
//    }cout << endl;
    
    // 余りのコマを割り当てる
    int coma_num = student.schedule[0].size();
    while (!CheckRemainigComa(coma_num_of_subject)) {
        // 各phaseの平均の割り当て可能コマ数を1引き上げる
        coma_thre_of_all++;
        UpdatePhaseEmptyRate(phase_priority, empty_rate);
        while (!phase_priority.empty()) {
            int phase = phase_priority.top().second;
            phase_priority.pop();
            //        cout << "phase:" << phase << endl;
            while (!subject_priority.empty()) {
                int subject_idx = subject_priority.top().second;
                subject_priority.pop();
                //            cout << "subject:" << subject_idx << endl;
                int whole_day_idx = PhaseToWholeDayIndex(phase);
                for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                    if (!student.nomination_teacher_id[subject_idx].empty()) {
                        BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                            BOOST_FOREACH(TEACHER teacher, teachers) {
                                if (teacher.id == nominate_teacher_id) {
                                    if (student.schedule[whole_day_idx][coma_idx] == 1 && teacher.schedule[whole_day_idx][coma_idx] == 1) {
                                        if (total_coma_num_phase[phase] >= coma_thre_of_all || teacher.empty_of_phase[phase] == 0) break;
                                        if (coma_num_of_subject[subject_idx] == 0) break;
                                        student.coma_of_subject_phase[phase][subject_idx]++;
                                        coma_num_of_subject[subject_idx]--;
                                        teacher.empty_of_phase[phase]--;
                                        empty_rate[phase].first--;
                                        total_coma_num_phase[phase]++;
                                    }
                                }
                            }
                        }
                    } else {
                        BOOST_FOREACH(TEACHER teacher, teachers) {
                            if (student.schedule[whole_day_idx][coma_idx] == 1 && teacher.schedule[whole_day_idx][coma_idx] == 1) {
                                if (total_coma_num_phase[phase] >= coma_thre_of_all || teacher.empty_of_phase[phase] == 0) break;
                                if (coma_num_of_subject[subject_idx] == 0) break;
                                student.coma_of_subject_phase[phase][subject_idx]++;
                                coma_num_of_subject[subject_idx]--;
                                teacher.empty_of_phase[phase]--;
                                empty_rate[phase].first--;
                                total_coma_num_phase[phase]++;
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


void ConvertMachine::GenerateLPProbrem(ofstream& lp, STUDENTS students, TEACHERS teachers) {
        fileout.OutputString(lp, "minimize");
        // 先生たちの指導回数の平均化（目的関数）
        GenerateTeachAverageingFunction(lp);
        fileout.OutputString(lp, "\nsubject to");
        // 先生の生徒に対する割り当て可能性
        GenerateTeacherAssignFomula(lp, students, teachers);
    //    // 生徒の先生に対する割り当て可能性
    //    convert_machine.GenerateStudentAssignFomula(lp, students, teachers);
    //    // 生徒のコマ数の制限
    //    convert_machine.GenerateComaFomula(lp, students, teachers);
    //    // 高校生に対するコマの制限
    //    convert_machine.GenerateHSFomula(lp, students, teachers);
    //    // 先生の指導回数の平均化（制約）
    //    convert_machine.GenerateTeachAverageingFomula(lp, students, teachers);
    //    // 01変数であることを宣言
//        fileiout.OutputString(lp, "\nbinary");
//        GenerateBinaryVariable(lp, students, teachers);
//        fileiout.OutputString(lp, "\ngeneral");
//        fileiout.OutputString(lp, "y_M y_m");
        fileiout.OutputString(lp, "\nend");
}


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












