void main(void)
{
    // General stuff
    vec2 uv = getUV();

    // Neon grid background
    vec2 grid = floor(uv * vec2(cColumns * 2.0, cColumns * 1.5));
    float gridPulse = sin(iTime * 0.5 + grid.x + grid.y * 1.3) * 0.2 + 0.8;
    vec3 neonBase = vec3(0.1, 0.05, 0.2) * gridPulse;

    // Rain with neon glow
    vec2 gv = floor(uv * cColumns);
    float rnd = h11(gv.x) + 0.1;
    float bw = 1.2 - fract((gv.y * 0.0024) + iTime * rnd * 1.5);

    // FFT-based brightness (audio reactivity)
    float fft = texture(iChannel0, vec2((1.0 - abs(uv.x)) * 0.7, 0.0)).x;
    fft -= abs(uv.x) * 0.25;

    // Amplify neon effect with audio
    bw *= 1.0 + fft * 0.6 * cRainHighlights;
    bw += bw * clamp(pow(fft * 1.5 * cRainHighlights, 2.0) - 8.0, 0.0, 1.0);
    bw += bw * clamp(pow(fft * 1.2 * cRainHighlights, 3.0) - 15.0, 0.0, 0.8);
    bw = min(bw, 2.5);

    // Neon color mapping (cyan/magenta)
    vec3 neonColor = vec3(
        sin(gv.x * 0.3 + iTime * 0.2) * 0.4 + 0.6,  // Red
        sin(gv.x * 0.3 + iTime * 0.2 + 2.0) * 0.4 + 0.6,  // Green
        sin(gv.x * 0.3 + iTime * 0.2 + 4.0) * 0.4 + 0.8   // Blue
    );

    // Combine with grid
    vec3 col = mix(neonBase, neonColor, bw * 0.8);

    // Add glow effect
    float glow = bw * 0.5;
    col += vec3(0.2, 0.4, 0.8) * glow * (1.0 - length(fract(uv * cColumns) - 0.5));

    // Vignette to focus the neon effect
    float vignette = length(uv) * cVIGNETTEINTENSITY * 1.5;
    col -= vignette * 0.5;

    FragColor = vec4(col, 1.0);
}
