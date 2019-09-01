#ifndef ERULES_OBJECTS_H
#define ERULES_OBJECTS_H

#include <cstdlib>
#include <cstdint>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <unordered_map>

namespace erules {

    struct object {
    public:

        using ptr = object *;
        using cptr = const object *;
        using uptr = std::unique_ptr<object>;
        using sptr = std::shared_ptr<object>;

        class info {
        public:
            class holder {
            public:
                holder(const info *i)
                    : info_(i)
                {}
                holder(holder &&) = default;
                holder& operator = (holder &&) = default;
                holder(const holder&) = default;
                holder& operator = (const holder &) = default;

                const info *get_info() const
                {
                    return info_;
                }
                const info *operator -> () const
                {
                    return info_;
                }
            private:
                const info *info_;
            };
            info(std::function<object::uptr()> f,
                 std::uintptr_t i)
                : id(i)
                , factory_(std::move(f))
            {}

            object::uptr create() const
            {
                return factory_();
            }

            template <typename T>
            static holder create()
            {
                static_assert (std::is_base_of<object, T>::value,
                               "T must derive from 'object'");
                static std::uintptr_t localid = 0xFFEEBBAA;
                static info sinfo{
                    [](){
                        return std::make_unique<T>();
                    },
                    reinterpret_cast<std::uintptr_t>(&localid),
                };
                return holder {&sinfo};
            }
            std::uint64_t id = 0;
        private:
            std::function<object::uptr()> factory_;
        };

        object(info::holder inf)
            :info_(std::move(inf))
        {}

        object()
            :info_(info::create<object>())
        {}

        virtual ~object() = default;

        const info::holder &get_info() const
        {
            return info_;
        }

        template <typename ToT>
        static cptr cast(cptr p)
        {
            static_assert (std::is_base_of<object, ToT>::value,
                           "T must derive from 'object'");
            assert_type<ToT>(p);
            return static_cast<const ToT *>(p);
        }

        template <typename ToT>
        static ptr cast(ptr p)
        {
            static_assert (std::is_base_of<object, ToT>::value,
                           "T must derive from 'object'");
            assert_type<ToT>(p);
            return static_cast<ToT *>(p);
        }

        template <typename ToT>
        static std::unique_ptr<ToT> cast(uptr p)
        {
            static_assert (std::is_base_of<object, ToT>::value,
                           "T must derive from 'object'");
            assert_type<ToT>(p.get());
            return std::make_unique<ToT>(static_cast<ToT>(p.release()));
        }

        template <typename ToT>
        static std::shared_ptr<ToT> cast(sptr p)
        {
            static_assert (std::is_base_of<object, ToT>::value,
                           "T must derive from 'object'");
            assert_type<ToT>(p.get());
            return std::shared_ptr<ToT>(p, static_cast<ToT *>(p.get()));
        }

    private:

        template <typename T>
        static
        void assert_type(object::cptr p)
        {
            if(p->get_info()->id != info::create<T>()->id) {
                throw std::runtime_error("object is not T");
            }
        }

        const info::holder info_;
    };

    template <typename T>
    class typed_object: public object
    {
    public:
        typed_object()
            :object(info::create<T>())
        { }
        typed_object(typed_object&&) = default;
        typed_object& operator = (typed_object&&) = default;
        typed_object(const typed_object&) = default;
        typed_object& operator = (const typed_object&) = default;
    };

#define erules_define_object(type_name) \
        type_name: public typed_object<type_name>

    inline bool operator < (const object::info::holder &lh,
                            const object::info::holder &rh)
    {
        return lh.get_info()->id < rh.get_info()->id;
    }
}

#endif // OBJECTS_H
