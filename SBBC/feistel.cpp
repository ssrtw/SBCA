﻿#include "feistel.h"

// 列印字串
// @param byte 要進行列印的vector
void print(vector<uint64_t>& v) {
    for (uint64_t ul : v) {
        printf("%016llx\n", ul);
    }
    printf("\n");
}

// 列印字串
// @param byte 要進行列印的vector
void print(vector<uint8_t>& byte) {
    for (int i = 0; i < byte.size(); i++) {
        cout << byte[i];
    }
}

// 做permutation
// @param block 要從放置換後的值
// @param from 原始資料
// @param position_from 從這個bit搬移
// @param position_to 搬到這個bit
void addbit(uint64_t& block, uint64_t from, int position_from, int position_to) {
    // 檢查如果為0就不用做shift與加法
    if (((from << (position_from)) & FIRSTBIT) != 0)
        block += (FIRSTBIT >> position_to);
}

// 對資料做反轉
// @param x 要反轉的資料
void reverse(uint64_t& x) {
    uint64_t n = x;
    n = ((n & 0xffffffff00000000) >> 32) | ((n & 0x00000000ffffffff) << 32);
    n = ((n & 0xffff0000ffff0000) >> 16) | ((n & 0x0000ffff0000ffff) << 16);
    n = ((n & 0xff00ff00ff00ff00) >> 8) | ((n & 0x00ff00ff00ff00ff) << 8);
    n = ((n & 0xf0f0f0f0f0f0f0f0) >> 4) | ((n & 0x0f0f0f0f0f0f0f0f) << 4);
    n = ((n & 0xcccccccccccccccc) >> 2) | ((n & 0x3333333333333333) << 2);
    n = ((n & 0xaaaaaaaaaaaaaaaa) >> 1) | ((n & 0x5555555555555555) << 1);
    x = n;
}

// 將uint64_t的前後半bit做對調
// @param val block左半值
// return Function算出的值
void highLowChange(uint64_t& val) {
    // uint64_t 是 8byte
    // >>16是移動4個byte，0xffffffff共32個1
    // 把高低兩邊對調
    val = (val >> 32) | ((val & 0xffffffff) << 32);
}

// 取sbox的值做替換
// @param val 原始資料
// @param hand 選用一個sbox
// @return 原始資料經過sbox替換後的值

uint32_t s(uint32_t val, int hand) {
    uint32_t res = 0;
    uint8_t tmp;
    for (int i = 0; i < 4; i++) {
        // 拿一個byte出來
        tmp = val >> 8 * i & 0xff;
        // 找box要替代的值
        res |= (sbox[hand][tmp >> 4][tmp & 0xf]) << 8 * i;
    }
    return res;
}

// 自訂的Feistel的Function
// @param val block左半值
// @param round 現在是第幾round
// @return Function算出的值
uint64_t F(uint64_t val, int round) {
    int lHand = hands[round][0], rHand = hands[round][1];
    uint32_t h = s((uint32_t)(val >> 32), lHand);
    uint32_t l = s((uint32_t)(val & 0xffffffffffffffff), rHand);
    val = (uint64_t)(h) << 32 | l;
    val ^= ssrH;
    reverse(val);
    val ^= ssrL;
    highLowChange(val);

    val ^= gojoH;
    reverse(val);
    val ^= gojoL;
    highLowChange(val);

    val ^= kirotoH;
    reverse(val);
    val ^= kirotoL;
    highLowChange(val);

    // Permutation
    uint64_t tmp = 0;
    for (int ii = 0; ii < 64; ii++)
        addbit(tmp, val, Pbox[ii], ii);
    val = tmp;
    return val;
}

// 運行Feistel結構做加密
// @param plain 明文vector
// @param cipher 密文vector
void encrypt(vector<uint64_t>& plain, vector<uint64_t>& cipher) {
    uint64_t L, R;
    for (int i = 0; i < plain.size(); i += 2) {
        L = plain[i];
        R = plain[i + 1];
        // 跑round
        for (int j = 0; j < 16; j++) {
            // 算F函數值
            uint64_t f = F(R, j);
            // 把左半XOR F函數的結果
            L ^= f;
            // 不是最後一round都要左右交換
            if (j != 15) {
                // swap兩邊
                R ^= L;
                L ^= R;
                R ^= L;
            }
        }
        cipher.push_back(L);
        cipher.push_back(R);
    }
}

// 運行Feistel結構做解密
// @param cipher 密文vector
// @param plain 明文vector
void decrypt(vector<uint64_t>& cipher, vector<uint64_t>& plain) {
    uint64_t L, R;
    for (int i = 0; i < cipher.size(); i += 2) {
        L = cipher[i];
        R = cipher[i + 1];
        // 跑round
        for (int j = 15; j >= 0; j--) {
            // 算F函數值
            uint64_t f = F(R, j);
            // 把左半XOR F函數的結果
            L ^= f;
            // 不是最後一round都要左右交換
            if (j != 0) {
                // swap兩邊
                R ^= L;
                L ^= R;
                R ^= L;
            }
        }
        plain.push_back(L);
        plain.push_back(R);
    }
}

// 將std::string轉為vector<uint8_t>
// @param str 字串
// @param byte 字串轉換後的vector
void str2Byte(string str, vector<uint8_t>& byte) {
    for (uint8_t c : str) {
        byte.push_back(c);
    }
}

// 將byte轉回uint64_t
// @param byte 要做資料轉換的vector
// @param data 存放轉換後的vector
void byte2Data(vector<uint8_t>& byte, vector<uint64_t>& data) {
    uint8_t* tmp = new uint8_t[DATA_SIZE];
    // 重新組裝為8byte作為半個block，這樣加密時就能一個一個讀取了
    for (int i = 0; i < byte.size(); i += DATA_SIZE) {
        for (int j = 0; j < DATA_SIZE; j++) {
            tmp[j] = byte[i + j];
        }
        // 將陣列uint64_t指標後取值
        data.push_back(*((uint64_t*)tmp));
    }
}

// 將uint64_t轉回byte
// @param byte 要做資料轉換的vector
// @param data 存放轉換後的vector
void data2Byte(vector<uint64_t>& data, vector<uint8_t>& byte) {
    for (uint64_t i : data) {
        byte.push_back(i & 0xff);
        byte.push_back((i >> 8) & 0xff);
        byte.push_back((i >> 16) & 0xff);
        byte.push_back((i >> 24) & 0xff);
        byte.push_back((i >> 32) & 0xff);
        byte.push_back((i >> 40) & 0xff);
        byte.push_back((i >> 48) & 0xff);
        byte.push_back((i >> 56) & 0xff);
    }
}

// 於明文後面加上padding
// @param byte 要做資料轉換的vector
void addPadding(vector<uint8_t>& byte) {
    // 檢查要padding多少
    // https://asecuritysite.com/encryption/padding_des
    // 使用RFC 5652(PKCS#7 Padding)方式，Padding到%16=0
    int needPadding = 16 - (byte.size() % 16);
    for (int i = 0; i < needPadding; i++) {
        byte.push_back(needPadding);
    }
}

// 移除解回明文的padding
// 以PKCS#7 padding
// @param byte 要移除padding的vector
void removePadding(vector<uint8_t>& byte) {
    uint8_t check = byte[byte.size() - 1];
    for (int i = 0; i < check; i++)
        byte.pop_back();
}