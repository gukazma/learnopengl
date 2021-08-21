#include "Engine.h"

// 13l_1 多光源实验1
namespace glx0131
{
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    void processInput(GLFWwindow *window);

    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间

    bool firstMouse = false;
    float lastX = 400, lastY = 300;

    bool isMouseEnalbe = true;

    Camera mycamera(glm::vec3(0.5f, 2.0f, 12.5f), glm::vec3(0.0f, 1.0f, 0.0f), 269, -5.2f);

    struct Material
    {
        glm::vec3 specular;
        float shininess;
    };

    // 点光源
    struct PointLight
    {
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
    struct DirLight
    {
        glm::vec3 direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
    struct SpotLight
    {
        glm::vec3 position;
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

    int main()
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        ImFont *font = io.Fonts->AddFontFromFileTTF("./fonts/楷体_GB2312.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
        // io.Fonts->GetGlyphRangesChineseFull();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

        // obj VAO init
        unsigned int objVBO, objVAO;
        glGenVertexArrays(1, &objVAO);
        glGenBuffers(1, &objVBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(objVAO);

        glBindBuffer(GL_ARRAY_BUFFER, objVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // You can unbind the objVAO afterwards so other objVAO calls won't accidentally modify this objVAO, but this rarely happens. Modifying other
        // objVAOs requires a call to glBindVertexArray anyways so we generally don't unbind objVAOs (nor objVBOs) when it's not directly necessary.
        glBindVertexArray(0);
        // note that this is allowed, the call to glVertexAttribPointer registered objVBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // light VAO init
        unsigned int lightVBO, lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor lightVBOs) when it's not directly necessary.
        glBindVertexArray(0);
        // note that this is allowed, the call to glVertexAttribPointer registered lightVBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        stbi_set_flip_vertically_on_load(true);
        unsigned int texture1;
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载纹理图片
        int width, height, nrChannels;
        unsigned char *data = stbi_load("./imgs/container2.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture1" << std::endl;
        }
        stbi_image_free(data);

        unsigned int texture2;
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        data = stbi_load("./imgs/container2_specular.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture1" << std::endl;
        }
        stbi_image_free(data);

        Shader objshader("./13/l1/ObjectVertex.vert", "./13/l1/ObjectFragment.frag");
        Shader lightshader("./13/l1/LightVertex.vert", "./13/l1/LightFragment.frag");

        objshader.use();
        objshader.setInt("material.diffuse", 0);
        objshader.setInt("material.specular", 1);
        glm::mat4 view = glm::mat4(1), model = glm::mat4(1), projection = glm::mat4(1);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

        glEnable(GL_DEPTH_TEST);

        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)};
        glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)};

        // render loop
        // -----------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glm::vec3 objectColor(0.0f, 0.5f, 0.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        float ambientStrength = 0.1;
        float diffuseStrength = 0.5;
        float specularStrength = 1.0;

        Material material;
        material.specular = glm::vec3(0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        material.shininess = 32.0f;
        PointLight pointLights[4];
        for (int i = 0; i < 4; i++)
        {
            pointLights[i].ambient = glm::vec3(ambientStrength);
            pointLights[i].diffuse = glm::vec3(diffuseStrength);
            pointLights[i].specular = glm::vec3(specularStrength);
            pointLights[i].constant = 1.0f;
            pointLights[i].linear = 0.09f;
            pointLights[i].quadratic = 0.032f;
            pointLights[i].position = pointLightPositions[i];
            objshader.use();
            char s[50];
            sprintf(s, "pointLights[%d].position", i);
            objshader.setVec3(s, pointLights[i].position);
            sprintf(s, "pointLights[%d].ambient", i);
            objshader.setVec3(s, pointLights[i].ambient);
            sprintf(s, "pointLights[%d].diffuse", i);
            objshader.setVec3(s, pointLights[i].diffuse);
            sprintf(s, "pointLights[%d].specular", i);
            objshader.setVec3(s, pointLights[i].specular);
            sprintf(s, "pointLights[%d].constant", i);
            objshader.setFloat(s, pointLights[i].constant);
            sprintf(s, "pointLights[%d].linear", i);
            objshader.setFloat(s, pointLights[i].linear);
            sprintf(s, "pointLights[%d].quadratic", i);
            objshader.setFloat(s, pointLights[i].quadratic);
        }

        DirLight dirLight;
        dirLight.ambient = glm::vec3(ambientStrength);
        dirLight.diffuse = glm::vec3(diffuseStrength);
        dirLight.specular = glm::vec3(specularStrength);
        dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        objshader.setVec3("dirLight.ambient", dirLight.ambient);
        objshader.setVec3("dirLight.diffuse", dirLight.diffuse);
        objshader.setVec3("dirLight.specular", dirLight.specular);
        objshader.setVec3("dirLight.direction", dirLight.direction);

        // 聚光
        SpotLight spotLight;
        spotLight.ambient = glm::vec3(ambientStrength);
        spotLight.diffuse = glm::vec3(diffuseStrength);
        spotLight.specular = glm::vec3(specularStrength);
        spotLight.direction = mycamera.Front;
        spotLight.position = mycamera.Position;
        spotLight.constant = 1.0f;
        spotLight.linear = 0.09f;
        spotLight.quadratic = 0.032f;
        spotLight.cutOff = cos(glm::radians(12.5));
        spotLight.outerCutOff = cos(glm::radians(17.5));

        // 聚光设置uniform变量
        objshader.setVec3("spotLight.ambient", spotLight.ambient);
        objshader.setVec3("spotLight.diffuse", spotLight.diffuse);
        objshader.setVec3("spotLight.specular", spotLight.specular);
        objshader.setFloat("spotLight.constant", spotLight.constant);
        objshader.setFloat("spotLight.linear", spotLight.linear);
        objshader.setFloat("spotLight.quadratic", spotLight.quadratic);
        objshader.setFloat("spotLight.cutOff", spotLight.cutOff);
        objshader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);

        while (!glfwWindowShouldClose(window))
        {
            // light move
            // input
            // -----
            processInput(window);

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glBindVertexArray(objVAO);
            // 受光物体渲染
            objshader.use();
            view = mycamera.GetViewMatrix();
            objshader.setMat4("view", view);
            projection = glm::perspective(mycamera.Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
            objshader.setMat4("projection", projection);

            objshader.setVec3("viewPos", mycamera.Position);
            model = glm::mat4(1);
            objshader.setMat4("model", model);

            // material properties
            objshader.setVec3("material.specular", material.specular); // specular lighting doesn't have full effect on this object's material
            objshader.setFloat("material.shininess", material.shininess);


            // 聚光改变量
            spotLight.position = mycamera.Position;
            spotLight.direction = mycamera.Front;
            objshader.setVec3("spotLight.position", spotLight.position);
            objshader.setVec3("spotLight.direction", spotLight.direction);
            for (int i = 0; i < 10; i++)
            {
                model = glm::mat4(1);
                model = glm::translate(model, cubePositions[i]);

                objshader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // 发光立方体渲染
            glBindVertexArray(lightVAO);
            lightshader.use();
            lightshader.setMat4("view", view);
            lightshader.setMat4("projection", projection);
            lightshader.setVec3("lightColor", lightColor);
            for (int i = 0; i < 4; i++)
            {
                model = glm::mat4(1);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                lightshader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // imgui
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("光照模型测试窗口");
                ImGui::SetWindowSize(ImVec2(300, 500));
                ImGui::SetWindowPos(ImVec2(0, 0));
                ImGui::ColorEdit3("灯光颜色", (float *)&lightColor);
                ImGui::TextWrapped("相机坐标 = (%f, %f, %f)", mycamera.Position.x, mycamera.Position.y, mycamera.Position.z);
                ImGui::TextWrapped("相机yaw角 = %f", mycamera.Yaw);
                ImGui::TextWrapped("相机pitch角 = %f", mycamera.Pitch);
                // ImGui::InputFloat3("material.ambient",(float*)&(material.ambient));
                // ImGui::InputFloat3("material.diffuse",(float*)&(material.diffuse));
                ImGui::InputFloat3("material.specular", (float *)&(material.specular));
                ImGui::InputFloat("material.shininess", &(material.shininess));
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &objVAO);
        glDeleteVertexArrays(1, &lightVAO);
        glDeleteBuffers(1, &objVBO);
        glDeleteBuffers(1, &lightVBO);

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
        return 0;
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mycamera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mycamera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mycamera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mycamera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isMouseEnalbe = false;
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isMouseEnalbe = true;
        }
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
    void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    {
        if (isMouseEnalbe)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            mycamera.ProcessMouseMovement(xoffset, yoffset);
        }
    }
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        mycamera.ProcessMouseScroll(yoffset);
    }
}
