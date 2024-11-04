#include <json/json.h>
#include "KJson.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>


std::string removeComments(const std::string& jsonStr) {
    std::regex singleLineComment("//.*?(?=\n|$)");
    std::regex multiLineComment("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
    
    std::string result = std::regex_replace(jsonStr, singleLineComment, "");
    result = std::regex_replace(result, multiLineComment, "");
    
    return result;
}

void Test_KJson_type()
{
    KJson::Json j;
    j.insert("null",KJson::Json());
    j.insert("bool",KJson::Json(true));
    j.insert("int",KJson::Json(123));
    j.insert("double",KJson::Json(123.456));
    j.insert("string",KJson::Json("hello"));
    KJson::Json arr(KJson::JSON_ARRAY);
    arr.push_back(KJson::Json(1));
    arr.push_back(KJson::Json(2));
    arr.push_back(KJson::Json(3));
    std::cout << arr.to_pretty_string(1) << std::endl;
    j.insert("array",arr);
    std::cout<<j.to_pretty_string(1)<<std::endl;
    j["array"][0] = KJson::Json(100);
}

void Test_KJson_dynamic()
{
    KJson::Json userInfo(KJson::JSON_OBJECT);
    std::string key;
    std::cout << "please input key : " ;
    std::cin >> key;  //生成动态键名
    userInfo[key] = 25;  //生成键值对
    std::cout << userInfo.to_pretty_string(1) << std::endl;
}
void Test_KJson_XML()
{
    KJson::Json j;
    std::string jsonStr;
    std::ifstream file("D:\\Code\\kingsoft\\week01\\json-parser-dev\\assert\\input_json.txt");
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        jsonStr = buffer.str();
        file.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }
    std::string processedJson = removeComments(jsonStr);
    j.parseStr(processedJson);
    std::cout << j.to_XML(1) << std::endl;
}

void Test_bench_mark()
{
    std::string jsonStr;
    std::ifstream file("D:\\Code\\kingsoft\\week01\\json-parser-dev\\assert\\input_json.txt");
    std::ifstream file2("D:\\Code\\kingsoft\\week01\\json-parser-dev\\assert\\citm_catalog.json");
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file2.rdbuf();
        jsonStr = buffer.str();
        file2.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }

    // 预处理JSON字符串，删除注释
    std::string processedJson = removeComments(jsonStr);

    // 测试自定义JSON解析器
    {
        KJson::Json j;
        auto start = std::chrono::high_resolution_clock::now();
        j.parseStr(processedJson);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "自定义解析器耗时: " << duration.count() << " 毫秒" << std::endl;
    }

    // 测试JsonCpp解析器
    {
        Json::Value root;
        Json::Reader reader;
        auto start = std::chrono::high_resolution_clock::now();
        bool parsingSuccessful = reader.parse(processedJson, root);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (parsingSuccessful) {
            std::cout << "JsonCpp解析耗时: " << duration.count() << " 毫秒" << std::endl;
        } else {
            std::cerr << "JsonCpp解析失败" << std::endl;
        }
    }
}

int main()
{
    SetConsoleOutputCP(65001);
    /*Test_KJson_XML();*/
    /*Test_KJson_dynamic();*/
    // Test_KJson_type();
    Test_bench_mark();
    return 0;
}
