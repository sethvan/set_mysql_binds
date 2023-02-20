#ifndef INCLUDED_TRAITS_H
#define INCLUDED_TRAITS_H

#include "SqlTypes/SqlTypes.h"

namespace set_mysql_binds {

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

    using enum MysqlInputType;

    // BindType *******************************************************
    template <MysqlInputType T>
    struct BindType {};

    template <>
    struct BindType<INT> {
        using inType = InImpl<int, MYSQL_TYPE_LONG>;
        using outType = OutImpl<int, MYSQL_TYPE_LONG>;
    };
    template <>
    struct BindType<INT_UNSIGNED> {
        using inType = InImpl<unsigned int, MYSQL_TYPE_LONG>;
        using outType = OutImpl<unsigned int, MYSQL_TYPE_LONG>;
    };
    template <>
    struct BindType<CHAR> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<VARCHAR> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_VAR_STRING>;
    };
    template <>
    struct BindType<TINYTEXT> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_TINY_BLOB>;
    };
    template <>
    struct BindType<TEXT> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<BLOB> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<MEDIUMTEXT> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<MEDIUMBLOB> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<LONGTEXT> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<LONGBLOB> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_LONG_BLOB>;
    };
    template <>
    struct BindType<TINYINT> {
        using inType = InImpl<signed char, MYSQL_TYPE_TINY>;
        using outType = OutImpl<signed char, MYSQL_TYPE_TINY>;
    };
    template <>
    struct BindType<TINYINT_UNSIGNED> {
        using inType = InImpl<unsigned char, MYSQL_TYPE_TINY>;
        using outType = OutImpl<unsigned char, MYSQL_TYPE_TINY>;
    };
    template <>
    struct BindType<SMALLINT> {
        using inType = InImpl<short, MYSQL_TYPE_SHORT>;
        using outType = OutImpl<short, MYSQL_TYPE_SHORT>;
    };
    template <>
    struct BindType<SMALLINT_UNSIGNED> {
        using inType = InImpl<unsigned short, MYSQL_TYPE_SHORT>;
        using outType = OutImpl<unsigned short, MYSQL_TYPE_SHORT>;
    };
    template <>
    struct BindType<BIGINT> {
        using inType = InImpl<long, MYSQL_TYPE_LONGLONG>;
        using outType = OutImpl<long, MYSQL_TYPE_LONGLONG>;
    };
    template <>
    struct BindType<BIGINT_UNSIGNED> {
        using inType = InImpl<unsigned long, MYSQL_TYPE_LONGLONG>;
        using outType = OutImpl<unsigned long, MYSQL_TYPE_LONGLONG>;
    };
    template <>
    struct BindType<FLOAT> {
        using inType = InImpl<float, MYSQL_TYPE_FLOAT>;
        using outType = OutImpl<float, MYSQL_TYPE_FLOAT>;
    };
    template <>
    struct BindType<DOUBLE> {
        using inType = InImpl<double, MYSQL_TYPE_DOUBLE>;
        using outType = OutImpl<double, MYSQL_TYPE_DOUBLE>;
    };
    template <>
    struct BindType<DECIMAL> {
        using inType = InImpl<double, MYSQL_TYPE_DOUBLE>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_NEWDECIMAL>;
    };
    template <>
    struct BindType<DATE> {
        using inType = InImpl<MYSQL_TIME, MYSQL_TYPE_DATE>;
        using outType = OutImpl<MYSQL_TIME,MYSQL_TYPE_DATE>;
    };
    template <>
    struct BindType<DATETIME> {
        using inType = InImpl<MYSQL_TIME, MYSQL_TYPE_DATETIME>;
        using outType = OutImpl<MYSQL_TIME,MYSQL_TYPE_DATETIME>;
    };
    template <>
    struct BindType<TIMESTAMP> {
        using inType = InImpl<MYSQL_TIME, MYSQL_TYPE_TIMESTAMP>;
        using outType = OutImpl<MYSQL_TIME,MYSQL_TYPE_TIMESTAMP>;
    };
    template <>
    struct BindType<TIME> {
        using inType = InImpl<MYSQL_TIME, MYSQL_TYPE_TIME>;
        using outType = OutImpl<MYSQL_TIME,MYSQL_TYPE_TIME>;
    };
    template <>
    struct BindType<ENUM> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<SET> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<BOOLEAN> {
        using inType = InImpl<signed char, MYSQL_TYPE_BOOL>;
        using outType = OutImpl<signed char, MYSQL_TYPE_BOOL>;
    };
    template <>
    struct BindType<BIT> {
        using inType = OutImpl<unsigned long, MYSQL_TYPE_BIT>;
        using outType = OutImpl<unsigned long, MYSQL_TYPE_BIT>;
    };
    template <>
    struct BindType<GEOMETRY> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<JSON> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_JSON>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_JSON>;
    };
    template <>
    struct BindType<BINARY> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_STRING>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<VARBINARY> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_VAR_STRING>;
    };
    template <>
    struct BindType<TINYBLOB> {
        using inType = InImpl<std::basic_string<unsigned char>, MYSQL_TYPE_TINY_BLOB>;
        using outType = OutImpl<std::basic_string<unsigned char>,MYSQL_TYPE_TINY_BLOB>;
    };

}  // namespace set_mysql_binds

#endif  // INCLUDED_TRAITS_H