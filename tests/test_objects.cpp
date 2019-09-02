#include "erules/objects.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>


using namespace erules;
namespace test_objects {

class erules_define_object(test1){
    public : test1() : typed_object<test1>(__func__){}
};

class erules_define_object(test2){
    public : test2() : typed_object<test2>(__func__){}
};

class erules_define_object(test3){
    public : test3() : typed_object<test3>(__func__){}
};

void run()
{
    test1 t1;
    test2 t2;
    test3 t3;

    object::info::create<test1>();

    std::cout << object::cast<test1>(&t1)->type_info()->id << "\n";
    std::cout << t2.type_info()->id << "\n";
    std::cout << t3.type_info()->id << "\n";
    std::cout << t3.type_info()->id << "\n";
}
}
