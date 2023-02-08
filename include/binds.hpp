#ifndef INCLUDED_Binds_H
#define INCLUDED_Binds_H

#include <algorithm>
#include <exception>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "makeVec.h"
#include "sqlTypes.hpp"
#include "utilities.h"

/*
    Will be used to set up a prepared statement for creating, modifying or reading. Statement to be
   be prepared in another class. Once it is constructed, It's MYSQL_BIND array parameter is bound
   with a prepared statement that can be executed. If it is a write/modify statement then the values
   of each field can be updated between statement executions. If it is a read statement, then
   updated values can be read between executions if they have changed.
*/

namespace set_mysql_binds {

    template <typename T>
    class Binds {
       private:
        std::vector<std::unique_ptr<T>> columns;
        std::vector<MYSQL_BIND> selection;

       public:
        // Points to columns' elements. After object instantiated, do not want column elements
        // added or deleted, just access for selecting and modifying.
        std::vector<T*> fields;

        Binds() = delete;
        Binds( std::vector<std::unique_ptr<T>> _columns );  // To set once the correct order of
                                                            // MYSQL_BINDs for the prepared statement.

        void displayFields() const;
        void setBinds();
        void setBinds( const std::vector<std::string_view>& sc );
        MYSQL_BIND* getBinds() {
            return selection.data();
        }
        size_t getBindsSize() const {  // for testing during development
            return selection.size();
        }
    };

    template <typename T>
    Binds<T>::Binds( std::vector<std::unique_ptr<T>> _columns ) : columns( std::move( _columns ) ) {

        for ( size_t i = 0; i < columns.size(); ++i ) {
            fields.emplace_back( columns[i].get() );
        }
    }

    template <typename T>
    void Binds<T>::displayFields() const {
        puts( "" );
        std::cout << std::left << std::setw( 45 ) << "Field Name";
        std::cout << std::left << std::setw( 30 ) << "Field Type";
        std::cout << std::left << std::setw( 30 ) << "Field Value" << '\n';
        std::cout << std::left << std::setw( 105 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' ) << '\n';
        std::for_each( fields.begin(), fields.end(), [&]( const auto& o ) {
            std::cout << std::left << std::setw( 45 ) << o->fieldName;
            std::cout << std::left << std::setw( 30 ) << fieldTypes[o->bufferType];
            o->printValue();
            std::cout << std::endl;
        } );
        puts( "" );
    }

    template <typename T>
    void Binds<T>::setBinds() {
        std::for_each( columns.begin(), columns.end(), [&]( auto& o ) {
            if ( o->is_selected ) {
                selection.emplace_back();
                o->setBind( &( *std::next( selection.end(), -1 ) ) );
            }
        } );
    }

    template <typename T>
    void Binds<T>::setBinds( const std::vector<std::string_view>& sc ) {

        // to ensure arguments are valid
        std::for_each( sc.begin(), sc.end(), [&]( auto fieldName ) {
            auto found = std::find_if( columns.begin(), columns.end(),
                                       [&]( auto& column ) { return column->fieldName == fieldName; } );
            if ( found == columns.end() ) {
                std::ostringstream os;
                os << "Invalid selection \"" << fieldName << "\" not found in Binds object";
                throw std::runtime_error( std::move( os.str() ) );
            }
        } );

        // to ensure arguments are handled in the correct order
        std::for_each( columns.begin(), columns.end(), [&]( auto& column ) {
            auto it =
                std::find_if( sc.begin(), sc.end(), [&]( auto fieldName ) { return column->fieldName == fieldName; } );
            if ( it != sc.end() ) {
                column->is_selected = true;
            }
            else {
                column->is_selected = false;
            }
        } );

        selection.erase( selection.begin(), selection.end() );  // in case a previous selection was made
        Binds<T>::setBinds();
    }

}  // namespace set_mysql_binds

#endif  // INCLUDED_Binds_H