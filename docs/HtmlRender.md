# HtmlRender

在`CppTinParser/render.hpp`中定义和实现。

使用c++实现的简易Html编辑类。

## 简介

目前，c++有几个Html解析器，而少见便捷规范的html生成器，HtmlRender则提供了一个简单的、规范的html内容生成器。用c++实现html内容生成器，并不是简单的字符串拼接，这样会导致代码编写不规范、易读性下降，而且无法应对复杂html生成任务。HtmlRender借鉴了python的第三方html编写库——`dominate`。

---

## 使用接口

### 初始化

```c++
HtmlRender* item = HtmlRender(string tag, string content, map<string,string> kws, bool onetag=false, bool pre=false)
//tag 标签名称，如果为空字符串，则在生成时直接产生content，不添加标记
//content 标签内容
//kws 标签参数
//onetag 是否为单标签，比如<br>, <hr>等
//pre 是否显示为原文本
```

### 生成html文本

```c++
string result = HtmlRender.render()
```

该函数将生成并返回该`HtmlRender`所包含的html文本内容。一般应只用`tag = "html"`的`HtmlRender`使用该函数，以此来实现完整的html文本。当然，任意`HtmlRender`实例均可，单标签也可使用该函数，但是生成内容将大概率出现错误。

### 添加子元素

为了方便操作，`HtmlRender`使用list作为容器来存取子元素指针，这里的子元素同样是`HtmlRender`。具体增加代码如下：

```c++
HtmlRender* subitem = new HtmlRender(...)
item.add(subitem)
```

如果父元素本身就是通过`new`创建，则使用如下代码：

```c++
item->add(subitem)
```

### 设置标签元素内容和参数

设置内容：

```c++
item.configcnt(string content)
```

设置参数：

```c++
item.configkws(map<string, string> kws)
```

### 获取父元素指针

除了顶级元素，任何一个标签元素在被使用`add`方法后，都会有明确的父元素。

通过如下代码获取父元素指针：

```c++
HtmlRender* p_item = subitem->parent()
//p_item == &item
```

### 获取子元素指针列表

如果一个元素使用了`add`方法，则必然含有子元素。

使用如下代码获取子元素指针列表

```c++
list<HtmlRender*> children = item.children()
```

---

## 实现原理

### 添加子元素

获取子元素指针，在子元素指针列表中添加该指针。

```c++
void HtmlRender::add(HtmlRender* item) {
    //添加html内容
    this->htmlcontent.push_back(item);
    item->_parent = this;
}
```

### 渲染

首先生成元素自身标签、参数、内容，然后遍历子元素指针列表，获取所有子元素以及多级子元素的渲染生成内容，最后加上自身结束标签（如果有的话）。

```c++
string HtmlRender::render() {
    //渲染为html文本
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
        htmltext += "\n" + subtext;
    }

    if (this->onetag){
        //单标签
        htmltext += "\n";
    }else{
        htmltext += "\n</" + this->tag + ">";
    }
    return htmltext;
}

```

---

## 示例

测试代码：

```c++
int main(){
    HtmlRender html = HtmlRender("html", "", {});
    HtmlRender* head = new HtmlRender("head", "", {});
    HtmlRender* title = new HtmlRender("title", "TinML", {});
    head->add(title);
    html.add(head);

    HtmlRender* body = new HtmlRender("body", "", {});
    html.add(body);

    HtmlRender* t1 = new HtmlRender("h1", "TITLE", {});
    body->add(t1);
    for (int i=0; i<10; i++){
        HtmlRender* p = new HtmlRender("p", "paragraph - " + to_string(i), {});
        body->add(p);
    }

    string htmltext = html.render();
    cout << htmltext <<endl;

    return 0;
}
```

结果：

```html
<html>
<head>
<title>TinML
</title>
</head>
<body>
<h1>TITLE
</h1>
<p>paragraph - 0
</p>
<p>paragraph - 1
</p>
<p>paragraph - 2
</p>
<p>paragraph - 3
</p>
<p>paragraph - 4
</p>
<p>paragraph - 5
</p>
<p>paragraph - 6
</p>
<p>paragraph - 7
</p>
<p>paragraph - 8
</p>
<p>paragraph - 9
</p>
</body>
</html>
```

---

## 主要源码

```c++
string subreplace(std::string resource_str, std::string sub_str, std::string new_str){
    string dst_str = resource_str;
    string::size_type pos = -1;
    //这里要专门处理&转义后仍存在的&符号，所以find使用了pos
    while((pos = dst_str.find(sub_str, pos+1)) != std::string::npos)   //替换所有指定子串
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    return dst_str;
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
    string render();//输出html文本
    void add(HtmlRender* item);//添加html内容
    void configcnt(string content);//配置html内容
    void configkws(map<string, string> kws);//配置html内容
    HtmlRender* parent();
    list<HtmlRender*> children();//获取子HtmlRender内容
private:
    list<HtmlRender*> htmlcontent;//html文件内容
    string tag;//html标签
    string content;//标签内容
    map<string, string> kws;//html关键字
    bool onetag;//是否为单标签
    bool pre;//是否为原文本内容

    const map<string, string> ascii_ent = { { "\"", "quot" }, { "\'", "apos" }, { "&", "amp" }, { "<", "lt" }, { ">", "gt" } };//ascii转义

    void load_content(string content);//加载内容
    void load_kws(map<string, string> kws);//加载关键字
    string escape_ascii(string content);//转义ascii字符

};

string HtmlRender::render() {
    //渲染为html文本
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
        htmltext += "\n" + subtext;
    }

    if (this->onetag){
        //单标签
        htmltext += "\n";
    }else{
        htmltext += "\n</" + this->tag + ">";
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
        string value = kw.second;
        string ascii_ent = this->escape_ascii(value);
        kw.second = value;
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
```
