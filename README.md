![](https://picdl.sunbangyan.cn/2023/10/27/ca57115d66d7836ce70c5f00f9b364ea.png)

# Ez3DGL 

tldr: A Multi-layer Graphics Rendering Engine

一个对OpenGL多层抽象封装的图形学渲染引擎,可以帮助开发者快速绘制复杂的3D物体场景,而无需接触繁琐的OpenGL本身

[示例 Demos ![A Game made by Ez3DGL](https://github.com/MrAMS/Ez3DGL_demos/raw/master/screenshot/jetgame1.png)](https://github.com/MrAMS/Ez3DGL_demos)



## 框架设计

对OpenGL诸多底层概念进行抽象,封装为对象,开发者直接操作对象实现对OpenGL的控制

框架计划共三层抽象,依次自底向上,每一层继承建立于上一层抽象:
- `vertices` layer  对顶点层次的抽象,操作底层VAO,shader等概念
- `mesh`     layer  对网格层次的抽象,操作由顶点,纹理等组成的渲染对象(如由Assimp加载的模型网格)
- `entity`   layer  对实体层次的抽象,提供简单的物理引擎和场景管理等功能

```
  YOUR IDEA 
|-----------|   HIGH    LEVEL
|  entity   |   
|-----------|   ^
|  mesh     |   |
|-----------|   |
|  vertices |   
|-----------|   LOW     LEVEL
   OpenGL   
```

## 框架特性

- 多层次的抽象, 可灵活组合, 在需要定制的部分选择低层次抽象进行开发, 以获得最大的灵活性, 在普通业务逻辑的部分选择高层次抽象进行开发, 以获得最大的开发效率
- 封装了对于model矩阵的操作, 提供更友好的接口进行平移,旋转,缩放等变换, 支持父子关系绑定, 支持四元数运算
- 封装了对于顶点VAO, VBO, EBO等概念, 提供更友好的接口进行顶点数据的加载管理
- 封装了对纹理的抽象, 支持从图片文件和内存中加载纹理
- 封装了对着色器的抽象, 自动管理OpenGL上下文, 着色器的纹理绑定, 灯光设置等等, 高层抽象层提供开箱即用的预设
- 封装了灯光光源, 支持任意数量, 多种类型的光源, 包括平行光,点光源,聚光灯等
- 封装了简单的物理引擎, 提供飞行器的物理模型预设
- 封装了对于GLFW和IMGUI的初始化, 提供开箱即用的OpenGL环境, ImGui环境和窗口界面 



## 框架代码

```
├── core                        # 核心封装
│   ├── entity_layer.hpp            # entity 层面封装
│   ├── mesh_layer.hpp              # mesh 层面封装
│   └── vertices_layer.hpp/cpp      # vertices 层面封装
├── README.md
├── utils                       # 辅助工具
│   ├── debug.hpp                   # 调试工具
│   └── preset.hpp/cpp              # 实用预设
└── window                      # 窗口运行时,提供GLFWwindow,ImGui环境
    ├── window.cpp
    └── window.hpp
```

## 框架进度

- 已完成`vertices_layer`, 支持多种类型(点光源, 平行光源, 聚光灯)多个光源
- 正在完善`mesh layer`, 目前已支持加载显示`obj, fbk`等模型
- 未来进行 `entity layer` 的开发,目标实现场景内物体的自动管理和简单的物理模型

欢迎 Star&Pr
