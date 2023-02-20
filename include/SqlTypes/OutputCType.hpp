#ifndef INCLUDED_OUTPUTCTYPE_H
#define INCLUDED_OUTPUTCTYPE_H

#include "SqlTypes/SqlCType.h"

namespace set_mysql_binds
 {
    class OutputCType : public SqlCType {
       public:
        OutputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
                     unsigned long long _bufferLength = 0 )
            : SqlCType( _fieldName, type, _buffer, _bufferLength ) {
        }
        virtual ~OutputCType() = default;
        
        template <ApprovedType T>
        const T& Value() {
            return *static_cast<T*>( buffer );
        }
    };

    template <typename T, enum_field_types Type>
    class OutImpl : public OutputCType {
        T value;

       public:
        OutImpl() = delete;
        OutImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : OutputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ), &value, _bufferLength ) {
            if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
                value.resize( _bufferLength, '\0' );
            }
        }

        std::ostream& print_value( std::ostream& os ) const override {
            os << std::boolalpha << std::setprecision( 15 );
            if ( isNull ) {
                os << "NULL";
            }
            else if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
                std::copy( value.begin(), value.end(), std::ostream_iterator<unsigned char>{ os } );
            }
            else if ( Type == MYSQL_TYPE_TINY ) {
                os << static_cast<int>( value );
            }
            else if ( Type == MYSQL_TYPE_BOOL ) {
                os << static_cast<bool>( value );
            }
            else {
                os << value;
            }
            return os;
        }
    };

   
    
 }
#endif // INCLUDED_OUTPUTCTYPE_H