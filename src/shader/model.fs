#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform sampler2D texture_specular1;

uniform sampler2D depthTexture;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLight;

uniform vec3 viewPos;
uniform bool normalTexture;
uniform vec3 selected_color;
uniform vec3 diffuseTex;
uniform vec3 specularTex;
uniform vec3 ambientTex;

uniform mat4 view;
uniform mat4 directionalLightSpaceMatrix;
uniform bool enableOmnidirectionalPCF;
uniform int directionalFilterRadius;
uniform samplerCube depthCubeTexture;
uniform float pointLightZfar;
uniform bool enableShadow;

float DirectionShadowCalculation(vec3 fragPos)
{   
    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    vec4 fragPosLightSpace=directionalLightSpaceMatrix*vec4(fragPos,1.0f);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthTexture, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    float bias = max(0.0005* (1.0 - dot(Normal, dirLight.direction)), 0.0005);
    float shadow = 0.0;
    if(enableOmnidirectionalPCF)
    {
        vec2 texelSize = 1.0 / textureSize(depthTexture, 0);
		for(int x = -directionalFilterRadius; x <= directionalFilterRadius; ++x)
		{
			for(int y = -directionalFilterRadius; y <= directionalFilterRadius; ++y)
			{
				float pcfDepth = texture(depthTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
			}    
		}
		shadow /= (2*directionalFilterRadius+1)*(2*directionalFilterRadius+1);
		// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    }
    else
    {
       shadow = currentDepth-bias > closestDepth  ? 1.0 : 0.0; 
    }
    return shadow;
}
float PointShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = FragPos - pointLight.position;
    float closestDepth = texture(depthCubeTexture, fragToLight).r;
    closestDepth *= pointLightZfar;
    float shadow;
    float currentDepth = length(fragToLight);
    if(enableOmnidirectionalPCF)
    {
        float bias = 0.05; 
        float samples = 4.0;
        float offset = 0.1;
		for(float x = -offset; x < offset; x += offset / (samples * 0.5))
		{
			for(float y = -offset; y < offset; y += offset / (samples * 0.5))
			{
				for(float z = -offset; z < offset; z += offset / (samples * 0.5))
				{
					float closestDepth = texture(depthCubeTexture, fragToLight + vec3(x, y, z)).r; 
					closestDepth *= pointLightZfar;   // Undo mapping [0;1]
					if(currentDepth - bias > closestDepth)
						shadow += 1.0;
				}
			}
		}
		shadow /= (samples * samples * samples);
    }
    else
    {
        float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
        shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;   
    }
     
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0f);
    // 合并结果
    vec3 ambient = light.ambient*0.03 * ambientTex;
    vec3 diffuse = light.diffuse * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;
    float attenuation=0.5;
    if(!normalTexture)
    {
        ambient = light.ambient *0.03* vec3(texture(texture_diffuse1, TexCoord));
        diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));
        specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    }
    if(enableShadow)
    {
        float dShadow=DirectionShadowCalculation(FragPos);
        diffuse=(1.0-dShadow)*diffuse;
        specular=(1.0-dShadow)*specular;
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse+ specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient*0.03 * ambientTex;
    vec3 diffuse = light.diffuse * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;
    if(!normalTexture)
    {
        ambient = light.ambient*0.03 * vec3(texture(texture_diffuse1, TexCoord));
        diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));
        specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    if(enableShadow)
    {
        float pShadow=PointShadowCalculation(FragPos);
        diffuse=(1.0-pShadow)*diffuse;
        specular=(1.0-pShadow)*specular;

    }
    return (ambient + diffuse + specular);
}

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);    
    FragColor = vec4(result, 1.0);
}