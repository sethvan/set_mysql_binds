#ifndef INCLUDED_SQLCTYPE_H
#define INCLUDED_SQLCTYPE_H

#include <mysql/mysql.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "utilities.h"

/*
    This is the base class meant to be used in a wrapper for MYSQL_BIND arrays found in
   BindsArray.hpp. A concept, an enum and some type traits all related to MySQL and C type
   information are placed here as well.

   There are 8 C types and 1 MYSQL type used (char, short, int, long long, float, double, MYSQL_TIME
   and char[]). However in the case of char[], various enum_field_types that are valid for receiving
   data as char[] are not permitted to be used for requesting data. This is why I decided to amplify
   this class hierarchy using interfaces InputCType and OutputCType in their respective hpp files.
   Also have opted for using Long instead of Long Long for design reasons.
*/

namespace set_mysql_binds {

class SqlCType {
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
         is_selected( true ) {}

   void setBind( MYSQL_BIND* targetBind ) {
      bind = targetBind;
      std::memset( bind, 0, sizeof( *bind ) );
      bind->buffer_type = bufferType;
      bind->buffer = (char*)buffer;
      bind->is_null = &isNull;
      bind->length = &length;
      bind->error = &error;
      bind->buffer_length = bufferLength;
   }

   virtual ~SqlCType() = default;

   virtual std::ostream& print_value(
       std::ostream& os ) const = 0;  // mostly to make base class not instantiable
};

// is_approved_type*********************************************
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

enum class MysqlInputType {
   INT,
   INT_UNSIGNED,
   CHAR,
   VARCHAR,
   TINYTEXT,
   TEXT,
   BLOB,
   MEDIUMTEXT,
   MEDIUMBLOB,
   LONGTEXT,
   LONGBLOB,
   TINYINT,
   TINYINT_UNSIGNED,
   SMALLINT,
   SMALLINT_UNSIGNED,
   MEDIUMINT,
   MEDIUMINT_UNSIGNED,
   BIGINT,
   BIGINT_UNSIGNED,
   FLOAT,
   DOUBLE,
   DECIMAL,
   DATE,
   DATETIME,
   TIMESTAMP,
   TIME,
   ENUM,
   SET,
   BOOLEAN,
   BIT,
   GEOMETRY,
   JSON,
   BINARY,
   VARBINARY,
   TINYBLOB
};

// To facilitate Value() methods of InputCType and OutputCType which return references to the
// derived class's value members.
using enum MysqlInputType;

template <MysqlInputType T>
struct ValType {};
template <>
struct ValType<INT> {
   using type = int;
};
template <>
struct ValType<INT_UNSIGNED> {
   using type = unsigned int;
};
template <>
struct ValType<CHAR> {
   using type = unsigned char;
};
template <>
struct ValType<VARCHAR> {
   using type = unsigned char;
};
template <>
struct ValType<TINYTEXT> {
   using type = unsigned char;
};
template <>
struct ValType<TEXT> {
   using type = unsigned char;
};
template <>
struct ValType<BLOB> {
   using type = unsigned char;
};
template <>
struct ValType<MEDIUMTEXT> {
   using type = unsigned char;
};
template <>
struct ValType<MEDIUMBLOB> {
   using type = unsigned char;
};
template <>
struct ValType<LONGTEXT> {
   using type = unsigned char;
};
template <>
struct ValType<LONGBLOB> {
   using type = unsigned char;
};
template <>
struct ValType<TINYINT> {
   using type = signed char;
};
template <>
struct ValType<TINYINT_UNSIGNED> {
   using type = unsigned char;
};
template <>
struct ValType<SMALLINT> {
   using type = short;
};
template <>
struct ValType<SMALLINT_UNSIGNED> {
   using type = unsigned short;
};
template <>
struct ValType<BIGINT> {
   using type = long;
};
template <>
struct ValType<BIGINT_UNSIGNED> {
   using type = unsigned long;
};
template <>
struct ValType<FLOAT> {
   using type = float;
};
template <>
struct ValType<DOUBLE> {
   using type = double;
};
template <>
struct ValType<DECIMAL> {  // this one is special
   using type = double;
};
template <>
struct ValType<DATE> {
   using type = MYSQL_TIME;
};
template <>
struct ValType<DATETIME> {
   using type = MYSQL_TIME;
};
template <>
struct ValType<TIMESTAMP> {
   using type = MYSQL_TIME;
};
template <>
struct ValType<TIME> {
   using type = MYSQL_TIME;
};
template <>
struct ValType<ENUM> {
   using type = unsigned char;
};
template <>
struct ValType<SET> {
   using type = unsigned char;
};
template <>
struct ValType<BOOLEAN> {
   using type = signed char;
};
template <>
struct ValType<BIT> {
   using type = unsigned long;
};
template <>
struct ValType<GEOMETRY> {
   using type = unsigned char;
};
template <>
struct ValType<JSON> {
   using type = unsigned char;
};
template <>
struct ValType<BINARY> {
   using type = unsigned char;
};
template <>
struct ValType<VARBINARY> {
   using type = unsigned char;
};
template <>
struct ValType<TINYBLOB> {
   using type = unsigned char;
};

}  // namespace set_mysql_binds

#endif  // INCLUDED_SQLCTYPE_H