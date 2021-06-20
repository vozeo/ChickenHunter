# ChickenHunter
同济大学软件工程荣誉课2021年高级程序设计课程期末大项目

### 编译のTips

**Windows端**:

``` cmd
mkdir win32-build
cd win32-build
cmake .. -G "Visual Studio 16 2019" -A Win32
```

**Android端**:

由于使用了 ***yasio®*** 三方库，请确保 ***cmake*** 版本大于 **3.13.0** , ***gradle*** 版本大于等于 ***4.2.1***

### 基本操作

|     操作     |        效果        |
| :----------: | :----------------: |
| 按下`WSAD`键 | 上、下、左、右移动 |
| 点击鼠标左键 | 向鼠标位置进行攻击 |
|  按下空格键  |  人物翻滚（瞬移）  |
|  按下`E`键   |      捡拾物品      |
|   设置按钮   |   设置音量和外挂   |

### 特别鸣谢

- [li-letian/Monopoly: 软件荣誉课大一下项目 (github.com)](https://github.com/li-letian/Monopoly)
- [ZhichenRen/cocos2d-xGameDesign: 2020 C++ Assignment (github.com)](https://github.com/ZhichenRen/cocos2d-xGameDesign)
- https://github.com/Shmiwy/soul-knight
- 李乐天、Konakona在编写过程中提供的指导和帮助
- 许芷瑜、张子扬、李文科、谈瑞、杨明月、王翾龙、任嘉怡、刘铭烜、杨嘉仪等人参与了游戏的测试，提供了宝贵的意见
- 其他帮助过我们的小伙伴们

  
