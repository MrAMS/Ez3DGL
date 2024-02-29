![](https://picdl.sunbangyan.cn/2023/10/27/ca57115d66d7836ce70c5f00f9b364ea.png)

# Ez3DGL 

tldr: A Multi-layer Graphics Rendering Engine

一个对OpenGL多层抽象封装的图形学渲染引擎,可以帮助开发者快速绘制复杂的3D物体场景,而无需接触繁琐的OpenGL本身

## 框架设计

对OpenGL诸多底层概念进行抽象,封装为对象,开发者直接操作对象实现对OpenGL的控制

框架计划共三层抽象,依次自底向上,每一层继承建立于上一层抽象:
- `vertices` layer  对顶点层次的抽象,操作底层VAO,shader等概念
- `mesh`     layer  对网格层次的抽象,操作由顶点,纹理等组成的渲染对象(如由Assimp加载的模型网格)
- `scene`    layer  对场景层次的抽象,维护场景内对象

```
| your idea |
|-----------|   HIGH    LEVEL
|  scene    |   
|-----------|   ^
|  mesh     |   |
|-----------|   |
|  vertices |   
|-----------|   LOW     LEVEL
|  OpenGL   |
```

## 框架代码

```
├── core                        # 核心封装
│   ├── mesh_layer.hpp/cpp          # mesh 层面封装
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
- 未来进行 `scene layer` 的开发,目标实现场景内物体的自动管理和简单的物理模型

欢迎 Star&Pr
