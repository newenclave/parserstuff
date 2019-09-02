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
        info(std::function<object::uptr()> f, std::uintptr_t i)
            : id(i)
            , factory_(std::move(f))
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
            static info sinfo{
                []() { return std::make_unique<T>(); },
                reinterpret_cast<std::uintptr_t>(&localid),
            };
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

    object()
        : info_(info::create<object>())
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

    virtual const char* type_name() const override
    {
        return type_name_.c_str();
    }

    std::unique_ptr<T> create_empty() const
    {
        return object::cast<T>(object::info::create<T>()->create());
    }

private:
    std::string type_name_;
};

#define erules_define_object(type_name)                                        \
    type_name:                                                                 \
public                                                                         \
    typed_object<type_name>

inline bool operator<(const object::info::holder& lh,
                      const object::info::holder& rh)
{
    return lh.type_info()->id < rh.type_info()->id;
}
}

#endif // OBJECTS_H
