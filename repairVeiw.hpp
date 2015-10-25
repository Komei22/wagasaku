// repirVeiw.hpp
#ifndef repairVeiw_hpp
#define repairVeiw_hpp

#include <stdio.h>
#include <vector>
#include "student.hpp"
#include "teacher.hpp"
#include "convertMachine.hpp"

using namespace std;

//0:先生,1:生徒,2:科目,3:日,4:コマ
typedef vector<int> ASSIGN_STATE;


class RepairVeiw {
public:
    vector<ASSIGN_STATE> assign;
    
public:
    // 変数からスケジュールの復元
    void DecodeSchedule(STUDENTS, TEACHERS, ConvertMachine);
    
    
};


#endif /* repairVeiw_hpp */
