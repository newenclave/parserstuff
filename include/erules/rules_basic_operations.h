#ifndef ERULES_BASIC_OPERATIONS_OBJECTS_H
#define ERULES_BASIC_OPERATIONS_OBJECTS_H

#include "erules/objects.h"
#include "erules/operations.h"
#include "erules/rule_lexem.h"
#include <sstream>

namespace erules { namespace operations {

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class binary_operations {
    public:
        using lexem_type = filters::rule_lexem<CharT, LessType>;
        using id_type = typename lexem_type::id_type;
        using string_type = objects::string<CharT>;

        static objects::oprerations::binary<id_type> get()
        {
            using namespace objects;

            objects::oprerations::binary<id_type> result;
            /// + operation. numbers, float, strings
            result.template set<number, number>(
                constants::token_type::PLUS, [](auto l, auto r) {
                    return std::make_unique<number>(l->value() + r->value());
                });
            result.template set<floating, number>(
                constants::token_type::PLUS,
                [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        + static_cast<double>(r->value()));
                },
                true);
            result.template set<string_type, string_type>(
                constants::token_type::PLUS, [](auto l, auto r) {
                    return std::make_unique<string_type>(l->value()
                                                         + r->value());
                });
            /// - number floating
            result.template set<number, number>(
                constants::token_type::MINUS, [](auto l, auto r) {
                    return std::make_unique<number>(l->value() - r->value());
                });
            result.template set<floating, number>(
                constants::token_type::MINUS, [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        - static_cast<double>(r->value()));
                });
            result.template set<number, floating>(
                constants::token_type::MINUS, [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        - static_cast<double>(r->value()));
                });

            /// * numbers, float, string * number
            result.template set<number, number>(
                constants::token_type::MUL, [](auto l, auto r) {
                    return std::make_unique<number>(l->value() * r->value());
                });
            result.template set<floating, number>(
                constants::token_type::MUL,
                [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        * static_cast<double>(r->value()));
                },
                true);
            result.template set<string_type, number>(
                constants::token_type::MUL,
                [](auto l, auto r) {
                    std::basic_string<CharT> out;
                    out.reserve(l->value().size()
                                * static_cast<std::size_t>(r->value()));
                    auto count = r->value();
                    while (count-- > 0) {
                        out += l->value();
                    }
                    return std::make_unique<string_type>(std::move(out));
                },
                true);

            /// / number, float
            result.template set<number, number>(
                constants::token_type::DIV, [](auto l, auto r) {
                    return std::make_unique<number>(l->value() / r->value());
                });
            result.template set<floating, number>(
                constants::token_type::DIV, [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        / static_cast<double>(r->value()));
                });

            result.template set<number, floating>(
                constants::token_type::DIV, [](auto l, auto r) {
                    return std::make_unique<floating>(
                        static_cast<double>(l->value())
                        / static_cast<double>(r->value()));
                });

            /// mod numbers only
            result.template set<number, number>(
                constants::token_type::MOD, [](auto l, auto r) {
                    return std::make_unique<number>(l->value() % r->value());
                });

            fill_logic(result);
            return result;
        }

        static void fill_logic(objects::oprerations::binary<id_type>& result)
        {
            auto logic_EQ = [](auto left, auto right) { return left == right; };
            auto logic_NEQ
                = [](auto left, auto right) { return left != right; };
            auto logic_LT = [](auto left, auto right) { return left < right; };
            auto logic_GT = [](auto left, auto right) { return left > right; };
            auto logic_LEQ
                = [](auto left, auto right) { return left <= right; };
            auto logic_GEQ
                = [](auto left, auto right) { return left >= right; };

            /// numbers
            result.template set<number, number>(constants::token_type::EQ,
                                                create_logic(logic_EQ));
            result.template set<number, number>(constants::token_type::NOTEQ,
                                                create_logic(logic_NEQ));
            result.template set<number, number>(constants::token_type::LT,
                                                create_logic(logic_LT));
            result.template set<number, number>(constants::token_type::GT,
                                                create_logic(logic_GT));
            result.template set<number, number>(constants::token_type::LEQ,
                                                create_logic(logic_LEQ));
            result.template set<number, number>(constants::token_type::GEQ,
                                                create_logic(logic_GEQ));

            /// strings
            result.template set<string_type, string_type>(
                constants::token_type::EQ, create_logic(logic_EQ));
            result.template set<string_type, string_type>(
                constants::token_type::NOTEQ, create_logic(logic_NEQ));
            result.template set<string_type, string_type>(
                constants::token_type::LT, create_logic(logic_LT));
            result.template set<string_type, string_type>(
                constants::token_type::GT, create_logic(logic_GT));
            result.template set<string_type, string_type>(
                constants::token_type::LEQ, create_logic(logic_LEQ));
            result.template set<string_type, string_type>(
                constants::token_type::GEQ, create_logic(logic_GEQ));

            /// floating
            result.template set<floating, floating>(constants::token_type::EQ,
                                                    create_logic(logic_EQ));
            result.template set<floating, floating>(
                constants::token_type::NOTEQ, create_logic(logic_NEQ));
            result.template set<floating, floating>(constants::token_type::LT,
                                                    create_logic(logic_LT));
            result.template set<floating, floating>(constants::token_type::GT,
                                                    create_logic(logic_GT));
            result.template set<floating, floating>(constants::token_type::LEQ,
                                                    create_logic(logic_LEQ));
            result.template set<floating, floating>(constants::token_type::GEQ,
                                                    create_logic(logic_GEQ));

            /// boolean
            result.template set<boolean, boolean>(constants::token_type::EQ,
                                                  create_logic(logic_EQ));
            result.template set<boolean, boolean>(constants::token_type::NOTEQ,
                                                  create_logic(logic_NEQ));
            result.template set<boolean, boolean>(constants::token_type::LT,
                                                  create_logic(logic_LT));
            result.template set<boolean, boolean>(constants::token_type::GT,
                                                  create_logic(logic_GT));
            result.template set<boolean, boolean>(constants::token_type::LEQ,
                                                  create_logic(logic_LEQ));
            result.template set<boolean, boolean>(constants::token_type::GEQ,
                                                  create_logic(logic_GEQ));

            /// arrays
        }

    private:
        template <typename CallT>
        static auto create_logic(CallT call)
        {
            return [call](auto left, auto right) {
                return std::make_unique<boolean>(
                    call(left->value(), right->value()));
            };
        }
    };

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class unary_operations {
    public:
        using lexem_type = filters::rule_lexem<CharT, LessType>;
        using id_type = typename lexem_type::id_type;
        static objects::oprerations::unary<id_type> get()
        {
            using namespace objects;
            objects::oprerations::unary<id_type> result;

            return result;
        }
    };

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class transform_operations {
    public:
        using lexem_type = filters::rule_lexem<CharT, LessType>;
        using id_type = typename lexem_type::id_type;
        using string_type = objects::string<CharT>;
        using stream_type = std::basic_stringstream<CharT>;
        static objects::oprerations::transform get()
        {
            using namespace objects;
            objects::oprerations::transform result;

            result.template set<number, string_type>(
                [](auto num) { return to_string(num->value()); });

            result.template set<floating, string_type>(
                [](auto num) { return to_string(num->value()); });

            result.template set<boolean, string_type>([](auto val) {
                return to_string(
                    val->value() ? helpers::strings::to_string<CharT>("true")
                                 : helpers::strings::to_string<CharT>("false"));
            });

            result.template set<string_type, string_type>(
                [](auto val) { return val->clone(); });

            result.template set<string_type, number>([](auto num) {
                auto val = num->value();
                auto num_val
                    = helpers::reader::read_int(val.begin(), val.end());
                return std::make_unique<number>(num_val);
            });

            result.template set<string_type, number>(
                [](auto str) -> base::uptr {
                    auto val = str->value();
                    int inval = -1;
                    auto num_val = helpers::reader::read_int(val.begin(),
                                                             val.end(), &inval);
                    if (-1 != inval) {
                        return {};
                    }
                    return std::make_unique<number>(num_val);
                });

            result.template set<string_type, floating>([](auto str) {
                auto val = str->value();
                auto num_val
                    = helpers::reader::read_float(val.begin(), val.end());
                return std::make_unique<floating>(num_val);
            });

            result.template set<string_type, boolean>(
                [](auto str) -> base::uptr {
                    using std_string = std::basic_string<CharT>;
                    static const std_string bool_values[]
                        = { std_string({ 'f', 'a', 'l', 's', 'e' }),
                            std_string({ 't', 'r', 'u', 'e' }) };
                    auto val = str->value();
                    if (val == bool_values[0]) {
                        return std::make_unique<boolean>(false);
                    } else if (val == bool_values[1]) {
                        return std::make_unique<boolean>(true);
                    }

                    return {};
                });


            return result;
        }

        template <typename T>
        static objects::base::uptr to_string(const T& value)
        {
            stream_type ss;
            ss << value;
            return std::make_unique<string_type>(ss.str());
        }
    };
}}

#endif
