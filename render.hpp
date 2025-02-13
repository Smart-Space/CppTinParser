#include <iostream>
#include <filesystem>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <regex>
using namespace std;


vector<string> render_split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}


class HtmlRender {
    //html编辑类
public:
    HtmlRender()=default;
    HtmlRender(string tag, string content, map<string,string> kws, bool onetag=false, bool pre=false){
        this->tag = tag;
        // this->content = content;
        // this->kws = kws;
        this->pre = pre;
        if (pre){
            //原文本内容
            this->content = content;
        }else{
            this->load_content(content);
        }
        this->load_kws(kws);
        this->onetag = onetag;

        this->_parent = NULL;
    }
    HtmlRender* _parent;//父节点
    string render(string split_s);//输出html文本
    void add(HtmlRender* item);//添加html内容
    void configcnt(string content);//配置html内容
    void configkws(map<string, string> kws);//配置html内容
    HtmlRender* parent();
    list<HtmlRender*> children();//获取子HtmlRender内容
    ~HtmlRender(){
        // 析构函数，释放内存
        for (auto item : this->htmlcontent) {
            delete item;
        }
    }
private:
    list<HtmlRender*> htmlcontent;//html文件内容
    string tag;//html标签
    string content;//标签内容
    map<string, string> kws;//html关键字
    bool onetag;//是否为单标签
    bool pre;//是否为原文本内容

    const map<string, string> ascii_ent = {
        {"\"", "quot"},
        {"\'", "apos"},
        {"&", "amp"},
        {"<", "lt"},
        {">", "gt"},
    };//ascii转义

    void load_content(string content);//加载内容
    void load_kws(map<string, string> kws);//加载关键字
    string escape_ascii(string content);//转义ascii字符

};

string HtmlRender::render(string split_s="\n") {
    //渲染为html文本
    //split_s为元素分割文本，如果想输出为单行，则让其为""
    if (this->tag == ""){
        return this->content;
    }
    string htmltext = "<" + this->tag;
    for (auto &kw : this->kws) {
        htmltext += " " + kw.first + "=\"" + kw.second + "\"";
    }
    htmltext += ">";

    htmltext += this->content;
    
    for (auto item = this->htmlcontent.begin(); item != this->htmlcontent.end(); ++item) {
        string subtext = (*item)->render();
        htmltext += split_s + subtext;
    }

    if (this->onetag){
        //单标签
        htmltext += split_s;
    }else if (this->htmlcontent.size() > 0){
        //有子标签再加分隔符
        htmltext += split_s + "</" + this->tag + ">";
    }else{
        htmltext += "</" + this->tag + ">";
    }
    return htmltext;
}

void HtmlRender::add(HtmlRender* item) {
    //添加html内容
    this->htmlcontent.push_back(item);
    item->_parent = this;
}

void HtmlRender::configcnt(string content) {
    //配置html内容
    if (this->pre){
        //原文本内容
        this->content = content;
    }else{
        string ascii_ent = this->escape_ascii(content);
        this->content = ascii_ent;
    }
 }

void HtmlRender::configkws(map<string, string> kws) {
    //配置html内容
    for (auto &kw : kws) {
        string value = kw.second;
        string ascii_ent = this->escape_ascii(value);
        kw.second = value;
    }
    this->kws = kws;
 }

HtmlRender* HtmlRender::parent() {
    return this->_parent;
}

list<HtmlRender*> HtmlRender::children() {
    return this->htmlcontent;
}

void HtmlRender::load_content(string content) {
    //加载内容
    string ascii_ent = this->escape_ascii(content);
    this->content = ascii_ent;
 }

void HtmlRender::load_kws(map<string, string> kws) {
    //加载关键字
    for (auto &kw : kws) {
        //转义属性
        string value = kw.second;
        string ascii_ent = this->escape_ascii(value);
        kw.second = ascii_ent;
    }
    this->kws = kws;
 }

string HtmlRender::escape_ascii(string content) {
    //转义ascii字符
    for (auto &item : this->ascii_ent) {
        content = subreplace(content, item.first, "&" + item.second + ";");
    }
    return content;
 }


void load_para(HtmlRender* p, vector<string> lines){
    //添加<p>内容
    vector<char> tags = {'*','-','_','/','=','!','^','&','#'};
    regex reg(".*?!\\[(.*)\\]\\((.*)\\)");
    smatch result;
    int count = 0;

    if (lines.size()==0){
        //空行
        HtmlRender* br = new HtmlRender("br", "", {}, true);
        p->add(br);
        return;
    }
    
    for (auto &line : lines) {
        HtmlRender* last = p;//p最后一个元素

        if (line[0] == ' '){
            //空格
            string text = line.substr(1);
            HtmlRender* p_item = new HtmlRender("", text, {});
            p->add(p_item);
            continue;
        }

        string head;
        if (line.size() <=9){
            head = line;
        }else{
            head = line.substr(0, 9);
        }
        //<p>开头标记需要连续
        count = 0;
        for (auto &tag_char : head){
            if (find(tags.begin(), tags.end(), tag_char) != tags.end()){
                count++;
            }else{
                break;
            }
        }

        head = head.substr(0, count);

        if (count == 0){
            HtmlRender* p_item = new HtmlRender("", line, {});
            p->add(p_item);
            continue;
        }

        if (head.find('*')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("b", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('/')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("i", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('_')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("u", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('-')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("s", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('=')!=string::npos){
            // = 和 # 只能存在一个
            HtmlRender* nowlast = new HtmlRender("mark", "", {});
            last->add(nowlast);
            last = nowlast;
        }else if (head.find('#')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("code", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('^')!=string::npos){
            // ^ 和 & 只能存在一个
            HtmlRender* nowlast = new HtmlRender("sup", "", {});
            last->add(nowlast);
            last = nowlast;
        }else if (head.find('&')!=string::npos){
            HtmlRender* nowlast = new HtmlRender("sub", "", {});
            last->add(nowlast);
            last = nowlast;
        }
        if (head.find('!')!=string::npos){
            bool ismatch = regex_match(line, result, reg);
            if (ismatch) {
                string text = result[1];
                string url = result[2];
                if (text == ""){
                    text = url;
                }
                HtmlRender* p_item = new HtmlRender("a", text, {{"href",url}});
                last->add(p_item);
            }else{
                string text = line.substr(count);
                HtmlRender* p_item = new HtmlRender("", text, {});
                last->add(p_item);
            }
        }else{
            last->configcnt(line.substr(count));
        }
    }
}


class TinRender {
    //tin->html类
public:
    TinRender(const string &file) : file_(file){}
    HtmlRender* render(vector<map<string,string>> content, bool _style = true, bool _code_style = true);//转译为html
    void output(vector<map<string,string>> content);//输出
private:
    string file_;//html文件路径
    ofstream fout_;//html文件输出流
};

HtmlRender* TinRender::render(vector<map<string, string>> content, bool _style, bool _code_style){
    //转译为HtmlRender。可以直接使用output方法转译+输出保存
    //注意，CPPTinParser只是用c++实现的tinml转译器
    //不作为TinML规范的检查器
    string tag;
    HtmlRender* html = new HtmlRender("html", "", {});
    HtmlRender* head = new HtmlRender("head", "", {});

    HtmlRender* title = new HtmlRender("title", "TinML", {});
    head->add(title);

    HtmlRender* meta = new HtmlRender("meta", "", {{"charset","UTF-8"}}, true);
    head->add(meta);

    //读取css文件
    if (_style){
        string filename = "blubook.css";
        ifstream infile;
        infile.open(filename.data());   //将文件流对象与文件连接起来 
        assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
        string s, strAllLine;
        while (getline(infile, s)){
            strAllLine += s + "\n";
        }
        infile.close();             //关闭文件输入流
        HtmlRender* style = new HtmlRender ("style", strAllLine, {}, false, true);
        head->add(style);
    }

    //<code>样式
    if (_code_style){
        HtmlRender* codestyle = new HtmlRender("link", "", {{"rel","stylesheet"}, {"type", "text/css"}, {"href","https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/vs.min.css"}});
        head->add(codestyle);
        HtmlRender* codescript = new HtmlRender("script", "", {{"src","https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"}});
        head->add(codescript);
        HtmlRender* codestartscript = new HtmlRender("script", "hljs.highlightAll();", {});
        head->add(codestartscript);
    }
    
    HtmlRender* _body = new HtmlRender("body", "", {});
    HtmlRender* body = new HtmlRender("div", "", {{"id","content"}});
    _body->add(body);
    html->add(head);
    html->add(_body);

//     //初始化js代码
//     string loadjs = R"(var doc = document.getElementById("content");

// )";

    HtmlRender* table;
    HtmlRender* tbody;
    bool tablehead = false;

    HtmlRender* tabsview;
    bool pagestag = false;
    vector<string> pagesnames;
    vector<map<string, string>> pagescontent;
    int pagescount = 0;

    for (auto &item : content) {
        tag = item["tag"];
        
        //判断嵌套和包含元素
        if (pagestag){
            //标签页
            if (tag == "/page" || tag == "/pages"){
                //分割标签页或结束标签页，误操作，常规解析
            }else{
                pagescontent.push_back(item);
                continue;
            }
        }

        //常规转译TinML解析结果
        if (tag == "ac" || tag == "anchor"){
            string name = item["name"];
            if (name[0]=='#'){
                //锚点链接
                HtmlRender* anchor = new HtmlRender("a", "🔗", {{"href", name}});
                HtmlRender* lastitem = body->children().back();
                lastitem->add(anchor);
            }else{
                //锚点定义
                HtmlRender* anchor = new HtmlRender("a", "", {{"id", name}});
                body->add(anchor);
            }
        }else if (tag == "code"){
            //代码块
            //使用highlight.js，不支持tinml代码块
            string type = item["type"];
            string codes = item["codes"];
            if (type == "tin"){
                type = "nohighlight";
            }else{
                type = "language-" + type;
            }
            HtmlRender* pre = new HtmlRender("pre", "", {});
            HtmlRender* code = new HtmlRender("code", codes, {{"class",type}});
            pre->add(code);
            body->add(pre);
        }else if (tag == "html"){
            //html文本
            string htmltext = item["htmls"];
            HtmlRender* htmlcont = new HtmlRender("", htmltext, {}, false, true);
            body->add(htmlcont);
        }else if (tag == "img" || tag == "image"){
            //图片
            string name = item["name"];
            string url = item["url"];
            if (url.empty()){
                //因为CppTinParser只是用c++实现的tinml转译器
                //不作为TinML规范的检查器，也不作为tin文件渲染环境
                //所以不会有额外的资源文件储存位置
                //因此，不支持从本地文件中导入图片
                cout << "不支持从本地文件中导入图片:\033[33;1m " << name << "\033[0m" << endl;
                continue;
                // //获取绝对路径
                // std::filesystem::current_path();
                // string imgpath = "./data/imgs/" + name;
                // //...
            }
            vector<string> size = render_split(item["size"], 'x');
            string width = size[0];
            string height = size[1];
            HtmlRender* img = new HtmlRender("img", "", {{"alt", ""},{"src", url}, {"width", width}, {"height", height}});
            body->add(img);
        }else if (tag == "lnk" || tag == "link" || tag == "a"){
            //超链接
            string text = item["text"];
            string url = item["url"];
            if (url.empty()){
                cout << "超链接的链接不能为空:\033[33;1m " << url << "\033[0m" << endl;
                continue;
            }
            if (text == ""){
                text = url;
            }
            HtmlRender* link = new HtmlRender("a", text, {{"href", url}});
            HtmlRender* linkp = new HtmlRender("p", "", {});
            linkp->add(link);
            body->add(linkp);
        }else if (tag == "ls" || tag == "list"){
            //无序列表
            vector<string> lists = render_split(item["lists"], '\n');
            HtmlRender* ul;
            ul = new HtmlRender("ul", "", {});
            HtmlRender* rootul = ul;
            int nowlevel = 0;
            for (auto &list : lists){
                int level = 0;
                for(auto &ch : list){
                    if (ch == '|'){
                        level ++;
                    }else{
                        break;
                    }
                }
                if (level == nowlevel){
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});
                    ul->add(li);
                }else if (level > nowlevel){
                    HtmlRender* nowli = ul->children().back();
                    ul = new HtmlRender("ul", "", {});
                    nowli->add(ul);
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});
                    ul->add(li);
                }else if (level < nowlevel){
                    for (int i = 0; i < nowlevel - level; i++) {
                        ul = ul->parent()->parent();
                    }
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});;
                    ul->add(li);
                }
                nowlevel = level;
            }
            body->add(rootul);
        }else if (tag == "n" || tag == "note"){
            //引用说明文本
            vector<string> notes = render_split(item["notes"], '\n');
            HtmlRender* quote = new HtmlRender("blockquote", "", {});
            for (auto &note : notes){
                if (note.size() == 0 || note == " "){
                    //这里允许note为空格时代表换行
                    //这是为了兼容新版TinML的第一个<note>规范
                    HtmlRender* p = new HtmlRender("p", "", {});
                    HtmlRender* br = new HtmlRender("br", "", {}, true);
                    p->add(br);
                    quote->add(p);
                }else{
                    HtmlRender* p = new HtmlRender("p", note, {});
                    quote->add(p);
                }
            }
            body->add(quote);
        }else if (tag == "nl" || tag == "numlist"){
            //有序列表
            vector<string> lists = render_split(item["lists"], '\n');
            HtmlRender* ol;
            ol = new HtmlRender("ol", "", {});
            HtmlRender* rootol = ol;
            int nowlevel = 0;
            for (auto &list : lists){
                int level = 0;
                for(auto &ch : list){
                    if (ch == '|'){
                        level ++;
                    }else{
                        break;
                    }
                }
                if (level == nowlevel){
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});
                    ol->add(li);
                }else if (level > nowlevel){
                    HtmlRender* nowli = ol->children().back();
                    ol = new HtmlRender("ol", "", {});
                    nowli->add(ol);
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});
                    ol->add(li);
                }else if (level < nowlevel){
                    for (int i = 0; i < nowlevel - level; i++) {
                        ol = ol->parent()->parent();
                    }
                    HtmlRender* li = new HtmlRender("li", list.substr(level), {});;
                    ol->add(li);
                }
                nowlevel = level;
            }
            body->add(rootol);
        }else if (tag == "p"){
            //文本段落
            vector<string> para = render_split(item["texts"], '\n');
            HtmlRender* p = new HtmlRender("p", "", {});
            load_para(p, para);
            body->add(p);
        }else if (tag == "pages"){
            //标签页起始
            pagestag = true;
            pagesnames = render_split(item["name"], '\n');
            tabsview = new HtmlRender("div", "", {{"class", "tabs"}});
            // for (int i = 0; i < names_length; i++){
            //     HtmlRender* tab = new HtmlRender("div", "", {{"class", "tab"}});
            //     if (i==0){
            //         tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(i)}, {"checked", "true"}}));
            //     }else{
            //         tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(i)}}));
            //     }
            //     tab->add(new HtmlRender("label", names[i], {{"for", "tab-" + to_string(i)}}));
            // }
        }else if (tag == "/page"){
            //标签页分割
            HtmlRender* pagesdivrender = this->render(pagescontent, false, false)->children().back()->children().front();//<head> [<body> [<div>]]
            pagesdivrender->configkws({{"class", "tabcontent"}});
            HtmlRender* tab = new HtmlRender("div", "", {{"class", "tab"}});
            if (pagescount==0){
                tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(pagescount)}, {"checked", "true"}}));
            }else{
                tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(pagescount)}}));
            }
            tab->add(new HtmlRender("label", pagesnames[pagescount], {{"for", "tab-" + to_string(pagescount)}}));
            tab->add(pagesdivrender);
            tabsview->add(tab);
            pagescount ++;
            pagescontent.clear();
        }else if (tag == "/pages"){
            //标签页结束
            HtmlRender* pagesdivrender = this->render(pagescontent, false, false)->children().back()->children().front();//<head> [<body> [<div>]]
            pagesdivrender->configkws({{"class", "tabcontent"}});
            HtmlRender* tab = new HtmlRender("div", "", {{"class", "tab"}});
            if (pagescount==0){
                tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(pagescount)}, {"checked", "true"}}));
            }else{
                tab->add(new HtmlRender("input", "", {{"type", "radio"}, {"name", "tab-group1"}, {"id", "tab-" + to_string(pagescount)}}));
            }
            tab->add(new HtmlRender("label", pagesnames[pagescount], {{"for", "tab-" + to_string(pagescount)}}));
            tab->add(pagesdivrender);
            tabsview->add(tab);
            pagescount = 0;
            pagescontent.clear();
            pagesnames.clear();
            pagestag = false;
            body->add(tabsview);
        }else if (tag == "part" || tag == "pt"){
            //文本块
            HtmlRender* detials = new HtmlRender("details", "", {});
            HtmlRender* summary = new HtmlRender("summary", item["name"], {});
            detials->add(summary);
            body->add(detials);
            body = detials;
        }else if (tag == "/part" || tag == "/pt"){
            //文本块结束
            body = body->parent();
        }else if (tag == "sp" || tag == "separate"){
            //分割线
            HtmlRender* hr = new HtmlRender("hr", "", {}, true);
            body->add(hr);
        }else if (tag == "stop"){
            //!!!
            //本标签在转义到html时不会起作用
            //TinML转义到HTML易发生阅读信息与逻辑的丢失
            string time_s = item["time"];
            float time = atof(time_s.c_str());
            int time_int = (int)(time * 1000);
            time_s = to_string(time_int);
            string script_s = "const stopTime = Date.now() + " + time_s + "\n"\
            "while (Date.now() < stopTime);";
            HtmlRender* script = new HtmlRender("script", script_s, {}, false, true);
            body->add(script);
        }else if (tag == "tb" || tag == "table"){
            //表格
            vector<string> datas = render_split(item["datas"], '\n');
            if (tablehead){
                //已定义表头
                HtmlRender* tr = new HtmlRender("tr", "", {});
                for (auto &data : datas){
                    HtmlRender* td = new HtmlRender("td", data, {});
                    tr->add(td);
                }
                tbody->add(tr);
            }else{//未定义表头
                table =  new HtmlRender("table", "", {});
                HtmlRender* thead = new HtmlRender("thead", "", {});
                HtmlRender* tr = new HtmlRender("tr", "", {});
                for (auto &data : datas){
                    HtmlRender* th = new HtmlRender("th", data, {});
                    tr->add(th);
                }
                thead->add(tr);
                table->add(thead);
                tbody = new HtmlRender("tbody", "", {});
                tablehead = true;
            }
        }else if (tag == "/tb" || tag == "/table"){
            //表格数据记录结束
            table->add(tbody);
            body->add(table);
            tablehead = false;
            table = NULL;
            tbody = NULL;
        }else if (tag == "tinfile"){
            // 内嵌TinML文件内容
            // 由于不支持data目录，暂不做处理
            // 但和<img>不一样，<tinfile>不会直接用到本地文件，而是读取本地文件之后直接并入html结构中
        }else if (tag == "title" || tag == "t"){
            // 标题
            string level = item["level"];
            if (level == ""){
                level = "1";
            }
            HtmlRender* title = new HtmlRender("h" + level, item["title"], {});
            body->add(title);
        }
    }

    // //整合js代码
    // HtmlRender* jsscript = new HtmlRender("script", loadjs, {});

    // string htmltext = html.render();
    // // cout << htmltext;

    // this->fout_.open(this->file_);
    // this->fout_ << htmltext;
    // this->fout_.close();

    return html;
}

// void cleanhtmlrender(HtmlRender* render){
//     //释放通过new产生的HtmlRender节点
//     for (auto &child : render->children()){
//         cleanhtmlrender(child);
//     }
//     // cout << "clean" << render << endl;
//     delete render;
// }

void TinRender::output(vector<map<string,string>> content){
    //转译成html并写入文件中，包含TinRender::render操作
    HtmlRender* allhtml = this->render(content);
    string htmltext = allhtml->render();
    this->fout_.open(this->file_);
    this->fout_ << htmltext;
    this->fout_.close();

    // //清理内存
    // cleanhtmlrender(allhtml);

    delete allhtml;
}
