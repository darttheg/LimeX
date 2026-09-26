uniform mat4 mWorldViewProj;
uniform mat4 mWorldView;

varying float vDepth;

void main() {
	gl_Position = mWorldViewProj * gl_Vertex;
	vDepth = (mWorldView * gl_Vertex).z;
}
