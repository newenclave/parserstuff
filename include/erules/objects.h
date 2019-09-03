#ifndef ERULES_OBJECTS_H
#define ERULES_OBJECTS_H

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace erules {

struct object {
public:
    using ptr = object*;
    using cptr = const object*;
    using uptr = std::unique_ptr<object>;
    using sptr = std::shared_ptr<object>;

    class info {
    public:
        class holder {
        public:
            holder(const info* i)
                : info_(i)
            {
            }
            holder(holder&&) = default;
            holder& operator=(holder&&) = default;
            holder(const holder&) = default;
            holder& operator=(const holder&) = default;

            const info* type_info() const
            {
                return info_;
            }
            const info* operator->() const
            {
                return info_;
            }

        private:
            const info* info_;
        };
        info(std::uintptr_t i)
            : id(i)
        {
        }

        object::uptr create() const
        {
            return factory_();
        }

        template <typename T>
        static holder create()
        {
            static_assert(std::is_base_of<object, T>::value,
                          "T must derive from 'object'");
            static std::uintptr_t localid = 0xFFEEBBAA;
            static info sinfo(
                reinterpret_cast<std::uintptr_t>(&localid)
            );
            return holder{ &sinfo };
        }
        std::uintptr_t id = 0;

    private:
        std::function<object::uptr()> factory_;
    };

    object(info::holder inf)
        : info_(std::move(inf))
    {
    }

    virtual ~object() = default;

    template <typename ToT>
    static cptr cast(cptr p)
    {
        static_assert(std::is_base_of<object, ToT>::value,
                      "T must derive from 'object'");
        assert_type<ToT>(p);
        return static_cast<const ToT*>(p);
    }

    template <typename ToT>
    static ptr cast(ptr p)
    {
        static_assert(std::is_base_of<object, ToT>::value,
                      "T must derive from 'object'");
        assert_type<ToT>(p);
        return static_cast<ToT*>(p);
    }

    template <typename ToT>
    static std::unique_ptr<ToT> cast(uptr p)
    {
        static_assert(std::is_base_of<object, ToT>::value,
                      "T must derive from 'object'");
        assert_type<ToT>(p.get());
        return std::make_unique<ToT>(static_cast<ToT>(p.release()));
    }

    template <typename ToT>
    static std::shared_ptr<ToT> cast(sptr p)
    {
        static_assert(std::is_base_of<object, ToT>::value,
                      "T must derive from 'object'");
        assert_type<ToT>(p.get());
        return std::shared_ptr<ToT>(p, static_cast<ToT*>(p.get()));
    }

    std::uintptr_t type_id() const
    {
        return info_->id;
    }

    info::holder type_info() const
    {
        return info_;
    }

    virtual const char* type_name() const
    {
        return "object";
    }

    virtual uptr clone() const = 0;

protected:
    template <typename T>
    static void assert_type(object::cptr p)
    {
        if (p->type_info()->id != info::create<T>()->id) {
            throw std::runtime_error("object is not T");
        }
    }

private:
    const info::holder info_;
};

template <typename T>
class typed_object : public object {
public:
    typed_object(std::string tname)
        : object(info::create<T>())
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

using object_binary_operation
    = std::function<object::uptr(object::ptr, object::ptr)>;
using object_unary_operation = std::function<object::uptr(object::ptr)>;

// clang-format off
#define erules_define_object(type_name) \
    type_name: public typed_object<type_name>

#define erules_define_template_object(type_name, ...) \
    type_name: public typed_object<type_name<__VA_ARGS__> >
// clang-format on

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
    const string_type& value() const
    {
        return value_;
    }
    object::uptr clone() const override
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
    number(std::uint64_t val)
        : super_type(__func__)
    {
    }
    number()
        : super_type(__func__)
    {
    }
    std::int64_t value() const
    {
        return value_;
    }
    object::uptr clone() const override
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
    {
    }
    floating()
        : super_type(__func__)
    {
    }
    double value() const
    {
        return value_;
    }
    object::uptr clone() const override
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
    {
    }
    boolean()
        : super_type(__func__)
    {
    }

    bool value() const
    {
        return value_;
    }

    object::uptr clone() const override
    {
        return std::make_unique<floating>(value());
    }

private:
    bool value_ = 0;
};

inline bool operator<(const object::info::holder& lh,
                      const object::info::holder& rh)
{
    return lh.type_info()->id < rh.type_info()->id;
}
}

#endif // OBJECTS_H
