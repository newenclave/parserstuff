#ifndef OBJECTS_H
#define OBJECTS_H

#include <cstdlib>
#include <cstdint>
#include <memory>

namespace erules {

#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <unordered_map>

template <typename T>
struct object_id
{
    static std::uintptr_t id()
    {
        static std::uintptr_t data = 0;
        return reinterpret_cast<std::uintptr_t>(&data);
    }
};

struct object {
    using uptr = std::unique_ptr<object>;
    using sptr = std::shared_ptr<object>;
    using ptr = object *;
    using cptr = const object *;

    struct type_info {};

    template <typename T>
    static void assert_type(object::cptr val)
    {
        if(val->id() != object_id<T>::id()) {
            throw std::runtime_error(std::string("Object has a wrong type"));
        }
    }

    template <typename T>
    static const T * cast(object::cptr from)
    {
        static_assert(std::is_base_of<object, T>::value, "Target class must derive from 'object'");
        assert_type<T>(from);
        return static_cast<const T *>(from);
    }

    template <typename T>
    static T * cast(object::ptr from)
    {
        static_assert(std::is_base_of<object, T>::value, "Target class must derive from 'object'");
        assert_type<T>(from);
        return static_cast<T *>(from);
    }

    template <typename T>
    static std::unique_ptr<T> cast(object::uptr from)
    {
        static_assert(std::is_base_of<object, T>::value, "Target class must derive from 'object'");
        assert_type<T>(from.get());
        return std::unique_ptr<T>(static_cast<T *>(from.release()));
    }

    template <typename T>
    static std::shared_ptr<T> cast(object::sptr from)
    {
        static_assert(std::is_base_of<object, T>::value, "Target class must derive from 'object'");
        assert_type<T>(from.get());
        return std::shared_ptr<T>(from, static_cast<T *>(from.get()));
    }

    virtual ~object() = default;
    virtual std::uintptr_t id() const = 0;
    virtual uptr clone() const = 0;

    static std::map<std::uintptr_t, type_info> &infos()
    {
        static std::map<std::uintptr_t, type_info> infos_;
        return infos_;
    }
};

template<typename T>
struct id_object : public object
{
    id_object()
    {
        object::infos()[id()] = object::type_info{};
    }
    std::uintptr_t id() const override
    {
        static_assert(std::is_base_of<object, T>::value, "T must derive from 'object'");
        return object_id<typename std::remove_reference<T>::type>::id();
    }
};

#define object_class(name) name: public id_object<name>

struct object_class(string) {
    string() = default;
    string(std::string val) : value(std::move(val)) {}
    object::uptr clone() const override
    {
        return std::make_unique<string>(value);
    }
    std::string value;
};

struct object_class(integer) {
    integer() = default;
    integer(std::int64_t val) : value(val) {}
    std::int64_t value = 0;
    object::uptr clone() const override
    {
        return std::make_unique<integer>(value);
    }
};

struct object_class(floating) {
    floating() = default;
    floating(double val): value(val) {}
    double value = 0.0;
    object::uptr clone() const override
    {
        return std::make_unique<floating>(value);
    }
};

struct object_class(boolean) {
    boolean() = default;
    boolean(bool val) : value(val) {}
    bool value = false;
    object::uptr clone() const override
    {
        return std::make_unique<boolean>(value);
    }
};

#undef object_class

struct operation {
    enum type {
        NONE,
        ADD,
        SUB,
        MUL,
        LT,
        NOT,
        IN
    };
    operation (type ot) : oper_type(ot) {};
    operation (std::string ot) : name(ot) {};
    type oper_type = NONE;
    std::string name;
};

operation operator ""_op (const char *data, std::size_t)
{
    return operation(data);
}

bool operator < (const operation &lh, const operation &rh)
{
    return std::make_tuple(lh.oper_type, lh.name) <
        std::make_tuple(rh.oper_type, rh.name);
}

using binary_call = std::function<object::uptr(object::ptr, object::ptr)>;
using unary_call = std::function<object::uptr(object::ptr)>;
using transform_call = std::function<object::uptr(object::ptr)>;

using bin_operation_tuple = std::tuple<operation, std::uintptr_t, std::uintptr_t>;
using un_operation_tuple = std::tuple<operation, std::uintptr_t>;
using transform_operation_tuple = std::tuple<std::uintptr_t, std::uintptr_t>;

using bin_operation_map = std::map<bin_operation_tuple, binary_call>;
using un_operation_map = std::map<un_operation_tuple, unary_call>;
using transform_operation_map = std::map<transform_operation_tuple, transform_call>;

class operations_holder {

    template <typename LeftT, typename RightT, typename CallT>
    static auto create_bin_operation(CallT call)
    {
        return [call](object::ptr left, object::ptr right) {
            return call(object::cast<LeftT>(left), object::cast<RightT>(right));
        };
    }

    template <typename ValT, typename CallT>
    static auto create_un_operation(CallT call)
    {
        return [call](object::ptr value) {
            return call(object::cast<ValT>(value));
        };
    }

    template <typename FromT, typename CallT>
    static auto create_cast_operation(CallT call)
    {
        return [call](object::ptr value) {
            return call(object::cast<FromT>(value));
        };
    }

public:

    template <typename LObjT, typename RObjT, typename CallT>
    void add_bin_operation(operation op, CallT call, bool add_invert = false)
    {
        auto bin_call = create_bin_operation<LObjT, RObjT>(std::move(call));

        if(add_invert && object_id<LObjT>::id() != object_id<RObjT>::id()) {
            bin_map_[std::make_tuple(op, object_id<RObjT>::id(), object_id<LObjT>::id())] =
                [bin_call](auto left, auto right) {
                    return bin_call(right, left);
                };
        }
        bin_map_[std::make_tuple(op, object_id<LObjT>::id(), object_id<RObjT>::id())] = std::move(bin_call);
    }

    template <typename ValueT, typename CallT>
    void add_unary_operation(operation op, CallT call)
    {
        un_map_[std::make_tuple(op, object_id<ValueT>::id())] = create_un_operation<ValueT>(std::move(call));
    }

    template <typename FromT, typename ToT, typename CallT>
    void add_cast_operation(CallT call)
    {
        trans_map_[std::make_tuple(object_id<FromT>::id(), object_id<ToT>::id())] =
            create_un_operation<FromT>(std::move(call));
    }

    object::uptr bin_call(operation op, object::ptr left, object::ptr right)
    {
        auto operation = bin_map_.find(std::make_tuple(op, left->id(), right->id()));
        if(operation != bin_map_.end()) {
            return operation->second(left, right);
        }
        return nullptr;
    }

    template<typename LeftT, typename RightT>
    object::uptr bin_call(operation op, std::unique_ptr<LeftT> &left, std::unique_ptr<RightT> &right)
    {
        return bin_call(op, left.get(), right.get());
    }

    object::uptr unary_call(operation op, object::ptr value)
    {
        auto operation = un_map_.find(std::make_tuple(op, value->id()));
        if(operation != un_map_.end()) {
            return operation->second(value);
        }
        return nullptr;
    }

    template<typename ValueT>
    object::uptr unary_call(operation op, std::unique_ptr<ValueT> &value)
    {
        return unary_call(op, value.get());
    }

    template<typename ToT>
    std::unique_ptr<ToT> transform_call(object::ptr obj)
    {
        if(object_id<ToT>::id() == obj->id()) {
            return object::cast<ToT>(obj->clone());
        }
        auto operation = trans_map_.find(std::make_tuple(obj->id(), object_id<ToT>::id()));
        if(operation != trans_map_.end()) {
            return object::cast<ToT>(operation->second(obj));
        }
        return nullptr;
    }

    template<typename ToT, typename FromT>
    std::unique_ptr<ToT> transform_call(std::unique_ptr<FromT> &obj)
    {
        return transform_call<ToT>(obj.get());
    }

private:
    bin_operation_map bin_map_;
    un_operation_map un_map_;
    transform_operation_map trans_map_;
};

}

#endif // OBJECTS_H
