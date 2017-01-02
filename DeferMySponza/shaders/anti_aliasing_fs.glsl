#version 330

uniform	sampler2DRect frame;

out vec3 fragment_colour;

float threshold(in float min, in float max , in float val) {
	 if (val < min) { return 0.0; }
	 if (val > max) { return 1.0; }
	 return val;
}

float average(in vec3 val) {
	return (val.r + val.g + val.b) / 3;
}

float isEdge() {
	float avg[9];
	int k = 0;

	for(int i = -1; i < 2; i++) {
		for(int j = -1; j < 2; j++) {
			vec3 pix = texture(frame, gl_FragCoord.xy + vec2(i, j)).rgb;
			fragment_colour += pix;
			avg[k] = average(pix);
			k++;
		}
	}

	float delta = (abs(avg[1]-avg[7])+
		abs(avg[5]-avg[3]) +
		abs(avg[0]-avg[8])+
		abs(avg[2]-avg[6])
		)/4.;

	return threshold(0.05,.4,clamp(1.8*delta,0.0,1.0));
}

void main(void) {
	fragment_colour = vec3(0);
	bool edge = isEdge() != 0.0f;

	if(!edge) {
		fragment_colour = texture(frame, gl_FragCoord.xy).rgb;
	} else {
		fragment_colour /= 9;
	}
}