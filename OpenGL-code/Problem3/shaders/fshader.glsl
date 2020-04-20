#version 400 core

out vec4 frag_colour;

in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;

const float PI = 3.14159265359;

struct Material {
    sampler2D ALBEDO;
    vec3 specular;
    float shininess;
    sampler2D NORMAL;
    sampler2D AMBIENT;
    sampler2D ROUGHNESS;
    sampler2D HT ;
};

struct Light {
    vec3 position;    //light's position
    vec3 direction;
    vec3 specular;
    vec3 diffuse;
    float constant;
    float linear;
    float quadratic;
    int type;
    float cutOff;
    float outerCutOff;
};

uniform vec3 globalAmbient;
uniform Material material;
uniform sampler2D ourtexture ;
uniform Light light;
uniform mat4 normalMatrix;
uniform vec3 viewLoc;   //camera position
uniform vec3 camfront ; // camera's front
uniform int blinnPhong;

vec3 TangentLightPos, TangentViewPos, TangentFragPos ;
float heightScale = 0.1;
vec2 TexCoord = TexCoords ;

vec3 getNormalFromMap(){
    vec3 normal_n = texture(material.NORMAL, TexCoord).rgb;
    normal_n = (normal_n*2.0 - 1.0);
    
    vec3 Q1 = dFdx(FragPos) ;
    vec3 Q2 = dFdy(FragPos) ;
    vec2 st1 = dFdx(TexCoord) ;
    vec2 st2 = dFdy(TexCoord) ;
    
    vec3 N = normalize(normal) ;
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t) ;
    vec3 B = -normalize(cross(N, T)) ;
    mat3 TBN = mat3(T, B, N) ;

    TangentLightPos = light.position * TBN ;
    TangentViewPos = viewLoc * TBN ;
    TangentFragPos = FragPos * TBN ;

    return normalize(TBN*normal_n);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
//-----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
//-----------------------------------------------------------------------------
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.HT, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.HT, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.HT, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
//-----------------------------------------------------------------------------------

void main(){

///////////////////////////////////////////////////////////////////////////////////////////////

    if(light.type == 1){

	    vec3 albedo = pow(texture(material.ALBEDO, TexCoord).rgb, vec3(2.2));
	    float roughness = texture(material.ROUGHNESS, TexCoord).r ;
	    float ao = texture(material.AMBIENT, TexCoord).r ;
	    float metallic = 20 ;

	    vec3 N = getNormalFromMap() ;
	    vec3 V = normalize(viewLoc - FragPos) ;        // view dir
	    vec3 F0 = vec3(0.04);
	    F0 = mix(F0, albedo, metallic) ;

	    //reflectance eqn
	    vec3 Lo = vec3(0.0) ;
	    //calculate per light radiance
	    vec3 L = normalize(light.position - FragPos) ;  //light direc
	    vec3 H  = normalize(L + V) ;                      // half way
	    float distance = length(light.position - FragPos) ;
	    float attenuation = 1.0 / (distance*distance);
	    vec3 radiance = vec3(1.0) * attenuation ;
	    vec3 reflectDir = reflect(-L, N);   /////////////// ????????

	    //cook torrance BRDF

	    if (blinnPhong != 0){

            }
	    else{
               N = V; H = reflectDir ;
            }

	    float NDF = DistributionGGX(N, H, roughness) ;
	    float G = GeometrySmith(N, V, L, roughness) ;
	    vec3 F = fresnelSchlick(max(dot(H, V), 0), F0) ; 

	    vec3 nominator = NDF * G * F ;
	    float denominator = 4* max(dot(N, V), 0.0) * max(dot(N, L), 0.0) +0.001 ;// 0.001 to prevent divide by zero  
	    vec3 specular = nominator/denominator ;

	    //ks is equal to fresnel
	    vec3  kS= F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.     
	    vec3 kD = vec3(1.0) - kS;   
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light). 
	    kD *= 1.0 - metallic ;  

		// scale light by NdotL
	    float NdotL = max(dot(N, L), 0.0);   
	   // add to outgoing radiance Lo
	    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

	    // ambient lighting 
	    vec3 ambient = vec3(0.03) * albedo * ao;
	    
	    vec3 color = ambient + Lo;

	    // HDR tonemapping
	    color = color / (color + vec3(1.0));
	    // gamma correct
	    color = pow(color, vec3(1.0/2.2)); 

	    frag_colour = vec4(color, 1.0);
    }
    else if(light.type == 0){
	    // ambient
	    vec3 ambient = globalAmbient * texture(material.ALBEDO, TexCoords).rgb;
            ambient *= texture(material.AMBIENT, TexCoords).r ;
	    vec3 N = getNormalFromMap() ;	
	    // diffuse 

	    vec3 lightDir = normalize(-light.direction);  
	    float diff = max(dot(N, lightDir), 0.0);
	    vec3 diffuse = light.diffuse * diff * texture(material.AMBIENT, TexCoords).rgb; ;  
	    
	    // specular
	    vec3 viewDir = normalize(viewLoc - FragPos);
	    vec3 reflectDir = reflect(-lightDir, N);
            vec3 halfway = normalize(lightDir + viewDir);  
            float spec ;
	    if (blinnPhong != 0)
		spec = pow(max(dot(N, halfway), 0.0), material.shininess*(texture(material.ROUGHNESS, TexCoord).r));
	    else
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess* (texture(material.ROUGHNESS, TexCoord).r));
	    vec3 specular = light.specular * spec * material.specular;  
		
	    vec3 result = ambient + diffuse + specular;
	    frag_colour = vec4(result, 1.0);            

    }

    else {   // spotlight
	    // ambient
            int cutOffState = 0 ;
	    vec3 ambient = globalAmbient * pow(texture(material.ALBEDO, TexCoords).rgb, vec3(2.2));
	    vec3 N = getNormalFromMap() ;	 
	    // diffuse 

	    vec3 lightDir = normalize(light.position - FragPos);
	    float diff = max(dot(N, lightDir), 0.0);
	    vec3 diffuse = light.diffuse * diff * ambient;  
	    
	    // specular
	    vec3 viewDir = normalize(viewLoc - FragPos);
   	    vec3 halfway = normalize(lightDir + viewDir);
	    vec3 reflectDir = reflect(-lightDir, N);   
            float spec ;
	    if (blinnPhong != 0)
		spec = pow(max(dot(N, halfway), 0.0), material.shininess*(texture(material.ROUGHNESS, TexCoord).r));
	    else
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess* (texture(material.ROUGHNESS, TexCoord).r));
	    vec3 specular = light.specular * spec * material.specular;  
	    
	    // spotlight (soft edges)
	    float theta = dot(lightDir, normalize(light.position)); 
	    float epsilon = (light.cutOff - light.outerCutOff);
	    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	    diffuse  *= intensity;
	    specular *= intensity;
	    
	    // attenuation
	    float distance    = length(light.position - FragPos);
	    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	    //ambient  *= attenuation; 
	    diffuse   *= attenuation;
	    specular *= attenuation;   
		
	    vec3 result = ambient + diffuse + specular;
	    if (cutOffState==1){
		result = result - diffuse - specular;
		diffuse *= intensity; specular *= intensity;
		result = result + diffuse + specular;
	    }
	    frag_colour = vec4(result, 1.0);    



    }



}   
