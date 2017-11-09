# OpenSLDemo
OpenSlES Demo
OpenSLES 学习测试demo，完成实时采集及播放功能
在三星Note3测试问题：
1. output会自动中断，回调不执行；
2. input容易在调用opensles内部接口因内存分配释放等崩溃，可能是该手机不支持低延时方案的原因
