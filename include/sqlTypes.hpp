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

        SqlCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
                  unsigned long long _bufferLength = 0 )
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
        InputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
                    unsigned long long _bufferLength = 0 )
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
        TypeInputImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : InputCType( _fieldName, Type, &value, _bufferLength ) {
        }

        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << value;
        }

        // Arguments for `a` that are lvalues need to match type T and
        // arguments that are rvalues should be cast as T type so as to disambiguate
        // and prevent std::bad_any_cast exceptions.
        void set_value( const std::any& a ) override {
            value = std::any_cast<T>( a );
        }
    };

    class OutputCType : public SqlCType {
       public:
        OutputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
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
        TypeOutputImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : OutputCType( _fieldName, Type, &value, _bufferLength ) {
        }

        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << value;
        }
    };

    template <enum_field_types TYPE>
    class TypeMysqlTimeInput : public InputCType {
       private:
        MYSQL_TIME value;

       public:
        TypeMysqlTimeInput() = delete;
        TypeMysqlTimeInput( std::string_view _fieldName ) : InputCType( _fieldName, TYPE, &value ) {
            if ( !isTimeType( TYPE ) ) {
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

    template <enum_field_types TYPE>
    class TypeMysqlTimeOutput : public OutputCType {
       private:
        MYSQL_TIME value;

       public:
        TypeMysqlTimeOutput() = delete;
        TypeMysqlTimeOutput( std::string_view _fieldName ) : OutputCType( _fieldName, TYPE, &value ) {
            if ( !isTimeType( TYPE ) ) {
                throw std::runtime_error( "Invalid buffer type given for TypeMysqlTimeOutput" );
            }
        }
        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << "MYSQL_TIME Value";  // needs to be set
        }
    };

    template <enum_field_types TYPE>
    class TypeCharArrayInput : public InputCType {

       public:
        TypeCharArrayInput() = delete;
        TypeCharArrayInput( std::string_view _fieldName, unsigned long long _bufferLength )
            : InputCType( _fieldName, TYPE, nullptr, _bufferLength ) {
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

    template <enum_field_types TYPE>
    class TypeCharArrayOutput : public OutputCType {

       public:
        TypeCharArrayOutput() = delete;
        TypeCharArrayOutput( std::string_view _fieldName, unsigned long long _bufferLength )
            : OutputCType( _fieldName, TYPE, nullptr, _bufferLength ) {
        }
        void printValue() const override {
            std::cout << std::left << std::setw( 30 ) << charVec.data();
        }
    };

    using CharInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using CharOutput = TypeCharArrayInput<MYSQL_TYPE_STRING>;

    using VarCharInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using VarCharOutput = TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>;

    using TinyTextInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using TinyTextOutput = TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>;

    using TextInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using TextOutput = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;

    using BlobInput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    using BlobOutput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;

    using MediumTextInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using MediumTextOutput = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;

    using MediumBlobInput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    using MediumBlobOutput = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;

    using LongTextInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using LongTextOutput = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;

    using LongBlobInput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    using LongBlobOutput = TypeCharArrayOutput<MYSQL_TYPE_LONG_BLOB>;

    using TinyIntInput = TypeInputImpl<signed char, MYSQL_TYPE_TINY>;
    using TinyIntInputUnsigned = TypeInputImpl<unsigned char, MYSQL_TYPE_TINY>;
    using TinyIntOutput = TypeOutputImpl<signed char, MYSQL_TYPE_TINY>;
    using TinyIntOutputUnsigned = TypeOutputImpl<unsigned char, MYSQL_TYPE_TINY>;

    using SmallIntInput = TypeInputImpl<short, MYSQL_TYPE_SHORT>;
    using SmallIntInputUnsigned = TypeInputImpl<unsigned short, MYSQL_TYPE_SHORT>;
    using SmallIntOutput = TypeOutputImpl<short, MYSQL_TYPE_SHORT>;
    using SmallIntOutputUnsigned = TypeOutputImpl<unsigned short, MYSQL_TYPE_SHORT>;

    using MediumIntInput = TypeInputImpl<int, MYSQL_TYPE_LONG>;
    using MediumIntInputUnsigned = TypeInputImpl<unsigned int, MYSQL_TYPE_LONG>;
    using MediumIntOutput = TypeOutputImpl<int, MYSQL_TYPE_INT24>;
    using MediumIntOutputUnsigned = TypeOutputImpl<unsigned int, MYSQL_TYPE_INT24>;

    using IntInput = TypeInputImpl<int, MYSQL_TYPE_LONG>;
    using IntInputUnsigned = TypeInputImpl<unsigned int, MYSQL_TYPE_LONG>;
    using IntOutput = TypeOutputImpl<int, MYSQL_TYPE_LONG>;
    using IntOutputUnsigned = TypeOutputImpl<unsigned int, MYSQL_TYPE_LONG>;

    using BigIntInput = TypeInputImpl<long long, MYSQL_TYPE_LONGLONG>;
    using BigIntInputUnsigned = TypeInputImpl<unsigned long long, MYSQL_TYPE_LONGLONG>;
    using BigIntOutput = TypeOutputImpl<long long, MYSQL_TYPE_LONGLONG>;
    using BigIntOutputUnsigned = TypeOutputImpl<unsigned long long, MYSQL_TYPE_LONGLONG>;

    using FloatInput = TypeInputImpl<float, MYSQL_TYPE_FLOAT>;
    using FloatOutput = TypeOutputImpl<float, MYSQL_TYPE_FLOAT>;

    using DoubleInput = TypeInputImpl<double, MYSQL_TYPE_DOUBLE>;
    using DoubleOutput = TypeOutputImpl<double, MYSQL_TYPE_DOUBLE>;

    using DecimalInput = TypeInputImpl<double, MYSQL_TYPE_DOUBLE>;
    using DecimalOutput = TypeCharArrayOutput<MYSQL_TYPE_NEWDECIMAL>;

    using DateInput = TypeMysqlTimeInput<MYSQL_TYPE_DATE>;
    using DateOutput = TypeMysqlTimeOutput<MYSQL_TYPE_DATE>;

    using DateTimeInput = TypeMysqlTimeInput<MYSQL_TYPE_DATETIME>;
    using DateTimeOutput = TypeMysqlTimeOutput<MYSQL_TYPE_DATETIME>;

    using TimeStampInput = TypeMysqlTimeInput<MYSQL_TYPE_TIMESTAMP>;
    using TimeStampOutput = TypeMysqlTimeOutput<MYSQL_TYPE_TIMESTAMP>;

    using TimeInput = TypeMysqlTimeInput<MYSQL_TYPE_TIME>;
    using TimeOutput = TypeMysqlTimeOutput<MYSQL_TYPE_TIME>;

    using EnumInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using EnumOutput = TypeCharArrayOutput<MYSQL_TYPE_STRING>;

    using SetInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using SetOutput = TypeCharArrayOutput<MYSQL_TYPE_STRING>;

    using BooleanInput = TypeInputImpl<signed char, MYSQL_TYPE_TINY>;
    using Booleanoutput = TypeOutputImpl<signed char, MYSQL_TYPE_TINY>;

    using BitInput = TypeOutputImpl<unsigned long, MYSQL_TYPE_BIT>;
    using BitOutput = TypeOutputImpl<unsigned long, MYSQL_TYPE_BIT>;

    using GeometryInput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    using GeometryOutput = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;

    using JsonInput = TypeCharArrayInput<MYSQL_TYPE_JSON>;
    using JsonOutput = TypeCharArrayOutput<MYSQL_TYPE_JSON>;

    using BinaryInput = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    using BinaryOutput = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;

    using VarbinaryInput = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    using VarbinaryOutput = TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>;

    using TinyBlobInput = TypeCharArrayInput<MYSQL_TYPE_TINY_BLOB>;
    using TinyBlobOutput = TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>;

}  // namespace set_mysql_binds

#endif  // INCLUDED_SQLTYPES_H