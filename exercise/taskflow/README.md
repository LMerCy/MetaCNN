[toc]
https://netcan.github.io/presentation/metaprogramming
学习笔记

# 1. 基础数据类型



# 2. 高阶函数

高阶函数的主要作用是？（消除循环？）

## map

输入是列表和函数，输出是对输入列表中的每一个元素调用函数后的列表

## filter

输入是列表和谓词函数，输出是对列表中的每一个元素进行过滤操作，输出为只保留谓词函数为真的元素的列表。

## folder

输入为一个列表，二元函数和初值init,输出为一个元素，是输入列表中的每一个元素被二元函数递归调用的结果。