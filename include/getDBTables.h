#ifndef INCLUDED_GETDBTABLES_H
#define INCLUDED_GETDBTABLES_H
#include <mysql/mysql.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace set_mysql_binds {

    struct Field {
        std::string name;
        enum_field_types type;
        unsigned long int flags;
        std::string externalType;
    };

    struct Table {
        std::string name;
        std::vector<Field> fields;
    };

    std::vector<Table> getDBTables( const std::string& host, const std::string& user, const std::string& password,
                                    const std::string& database );
    void printDBTables( const std::string& host, const std::string& user, const std::string& password,
                        const std::string& database );
    void printDBTables( std::span<const Table> tables );

}  // namespace set_mysql_binds

#endif  // INCLUDED_GETTABLES_H