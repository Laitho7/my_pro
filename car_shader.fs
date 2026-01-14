#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

struct Material {
    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;
    float opacity;
    sampler2D baseColorMap;
    sampler2D normalMap;
    sampler2D metallicRoughnessMap;
    sampler2D aoMap;
    sampler2D emissiveMap;
    sampler2D specularMap;
    sampler2D glossinessMap;
    int hasBaseColorMap;
    int hasNormalMap;
    int hasMetallicRoughnessMap;
    int hasAoMap;
    int hasEmissiveMap;
    int hasSpecularMap;
    int hasGlossinessMap;
};

uniform Material material;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    vec4 baseColor = material.baseColorFactor;
    if (material.hasBaseColorMap == 1) {
        baseColor *= texture(material.baseColorMap, fs_in.TexCoords);
    }

    float metallic = material.metallicFactor;
    float roughness = material.roughnessFactor;
    if (material.hasMetallicRoughnessMap == 1) {
        vec4 mr = texture(material.metallicRoughnessMap, fs_in.TexCoords);
        metallic *= mr.b;
        roughness *= mr.g;
    }

    float ao = 1.0;
    if (material.hasAoMap == 1) {
        ao = texture(material.aoMap, fs_in.TexCoords).r;
    }

    vec3 emissive = material.emissiveFactor;
    if (material.hasEmissiveMap == 1) {
        emissive += texture(material.emissiveMap, fs_in.TexCoords).rgb;
    }

    vec3 normal = normalize(fs_in.Normal);
    if (material.hasNormalMap == 1) {
        vec3 tangentNormal = texture(material.normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
        normal = normalize(fs_in.TBN * tangentNormal);
    }

    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(lightDirection + viewDir);
    float shininess = mix(8.0, 256.0, 1.0 - clamp(roughness, 0.0, 1.0));
    float specPower = pow(max(dot(normal, halfDir), 0.0), shininess);

    float specStrength = mix(0.04, 1.0, clamp(metallic, 0.0, 1.0));
    if (material.hasSpecularMap == 1) {
        specStrength *= texture(material.specularMap, fs_in.TexCoords).r;
    }
    if (material.hasGlossinessMap == 1) {
        specPower *= texture(material.glossinessMap, fs_in.TexCoords).r;
    }

    vec3 ambient = 0.08 * baseColor.rgb * ao;
    vec3 diffuse = diff * baseColor.rgb;
    vec3 specular = specStrength * specPower * lightColor;

    vec3 color = (ambient + diffuse) * lightColor + specular + emissive;
    float alpha = baseColor.a * material.opacity;

    FragColor = vec4(color, alpha);
}