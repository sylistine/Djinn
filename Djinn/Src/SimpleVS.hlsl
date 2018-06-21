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
    float2 screenpos : ScreenPosition;
};

VSOutput VertMain(VSInput i)
{
    VSOutput o;
    o.position = float4(i.position * 0.25, 1);
    o.screenpos = (i.position.xy + 1) / 2;
    //o.position = mul(float4(i.position, 1), wvp);
    return o;
}
