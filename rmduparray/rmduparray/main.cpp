//
//  main.cpp
//  rmduparray
//
//  Created by liweijian on 2017/1/14.
//  Copyright © 2017年 liweijian. All rights reserved.
//

#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::endl;

using sizetype = typename vector<int>::size_type;

//方式1, 使用stl， distance, unique
template <typename T>
sizetype removedupinarray(vector<T> &v)
{
   return std::distance(v.begin(), std::unique(v.begin(), v.end()));
}


//方式2
template <typename T>
sizetype removedupinarray2(vector<T> &v)
{
    if (v.empty()) {
        return 0;
    }
    
    sizetype index = 0;
    sizetype len = v.size();
    
    for (int i = 0; i < len; i++ ) {
        if (v[i] != v[index]) {
            v[++index] = v[i];
        }
    }
    
    return index + 1;
}

//方式3，用迭代器实现方式2
template <typename T>
sizetype removedupinarray3(vector<T> &v)
{
    return std::distance(v.begin(), removedupinarray3(v.begin(), v.end(), v.begin()));
}

template <typename InIter, typename OutIter>
OutIter removedupinarray3(InIter first, InIter last, OutIter ouput)
{
    while (first != last) {
        *ouput++ = first;
        first = std::upper_bound(first, last, *first);
    }
    
    return ouput;
}

int main(int argc, const char * argv[]) {
   
    vector<int> vc{1,1,1,2,3,3,4,4,4,4,4,5,5};
    //vector<int>::size_type len = removedupinarray(vc);
    //vector<int>::size_type len = removedupinarray2(vc);
    vector<int>::size_type len = removedupinarray2(vc);
    std::for_each(vc.begin(), vc.begin()+len, [](int num){cout << num <<" ";});
    
    return 0;
}
