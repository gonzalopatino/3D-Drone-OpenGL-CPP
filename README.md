# CS-330 Final Project – Gonzalo Patino
## 3D Drone Scene Using OpenGL

### Project Overview
This project represents the culmination of my work in the CS-330: Computational Graphics and Visualization course. I created a 3D drone model rendered in a virtual environment using OpenGL, with interactive camera controls, realistic textures, and modular code architecture.

---

## Reflection

### How do I approach designing software?
I approach software design with an emphasis on modularity and clarity. I begin by identifying the core components of the system and breaking them down into logical units. For this project, that meant structuring the drone into geometric parts like the body, arms, and camera. Each part was programmed as a distinct function to isolate concerns and keep the codebase manageable.

### What new design skills has your work on the project helped you to craft?
Through this project, I refined my hierarchical modeling skills and gained a deeper understanding of applying transformations in 3D space. I also developed a more deliberate use of lighting, texture mapping, and camera systems in OpenGL. Managing user interaction and scene complexity taught me the importance of a clean rendering pipeline and reusable code.

### What design process did you follow for your project work?
I followed an iterative, component-based design process. First, I outlined the drone’s structure and functionality. Then I focused on one element at a time, testing and adjusting before moving on. Each milestone helped incrementally shape the full scene. The use of a `SceneManager` and `RenderDrone()` function ensured separation of logic and easy updates.

### How could tactics from your design approach be applied in future work?
Breaking down problems into modular parts and using abstraction to separate concerns can be applied in any software development context. Whether it's a 3D simulation, web application, or AI system, this method ensures flexibility, readability, and scalability in development.

### How do I approach developing programs?
I develop programs using a problem-solving mindset rooted in iteration and testing. My approach begins with defining the main functionality and architecture, then building incrementally, constantly testing and refining each part before scaling up. I emphasize code readability and maintainability through comments and modular functions.

### What new development strategies did you use while working on your 3D scene?
I adopted a shader-based rendering approach using GLSL to control lighting and materials at the pixel level. I also used GLM to handle vector and matrix operations efficiently. Encapsulating rendering logic into functions like `RenderDrone()` streamlined my workflow and allowed for better experimentation and debugging.

### How did iteration factor into your development?
Iteration was crucial. I often prototyped features like camera control, lighting, and texturing in isolation before integrating them. When issues arose—like lighting not behaving as expected—I iterated by adjusting material properties, tweaking shader parameters, and reevaluating my coordinate transformations.

### How has your approach to developing code evolved throughout the milestones, which led you to the project’s completion?
Initially, I wrote code more reactively, solving one problem at a time. As I progressed, I learned to anticipate architectural needs. By milestone three, I began planning interfaces between components in advance and testing in layers. This evolution helped me deliver a polished, functional final product with clear structure and consistent interaction.

### How can computer science help me in reaching my goals?
Computer science is a foundation for innovation and problem-solving in any tech-driven field. For me, it's the core of my career as a future full-stack AIoT engineer. It equips me with the tools to develop intelligent, connected systems that have real-world impact—whether in automation, sustainability, or human-computer interaction.

### How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future educational pathway?
Learning computational graphics has deepened my understanding of how data can be transformed into immersive experiences. This knowledge is transferable to fields like game development, UI/UX design, and AR/VR systems. It also supports future study in 3D simulation, digital twins, and advanced human-machine interfaces.

### How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future professional pathway?
In the professional world, especially in AIoT, visualization is essential for interpreting sensor data and creating interactive dashboards. This project has helped me understand the pipeline from mathematical representation to visual output. Skills in OpenGL, scene management, and rendering logic are directly applicable in fields like robotics, simulation, and edge computing interfaces.

---

**Created by Gonzalo Patino – 2025**  
Bachelor of Science in Computer Science – Software Engineering Minor  
Southern New Hampshire University  
