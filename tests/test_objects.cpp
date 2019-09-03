#include "erules/objects.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>


using namespace erules;
namespace test_objects {


void run()
{
    string<char> t1;
    number t2;
    floating t3;

    std::cout << object::cast<string<char>>(&t1)->type_info()->id << "\n";
    std::cout << t2.type_info()->id << "\n";
    std::cout << t3.type_info()->id << "\n";
    std::cout << t1.type_info()->id << "\n";
}
}
