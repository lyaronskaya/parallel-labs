#ifndef CommandException_h
#define CommandException_h

#include <stdexcept>
#include <string>

using namespace std;

class IncorrectCommandException: public std::runtime_error {
public:
    explicit IncorrectCommandException(const std::string& what) : std::runtime_error(what) {}
};


class PthreadException: public std::runtime_error {
public:
    explicit PthreadException(const std::string& what) : std::runtime_error(what) {}
};

class MPIException: public std::runtime_error {
public:
    explicit MPIException(const std::string& what) : std::runtime_error(what) {}
};

#endif
