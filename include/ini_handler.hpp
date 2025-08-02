#ifndef _INI_HANDLER_H_
#define _INI_HANDLER_H_
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace multi
{
namespace ini{
class IniHandler
{
public:
    using KVStruct=std::unordered_map<std::string,std::string>;

    IniHandler(const char* file);
    ~IniHandler() = default;
    
    std::string get_value(const std::string& section_name ,const std::string& key 
        ,const std::string& defaultValue = std::string())const{
    }
    
    std::string get_value(const std::string& key 
        ,const std::string& defaultValue = std::string())const{
        return get_value("global",key ,defaultValue);
    }
    void dump();
private:
/**
 * 解析section名称（如"[server]" -> "server"）
 * @param str 原始行内容
 * @return 处理后的section名称（若格式错误返回空）
 */
    std::string name_handler(const std::string& str);
    std::pair<std::string,std::string> kv_handler(std::string& str,int line);
    void trim(std::string& str);
private:
    std::unordered_map<std::string,KVStruct> section_;
};

IniHandler::IniHandler(const char* file)
{
    std::ifstream ifs(file);
    std::string buffer;
    std::string name("global");
    int line = 0;
    while (std::getline(ifs,buffer))
    {
        ++line;
        if(buffer.empty()){continue;}
        char first = buffer[0];
        if(first == ' '){
            trim(buffer);
            if(buffer.empty()){
                continue;
            }
            first = buffer[0];
        }
        if(first == '['){name = name_handler(buffer);}
        else if(first == ';' |first == '#' ){continue;}
        else {
            std::pair<std::string,std::string> kv = kv_handler(buffer,line);
            section_[name][kv.first] = kv.second;
        }
    }
    ifs.close(); 
}
std::string IniHandler::name_handler(const std::string& str){
    size_t index = str.find(']');
    if(index == std::string::npos){
        return str.substr(1,str.size()+1);;
    }
    return str.substr(1,index-1);
}
std::pair<std::string,std::string> IniHandler::kv_handler(std::string& str,int line){
    size_t comment_index = str.find(";");
    if(comment_index != std::string::npos){
        str = str.substr(0,comment_index);
    }
    comment_index = str.find("#");
    if(comment_index != std::string::npos){
        str = str.substr(0,comment_index);
    }

    size_t index = str.find("=");
    if(index == std::string::npos){
        std::cerr << "line:" << line << " Format error\n";
    }
    
    std::string key = str.substr(0,index);
    std::string value = str.substr(index+1,str.size()+1);
    
    trim(key);
    trim(value);
    return std::pair<std::string,std::string>(key,value);
}
void IniHandler::trim(std::string& str){
    if(str.empty()){return;}
    auto start = std::find_if(str.begin(), str.end(), 
        [](int ch) { return !std::isspace(ch); });
    if(start == str.end()){return str.clear();}
    auto end = std::find_if(str.rbegin(), str.rend(), 
        [](int ch) { return !std::isspace(ch); }).base();
    str = std::string(start, end);
    if (str.empty()) {return;}

    const char first = str.front();
    const char last = str.back();
    const bool is_double_quote = (first == '"' && last == '"');
    const bool is_single_quote = (first == '\'' && last == '\'');

    if ((is_double_quote || is_single_quote) && str.size() >= 2) {
        str = str.substr(1, str.size() - 2);
    }
}
void IniHandler::dump(){
    for(auto& section : section_){
        std::cout <<"section==========start\n";
        std::cout <<section.first <<"\n";
        for(auto& kv :section.second){
            std::cout << "key:"<<kv.first <<"value:" << kv.second <<"\n";
            
        }
        std::cout <<"section==========end\n\n";
    }
}
} // namespace ini
} // namespace multi

#endif