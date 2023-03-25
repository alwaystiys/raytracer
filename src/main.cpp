
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

//#include<color.h>
#include <rtweekend.h>
#include <hittable_list.h>
#include <sphere.h>
#include <camera.h>
#include <iostream>

int inputSize[2]{ 1000, 1000 };
ImVec2 imageSize(1000, 1000);
uint8_t* pixels = nullptr;
int samples_per_pixel = 1;
int max_depth = 50;

void updateImageSize(int width, int height) {
    imageSize.x = width;
    imageSize.y = height;

    if (pixels != nullptr)
        delete[] pixels;

    pixels = new uint8_t[imageSize.x * imageSize.y * 4];
}

void fillPixels(color& pixel_color, int index, bool is_sample) {

    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    if (is_sample) {
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;
    }

    pixels[index * 4] = static_cast<int>(255.999 * r);
    pixels[index * 4 + 1] = static_cast<int>(255.999 * g);
    pixels[index * 4 + 2] = static_cast<int>(255.999 * b);
    pixels[index * 4 + 3] = 255;
}

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

void outputRaytracingSimpleImage() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 1000;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera and Viewport

    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;

    // the distance between camera and viewports
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {
            auto u = double(i) / (imageSize.x - 1);
            auto v = double(j) / (imageSize.y - 1);

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            ray r = ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);

            vec3 unit_direction = unit_vector(r.direction());
            auto t = 0.5 * (unit_direction.y() + 1.0);
            color c = (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);

            int ir = static_cast<int>(255.999 * c.x());
            int ig = static_cast<int>(255.999 * c.y());
            int ib = static_cast<int>(255.999 * c.z());

            int index = i + j * imageSize.y;
            pixels[index * 4] = ir;
            pixels[index * 4 + 1] = ig;
            pixels[index * 4 + 2] = ib;
            pixels[index * 4 + 3] = 255;
        }
    }
}

bool hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
     
    auto a = dot(r.direction(), r.direction());
    auto b = 2 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;

    auto discriminant = b * b - 4 * a * c;
    return discriminant > 0;
}


void outputRaySphere() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 1000;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera and Viewport

    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;

    // the distance between camera and viewports
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {
            auto u = double(i) / (imageSize.x - 1);
            auto v = double(j) / (imageSize.y - 1);

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            ray r = ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
                
            // calc hit sphere
            color c;
            if (hit_sphere(point3(0, 0, -2), 0.5, r)) {
                c = color(1.0, 0.0, 0.0);
            }
            else {
                vec3 unit_direction = unit_vector(r.direction());
                auto t = 0.5 * (unit_direction.y() + 1.0);
                c = (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
            }

            int ir = static_cast<int>(255.999 * c.x());
            int ig = static_cast<int>(255.999 * c.y());
            int ib = static_cast<int>(255.999 * c.z());

            int index = i + j * imageSize.y;
            pixels[index * 4] = ir;
            pixels[index * 4 + 1] = ig;
            pixels[index * 4 + 2] = ib;
            pixels[index * 4 + 3] = 255;
        }
    }
}

/*
    COLOR NORMAL
    Updataed 2023.03.05
*/
double hit_sphere_ver2(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;

    auto a = dot(r.direction(), r.direction());
    auto b = 2 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;

    auto discriminant = b * b - 4 * a * c;

    if (discriminant < 0.0) {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
    return discriminant > 0;
}

double hit_sphere_simple_ver2(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;

    auto a = dot(r.direction(), r.direction());
    //auto b = 2 * dot(oc, r.direction());
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0.0) {
        return -1.0;
    }
    else {
        return (-half_b - sqrt(discriminant)) / (2.0 * a);
    }
    return discriminant > 0;
}


void outputRayColorNormalSphere() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 1000;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera and Viewport

    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;

    // the distance between camera and viewports
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {
            auto u = double(i) / (imageSize.x - 1);
            auto v = double(j) / (imageSize.y - 1);

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            ray r = ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);

            // calc hit sphere
            color c;
            auto t = hit_sphere_simple_ver2(point3(0, 0, -2), 0.5, r);
            if (t > 0.0) {
                c = color(1.0, 0.0, 0.0);
                vec3 normal = unit_vector(r.at(t) - point3(0, 0, -2));
                c = 0.5 * color(normal.x() + 1.0, normal.y() + 1.0, normal.z() + 1.0);
            }
            else {
                vec3 unit_direction = unit_vector(r.direction());
                auto t = 0.5 * (unit_direction.y() + 1.0);
                c = (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
            }

            int ir = static_cast<int>(255.999 * c.x());
            int ig = static_cast<int>(255.999 * c.y());
            int ib = static_cast<int>(255.999 * c.z());

            int index = i + j * imageSize.y;
            pixels[index * 4] = ir;
            pixels[index * 4 + 1] = ig;
            pixels[index * 4 + 2] = ib;
            pixels[index * 4 + 3] = 255;
        }
    }
}


void outputRayColorNormalMultSphere() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 800;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));


    // Camera and Viewport
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;

    // the distance between camera and viewports
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {
            auto u = double(i) / (imageSize.x - 1);
            auto v = double(j) / (imageSize.y - 1);

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            ray r = ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);

            // calc hit sphere
            color c;

            hit_record rec;
            if (world.hit(r, 0, infinity, rec)) {
                c =  0.5 * (rec.normal + color(1, 1, 1));
            }
            else {
                vec3 unit_direction = unit_vector(r.direction());
                auto t = 0.5 * (unit_direction.y() + 1.0);
                c = (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
            }
            int index = i + j * imageSize.y;
            fillPixels(c, index, false);
        }
    }
}


void outputRayColorMultiSample() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 800;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera and Viewport
    camera cam;

    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            // calc hit sphere
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double2()) / (imageSize.x - 1);
                auto v = (j + random_double2()) / (imageSize.y - 1);
                ray r = cam.get_ray(u, v);

                hit_record rec;
                color c;
                if (world.hit(r, 0, infinity, rec)) {
                    c = 0.5 * (rec.normal + color(1, 1, 1));
                }
                else {
                    vec3 unit_direction = unit_vector(r.direction());
                    auto t = 0.5 * (unit_direction.y() + 1.0);
                    c = (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
                }
                pixel_color += c;
            }

            int index = i + j * imageSize.y;
            fillPixels(pixel_color, index, true);
        }
    }
}

color ray_color(const ray& r, const hittable& world, int depth) {

    if (depth <= 0) {
        return color(0, 0, 0);
    }

    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        // random point 
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    }

}


void outPutRayColorRandomNormalSphere() {
    // Canvas
    const auto aspect_ratio = 16 / 9;
    int image_width = 800;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera and Viewport
    camera cam;

    updateImageSize(image_width, image_height);

    for (int i = 0; i < imageSize.x; i++) {
        for (int j = 0; j < imageSize.y; j++) {

            // lower_left_corner + u * horizontal + v * vertical 视口上的点
            // lower_left_corner + u * horizontal + v * vertical - origin 由原点指向视口上的点的向量
            // calc hit sphere
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double2()) / (imageSize.x - 1);
                auto v = (j + random_double2()) / (imageSize.y - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }

            int index = i + j * imageSize.y;
            fillPixels(pixel_color, index, true);
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

    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.2f, 0.25f, 0.40f, 1.00f);

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

    const char* items[] = { 
        "SimpleImage", 
        "RaySimpleImage", 
        "RaySphere", 
        "RayColorNormalSphere", 
        "RayColorNormalMultSphere",
        "RayColorMultiSample", 
        "RayColorRandomNormalSphere", 
        "Watermelon" 
    };
    static int item_current = 0;

    while (!glfwWindowShouldClose(window))
    {
        /* Swap front and back buffers */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window// (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);
        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Begin("Control Panel");                        

        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::InputInt2("image size", inputSize);
        ImGui::ListBox("select image", &item_current, items, IM_ARRAYSIZE(items), 6);
        ImGui::Text("curr select = %d", item_current);

        ImGui::InputInt("sample", &samples_per_pixel);
        ImGui::InputInt("max_depth", &max_depth);
            
        ImGuiIO& io = ImGui::GetIO();
        float scale = 2.0f;
        io.FontGlobalScale = scale;
        //ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3, 2.0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        //ImGui::SetWindowFontScale(2.0f);
        if (ImGui::Button("Demo")) {
            show_demo_window = !show_demo_window;
        }


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
            case 1:
                outputRaytracingSimpleImage();
                break;
            case 2:
                outputRaySphere();
                break;
            case 3:
                outputRayColorNormalSphere();
                break;
            case 4:
                outputRayColorNormalMultSphere();
                break;
            case 5:
                outputRayColorMultiSample();
                break;
            case 6:
                outPutRayColorRandomNormalSphere();
            default:
                break;
            }
            showResult = true;
        }
        ImGui::End();

        if (showResult)
        {
            ImGui::SetNextWindowSize(imageSize + ImVec2(20, 35));
            ImGui::Begin("result", &showResult);

            glBindTexture(GL_TEXTURE_2D, renderTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

            // ImVec2(0, 1) 左下角
            // ImVec2(1, 0) 右上角
            ImGui::Image((ImTextureID)(intptr_t)renderTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
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
