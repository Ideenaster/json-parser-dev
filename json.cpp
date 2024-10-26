#include "json.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
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
    default:
        _value._int = 0;
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

void Json::push_back(Json&& value)//右值移动语义用于处理临时对象 ，比如函数返回值
{
    if(_type != JSON_ARRAY)
        throw std::runtime_error("Json is not a array");
    _value._array->push_back(std::move(value));
    value._type = JSON_NULL;
    value._value._int = 0;
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
                if (i != array_size() - 1)ss << ",";
            }
            ss<<"]";
            break;
        case JSON_OBJECT:
            ss<<"{";
            for(auto it = (*_value._object).begin(); it != (*_value._object).end(); it++){
                ss<<'"'<<it->first<<'"'<<":"<<it->second.to_string();
                if(it != std::prev((*_value._object).end()))
                    ss<<",";
            }
            ss<<"}";
            break;
        default:
            break;
    }
    return ss.str();
}


//解析器
void Json::skip_space(){
    while(_pos < _str.size() && isspace(_str[_pos]))
        _pos++;
}

char Json::get_next(){
    skip_space();
    if(_pos >= _str.size())
        throw std::runtime_error("Unexpected end of input");
    return _str[_pos++];
}


Json Json::parse(){
    skip_space();
    char ch = get_next();
    switch(ch)
    {
        case 'n':
            _pos--;
            return parse_null();
        case 't':
        case 'f':
            _pos--;
            return parse_bool();
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            _pos--;
            return parse_number(); 
        case '"':
            return parse_string();
        case '[':
            return parse_array();
        case '{':
            return parse_object();
        default:
            std::cout << "Unexpected character is : " << static_cast<int>(ch) << std::endl;
    }
    throw std::logic_error("Unexpected Character");
}

Json Json::parse_null(){
    if(_str.compare(_pos, 4, "null") == 0){
        _pos += 4;
        return Json(Json::JSON_NULL);
    }
    throw std::logic_error("Unexpected null value , position is " + std::to_string(_pos));
}

Json Json::parse_bool(){
    if(_str.compare(_pos, 4, "true") == 0){
        _pos += 4;
        return Json(true);
    }
    else if(_str.compare(_pos, 5, "false") == 0){
        _pos += 5;
        return Json(false);
    }
    throw std::logic_error("Unexpected bool value , position is " + std::to_string(_pos));
}

Json Json::parse_number(){
    size_t end = _pos;
    bool double_flag = false;
    while (end < _str.size() && ((isdigit(_str[end]) || _str[end] == '.') || _str[end] == 'e' || _str[end] == 'E')) {
        if (_str[end] == '.' || _str[end] == 'e' || _str[end] == 'E')
            double_flag = true;
        end++;
    }
    size_t _pos_o = _pos;
    _pos = end;
    if(double_flag)
        return Json(std::stod(_str.substr(_pos_o, end - _pos_o)));
    else
        return Json(std::stoi(_str.substr(_pos_o, end - _pos_o)));
}

Json Json::parse_string(){
    if(_str.find('"', _pos) == std::string::npos)
        throw std::logic_error("Format error: Unexpected string value, position is " + std::to_string(_pos));
    size_t _pos_o = _pos;
    size_t end = _str.find('"', _pos);
    _pos = end+1;
    return Json(_str.substr(_pos_o, end - _pos_o));
}

Json Json::parse_array()
{
    Json arr(JSON_ARRAY); // 声明一个数组类型的Json对象
    char ch = get_next();
    if(ch == ']') {
        return arr; // 返回空数组类型
    }
    _pos--;
    while(true) {
        arr.push_back(parse()); 
        ch = get_next();
        if(ch == ']') {
            break;
        }
        if(ch != ',') { // 下个字符是否含逗号
            throw std::logic_error("Expected ',' in array, position is " + std::to_string(_pos));
        }
        else { 
            _pos--;
        }
        _pos++;
    }
    return arr;
}


Json Json::parse_object()
{
    Json obj(JSON_OBJECT);    
    char ch = get_next();
    if(ch == '}') {
        return obj; // 返回空对象
    }
    _pos--;
    while(true) {
        ch = get_next();
        if(ch != '"') {
            throw std::logic_error("parse object error");
        }
        std::string key = *(parse_string()._value._string); // 解析键key
        ch = get_next();
        if(ch != ':') {
            throw std::logic_error("parse object error");
        }
        obj[key] = parse(); // 递归解析值value
        ch = get_next();
        if(ch == '}') {
            break;
        }
        if(ch != ',') {
            throw std::logic_error("parse object error");
        }
    }
    return obj;
}

void Json::parseStr(const std::string &str)
{
    _str = str;
    _pos = 0;
    Json temp = parse();
    *this = std::move(temp);
    temp._type = JSON_NULL;
    temp._value._int = 0;
}


Json& Json::operator=(Json&& other)
{
    if (this != &other) {
        clear();
        _type = other._type;
        _value = other._value;
        other._type = JSON_NULL;
        other._value._int = 0;
    }
    return *this;
}

Json& Json::operator=(const Json& other)
{
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}
