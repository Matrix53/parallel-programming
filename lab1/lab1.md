## 题目描述

**蒙特卡罗法**也称统计模拟法、统计试验法。是把概率现象作为研究对象的数值模拟方法。是按抽样调查法求取统计值来推定未知特性量的计算方法。蒙特卡罗是摩纳哥的著名赌城，该法为表明其随机抽样的本质而命名。

使用蒙特卡罗方法可以计算 PI 值，其具体方案为在一个正方形中作内切圆，向正方形中随机投掷 n 个足够多的点，根据面积关系计算 PI 值。

请你使用 **MPI** 实现蒙特卡罗方法计算 PI 值的并行化。

运行方式：`mpirun -np -8 <<excutable>>`

## 输入形式

一行一个数字，表示一个进程模拟投掷的次数

## 输出形式

输出计算出的 PI 值

## 输入样例

```
10000000
```

## 输出样例

```
3.141369
```
