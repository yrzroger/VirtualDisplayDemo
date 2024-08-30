### VirtualDisplayDemo
基于Android虚拟屏幕演示示例

#### 详细说明
参见文章：关注公众号 "Android元宇宙"，图形图像专栏获取程序解读
![图片](https://github.com/yrzroger/NativeSFDemo/assets/18068017/e4ddc7ce-cb94-4029-847c-cdabaa5f5dcd)


#### 分支
main : 基于Android 14平台开发 


#### 使用方法
1. 下载代码放到android源码目录下
2. 执行mm编译获得可执行档VirtualDisplayDemo
3. adb push VirtualDisplayDemo /system/bin/
4. adb shell VirtualDisplayDemo 运行程序
5. adb shell VirtualDisplayDemo -m , -m选项开启mirrorDisplay

    $ adb shell VirtualDisplayDemo  
       Create Virtual Display(layer_stack=99)  
       Press any key to exit, waiting ...  
       按下任意键后退出  
       Exit and destroy Virtual Display  


#### 效果展示

使用专栏中提供的绘图示例，进行显示测试  
https://mp.weixin.qq.com/s/_WF-wSe8b9uJF_obT-syLA  
https://mp.weixin.qq.com/s/8qTF8iacXFveJ71DGiRLEA  

NativeSFDemo 显示到指定的屏幕上    
adb shell NativeSFDemo -d layerStack    
![fxzay-5xwln](https://github.com/user-attachments/assets/5414042f-6c30-4434-9524-67e75b74f995)


MultiDisplayDemoPro 显示到指定的屏幕上 
adb shell MultiDisplayDemoPro -f /sdcard/Movies/video1.mp4 -d 2  

![z9arv-t58i0](https://github.com/user-attachments/assets/b77c08b1-6dc9-471d-a998-e4230c21098d)


-m选项开启mirrorDisplay并显示到虚拟屏幕上
![image](https://github.com/user-attachments/assets/e6786795-3c32-4f0c-8d9a-9b2ca09ae6d1)

![image](https://github.com/user-attachments/assets/89b1d48e-a6ae-4c21-b2ab-0dce2aa22466)


