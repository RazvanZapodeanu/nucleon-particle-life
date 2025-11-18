#ifndef SPATIAL_GRID_HPP
#define SPATIAL_GRID_HPP

#include <vector>
#include <cstdint>

class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cell_size);

    void clear();
    void insert(size_t particle_idx, float x, float y);
    void query_neighbors(float x, float y, std::vector<size_t>& neighbors);
    void insert_parallel(const std::vector<float>& x, const std::vector<float>& y, size_t count);

private:
    float width_, height_;
    float cell_size_;
    int grid_width_, grid_height_;
    std::vector<std::vector<size_t>> cells_;

    int get_cell_x(float x) const;
    int get_cell_y(float y) const;
    int get_cell_index(int cell_x, int cell_y) const;
};

#endif