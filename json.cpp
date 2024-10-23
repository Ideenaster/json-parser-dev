#include "json.h"
#include <stdexcept>
#include <sstream>
using namespace KJson;

Json::Json() : _type(JSON_NULL) 
{
    _value._int = 0;
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
Json::Json(const char* value) : _type(JSON_STRING)
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
        _value._int = 0;
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

void Json::clear()
{
    switch(_type)
    {
        case JSON_NULL:
        case JSON_BOOL:
        case JSON_INT:
        case JSON_DOUBLE:
            break;
        case JSON_STRING:
        {
            if(_value._string != nullptr) {
                delete _value._string;
                _value._string = nullptr;
            }
            break;
        }   
        case JSON_ARRAY:
        {
            if(_value._array != nullptr) {
                for(auto it = (_value._array)->begin(); it != (_value._array)->end(); it++) {
                    it->clear(); // 递归调用
                }
                delete _value._array; // 数组里的内容可能也涉及动态内存分配，要先一步释放
                _value._array = nullptr;
            }
            break;
        }
        case JSON_OBJECT:
        {
            if(_value._object != nullptr) {
                for(auto it = (_value._object)->begin(); it != (_value._object)->end(); it++) {
                    (it->second).clear(); 
                }
                delete _value._object;
                _value._object = nullptr;
                break;
            }
            break;
        }
        default:
            break;
        _type = JSON_NULL; // 类型设为空
    }
}

Json::~Json()
{
    clear();
}


void Json::copy(const Json& other)
{
    switch(other._type){
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
        _value._string = new std::string(*other._value._string);
        break;
    case JSON_ARRAY:
        _value._array = new std::vector<Json>(*other._value._array);
        break;
    case JSON_OBJECT:
        _value._object = new std::map<std::string, Json>(*other._value._object);
        break;
    }
}

Json::Json(const Json& other) : _type(other._type)   
{
    copy(other);
    //目前为深拷贝
}

//数组API
void Json::push_back(const Json& value)
{
    if(_type != JSON_ARRAY)
        throw std::runtime_error("Json is not a array");
    _value._array->push_back(value);
}
Json& Json::operator[](size_t index)
{
    if(_type != JSON_ARRAY)
        throw std::runtime_error("Json is not a array");
    return (*_value._array)[index];
}
size_t Json::array_size() const
{
    if(_type != JSON_ARRAY)
        throw std::runtime_error("Json is not a array");
    return _value._array->size();
}


//对象API
void Json::insert(const std::string& key, const Json& value)
{
    if(_type != JSON_OBJECT){
        clear();
        _type = JSON_OBJECT;
        _value._object = new std::map<std::string, Json>();
    }
    (*_value._object)[key] = value;
}
Json& Json::operator[](const std::string& key)
{
    if(_type != JSON_OBJECT)
        throw std::logic_error("Json is not a object");
    return (*_value._object)[key];
}
size_t Json::object_size() const
{
    if(_type != JSON_OBJECT)
        throw std::runtime_error("Json is not a object");
    return _value._object->size();

}


//序列化
std::string Json::to_string() const
{
    std::stringstream ss;
    switch(_type){
        case JSON_NULL:
            ss<<"null";
            break;
        case JSON_BOOL:
            ss<<(_value._bool ? "true" : "false");
            break;
        case JSON_INT:
            ss<<std::to_string(_value._int);
            break;
        case JSON_DOUBLE:
            ss<<std::to_string(_value._double);
            break;  
        case JSON_STRING:
            ss<<'"'<<*_value._string<<'"';
            break;
        case JSON_ARRAY:
            ss<<"[";
            for(size_t i = 0; i < array_size(); i++){
                ss<<(*_value._array)[i].to_string();
            }
            ss<<"]";
            break;
        case JSON_OBJECT:
            ss<<"{";
            for(auto it = (*_value._object).begin(); it != (*_value._object).end(); it++){
                ss<<'"'<<it->first<<'"'<<":"<<it->second.to_string();
                if(it != (*_value._object).end())
                    ss<<",";
            }
            ss<<"}";
            break;
        default:
            break;
    }
    return ss.str();
}