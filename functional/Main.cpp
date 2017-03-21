#include <algorithm>
#include <array>
#include <chrono>
#include <numeric>
#include <stdexcept>
#include <string>
#include <Windows.h>

#include "Shape.h"
#include "ShapesRenderer.h"
#include "Window.h"

using namespace math;
using namespace std;

extern vector<Shape> createShapes(uint32_t const numShapes, float const canvasW, float const canvasH);
extern Shape createWall(float const w, float const h, Vec2 const pos);
extern vector<float> getShaderData(vector<Shape> const shapes);
extern vector<Shape> updateSimulation(float const dt, vector<Shape> const shapes, float const width, float const height);

string getElapsedTimeStr(chrono::milliseconds const time, size_t const n)
{
	static uint32_t const numTimeSamples{ 16 };
	static array<long long, numTimeSamples> timeSamples;
	static int counter{ 0 };

	if (counter >= numTimeSamples)
		counter = 0;

	timeSamples[counter++] = time.count();
	long long const sum{ accumulate(timeSamples.begin(), timeSamples.end(), 0LL) };
	return "num shapes: " + to_string(n) + ", update time: " + to_string(sum / numTimeSamples) + "ms";
}

int WINAPI WinMain(HINSTANCE const, HINSTANCE const, LPSTR const, int const)
{
	Window window{ 100.0f, 100.0f, 800.0f, 600.0f, "" };
	ShapesRenderer renderer{ 3, window };

	uint32_t const numShapes{ 330 };
	renderer.createVertexBuffer(static_cast<UINT>((numShapes + 4) * 10 * 3 * sizeof(ShapeShaderData)));

	Vec2 const wSize{ window.getSize() };

	vector<Shape> shapes;
	shapes.push_back(createWall(10.0f, wSize.y, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(createWall(wSize.x, 10.0f, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(createWall(10.0f, wSize.y, Vec2{ wSize.x - 10.0f, 0.0f }));
	shapes.push_back(createWall(wSize.x, 10.0f, Vec2{ 0.0f, wSize.y - 10.0f }));

	vector<Shape> const tmp{ createShapes(numShapes, wSize.x, wSize.y) };
	for_each(tmp.begin(), tmp.end(), [&shapes](Shape const shape) {
		shapes.push_back(shape);
	});

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	float dt{ 0.0f };

	using namespace chrono;
	while (msg.message != WM_QUIT)
	{
		BOOL const r{ PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) };
		if (r == 0)
		{
			try
			{
				auto const start{ high_resolution_clock::now() };

				{
					Vec2 const wSize{ window.getSize() };

					auto const simStart{ high_resolution_clock::now() };

					static uint32_t const numPhysicsSteps{ 20 };
					float const dtStep{ dt / numPhysicsSteps };
					for (uint32_t s{ 0 }; s < numPhysicsSteps; ++s)
					{
						shapes = updateSimulation(dtStep, move(shapes), wSize.x, wSize.y);
					}

					auto const simTime{ duration_cast<milliseconds>(high_resolution_clock::now() - simStart) };

					window.setTitle(getElapsedTimeStr(simTime, shapes.size()));
				}

				vector<float> const shaderData{ getShaderData(shapes) };
				renderer.render(move(shaderData));

				duration<float> const time{ high_resolution_clock::now() - start };
				dt = time.count();
			}
			catch (runtime_error const & err)
			{
				MessageBox(nullptr, err.what(), "Error", MB_OK);
				return 0;
			}
		}
		else
		{
			DispatchMessage(&msg);
		}
	}

	return 0;
}