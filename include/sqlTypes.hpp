#ifndef INCLUDED_SQLTYPES_H
#define INCLUDED_SQLTYPES_H

#include <mysql/mysql.h>

#include <algorithm>
#include <any>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <span>
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

        // virtual void printValue() const = 0;  // mostly to make base class not instantiable
    };

    // is_valid_value_method_type*********************************************
    template <typename T>
    struct is_approved_type : std::false_type {};
    template <>
    struct is_approved_type<bool> : std::true_type {};
    template <>
    struct is_approved_type<signed char> : std::true_type {};
    template <>
    struct is_approved_type<unsigned char> : std::true_type {};
    template <>
    struct is_approved_type<short> : std::true_type {};
    template <>
    struct is_approved_type<unsigned short> : std::true_type {};
    template <>
    struct is_approved_type<int> : std::true_type {};
    template <>
    struct is_approved_type<unsigned int> : std::true_type {};
    template <>
    struct is_approved_type<long> : std::true_type {};
    template <>
    struct is_approved_type<unsigned long> : std::true_type {};
    template <>
    struct is_approved_type<float> : std::true_type {};
    template <>
    struct is_approved_type<double> : std::true_type {};
    template <>
    struct is_approved_type<std::basic_string<unsigned char>> : std::true_type {};
    template <>
    struct is_approved_type<MYSQL_TIME> : std::true_type {};

    template <typename T>
    concept ApprovedType = is_approved_type<T>::value;

    class InputCType : public SqlCType {
       public:
        InputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
                    unsigned long long _bufferLength = 0 )
            : SqlCType( _fieldName, type, _buffer, _bufferLength ) {
        }
        virtual ~InputCType() = default;
        virtual void set_value( long double newValue ) = 0;
        virtual void set_value( std::string_view newValue ) = 0;
        virtual void set_value( std::span<const unsigned char> newValue ) = 0;
        virtual void set_value( const MYSQL_TIME& newValue ) = 0;
        virtual std::ostream& print_value( std::ostream& os ) const = 0;

        template <ApprovedType T>
        T& Value() {
            return *static_cast<T*>( buffer );
        }
    };

    // The original idea for these templates from reddit user u/IyeOnline
    template <ApprovedType T, enum_field_types Type>
    class InputImpl : public InputCType {
        static constexpr const char* mismatch =
            "\'value member type\' does not match argument given to set_value() method";
        T value;

       public:
        InputImpl() = delete;
        InputImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
            : InputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ), &value, _bufferLength ) {
            if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
                value.resize( _bufferLength, '\0' );
            }
        }

        void set_value( long double newValue ) override {
            if constexpr ( std::integral<T> or std::floating_point<T> ) {
                T x = static_cast<T>( newValue );
                if ( static_cast<long double>( x ) != newValue ) {
                    throw std::runtime_error( mismatch );
                }
                value = x;
            }
            else {
                throw std::runtime_error( mismatch );
            }
        }
        void set_value( std::string_view newValue ) override {
            if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
                std::copy( newValue.begin(), newValue.end(), value.begin() );
            }
            else {
                throw std::runtime_error( mismatch );
            }
        }
        void set_value( std::span<const unsigned char> newValue ) override {
            if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
                std::copy( newValue.begin(), newValue.end(), value.begin() );
            }
            else {
                throw std::runtime_error( mismatch );
            }
        }
        void set_value( const MYSQL_TIME& newValue ) override {
            if constexpr ( std::same_as<T, MYSQL_TIME> ) {
                value = newValue;
            }
            else {
                throw std::runtime_error( mismatch );
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
        template <ApprovedType T>
        const T& Value() {
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
        // void printValue() const override {
        //     std::cout << std::left << std::setw( 30 ) << "MYSQL_TIME Value";  // needs to be set
        // }
    };

    template <enum_field_types TYPE>
    class TypeCharArrayOutput : public OutputCType {

       public:
        TypeCharArrayOutput() = delete;
        TypeCharArrayOutput( std::string_view _fieldName, unsigned long long _bufferLength )
            : OutputCType( _fieldName, TYPE, nullptr, _bufferLength ) {
        }
        // void printValue() const override {
        //     std::cout << std::left << std::setw( 30 ) << charVec.data();
        // }
    };

    std::ostream& operator<<( std::ostream& os, const std::unique_ptr<InputCType>& obj ) {
        return obj->print_value( os );
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_SQLTYPES_H