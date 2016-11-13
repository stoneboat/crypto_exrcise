## 作业提交人 卫煜 信安 1410657
## 古典移位密码
### 算法流程图
![](flow_control_transfer.png)
### 运行结果
![](transfer_demo.png)
## 古典置换密码
### 算法流程图
![](flow_control_permutation.png)
### 运行结果
![](permutation_demo.png)
## 频率攻击
### 置换表结果
![](exploition_resource1.png)
### 攻击步骤
- 统计密文字母频率分布表，对照已知大样本统计结果，可以确定前六个对应字母对应关系。（见上图）
- 统计密文单词频率分布表，对照已知大样本统计结果，可以确认上一步结果，并确定字母I对应关系 (下为部分统计结果图)
![](exploition_resource2.png)
+ 从网上查找常用字母的数据集，和密文中单词进行匹配，缩小候选范围，下举一例
    * 确定密文文本 QCRRNEC <br>
    ![](exploit_demo01.png)
    * 依据前两步结果 可翻译为 Q 'E' RR 'A' E 'E' (引号中为翻译后明文)<br>
    构造正则表达式<br>
    ![](exploit_demo02.png)
    * 从网上获取七个字母的英文单词集合 <br>
    ![](exploit_demo03.png)
    * 程序匹配，输出可能结果<br>
    ![](exploit_demo04.png)
    * 最后综合确定明文为 message ，即可得到几个字母新的对应结果
- 重复上步若干次，越来越容易得出结论

---
### 参考链接
[bestwordlist.com](https://www.bestwordlist.com/)
-题目<br>
![](exploit_demo05.png)