#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <windows.h>
#include "util.hpp"
using namespace std;

string tinfilepath("./data/tinfile");

class TinLexer{

public:
    TinLexer(string contents){
        this->content = '\n' + contents + '\n';
    }

    void run_test();
    vector<string> run();
    vector<map<string,vector<string>>> lex();

private:
    string content;

};


void TinLexer::run_test(){
    cout << "TinLexer run" << endl;
    cout << this->content << endl;
}

vector<string> TinLexer::run(){
    vector <string> contents;

    contents = split(this->content, '\n');
    for (auto const &i : contents){
        cout << i << endl;
    }

    return contents;
}

vector<map<string,vector<string>>> TinLexer::lex(){
    // 预处理
    // tackle <tinfile>: add context of the specific TinML file into this->content
    regex tinfile_pattern("^<tinfile>(.*?)$", regex_constants::multiline);
    smatch tinfile_result;
    string::const_iterator tinfile_search_start(this->content.cbegin());
    while (regex_search(tinfile_search_start, this->content.cend(), tinfile_result, tinfile_pattern)){
        string tinfile_name = tinfilepath + "/" + tinfile_result[1].str();
        cout << tinfile_name << endl;
        string tinfile_content = readFile(tinfile_name);
        // 或许可以这样写吧，还没试过
        this->content = this->content.replace(tinfile_result[0].first, tinfile_result[0].second, tinfile_content);
        tinfile_search_start = tinfile_result[0].second;
    }
    // ...

    vector<map<string,vector<string>>> tinresults;//总结果
    
    vector<string> contents;
    string nowtag;//当前标签

    contents = split(this->content, '\n');

    bool Lines = false;//多行模式
    regex pattern("^<(.*?)>(.*)");
    smatch result;
    vector<string> args;//参数列表
    
    for (const auto &i : contents){
        if (size(i) == 1){
            // 实际上是empty，不过貌似写不对
            // 既有可能是因为getline将\n转为\0，导致该字符串截止
            continue;
        }
        
        if (size(i)>=2 && i.substr(0,2)=="|-"){
            //  cout << "Comment: " << i << endl;
            continue;
        }

        map<string,vector<string>> lineargs;//单行参数

        if (Lines){
            if (i[0]=='|'){
                if (size(i)>2 && i[i.length()-2]!='|'){
                    string content = i.substr(1,size(i)-2);
                    args.push_back(subreplace(content, "%VEB%", "|"));
                }else if (size(i)==2){
                    args.push_back("");
                }else{
                    string content = i.substr(1,size(i)-3);
                    args.push_back(subreplace(content, "%VEB%", "|"));
                    // for (auto const &j : args){
                    //     cout << "Arg: " << j << endl;
                    // }
                    lineargs[nowtag] = args;
                    tinresults.push_back(lineargs);
                    args.clear();
                    Lines = false;
                }
            }
        }else if (i[i.length()-2] == ';'){
            // getline最后一个为\0，因此-2
            // string本身不以\0结尾，但是getline会改成\0
            Lines = true;
            bool ismatch = regex_search(i, result, pattern);
            if (ismatch){
                // cout << "Tag: " << result[1] << endl;
                nowtag = result[1];
                string content = result[2];
                int lastindex = content.find_last_of(';');
                content = content.substr(0, lastindex);
                args.push_back(subreplace(content, "%VEB%", "|"));
            }else{
                cout << "\033[33;1m不可被解析，当作<p>处理：" << i << "\033[0m" << endl;
                args.push_back(subreplace(i, "%VEB%", "|"));
                lineargs["p"] = args;
                tinresults.push_back(lineargs);
                args.clear();
            }
        }else{
            bool ismatch = regex_search(i, result, pattern);
            if (ismatch){
                // cout << "Tag: " << result[1] << endl;
                nowtag = result[1];
                string content = result[2];
                auto oargs = split(content, '|');
                for (auto const &j : oargs)
                    args.push_back(subreplace(j, "%VEB%", "|"));
                // for (auto const &j : args){
                //     cout << "Arg: " << j << endl;
                // }
                lineargs[nowtag] = args;
                tinresults.push_back(lineargs);
                args.clear();
            }else{
                //无法匹配当作p处理
                cout << "\033[33;1m不可被解析，当作<p>处理：" << i << "\033[0m" << endl;
                args.push_back(subreplace(i, "%VEB%", "|"));
                lineargs["p"] = args;
                tinresults.push_back(lineargs);
                args.clear();
            }
        }
    }

    return tinresults;
}