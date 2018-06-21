//cbuffer ConstantData : register(b0)
//{
//    float4x4 wvp;
//};

struct VSInput
{
    float3 position : Position;
};

struct VSOutput
{
    float4 position : SV_Position;
};

VSOutput VertMain(VSInput i)
{
    VSOutput o;
    o.position = float4(i.position, 1);
    //o.position = mul(float4(i.position, 1), wvp);
    return o;
}
