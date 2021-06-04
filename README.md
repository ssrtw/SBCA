# スターバストブロック暗号

Star Burst Block Cipher(SBBC, 星爆區塊密)
109TTU網路安全期末作業，設計Feistel結構的加解密演算法
需要在中間加入與自己相關訊息，其餘部分主要與星爆或桐人有關。

## 說明

使用Feistel結構的演算法
Block size是128bit
Key size是128bit
每個Block執行16 Round
使用S-Box與Permutation，與DES一樣為符合[香農](https://zh.wikipedia.org/wiki/克劳德·香农)的[混淆與擴散](https://zh.wikipedia.org/wiki/混淆與擴散)
用PKCS#7作padding

## sbox

sbox是用48763^48763^的最高位開始排下來的hex值，共取8個sbox，$\because$每個sbox為$16\times16$，$\therefore8\cdot16\cdot16=2048$，共取2048個byte。
計算的答案來源：[網址](https://defuse.ca/big-number-calculator.htm)

拿雙手動作來作為選用sbox的順序，因往下的動作較少，將前刺的動作於偶數次使用sbox。

| 動作         | 對應sbox編號 |
| ------------ | ------------ |
| 前(odd)或上  | 0            |
| 後(even)或下 | 1            |
| 往左         | 2            |
| 往右         | 3            |
| 左上         | 4            |
| 右上         | 5            |
| 左下         | 6            |
| 右下         | 7            |

分解動作(已省略部分動作，只保留兩劍同時的動作)：
1. 右劍由左向右揮，左劍刺入後從右上帶出
2. 右劍由左向右揮，左劍由左向右揮
3. 右手在上，左手在下，作平行狀，由左向右揮
4. 左右手同時舉起，劃X字形(往下的)
5. 再以相反方向再劃一次X形(往上斬出)
6. 雙手同時突刺
7. 左劍從左下帶出，右劍從右上帶出
8. 雙劍突刺
9.  右劍由左上往右下揮，同時左劍由右下往左上揮
10. 左劍由左上往右下揮，右劍由右下往左上揮
11. 右劍由右上往左下揮，旋轉完畢同時左劍由右往左揮
12. 此時右劍由右上往左下格擋，左劍由下往上揮
13. 右劍由右上往正下方揮，左劍在下，作平行狀，由左向右揮
14. 左劍在上，右劍在下，作平行狀，由右向左揮
15. 左劍在上，右劍在下，作平行狀，由左向右揮
16. 右劍突刺，左劍突刺

照上表格16個round用不同的sbox
| 動作 | 右    | 左    | 右手 | 左手 |
| ---- | ----- | ----- | ---- | ---- |
| 1    | 向右  | 右上  | 3    | 5    |
| 2    | 向右  | 向右  | 3    | 3    |
| 3    | 向右  | 向右  | 3    | 3    |
| 4    | 左下  | 右下  | 6    | 7    |
| 5    | 右上  | 左上  | 5    | 4    |
| 6    | 前刺1 | 前刺1 | 0    | 0    |
| 7    | 右上  | 左下  | 5    | 6    |
| 8    | 前刺2 | 前刺2 | 1    | 1    |
| 9    | 右下  | 左上  | 7    | 4    |
| 10   | 左上  | 右下  | 4    | 7    |
| 11   | 左下  | 往左  | 6    | 2    |
| 12   | 左下  | 往上  | 6    | 0    |
| 13   | 正下  | 向右  | 1    | 3    |
| 14   | 向左  | 向左  | 2    | 2    |
| 15   | 向右  | 向右  | 3    | 3    |
| 16   | 突刺3 | 突刺3 | 0    | 0    |

## Permutation

Feistel F function的置換表
第一列前五個值48763是星爆氣流斬，第一欄(idx=0,8,16...)是學號:410612011

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

## TODO

- [x] 包裝函數
- [ ] 加入test
- [x] 將目前的64bit Block變成128bit
- [x] 加入sbox
- [ ] 加入Key，算出16個Round的key
- [ ] 用Initial permutation，與IP^-1^
- [x] 把目前padding使用的PKCS#5改成PKCS#7
- [ ] 把ECB模式換成其他[區塊加密法工作模式](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)