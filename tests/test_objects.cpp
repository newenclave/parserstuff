#include <cstdint>
#include <iostream>
#include <memory>
#include <functional>
#include "erules/objects.h"


using namespace erules;
namespace test_objects {

class erules_define_object(test1) {
} ;

class erules_define_object(test2) {
} ;

class erules_define_object(test3) {
} ;

void run()
{
    test1 t1;
    test2 t2;
    test3 t3;

    object::info::create<test1>();

    std::cout << object::cast<test2>(&t1)->get_info()->id << "\n";
    std::cout << t2.get_info()->id << "\n";
    std::cout << t3.get_info()->id << "\n";
}
}
