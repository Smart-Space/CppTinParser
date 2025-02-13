//CppTinParser
//Author: Smart-Space <smart-space@qq.com>
//使用c++实现的tin语言转译器
//主要技术模块：
//1. 词法分析 lexer.hpp
//2. 标签解释 parser.hpp
//3. 转义器 render.hpp 功能集成在parser.hpp中
//4. Html编辑类 在render.hpp中定义
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <map>
#include <fstream>
#include <cassert>
#include <locale>
#include <codecvt>
#include "util.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "command\cmd.hpp"
// #include "command\totinp.hpp"

using namespace std;

// string UTF8ToGB(const char* str);
string readFile(string file);


int main(int argc, char* argv[]){
    SetConsoleOutputCP(CP_UTF8);

    if (argc < 2) {
        cout << "\033[31;1m请输入文件名\033[0m" << endl;
        system("pause");
        return -1;
    }
    
    //解析命令行
    // tag inlcude html -> html generate
    // adding =====
    // map inlcude o/out/output -> output dir
    //x tag include tinp -> TINP file generate
    //x  also need map key value
    //x tag include untinp -> TINP file to TIN file generate
    //x  also need map key value
    Cmd_Argv* argv_ = readargv(argv);

    //输出当前文件名
    cout << "Current file: " << argv_->programe << endl;
    
    //输出要操作的TIN格式文件
    cout << "start translating: " << argv_->argv_list[0] << endl;

    string outfile = "";
    if (argv_->getkey("o") != "" || argv_->getkey("out") != "" || argv_->getkey("output") != ""){
        outfile = argv_->getkey("o") != "" ? argv_->getkey("o") : (argv_->getkey("out") != "" ? argv_->getkey("out") : argv_->getkey("output"));
        if (outfile == ""){
            outfile = "test.html";
        }
    }else{
        outfile = "test.html";
    }

    //判断是否是转译为html的命令
    //命令行要求，tag中有 html 标签，或者tag中什么都没有
    if (argv_->checktag("html") || argv_->argv_tag.empty()){
        cout << "MODE: HTML GENERATION MODE" << endl;
    }

    // string content = readFile(argv[1]);
    string content = readFile(argv_->argv_list[0]);

    // if (argv_->checktag("tinp")){
    //     //要求生成TINP格式文件
    //     cout << "MODE: TINP GENERATION MODE" << endl;
    //     cout << "KEY: " << argv_->getkey("key") << endl;
    //     string resstring = "";
    //     if (argv_->getkey("key") != ""){
    //         //TAG中含tinp时，需要有-key值
    //         resstring = xorstring(content, argv_->getkey("key"));
    //     }
    //     cout << resstring << endl;
    //     return 0;
    // }else if (argv_->checktag("untinp")){
    //     //解码TINP格式文件
    //     cout << "MODE: TINP DECODE MODE" << endl;
    //     cout << "KEY: " << argv_->getkey("key") << endl;
    //     string resstring = "";
    //     if (argv_->getkey("key") != ""){
    //         //TAG中含untinp时，需要有-key值
    //         resstring = xorstring(content, argv_->getkey("key"));
    //     }
    //     cout << resstring << endl;
    //     return 0;
    // }

    TinLexer lexer(content);
    auto lexresult = lexer.lex();
    map<string,vector<string>>::iterator iter;

    cout << "==========" << endl;

    //语法分析
    for (auto item : lexresult) {
        for (iter = item.begin(); iter != item.end(); ++iter) {
            cout <<"[" << size(iter->second) << "] ";
            cout << iter->first << ": ";
            for (auto str : iter->second) {
                cout << str << " | ";
            }
            cout << endl;
        }
    }

    //标记意义解释
    cout << "==========" << endl;
    loadkws();
    TinParser parser(lexresult);
    parser.parse();

    //生成html
    cout << "==========" << endl;
    parser.render(outfile);

    cout << "\033[34;1m生成完成\033[0m" << endl;
    
    delete argv_;
    argv_ = NULL;
    system("pause");

    return 0;
}
