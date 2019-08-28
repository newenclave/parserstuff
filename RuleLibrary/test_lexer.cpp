#include "pch.h"

#include "lexer.h"
#include <iostream>
#include <vector>

std::string operator""_s(const char* data, std::size_t length)
{
    return data;
}

namespace test_lexer {

struct LexerState {
    using less_type = std::less<char>;
    using iterator = std::string::const_iterator;
    LexerState(int val)
        : value(val)
    {
    }
    int value;
    iterator tokenBegin;
    iterator tokenEnd;
};
using MyLexer = lexer<std::string, LexerState>;

MyLexer::token_state_factory createFactory(int value)
{
    return [value](auto state, auto iState) {
        state.value = value;
        state.tokenBegin = iState.begin();
        state.tokenEnd = iState.end();
        return state;
    };
}

void test_01()
{
    std::string test_input = R"(12312312344444)";

    MyLexer lex([&test_input]() { return LexerState{ 0 }; });

    lex.add_factory("123", createFactory(123));
    lex.add_factory("123123", createFactory(123123));
    lex.add_factory("4", createFactory(4));
    lex.add_factory("44", createFactory(44));

    auto b = test_input.cbegin();
    auto e = test_input.cend();
    std::vector<int> result;
    while (b != e) {
        auto state = lex.next(b, e);
        result.emplace_back(state.value);
        b = state.tokenEnd;
    }
    std::cout << "Result length: " << result.size() << std::endl;
    int i = 0;
    for (auto val : result) {
        std::cout << "Result[" << i++ << "]: " << val << std::endl;
    }
}

void run()
{
    test_01();
}
}