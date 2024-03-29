#include "erules/objects.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

#include "erules/rules_basic_operations.h"

using namespace erules;
using namespace erules::objects;

namespace test_objects {


void run()
{
    using string_obj = string<char>;
    string<char> t0("true");
    string<char> t1;
    number t2(3);
    floating t3(11.11);

    std::cout << base::cast<string<char>>(&t1)->type_info()->id << "\n";
    std::cout << t2.type_info()->id << "\n";
    std::cout << t3.type_info()->id << "\n";
    std::cout << t1.type_info()->id << "\n";

    auto binop = operations::binary_operations<char>::get();
    auto transop = operations::transform_operations<char>::get();

    auto bval = transop.call<boolean>(&t0);

    std::cout << binop.call_cast<boolean>(constants::token_type::EQ, &t2, &t2)
                     ->value()
              << "\n";
}

}
