#ifndef INCLUDED_MAKEBINDS_H
#define INCLUDED_MAKEBINDS_H

#include <memory>

#include "traits.h"

namespace set_mysql_binds {

    template <MysqlInputType T>
    struct Element {
        using inType = BindType<T>::inType;
        using outType = BindType<T>::outType;

        std::string_view name;
        unsigned long buffer_size;

        Element( std::string_view _name, unsigned long _buffer_size = 0 )
            : name( _name ), buffer_size( +_buffer_size ) {
        }

        std::unique_ptr<InputCType> makeInput() {
            return std::make_unique<inType>( name, buffer_size );
        }

        std::unique_ptr<OutputCType> makeOutput() {
            return std::make_unique<outType>( name, buffer_size );
        }
    };

    template <MysqlInputType T>
    std::unique_ptr<Element<T>> Bind( std::string_view _name, unsigned long _buffer_size = 0 ) {
        return std::make_unique<Element<T>>( _name, _buffer_size );
    }

    template <typename... Ptrs>
    Binds<InputCType> makeInputBinds( Ptrs&&... ptrs ) {
        std::vector<std::unique_ptr<InputCType>> vec;
        ( vec.emplace_back( std::forward<std::unique_ptr<InputCType>>( ptrs->makeInput() ) ), ... );
        return Binds<InputCType>( std::move( vec ) );
    }

    template <typename... Ptrs>
    Binds<OutputCType> makeOutputBinds( Ptrs&&... ptrs ) {
        std::vector<std::unique_ptr<OutputCType>> vec;
        ( vec.emplace_back( std::forward<std::unique_ptr<OutputCType>>( ptrs->makeOutput() ) ), ... );
        return Binds<OutputCType>( std::move( vec ) );
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_MAKEBINDS_H