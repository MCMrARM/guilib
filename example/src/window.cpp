#include <iostream>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <guilib/container.h>
#include <guilib/text.h>
#include <guilib/font.h>
#include "ascii_pixel_font.h"

using namespace guilib;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

std::shared_ptr<GuiContainer> createTestUi(float w, float h) {
    std::shared_ptr<GuiContainer> container(new GuiContainer(std::weak_ptr<GuiElement>()));
    container->style().width(w);
    container->style().height(h);
    container->style().paddingLeft(5.f);
    container->style().paddingRight(50.f);
    container->style().paddingTop(5.f);
    std::shared_ptr<GuiText> text(new GuiText(container, "This is a really long test text. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin nibh augue, suscipit a, scelerisque sed, lacinia in, mi. Cras vel lorem. Etiam pellentesque aliquet tellus. Phasellus pharetra nulla ac diam. Quisque semper justo at risus."));
    text->style().textColor(GuiColor(0xFFFF00));
    text->style().display(GuiDisplayMode::INLINE);
    text->style().paddingLeft(10.f);
    text->style().paddingRight(10.f);
    container->addElement(text);
    std::shared_ptr<GuiText> text2(new GuiText(container, "If this will work it'll be perfect."));
    text2->style().textColor(GuiColor(0x00FF00));
    text2->style().display(GuiDisplayMode::INLINE);
    container->addElement(text2);
    container->repositionElements();
    return std::move(container);
}

int main()
{
    int windowWidth = 800, windowHeight = 600;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Guilib", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Epoxy GL Version: " << epoxy_gl_version() << std::endl;

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, windowWidth, windowHeight);

    std::shared_ptr<GuiFont> font(new GuiPixelFont());
    GuiFont::registerFont(font);
    std::shared_ptr<GuiContainer> testUi (createTestUi(windowWidth / 3.f, windowHeight / 3.f));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        testUi->draw(0.f, 0.f);
        return 0;
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}