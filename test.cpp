#include "json.h"
//#include "json.cpp"
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
using namespace KJson;

std::string removeComments(const std::string& jsonStr) {
    std::regex singleLineComment("//.*?(?=\n|$)");
    std::regex multiLineComment("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
    
    std::string result = std::regex_replace(jsonStr, singleLineComment, "");
    result = std::regex_replace(result, multiLineComment, "");
    
    return result;
}

int main()
{
    SetConsoleOutputCP(65001);
    std::string jsonStr;
    std::ifstream file("D:\\Code\\kingsoft\\week01\\json-parser-dev\\assert\\input_json.txt");
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        jsonStr = buffer.str();
        file.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    // 预处理JSON字符串，删除注释
    std::string processedJson = removeComments(jsonStr);

    Json j;
    j.parseStr(processedJson);
    /*std::cout << j.to_XML(1) << std::endl;*/
    std::cout << j.to_pretty_string(1) << std::endl;

    return 0;
}
