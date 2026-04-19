#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 flash;

void main()
{
    vec4 texelColor = texture2D(texture0, fragTexCoord) * colDiffuse * fragColor;

    texelColor.rgb = mix(texelColor.rgb, vec3(1.0, 1.0, 1.0), flash.x);

    gl_FragColor = texelColor;
}