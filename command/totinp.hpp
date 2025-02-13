#ifndef XORSTRING_H
#define XORSTRING_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


// 函数用于扩展密钥到与内容相同的长度
string expandKey(const string& key, size_t length) {
    string expandedKey;
    expandedKey.reserve(length);
    while (expandedKey.size() < length) {
        expandedKey += key;
    }
    // 截断扩展后的密钥到目标长度
    expandedKey.resize(length);
    return expandedKey;
}

/* @brief xorstring XOR加密
 * @param content 需要加密的内容
 * @param key 加密的key
 * @return 加密后的字符串
*/
string xorstring(const string& content, const string& key) {
    std::string expandedKey = expandKey(key, content.size());
    std::string encryptedContent;
    for (size_t i = 0; i < content.size(); ++i) {
        encryptedContent += content[i] ^ expandedKey[i];
    }
    return encryptedContent;
}

#endif