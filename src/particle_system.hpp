#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

#include <vector>
#include <cstdint>
#include "spatial_grid.hpp"

struct ParticleSystem {
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<uint8_t> type;
    size_t count;

    ~ParticleSystem() { cleanup(); }

    float attraction_matrix[10][10];
    int num_types;

    SpatialGrid* grid;

    void init(size_t num_particles, int num_types, float world_width, float world_height);
    void reinit(size_t num_particles, int num_types);
    void cleanup();
    void update(float dt);
    void set_attraction(int type1, int type2, float value);
    void randomize_rules();
    void reset_particles();
    void apply_mouse_force(float mouse_x, float mouse_y, float force_strength, float radius);

private:
    void apply_forces();
    float world_width_;
    float world_height_;
};

#endif