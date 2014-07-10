struct PixelShaderInput
{
	float4 color: COLOR;
};

float4 Main(PixelShaderInput IN): SV_TARGET
{
	return IN.color;
}