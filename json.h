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
        Json(const char* value);
        Json(std::vector<Json> value);
        Json(std::map<std::string, Json> value);
        Json(JsonType type);
        Json(const Json& other);

        // 深拷贝API
        void copy(const Json& other);

        // 赋值运算符
        //Json& operator=(const Json& other);
        //清除
        void clear();
        ~Json();

        // 数组API
        void push_back(const Json& value);
        Json& operator[](size_t index);
        size_t array_size() const;

        // 对象API
        void insert(const std::string& key, const Json& value);
        Json& operator[](const std::string& key);
        size_t object_size() const;

        //序列化为字符串
        std::string to_string() const;

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