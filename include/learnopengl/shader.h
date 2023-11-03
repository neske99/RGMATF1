#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <common.h>
#include<learnopengl/camera.h>
using namespace std;
struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct DirLight {
    glm::vec3 direction;

    glm:: vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};



struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    vector<Camera>cameras;
    int currCameraIndex;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 backpackPosition = glm::vec3(0.0f);
    float backpackScale = 1.0f;
    PointLight pointLight;
    DirLight dirlight;
    vector<PointLight>pointlights;
    SpotLight spotLight;
    unsigned int SCR_WIDTH=800;
    unsigned int SCR_HEIGHT=600;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {
        currCameraIndex=0;
        cameras.emplace_back(glm::vec3(0.0f,0.0f,3.0f));
        cameras.emplace_back(glm::vec3(1.0f,0.0f,3.0f));
        cameras.emplace_back(glm::vec3(2.0f,0.0f,3.0f));
        cameras.emplace_back(glm::vec3(-1.0f,0.0f,3.0f));

        dirlight.direction=glm::vec3( -0.2f, -1.0f, -0.3f);
        dirlight.ambient=glm::vec3( 0.05f, 0.05f, 0.05f);
        dirlight.diffuse=glm::vec3( 0.4f, 0.4f, 0.4f);
        dirlight.specular=glm::vec3( 0.5f, 0.5f, 0.5f);

        PointLight pl;
        vector<glm::vec3> pointLightPositions = {
                glm::vec3( 0.7f,  0.2f,  2.0f),
                glm::vec3( 2.3f, -3.3f, -4.0f),
                glm::vec3(-4.0f,  2.0f, -12.0f),
                /*glm::vec3( 0.0f,  0.0f, -3.0f)*/
        };

        pl.ambient=glm::vec3( 0.05f, 0.05f, 0.05f);
        pl.diffuse=glm::vec3( 0.8f, 0.8f, 0.8f);
        pl.specular=glm::vec3( 1.0f, 1.0f, 1.0f);
        pl.constant=1.0f;
        pl.linear= 0.09f;
        pl.quadratic= 0.032f;
        for(int i=0;i<pointLightPositions.size();i++){
            pl.position=pointLightPositions[i];
            pointlights.push_back(pl);
        }
    }
    void toggleCamera(){
        currCameraIndex+=1;
        currCameraIndex=currCameraIndex%cameras.size();
    }
    Camera& getCurrCamera(){
        return cameras[currCameraIndex];
    }

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        std::string vertexPathString(vertexPath);
        std::string fragmentPathString(fragmentPath);

        vertexPath = vertexPathString.c_str();
        fragmentPath= fragmentPathString.c_str();
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();			
            // if geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr)
            {
                std::string geometryPathString(geometryPath);
                geometryPath = geometryPathString.c_str();
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if(geometryPath != nullptr)
        {
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if(geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setProgramState(ProgramState*programState){
        setInt("plCount",programState->pointlights.size());
        setInt("slCount",programState->cameras.size());
        ;

        for(int i=0;i<programState->pointlights.size();i++) {
            setVec3("pointLights["+ to_string(i)+"].position", programState->pointlights[i].position);
            setVec3("pointLights["+ to_string(i)+"].ambient", programState->pointlights[i].ambient);
            setVec3("pointLights["+ to_string(i)+"].diffuse", programState->pointlights[i].diffuse);
            setVec3("pointLights["+ to_string(i)+"].specular", programState->pointlights[i].specular);
            setFloat("pointLights["+ to_string(i)+"].constant", programState->pointlights[i].constant);
            setFloat("pointLights["+ to_string(i)+"].linear", programState->pointlights[i].linear);
            setFloat("pointLights["+ to_string(i)+"].quadratic", programState->pointlights[i].quadratic);

        }
        setVec3("viewPosition", programState->getCurrCamera().Position);
        setFloat("material.shininess", 32.0f);
        for(int i=0;i<programState->cameras.size();i++) {
            setVec3("spotlights["+ to_string(i)+"].position", programState->cameras[i].spotLight.position);
            setVec3("spotlights["+ to_string(i)+"].direction", programState->cameras[i].spotLight.direction);
            setVec3("spotlights["+ to_string(i)+"].diffuse", programState->cameras[i].spotLight.diffuse);
            setVec3("spotlights["+ to_string(i)+"].specular", programState->cameras[i].spotLight.specular);
            setVec3("spotlights["+ to_string(i)+"].ambient", programState->cameras[i].spotLight.ambient);
            setFloat("spotlights["+ to_string(i)+"].constant", programState->cameras[i].spotLight.constant);
            setFloat("spotlights["+ to_string(i)+"].linear", programState->cameras[i].spotLight.linear);
            setFloat("spotlights["+ to_string(i)+"].quadratic", programState->cameras[i].spotLight.quadratic);
            setFloat("spotlights["+ to_string(i)+"].outerCutOff", programState->cameras[i].spotLight.outerCutOff);
            setFloat("spotlights["+ to_string(i)+"].cutOff", programState->cameras[i].spotLight.cutOff);
            setBool("spotlights["+ to_string(i)+"].on",programState->cameras[i].spotlightOn);
        }
        setVec3("dirlight.direction",programState->dirlight.direction);
        setVec3("dirlight.ambient",programState->dirlight.ambient);
        setVec3("dirlight.specular",programState->dirlight.specular);
        setVec3("dirlight.diffuse",programState->dirlight.diffuse);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->getCurrCamera().Zoom),
                                                (float) programState->SCR_WIDTH / (float) programState->SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->getCurrCamera().GetViewMatrix();
        setMat4("projection", projection);
        setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->backpackPosition); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(programState->backpackScale));    // it's a bit too big for our scene, so scale it down
        setMat4("model", model);







        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif
