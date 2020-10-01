#pragma once

#include <imgui.h>
#include <raylib.h>

#include "utils/Logger.h"
#include "utils/Macros.h"

namespace render
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

        void resize_to_fit_the_screen()
        {
            if (_width != GetScreenWidth() ||
                _height != GetScreenHeight())
            {
                _width = GetScreenWidth();
                _height = GetScreenHeight();

                UnloadRenderTexture(_texture);

                _texture = LoadRenderTexture(_width, _height);

                linfo("Render target resized with size=%dx%d", _width, _height);
            }
        }

        void display()
        {
            ImGui::Image(reinterpret_cast<void *>(_texture.texture.id), ImVec2(_width / 16.0, _height / 16.0));
        }

        RenderTexture2D &underlying_texture()
        {
            return _texture;
        }
    };
} // namespace render