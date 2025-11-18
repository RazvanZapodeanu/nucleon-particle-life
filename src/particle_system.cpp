#include "particle_system.hpp"
#include <random>
#include <cmath>

static const float WORLD_WIDTH = 1600.0f;
static const float WORLD_HEIGHT = 900.0f;

inline float wrap_distance(float delta, float world_size) {
    if (delta > world_size * 0.5f) {
        return delta - world_size;
    } else if (delta < -world_size * 0.5f) {
        return delta + world_size;
    }
    return delta;
}

inline float fast_inv_sqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

void ParticleSystem::init(size_t num_particles, int n_types, float world_width, float world_height) {
    count = num_particles;
    num_types = n_types;
    world_width_ = world_width;
    world_height_ = world_height;

    x.resize(count);
    y.resize(count);
    vx.resize(count);
    vy.resize(count);
    type.resize(count);

    grid = new SpatialGrid(world_width, world_height, 80.0f);

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            attraction_matrix[i][j] = 0.0f;
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_x(0.0f, world_width);
    std::uniform_real_distribution<float> pos_y(0.0f, world_height);
    std::uniform_int_distribution<int> type_dist(0, num_types - 1);

    for (size_t i = 0; i < count; ++i) {
        x[i] = pos_x(gen);
        y[i] = pos_y(gen);
        vx[i] = 0.0f;
        vy[i] = 0.0f;
        type[i] = type_dist(gen);
    }
}

void ParticleSystem::set_attraction(int type1, int type2, float value) {
    attraction_matrix[type1][type2] = value;
}

void ParticleSystem::apply_forces() {
    const float max_distance = 80.0f;
    const float max_distance_sq = max_distance * max_distance;

    grid->insert_parallel(x, y, count);

#pragma omp parallel
    {
        std::vector<size_t> neighbors;

#pragma omp for
        for (int i = 0; i < static_cast<int>(count); ++i) {
            float fx = 0.0f;
            float fy = 0.0f;

            grid->query_neighbors(x[i], y[i], neighbors);

            for (size_t j : neighbors) {
                if (static_cast<size_t>(i) == j) continue;
                float dx = wrap_distance(x[j] - x[i], WORLD_WIDTH);
                float dy = wrap_distance(y[j] - y[i], WORLD_HEIGHT);
                float dist_sq = dx * dx + dy * dy;

                if (dist_sq > 0 && dist_sq < max_distance_sq) {
                    float inv_dist = fast_inv_sqrt(dist_sq);
                    float dist = dist_sq * inv_dist;
                    float force = attraction_matrix[type[i]][type[j]] * inv_dist;
                    fx += force * dx;
                    fy += force * dy;
                }
            }

            vx[i] = (vx[i] + fx) * 0.5f;
            vy[i] = (vy[i] + fy) * 0.5f;
        }
    }
}

void ParticleSystem::update(float dt) {
    apply_forces();

    for (size_t i = 0; i < count; ++i) {
        x[i] += vx[i] * dt * 60.0f;
        y[i] += vy[i] * dt * 60.0f;

        if (x[i] < 0) x[i] += WORLD_WIDTH;
        if (x[i] > WORLD_WIDTH) x[i] -= WORLD_WIDTH;
        if (y[i] < 0) y[i] += WORLD_HEIGHT;
        if (y[i] > WORLD_HEIGHT) y[i] -= WORLD_HEIGHT;
    }
}

void ParticleSystem::randomize_rules() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rule_dist(-1.0f, 1.0f);

    for (int i = 0; i < num_types; ++i) {
        for (int j = 0; j < num_types; ++j) {
            attraction_matrix[i][j] = rule_dist(gen);
        }
    }
}

void ParticleSystem::reset_particles() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_x(0.0f, world_width_);
    std::uniform_real_distribution<float> pos_y(0.0f, world_height_);

    for (size_t i = 0; i < count; ++i) {
        x[i] = pos_x(gen);
        y[i] = pos_y(gen);
        vx[i] = 0.0f;
        vy[i] = 0.0f;
    }
}

void ParticleSystem::cleanup() {
    if (grid) {
        delete grid;
        grid = nullptr;
    }
}

void ParticleSystem::reinit(size_t num_particles, int num_types_new) {
    cleanup();
    init(num_particles, num_types_new, world_width_, world_height_);
}

void ParticleSystem::apply_mouse_force(float mouse_x, float mouse_y, float force_strength, float radius) {
    const float max_distance_sq = radius * radius;

    for (size_t i = 0; i < count; ++i) {
        float dx = wrap_distance(mouse_x - x[i], WORLD_WIDTH);
        float dy = wrap_distance(mouse_y - y[i], WORLD_HEIGHT);
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq < max_distance_sq && dist_sq > 1.0f) {
            float dist = std::sqrt(dist_sq);
            float force = (force_strength * 100.0f) / dist;
            vx[i] += force * dx / dist;
            vy[i] += force * dy / dist;
        }
    }
}