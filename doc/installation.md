# NEWorld编译指南

### Windows
VS配置方法(32位)：

1. 安装VS2015或以上版本

2. Clone NEWorld

3. 下载dependencies.zip(位于新创无际开发组 QQ群 308354849，有空发上去)，解压到.\

4. 安装Python(推荐Python3，但Python2也可以)

5. 运行./scripts/bootstrap.bat

6. 打开./build/visualstudio_v14\NEWorld.sln，设置调试设置

   RunClient调试设置：
   
   - 命令 NEWorldLauncher.exe
   
   - 命令参数 client
   
   - 工作目录 $(SolutionDir)../../release/

   RunServer调试设置：

   - 命令 NEWorldLauncher.exe
   
   - 命令参数 server
   
   - 工作目录 $(SolutionDir)../../release/

7. 下载boost, 复制（或软连接）boost/boost到VC/include或dependencies/include

8. 编译运行！


VS配置方法(64位)：

配置方法和32位雷同，只是将相关文件改为64位版本，请自行摸索~

### Linux

Compile NEWorld in Linux

1. You need have the dependences installed.
   dependences: git,gcc/clang,cmake,boost(1.61.0+),libgl,glew,sdl2,sdl_image,freetype2,alsa,raknet,flatbuffers,gtest(optional)
2. git clone https://github.com/Infinideastudio/NEWorld.git
3. mkdir /build/cmake/
4. cd /build/cmake/
5. cmake ../../
6. make
