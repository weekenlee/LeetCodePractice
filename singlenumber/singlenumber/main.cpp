//
//  main.cpp
//  singlenumber
//
//  Created by liweijian on 16/7/21.
//  Copyright © 2016年 liweijian. All rights reserved.
//

#include <iostream>
#include <vector>
#include <numeric>
using namespace std;


int singlenum(vector<int> &num) {
    return accumulate(num.cbegin(), num.cend(), 0, bit_xor<int>());
}


int main(int argc, const char * argv[]) {
    vector<int> arr = {1,1,2,3,4,4,3};
    std::cout << singlenum(arr) <<endl;
    return 0;
}
