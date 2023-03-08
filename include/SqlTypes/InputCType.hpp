#ifndef INCLUDED_INPUTCTYPE_H
#define INCLUDED_INPUTCTYPE_H

#include <iostream>

#include "SqlTypes/SqlCType.h"

namespace set_mysql_binds {

using enum MysqlInputType;

class InputCType : public SqlCType {
  public:
   InputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
               unsigned long long _bufferLength = 0 )
       : SqlCType( _fieldName, type, _buffer, _bufferLength ) {}
   virtual ~InputCType() = default;
   InputCType& operator=( const InputCType& ) = delete;
   virtual void operator=( long double newValue ) = 0;
   virtual void operator=( const std::string& newValue ) = 0;
   virtual void operator=( std::span<const unsigned char> newValue ) = 0;
   virtual void operator=( const MYSQL_TIME& newValue ) = 0;

   template <MysqlInputType type>
   auto& Value() {
      return *static_cast<ValType<type>::type*>( buffer );
   }
};

// The original idea for these templates from reddit user u/IyeOnline
template <ApprovedType T, enum_field_types Type>
class InImpl : public InputCType {
   static constexpr const char* mismatch =
       "\'value member type\' does not match argument given to operator=() method\n";
   T value;

  public:
   InImpl() = delete;
   InImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
       : InputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ),
                     ( std::same_as<T, std::basic_string<unsigned char>> ? nullptr : &value ),
                     _bufferLength ) {
      if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         value.resize( _bufferLength, '\0' );
         buffer = value.data();
      }
      if constexpr ( std::same_as<T, MYSQL_TIME> ) {
         std::memset( &value, 0, sizeof( value ) );
      }
   }

   void operator=( long double newValue ) override {
      if constexpr ( std::integral<T> or std::floating_point<T> ) {
         T x = static_cast<T>( newValue );
         if ( strict_fundamental_type_checking ) {
            if ( static_cast<long double>( x ) != newValue ) {
               throw std::runtime_error( mismatch );
            }
         }
         value = x;
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void operator=( const std::string& newValue ) override {
      if ( !newValue.length() ) {
         isNull = true;
         return;
      }
      if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         std::copy( newValue.begin(), newValue.end(), value.begin() );
         length = newValue.size();
      } else if constexpr ( std::integral<T> ) {
         value = std::stol( newValue );
      } else if constexpr ( std::same_as<T, float> ) {
         value = std::stof( newValue );
      } else if constexpr ( std::same_as<T, double> ) {
         value = std::stod( newValue );
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void operator=( std::span<const unsigned char> newValue ) override {
      if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         std::copy( newValue.begin(), newValue.end(), value.begin() );
         length = newValue.size();
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void operator=( const MYSQL_TIME& newValue ) override {
      if constexpr ( std::same_as<T, MYSQL_TIME> ) {
         value = newValue;
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   std::ostream& print_value( std::ostream& os ) const override {
      os << std::boolalpha << std::setprecision( 15 );
      if ( isNull ) {
         os << "NULL";
      } else if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         std::copy( value.begin(), value.end(), std::ostream_iterator<unsigned char>{ os } );
      } else if constexpr ( Type == MYSQL_TYPE_TINY ) {
         os << static_cast<int>( value );
      } else if constexpr ( Type == MYSQL_TYPE_BOOL ) {
         os << static_cast<bool>( value );
      } else if constexpr ( std::same_as<T, MYSQL_TIME> ) {
         os << value.year << "-" << ( value.month > 9 ? "" : "0" ) << std::to_string( value.month )
            << "-" << ( value.day > 9 ? "" : "0" ) << std::to_string( value.day ) << " "
            << ( value.hour > 9 ? "" : "0" ) << std::to_string( value.hour ) << ":"
            << ( value.minute > 9 ? "" : "0" ) << std::to_string( value.minute ) << ":"
            << ( value.second > 9 ? "" : "0" ) << std::to_string( value.second );
      } else {
         os << value;
      }
      return os;
   }
};

}  // namespace set_mysql_binds
#endif  // INCLUDED_INPUTCTYPE_H