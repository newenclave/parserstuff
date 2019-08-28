#pragma once
#include <functional>
#include <map>
#include <vector>

// Lexemtype:
//    Lexemtype::IdType some integral type
//    Lexemtype::IdType Lexemtype::id() -- call to get Id by intance

template <typename NodeT, typename LexemT>
class parser {
public:
    using node_type = NodeT;
    using lexem_type = LexemT;
    class state {
        friend class parser;
        state(typename std::vector<lexem_type>::const_iterator current,
              typename std::vector<lexem_type>::const_iterator next)
        {
            current_ = current;
            next_ = next;
        }
        typename std::vector<lexem_type>::const_iterator current_;
        typename std::vector<lexem_type>::const_iterator next_;
    };

    using led_call_type = std::function<node_type(node_type)>;
    using nud_call_type = std::function<node_type()>;

    using id_type = typename lexem_type::id_type;
    virtual ~parser() = default;

    parser(std::vector<lexem_type> lexem)
        : lexem_(std::move(lexem))
        , current_(lexem_.begin())
        , next_(lexem_.begin())
    {
        advance();
    }

    template <typename CallT>
    void set_led(id_type id, CallT call)
    {
        leds_[id] = std::move(call);
    }

    template <typename CallT>
    void set_nud(id_type id, CallT call)
    {
        nuds_[id] = std::move(call);
    }

    template <typename CallT>
    void set_default_nud(CallT call)
    {
        default_nud_ = std::move(call);
    }

    template <typename CallT>
    void set_default_led(CallT call)
    {
        default_led_ = std::move(call);
    }

    void set_precedense(id_type id, int value)
    {
        precedenses_[id] = value;
    }

    node_type parse_expression(int p)
    {
        auto nud = nuds_.find(lexem_type::id(current()));
        node_type left;
        if (nud == nuds_.end()) {
            left = default_nud();
        } else {
            left = nud->second();
        }

        if (!left) {
            return nullptr;
        }

        int pp = next_precednse();
        lexem_type pt = next();
        while (p < pp) {
            auto led_call
                = [this](auto left) { return default_led(std::move(left)); };

            auto led = leds_.find(lexem_type::id(pt));
            if (led != leds_.end()) {
                led_call = led->second;
            }

            advance();
            left = led_call(std::move(left));
            if (!left) {
                return left;
            }

            pp = next_precednse();
            pt = next();
        }
        return left;
    }

    void advance()
    {
        current_ = next_;
        if (!next_eof()) {
            ++next_;
        }
    }

    bool expect(id_type id)
    {
        if (!next_eof() && lexem_type::id(next()) == id) {
            advance();
            return true;
        }
        return false;
    }

    bool eof() const
    {
        return current_ == lexem_.cend();
    }

    bool next_eof() const
    {
        return next_ == lexem_.cend();
    }

    lexem_type current() const
    {
        return eof() ? lexem_type{} : *current_;
    }

    lexem_type next() const
    {
        return next_eof() ? lexem_type{} : *next_;
    }

    int current_precednse()
    {
        return itr_precednse(current_);
    }

    int next_precednse()
    {
        return itr_precednse(next_);
    }

    node_type default_nud() const
    {
        return default_nud_ ? default_nud_() : node_type{};
    }

    node_type default_led(node_type left) const
    {
        return default_led_ ? default_led_(std::move(left)) : node_type{};
    }

    state store() const
    {
        return { current_, next_ };
    }

    void restore(state state)
    {
        current_ = state.current_;
        next_ = state.next_;
    }

private:
    int itr_precednse(typename std::vector<lexem_type>::const_iterator itr)
    {
        if (itr == lexem_.cend()) {
            return -1;
        }
        auto found = precedenses_.find(lexem_type::id(*itr));
        return found == precedenses_.end() ? -1 : found->second;
    }

    std::map<id_type, nud_call_type> nuds_;
    std::map<id_type, led_call_type> leds_;

    nud_call_type default_nud_;
    led_call_type default_led_;

    std::map<id_type, int> precedenses_;
    std::vector<lexem_type> lexem_;
    typename std::vector<lexem_type>::const_iterator current_;
    typename std::vector<lexem_type>::const_iterator next_;
};
