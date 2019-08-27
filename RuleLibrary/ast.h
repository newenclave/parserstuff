#pragma once
#include <string>
#include <memory>
#include "constants.h"

namespace ast {

	struct node {
		virtual ~node() = default;
		using uptr = std::unique_ptr<node>;
		node(constants::Token t)
			:token(t)
		{}
		virtual std::string to_string() const 
		{
			return "None!";
		}
		constants::Token token;
	};

	struct ident: public node {

		ident(std::string val)
			:node(constants::Token::IDENT)
			,value(std::move(val))
		{}

		std::string to_string() const override
		{
			return value;
		}

		std::string value;
	};

	struct string: public node {
		string(std::string val)
			:node(constants::Token::STRING)
			,value(std::move(val))
		{}

		std::string to_string() const override
		{
			return "\"" + value + "\"";
		}
		std::string value;
	};

	struct number: public node {
		number(std::int64_t val)
			:node(constants::Token::NUMBER)
			,value(val)
		{}

		std::string to_string() const override
		{
			return std::to_string(value);
		}
		std::int64_t value;
	};

	struct logic_operator: public node {

		logic_operator(node::uptr l, node::uptr r, std::string o, constants::Token t)
			:node(t)
			,left(std::move(l))
			,right(std::move(r))
			,oper(std::move(o))
		{}

		std::string to_string() const override
		{
			return "(" + left->to_string() + " " + oper + " " + right->to_string() + ")";
		}
		node::uptr left;
		node::uptr right;
		std::string oper;
	};

	struct LogicNotOperator: public node {

		LogicNotOperator(node::uptr r, std::string o, constants::Token t)
			:node(t)
			,right(std::move(r))
			,oper(std::move(o))
		{}

		std::string to_string() const override
		{
			return "(" + oper + " " + right->to_string() + ")";
		}
		node::uptr right;
		std::string oper;
	};

	struct binary_operator: public node {

		binary_operator(node::uptr l, node::uptr r, std::string o, constants::Token t)
			:node(t)
			,left(std::move(l))
			,right(std::move(r))
			,oper(std::move(o))
		{}

		std::string to_string() const override
		{
			return "(" + left->to_string() + " " + oper + " " + right->to_string() + ")";
		}
		node::uptr left;
		node::uptr right;
		std::string oper;
	};
}
