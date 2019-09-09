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
        info(std::uintptr_t i, std::function<object::uptr()> factory)
            : id(i)
            , factory_(std::move(factory))
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
            static info sinfo(reinterpret_cast<std::uintptr_t>(&localid),
                              []() { return std::make_unique<T>(); });
            return holder { &sinfo };
        }

        template <>
        static holder create<object>()
        {
            static std::uintptr_t localid = 0xFFEEBBAA;
            static info sinfo(reinterpret_cast<std::uintptr_t>(&localid),
                              nullptr);
            return holder { &sinfo };
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
    static const ToT* cast(cptr p)
    {
        static_assert(std::is_base_of<object, ToT>::value,
                      "T must derive from 'object'");
        assert_type<ToT>(p);
        return static_cast<const ToT*>(p);
    }

    template <typename ToT>
    static ToT* cast(ptr p)
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
        return std::unique_ptr<ToT>(static_cast<ToT*>(p.release()));
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

// clang-format off
#define erules_define_object(type_name) \
    type_name: public typed_object<type_name>

#define erules_define_template_object(type_name, ...) \
    type_name: public typed_object<type_name<__VA_ARGS__> >
// clang-format on

template <typename IdType>
class object_binary_operations {
public:
    using id_type = IdType;

    using function_type = std::function<object::uptr(object::ptr, object::ptr)>;
    using index_type
        = std::tuple<id_type, object::info::holder, object::info::holder>;
    using map_type = std::map<index_type, function_type>;

private:
    template <typename LeftT, typename RightT, typename CallT>
    static function_type create_call(CallT call)
    {
        return [call](auto left, auto right) {
            return call(object::cast<LeftT>(left), object::cast<RightT>(right));
        };
    }

    template <typename LeftT, typename RightT, typename CallT>
    static function_type create_reverse_call(CallT call)
    {
        return [call](auto left, auto right) {
            return call(object::cast<LeftT>(right), object::cast<RightT>(left));
        };
    }

public:
    template <typename LeftT, typename RightT, typename CallT>
    void set(id_type op, CallT call, bool add_revert = false)
    {
        if (!std::is_same<LeftT, RightT>::value && add_revert) {
            set_impl(op, object::info::create<RightT>(),
                     object::info::create<LeftT>(),
                     create_reverse_call<LeftT, RightT>(call));
        }
        set_impl(op, object::info::create<LeftT>(),
                 object::info::create<RightT>(),
                 create_call<LeftT, RightT, CallT>(std::move(call)));
    }

    object::uptr call(id_type op, object::ptr left, object::ptr right)
    {
        if (auto call = get(op, left, right)) {
            return call(left, right);
        }
        return {};
    }

    template <typename TargetObj>
    std::unique_ptr<TargetObj> call_cast(id_type op, object::ptr left,
                                         object::ptr right)
    {
        return object::cast<TargetObj>(call(op, left, right));
    }

    function_type get(id_type op, object::ptr left, object::ptr right)
    {
        auto id = std::make_tuple(op, left->type_info(), right->type_info());
        auto find = bin_map_.find(id);
        if (find != bin_map_.end()) {
            return find->second;
        }
        return {};
    }

    template <typename LeftT, typename RightT>
    function_type get(id_type op)
    {
        auto id = std::make_tuple(op, object::info::create<LeftT>(),
                                  object::info::create<RightT>());
        auto find = bin_map_.find(id);
        if (find != bin_map_.end()) {
            return find->second;
        }
        return {};
    }

private:
    void set_impl(id_type op, object::info::holder left_type,
                  object::info::holder right_type, function_type call)
    {
        bin_map_[std::make_tuple(op, left_type, right_type)] = std::move(call);
    }

    map_type bin_map_;
};

template <typename IdType>
class object_unary_operations {
public:
    using id_type = IdType;
    using function_type = std::function<object::uptr(object::ptr)>;
    using index_type = std::tuple<id_type, object::info::holder>;
    using map_type = std::map<index_type, function_type>;

private:
    template <typename ValueT, typename CallT>
    static function_type create_call(CallT call)
    {
        return [call](auto obj) { return call(object::cast<ValueT>(obj)); };
    }

public:
    template <typename ValueT, typename CallT>
    void set(id_type op, CallT call)
    {
        set_impl(op, object::info::create<ValueT>(),
                 create_call<ValueT, CallT>(std::move(call)));
    }

    object::uptr call(id_type op, object::ptr value)
    {
        if (auto call = get(op, value)) {
            return call(value);
        }
        return {};
    }

    template <typename TargetObj>
    std::unique_ptr<TargetObj> call_cast(id_type op, object::ptr val)
    {
        return object::cast<TargetObj>(call(op, val));
    }

    function_type get(id_type op, object::ptr value)
    {
        auto id = std::make_tuple(op, value->type_info());
        auto find = un_map_.find(id);
        if (find != un_map_.end()) {
            return find->second;
        }
        return {};
    }

    template <typename ValueT>
    function_type get(id_type op)
    {
        auto id = std::make_tuple(op, object::info::create<ValueT>());
        auto find = un_map_.find(id);
        if (find != un_map_.end()) {
            return find->second;
        }
        return {};
    }

private:
    void set_impl(id_type op, object::info::holder value_type,
                  function_type call)
    {
        un_map_[std::make_tuple(op, value_type)] = std::move(call);
    }

private:
    map_type un_map_;
};

class object_transform_operations {
public:
    using function_type = std::function<object::uptr(object::ptr)>;
    using index_type = std::tuple<object::info::holder, object::info::holder>;
    using map_type = std::map<index_type, function_type>;

private:
    template <typename ValueT, typename CallT>
    static function_type create_unary_call(CallT call)
    {
        return [call](auto obj) { return call(object::cast<ValueT>(obj)); };
    }

public:
    template <typename FromT, typename ToT, typename CallT>
    void set(CallT call)
    {
        set_impl(object::info::create<FromT>(), object::info::create<ToT>(),
                 create_unary_call<FromT>(std::move(call)));
    }

    template <typename ToT>
    std::unique_ptr<ToT> call(object::ptr value)
    {
        auto id
            = std::make_tuple(value->type_info(), object::info::create<ToT>());
        auto find = trans_map_.find(id);
        if (find != trans_map_.end()) {
            return object::cast<ToT>(find->second(value));
        }
        return {};
    }

    template <typename ToT>
    std::function<std::unique_ptr<ToT>(object::ptr)> get(object::ptr value)
    {
        auto id
            = std::make_tuple(value->type_info(), object::info::create<ToT>());
        auto find = trans_map_.find(id);
        if (find != trans_map_.end()) {
            auto call = find->second;
            return
                [call](auto value) { return object::cast<ToT>(call(value)); };
        }
        return {};
    }

    template <typename FromT, typename ToT>
    std::function<std::unique_ptr<ToT>(object::ptr)> get()
    {
        auto id = std::make_tuple(object::info::create<FromT>(),
                                  object::info::create<ToT>());
        auto find = trans_map_.find(id);
        if (find != trans_map_.end()) {
            auto call = find->second;
            return
                [call](auto value) { return object::cast<ToT>(call(value)); };
        }
        return {};
    }

private:
    void set_impl(object::info::holder from_type, object::info::holder to_type,
                  function_type call)
    {
        trans_map_[std::make_tuple(from_type, to_type)] = std::move(call);
    }
    map_type trans_map_;
};

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

    object::uptr clone() const override
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
    pair(object::uptr lft, object::uptr rght)
        : super_type(__func__)
        , left_(std::move(lft))
        , right_(std::move(rght))
    {
    }
    pair()
        : super_type(__func__)
    {
    }

    void set_left(object::uptr val)
    {
        left_ = std::move(val);
    }

    void set_right(object::uptr val)
    {
        right_ = std::move(val);
    }

    object::uptr& left()
    {
        return left_;
    }
    object::uptr& right()
    {
        return right_;
    }

    object::uptr clone() const override
    {
        return std::make_unique<pair>(left_->clone(), right_->clone());
    }

private:
    object::uptr left_;
    object::uptr right_;
};

class erules_define_object(array)
{
    using super_type = typed_object<array>;

public:
    array(std::vector<object::uptr> val)
        : super_type(__func__)
        , value_(std::move(val))
    {
    }

    array()
        : super_type(__func__)
    {
    }

    void set_value(std::vector<object::uptr> val)
    {
        value_ = std::move(val);
    }

    std::vector<object::uptr>& value()
    {
        return value_;
    }
    object::uptr clone() const override
    {
        std::vector<object::uptr> tmp;
        for (auto& o : value_) {
            tmp.emplace_back(o->clone());
        }
        return std::make_unique<array>(std::move(tmp));
    }

private:
    std::vector<object::uptr> value_;
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

    object::uptr clone() const override
    {
        using this_type = interval<ObjT>;
        return std::make_unique<this_type>(object::cast<ObjT>(left_->clone()),
                                           object::cast<ObjT>(right_->clone()));
    }

private:
    std::unique_ptr<ObjT> left_;
    std::unique_ptr<ObjT> right_;
};

inline bool operator<(const object::info::holder& lh,
                      const object::info::holder& rh)
{
    return lh.type_info()->id < rh.type_info()->id;
}
}

#endif // OBJECTS_H
