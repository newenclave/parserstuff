#pragma once
#include "erules/objects.h"

namespace erules { namespace objects { namespace oprerations {

    template <typename IdType>
    class binary {
    public:
        using id_type = IdType;

        using function_type = std::function<base::uptr(base::ptr, base::ptr)>;
        using index_type
            = std::tuple<id_type, base::info::holder, base::info::holder>;
        using map_type = std::map<index_type, function_type>;

    private:
        template <typename LeftT, typename RightT, typename CallT>
        static function_type create_call(CallT call)
        {
            return [call](auto left, auto right) {
                return call(base::cast<LeftT>(left), base::cast<RightT>(right));
            };
        }

        template <typename LeftT, typename RightT, typename CallT>
        static function_type create_reverse_call(CallT call)
        {
            return [call](auto left, auto right) {
                return call(base::cast<LeftT>(right), base::cast<RightT>(left));
            };
        }

    public:
        template <typename LeftT, typename RightT, typename CallT>
        void set(id_type op, CallT call, bool add_revert = false)
        {
            if (!std::is_same<LeftT, RightT>::value && add_revert) {
                set_impl(op, base::info::create<RightT>(),
                         base::info::create<LeftT>(),
                         create_reverse_call<LeftT, RightT>(call));
            }
            set_impl(op, base::info::create<LeftT>(),
                     base::info::create<RightT>(),
                     create_call<LeftT, RightT, CallT>(std::move(call)));
        }

        base::uptr call(id_type op, base::ptr left, base::ptr right)
        {
            if (auto call = get(op, left, right)) {
                return call(left, right);
            }
            return {};
        }

        template <typename TargetObj>
        std::unique_ptr<TargetObj> call_cast(id_type op, base::ptr left,
                                             base::ptr right)
        {
            return base::cast<TargetObj>(call(op, left, right));
        }

        function_type get(id_type op, base::ptr left, base::ptr right)
        {
            auto id
                = std::make_tuple(op, left->type_info(), right->type_info());
            auto find = bin_map_.find(id);
            if (find != bin_map_.end()) {
                return find->second;
            }
            return {};
        }

        template <typename LeftT, typename RightT>
        function_type get(id_type op)
        {
            auto id = std::make_tuple(op, base::info::create<LeftT>(),
                                      base::info::create<RightT>());
            auto find = bin_map_.find(id);
            if (find != bin_map_.end()) {
                return find->second;
            }
            return {};
        }

    private:
        void set_impl(id_type op, base::info::holder left_type,
                      base::info::holder right_type, function_type call)
        {
            bin_map_[std::make_tuple(op, left_type, right_type)]
                = std::move(call);
        }

        map_type bin_map_;
    };

    template <typename IdType>
    class unary {
    public:
        using id_type = IdType;
        using function_type = std::function<base::uptr(base::ptr)>;
        using index_type = std::tuple<id_type, base::info::holder>;
        using map_type = std::map<index_type, function_type>;

    private:
        template <typename ValueT, typename CallT>
        static function_type create_call(CallT call)
        {
            return [call](auto obj) { return call(base::cast<ValueT>(obj)); };
        }

    public:
        template <typename ValueT, typename CallT>
        void set(id_type op, CallT call)
        {
            set_impl(op, base::info::create<ValueT>(),
                     create_call<ValueT, CallT>(std::move(call)));
        }

        base::uptr call(id_type op, base::ptr value)
        {
            if (auto call = get(op, value)) {
                return call(value);
            }
            return {};
        }

        template <typename TargetObj>
        std::unique_ptr<TargetObj> call_cast(id_type op, base::ptr val)
        {
            return base::cast<TargetObj>(call(op, val));
        }

        function_type get(id_type op, base::ptr value)
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
            auto id = std::make_tuple(op, base::info::create<ValueT>());
            auto find = un_map_.find(id);
            if (find != un_map_.end()) {
                return find->second;
            }
            return {};
        }

    private:
        void set_impl(id_type op, base::info::holder value_type,
                      function_type call)
        {
            un_map_[std::make_tuple(op, value_type)] = std::move(call);
        }

    private:
        map_type un_map_;
    };

    class transform {
    public:
        using function_type = std::function<base::uptr(base::ptr)>;
        using index_type = std::tuple<base::info::holder, base::info::holder>;
        using map_type = std::map<index_type, function_type>;

    private:
        template <typename ValueT, typename CallT>
        static function_type create_unary_call(CallT call)
        {
            return [call](auto obj) { return call(base::cast<ValueT>(obj)); };
        }

    public:
        template <typename FromT, typename ToT, typename CallT>
        void set(CallT call)
        {
            set_impl(base::info::create<FromT>(), base::info::create<ToT>(),
                     create_unary_call<FromT>(std::move(call)));
        }

        template <typename ToT>
        std::unique_ptr<ToT> call(base::ptr value)
        {
            auto id = std::make_tuple(value->type_info(),
                                      base::info::create<ToT>());
            std::cout << value->type_info()->id << " "
                      << base::info::create<ToT>()->id << std::endl;

            auto find = trans_map_.find(id);
            if (find != trans_map_.end()) {
                return base::cast<ToT>(find->second(value));
            }
            return {};
        }

        template <typename ToT>
        std::function<std::unique_ptr<ToT>(base::ptr)> get(base::ptr value)
        {
            auto id = std::make_tuple(value->type_info(),
                                      base::info::create<ToT>());
            auto find = trans_map_.find(id);
            if (find != trans_map_.end()) {
                auto call = find->second;
                return
                    [call](auto value) { return base::cast<ToT>(call(value)); };
            }
            return {};
        }

        template <typename FromT, typename ToT>
        std::function<std::unique_ptr<ToT>(base::ptr)> get()
        {
            auto id = std::make_tuple(base::info::create<FromT>(),
                                      base::info::create<ToT>());
            auto find = trans_map_.find(id);
            if (find != trans_map_.end()) {
                auto call = find->second;
                return
                    [call](auto value) { return base::cast<ToT>(call(value)); };
            }
            return {};
        }

    private:
        void set_impl(base::info::holder from_type, base::info::holder to_type,
                      function_type call)
        {
            trans_map_[std::make_tuple(from_type, to_type)] = std::move(call);
            std::cout << from_type->id << " " << to_type->id << std::endl;
        }
        map_type trans_map_;
    };


}}}