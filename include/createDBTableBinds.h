#ifndef INCLUDED_CREATEDBTABLEBINDS_H
#define INCLUDED_CREATEDBTABLEBINDS_H

#include <getDBTables.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace set_mysql_binds {

    void createDBTableBinds( const std::string& host, const std::string& user, const std::string& password,
                             const std::string& database, const std::string& declFile, const std::string& defnFile,
                             const std::string& includeStr,
                             unsigned long buff_size );  // buff_size will be the buff size for all the
                                                         // binds created that are char[]

}  // namespace set_mysql_binds

#endif  // INCLUDED_CREATEDBTABLEBINDS_H