#pragma once
#include <string>
#include <algorithm>

namespace helpers {

	class reader 
	{
	public:

		template <typename CharT>
		static bool is_space(CharT c)
		{
			switch (c)
			{
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				return true;
			}
			return false;
		}

		template <typename Itr>
		static Itr skip_spaces(Itr begin, Itr end)
		{
			return std::find_if_not(begin, end, [](auto c){ return is_space(c); });
		}

		template <typename Itr, typename Itr2>
		static bool begins_with(Itr bTest, Itr eTest, Itr2 bVal, Itr2 eVal)
		{
			for(; (bVal!=eVal) && (bTest!=eTest) && (*bTest == *bVal); ++bVal, ++bTest)
			{}
			return bVal == eVal;
		}

		template <typename Itr, typename ContaiterT>
		static bool begins_with(Itr bTest, Itr eTest, const ContaiterT &container)
		{
			return begins_with(bTest, eTest, std::begin(container), std::end(container));
		}

		template <typename ItrT, typename ContainerT>
		static auto read_string(ItrT begin, ItrT end, const ContainerT &stop) 
			-> std::basic_string<typename ItrT::value_type>
		{
			using string_type = std::basic_string<typename ItrT::value_type>;
			string_type res;
			for( ; (begin != end) && !begins_with(begin, end, stop); ++begin ) {

				auto next = std::next(begin);

				if( *begin == '\\' && next != end ) {
					if( stop[0] == *next ) {
						res.push_back(stop[0]);
					} else {
						switch (*next) {
						case 'n':
							res.push_back('\n');
							break;
						case 'r':
							res.push_back('\r');
							break;
						case 't':
							res.push_back('\t');
							break;
						case '\\':
							res.push_back('\\');
							break;
						case '"':
							res.push_back('"');
							break;
						case '\'':
							res.push_back('\'');
							break;
						case '0':
							res.push_back('\0');
							break;
						default:
							res.push_back('\\');
							res.push_back(*next);
							break;
						}
					}
					begin = next;
					if( begin == end ) {
						break;
					}
				} else {
					res.push_back( *begin );
				}
			}
			if( begin != end ) {
				++begin;
			}
			return res;
		}
	};
}
