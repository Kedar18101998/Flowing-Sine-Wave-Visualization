#include <iostream>
#include <cmath>
#include <Windows.h>
#include <cuda_runtime.h>

const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_POINTS = WIDTH;

// CUDA kernel to calculate sine values
__global__ void calculateSine(float *sineValues, float time) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < NUM_POINTS) {
        float x = idx * 6.28f / static_cast<float>(NUM_POINTS); // Map idx to [0, 2*PI]
        sineValues[idx] = sin(x - time); // Calculate sine wave
    }
}

int main() {
    // Initialize Win32 window
    HWND hwnd = GetConsoleWindow();
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // CUDA initialization
    float *d_sineValues;
    cudaMalloc((void **)&d_sineValues, NUM_POINTS * sizeof(float));

    // Main loop
    float time = 0.0f;
    float dt = 0.05f;
    while (true) {
        // Calculate sine values using CUDA
        calculateSine<<<(NUM_POINTS + 255) / 256, 256>>>(d_sineValues, time);
        cudaMemcpy(sineValues, d_sineValues, NUM_POINTS * sizeof(float), cudaMemcpyDeviceToHost);

        // Draw sine wave
        for (int i = 0; i < NUM_POINTS - 1; ++i) {
            MoveToEx(hdc, i, (1.0f + sineValues[i]) * HEIGHT / 2, NULL);
            LineTo(hdc, i + 1, (1.0f + sineValues[i + 1]) * HEIGHT / 2);
        }

        // Update time
        time += dt;

        // Wait for a while to control the speed of animation
        Sleep(50);

        // Clear the screen
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
    }

    // Cleanup
    cudaFree(d_sineValues);
    ReleaseDC(hwnd, hdc);
    return 0;
}
