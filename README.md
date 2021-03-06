# Starburst Block Cipher Algorithm

109TTU網路安全期末作業，設計Feistel結構的加解密演算法
星爆區塊加密演算法、スターバストブロックサイファーアルゴリズム
加密過程中需加入與個人資料，除去個人資料其餘內容主要以星爆或桐人有關。

## 說明

* 使用Feistel結構的演算法
* Block size是128bits
* 每個Block執行16 Round
* 使用的key不定長度，會做運算得到16個64bits的roundkey
* 使用S-Box與Permutation，參考自DES為符合[混淆與擴散](https://zh.wikipedia.org/wiki/混淆與擴散)
* 用PKCS#7作padding

## 加密流程

### Block

1. 每次讀取128bits(一個block)
2. block拆分為L與R兩半，將R(低的64bits)丟入Feistel Function中與round key進行運算
3. 將運算結果與L(高的64bits)與2的結果做XOR
4. 將左右兩半bits交換，如果是最後1 round就不交換
5. 重複以上四個動作，共進行16次

![Feistel](img/Feistel.svg)

### Round

1. 輸入64bits與round key做XOR
2. 分拆為兩個32bits，放入該round對應到的兩個sbox做置換
3. 將置換完的值放入定義好的G函數中，與兩個常數做運算。共執行三次G函數運算
4. 經由Permutation box將資料的bit調換位置

![Function](img/Function.svg)

### sbox

sbox是用48763^48763^的最高位開始排下來的hex值，共取8個sbox。![Equation](https://math.now.sh?from=%5Cbecause%20Per%5C%20sbox%3D16%5Ctimes16%28bits%29%5Ctherefore8%5Ccdot16%5Ccdot16%3D2048%28bits%29)，共取2048個byte。係使用[這個網站](https://defuse.ca/big-number-calculator.htm)計算運算出結果。
16個round每次都選用不同的box，選用理由是桐人對青眼惡魔使用星爆氣流斬雙手動作決定(16連擊對應到16個round)。
雙手動作如[sbox選取依據](select_sbox.md)md所寫。
因此每個round對應的sbox：

| round | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  | 15  | 16  |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 左    | 3   | 3   | 3   | 6   | 5   | 0   | 5   | 1   | 7   | 4   | 6   | 6   | 1   | 2   | 3   | 0   |
| 右    | 5   | 3   | 3   | 7   | 4   | 0   | 6   | 1   | 4   | 7   | 2   | 0   | 3   | 2   | 3   | 0   |

將half block的64bits，再拆為兩半32bits，分別將32bit丟入上方表格對應的每round的兩個sbox取出對應值。

![sbox](img/sbox.svg)

### G函數

下面是G函數的結構，Я為將bit做反轉的操作

1. 輸入64bit的資料XOR資料與輸入的第一個常數
2. 對1算完的值做bit的反轉
3. XOR反轉後的值與第二個常數
4. 輸出交換高低兩個32bits後的值

![FuncG](img/FuncG.svg)

### Permutation

Feistel Function的置換表
第一列前五個值48763是星爆氣流斬
第一欄(idx=0,8,16...)是學號:410612011

| 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 4   | 8   | 7   | 6   | 3   | 14  | 16  | 12  |
| 10  | 40  | 51  | 57  | 59  | 60  | 29  | 42  |
| 61  | 2   | 15  | 1   | 25  | 31  | 0   | 22  |
| 20  | 54  | 50  | 48  | 58  | 55  | 18  | 36  |
| 11  | 9   | 13  | 26  | 24  | 30  | 35  | 43  |
| 63  | 33  | 46  | 32  | 56  | 62  | 41  | 53  |
| 49  | 23  | 19  | 17  | 27  | 21  | 28  | 5   |
| 44  | 39  | 38  | 45  | 34  | 47  | 52  | 37  |

## key generate

設計一個可以將不定長度的key產生round key的方法(用hash的方式)。
先將key讀取進來，將其轉為byte vector並padding。
padding的方法如MD5與SHA2一樣，先padding 100...00足到448bit，再補上64bit的原始字串的長度數值，使這個vector的bit數能被512整除。
hash的過程如下圖，跟MD5結構一樣，但是我將部分F的函數換掉，把左旋的函數改成右旋。

將128bits分為4個32bits:a, b, c, d。
將a, b, c, d的初始值設為：

| 1        | a          | b          | c                | d                |
| -------- | ---------- | ---------- | ---------------- | ---------------- |
| 初始值   | 0x19990423 | 0x20081007 | 0x5ce34160       | 0xa1f04b47       |
| 找值原因 | 我的生日   | 桐人生日   | C87取63最高4byte | C87取63次高4byte |

每次對block進行運算會跑64round，分成4組16次的迴圈。
M~i~表示32bits的輸入資料，i從0到15，共512bits(一個block大小)。
| round | function                               |
| ----- | -------------------------------------- |
| 0~15  | (a>>>2)\^(a>>>13)\^(a>>>22)            |
| 16~31 | (d>>>6)\^(d>>>11)\^(d>>>25)            |
| 32~47 | b\^c\^d                                |
| 48~63 | c ^ (b \| (~d)) + (a & b) ^ ((~a) & c) |

完整生成round key的結構圖：

![keyHash](img/KeyHash.svg)

### IP

加密演算法的IP與IP^-1^借用DES的IP，但把原本最後8bit的值跟48763(48, 7, 63)與410612011(4, 10, 61, 20, 11)這兩組值能組成的8個值交換位置，如下表。

|     |     |     |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 57  | 49  | 41  | 33  | 25  | 17  | 9   | 1   |
| 59  | 51  | 43  | 35  | 27  | 19  | 6   | 3   |
| 22  | 53  | 45  | 37  | 29  | 21  | 13  | 5   |
| 46  | 55  | 47  | 39  | 31  | 23  | 15  | 54  |
| 56  | 62  | 40  | 32  | 24  | 16  | 8   | 0   |
| 58  | 50  | 42  | 34  | 26  | 18  | 30  | 2   |
| 60  | 52  | 44  | 36  | 28  | 14  | 12  | 38  |
| 48  | 7   | 63  | 4   | 10  | 61  | 20  | 11  |

### CBC mode

加密程式提供基本的ECB與CBC模式。CBC的模式如[Wiki](https://zh.wikipedia.org/wiki/區塊加密法工作模式)的方式，但在IV要與資料做XOR時，額外XOR了經由hash生成的key[0]與key[1]，希望比固定的IV值來得好的效果。
InitialVectorL = 0x9E3779B97F4A7C15 =>黃金比例小數後前16個hex
InitialVectorR = 0x93C467E37DB0C7A4 =>尤拉數小數後前16個hex


### 程式的參數列flag
| Flag         | 說明                                                                                    |
| ------------ | --------------------------------------------------------------------------------------- |
| -c,--cbc     | 以CBC模式加解密，不輸入代表使用ECB                                                      |
| -k           | 加解密用的密鑰，預設為I4B53-410612011                                                   |
| -I,--input   | Input的filename，解密時一定要使用-i參數。加密時可用-t                                   |
| -t,--text    | 以字串進行加密，-t附帶的參數即是明文字串                                                |
| -o,--output  | Output的filename，若進行加密filename預設為plain.txt，若進行解密filename預設為cipher.bin |
| -v,--verbose | 輸出較多資訊，會輸出加(解)密花費時間與round key產生花費的時間                           |


### 開發環境

使用Visual Studio 2019作為IDE進行開發
C++的版本為MSVC C++17(因[argparse](http://github.com/p-ranav/argparse)函式庫必須在C++17規範使用)
