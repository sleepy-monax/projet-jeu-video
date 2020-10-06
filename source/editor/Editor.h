#pragma once

#include <raylib.h>

#include "core/components/Acceleration.h"
#include "core/components/Position.h"
#include "core/components/Velocity.h"
#include "core/debug/FPSCounter.h"
#include "core/debug/Probe.h"
#include "core/game/RenderContext.h"
#include "core/game/UpdateContext.h"
#include "core/glue/Glue.h"
#include "core/glue/ImGuiExtension.h"
#include "core/world/World.h"

#include "editor/Panel.h"

namespace editor
{
    class Editor
    {
    private:
        int _view_port_width = 128;
        int _view_port_height = 128;

        core::world::World &_world;
        core::game::RenderContext _render_context{};

        core::debug::FPSCounter fps_counter{};

        core::debug::Probe frame_time_probe{"Frame Time"};
        core::debug::Probe update_probe{"Update"};
        core::debug::Probe render_probe{"Render"};
        core::debug::Probe display_probe{"Display"};

        utils::Vector<utils::OwnPtr<Panel>> _panels{};

    public:
        auto &world() { return _world; }
        auto &entities() { return _world.entities(); }
        auto &system() { return _world.systems(); }
        auto &registry() { return _world.registry(); }

        Editor(core::world::World &world)
            : _world(world)
        {
        }

        ~Editor() {}

        template <typename TPanel, typename... TArgs>
        TPanel &open(TArgs &&... args)
        {
            auto ptr = utils::own<TPanel>(*this, std::forward<TArgs>(args)...);
            auto &ref = *ptr;
            _panels.push_back(ptr);
            return ref;
        }

        void run()
        {
            frame_time_probe.mesure_time([&]() {
                _render_context.resize_to_fit(_view_port_width, _view_port_height);

                update_probe.mesure_time([&]() {
                    update();
                });

                render_probe.mesure_time([&]() {
                    render();
                });

                display_probe.mesure_time([&]() {
                    display();
                });
            });
        }

        void update()
        {
            core::game::UpdateContext context{GetFrameTime(), GetTime()};
            _world.update(context);

            _panels.foreach ([&](auto &panel) {
                panel->update(context);

                return utils::Iteration::CONTINUE;
            });
        }

        void render()
        {
            _render_context.clear();

            _world.render(_render_context);

            _panels.foreach ([](auto &panel) {
                panel->render();

                return utils::Iteration::CONTINUE;
            });

            _render_context.compose();
        }

        void display()
        {
            ImGui::DockSpaceOverViewport();

            _panels.foreach ([](auto &panel) {
                panel->do_display();

                return utils::Iteration::CONTINUE;
            });

            ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::SetScrollX(0);
            ImGui::SetScrollY(0);

            ImGui::GetWindowSize();
            auto size = ImGui::GetWindowSize();

            _view_port_width = size.x;
            _view_port_height = size.y;

            ImGui::Viewport(reinterpret_cast<void *>(_render_context.composite().underlying_texture().id), ImVec2(_render_context.composite().width(), _render_context.composite().height()));

            ImGui::End();

            ImGui::Begin("Profiler");
            inspect(fps_counter);
            inspect(frame_time_probe);
            inspect(update_probe);
            inspect(render_probe);
            inspect(display_probe);
            ImGui::End();

            _render_context.display();
        }
    };
} // namespace editor