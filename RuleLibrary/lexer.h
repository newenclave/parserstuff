#pragma once
#include "trie.h"
#include <functional>

template <typename ConteinerType, typename StateTrait>
class lexer {
public:
    using value_type = typename ConteinerType::value_type;
    using iterator = typename ConteinerType::iterator;
    using const_iterator = typename ConteinerType::const_iterator;

    using state_type = StateTrait;
    using less_type = typename state_type::LessType;

    class internal_state {
    public:
        internal_state() = default;
        internal_state(internal_state&&) = default;
        internal_state& operator=(internal_state&&) = default;
        internal_state(const internal_state&) = default;
        internal_state& operator=(const internal_state&) = default;

        internal_state(const_iterator b, const_iterator e)
            : _begin()
            , _end(e)
        {
        }
        const_iterator begin() const
        {
            return _begin;
        }
        const_iterator end() const
        {
            return _end;
        }

    private:
        const_iterator _begin;
        const_iterator _end;
    };

    using create_state_factory = std::function<state_type()>;
    using token_state_factory
        = std::function<state_type(state_type, internal_state)>;
    using token_trie = trie<value_type, token_state_factory, less_type>;

    ~lexer() = default;

    lexer(create_state_factory factory,
          token_state_factory default_factory = nullptr)
        : state_factory_(std::move(factory))
        , default_factory_(get_default_factory(default_factory))
    {
    }

    lexer(lexer&&) = default;
    lexer& operator=(lexer&&) = default;
    lexer(const lexer&) = default;
    lexer& operator=(const lexer&) = default;

    lexer& add_factory(const ConteinerType& input, token_state_factory factory)
    {
        trie_.set(std::begin(input), std::end(input), std::move(factory));
        return *this;
    }

    lexer& set_default_factory(token_state_factory factory)
    {
        default_factory_ = std::move(factory);
        return *this;
    }

    state_type next(const_iterator begin, const_iterator end)
    {
        auto state = state_factory_();
        auto next_getter = trie_.get(begin, end, true);
        if (next_getter) {
            return (*next_getter)(std::move(state),
                                  { next_getter.begin(), next_getter.end() });
        }
        return default_factory_(std::move(state),
                                { next_getter.begin(), next_getter.end() });
    }

private:
    static token_state_factory get_default_factory(token_state_factory val)
    {
        return val ? val : [](auto state, auto) { return state; };
    }

    token_trie trie_;
    create_state_factory state_factory_;
    token_state_factory default_factory_
        = [](auto state, auto) { return state; };
};
