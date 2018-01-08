struct VertexOut
{
    float4 posH : SV_Position;
};


float4 main(VertexOut pin) : SV_Target
{
    return float4(1, 1, 1, 1);
}