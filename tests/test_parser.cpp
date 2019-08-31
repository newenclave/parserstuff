
#include <iostream>
#include <vector>

#include "erules/parser.h"

struct TestState {
    using id_type = int;
    static id_type id(const TestState& state)
    {
        return state.id_;
    }
    int id_;
};

struct TestNode {
    int id;
};

using TestParser = parser<TestNode, TestState>;

namespace test_parser {
void run()
{
    std::vector<TestState> lex = { { 1 }, { 1 }, { 2 }, { 2 }, { 4 } };
    TestParser parser(lex);
    parser.advance();
    parser.advance();
    std::cout << "2=" << parser.current().id_ << "\n";
    auto state = parser.store();
    parser.advance();
    parser.advance();
    parser.restore(state);
    std::cout << "2=" << parser.current().id_ << "\n";
}
}
