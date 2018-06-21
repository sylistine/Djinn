//cbuffer ConstantData : register(b0)
//{
//    float4x4 wvp;
//};

struct FSInput
{
    float4 position : SV_Position;
};

float4 FragMain(FSInput i) : SV_Target
{
    return float4(1, 1, 1, 1);
}
