#ifndef INCLUDED_VALTYPES_H
#define INCLUDED_VALTYPES_H

#include <SqlTypes/SqlCType.h>

namespace set_mysql_binds {

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
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<TINYTEXT> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<TEXT> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<BLOB> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<MEDIUMTEXT> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<MEDIUMBLOB> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<LONGTEXT> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<LONGBLOB> {
   using type = std::basic_string<unsigned char>;
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
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<SET> {
   using type = std::basic_string<unsigned char>;
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
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<JSON> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<BINARY> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<VARBINARY> {
   using type = std::basic_string<unsigned char>;
};
template <>
struct ValType<TINYBLOB> {
   using type = std::basic_string<unsigned char>;
};

}  // namespace set_mysql_binds
#endif  // INCLUDED_VALTYPES_H