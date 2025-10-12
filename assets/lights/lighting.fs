// ================================================
// AI usage note: This module was developed based on AI-generated (ChatGPT) code.
// The author made additional changes and performed partial refactoring.
// ================================================
#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 ambient;

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
    float diff = max(dot(norm, -lightDir), 0.0);

    vec4 texColor = texture(texture0, fragTexCoord);
    finalColor = texColor * (ambient + vec4(vec3(diff), 1.0));
}
