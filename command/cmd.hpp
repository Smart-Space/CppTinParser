#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;


typedef struct Cmd_Argv
{
    string programe = "";//程序名
    map<string, string> argv_map = {};//参数字典
    vector<string> argv_list = {};//参数列表
    vector<string> argv_tag = {};//参数标签

    bool checktag(string tag){
        for (auto it = this->argv_tag.begin(); it != this->argv_tag.end(); ++it){
            if (*it == tag) return true;
        }return false;
    };

    string getkey(string key){
        if (this->argv_map.count(key) > 0){
            return this->argv_map[key];
        }else{
            return "";
        }
    };
} Cmd_Argv;


Cmd_Argv* __readargv(vector<map<string, string>> argvs){
    //将参数列表解析为结构体，并返回结构体的指针
    Cmd_Argv* cmd_argv = new Cmd_Argv();
    cmd_argv->programe = argvs[0]["program"];

    if (argvs.size() == 1){
        return cmd_argv;
    }

    for (int i = 1; i < argvs.size(); i++){
        auto argv = argvs[i].begin();
        if (argv->first == "TAG"){
            //参数为标签，添加标签字符串到标签列表
            // -tag -> tag
            cmd_argv->argv_tag.push_back(argv->second.substr(1));
        }else if (argv->first == "VALUE"){
            //参数为值，添加值字符串到值列表
            // value -> value
            cmd_argv->argv_list.push_back(argv->second);
        }else{
            //参数为键值对，添加键值对到字典
            // --key value -> key : value
            cmd_argv->argv_map[argv->first.substr(2)] = argv->second;
        }
    }

    return cmd_argv;
}


Cmd_Argv* readargv(char* argvs[]){
    //读取参数
    //解析参数，返回参数结构体
    vector<map<string, string>> argv_list;
    string last_argv = "";

    for(int i = 0; argvs[i] != NULL; i++){
        map<string, string> argv_map;
        //第一个参数为程序名
        if (i == 0){
            argv_map["program"] = argvs[i];
            argv_list.push_back(argv_map);
            continue;
        }
        if (argvs[i][0] == '-'){
            if (argvs[i][1] == '-'){
                //参数标签-键值对
                last_argv = argvs[i];
            }else{
                //参数标签
                argv_map["TAG"] = argvs[i];
                argv_list.push_back(argv_map);
            }
        }else{
            if (last_argv != ""){
                //上一个为参数标签键
                argv_map[last_argv] = argvs[i];
                argv_list.push_back(argv_map);
                last_argv = "";
            }else{
                //普通参数值
                argv_map["VALUE"] = argvs[i];
                argv_list.push_back(argv_map);
            }
        }
    }

    Cmd_Argv* cmd_argv = __readargv(argv_list); //将参数列表解析为结构体，并返回结构体的指针
    return cmd_argv;
}
