// convertMachine.cpp
#include <math.h>
#include <queue>
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
    student.devide_coma_num_of_subject.resize(phase_num);
    for (int phase = 0; phase < phase_num; phase++) {
        student.devide_coma_num_of_subject[phase].assign(subject_num, 0);
    }
    
    int phase_start_idx = 0;
    vector<int> coma_num_of_subject;
    copy(student.subject.begin(), student.subject.end(), back_inserter(coma_num_of_subject));
    // あるphaseに割り当てができるかをチェックして割り当て可能なら割り当てていく、空きコマが足りない場合は出来るだけ割り当てておく
    for (int phase = 0; phase < phase_num; phase++) {
        AssignComaInFhase(student, teachers, phase_start_idx, phase, coma_num_of_subject, coma_thre_of_subject, coma_thre_of_all);
        phase_start_idx += devide_piriod_list[phase].size();
    }
    
    // 割り当てなければならないコマが残っていたとき、余ってる分を余裕のあるphaseに割り当てる
    
    
    
    
    //    デバッグ用　phaseごとの各科目のコマ数
    cout << student.name << endl;
    BOOST_FOREACH(vector<int> subject, student.devide_coma_num_of_subject) {
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
    
    coma_thre_of_all = ceil(coma_thre_of_all/(double)phase_num);
    
    return coma_thre_of_all;
}


void ConvertMachine::AssignComaInFhase(STUDENT& student, TEACHERS& teachers, int phase_start_idx, int phase, vector<int> &coma_num_of_subject, vector<int> coma_thre_of_subject, int coma_thre_of_all) {
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
                                    if (teacher.empty_of_phase[phase] == 0) break;
                                    if (total_assign_num == coma_thre_of_all) break;
                                    if (student.devide_coma_num_of_subject[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                        student.devide_coma_num_of_subject[phase][subject_idx]++;
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
                            if (teacher.empty_of_phase[phase] == 0) break;
                            if (total_assign_num == coma_thre_of_all) break;
                            if (student.devide_coma_num_of_subject[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                student.devide_coma_num_of_subject[phase][subject_idx]++;
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












