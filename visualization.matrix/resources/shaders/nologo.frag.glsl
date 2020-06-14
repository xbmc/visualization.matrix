void main(void)
{
    //general stuff
    vec2 uv = (gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y;
    
    //rain
    vec2 gv = floor(uv*cColumns);
    float rnd = h11(gv.x) + 0.1;
    float bw = .65 - fract((gv.y*.0024)+iTime*rnd);
    
    //FFT stuff (visualization)
    float fft = texture(iChannel0, vec2((1.-abs(uv.x))*.7,0.0)).x;
    fft -= abs(uv.x)*.25;

    bw *= 1. + fft*0.4;
    bw += bw*clamp((pow(fft*1.3,2.)-12.),.0,.6);
    bw += bw*clamp((pow(fft*1.0,3.)-23.),.0,.7);
    bw = min(bw,1.99);
    
    //noise texture
	bw *= texture(iChannel2, vec2(gl_FragCoord.xy/(256.*iDotSize))).x;

	//vignette effect
	float vignette = length(uv)*VIGNETTEINTENSITY;
	bw -= vignette;
	
    //pseudo pixels (dots)
    float d = length(fract(uv*cColumns)-.5);
    float peakcolor = smoothstep(.35,.00,d)*bw;
    float basecolor = smoothstep(.85,.00,d)*bw;

    //vec3 col = vec3(basecolor*RED+peakcolor,basecolor*GREEN+peakcolor,basecolor*BLUE+peakcolor);
    vec3 col = basecolor*cColor+peakcolor;

    col *= INTENSITY;    
    
    FragColor = vec4(col,1.0);
}
