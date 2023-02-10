#ifndef INCLUDED_MAKEBINDS_H
#define INCLUDED_MAKEBINDS_H

#include <memory>

#include "traits.h"

namespace set_mysql_binds {

    struct ElementBase {
        std::string_view name;
        unsigned long buffer_size;
        ElementBase( std::string_view _name, unsigned long _buffer_size = 0 )
            : name( _name ), buffer_size( _buffer_size ) {
        }

        virtual ~ElementBase() = default;

        virtual std::unique_ptr<InputCType> makeInput() = 0;

        virtual std::unique_ptr<OutputCType> makeOutput() = 0;
    };

    template <MysqlInputType T>
    struct Element : public ElementBase {
        using inType = BindType<T>::inType;
        using outType = BindType<T>::outType;

        Element( std::string_view _name, unsigned long _buffer_size = 0 ) : ElementBase( _name, _buffer_size ) {
        }

        std::unique_ptr<InputCType> makeInput() override {
            return std::make_unique<inType>( name, buffer_size );
        }

        std::unique_ptr<OutputCType> makeOutput() override {
            return std::make_unique<outType>( name, buffer_size );
        }
    };

    template <MysqlInputType T>
    std::unique_ptr<ElementBase> Bind( std::string_view _name, unsigned long _buffer_size = 0 ) {
        return std::make_unique<Element<T>>( _name, _buffer_size );
    }

    template <typename... Ptrs>
    Binds<InputCType> makeInputBinds( Ptrs&&... ptrs ) {
        std::vector<std::unique_ptr<ElementBase>> baseVec;
        ( baseVec.emplace_back( std::forward<Ptrs>( ptrs ) ), ... );

        std::vector<std::unique_ptr<InputCType>> vec;
        std::for_each( baseVec.begin(), baseVec.end(),
                       [&]( auto& element ) { vec.emplace_back( element->makeInput() ); } );
        return Binds<InputCType>( std::move( vec ) );
    }

    template <typename... Ptrs>
    Binds<OutputCType> makeOutputBinds( Ptrs&&... ptrs ) {
        std::vector<std::unique_ptr<ElementBase>> baseVec;
        ( baseVec.emplace_back( std::forward<Ptrs>( ptrs ) ), ... );

        std::vector<std::unique_ptr<OutputCType>> vec;
        std::for_each( baseVec.begin(), baseVec.end(),
                       [&]( auto& element ) { vec.emplace_back( element->makeOutput() ); } );
        return Binds<OutputCType>( std::move( vec ) );
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_MAKEBINDS_H