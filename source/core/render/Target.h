#pragma once

#include <imgui.h>
#include <raylib.h>

#include "utils/Logger.h"
#include "utils/Macros.h"

namespace core::render
{
    class Target
    {
    private:
        int _width;
        int _height;
        RenderTexture2D _texture;

        // No point of creating copies of this object.
        __noncopyable(Target);
        __nonmovable(Target);

    public:
        int width() { return _width; }
        int height() { return _height; }

        Target()
        {
            _width = GetScreenWidth();
            _height = GetScreenHeight();

            _texture = LoadRenderTexture(_width, _height);

            linfo("Created render target of size=%dx%d", _width, _height);
        }

        ~Target()
        {
            UnloadRenderTexture(_texture);
        }

        // This function let us not think about starting and ending texture mode.
        template <typename TCallback>
        void use_and_do(TCallback callback)
        {
            BeginTextureMode(_texture);
            callback();
            EndTextureMode();
        }

        void resize_to_fit(int width, int height)
        {
            if (_width != width || _height != height)
            {

                UnloadRenderTexture(_texture);

                _width = width;
                _height = height;

                _texture = LoadRenderTexture(_width, _height);

                linfo("Render target resized with size=%dx%d", _width, _height);
            }
        }

        void display(double scale)
        {
            ImGui::Image(reinterpret_cast<void *>(_texture.texture.id), ImVec2(_width * scale, _height * scale));
        }

        Texture2D &underlying_texture()
        {
            return _texture.texture;
        }

        RenderTexture2D &underlying_render_texture()
        {
            return _texture;
        }
    };
} // namespace core::render