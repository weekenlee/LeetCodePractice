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
#include <iterator>

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
    using lineit = set<lineno>::const_iterator;
    QueryResult(string s,
                shared_ptr<set<lineno>> p,
                shared_ptr<vector<string>> f):
    sought(s),lines(p),file(f) {};
    
    shared_ptr<vector<string>> get_file() const
    {return file;}
    
    lineit begin() const {return lines->cbegin();}
    lineit end() const {return lines->cend();}
    
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


class Query_base
{
    friend class Query;
    
protected:
    using lineno = TextQuery::lineno;
    virtual ~Query_base() = default;
    
private:
    virtual QueryResult eval(const TextQuery&) const = 0;
    virtual string rep() const = 0;
};


class Query
{
    friend Query operator~(const Query&);
    friend Query operator|(const Query&, const Query&);
    friend Query operator&(const Query&, const Query&);
public:
    Query(const string&);
    QueryResult eval(const TextQuery &t) const
    {
        return q->eval(t);
    }
    string rep() const
    {
        return q->rep();
    }
    
private:
    Query(std::shared_ptr<Query_base> query):q(query) {}
    std::shared_ptr<Query_base> q;
};


class WordQuery:public Query_base
{
    friend class Query;  //friend 不能被继承
    
    WordQuery(const string &s): query_word(s){};
    
    QueryResult eval(const TextQuery &qr) const
    {
        return qr.query(query_word);
    }
    
    string rep() const {return query_word;}
    
    string query_word;
};

inline Query::Query(const std::string &s) : q(new WordQuery(s)) {}


class NotQuery: public Query_base
{
    friend Query operator~(const Query &);
    NotQuery(const Query &q):query(q) {}
    
    string rep() const {return "~(" + query.rep() + ")";}
    QueryResult eval(const TextQuery &qr) const;
    Query query;
};

inline Query operator~(const Query &operand)
{
    return std::shared_ptr<Query_base>(new NotQuery(operand));
}


QueryResult
NotQuery::eval(const TextQuery &text) const
{
    auto result = query.eval(text);
    auto ret_lines = std::make_shared<set<lineno>>();
    auto beg = result.begin(), end = result.end();
    
    auto sz = result.get_file()->size();
    
    for (size_t n = 0; n != sz; ++n) {
        if (beg == end || *beg != n) {
            ret_lines->insert(n);
        }else if(beg != end){
            ++beg;
        }
    }
    
    return QueryResult(rep(), ret_lines, result.get_file());
}



class BinaryQuery: public Query_base
{
protected:
    BinaryQuery(const Query &l,  const Query &r, string s):lhs(l), rhs(r), opSym(s) {}
    
    string rep() const
    {
        return "(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")";
    }
    
    Query lhs, rhs;
    string opSym;
};


class AndQuery: public BinaryQuery
{
    friend Query operator&(const Query&, const Query&);
    AndQuery(const Query &left, const Query &right):BinaryQuery(left, right, "&"){}
    
    QueryResult eval(const TextQuery&) const;
};

inline Query operator&(const Query &lhs, const Query &rhs)
{
    return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

QueryResult
AndQuery::eval(const TextQuery& text) const
{
    auto left = lhs.eval(text), right = rhs.eval(text);
    auto ret_lines = std::make_shared<set<lineno>>();
    
    std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
                          std::inserter(*ret_lines, ret_lines->begin()));
    
    return QueryResult(rep(), ret_lines, left.get_file());
}


int main(int argc, const char * argv[]) {
    
    ifstream file("/Users/liweijian/Code/leetcode/queryword/queryword/File");
    if (file.is_open()) {
        TextQuery tq(file);
        
        print(cout, tq.query("你好"));
        
        Query query = Query("你好")&Query("hi");
        print(cout, query.eval(tq));
        
        Query query2 = ~(~Query("你好"));
        print(cout, query2.eval(tq));
        
    }
   
    
    return 0;
}
