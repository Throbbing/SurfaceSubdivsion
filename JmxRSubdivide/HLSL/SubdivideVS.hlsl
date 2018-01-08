cbuffer cbTrans
{
    float4x4 vp;
};


struct VertexIn
{
    float3 posW : POSITION;
};

struct VertexOut
{
    float4 posH : SV_Position;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.posH = mul(float4(vin.posW, 1.f), vp);

    return vout;
}