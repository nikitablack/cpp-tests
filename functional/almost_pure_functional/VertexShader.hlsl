cbuffer ConstantBufferProjectionMatrix : register(b0)
{
	row_major float4x4 projMatrix;
};

struct VertexData
{
	float2 pos : POSITION;
	float3 color : COLOR;
};

struct VertexToPixel
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

VertexToPixel main(VertexData input)
{
	VertexToPixel output;
	output.pos = mul(float4(input.pos, 0.0f, 1.0f), projMatrix);
	output.color = input.color;

	return output;
}