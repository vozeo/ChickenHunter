# ChickenHunter
A 2D airsoft.

### 编译のtips

**Windows端**:

``` cmd
mkdir win32-build
cd win32-build
cmake .. -G "Visual Studio 16 2019" -A Win32
```

**Android端**:

由于使用了 ***yasio®*** 三方库，请确保 ***cmake*** 版本大于 **3.13.0** , ***gradle*** 版本等于 ***4.2.1***

