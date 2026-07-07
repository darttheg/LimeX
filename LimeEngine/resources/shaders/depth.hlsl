float4x4 mWorldViewProj;
float near;
float far;

struct VS_OUTPUT {
    float4 Position : POSITION;
    float2 Depth : TEXCOORD0;
};

VS_OUTPUT vertexMain(float4 vPosition : POSITION) {
    VS_OUTPUT o;
    o.Position = mul(vPosition, mWorldViewProj);
    o.Depth = o.Position.zw;
    return o;
}

struct PS_OUTPUT {
    float4 RGBColor : COLOR0;
};

PS_OUTPUT pixelMain(float2 Depth : TEXCOORD0) {
    float d = Depth.x / Depth.y;
    float lDepth = (2.0 * near) / (far + near - d * (far - near));
    PS_OUTPUT o;
    
    o.RGBColor = float4(lDepth, lDepth, lDepth, 1.0);
    return o;
}