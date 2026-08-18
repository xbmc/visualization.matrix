// Odysseus Matrix Shader
// Combines Odysseus, Hermes, π, Helvetia, and Mythos² themes
// Dynamic, interactive matrix visualization with audio reactivity

precision highp float;

uniform vec2 iResolution;
uniform float iTime;
uniform sampler2D iChannel0; // Audio texture
uniform sampler2D iChannel1; // Noise texture (if available)

// Hash function for pseudo-randomness (π-inspired)
float hash(vec2 p) {
    float pi = 3.14159265359;
    p = 50.0 * fract(p * pi);
    return fract(p.x * p.y * (p.x + p.y));
}

// Swiss cross SDF (Helvetia)
float crossSDF(vec2 uv, float size) {
    vec2 a = abs(uv);
    float m = min(a.x, a.y);
    return max(m - size/4.0, length(a) - size/2.0);
}

// Odysseus' spiral path
vec2 odysseusPath(float t) {
    float speed = 0.1;
    return vec2(
        cos(t * speed) * 0.4,
        sin(t * speed) * 0.4
    );
}

// Hermes' wing pattern (abstract)
float hermesWings(vec2 uv, float t) {
    float wing = sin(uv.x * 10.0 + t * 2.0) * cos(uv.y * 10.0 + t * 1.5);
    return wing * 0.5 + 0.5;
}

// π-based noise for matrix symbols
float piNoise(vec2 uv, float t) {
    float pi = 3.14159265359;
    float n = hash(uv + t * 0.1);
    return fract(n * pi * 1000.0);
}

// Symbol selection (0: Odysseus, 1: Hermes, 2: Helvetia, 3: π)
vec4 getSymbol(vec2 uv, float t, float audioLevel) {
    // Divide screen into a grid
    vec2 gridUV = fract(uv * 20.0);
    float cellId = floor(uv.x * 20.0) + floor(uv.y * 20.0) * 20.0;

    // Use π-based noise to select symbols
    float symbolType = fract(cellId * 0.1031 + t * 0.05 + audioLevel * 10.0);

    // Symbol colors
    vec3 odysseusColor = vec3(0.0, 1.0, 0.0); // Green (ship)
    vec3 hermesColor = vec3(1.0, 1.0, 0.0);   // Yellow (wings)
    vec3 helvetiaColor = vec3(1.0, 0.0, 0.0); // Red (cross)
    vec3 piColor = vec3(1.0, 0.5, 0.0);       // Orange (π)

    // Assign symbols based on noise
    vec3 symbolColor;
    if (symbolType < 0.25) {
        symbolColor = odysseusColor; // ⛵ Odysseus
    } else if (symbolType < 0.5) {
        symbolColor = hermesColor;   // ✈ Hermes
    } else if (symbolType < 0.75) {
        symbolColor = helvetiaColor; // ➕ Helvetia
    } else {
        symbolColor = piColor;        // π
    }

    // Falling effect (Matrix-style)
    float fallPos = fract(t * 0.5 + cellId * 0.01 + audioLevel * 5.0);
    float symbolAlpha = smoothstep(0.0, 0.1, fallPos) * (1.0 - smoothstep(0.9, 1.0, fallPos));

    return vec4(symbolColor, symbolAlpha);
}

// Mythos²: Recursive matrix layers
vec4 mythos2(vec2 uv, float t, float audioLevel) {
    vec4 col = vec4(0.0);

    // Layer 1: Background matrix
    col += getSymbol(uv, t, audioLevel) * 0.7;

    // Layer 2: Smaller nested matrix
    col += getSymbol(uv * 2.0, t * 1.5, audioLevel * 0.5) * 0.3;

    return col;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 p = uv * 2.0 - 1.0; // Center coordinates

    // Audio reactivity (sample from iChannel0)
    float audioLevel = 0.0;
    if (iChannel0 != sampler2D(vec2(0.0))) {
        vec2 audioUV = vec2(fract(iTime * 0.1), 0.5);
        audioLevel = texture2D(iChannel0, audioUV).r;
    }

    // Base color (dark background)
    vec4 col = vec4(0.05, 0.05, 0.1, 1.0);

    // Mythos²: Recursive matrix layers
    col += mythos2(uv, iTime, audioLevel);

    // Odysseus' path (green spiral)
    vec2 pathPos = odysseusPath(iTime);
    float pathDist = distance(p, pathPos);
    col.rgb += smoothstep(0.05, 0.0, pathDist) * vec3(0.0, 1.0, 0.0) * (0.5 + audioLevel * 0.5);

    // Hermes' wings (yellow dynamic shapes)
    float wingPattern = hermesWings(uv, iTime);
    col.rgb += wingPattern * vec3(1.0, 1.0, 0.0) * (0.3 + audioLevel * 0.2);

    // Helvetia's Swiss cross (red, centered)
    float cross = crossSDF(p, 0.5);
    col.rgb += smoothstep(0.05, 0.0, cross) * vec3(1.0, 0.0, 0.0) * (0.7 + audioLevel * 0.3);

    // π-based noise overlay
    float noise = piNoise(uv, iTime);
    col.rgb += noise * 0.1 * vec3(1.0, 0.5, 0.0);

    gl_FragColor = col;
}
