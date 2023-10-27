# Ez3DGL 

一个对OpenGL多层抽象封装的框架,可以帮助开发者快速绘制复杂的3D物体场景,而无需接触繁琐的OpenGL本身

## 框架设计

对OpenGL诸多底层概念进行抽象,封装为对象,开发者直接操作对象实现对OpenGL的控制

计划框架共三层抽象,依次自底向上,每一层继承建立于上一层抽象:
- vertices layer    对顶点层次的抽象,操作底层VAO,shader等概念
- mesh layer        对网格层次的抽象,操作由Assimp加载的模型网格
- object layer      对物体层次的抽象,操作在世界中具有速度,重力的实体

```
|-----------|
|object     |
|-----------|
|mesh       |
|-----------|
|vertices   |
|-----------|
```

## 框架代码

```
├── core                        # 核心封装
│   ├── mesh_layer.hpp/cpp          # mesh 层面封装
│   └── vertices_layer.hpp/cpp      # vertices 层面封装
├── ray_tracker.hpp             # 光线追踪[TODO]
├── README.md
├── utils                       # 辅助工具
│   ├── debug.hpp                   # 调试工具
│   └── preset.hpp/cpp              # 实用预设
└── window                      # 窗口运行时,提供GLFWwindow,ImGui环境
    ├── window.cpp
    └── window.hpp
```

## 框架进度

- 仍在完善 `vertices layer`, 已实现 `vao`,`shader`,`texture`,`camera`,`light` 等概念的封装,目前正在完善灯光支持
- 下一步进行 `mesh layer` 的开发,目标实现通过`Assimp`加载模型并显示
- 完善框架examples

欢迎 Star&Pr