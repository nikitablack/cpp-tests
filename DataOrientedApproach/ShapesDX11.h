#pragma once

#include "GraphicsDX11.h"

class ShapesDX11 : public GraphicsDX11
{
public:
	ShapesDX11(UINT bufferCount, std::string name, LONG width, LONG height);
	void render();

	void createVertexBuffer(UINT numVertices);
	void setVertices(const std::vector<float>& data);

private:
	void createConstantBuffer();
	void createVertexShaderAndInputLayout();
	void createPixelShader();
	void createRasterizerState();
	void createDepthStencilState();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	D3D11_VIEWPORT viewport;
};