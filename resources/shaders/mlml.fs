#version 330 core
out vec4 FragColor;
#define MAX_LIGHT_COUNT (16)
struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
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
    bool on;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};
struct aMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
uniform int plCount;
uniform int slCount;
uniform PointLight pointLights[MAX_LIGHT_COUNT];

uniform Material material;
uniform SpotLight spotlights[MAX_LIGHT_COUNT];
uniform DirLight dirlight;
uniform vec3 viewPosition;
uniform aMaterial mat;
// calculates the color when using a point light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform bool hasTexture;

vec3 mydiff=mat.diffuse;
vec3 myspec=mat.specular;
vec3 myambient=mat.ambient;
float myshine=mat.shininess;

void main()
{
if(hasTexture){
    mydiff=mydiff*texture(material.texture_diffuse1,TexCoords).xyz;
    myspec=myspec*texture(material.texture_specular1,TexCoords).xyz;
    myambient=myambient;
    myshine=myshine*material.shininess;
}
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result =vec3(0);
     result+=CalcDirLight(dirlight,normal,viewDir);
     for(int i=0;i<plCount;i++)
        result+=CalcPointLight(pointLights[i], normal, FragPos, viewDir);
     for(int i=0;i<slCount;i++)
        result+=CalcSpotLight(spotlights[i],normal,FragPos,viewDir);
    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), myshine);
    // combine results
    vec3 ambient = light.ambient *myambient;
    vec3 diffuse = light.diffuse * diff *mydiff;
    vec3 specular = light.specular * spec *myspec;
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), myshine);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient *myambient;
    vec3 diffuse = light.diffuse * diff *mydiff;
    vec3 specular = light.specular * spec *myspec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if(!light.on)
        return vec3(0);
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), myshine);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient *myambient;
    vec3 diffuse = light.diffuse * diff *mydiff;
    vec3 specular = light.specular * spec *myspec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}