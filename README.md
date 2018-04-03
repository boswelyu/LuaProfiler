这是一个tolua的扩展，可以用来跟踪各个lua函数的执行时间来进行性能分析和调优。目前支持Unity Editor和Android设备两种调试环境。

使用方法：
1、检出tolua runtime的源代码
2、将本目录下的四个文件添加到tolua runtime的源代码目录下，并在tolua的makefile文件中添加三个C源文件
3、编译tolua动态库，编译成功后替换Unity工程或Android工程目录选的tolua.so库
4、在需要开始跟踪的时候，调用profiler.start(); 在结束跟踪的时候后调用profiler.stop()，在这期间收集到的运行数据就会被输出到记录文件中。在PC上的Unity Editor上，输出文件位于你的Unity工程项目的根目录，文件名为lua_cpu_profiler.out；在Android设备上，信息会被输出到logcat日志上，需要一点点grep处理工作，把需要的信息弄到一个输出文件上做后续的分析；
5、用summary.lua分析是一步中生成的数据文件，就可以得到收集过程中各个lua函数的耗时情况了。

使用中有任何问题，可以发邮件给我：yubooster@163.com，或者加我微信 Y2318105381 咨询，我会尽快回复。

其他说明：这个工具参考了Kepler同学的LuaProfiler项目，项目链接：http://www.keplerproject.org/luaprofiler
但是使用中发现这个工具的输出数据有点奇怪，而且在Android设备上会崩溃，所以就按照自己的思路，重写了数据收集部分的代码。
这个工具第一版提交的时候，甚至还保留了Kepler同学的版权声明。但是因为大部分代码都是重写的，所以后来把版权声明改成了我自己的。
如果有人认为我是属于侵权，或者Kepler同学看到了觉得不高兴了，都请立刻马上直接告诉我。我会认真听取您的所有意见。

This tool referenced LuaProfiler which made by Kepler (http://www.keplerproject.org/luaprofiler). But I found I always get some odd output in my project, besides, it always crash on android device. So I rewrite the data collection logic according to my own idea. 
I still reserved Kepler's copyright on my first version of code, while I replaced it by mine considering most of code are changed.
Mr. Kepler, if you see this and feel not happy about that, please feel free to drop me a mail (yubooster@163.com) to let me know.
