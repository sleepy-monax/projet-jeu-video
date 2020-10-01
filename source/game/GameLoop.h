#pragma once

#include <raylib.h>

#include "debug/FPSCounter.h"
#include "debug/Profiler.h"
#include "game/RenderContext.h"
#include "game/UpdateContext.h"
#include "glue/Glue.h"
#include "world/World.h"

namespace game
{
    class GameLoop
    {
    private:
        bool _debugging = false;

        world::World &_world;
        RenderContext _render_context{};

        debug::FPSCounter fps_counter{};

        debug::Profiler frame_time_profiler{"Frame Time"};
        debug::Profiler update_profiler{"Update"};
        debug::Profiler render_profiler{"Render"};
        debug::Profiler display_profiler{"Display"};

    public:
        GameLoop(world::World &world)
            : _world(world)
        {
        }

        ~GameLoop() {}

        void run()
        {
            while (!glue::should_exit())
            {
                if (IsKeyPressed(KEY_F3))
                {
                    _debugging = !_debugging;
                }

                glue::begin_frame();
                debug::Profiler::new_frame();

                frame_time_profiler.mesure([&]() {
                    update_profiler.mesure([&]() {
                        update();
                    });

                    render_profiler.mesure([&]() {
                        render();
                    });

                    display_profiler.mesure([&]() {
                        if (_debugging)
                            display();
                    });
                });

                glue::end_frame();
            }
        }

        void update()
        {
            UpdateContext context{GetFrameTime(), GetTime()};
            _world.update(context);
        }

        void render()
        {
            _render_context.use_and_do([&]() {
                _world.render(_render_context);
            });
        }

        void display()
        {
            ImGui::Begin("Profiler");
            fps_counter.mesure_and_display();
            frame_time_profiler.display();
            update_profiler.display();
            render_profiler.display();
            display_profiler.display();
            ImGui::End();

            _render_context.display();
            _world.display();
        }
    };
} // namespace game