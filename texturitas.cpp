#include "glad.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glm/glm.hpp> //operaciones avanzadas
#include <glm/gtc/matrix_transform.hpp> //transformaciones matriciales
#include <glm/gtc/type_ptr.hpp> //transformaciones de tipos de datos

#include <shader/shader.h>
#include <camara/camara.h>

#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //almacenar nuestra ventana y configs
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window); //acciones para nuestra ventana
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

//medidas de la pantalla
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

//camara
Camara camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

//timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    //inicializar glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //creamos nuestra ventana
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rancho", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Fallo en gcrear GLFW y la ventana date un balazo" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //decirle a glfw que va a recibir señales de mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //comprobar que glad se este corriendo o se haya inicializado correctamente
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "no se esta ejecutando el alegre" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); //comprobacion del buffer z

    Shader nuestroShader("texturas.vs", "texturas.fs");
    Shader segundoShader("vertex2.vs", "vertex2.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    float piso[]{
         -10.5f, -0.5f, -10.5f,  0.0f, 1.0f,//bottom
          10.5f, -0.5f, -10.5f,  1.0f, 1.0f,
          10.5f, -0.5f,  10.5f,  1.0f, 0.0f,
          10.5f, -0.5f,  10.5f,  1.0f, 0.0f,
         -10.5f, -0.5f,  10.5f,  0.0f, 0.0f,
         -10.5f, -0.5f, -10.5f,  0.0f, 1.0f,
    };

    float base[]{
        //posiciones     //colores     //texturas
     -0.8f, -0.5f, -1.5f,  0.0f, 0.0f, // back
      0.8f, -0.5f, -1.5f,  1.0f, 0.0f,
      0.8f,  0.5f, -1.5f,  1.0f, 1.0f,
      0.8f,  0.5f, -1.5f,  1.0f, 1.0f,
     -0.8f,  0.5f, -1.5f,  0.0f, 1.0f,
     -0.8f, -0.5f, -1.5f,  0.0f, 0.0f,

     -0.8f, -0.5f, 1.5f,  0.0f, 0.0f, //front
      0.8f, -0.5f, 1.5f,  1.0f, 0.0f,
      0.8f,  0.5f, 1.5f,  1.0f, 1.0f,
      0.8f,  0.5f, 1.5f,  1.0f, 1.0f,
     -0.8f,  0.5f, 1.5f,  0.0f, 1.0f,
     -0.8f, -0.5f, 1.5f,  0.0f, 0.0f,

     -0.8f,  0.5f,  1.5f,  1.0f, 0.0f,//left
     -0.8f,  0.5f, -1.5f,  1.0f, 1.0f,
     -0.8f, -0.5f, -1.5f,  0.0f, 1.0f,
     -0.8f, -0.5f, -1.5f,  0.0f, 1.0f,
     -0.8f, -0.5f,  1.5f,  0.0f, 0.0f,
     -0.8f,  0.5f,  1.5f,  1.0f, 0.0f,

      0.8f,  0.5f,  1.5f,  1.0f, 0.0f,//right
      0.8f,  0.5f, -1.5f,  1.0f, 1.0f,
      0.8f, -0.5f, -1.5f,  0.0f, 1.0f,
      0.8f, -0.5f, -1.5f,  0.0f, 1.0f,
      0.8f, -0.5f,  1.5f,  0.0f, 0.0f,
      0.8f,  0.5f,  1.5f,  1.0f, 0.0f,

     -0.8f, -0.5f, -1.5f,  0.0f, 1.0f,//bottom
      0.8f, -0.5f, -1.5f,  1.0f, 1.0f,
      0.8f, -0.5f,  1.5f,  1.0f, 0.0f,
      0.8f, -0.5f,  1.5f,  1.0f, 0.0f,
     -0.8f, -0.5f,  1.5f,  0.0f, 0.0f,
     -0.8f, -0.5f, -1.5f,  0.0f, 1.0f,

     -0.8f,  0.5f, -1.5f,  0.0f, 1.0f,//top
      0.8f,  0.5f, -1.5f,  1.0f, 1.0f,
      0.8f,  0.5f,  1.5f,  1.0f, 0.0f,
      0.8f,  0.5f,  1.5f,  1.0f, 0.0f,
     -0.8f,  0.5f,  1.5f,  0.0f, 0.0f,
     -0.8f,  0.5f, -1.5f,  0.0f, 1.0f
    };

    float techorancho[] = {
     -0.9f, -0.5f, -1.5f,  0.0f, 0.0f, // back
      0.9f, -0.5f, -1.5f,  1.0f, 0.0f,
      0.5f,  0.0f, -1.5f,  1.0f, 1.0f,
      0.5f,  0.0f, -1.5f,  1.0f, 1.0f,
     -0.5f,  0.0f, -1.5f,  0.0f, 1.0f,
     -0.9f, -0.5f, -1.5f,  0.0f, 0.0f,//

     -0.9f, -0.5f, 1.5f,  0.0f, 0.0f, //front
      0.9f, -0.5f, 1.5f,  1.0f, 0.0f,
      0.5f,  0.0f, 1.5f,  1.0f, 1.0f,
      0.5f,  0.0f, 1.5f,  1.0f, 1.0f,
     -0.5f,  0.0f, 1.5f,  0.0f, 1.0f,
     -0.9f, -0.5f, 1.5f,  0.0f, 0.0f,//

     -0.5f,  0.0f,  1.5f,  1.0f, 0.0f,//left
     -0.5f,  0.0f, -1.5f,  1.0f, 1.0f,
     -0.9f, -0.5f, -1.5f,  0.0f, 1.0f,
     -0.9f, -0.5f, -1.5f,  0.0f, 1.0f,
     -0.9f, -0.5f,  1.5f,  0.0f, 0.0f,
     -0.5f,  0.0f,  1.5f,  1.0f, 0.0f,//

      0.5f,  0.0f,  1.5f,  1.0f, 0.0f,//right
      0.5f,  0.0f, -1.5f,  1.0f, 1.0f,
      0.9f, -0.5f, -1.5f,  0.0f, 1.0f,
      0.9f, -0.5f, -1.5f,  0.0f, 1.0f,
      0.9f, -0.5f,  1.5f,  0.0f, 0.0f,
      0.5f,  0.0f,  1.5f,  1.0f, 0.0f,//

     -0.9f, -0.5f, -1.5f,  0.0f, 1.0f,//bottom
      0.9f, -0.5f, -1.5f,  1.0f, 1.0f,
      0.9f, -0.5f,  1.5f,  1.0f, 0.0f,
      0.9f, -0.5f,  1.5f,  1.0f, 0.0f,
     -0.9f, -0.5f,  1.5f,  0.0f, 0.0f,
     -0.9f, -0.5f, -1.5f,  0.0f, 1.0f,//
     -0.5f,  0.0f, -1.5f,  0.0f, 1.0f,//top
      0.5f,  0.0f, -1.5f,  1.0f, 1.0f,
      0.5f,  0.0f,  1.5f,  1.0f, 0.0f,
      0.5f,  0.0f,  1.5f,  1.0f, 0.0f,
     -0.5f,  0.0f,  1.5f,  0.0f, 0.0f,
     -0.5f,  0.0f, -1.5f,  0.0f, 1.0f//
    };

    float silobase[] = {
        //posiciones     //colores     //texturas
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // back
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.4f,  2.0f, -0.4f,  1.0f, 1.0f,
       0.4f,  2.0f, -0.4f,  1.0f, 1.0f,
      -0.4f,  2.0f, -0.4f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //front
       0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
       0.4f,  2.0f, 0.4f,  1.0f, 1.0f,
       0.4f,  2.0f, 0.4f,  1.0f, 1.0f,
      -0.4f,  2.0f, 0.4f,  0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.4f,  2.0f,  0.4f,  1.0f, 0.0f,//left
      -0.4f,  2.0f, -0.4f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.4f,  2.0f,  0.4f,  1.0f, 0.0f,
       0.4f,  2.0f,  0.4f,  1.0f, 0.0f,//right
       0.4f,  2.0f, -0.4f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.4f,  2.0f,  0.4f,  1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,//bottom
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.4f,  2.0f, -0.4f,  0.0f, 1.0f,//top
       0.4f,  2.0f, -0.4f,  1.0f, 1.0f,
       0.4f,  2.0f,  0.4f,  1.0f, 0.0f,
       0.4f,  2.0f,  0.4f,  1.0f, 0.0f,
      -0.4f,  2.0f,  0.4f,  0.0f, 0.0f,
      -0.4f,  2.0f, -0.4f,  0.0f, 1.0f
    };



    float barnDoor[] = {
       -0.6f, -0.5f, 1.5f,  0.0f, 0.0f, //front
        0.6f, -0.5f, 1.5f,  1.0f, 0.0f,
        0.6f,  0.3f, 1.5f,  1.0f, 1.0f,
        0.6f,  0.3f, 1.5f,  1.0f, 1.0f,
       -0.6f,  0.3f, 1.5f,  0.0f, 1.0f,
       -0.6f, -0.5f, 1.5f,  0.0f, 0.0f,
    };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    unsigned int indicespiso[]
    {
        0, 1, 2,
        3, 4, 5
    };

    unsigned int indicesdoor[]
    {
        0, 1, 2,
        3, 4, 5
    };

    unsigned int indices[]
    {
       0, 1, 2,
      3, 4, 5,
      6, 7, 8,
      9, 10, 11,
      12, 13, 14,
      14, 16, 17,
      18, 19, 20,
      21, 22, 23,
      24, 25, 26,
      27, 28, 29,
      30, 31, 32,
      33, 34, 35
    };

    unsigned int indicestechorancho[]
    {
       0, 1, 2,
      3, 4, 5,
      6, 7, 8,
      9, 10, 11,
      12, 13, 14,
      14, 16, 17,
      18, 19, 20,
      21, 22, 23,
      24, 25, 26,
      27, 28, 29,
      30, 31, 32,
      33, 34, 35
    };

    unsigned int indicessilobase[]
    {
       0, 1, 2,
      3, 4, 5,
      6, 7, 8,
      9, 10, 11,
      12, 13, 14,
      14, 16, 17,
      18, 19, 20,
      21, 22, 23,
      24, 25, 26,
      27, 28, 29,
      30, 31, 32,
      33, 34, 35
    };

    glm::vec3 posicionesPiso[] = {
     glm::vec3(0.0f,  0.0f,  0.0f),
    };

    glm::vec3 posicionesCubo[] = {
      glm::vec3(0.0f,  0.0f,  0.0f),
    };

    glm::vec3 posicionesBaseRancho[] = {
      glm::vec3(0.0f,  0.0f,  0.0f),
    };

    glm::vec3 posicionesTechoRancho[] = {
      glm::vec3(0.0f,  1.0f,  0.0f),
    };

    glm::vec3 posicionesSiloBase[] = {
      glm::vec3(2.0f,  0.0f,  0.0f),
    };


    glm::vec3 posicionesDoor[] = {
     glm::vec3(0.0f,  0.0f,  0.01f),
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        ("skybox/right.jpg"),
        ("skybox/left.jpg"),
        ("skybox/top.jpg"),
        ("skybox/bottom.jpg"),
        ("skybox/front.jpg"),
        ("skybox/back.jpg")
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    //configuracion del shader
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    //---------------piso vertex-----------------------

    unsigned int VBOpiso, VAOpiso, EBOpiso; //Vertex Buffer Object, Vertex Array Object y Extendet Array Object
    glGenVertexArrays(1, &VAOpiso);
    glGenBuffers(1, &VBOpiso);
    glGenBuffers(1, &EBOpiso);
    //unir o linkear
    glBindVertexArray(VAOpiso);

    glBindBuffer(GL_ARRAY_BUFFER, VBOpiso);
    glBufferData(GL_ARRAY_BUFFER, sizeof(piso), piso, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOpiso);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicespiso), indicespiso, GL_STATIC_DRAW);
    //posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //---------------primer vertex-----------------------

    unsigned int VBO, VAO, EBO; //Vertex Buffer Object, Vertex Array Object y Extendet Array Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    //unir o linkear
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(base), base, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //--------------- segundo vertex------------
    unsigned int VBOtecho, VAOtecho, EBOtecho; //Vertex Buffer Object, Vertex Array Object y Extendet Array Object
    glGenVertexArrays(1, &VAOtecho);
    glGenBuffers(1, &VBOtecho);
    glGenBuffers(1, &EBOtecho);
    //unir o linkear
    glBindVertexArray(VAOtecho);

    glBindBuffer(GL_ARRAY_BUFFER, VBOtecho);
    glBufferData(GL_ARRAY_BUFFER, sizeof(techorancho), techorancho, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtecho);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicestechorancho), indicestechorancho, GL_STATIC_DRAW);
    //posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //--------------- silo base vertex------------
    unsigned int VBOsiloBase, VAOsiloBase, EBOsiloBase; //Vertex Buffer Object, Vertex Array Object y Extendet Array Object
    glGenVertexArrays(1, &VAOsiloBase);
    glGenBuffers(1, &VBOsiloBase);
    glGenBuffers(1, &EBOsiloBase);
    //unir o linkear
    glBindVertexArray(VAOsiloBase);

    glBindBuffer(GL_ARRAY_BUFFER, VBOsiloBase);
    glBufferData(GL_ARRAY_BUFFER, sizeof(silobase), silobase, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsiloBase);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicessilobase), indicessilobase, GL_STATIC_DRAW);
    //posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //--------------- silo base vertex------------
    unsigned int VBOdoor, VAOdoor, EBOdoor; //Vertex Buffer Object, Vertex Array Object y Extendet Array Object
    glGenVertexArrays(1, &VAOdoor);
    glGenBuffers(1, &VBOdoor);
    glGenBuffers(1, &EBOdoor);
    //unir o linkear
    glBindVertexArray(VAOdoor);

    glBindBuffer(GL_ARRAY_BUFFER, VBOdoor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(barnDoor), barnDoor, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOdoor);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicespiso), indicespiso, GL_STATIC_DRAW);
    //posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    ////////////////////////////////////
    //crear y cargar nuestras texturas//
    ////////////////////////////////////
    unsigned int texturabase, texturatecho, texturasilo, texturatechosilo, texturapiso, texturadoor;
    //primera textura
    glGenTextures(1, &texturabase);
    glBindTexture(GL_TEXTURE_2D, texturabase);
    //configurar el comportamiento de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //configuramos el filtrado de la textura en caso que se expanda
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //cargar la imagen, crear la textura y generar los mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    //cargar nuestra textura
    unsigned char* data = stbi_load("main.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "fallaste en cargar la primera textura" << std::endl;
    }
    stbi_image_free(data);

    //Textura2
    glGenTextures(1, &texturatecho);
    glBindTexture(GL_TEXTURE_2D, texturatecho);
    //configurar el comportamiento de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //configuramos el filtrado de la textura en caso que se expanda
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //cargar nuestra textura
    data = stbi_load("techo.jfif", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "fallaste en cargar la textura de techo" << std::endl;
    }
    stbi_image_free(data);

    //TexturaPiso
    glGenTextures(1, &texturapiso);
    glBindTexture(GL_TEXTURE_2D, texturapiso);
    //configurar el comportamiento de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //configuramos el filtrado de la textura en caso que se expanda
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //cargar nuestra textura
    data = stbi_load("grass.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "fallaste en cargar la textura de piso" << std::endl;
    }
    stbi_image_free(data);

    //TexturaSiloBase
    glGenTextures(1, &texturasilo);
    glBindTexture(GL_TEXTURE_2D, texturasilo);
    //configurar el comportamiento de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //configuramos el filtrado de la textura en caso que se expanda
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //cargar nuestra textura
    data = stbi_load("brick.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "fallaste en cargar la textura de base de silo" << std::endl;
    }
    stbi_image_free(data);

    //TexturaPuerta
    glGenTextures(1, &texturadoor);
    glBindTexture(GL_TEXTURE_2D, texturadoor);
    //configurar el comportamiento de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //configuramos el filtrado de la textura en caso que se expanda
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //cargar nuestra textura
    data = stbi_load("barn-door.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "fallaste en cargar la textura de base de silo" << std::endl;
    }
    stbi_image_free(data);

    nuestroShader.use();
    nuestroShader.setInt("texturabase", 0);
    nuestroShader.setInt("texturatecho", 1);
    nuestroShader.setInt("texturapiso", 2);
    nuestroShader.setInt("texturasilo", 3);
    nuestroShader.setInt("texturadoor", 4);

    //loop para que se pueda visualizar nuestra pantalla
    while (!glfwWindowShouldClose(window))
    {
        //calculo para el mouse para que pueda captar los movimeintos en tiempo real
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        //Renderizado
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpieza del buffer z

        //activado  y union de las texturas
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturabase);

        nuestroShader.use();

        //mvp
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        nuestroShader.setMat4("projection", projection);
        nuestroShader.setMat4("view", view);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, posicionesCubo[i]);
            nuestroShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturatecho);

        glBindVertexArray(VAOtecho);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, posicionesTechoRancho[i]);
            nuestroShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturapiso);

        glBindVertexArray(VAOpiso);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, posicionesPiso[i]);
            nuestroShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturasilo);

        glBindVertexArray(VAOsiloBase);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, posicionesSiloBase[i]);
            nuestroShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturadoor);

        glBindVertexArray(VAOdoor);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, posicionesDoor[i]);
            nuestroShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);
        }

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default



        //detecte eventos de IO
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //nuestra primera configuracion de la camara
    glViewport(0, 0, width, height);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}