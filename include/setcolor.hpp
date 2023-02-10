#ifndef _INCLUDED_SETCOLOR_HPP
#define _INCLUDED_SETCOLOR_HPP

#include <stdio.h>

#include <iostream>
#include <sstream>

namespace set_mysql_binds {

    enum class Color {
        GREY = 2,
        WHITE_BACK = 7,
        DARK_GREY = 30,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        BLACK_BACK = 40,
        RED_BACK = 41,
        GREEN_BACK = 42,
        YELLOW_BACK = 43,
        BLUE_BACK = 44,
        MAG_BACK = 45,
        CYAN_BACK = 46
    };

    struct setcolor {

        setcolor( Color color ) : color{ color } {
        }

        template <typename T>
        std::ostream &operator<<( T const &t ) const {
            std::ostringstream os;
            os << t;
            char buf[os.str().size() + 20];
            int kolor = static_cast<int>( color );
            sprintf( buf, "\e[1;%dm%s\e[0m", kolor, os.str().c_str() );
            *out << buf;
            return *out;
        }

        friend setcolor const &operator<<( std::ostream &out, setcolor const &sColor ) {
            sColor.out = &out;
            return sColor;
        }

        mutable std::ostream *out;
        Color color;
    };
}  // namespace set_mysql_binds

#endif  // _INCLUDED_SETCOLOR_HPP