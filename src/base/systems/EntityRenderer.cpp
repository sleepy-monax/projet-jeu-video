#include <raylib.h>

#include "core/Graphics.h"
#include "core/Registry.h"
#include "core/World.h"

#include "base/components/Position.h"
#include "base/components/Sprite.h"
#include "base/systems/EntityRenderer.h"

namespace base
{
    struct TextureDestinationPosition
    {
        core::Texture texture;
        utils::Rectf destination;
        Position position;
    };

    void EntityRenderer::render(core::World &world, core::Camera &camera)
    {
        auto view = world.entities().view<Position, Sprite>();

        utils::Vector<TextureDestinationPosition> on_screen_sprites{};

        view.each([&](auto &position, auto &sprite) {
            auto tex = sprite.texture;

            auto destination = tex.bound().moved(position.pos2d() - tex.bound().bottom_center());

            if (destination.colide_with(camera.bound_world()))
            {
                on_screen_sprites.push_back({tex, destination, position});
            }
        });

        if (on_screen_sprites.count() > 0)
        {

            for (size_t i = 0; i < on_screen_sprites.count() - 1; i++)
            {
                for (size_t j = i + 1; j < on_screen_sprites.count(); j++)
                {
                    if (on_screen_sprites[i].position.y > on_screen_sprites[j].position.y)
                    {
                        std::swap(on_screen_sprites[i], on_screen_sprites[j]);
                    }
                }
            }

            camera.with_entities([&]() {
                for (size_t i = 0; i < on_screen_sprites.count(); i++)
                {
                    core::draw_texture(on_screen_sprites[i].texture, on_screen_sprites[i].destination, WHITE);
                }
            });
        }
    }
} // namespace base
