//
//  main.cpp
//  queryword
//
//  Created by liweijian on 2017/1/19.
//  Copyright © 2017年 liweijian. All rights reserved.
//

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <memory>
#include <set>
#include <sstream>

using std::cout;
using std::endl;
using std::ostream;
using std::vector;
using std::string;
using std::map;
using std::set;
using std::ifstream;
using std::shared_ptr;
using std::istringstream;


class QueryResult
{
    friend ostream& print(ostream&, const QueryResult&);
public:
    using lineno = vector<string>::size_type;
    QueryResult(string s,
                shared_ptr<set<lineno>> p,
                shared_ptr<vector<string>> f):
    sought(s),lines(p),file(f) {};
    
private:
    string sought; //查询单词
    shared_ptr<set<lineno>> lines; //出现的行号
    shared_ptr<vector<string>> file;  //输入文件
};


class TextQuery
{
public:
    using lineno = vector<string>::size_type;
    TextQuery(ifstream&);
    QueryResult query(const string&) const;
    
private:
    shared_ptr<vector<string>> file;
    map<string, shared_ptr<set<lineno>>> wm;
};

TextQuery::TextQuery(ifstream &is):file(new vector<string>)
{
    string text;
    while (getline(is, text)) {
        file->push_back(text);
        lineno n = file->size() - 1; // 当前行号
        istringstream line(text);
        string word;
        while (line >> word) {
            auto &lines = wm[word];
            if(!lines) {//第一次，分配一个set
                lines.reset(new set<lineno>);
            }
            lines->insert(n);
        }
    }
}

QueryResult TextQuery::query(const string &s) const
{
    static shared_ptr<set<lineno>> nodata(new set<lineno>);
    auto loc = wm.find(s);
    if (loc == wm.end()) {
        return QueryResult(s, nodata, file);
    }else {
        return QueryResult(s, loc->second, file);
    }
}


//打印结果
ostream &print(ostream & os, const QueryResult &qr)
{
    os << qr.sought << "出现了" << qr.lines->size() << "次" <<endl;
    
    for (auto num: *qr.lines) {
        cout <<"\t (行 "<< num +1 <<")"
        << *(qr.file->begin() + num) << endl;
    }
    
    return os;
}


int main(int argc, const char * argv[]) {
    
    ifstream file("/Users/liweijian/Code/c++/queryword/queryword/File");
    if (file.is_open()) {
        TextQuery tq(file);
        print(cout, tq.query("she"));
    }
   
    
    return 0;
}
