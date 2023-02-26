
// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>

int inputSize[2]{ 1000, 1000 };
ImVec2 imageSize(1000, 1000);
uint8_t* pixels = nullptr;

void outputSimpleImage() {
    for (int i = 0; i < imageSize.x; i++)
    {
        for (int j = 0; j < imageSize.y; j++)
        {
            auto r = double(i) / (imageSize.x - 1);
            auto g = double(j) / (imageSize.y - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            int index = i + j * imageSize.y;
            pixels[index * 4] = ir;
            pixels[index * 4 + 1] = ig;
            pixels[index * 4 + 2] = ib;
            pixels[index * 4 + 3] = 255;
        }
    }

}

int main(void)
{
    // glfw: initialize and configure
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(1920, 1200, "RayTracer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImGui::CreateContext();     // Setup Dear ImGui context
    ImGui::StyleColorsDark();       // Setup Dear ImGui style
    ImGui_ImplGlfw_InitForOpenGL(window, true);     // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool showResult = false;

    GLuint renderTexture;
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

    const char* items[] = { "SimpleImage", "RaySimpleImage", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
    static int item_current = 0;

    while (!glfwWindowShouldClose(window))
    {
        /* Swap front and back buffers */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window// (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);
        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Begin("Control Panel");                        

        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::InputInt2("image size", inputSize);
        ImGui::ListBox("select image", &item_current, items, IM_ARRAYSIZE(items), 4);
        ImGui::Text("curr select = %d", item_current);

        if (ImGui::Button("Render")) {
            imageSize.x = inputSize[0];
            imageSize.y = inputSize[1];

            if (pixels != nullptr)
                delete[] pixels;

            pixels = new uint8_t[imageSize.x * imageSize.y * 4];

            switch (item_current)
            {
            case 0:
                outputSimpleImage();
                break;
            default:
                break;
            }

            //for (int i = 0; i < imageSize.x; i++)
            //{
            //    for (int j = 0; j < imageSize.y; j++)
            //    {
            //        auto r = double(i) / (imageSize.x - 1);
            //        auto g = double(j) / (imageSize.y - 1);
            //        auto b = 0.25;

            //        int ir = static_cast<int>(255.999 * r);
            //        int ig = static_cast<int>(255.999 * g);
            //        int ib = static_cast<int>(255.999 * b);

            //        int index = i + j * imageSize.y;
            //        pixels[index * 4] = ir;
            //        pixels[index * 4 + 1] = ig;
            //        pixels[index * 4 + 2] = ib;
            //        pixels[index * 4 + 3] = 255;
            //    }
            //}
            showResult = true;


        }
        ImGui::End();

        if (showResult)
        {
            ImGui::SetNextWindowSize(imageSize + ImVec2(20, 35));
            ImGui::Begin("result", &showResult);

            glBindTexture(GL_TEXTURE_2D, renderTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

            ImGui::Image((ImTextureID)(intptr_t)renderTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (pixels != nullptr)
        delete[] pixels;

    return 0;
}
