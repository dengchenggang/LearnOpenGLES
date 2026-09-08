#version 300 es
precision mediump float;

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main() {
    fragColor = texture(uTexture, vTexCoord) * uColor;
}