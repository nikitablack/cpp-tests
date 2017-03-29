#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Shape.h"
#include "ShapesRenderer.h"
#include "Window.h"

using namespace math;
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

ShapesRenderer::ShapesRenderer(UINT const bufferCount, Window const & window) : Graphics{ bufferCount, window }
{
	createConstantBuffer(window);
	createVertexShaderAndInputLayout();
	createPixelShader();
	createRasterizerState();
	createDepthStencilState();

	Vec2 const wSize(window.getSize());

	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width = wSize.x;
	_viewport.Height = wSize.y;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	_context->IASetInputLayout(_inputLayout.Get());
	_context->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_context->PSSetShader(_pixelShader.Get(), nullptr, 0);
	_context->RSSetViewports(1, &_viewport);
	_context->RSSetState(_rasterizerState.Get());
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShapesRenderer::render(vector<float> const & data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(_context->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		throw(runtime_error{ "Error mapping vertex buffer" });
	}

	memcpy(mappedResource.pData, data.data(), data.size() * sizeof(float));

	_context->Unmap(_vertexBuffer.Get(), 0);

	UINT numVertices{ static_cast<UINT>(data.size() / 5) };

	UINT presentCount;
	_swapChain->GetLastPresentCount(&presentCount);

	static FLOAT const clearColor[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	ID3D11RenderTargetView* currRtv{ _renderTargetViews[presentCount % _bufferCount].Get() };
	_context->OMSetRenderTargets(1, &currRtv, nullptr);
	_context->ClearRenderTargetView(currRtv, clearColor);
	_context->Draw(numVertices, 0);
	_swapChain->Present(0, 0);
}

void ShapesRenderer::createVertexBuffer(UINT const vertexBufferSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = vertexBufferSize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (FAILED(_device->CreateBuffer(&desc, nullptr, _vertexBuffer.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating vertex buffer." });
	}

	UINT const stride{ sizeof(ShapeShaderData) };
	UINT const offset{ 0 };
	_context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
}

void ShapesRenderer::createConstantBuffer(Window const & window)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(XMFLOAT4X4);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	Vec2 const wSize(window.getSize());

	XMFLOAT4X4 matrixProjection;
	XMMATRIX const matrixProjectionDX(XMMatrixOrthographicOffCenterLH(0.0f, wSize.x, wSize.y, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&matrixProjection, matrixProjectionDX);

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = &matrixProjection;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	if (FAILED(_device->CreateBuffer(&desc, &initialData, _constantBuffer.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating constant buffer." });
	}

	_context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
}

void ShapesRenderer::createVertexShaderAndInputLayout()
{
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3DReadFileToBlob(L"VertexShader.cso", blob.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error reading vertex shader." });
	}

	if (FAILED(_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, _vertexShader.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating vertex shader." });
	}

	vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
	inputLayoutDescs.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	inputLayoutDescs.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 });

	if (FAILED(_device->CreateInputLayout(inputLayoutDescs.data(), static_cast<UINT>(inputLayoutDescs.size()), blob->GetBufferPointer(), blob->GetBufferSize(), _inputLayout.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating input layout." });
	}
}

void ShapesRenderer::createPixelShader()
{
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3DReadFileToBlob(L"PixelShader.cso", blob.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error reading pixel shader." });
	}

	if (FAILED(_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, _pixelShader.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating pixel shader." });
	}
}

void ShapesRenderer::createRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = FALSE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	if (FAILED(_device->CreateRasterizerState(&desc, _rasterizerState.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating rasterizer state." });
	}
}

void ShapesRenderer::createDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = false;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	if (FAILED(_device->CreateDepthStencilState(&desc, _depthStencilState.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating rasterizer state." });
	}
}
