#include "erules/objects.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

#include "erules/rules_basic_operations.h"

using namespace erules;
namespace test_objects {


void run()
{
    using string_obj = string<char>;
    string<char> t0("!");
    string<char> t1;
    number t2(3);
    floating t3(11.11);

    std::cout << object::cast<string<char>>(&t1)->type_info()->id << "\n";
    std::cout << t2.type_info()->id << "\n";
    std::cout << t3.type_info()->id << "\n";
    std::cout << t1.type_info()->id << "\n";

    auto binop = erules::binary_operations<char>::get();
    std::cout << binop
                     .call_cast<string_obj>(constants::token_type::MUL, &t0,
                                            &t2)
                     ->value()
              << "\n";
    std::cout << binop
                     .call_cast<string_obj>(constants::token_type::MUL, &t2,
                                            &t0)
                     ->value()
              << "\n";

    std::cout << binop
                     .call_cast<floating>(constants::token_type::MUL, &t3, &t2)
                     ->value()
              << "\n";
    std::cout << binop
                     .call_cast<floating>(constants::token_type::MUL, &t2, &t3)
                     ->value()
              << "\n";

    std::cout << binop
                     .call_cast<floating>(constants::token_type::MINUS, &t2,
                                          &t3)
                     ->value()
              << "\n";
    std::cout << binop
                     .call_cast<floating>(constants::token_type::MINUS, &t3,
                                          &t2)
                     ->value()
              << "\n";
}

}
