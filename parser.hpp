#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "render.hpp"
#include "error.hpp"
using namespace std;

static map<string, vector<string>> tinkws;

void loadkws()
{
    // 标记对应的参数键
    tinkws["ac"] = {"name"};
    tinkws["anchor"] = {"name"};
    tinkws["code"] = {"type", "codes"};
    tinkws["fl"] = {};
    tinkws["follow"] = {};
    tinkws["html"] = {"htmls"};
    tinkws["img"] = {"name", "url", "size"};
    tinkws["image"] = {"name", "url", "size"};
    tinkws["lnk"] = {"text", "url", "description"};
    tinkws["link"] = {"text", "url", "description"};
    tinkws["a"] = {"text", "url", "description"};
    tinkws["ls"] = {"lists"};
    tinkws["list"] = {"lists"};
    tinkws["n"] = {"notes"};
    tinkws["note"] = {"notes"};
    tinkws["nl"] = {"lists"};
    tinkws["numlist"] = {"lists"};
    tinkws["p"] = {"texts"};
    tinkws["pages"] = {"name"};
    tinkws["/page"] = {""};
    tinkws["/pages"] = {""};
    tinkws["part"] = {"name"};
    tinkws["pt"] = {"name"};
    tinkws["/part"] = {"name"};
    tinkws["/pt"] = {"name"};
    tinkws["sp"] = {"color"};
    tinkws["separate"] = {"color"};
    tinkws["stop"] = {"time"};
    tinkws["tb"] = {"datas"};
    tinkws["table"] = {"datas"};
    tinkws["/tb"] = {""};
    tinkws["/table"] = {""};
    tinkws["tin"] = {""};
    tinkws["tinfile"] = {"name", "mode"};
    tinkws["title"] = {"title", "level"};
    tinkws["t"] = {"title", "level"};
    tinkws["wait"] = {"content"};
    tinkws["w"] = {"content"};
    //...
}

map<string, string> tokeywords(string tag, vector<string> contents)
{
    // 将列表顺序参数转为键、值类参数
    map<string, string> keywords;
    cout << "Tag: " << tag << endl;
    keywords["tag"] = tag;

    int args_num = contents.size();
    int index = 0;
    for (auto kws : tinkws[tag])
    {
        if (index >= args_num)
        {
            // 剩下键全为空
            keywords[kws] = "";
            continue;
        }
        keywords[kws] = contents[index];
        // cout << kws << " = " << contents[index] << endl;
        index++;
    }

    if (index < args_num)
    {
        // 将剩下的参数全部放入最后一个键中
        for (int i = index; i < args_num; i++)
        {
            keywords[tinkws[tag].back()] += "\n" + contents[i];
        }
    }

    for (auto kw : keywords)
    {
        if (kw.first == "tag")
        { // 跳过tag键
            continue;
        }
        cout << kw.first << " = " << kw.second << endl;
    }

    return keywords;
}

class TinParser
{ // 生成HTML文件
public:
    TinParser(vector<map<string, vector<string>>> contents)
    {
        this->contents = contents;
    }
    void parse();
    void render(string file);

private:
    vector<map<string, vector<string>>> contents;
    vector<map<string, string>> result;
};

void TinParser::parse()
{
    // 判断tag是否存在
    contents = this->contents;
    for (auto content : contents)
    {
        string tag = content.begin()->first;
        if (tinkws.find(tag) == tinkws.end())
        {
            cerr << "\033[33;1mtag not found: " << tag << "\033[0m" << endl;
            // //string转char
            // char c_tag;
            // strcpy(&c_tag, tag.c_str());
            // throw NoTagName(c_tag);
            continue;
        }
        map<string, string> results = tokeywords(tag, content.begin()->second);
        result.push_back(results);
    }
}

void TinParser::render(string file = "test.html")
{
    // 渲染HTML文件
    //  string file = "test.html";
    TinRender render(file);
    // render.render(result);
    render.output(result);
}