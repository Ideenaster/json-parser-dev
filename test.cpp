#include "json.h"
//#include "json.cpp"
#include <iostream>
using namespace KJson;
int main()
{
    Json json;
    Json str("azz");
    json.insert("name", str);
    json.insert("age", 20);
    json.insert("score", 85.5);
    std::cout << json.to_string() << std::endl;
    return 0;
}
