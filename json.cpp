#include "json.h"
using namespace KJson;

Json::Json() : _type(JSON_NULL) 
{

}
Json::Json(bool value) : _type(JSON_BOOL)   
{
    _value._bool = value;
}
Json::Json(int value) : _type(JSON_INT)   
{
    _value._int = value;
}
Json::Json(double value) : _type(JSON_DOUBLE)   
{
    _value._double = value;
}
Json::Json(const std::string& value) : _type(JSON_STRING)   
{
    _value._string = new std::string(value);
}
Json::Json(std::vector<Json> value) : _type(JSON_ARRAY)   
{
    _value._array = new std::vector<Json>(value);
}
Json::Json(std::map<std::string, Json> value) : _type(JSON_OBJECT)   
{
    _value._object = new std::map<std::string, Json>(value);
}
Json::Json(JsonType type) : _type(type)   
{
    switch (type)
    {
    case JSON_NULL:
        break;
    case JSON_BOOL:
        _value._bool = false;
        break;
    case JSON_INT:
        _value._int = 0;
        break;
    case JSON_DOUBLE:
        _value._double = 0.0;
        break;
    case JSON_STRING:
        _value._string = new std::string("");
        break;
    case JSON_ARRAY:
        _value._array = new std::vector<Json>();
        break;
    case JSON_OBJECT:
        _value._object = new std::map<std::string, Json>();
        break;
    }

}
//注意目前的拷贝实现为浅拷贝方式

Json::Json(const Json& other) : _type(other._type)   
{
    switch (other._type)
    {
    case JSON_NULL:
        break; 
    case JSON_BOOL:
        _value._bool = other._value._bool;
        break;
    case JSON_INT:
        _value._int = other._value._int;
        break;  
    case JSON_DOUBLE:
        _value._double = other._value._double;
        break;
    case JSON_STRING:
        _value._string = other._value._string;
        break;
    case JSON_ARRAY:
        _value._array = other._value._array;
        break;
    case JSON_OBJECT:
        _value._object = other._value._object;
        break;
    }
}
