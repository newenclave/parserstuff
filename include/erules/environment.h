#pragma once

#include "erules/objects.h"

namespace erules {
	template <typename CharT, typename LessT = std::less<CharT> >
	class environment {
	public:
		using string_type = std::basic_string<CHarT>;
		virtual ~environment() = default;
		object::uptr get_ident(const string_type &name) = 0; 
		void set_ident(const string_type &name, object::uptr value) = 0; 
	};
}
