# NEWorld结构说明

更新时间: 16/8/31

## NEWorld(Shared) 

### Common

| 类                  | 文件                | 作用               | 内含   | 备注                | 单例/static |
| ------------------ | ----------------- | ---------------- | ---- | ----------------- | --------- |
| AABB               | aabb.h            | 碰撞箱              |      |                   |           |
| CommandExecuteStat | command.h         | 命令执行结果           |      |                   |           |
| Command            | command.h         | 一条指令实例           |      |                   |           |
| CommandInfo        | command.h         | 一条指令的信息          |      | 作者、帮助信息等          |           |
| -                  | consolecolor.h    | Logger用的颜色       |      |                   |           |
| Logger             | logger.h          | 用于输出             |      | 每次输出都会实例化一个Logger |           |
| -                  | settings.cpp      | 用于加载shared的设置    |      | 调用Settings类       |           |
| Settings           | settingsmanager.h | 用于加载设置           |      |                   |           |
| -                  | type.h            | 用于把string转换成对应类型 |      |                   |           |
| -                  | utils.h           | 一些实用工具           |      |                   |           |
|                    | vec3.h            | 三维向量             |      |                   |           |


## NEWorld(Client)

### Main

| 类           | 文件        | 作用                    | 内含                          | 备注   | 单例/static |
| ----------- | --------- | --------------------- | --------------------------- | ---- | --------- |
| Application | NEWorld.h | 客户端程序的开始，管理整个程序的全局变量。 | BlockManager, PluginManager |      | Y         |

### Network

客户端的网络部分，结构未确定

### Player

玩家部分，结构未确定

### Renderer

渲染相关的代码。

| 类             | 文件              | 作用                 | 内含                              | 备注                            | 单例/static |
| ------------- | --------------- | ------------------ | ------------------------------- | ----------------------------- | --------- |
| ChunkRenderer | chunkrenderer.h | 用于渲染单个chunk。       | VertexBuffer和static VertexArray | 和chunk是一对一的。                  |           |
| Mat4          | Mat4.h          | 矩阵类。用来处理矩阵运算。      |                                 |                               |           |
| -             | opengl.h        | 用于引入OpenGL。        |                                 | 所有用到OpenGL的文件都应该#include这个文件。 |           |
| Renderer      | renderer.h      | 渲染器。底层的渲染器，用来渲染图形。 |                                 |                               | Y         |
| Texture       | texture.h       | 材质类。用于加载和储存材质。     |                                 |                               |           |
| VertexFormat  | vertexarray.h   | 用来储存VAO的格式。        |                                 |                               |           |
| VertexArray   | vertexarray.h   | 用来储存VAO。           | VertexFormat                    |                               |           |
| VertexBuffer  | vertexarray.h   | 用来储存VBO。           | VertexFormat                    |                               |           |
| WorldRenderer | worldrenderer.h | 用来渲染整个世界。          | ChunkRenderer                   |                               | Y         |

### Scene

| 类          | 文件          | 作用      | 内含                           | 备注   | 单例/static |
| ---------- | ----------- | ------- | ---------------------------- | ---- | --------- |
| GameScene  | gamescene.h | 整个游戏场景。 | World, Player, WorldRenderer |      | Y [1]     |
| MainMenu   | mainmenu.h  | 菜单场景。   |                              |      | Y [1]     |
| BackGround | mainmenu.h  | 菜单的背景。  |                              |      | Y [1]     |
| MainWindow | window.h    | 窗口类。    |                              |      | Y [1]     |

[1] 整个游戏过程中只会**同时**存在一个

### main.cpp

main函数所在的文件