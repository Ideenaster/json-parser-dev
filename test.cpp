#include "json.h"
//#include "json.cpp"
#include <iostream>
#include <string>
using namespace KJson;
int main()
{
    /*Json json;
    Json str("azz");
    json.insert("name", str);
    json.insert("age", 20);
    json.insert("score", 85.5);
    std::cout << json.to_string() << std::endl;*/
    std::string teststr = "[true,{\"fsdfsd\":[1,2,3,4]},1.2203,null,\"dfshsjdkhf\"]";
    Json j;
    j.parseStr(teststr);
    std::cout << j.to_string() << std::endl;
    return 0;
}
