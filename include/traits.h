#ifndef INCLUDED_TRAITS_H
#define INCLUDED_TRAITS_H

#include "sqlTypes.hpp"

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
        using inType = TypeInputImpl<int, MYSQL_TYPE_LONG>;
        using outType = TypeOutputImpl<int, MYSQL_TYPE_LONG>;
    };
    template <>
    struct BindType<INT_UNSIGNED> {
        using inType = TypeInputImpl<unsigned int, MYSQL_TYPE_LONG>;
        using outType = TypeOutputImpl<unsigned int, MYSQL_TYPE_LONG>;
    };
    template <>
    struct BindType<CHAR> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<VARCHAR> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>;
    };
    template <>
    struct BindType<TINYTEXT> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>;
    };
    template <>
    struct BindType<TEXT> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<BLOB> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
        using outType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<MEDIUMTEXT> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<MEDIUMBLOB> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<LONGTEXT> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>;
    };
    template <>
    struct BindType<LONGBLOB> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_LONG_BLOB>;
    };
    template <>
    struct BindType<TINYINT> {
        using inType = TypeInputImpl<signed char, MYSQL_TYPE_TINY>;
        using outType = TypeOutputImpl<signed char, MYSQL_TYPE_TINY>;
    };
    template <>
    struct BindType<TINYINT_UNSIGNED> {
        using inType = TypeInputImpl<unsigned char, MYSQL_TYPE_TINY>;
        using outType = TypeOutputImpl<unsigned char, MYSQL_TYPE_TINY>;
    };
    template <>
    struct BindType<SMALLINT> {
        using inType = TypeInputImpl<short, MYSQL_TYPE_SHORT>;
        using outType = TypeOutputImpl<short, MYSQL_TYPE_SHORT>;
    };
    template <>
    struct BindType<SMALLINT_UNSIGNED> {
        using inType = TypeInputImpl<unsigned short, MYSQL_TYPE_SHORT>;
        using outType = TypeOutputImpl<unsigned short, MYSQL_TYPE_SHORT>;
    };
    template <>
    struct BindType<BIGINT> {
        using inType = TypeInputImpl<long long, MYSQL_TYPE_LONGLONG>;
        using outType = TypeOutputImpl<long long, MYSQL_TYPE_LONGLONG>;
    };
    template <>
    struct BindType<BIGINT_UNSIGNED> {
        using inType = TypeInputImpl<unsigned long long, MYSQL_TYPE_LONGLONG>;
        using outType = TypeOutputImpl<unsigned long long, MYSQL_TYPE_LONGLONG>;
    };
    template <>
    struct BindType<FLOAT> {
        using inType = TypeInputImpl<float, MYSQL_TYPE_FLOAT>;
        using outType = TypeOutputImpl<float, MYSQL_TYPE_FLOAT>;
    };
    template <>
    struct BindType<DOUBLE> {
        using inType = TypeInputImpl<double, MYSQL_TYPE_DOUBLE>;
        using outType = TypeOutputImpl<double, MYSQL_TYPE_DOUBLE>;
    };
    template <>
    struct BindType<DECIMAL> {
        using inType = TypeInputImpl<double, MYSQL_TYPE_DOUBLE>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_NEWDECIMAL>;
    };
    template <>
    struct BindType<DATE> {
        using inType = TypeMysqlTimeInput<MYSQL_TYPE_DATE>;
        using outType = TypeMysqlTimeOutput<MYSQL_TYPE_DATE>;
    };
    template <>
    struct BindType<DATETIME> {
        using inType = TypeMysqlTimeInput<MYSQL_TYPE_DATETIME>;
        using outType = TypeMysqlTimeOutput<MYSQL_TYPE_DATETIME>;
    };
    template <>
    struct BindType<TIMESTAMP> {
        using inType = TypeMysqlTimeInput<MYSQL_TYPE_TIMESTAMP>;
        using outType = TypeMysqlTimeOutput<MYSQL_TYPE_TIMESTAMP>;
    };
    template <>
    struct BindType<TIME> {
        using inType = TypeMysqlTimeInput<MYSQL_TYPE_TIME>;
        using outType = TypeMysqlTimeOutput<MYSQL_TYPE_TIME>;
    };
    template <>
    struct BindType<ENUM> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<SET> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_STRING>;
    };
    template <>
    struct BindType<BOOLEAN> {
        using inType = TypeInputImpl<signed char, MYSQL_TYPE_BOOL>;
        using outType = TypeOutputImpl<signed char, MYSQL_TYPE_BOOL>;
    };
    template <>
    struct BindType<BIT> {
        using inType = TypeOutputImpl<unsigned long, MYSQL_TYPE_BIT>;
        using outType = TypeOutputImpl<unsigned long, MYSQL_TYPE_BIT>;
    };
    template <>
    struct BindType<GEOMETRY> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<JSON> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_JSON>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_JSON>;
    };
    template <>
    struct BindType<BINARY> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_STRING>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_BLOB>;
    };
    template <>
    struct BindType<VARBINARY> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_BLOB>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>;
    };
    template <>
    struct BindType<TINYBLOB> {
        using inType = TypeCharArrayInput<MYSQL_TYPE_TINY_BLOB>;
        using outType = TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>;
    };

}  // namespace set_mysql_binds

#endif  // INCLUDED_TRAITS_H