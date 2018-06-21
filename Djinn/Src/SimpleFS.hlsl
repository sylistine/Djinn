//cbuffer ConstantData : register(b0)
//{
//    float4x4 wvp;
//};

struct FSInput
{
    float4 position : SV_Position;
    float2 screenpos : ScreenPosition;
};

float4 FragMain(FSInput i) : SV_Target
{
    return float4(i.screenpos.xy, 1, 1);
}
