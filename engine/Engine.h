#ifndef ENGINE_ENGINE_H_
#define ENGINE_ENGINE_H_

#include "../parser/Driver.h"
#include "execute.h"

#include <string>
#include <locale>

class Engine
{
public:
    Engine() : qm(&sm), visitor(&qm) {}

    virtual void run() = 0;
    std::string execute(std::string& input);

protected:
    Driver driver;
    QueryManager qm;
    SystemManager sm;
    ExecuteVisitor visitor;
};

// http://coliru.stacked-crooked.com/a/6088d39e5db3c18d
template< typename C, typename T, typename A >
std::basic_string<C,T,A> trim( const std::basic_string<C,T,A>& str,
                               const std::locale& loc = std::locale::classic() )
{
    auto begin = str.begin() ; // first non ws from left
    while( begin != str.end() && std::isspace( *begin, loc ) ) ++begin ;
    if( begin == str.end() ) return {} ; // EDIT

    auto rbegin = str.rbegin() ; // first non ws from right
    while( rbegin != str.rend() && std::isspace( *rbegin, loc ) ) ++rbegin ;

    return { begin, rbegin.base() } ;
}

class InteractiveEngine : public Engine
{
public:
    InteractiveEngine() {}

    virtual void run();
};

#endif // ENGINE_ENGINE_H_
