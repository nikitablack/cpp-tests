#pragma once

#include <vector>

#include "Graphics.h"

class ShapesRenderer : public Graphics
{
public:
	ShapesRenderer(UINT const bufferCount, class Window const & window);
	void render(std::vector<float> const & data);
	void createVertexBuffer(UINT const vertexBufferSize);

private:
	void createConstantBuffer(class Window const & window);
	void createVertexShaderAndInputLayout();
	void createPixelShader();
	void createRasterizerState();
	void createDepthStencilState();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _colorsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;
	D3D11_VIEWPORT _viewport;
};