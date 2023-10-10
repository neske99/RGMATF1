//
// Created by neske on 8.10.23..
//

#ifndef PROJECT_BASE_LIGHTS_H
#define PROJECT_BASE_LIGHTS_H

#endif //PROJECT_BASE_LIGHTS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace  glm;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    PointLight(vec3 position,vec3 ambient,vec3 diffuse,vec3 specular,float constant,float linear,float quadratic){
        this->position=position;
        this->constant=constant;
        this->linear=linear;
        this->quadratic=quadratic;
        this->ambient=ambient;
        this->diffuse=diffuse;
        this->specular=specular;
    }
    PointLight(){

    }
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
