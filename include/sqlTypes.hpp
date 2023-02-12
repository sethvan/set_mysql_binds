#ifndef INCLUDED_SQLTYPES_H
#define INCLUDED_SQLTYPES_H

#include <mysql/mysql.h>

#include <algorithm>
#include <any>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "setcolor.hpp"
#include "utilities.h"

/*
    These are all classes meant to be used in a wrapper for MYSQL_BIND arrays found in
   BindsArray.hpp. They are composed of the data types and the enum_field_types that are permitted to be used in
   prepared statements.

   There are 8 C types and 1 MYSQL type used (char, short, int, long long, float, double,
   MYSQL_TIME and char[]). However in the case of char[], various enum_field_types that are valid for receiving data as
   char[] are not permitted to be used for requesting data. This is why I decided to amplify this class hierarchy using
   interfaces InputCType and OutputCType and as well change the wrapper to a template class.
*/

namespace set_mysql_binds {

    class SqlCType {
       protected:
        std::vector<unsigned char> charVec;

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

    // is_valid_value_method_type*********************************************
    template <typename T>
    struct is_valid_value_method_type : std::false_type {};
    template <>
    struct is_valid_value_method_type<char> : std::true_type {};
    template <>
    struct is_valid_value_method_type<unsigned char> : std::true_type {};
    template <>
    struct is_valid_value_method_type<short> : std::true_type {};
    template <>
    struct is_valid_value_method_type<unsigned short> : std::true_type {};
    template <>
    struct is_valid_value_method_type<int> : std::true_type {};
    template <>
    struct is_valid_value_method_type<unsigned int> : std::true_type {};
    template <>
    struct is_valid_value_method_type<long long> : std::true_type {};
    template <>
    struct is_valid_value_method_type<unsigned long long> : std::true_type {};
    template <>
    struct is_valid_value_method_type<float> : std::true_type {};
    template <>
    struct is_valid_value_method_type<double> : std::true_type {};
    template <>
    struct is_valid_value_method_type<MYSQL_TIME> : std::true_type {};

    class InputCType : public SqlCType {
       public:
        InputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
                    unsigned long long _bufferLength = 0 )
            : SqlCType( _fieldName, type, _buffer, _bufferLength ) {
        }
        virtual ~InputCType() = default;
        virtual void set_value( const std::any& a ) = 0;
        std::vector<unsigned char>& CharVec() {
            if ( !isCharArray( bufferType ) ) {
                std::ostringstream os;
                os << setcolor( Color::RED ) << "Error: "
                   << "set_mysql_binds::SqlCType::CharVec() should only be called when "
                   << "set_mysql_binds::isCharArray(set_mysql_binds::SqlCType::bufferType)"
                   << " evaluates to " << setcolor( Color::GREEN ) << "true"
                   << ".\n           set_mysql_binds::SqlCType::bufferType " << fieldTypes[bufferType]
                   << " evaluates to " << setcolor( Color::RED ) << "false"
                   << ".\n";
                throw std::runtime_error( os.str() );
            }
            else {
                return charVec;
            }
        }

        template <typename T>
        T& Value() {
            static_assert( is_valid_value_method_type<T>::value,
                           "Type not permitted for method set_mysql_binds::SqlCType::Value()\npermitted types are: { "
                           "char, unsigned char, short, unsigned short, int, unsigned int, long long, unsigned long "
                           "long, float, double,  MYSQL_TIME }\n " );
            return *static_cast<T*>( buffer );
        }
    };

    // The idea for these templates from reddit user u/IyeOnline
    template <typename T, enum_field_types Type>
    class TypeInputImpl : public InputCType {
        T value;

       public:
        TypeInputImpl() = delete;
        TypeInputImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : InputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ), &value, _bufferLength ) {
        }

        void printValue() const override {
            switch ( Type ) {
                case MYSQL_TYPE_TINY:
                    std::cout << std::left << std::setw( 30 ) << static_cast<int>( value );
                    break;
                case MYSQL_TYPE_BOOL: {
                    bool x = static_cast<bool>( value );
                    std::cout << std::boolalpha << std::left << std::setw( 30 ) << x;
                    break;
                }
                default:
                    std::cout << std::left << std::setw( 30 ) << value;
                    break;
            }
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
        const std::vector<unsigned char>& CharVec() {
            if ( !isCharArray( bufferType ) ) {
                std::ostringstream os;
                os << setcolor( Color::MAGENTA ) << "Warning: "
                   << "set_mysql_binds::SqlCType::CharVec() should only be called when "
                   << "set_mysql_binds::isCharArray(set_mysql_binds::SqlCType::bufferType)"
                   << " evaluates to " << setcolor( Color::GREEN ) << "true."
                   << "\n         set_mysql_binds::SqlCType::bufferType " << fieldTypes[bufferType] << " evaluates to "
                   << setcolor( Color::RED ) << "false"
                   << ".\n";
                std::cerr << os.str();
            }
            return charVec;
        }
        template <typename T>
        const T& Value() {
            static_assert( is_valid_value_method_type<T>::value,
                           "Type not permitted for method set_mysql_binds::SqlCType::Value()\npermitted types are: { "
                           "char, unsigned char, short, unsigned short, int, unsigned int, long long, unsigned long "
                           "long, float, double,  MYSQL_TIME }\n " );
            return *static_cast<T*>( buffer );
        }
    };

    template <typename T, enum_field_types Type>
    class TypeOutputImpl : public OutputCType {
        T value;

       public:
        TypeOutputImpl() = delete;
        TypeOutputImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : OutputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ), &value, _bufferLength ) {
        }

        void printValue() const override {
            switch ( Type ) {
                case MYSQL_TYPE_TINY:
                    std::cout << std::left << std::setw( 30 ) << static_cast<int>( value );
                    break;
                case MYSQL_TYPE_BOOL: {
                    bool x = static_cast<bool>( value );
                    std::cout << std::boolalpha << std::left << std::setw( 30 ) << x;
                    break;
                }
                default:
                    std::cout << std::left << std::setw( 30 ) << value;
                    break;
            }
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
            std::string newValue = std::any_cast<std::string>( a );
            std::copy( newValue.begin(), newValue.end(), charVec.begin() );
            length = newValue.size();
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

}  // namespace set_mysql_binds

#endif  // INCLUDED_SQLTYPES_H