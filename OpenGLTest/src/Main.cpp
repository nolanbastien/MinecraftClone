#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) // npos is returned when not found
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }

    }

    return { ss[0].str(), ss[1].str() };
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) _malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        // std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}

int main(void)
{
    // Create window
    if (!glfwInit())
        return -1;

    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Link Graphic card functions to OpenGL API
    if (glewInit() != GLEW_OK) 
        std::cout << "Not ok!" << std::endl;

    // Print version
    std::cout << glGetString(GL_VERSION) << std::endl;

    // DATA
    float positions1[] = {
        // First triangle
        -0.5f, -0.5f, //Left, bottom
         0.5f, -0.5f, // Right, bottom
         0.5f,  0.5f // Right, top
    };

    float positions2[] = {
        // Second triangle
        -0.6f, -0.5f, //Left, bottom
        -0.6f,  0.5f, //Left, top
         0.4f,  0.5f  //Right, top
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    // TRIANGLE 1
    // Create VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create VBO
    unsigned int buffer1;
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions1, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // TRIANGLE 2
    //Create VAO
    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    unsigned int buffer2;
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions2, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // GET AND COMPILE SHADERS
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    ShaderProgramSource source2 = ParseShader("res/shaders/2ndTriangleShader.shader");
    unsigned int shader2 = CreateShader(source2.VertexSource, source2.FragmentSource);

    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering commands here
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(shader);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(shader2);

        // Swap Buffers
        glfwSwapBuffers(window); // As soon as the rendering commands are finished, the front and back buffers are swapped.

        glfwPollEvents(); // Checks if any events are triggered (like keyboard or mouse mouvement).
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}