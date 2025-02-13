#include <iostream>
#include <string>
#include <vector>
using namespace std;

string jsbind(string htmltext){
    //返回js动态添加html的局部代码
    //...working
    string jscontent;
    jscontent = "doc.insertAdjacentHTML('beforeend'," + htmltext + ")";
    return jscontent;
}

string jssleep(float time){
    //返回js动态暂停的局部代码
    string jscontent;
    //...working
}

string jsifpart(string partname){
    //返回js动态渲染某个TinML文本块的局部代码
    string jscontent;
    //...working
}

string jswait(string context){
    //返回js动态等待继续渲染的局部代码
    string jscontent;
    //...working
}