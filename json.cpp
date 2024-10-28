#include "json.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <stack>
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
Json::Json(const std::string &value) : _type(JSON_STRING)   
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
Json::Json(std::unordered_map<std::string, Json> value) : _type(JSON_OBJECT)   
{
    _value._object = new std::unordered_map<std::string, Json>(value);
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
        _value._object = new std::unordered_map<std::string, Json>();
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
        _value._object = new std::unordered_map<std::string, Json>(*other._value._object);
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
        _value._object = new std::unordered_map<std::string, Json>();
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
std::string Json::to_pretty_string(int indent_level = 0) const
{
    const std::string indent(indent_level * 2, ' '); // 每级缩进两个空格
    std::stringstream ss;

    switch(_type) {
        case JSON_NULL:
            ss << "null";
            break;
        case JSON_BOOL:
            ss << (_value._bool ? "true" : "false");
            break;
        case JSON_INT:
            ss << _value._int;
            break;
        case JSON_DOUBLE:
            ss << _value._double;
            break;
        case JSON_STRING:
            ss << '"' << *_value._string << '"';
            break;
        case JSON_ARRAY:
            if (_value._array->empty()) {
                ss << "[]";
            } else {
                ss << "[\n";
                for (size_t i = 0; i < _value._array->size(); ++i) {
                    ss << indent << "  " << (*_value._array)[i].to_pretty_string(indent_level + 1);
                    if (i < _value._array->size() - 1) {
                        ss << ",";
                    }
                    ss << "\n";
                }
                ss << indent << "]";
            }
            break;
        case JSON_OBJECT:
            if (_value._object->empty()) {
                ss << "{}";
            } else {
                ss << "{\n";
                auto it = _value._object->begin();
                auto end = _value._object->end();
                while (it != end) {
                    ss << indent << "  \"" << it->first << "\": " << it->second.to_pretty_string(indent_level + 1);
                    if (++it != end) {
                        ss << ",";
                    }
                    ss << "\n";
                }
                ss << indent << "}";
            }
            break;
    }
    return ss.str();
}

//XML序列化
std::string Json::to_XML(int indent_level = 0) const 
{
    const std::string indent(indent_level * 2, ' '); // 每级缩进两个空格
    std::stringstream ss;

    switch(_type) {
        case JSON_NULL:
            ss << indent << "<null/>";
            break;
        case JSON_BOOL:
            ss << indent << "<boolean>" << (_value._bool ? "true" : "false") << "</boolean>";
            break;
        case JSON_INT:
            ss << indent << "<number>" << _value._int << "</number>";
            break;
        case JSON_DOUBLE:
            ss << indent << "<number>" << _value._double << "</number>";
            break;
        case JSON_STRING:
            ss << indent << "<string>" << *_value._string << "</string>";
            break;
        case JSON_ARRAY:
            ss << indent << "<array>\n";
            for (size_t i = 0; i < _value._array->size(); ++i) {
                ss << (*_value._array)[i].to_XML(indent_level + 1);
                ss << "\n";
            }
            ss << indent << "</array>";
            break;
        case JSON_OBJECT:
            ss << indent << "<object>\n";
            for (const auto& pair : *_value._object) {
                ss << indent << "    <key>" << pair.first << "</key>\n";
                ss << indent << "    <value>\n";
                ss << pair.second.to_XML(indent_level + 3);
                ss << "\n" << indent << "    </value>\n";
            }
            ss << indent << "</object>";
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

// 解析状态
enum ParseState {
    PARSE_VALUE,
    PARSE_OBJECT_KEY,
    PARSE_OBJECT_VALUE,
    PARSE_ARRAY_VALUE
};

// 存储解析上下文
struct ParseContext {
    ParseState state;
    Json* json;
    std::string key;  // 用于对象的键
    ParseContext(ParseState s, Json* j) : state(s), json(j) {}
};

Json Json::parse() {
    std::stack<ParseContext> parseStack;
    Json root;
    parseStack.push(ParseContext(PARSE_VALUE, &root));

    while (!parseStack.empty()) {
        ParseContext& context = parseStack.top();
        
        skip_space();
        if (_pos >= _str.size()) {
            if (parseStack.size() > 1) {  // 如果不是最外层，说明JSON未正确结束
                throw std::runtime_error("Unexpected end of input");
            }
            break;  // 正常结束
        }

        char ch = _str[_pos];
        
        switch (context.state) {
            case PARSE_VALUE:
                if (ch == 'n') {
                    try{
                        *context.json = parse_null();
                    }
                    catch(std::logic_error& e){
                        throw std::logic_error(std::string(e.what()) + 
                          "\nContext: " + "PARSE_VALUE" + 
                          "\nPosition: " + std::to_string(_pos));
                    }
                    parseStack.pop();
                } else if (ch == 't' || ch == 'f') {
                    try{
                        *context.json = parse_bool();
                    }
                    catch(std::logic_error& e){
                        throw std::logic_error(std::string(e.what()) + 
                          "\nContext: " + "PARSE_VALUE" + 
                          "\nPosition: " + std::to_string(_pos));
                    }
                    parseStack.pop();
                } else if (ch == '-' || (ch >= '0' && ch <= '9')) {
                    try{
                        *context.json = parse_number();
                    }
                    catch(std::logic_error& e){
                        throw std::logic_error(std::string(e.what()) + 
                          "\nContext: " + "PARSE_VALUE" + 
                          "\nPosition: " + std::to_string(_pos));
                    }
                    parseStack.pop();
                } else if (ch == '"') {
                    _pos++;
                    try{
                        *context.json = parse_string();
                    }
                    catch(std::logic_error& e){
                        throw std::logic_error(std::string(e.what()) + 
                          "\nContext: " + "PARSE_VALUE" + 
                          "\nPosition: " + std::to_string(_pos));
                    }
                    parseStack.pop();
                } else if (ch == '[') {
                    *context.json = Json(JSON_ARRAY);
                    _pos++;
                    parseStack.push(ParseContext(PARSE_ARRAY_VALUE, context.json));
                } else if (ch == '{') {
                    *context.json = Json(JSON_OBJECT);
                    _pos++;
                    parseStack.push(ParseContext(PARSE_OBJECT_KEY, context.json));
                } else {
                    throw std::logic_error(std::string("parser can not identify the type of the value") +
                    "\nContext: " + "PARSE_VALUE" +
                    "\nPosition: " + std::to_string(_pos));
                }
                break;

            case PARSE_OBJECT_KEY:
                if (ch == '}') {
                    _pos++;
                    parseStack.pop();
                    // 如果这是最后一个状态且是初始的PARSE_VALUE，则结束解析
                    if (parseStack.size() == 1 && parseStack.top().state == PARSE_VALUE) {
                        parseStack.pop();
                    } else {
                        // 否则检查后续的逗号
                        skip_space();
                        if (_pos < _str.size() && _str[_pos] == ',') {
                            _pos++;
                        }
                    }
                } else {
                    if (ch != '"') {
                        throw std::logic_error(std::string("Expected '\"' for object key") +
                        "\nContext: " + "PARSE_OBJECT_KEY" +
                        "\nPosition: " + std::to_string(_pos));
                    }
                    _pos++;
                    Json keyJson(JSON_STRING);
                    try{
                        keyJson = parse_string();
                    }
                    catch(std::logic_error& e){
                        throw std::logic_error(std::string(e.what()) + 
                          "\nContext: " + "PARSE_OBJECT_KEY" + 
                          "\nPosition: " + std::to_string(_pos));
                    }
                    context.key = *keyJson._value._string;
                    
                    skip_space();
                    if (_str[_pos] != ':') {
                        throw std::logic_error(std::string("Expected ':' after object key string") +
                        "\nContext: " + "PARSE_OBJECT_KEY" +
                        "\nPosition: " + std::to_string(_pos));
                    }
                    _pos++;
                    context.state = PARSE_OBJECT_VALUE;   //状态转移 
                }
                break;

            case PARSE_OBJECT_VALUE: {
                Json* valuePtr = &((*context.json)[context.key]);
                if (ch == '{') {
                    *valuePtr = Json(JSON_OBJECT);
                    _pos++;
                    parseStack.push(ParseContext(PARSE_OBJECT_KEY, valuePtr));
                } else if (ch == '[') {
                    *valuePtr = Json(JSON_ARRAY);
                    _pos++;
                    parseStack.push(ParseContext(PARSE_ARRAY_VALUE, valuePtr));
                } else {
                    if (ch == 'n') {
                        try{
                            *valuePtr = parse_null();
                        }
                        catch(std::logic_error& e){
                            throw std::logic_error(std::string(e.what()) + 
                              "\nContext: " + "PARSE_OBJECT_VALUE" + 
                              "\nPosition: " + std::to_string(_pos));
                        }
                    }   
                    else if (ch == 't' || ch == 'f') {
                        try{
                            *valuePtr = parse_bool();
                        }
                        catch(std::logic_error& e){
                            throw std::logic_error(std::string(e.what()) + 
                              "\nContext: " + "PARSE_OBJECT_VALUE" + 
                              "\nPosition: " + std::to_string(_pos));
                        }
                    }
                    else if (ch == '-' || (ch >= '0' && ch <= '9')) {   
                        try{
                            *valuePtr = parse_number();
                        }
                        catch(std::logic_error& e){
                            throw std::logic_error(std::string(e.what()) + 
                              "\nContext: " + "PARSE_OBJECT_VALUE" + 
                              "\nPosition: " + std::to_string(_pos));
                        }
                    }
                    else if (ch == '"') {
                        _pos++; 
                        try{
                            *valuePtr = parse_string();
                        }
                        catch(std::logic_error& e){
                            throw std::logic_error(std::string(e.what()) + 
                              "\nContext: " + "PARSE_OBJECT_VALUE" + 
                              "\nPosition: " + std::to_string(_pos));
                        }   
                    }
                }
                context.state = PARSE_OBJECT_KEY;
                
                skip_space();
                if (_pos < _str.size() && _str[_pos] == ',') {
                    _pos++;
                }
                break;
            }

            case PARSE_ARRAY_VALUE:
                if (ch == ']') {
                    _pos++;
                    parseStack.pop();
                    // 如果这是最后一个状态且是初始的PARSE_VALUE，则结束解析
                    if (parseStack.size() == 1 && parseStack.top().state == PARSE_VALUE) {
                        parseStack.pop();
                    } else {
                        // 否则检查后续的逗号
                        skip_space();
                        if (_pos < _str.size() && _str[_pos] == ',') {
                            _pos++;
                        }
                    }
                } else {
                    Json newElement;
                    context.json->push_back(newElement);
                    Json* valuePtr = &((*context.json)[context.json->array_size() - 1]);
                    
                    if (ch == '{') {
                        *valuePtr = Json(JSON_OBJECT);
                        _pos++;
                        parseStack.push(ParseContext(PARSE_OBJECT_KEY, valuePtr));
                    } else if (ch == '[') {
                        *valuePtr = Json(JSON_ARRAY);
                        _pos++;
                        parseStack.push(ParseContext(PARSE_ARRAY_VALUE, valuePtr));
                    } else {
                        if (ch == 'n') {
                            try{
                                *valuePtr = parse_null();
                            }
                            catch(std::logic_error& e){
                                throw std::logic_error(std::string(e.what()) + 
                                  "\nContext: " + "PARSE_ARRAY_VALUE" + 
                                  "\nPosition: " + std::to_string(_pos));
                            }
                        }
                        else if (ch == 't' || ch == 'f') {
                            try{
                                *valuePtr = parse_bool();
                            }
                            catch(std::logic_error& e) {
                                throw std::logic_error(std::string(e.what()) + 
                                  "\nContext: " + "PARSE_ARRAY_VALUE" + 
                                  "\nPosition: " + std::to_string(_pos));
                            }
                            _pos++;
                        }

                        else if (ch == '"') {
                            _pos++;
                            try{
                                *valuePtr = parse_string();
                            }
                            catch(std::logic_error& e){
                                throw std::logic_error(std::string(e.what()) + 
                                  "\nContext: " + "PARSE_ARRAY_VALUE" + 
                                  "\nPosition: " + std::to_string(_pos));
                            }
                        }
                        else if (ch == '-' || (ch >= '0' && ch <= '9')) {
                            try{
                                *valuePtr = parse_number();
                            }
                            catch(std::logic_error& e){
                                throw std::logic_error(std::string(e.what()) + 
                                  "\nContext: " + "PARSE_ARRAY_VALUE" + 
                                  "\nPosition: " + std::to_string(_pos));
                            }
                        }
                        skip_space();
                        if (_pos < _str.size() && _str[_pos] == ',') {
                            _pos++;
                        }
                    }
                }
                break;
        }
    }

    // 检查是否还有未解析的内容
    skip_space();
    if (_pos < _str.size()) {
        throw std::runtime_error(std::string("Extra characters after JSON") +
        "\nContext: " + "AFTER_PARSE" +
        "\nPosition: " + std::to_string(_pos));
    }

    return root;
}

Json Json::parse_null(){
    if(_str.compare(_pos, 4, "null") == 0){
        _pos += 4;
        return Json(Json::JSON_NULL);
    }
    throw std::logic_error("Unexpected null value");
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
    throw std::logic_error("Unexpected bool value");
}

Json Json::parse_number(){
    size_t end = _pos;
    bool double_flag = false;
    while (end < _str.size() && isdigit(_str[end]) || _str[end] == '.'||  (_str[end] == '+'|| _str[end] == '-')
     || _str[end] == 'e' || _str[end] == 'E') {
        if (_str[end] == '.' || _str[end] == 'e' || _str[end] == 'E')
            double_flag = true;
        end++;
    }
    size_t _pos_o = _pos;
    _pos = end;
    if(double_flag){
        try{
            double number = std::stod(_str.substr(_pos_o, end - _pos_o));
            return Json(number);
        }
        catch(std::invalid_argument& e){
            throw std::logic_error("Invalid double number format");
        }
        catch(std::out_of_range& e){
            throw std::logic_error("Double number out of range");
        }
    }    
    else{
        try{
            return Json(std::stoi(_str.substr(_pos_o, end - _pos_o)));
        }
        catch(std::invalid_argument& e){
            throw std::logic_error("Invalid integer format");
        }
        catch(std::out_of_range& e){
            throw std::logic_error("Integer out of range");
        }
    }
}

Json Json::parse_string(){
    if(_str.find('"', _pos) == std::string::npos)
        throw std::logic_error("Format error: Cannot find matching \"");
    size_t _pos_o = _pos;
    size_t end = _str.find('"', _pos);
    _pos = end+1;
    return Json(_str.substr(_pos_o, end - _pos_o));
}

void Json::parseStr(const std::string &str)
{
    _str = str;
    _pos = 0;
    Json temp(JSON_NULL);
    try{
        temp = parse();
    }
    catch(std::exception& e)
    {
        std::cerr<<e.what()<<std::endl;
        //用户可能无法根据_pos定位错误，此处给出进一步上下文字符串
        std::cout<<"Do you want to check the context string? "<<std::endl;
        std::cout<<"[Y/N]"<<std::endl;
        char choice;
        std::cin>>choice;
        if(choice == 'Y'){//输出错误位置前后30字符
            std::cout<<_str.substr(_pos-30, 60)<<std::endl;
        }
        temp.clear();                  // 清空temp, 防止内存泄漏
        *this = Json(JSON_NULL);
        return;
    }
    *this = std::move(temp);           //成功Parse后移动temp
}


Json& Json::operator = (Json&& other)
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





