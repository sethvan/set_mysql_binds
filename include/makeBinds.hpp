#ifndef INCLUDED_MAKEBINDS_H
#define INCLUDED_MAKEBINDS_H

#include <memory>

#include "traits.h"

namespace set_mysql_binds {

    template <MysqlInputType T>
    struct Bind {
        using inType = BindType<T>::inType;
        using outType = BindType<T>::outType;

        std::string_view name;
        unsigned long buffer_size;

        Bind( std::string_view _name, unsigned long _buffer_size = 0 ) : name( _name ), buffer_size( _buffer_size ) {
        }

        std::unique_ptr<InputCType> makeInput() {
            return std::make_unique<inType>( name, buffer_size );
        }

        std::unique_ptr<OutputCType> makeOutput() {
            return std::make_unique<outType>( name, buffer_size );
        }
    };

    template <MysqlInputType... Ts>
    BindsArray<InputCType> makeInputBindsArray( Bind<Ts>... objects ) {
        std::vector<std::unique_ptr<InputCType>> vec;
        vec.reserve( sizeof...( objects ) );
        ( vec.emplace_back( objects.makeInput() ), ... );
        return BindsArray<InputCType>( std::move( vec ) );
    }

    template <MysqlInputType... Ts>
    BindsArray<OutputCType> makeOutputBindsArray( Bind<Ts>... objects ) {
        std::vector<std::unique_ptr<OutputCType>> vec;
        vec.reserve( sizeof...( objects ) );
        ( vec.emplace_back( objects.makeOutput() ), ... );
        return BindsArray<OutputCType>( std::move( vec ) );
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_MAKEBINDS_H