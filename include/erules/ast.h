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


    template <typename CharT>
    class node : object {
    public:
        using char_type = CharT;
        using string_type = std::basic_string<char_type>;
        virtual ~node() = default;
        using uptr = std::unique_ptr<node>;

        node(object::info::holder info, std::string name)
            : object(std::move(info))
            , name_("ast::" + std::move(name))
        {
        }

        const char* type_name() const override
        {
            return name_.c_str();
        }

    private:
        std::string name_;
    };


    class operations {
    };

}}