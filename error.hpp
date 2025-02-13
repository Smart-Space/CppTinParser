#include <iostream>
#include <exception>
using namespace std;


class MuchArgs : public exception {
public:
    MuchArgs(char tag){
        this->tag = tag;
    }
    const char *what() const throw() { return "Too many arguments for <"+this->tag+'>'; }
private:
    char tag;
};


class NoTagName : public exception {
public:
    NoTagName(char tag){
        this->tag = tag;
    }
    const char *what() const throw() { return "No tag name: "+this->tag; }
private:
    char tag;
};