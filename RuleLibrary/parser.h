#pragma once
#include <vector>
#include <functional>
#include <map>

// Lexemtype:
//    Lexemtype::IdType some integral type
//    Lexemtype::IdType Lexemtype::id() -- call to get Id by intance

template <typename NodeType, typename LexemType>
class Parser {
public:

	class State {
		friend class Parser;
		State(typename std::vector<LexemType>::const_iterator current, 
			typename std::vector<LexemType>::const_iterator next)
		{
			current_ = current;
			next_ = next;
		}
		typename std::vector<LexemType>::const_iterator current_;
		typename std::vector<LexemType>::const_iterator next_;
	};

	using LedCallType = std::function<NodeType(NodeType)>;
	using NudCallType = std::function<NodeType()>;

	using IdType = typename LexemType::IdType;
	virtual ~Parser() = default;

	Parser(std::vector<LexemType> lexem)
		:lexem_(std::move(lexem))
		,current_(lexem_.begin())
		,next_(lexem_.begin())
	{
		advance();
	}

	template <typename CallT>
	void setLed(IdType id, CallT call)
	{
		leds_[id] = std::move(call);
	}

	template <typename CallT>
	void setNud(IdType id, CallT call)
	{
		nuds_[id] = std::move(call);
	}

	template <typename CallT>
	void setDefaultNud(CallT call) 
	{
		default_nud_ = std::move(call);
	}

	template <typename CallT>
	void setDefaultLed(CallT call) 
	{
		default_led_ = std::move(call);
	}

	void setPrecedense(IdType id, int value)
	{
		precedenses_[id] = value;
	}

	NodeType parseExpression(int p)
	{
		auto nud = nuds_.find(LexemType::id(current()));
		NodeType left;
		if(nud == nuds_.end()) 
		{
			left = defaultNud();
		} 
		else 
		{
			left = nud->second();
		}

		if(!left) 
		{
			return nullptr;
		}

		int pp = nextPrecednse();
		LexemType pt = next();
		while(p < pp) 
		{
			auto led_call = [this](auto left) { 
				return defaultLed(std::move(left)); 
			};

			auto led = leds_.find(LexemType::id(pt));
			if(led != leds_.end()) 
			{
				led_call = led->second;
			} 

			advance();
			left = led_call(std::move(left));
			if(!left) 
			{
				return left;
			}

			pp = nextPrecednse();
			pt = next();
		}
		return left;
	}

	void advance()
	{
		current_ = next_;
		if(!nextEof()) 
		{
			++next_;
		}
	}

	bool expect(IdType id) 
	{
		if(!nextEof() && LexemType::id(next()) == id)
		{
			advance();
			return true;
		}
		return false;
	}

	bool eof() const 
	{
		return current_ == lexem_.cend();
	}

	bool nextEof() const 
	{
		return next_ == lexem_.cend();
	}

	LexemType current() const 
	{
		return eof() ? LexemType {} : *current_;
	}

	LexemType next() const 
	{
		return nextEof() ? LexemType {} : *next_;
	}

	int currentPrecednse()
	{
		return itrPrecednse(current_);
	}

	int nextPrecednse()
	{
		return itrPrecednse(next_);
	}

	NodeType defaultNud() const 
	{
		return default_nud_ ? default_nud_() : NodeType{};
	}

	NodeType defaultLed(NodeType left) const 
	{
		return default_led_ ? default_led_(std::move(left)) : NodeType{};
	}

	State store() const 
	{
		return {current_, next_};
	}

	void restore(State state) 
	{
		current_ = state.current_;
		next_ = state.next_;
	}

private:

	int itrPrecednse(typename std::vector<LexemType>::const_iterator itr)
	{
		if(itr == lexem_.cend()) 
		{
			return -1;
		}
		auto found = precedenses_.find(LexemType::id(*itr));
		return found == precedenses_.end() ? -1 : found->second;
	}

	std::map<IdType, NudCallType> nuds_;
	std::map<IdType, LedCallType> leds_;

	NudCallType default_nud_;
	LedCallType default_led_;

	std::map<IdType, int> precedenses_;
	std::vector<LexemType> lexem_;
	typename std::vector<LexemType>::const_iterator current_;
	typename std::vector<LexemType>::const_iterator next_;
};

