#ifndef ERULES_OBJECTS_H
#define ERULES_OBJECTS_H

#include <map>
#include <string>
#include <vector>

#include "erules/objects/base.h"

namespace erules { namespace objects {

    template <typename T>
    class typed_object : public base {
    public:
        typed_object(std::string tname)
            : base(info::create<T>())
            , type_name_(std::move(tname))
        {
        }
        typed_object(typed_object&&) = default;
        typed_object& operator=(typed_object&&) = default;
        typed_object(const typed_object&) = default;
        typed_object& operator=(const typed_object&) = default;

        const char* type_name() const override
        {
            return type_name_.c_str();
        }

    private:
        std::string type_name_;
    };

// clang-format off
#define erules_define_object(type_name) \
    type_name: public typed_object<type_name>

#define erules_define_template_object(type_name, ...) \
    type_name: public typed_object<type_name<__VA_ARGS__> >
    // clang-format on

    /// defining objects
    template <typename CharT>
    class erules_define_template_object(string, CharT)
    {
    public:
        using super_type = typed_object<string<CharT>>;
        using string_type = std::basic_string<CharT>;

        string(string_type val)
            : super_type(__func__)
            , value_(std::move(val))
        {
        }

        string()
            : super_type(__func__)
        {
        }

        void set_value(string_type val)
        {
            value_ = std::move(val);
        }

        const string_type& value() const
        {
            return value_;
        }
        base::uptr clone() const override
        {
            return std::make_unique<string<CharT>>(value());
        }

    private:
        string_type value_;
    };

    template <typename CharT>
    class erules_define_template_object(ident, CharT)
    {
    public:
        using super_type = typed_object<string<CharT>>;
        using string_type = std::basic_string<CharT>;

        ident(string_type val)
            : super_type(__func__)
            , value_(std::move(val))
        {
        }
        ident()
            : super_type(__func__)
        {
        }
        void set_value(string_type val)
        {
            value_ = std::move(val);
        }
        const string_type& value() const
        {
            return value_;
        }
        base::uptr clone() const override
        {
            return std::make_unique<string<CharT>>(value());
        }

    private:
        string_type value_;
    };

    class erules_define_object(number)
    {
        using super_type = typed_object<number>;

    public:
        number(std::int64_t val)
            : super_type(__func__)
            , value_(val)
        {
        }
        number()
            : super_type(__func__)
        {
        }

        void set_value(std::int64_t val)
        {
            value_ = val;
        }

        std::int64_t value() const
        {
            return value_;
        }
        base::uptr clone() const override
        {
            return std::make_unique<number>(value());
        }

    private:
        std::int64_t value_ = 0;
    };

    class erules_define_object(floating)
    {
        using super_type = typed_object<floating>;

    public:
        floating(double val)
            : super_type(__func__)
            , value_(val)
        {
        }
        floating()
            : super_type(__func__)
        {
        }

        void set_value(double val)
        {
            value_ = val;
        }

        double value() const
        {
            return value_;
        }
        base::uptr clone() const override
        {
            return std::make_unique<floating>(value());
        }

    private:
        double value_ = 0;
    };

    class erules_define_object(boolean)
    {
        using super_type = typed_object<boolean>;

    public:
        boolean(bool val)
            : super_type(__func__)
            , value_(val)
        {
        }
        boolean()
            : super_type(__func__)
        {
        }

        void set_value(bool val)
        {
            value_ = val;
        }

        bool value() const
        {
            return value_;
        }

        base::uptr clone() const override
        {
            return std::make_unique<floating>(value());
        }

    private:
        bool value_ = false;
    };

    class erules_define_object(pair)
    {
        using super_type = typed_object<pair>;

    public:
        pair(base::uptr lft, base::uptr rght)
            : super_type(__func__)
            , left_(std::move(lft))
            , right_(std::move(rght))
        {
        }
        pair()
            : super_type(__func__)
        {
        }

        void set_left(base::uptr val)
        {
            left_ = std::move(val);
        }

        void set_right(base::uptr val)
        {
            right_ = std::move(val);
        }

        base::uptr& left()
        {
            return left_;
        }
        base::uptr& right()
        {
            return right_;
        }

        base::uptr clone() const override
        {
            return std::make_unique<pair>(left_->clone(), right_->clone());
        }

    private:
        base::uptr left_;
        base::uptr right_;
    };

    class erules_define_object(array)
    {
        using super_type = typed_object<array>;

    public:
        array(std::vector<base::uptr> val)
            : super_type(__func__)
            , value_(std::move(val))
        {
        }

        array()
            : super_type(__func__)
        {
        }

        void set_value(std::vector<base::uptr> val)
        {
            value_ = std::move(val);
        }

        std::vector<base::uptr>& value()
        {
            return value_;
        }
        base::uptr clone() const override
        {
            std::vector<base::uptr> tmp;
            for (auto& o : value_) {
                tmp.emplace_back(o->clone());
            }
            return std::make_unique<array>(std::move(tmp));
        }

    private:
        std::vector<base::uptr> value_;
    };

    template <typename ObjT>
    class erules_define_template_object(interval, ObjT)
    {
        using super_type = typed_object<interval<ObjT>>;

    public:
        interval(std::unique_ptr<ObjT> lft, std::unique_ptr<ObjT> rght)
            : super_type(__func__)
            , left_(std::move(lft))
            , right_(std::move(rght))
        {
        }

        void set_left(std::unique_ptr<ObjT> val)
        {
            left_ = std::move(val);
        }

        void set_right(std::unique_ptr<ObjT> val)
        {
            right_ = std::move(val);
        }

        std::unique_ptr<ObjT>& left()
        {
            return left_;
        }

        std::unique_ptr<ObjT>& right()
        {
            return right_;
        }

        base::uptr clone() const override
        {
            using this_type = interval<ObjT>;
            return std::make_unique<this_type>(
                base::cast<ObjT>(left_->clone()),
                base::cast<ObjT>(right_->clone()));
        }

    private:
        std::unique_ptr<ObjT> left_;
        std::unique_ptr<ObjT> right_;
    };


}}

#endif // OBJECTS_H
