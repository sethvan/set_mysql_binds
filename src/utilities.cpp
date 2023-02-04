#include "utilities.h"

#include <algorithm>
#include <initializer_list>

namespace set_mysql_binds {

    static const std::array<std::string_view, 256> getFieldTypesArray() {
        std::array<std::string_view, 256> fieldTypes{
            "MYSQL_TYPE_DECIMAL",     "MYSQL_TYPE_TINY",
            "MYSQL_TYPE_SHORT",       "MYSQL_TYPE_LONG",
            "MYSQL_TYPE_FLOAT",       "MYSQL_TYPE_DOUBLE",
            "MYSQL_TYPE_NULL",        "MYSQL_TYPE_TIMESTAMP",
            "MYSQL_TYPE_LONGLONG",    "MYSQL_TYPE_INT24",
            "MYSQL_TYPE_DATE",        "MYSQL_TYPE_TIME",
            "MYSQL_TYPE_DATETIME",    "MYSQL_TYPE_YEAR",
            "MYSQL_TYPE_NEWDATE", /**< Internal to MySQL. Not used in protocol */
            "MYSQL_TYPE_VARCHAR",     "MYSQL_TYPE_BIT",
            "MYSQL_TYPE_TIMESTAMP2",  "MYSQL_TYPE_DATETIME2", /**< Internal to MySQL. Not used in protocol */
            "MYSQL_TYPE_TIME2",                               /**< Internal to MySQL. Not used in protocol */
            "MYSQL_TYPE_TYPED_ARRAY",                         /**< Used for replication only */
        };

        std::initializer_list<const char*> il = {
            "MYSQL_TYPE_INVALID",    "MYSQL_TYPE_BOOL", /**< Currently just a placeholder */
            "MYSQL_TYPE_JSON",       "MYSQL_TYPE_NEWDECIMAL",  "MYSQL_TYPE_ENUM",      "MYSQL_TYPE_SET",
            "MYSQL_TYPE_TINY_BLOB",  "MYSQL_TYPE_MEDIUM_BLOB", "MYSQL_TYPE_LONG_BLOB", "MYSQL_TYPE_BLOB",
            "MYSQL_TYPE_VAR_STRING", "MYSQL_TYPE_STRING",      "MYSQL_TYPE_GEOMETRY" };

        auto ftIt = std::next( fieldTypes.begin(), 243 );
        std::for_each( il.begin(), il.end(), [&]( const char* str ) { *ftIt++ = str; } );

        return fieldTypes;
    }

    const std::array<std::string_view, 256> fieldTypes = getFieldTypesArray();

    // Below are NOT all the enum_field_types for such data but just the permitted buffer types for use
    // in prepared statements
    bool isCharArray( enum_field_types type ) {  // The only char[] buffer types permitted in Output
        int enumValue = static_cast<int>( type );
        return ( ( enumValue > 248 && enumValue < 255 ) || enumValue == 246 );
    }

    bool isCharArrayReq( enum_field_types type ) {  // The only buffer types permitted in char[]
                                                    // request. It is subset of above
        return ( type == MYSQL_TYPE_BLOB || type == MYSQL_TYPE_STRING );
    }

    bool isTimeType( enum_field_types type ) {
        int enumValue = static_cast<int>( type );
        return ( enumValue == 7 || ( enumValue > 9 && enumValue < 13 ) );
    }
}  // namespace set_mysql_binds