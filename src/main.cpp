#include <cstdio>
#include <SDL3/SDL.h>
#include "particle_system.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <cmath>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

SDL_Texture* create_circle_texture(SDL_Renderer* renderer, int radius) {
    int diameter = radius * 2 + 2;
    SDL_Surface* surface = SDL_CreateSurface(diameter, diameter, SDL_PIXELFORMAT_RGBA8888);

    SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, 0, 0, 0, 0));

    Uint32* pixels = (Uint32*)surface->pixels;
    float center = diameter / 2.0f;

    for (int y = 0; y < diameter; y++) {
        for (int x = 0; x < diameter; x++) {
            float dx = x - center + 0.5f;
            float dy = y - center + 0.5f;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist <= radius) {
                float alpha = 1.0f;
                if (dist > radius - 1.0f) {
                    alpha = radius - dist;
                }
                Uint8 a = (Uint8)(alpha * 255);
                pixels[y * diameter + x] = SDL_MapSurfaceRGBA(surface, 255, 255, 255, a);
            }
        }
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_DestroySurface(surface);

    return texture;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Nucleon", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* particle_texture = create_circle_texture(renderer, 4);
    SDL_Texture* glow_texture = create_circle_texture(renderer, 8);
    SDL_SetTextureBlendMode(glow_texture, SDL_BLENDMODE_ADD);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ParticleSystem particles;
    particles.init(5000, 3, WINDOW_WIDTH, WINDOW_HEIGHT);

    particles.set_attraction(0, 0, -0.32f);
    particles.set_attraction(0, 1, -0.17f);
    particles.set_attraction(0, 2, 0.34f);
    particles.set_attraction(1, 1, -0.1f);
    particles.set_attraction(1, 2, -0.34f);
    particles.set_attraction(1, 0, 0.15f);
    particles.set_attraction(2, 2, 0.15f);
    particles.set_attraction(2, 0, -0.2f);
    particles.set_attraction(2, 1, 0.1f);

    std::vector<SDL_FRect> yellow_rects, red_rects, green_rects;
    yellow_rects.reserve(particles.count / 3);
    red_rects.reserve(particles.count / 3);
    green_rects.reserve(particles.count / 3);

    bool running = true;
    SDL_Event event;

    static int config_num_particles = 5000;
    static int config_num_types = 3;
    static bool needs_reinit = false;
    static float attraction_values[36] = {-0.32f, -0.17f, 0.34f, -0.1f, -0.34f, 0.15f, 0.15f, -0.2f, 0.1f};
    static float mouse_force_strength = 5.0f;
    static float mouse_force_radius = 150.0f;
    static float simulation_speed = 1.0f;
    static float particle_size = 2.0f;
    static bool enable_glow = false;
    static bool enable_trail = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.FrameRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.35f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.42f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.35f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.55f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5f, 0.7f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(380, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("Nucleon", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SeparatorText("Statistics");
        ImGui::Text("Particles: %zu", particles.count);
        ImGui::Text("Types: %d", particles.num_types);
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "FPS: %.1f", io.Framerate);

        ImGui::Spacing();
        ImGui::SeparatorText("Configuration");

        if (ImGui::CollapsingHeader("Simulation Settings")) {
            ImGui::SliderInt("Particle Count", &config_num_particles, 1000, 10000);
            ImGui::SliderInt("Particle Types", &config_num_types, 1, 6);

            if (ImGui::Button("Apply Changes", ImVec2(360, 0))) {
                needs_reinit = true;
            }
        }

        if (needs_reinit) {
            particles.reinit(config_num_particles, config_num_types);

            for (int i = 0; i < config_num_types; ++i) {
                for (int j = 0; j < config_num_types; ++j) {
                    attraction_values[i * 6 + j] = particles.attraction_matrix[i][j];
                }
            }

            needs_reinit = false;
        }

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Attraction Rules")) {
            const char* type_names[] = {"Blue", "Red", "Purple", "Yellow", "Green", "Orange"};

            for (int i = 0; i < particles.num_types; ++i) {
                for (int j = 0; j < particles.num_types; ++j) {
                    char label[64];
                    snprintf(label, sizeof(label), "%s -> %s", type_names[i], type_names[j]);
                    ImGui::SliderFloat(label, &attraction_values[i * 6 + j], -1.0f, 1.0f);
                    particles.set_attraction(i, j, attraction_values[i * 6 + j]);
                }
            }
        }

        if (ImGui::Button("Randomize Rules", ImVec2(175, 0))) {
            particles.randomize_rules();
            for (int i = 0; i < particles.num_types; ++i) {
                for (int j = 0; j < particles.num_types; ++j) {
                    attraction_values[i * 6 + j] = particles.attraction_matrix[i][j];
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset Particles", ImVec2(175, 0))) {
            particles.reset_particles();
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Interaction");

        ImGui::SliderFloat("Mouse Force", &mouse_force_strength, 0.0f, 25.0f);
        ImGui::SliderFloat("Mouse Radius", &mouse_force_radius, 10.0f, 1000.0f);
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Left Click = Repel | Right Click = Attract");

        ImGui::Spacing();
        ImGui::SeparatorText("Visual");

        ImGui::SliderFloat("Simulation Speed", &simulation_speed, 0.0f, 10.0f);
        ImGui::SliderFloat("Particle Size", &particle_size, 1.0f, 5.0f);

        ImGui::Spacing();
        ImGui::Checkbox("Glow Effect", &enable_glow);
        ImGui::SameLine(200);
        ImGui::Checkbox("Trail Effect", &enable_trail);

        static float last_particle_size = 2.0f;
        if (particle_size != last_particle_size) {
            SDL_DestroyTexture(particle_texture);
            SDL_DestroyTexture(glow_texture);
            particle_texture = create_circle_texture(renderer, (int)particle_size);
            glow_texture = create_circle_texture(renderer, (int)(particle_size * 2));
            SDL_SetTextureBlendMode(glow_texture, SDL_BLENDMODE_ADD);
            last_particle_size = particle_size;
        }

        ImGui::End();

        Uint32 mouse_state = SDL_GetMouseState(nullptr, nullptr);
        float mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        if (mouse_state & SDL_BUTTON_LMASK) {
            particles.apply_mouse_force(mouse_x, mouse_y, -mouse_force_strength, mouse_force_radius);
        }
        if (mouse_state & SDL_BUTTON_RMASK) {
            particles.apply_mouse_force(mouse_x, mouse_y, mouse_force_strength, mouse_force_radius);
        }

        if (simulation_speed > 0.01f) {
            particles.update(0.016f * simulation_speed);
        }

        if (enable_trail) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 10);
            SDL_FRect screen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(renderer, &screen);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        std::vector<std::vector<SDL_FRect>> color_rects(particles.num_types);
        for (auto& vec : color_rects) {
            vec.reserve(particles.count / particles.num_types);
        }

        for (size_t i = 0; i < particles.count; ++i) {
            SDL_FRect rect = {particles.x[i], particles.y[i], particle_size, particle_size};
            color_rects[particles.type[i]].push_back(rect);
        }

        SDL_Color colors[] = {
            {0, 0, 255, 255},
            {255, 0, 0, 255},
            {255, 0, 255, 255},
            {255, 255, 0, 255},
            {0, 255, 0, 255},
            {255, 165, 0, 255}
        };

        for (int t = 0; t < particles.num_types; ++t) {
            if (enable_glow) {
                SDL_SetTextureColorMod(glow_texture, colors[t].r, colors[t].g, colors[t].b);
                SDL_SetTextureAlphaMod(glow_texture, 60);

                for (const auto& rect : color_rects[t]) {
                    SDL_FRect dest = {rect.x - particle_size * 2, rect.y - particle_size * 2, particle_size * 4, particle_size * 4};
                    SDL_RenderTexture(renderer, glow_texture, nullptr, &dest);
                }
            }

            SDL_SetTextureColorMod(particle_texture, colors[t].r, colors[t].g, colors[t].b);
            SDL_SetTextureAlphaMod(particle_texture, 255);

            for (const auto& rect : color_rects[t]) {
                SDL_FRect dest = {rect.x - particle_size, rect.y - particle_size, particle_size * 2, particle_size * 2};
                SDL_RenderTexture(renderer, particle_texture, nullptr, &dest);
            }
        }


        // Render ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}