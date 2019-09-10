#ifndef ERULES_BASIC_OPERATIONS_OBJECTS_H
#define ERULES_BASIC_OPERATIONS_OBJECTS_H

#include "erules/objects.h"
#include "erules/operations.h"
#include "erules/rule_lexem.h"

namespace erules { namespace operations {

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class binary_operations {
    public:
        using lexem_type = rule_lexem<CharT, LessType>;
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

            return result;
        }
    };

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class unary_operations {
    public:
        using lexem_type = rule_lexem<CharT, LessType>;
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
        using lexem_type = rule_lexem<CharT, LessType>;
        using id_type = typename lexem_type::id_type;
        using string_type = objects::string<CharT>;
        static objects::oprerations::transform get()
        {
            using namespace objects;
            objects::oprerations::transform result;

            //        result.set<ident<CharT>, object>([](auto ptr) {
            //            return std::make_unique<string_type>(ptr->value());
            //        });

            return result;
        }
    };
}}

#endif
