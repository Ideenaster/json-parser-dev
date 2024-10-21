#pragma once
#include <string>
#include <vector>
#include <map>
    
namespace KJson{
    class Json{
        public:
        // 类型
            enum JsonType{
                JSON_NULL,
                JSON_BOOL,
                JSON_INT,
                JSON_DOUBLE,
                JSON_STRING,
                JSON_ARRAY,
                JSON_OBJECT
            };

        // 构造函数
        Json();
        Json(bool value);
        Json(int value);
        Json(double value);
        Json(const std::string& value);
        Json(std::vector<Json> value);
        Json(std::map<std::string, Json> value);
        Json(JsonType type);
        Json(const Json& other);
        Json& operator=(const Json& other);
        ~Json();

        private:
            JsonType _type;
            // 值,使用Union实现类似于动态类型的效果
            union JsonValue {
                bool _bool;
                int _int;
                double _double;
                std::string* _string;
                std::vector<Json>* _array;
                std::map<std::string, Json>* _object;
            };
            JsonValue _value;
    };
};