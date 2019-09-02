#pragma once
#include <memory>
#include <string>

#include "erules/constants.h"
#include "erules/helpers.h"
#include "erules/objects.h"

namespace erules { namespace ast {

    enum node_type : int {
        NONE,
        NUMBER,
        FLOAT,
        BOOLEAN,
        STRING,
        OPERATOR,
        PREFIX_OPERATOR,
        SUFFIX_OPERATOR,
    };

    template <typename CharT, typename AstT>
    class node : public object::typed_object<AstT> {
    public:
        using super_type = object::typed_object<AstT>;
        using char_type = CharT;
        using string_type = std::basic_string<char_type>;
        virtual ~node() = default;
        using uptr = std::unique_ptr<node>;

        node(std::string name)
            : super_type(std::move(name))
        {
        }

    private:
        node_type type_;
    };

#define erules_define_ast_object(char_type, type_name)                         \
    type_name:                                                                 \
public                                                                         \
    node<char_type, type_name>


}}