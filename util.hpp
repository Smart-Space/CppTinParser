// 杂项
#ifndef UTIL_HPP
#define UTIL_HPP
#else
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

string readFile(string file){
    ifstream infile(file, ios::binary);
    if (!infile.is_open()){
        cout << "文件打开失败" << endl;
        return "";
    }

    infile.seekg(0, ios::end);
    streamsize size = infile.tellg();
    infile.seekg(0, ios::beg);

    string content(size, '\0');
    infile.read(&content[0], size);
    
    infile.close();

    return content;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
 }

 string subreplace(std::string resource_str, std::string sub_str, std::string new_str){
    string dst_str = resource_str;
    string::size_type pos = -1;
    //这里要专门处理&转义后仍存在的&符号之类的情况，避免死循环，所以find使用了pos
    while((pos = dst_str.find(sub_str, pos+1)) != std::string::npos)   //替换所有指定子串
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    return dst_str;
}

#endif