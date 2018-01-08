
cbuffer cbIndex
{
    float4 meshIndex;
};

SamplerState    sam;
Texture2D       tex;


struct VertexOut
{
    float4 posH : SV_Position;
    float3 posW : POSITIONT;
    float3 normalW : NORMAL;
    float2 tex : TEXCOORD;
};

struct PixelOut
{
    float4 coloar : SV_Target0;
    float4 index : SV_Target1;
};
PixelOut main(VertexOut pin)
{
    PixelOut pout;
    float factor = dot(normalize(pin.normalW), normalize(float3(1, 1, 1)));
    pout.coloar = tex.Sample(sam, pin.tex)*factor+float4(0.2f,0.2f,0.2f,0.f);
    pout.index = meshIndex;
    return pout;
}