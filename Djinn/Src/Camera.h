#pragma once

namespace Djinn {
    class Camera {
    public:
        Camera();
        ~Camera();
    private:
        Camera(const Camera& other);
    };
}