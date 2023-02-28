#ifndef INCLUDED_INPUTCTYPE_H
#define INCLUDED_INPUTCTYPE_H

#include "SqlTypes/SqlCType.h"
#include "valTypes.h"

namespace set_mysql_binds {

using enum MysqlInputType;

class InputCType : public SqlCType {
  public:
   InputCType( std::string_view _fieldName, enum_field_types type, void* _buffer,
               unsigned long long _bufferLength = 0 )
       : SqlCType( _fieldName, type, _buffer, _bufferLength ) {}
   virtual ~InputCType() = default;
   virtual void set_value( long double newValue ) = 0;
   virtual void set_value( std::string_view newValue ) = 0;
   virtual void set_value( std::span<const unsigned char> newValue ) = 0;
   virtual void set_value( const MYSQL_TIME& newValue ) = 0;

   template <MysqlInputType type>
   auto& Value() {
      return *static_cast<ValType<type>::type*>( buffer );
   }
};

// The original idea for these templates from reddit user u/IyeOnline
template <ApprovedType T, enum_field_types Type>
class InImpl : public InputCType {
   static constexpr const char* mismatch =
       "\'value member type\' does not match argument given to set_value() method";
   T value;

  public:
   InImpl() = delete;
   InImpl( std::string_view _fieldName, unsigned long long _bufferLength = 0 )
       : InputCType( _fieldName, ( Type == MYSQL_TYPE_BOOL ? MYSQL_TYPE_TINY : Type ), &value,
                     _bufferLength ) {
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
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void set_value( std::string_view newValue ) override {
      if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         std::copy( newValue.begin(), newValue.end(), value.begin() );
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void set_value( std::span<const unsigned char> newValue ) override {
      if constexpr ( std::same_as<T, std::basic_string<unsigned char>> ) {
         std::copy( newValue.begin(), newValue.end(), value.begin() );
      } else {
         throw std::runtime_error( mismatch );
      }
   }
   void set_value( const MYSQL_TIME& newValue ) override {
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
      } else if ( Type == MYSQL_TYPE_TINY ) {
         os << static_cast<int>( value );
      } else if ( Type == MYSQL_TYPE_BOOL ) {
         os << static_cast<bool>( value );
      } else {
         os << value;
      }
      return os;
   }
};

std::ostream& operator<<( std::ostream& os, const std::unique_ptr<InputCType>& obj ) {
   return obj->print_value( os );
}
}  // namespace set_mysql_binds
#endif  // INCLUDED_INPUTCTYPE_H