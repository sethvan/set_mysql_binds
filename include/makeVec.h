#ifndef INCLUDED_MAKEVEC_H
#define INCLUDED_MAKEVEC_H

#include <memory>
#include <string_view>

#include "sqlTypes.hpp"

namespace set_mysql_binds {

    // from reddit user u/IyeOnline
    template <typename T, typename... Ptrs>
    auto make_vector( Ptrs&&... ptrs ) {
        std::vector<std::unique_ptr<T>> vec;
        ( vec.emplace_back( std::forward<Ptrs>( ptrs ) ), ... );
        return vec;
    }

    template <typename T>
    std::unique_ptr<InputCType> inBind( std::string_view name, unsigned long buffer_size = 0 ) {
        return std::make_unique<T>( name, buffer_size );
    }
    template <typename T>
    std::unique_ptr<OutputCType> outBind( std::string_view name, unsigned long buffer_size = 0 ) {
        return std::make_unique<T>( name, buffer_size );
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_MAKEVEC_H