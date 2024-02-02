#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

ProgramState *programState;
const unsigned int SCR_WIDTH=800;
const unsigned int SCR_HEIGHT=600;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
void DrawImGui(ProgramState *programState);


int main() {
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
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable( GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/mlml.fs");
    Shader ninjaShader("resources/shaders/basicfrag.vs","resources/shaders/mlml.fs");
    Shader lightbulbShader("resources/shaders/2.model_lighting.vs","resources/shaders/basicfrag.fs");

    // load models

    // -----------
    Model ourModel("resources/objects/car/car.obj");
    ourModel.SetShaderTextureNamePrefix("material.");

    Model ninjaModel("resources/objects/naruto/naruto.obj");
    ninjaModel.SetShaderTextureNamePrefix("material.");

    Model lightbulbModel("resources/objects/lightbulb/lightbulb.obj");


    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;
    // render loop
    // -----------
    const int ninjaCount=10;
    const float angle=glm::radians(360.0f/ninjaCount);
    glm::mat4*modelMatrices;
    modelMatrices=new glm::mat4[ninjaCount];

    for(int i=0;i<ninjaCount;i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,5.0f*glm::vec3(glm::cos(i*angle),glm::sin(i*angle),0));
        model=glm::scale(model,glm::vec3(0.1f));
        modelMatrices[i]=(model);
        /*s.setMat4("model", model);
        ourModel.Draw(s);*/
    }

    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,ninjaCount*sizeof(glm::mat4 ),&modelMatrices[0],GL_STATIC_DRAW);
    for (unsigned int i = 0; i < ninjaModel.meshes.size(); i++)
    {
        unsigned int VAO = ninjaModel.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }








    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        pointLight.position = glm::vec3(4.0 * cos(currentFrame), 4.0f, 4.0 * sin(currentFrame));

        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms

        ninjaShader.use();
        ninjaShader.setInt("plCount",programState->pointlights.size());
        ninjaShader.setInt("slCount",programState->cameras.size());
        ninjaShader.setBool("hasTexture", false);
        ninjaShader.setProgramState(programState);

        for(unsigned int i = 0;i<ninjaModel.meshes.size();i++){
            ninjaShader.setVec3("mat.ambient",ninjaModel.meshes[i].mat.ambient);
            ninjaShader.setVec3("mat.diffuse",ninjaModel.meshes[i].mat.diffuse);
            ninjaShader.setVec3("mat.specular",ninjaModel.meshes[i].mat.specular);
            ninjaShader.setFloat("mat.shininess",ninjaModel.meshes[i].mat.shininess);
            glBindVertexArray(ninjaModel.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES,static_cast<unsigned int>(ninjaModel.meshes[i].indices.size()),GL_UNSIGNED_INT,0,ninjaCount);
            glBindVertexArray(0);
        }
        ourShader.use();
        ourShader.setInt("plCount",programState->pointlights.size());
        ourShader.setInt("slCount",programState->cameras.size());
        ;

        ourShader.setProgramState(programState);
        ourModel.Draw(ourShader);

        lightbulbShader.use();
        lightbulbShader.setFloat("alpha",glm::sin(glfwGetTime())/2+0.5);
        lightbulbShader.setProgramState(programState);

        map<float,int>indexMap;
        for(int i=0;i<programState->pointlights.size();i++){
            auto distance=glm::distance(programState->getCurrCamera().Position,programState->pointlights[i].position);
            indexMap[distance]=i;
        }

        for(auto it=indexMap.rbegin();it!=indexMap.rend();++it) {
            int i=it->second;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model,programState->pointlights[i].position);
            lightbulbShader.setMat4("model", model);
            lightbulbModel.Draw(lightbulbShader);
        }

        if (programState->ImGuiEnabled)
            DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->getCurrCamera().ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->getCurrCamera().ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->getCurrCamera().ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->getCurrCamera().ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->getCurrCamera().ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->getCurrCamera().ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("Backpack position", (float*)&programState->backpackPosition);
        ImGui::DragFloat("Backpack scale", &programState->backpackScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->getCurrCamera();
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

    }
    if(key==GLFW_KEY_T && action==GLFW_PRESS){
        programState->toggleCamera();
    }
    if(key==GLFW_KEY_F && action==GLFW_PRESS){
        programState->getCurrCamera().toggleSpotlightOn();
    }
}









