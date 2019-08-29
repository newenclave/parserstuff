#pragma once
#include "constants.h"
#include <memory>
#include <string>

namespace ast {

	enum ast_type: int {
		NONE,
		NUMBER,
		FLOAT,
		BOOLEAN,
		STRING,
		OPERATOR,
		PREFIX_OPERATOR,
	};

template <typename CharT>
class node {
public:
	using char_type = CharT;
	using string_type = std::basic_string<char_type>;
    virtual ~node() = default;
    using uptr = std::unique_ptr<node>;
    
	node(ast_type t)
        : type_(t)
    {
    }

    virtual string_type to_string() const
    {
		return {'N', 'u', 'l', 'l', '\0'};
    }

	ast_type type() const
	{
		return type_;
	}

private:

	ast_type type_;
};

template <typename CharT>
class string : public node<CharT>
{
public:
	using string_type = typename node<CharT>::string_type;
	string(string_type val)
		:node(ast_type::STRING)
		,value(std::move(val))
	{}
	const string_type &value() const 
	{
		return value_;
	}
	string_type to_string() const override
	{
		return '"' + value_ + '"';
	}
private:
	string_type value_;
};

template <typename CharT>
class floating : public node<CharT>
{
public:
	using string_type = typename node<CharT>::string_type;
	floating(double val)
		:node(ast_type::FLOAT)
		,value(val)
	{}
	
	double value() const 
	{
		return value_;
	}
	
	string_type to_string() const override
	{
		std::basic_stringstream<CharT> ss;
		ss << value_;
		return ss.str();
	}
private:
	double value_;
};

template <typename CharT>
class number : public node<CharT>
{
public:
	using string_type = typename node<CharT>::string_type;
	number(std::int64_t val)
		:node(ast_type::NUMBER)
		,value(val)
	{}

	double value() const 
	{
		return value_;
	}

	string_type to_string() const override
	{
		std::basic_stringstream<CharT> ss;
		ss << value_;
		return ss.str();
	}
private:
	std::int64_t value_;
};

}
