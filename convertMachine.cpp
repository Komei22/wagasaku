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
    
    
    // 一週間ごとに期間を分割
    //    int devide_day_start = 0;
    //    int training_priod = piriod.size();
    //    DEVIDE_PIRIOD devide_piriod;
    //    vector<DEVIDE_PIRIOD> devide_piriod_list_tmp;
    //    while (devide_day_start <= training_priod) {
    //        for (int day = devide_day_start; day < devide_day_start + DEVIDE_SIZE; day++) {
    //            devide_piriod.push_back(piriod[day]);
    //            if (day == training_priod) break;
    //        }
    //        devide_piriod_list_tmp.push_back(devide_piriod);
    //        devide_piriod.clear();
    //        devide_day_start = devide_day_start + DEVIDE_SIZE;
    //    }
    
    // 最後分割部分が4日より小さかった場合、最後一つ前のリストの平均化
    //    int devided_list_size = devide_piriod_list_tmp.size();
    //    devide_piriod_list.resize(devided_list_size);
    //    if (devide_piriod_list_tmp.back().size() < DEVIDE_SIZE-2) {
    //        int back_list_mean = ceil((devide_piriod_list_tmp.back().size() + devide_piriod_list_tmp[devided_list_size-2].size())/2);
    //        for (int idx = 0; idx < devided_list_size; idx++) {
    //            if (idx == devided_list_size-2) {
    //                int push_num = 0;
    //                for (int day_idx = (idx)*DEVIDE_SIZE; day_idx < training_priod; day_idx++) {
    //                    if (push_num < back_list_mean) {
    //                        devide_piriod_list[idx].push_back(piriod[day_idx]);
    //                    } else {
    //                        devide_piriod_list[idx+1].push_back(piriod[day_idx]);
    //                    }
    //                    push_num++;
    //                }
    //                break;
    //            } else {
    //                devide_piriod_list[idx] = devide_piriod_list_tmp[idx];
    //            }
    //        }
    //    } else {
    //        for (int idx = 0; idx < devided_list_size; idx++) {
    //            devide_piriod_list[idx] = devide_piriod_list_tmp[idx];
    //        }
    //    }
    
    //        BOOST_FOREACH(DEVIDE_PIRIOD dp, devide_piriod_list) {
    ////            cout << dp.size() << endl;
    //            BOOST_FOREACH(string str, dp) {
    //                cout << str << endl;
    //            }
    //        }
    
}


void ConvertMachine::DevideSubject(STUDENTS students, TEACHERS teachers) {
    BOOST_FOREACH(STUDENT student, students) {
        int phase_num = devide_piriod_list.size();
        int subject_num = student.subject.size();
        // ある生徒の各教科毎の1フェイズのコマ数の閾値を計算
        vector<int> coma_thre_of_subject(subject_num, 0);
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
        
        DistributeComaForFhase(student, teachers, coma_thre_of_subject);
        
    }
}

void ConvertMachine::DistributeComaForFhase(STUDENT student, TEACHERS teachers, vector<int> coma_thre_of_subject) {
    int phase_num = devide_piriod_list.size();
    int phase_start_idx = 0;
    
    // 生徒のphaseごとの科目のコマ数の初期化
    int subject_num = student.subject.size();
    student.devide_coma_num_of_subject.resize(phase_num);
    for (int phase = 0; phase < phase_num; phase++) {
        student.devide_coma_num_of_subject[phase].assign(subject_num, 0);
    }
    
    vector<int> coma_num_of_subject;
    copy(student.subject.begin(), student.subject.end(), back_inserter(coma_num_of_subject));
    // あるphaseに割り当てができるかをチェックして割り当て可能なら割り当てていく、空きコマが足りない場合は出来るだけ割り当てておく
    for (int phase = 0; phase < phase_num; phase++) {
        AssignComaInFhase(student, teachers, phase_start_idx, phase, coma_num_of_subject, coma_thre_of_subject);
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

void ConvertMachine::AssignComaInFhase(STUDENT& student, TEACHERS teachers, int phase_start_idx, int phase, vector<int> &coma_num_of_subject, vector<int> coma_thre_of_subject) {
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
    // コマの割り当て
    int empty_shcedule_num = 0;
    for (int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
        if (coma_num_of_subject[subject_idx] <= 0) continue;
        for (int day_idx = phase_start_idx; day_idx < phase_start_idx+devide_piriod_list[phase].size(); day_idx++) {
            if (!student.nomination_teacher_id[subject_idx].empty()) {
                BOOST_FOREACH(int nominate_teacher_id, student.nomination_teacher_id[subject_idx]) {
                    BOOST_FOREACH(TEACHER teacher, teachers) {
                        if (nominate_teacher_id == teacher.id) {
                            for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                                if (student.schedule[day_idx][coma_idx] ==1 && teacher.schedule[day_idx][coma_idx] == 1) {
                                    if(subject_idx == first_search_subject) empty_shcedule_num++;
                                    if (empty_shcedule_num == 0) break;
                                    if (student.devide_coma_num_of_subject[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                        student.devide_coma_num_of_subject[phase][subject_idx]++;
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
                BOOST_FOREACH(TEACHER teacher, teachers) {
                    for (int coma_idx = 0; coma_idx < coma_num; coma_idx++) {
                        if (student.schedule[day_idx][coma_idx] ==1 && teacher.schedule[day_idx][coma_idx] == 1) {
                            if(subject_idx == first_search_subject) empty_shcedule_num++;
                            if (empty_shcedule_num == 0) break;
                            if (student.devide_coma_num_of_subject[phase][subject_idx] < coma_thre_of_subject[subject_idx]) {
                                student.devide_coma_num_of_subject[phase][subject_idx]++;
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












