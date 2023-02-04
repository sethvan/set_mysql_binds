#ifndef INCLUDED_SQLTYPES_H
#define INCLUDED_SQLTYPES_H

#include <mysql/mysql.h>

#include <any>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "utilities.h"

/*
    These are all classes meant to be used in a wrapper for MYSQL_BIND arrays found in
   binds.hpp. They are composed of the data types and the enum_field_types that are permitted to be used in
   prepared statements.

   There are 8 C types and 1 MYSQL type used (char, short, int, long long, float, double,
   MYSQL_TIME and char[]). However in the case of char[], various enum_field_types that are valid for receiving data as
   char[] are not permitted to be used for requesting data. This is why I decided to amplify this class hierarchy using
   interfaces InputCType and OutputCType and as well change the wrapper to a template class.
*/

namespace set_mysql_binds {

    class SqlCType {
       protected:
        std::vector<char> charVec;

       public:
        const std::string_view fieldName;
        const enum_field_types bufferType;
        bool isNull;
        bool error;
        unsigned long length;
        MYSQL_BIND* bind;
        void* buffer;
        unsigned long long bufferLength;
        bool is_selected;

        SqlCType( const char* _fieldName, enum_field_types type, void* _buffer, unsigned long long _bufferLength = 0 )
            : fieldName( _fieldName ),
              bufferType( type ),
              isNull( 0 ),
              error( 0 ),
              length( 0 ),
              bind( nullptr ),
              buffer( _buffer ),
              bufferLength( _bufferLength ),
              is_selected( true ) {
            if ( isCharArray( bufferType ) ) {

                for ( unsigned long long i = 0; i < bufferLength; ++i ) {
                    charVec.push_back( '\0' );
                }
                buffer = charVec.data();
            }
        }

        void setBind( MYSQL_BIND* targetBind ) {
            bind = targetBind;
            std::memset( bind, 0, sizeof( *bind ) );
            bind->buffer_type = bufferType;
            bind->buffer = (char*)buffer;
            bind->is_null = &isNull;
            bind->length = &length;
            bind->error = &error;
            if ( isCharArray( bufferType ) ) {
                bind->buffer_length = bufferLength;
            }
        }

        virtual ~SqlCType() = default;

        virtual void printValue() const = 0;  // mostly to make base class not instantiable
    };

    class InputCType : public SqlCType {
       public:
        InputCType( const char* _fieldName, enum_field_types type, void* _buffer, unsigned long long _bufferLength = 0 )
            : SqlCType( _fieldName, type, _buffer, _bufferLength ) {
        }
        virtual ~InputCType() = default;
        virtual void set_value( const std::any& a ) = 0;
    };

    // The idea for these templates from reddit user u/IyeOnline
    template <typename T, enum_field_types Type>
    class TypeInputImpl : public InputCType {
        T value;

       public:
        TypeInputImpl() = delete;
        TypeInputImpl( const char* _fieldName ) : InputCType( _fieldName, Type, &value ) {
        }

        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << value;
        }

        void set_value( const std::any& a ) override {
            value = std::any_cast<T>( a );
        }
    };

    class OutputCType : public SqlCType {
       public:
        OutputCType( const char* _fieldName, enum_field_types type, void* _buffer,
                     unsigned long long _bufferLength = 0 )
            : SqlCType( _fieldName, type, _buffer, _bufferLength ) {
        }
        virtual ~OutputCType() = default;
    };

    template <typename T, enum_field_types Type>
    class TypeOutputImpl : public OutputCType {
        T value;

       public:
        TypeOutputImpl() = delete;
        TypeOutputImpl( const char* _fieldName ) : OutputCType( _fieldName, Type, &value ) {
        }

        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << value;
        }
    };

    using TypeCharInput = TypeInputImpl<signed char, MYSQL_TYPE_TINY>;
    using TypeCharOutput = TypeOutputImpl<signed char, MYSQL_TYPE_TINY>;

    using TypeShortInput = TypeInputImpl<short, MYSQL_TYPE_SHORT>;
    using TypeShortOutput = TypeOutputImpl<short, MYSQL_TYPE_SHORT>;

    using TypeIntInput = TypeInputImpl<int, MYSQL_TYPE_LONG>;
    using TypeIntOutput = TypeOutputImpl<int, MYSQL_TYPE_LONG>;

    using TypeLLInput = TypeInputImpl<long long, MYSQL_TYPE_LONGLONG>;
    using TypeLLOutput = TypeOutputImpl<long long, MYSQL_TYPE_LONGLONG>;

    using TypeFloatInput = TypeInputImpl<float, MYSQL_TYPE_FLOAT>;
    using TypeFloatOutput = TypeOutputImpl<float, MYSQL_TYPE_FLOAT>;

    using TypeDoubleInput = TypeInputImpl<double, MYSQL_TYPE_DOUBLE>;
    using TypeDoubleOutput = TypeOutputImpl<double, MYSQL_TYPE_DOUBLE>;

    class TypeMysqlTimeInput : public InputCType {
       private:
        MYSQL_TIME value;

       public:
        TypeMysqlTimeInput() = delete;
        TypeMysqlTimeInput( const char* _fieldName, enum_field_types type ) : InputCType( _fieldName, type, &value ) {
            if ( !isTimeType( type ) ) {
                throw std::runtime_error( "Invalid buffer type given for TypeMysqlTimeInput" );
            }
        }
        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << "MYSQL_TIME Value";  // needs to be set
        }
        void set_value( const std::any& a ) override {
            value = std::any_cast<MYSQL_TIME>( a );
        }
    };

    class TypeMysqlTimeOutput : public OutputCType {
       private:
        MYSQL_TIME value;

       public:
        TypeMysqlTimeOutput() = delete;
        TypeMysqlTimeOutput( const char* _fieldName, enum_field_types type ) : OutputCType( _fieldName, type, &value ) {
            if ( !isTimeType( type ) ) {
                throw std::runtime_error( "Invalid buffer type given for TypeMysqlTimeOutput" );
            }
        }
        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << "MYSQL_TIME Value";  // needs to be set
        }
    };

    class TypeCharArrayInput : public InputCType {

       public:
        TypeCharArrayInput() = delete;
        TypeCharArrayInput( const char* _fieldName, enum_field_types type, unsigned long long _bufferLength )
            : InputCType( _fieldName, type, nullptr, _bufferLength ) {
            if ( !isCharArrayReq( type ) ) {
                throw std::runtime_error( "Invalid buffer type given for TypeCharArrayInput" );
            }
        }

        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << charVec.data();
        }
        void set_value( const std::any& a ) override {
            const char* newValue = std::any_cast<const char*>( a );
            std::strcpy( charVec.data(), newValue );
            length = std::strlen( charVec.data() );
        }
    };

    class TypeCharArrayOutput : public OutputCType {

       public:
        TypeCharArrayOutput() = delete;
        TypeCharArrayOutput( const char* _fieldName, enum_field_types type, unsigned long long _bufferLength )
            : OutputCType( _fieldName, type, nullptr, _bufferLength ) {
            if ( !isCharArray( type ) ) {
                throw std::runtime_error( "Invalid buffer type given for TypeCharArrayOutput" );
            }
        }
        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << charVec.data();
        }
    };

}  // namespace set_mysql_binds

#endif  // INCLUDED_SQLTYPES_H