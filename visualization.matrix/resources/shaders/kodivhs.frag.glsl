void main(void)
{
    //general stuff
    vec2 uv = (gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y;
    
    float wav = texture( iChannel0, vec2((uv.y +1.)*.5,1.0) ).x-.5;
	float distort = sign(wav) * max(abs(wav)-cDistortThreshold,.0);
	uv += distort*.5;
    vec2 gv = floor(uv*cColumns);
    //rain
    float rnd = h11(gv.x) + 0.1;
    float bw = 1. - fract((gv.y*.0024)+iTime*rnd);
    bw *= .25;
    
    //VHS-like distortions
	
    
    //KODI texture
    float tex = texture(iChannel1, uv+.5-vec2(distort*DISTORTFACTORX,distort*DISTORTFACTORY)).x;
    //float tex = texture(iChannel1, uv+.5-vec2(distort,distort)).x;
    //float tex = texture(iChannel1, uv+.5).x;
    tex *= .9 - wav*.2;
    //Shadow effect around the KODI texture. Needs a prepared texture to work.
    float shadow = (wav+.5)*.25;
    tex = (max(shadow,tex)-shadow)/(1.-shadow);
    
    //"interlaced" logo distortion effect
    float line = mod(gv.y*sign(wav),2.);
    float distort_abs = abs(distort);
    //tex *= 1. - (line*10.*distort_abs + 5.*distort_abs);
    tex *= 1. - (line*10.*distort_abs + 5.*distort_abs);
    
    bw *= tex + .1;
    
    //brightens lines where distortion are occuring
	bw += distort_abs*.1;
    
    //FFT stuff (visualization)
    //might need some scaling
    float fft = texture( iChannel0, vec2((1.-abs(uv.x-distort*.2))*.7,0.0) ).x;
    fft *= (3.2 -abs(0.-uv.x*1.3))*0.75;
    fft *= 1.8;
    
    bw=bw+bw*fft*0.4;
    	bw += bw*max((pow(fft*1.3,2.)-12.),.6);
    bw += bw*max((pow(fft*1.0,3.)-23.),.7);
    bw = min(bw,1.99);
    
    //noise texture
	bw *= noise(gv);

	//vignette effect
	float vignette = length(uv)*VIGNETTEINTENSITY;
	bw -= vignette;
	
    //pseudo pixels (dots)
    vec3 col = bw2col(bw,uv);
    //col.r = tex;
    //col *= INTENSITY;
    //col += vertex_color*0.2;
    FragColor = vec4(col,1.0);
}