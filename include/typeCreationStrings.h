#ifndef INCLUDED_TYPECREATIONSTRINGS_H
#define INCLUDED_TYPECREATIONSTRINGS_H

#include <array>
#include <string_view>

namespace set_mysql_binds {

    // Taken from Jason Turner YT video
    // https://www.youtube.com/watch?v=INn3xa4pMfg&list=PLs3KjaCtOwSaqPapPV4pc1SRjypnwrXYV&index=8

    template <typename Key, typename Value, std::size_t Size>
    struct TurnerMap {
        std::array<std::pair<Key, Value>, Size> data;

        [[nodiscard]] constexpr Value at( const Key& key ) const {
            const auto it =
                std::find_if( data.begin(), data.end(), [&key]( const auto& v ) { return v.first == key; } );
            if ( it != data.end() ) {
                return it->second;
            }
            else {
                throw std::range_error( "Not Found" );
            }
        }
    };

    struct DataType {
        std::string_view cTypeInputSigned;
        std::string_view cTypeInputUnsigned;
        std::string_view cTypeOutputSigned;
        std::string_view cTypeOutputUnsigned;
    };

    // need to add a couple of types such as 'year' and maybe some spatial ones, need to check
    constexpr std::array<std::pair<std::string_view, DataType>, 30> userInputTypes = {
        { { "char",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)" } },
          { "varchar",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>>( \"NAME\", BUFF_SIZE)" } },
          { "tinytext",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "text",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "blob",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "mediumtext",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "mediumblob",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "longtext",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_MEDIUM_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "longblob",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_LONG_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_LONG_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "tinyint",
            { "std::make_unique<InImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<InImpl<unsigned char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<OutImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned char, MYSQL_TYPE_TINY>>(\"NAME\")" } },
          { "smallint",
            { "std::make_unique<InImpl<short, MYSQL_TYPE_SHORT>>(\"NAME\")",
              "std::make_unique<InImpl<unsigned short, MYSQL_TYPE_SHORT>>(\"NAME\")",
              "std::make_unique<OutImpl<short, MYSQL_TYPE_SHORT>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned short, MYSQL_TYPE_SHORT>>(\"NAME\")" } },
          { "mediumint",
            { "std::make_unique<InImpl<int, MYSQL_TYPE_LONG>>(\"NAME\")",
              "std::make_unique<InImpl<unsigned int, MYSQL_TYPE_LONG>>(\"NAME\")",
              "std::make_unique<OutImpl<int MYSQL_TYPE_INT24>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned int, MYSQL_TYPE_INT24>>(\"NAME\")" } },
          { "int",
            { "std::make_unique<InImpl<int, MYSQL_TYPE_LONG>>(\"NAME\")",
              "std::make_unique<InImpl<unsigned int, MYSQL_TYPE_LONG>>(\"NAME\")",
              "std::make_unique<OutImpl<int, MYSQL_TYPE_LONG>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned int, MYSQL_TYPE_LONG>>(\"NAME\")" } },
          { "bigint",
            { "std::make_unique<InImpl<long long, MYSQL_TYPE_LONGLONG>>(\"NAME\")",
              "std::make_unique<InImpl<unsigned long long, MYSQL_TYPE_LONGLONG>>(\"NAME\")",
              "std::make_unique<OutImpl<long long, MYSQL_TYPE_LONGLONG>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned long long, MYSQL_TYPE_LONGLONG>>(\"NAME\")" } },
          { "float",
            { "std::make_unique<InImpl<float, MYSQL_TYPE_FLOAT>>(\"NAME\")",
              "std::make_unique<InImpl<float, MYSQL_TYPE_FLOAT>>(\"NAME\")",
              "std::make_unique<OutImpl<float, MYSQL_TYPE_FLOAT>>(\"NAME\")",
              "std::make_unique<OutImpl<float, MYSQL_TYPE_FLOAT>>(\"NAME\")" } },
          { "double",
            { "std::make_unique<InImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")",
              "std::make_unique<InImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")",
              "std::make_unique<OutImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")",
              "std::make_unique<OutImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")" } },
          { "decimal",
            { "std::make_unique<InImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")",
              "std::make_unique<InImpl<double, MYSQL_TYPE_DOUBLE>>(\"NAME\")",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_NEWDECIMAL>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_NEWDECIMAL>>( \"NAME\", BUFF_SIZE)" } },
          { "date",
            { "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_DATE>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_DATE>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_DATE>>( \"NAME\")",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_DATE>>( \"NAME\")" } },
          { "datetime",
            { "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_DATETIME>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_DATETIME>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_DATETIME>>( \"NAME\")",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_DATETIME>>( \"NAME\")" } },
          { "timestamp",
            { "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_TIMESTAMP>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_TIMESTAMP>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_TIMESTAMP>>( \"NAME\")",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_TIMESTAMP>>( \"NAME\")" } },
          { "time",
            { "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_TIME>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeInput<MYSQL_TYPE_TIME>>( \"NAME\" )",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_TIME>>( \"NAME\")",
              "std::make_unique<TypeMysqlTimeOutput<MYSQL_TYPE_TIME>>( \"NAME\")" } },
          { "enum",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)"
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)" } },
          { "set",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)"
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)" } },
          { "boolean",
            { "std::make_unique<InImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<InImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<OutImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")",
              "std::make_unique<OutImpl<signed char, MYSQL_TYPE_TINY>>(\"NAME\")" } },
          { "bit",
            { "std::make_unique<OutImpl<unsigned long, MYSQL_TYPE_BIT>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned long, MYSQL_TYPE_BIT>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned long, MYSQL_TYPE_BIT>>(\"NAME\")",
              "std::make_unique<OutImpl<unsigned long, MYSQL_TYPE_BIT>>(\"NAME\")" } },
          { "geometry",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "json",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_JSON>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_JSON>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_JSON>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_JSON>>( \"NAME\", BUFF_SIZE)" } },
          { "binary",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)" } },
          { "varbinary",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_VAR_STRING>>( \"NAME\", BUFF_SIZE)" } },
          { "tinyblob",
            { "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayInput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)",
              "std::make_unique<TypeCharArrayOutput<MYSQL_TYPE_TINY_BLOB>>( \"NAME\", BUFF_SIZE)" } } } };

    constexpr auto typeCreationStrings =
        TurnerMap<std::string_view, DataType, userInputTypes.size()>{ { userInputTypes } };

}  // namespace set_mysql_binds

#endif  // INCLUDED_TYPECREATIONSTRINGS_H