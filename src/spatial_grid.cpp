#include "spatial_grid.hpp"
#include <cmath>

SpatialGrid::SpatialGrid(float width, float height, float cell_size)
    : width_(width), height_(height), cell_size_(cell_size) {

    grid_width_ = static_cast<int>(std::ceil(width / cell_size));
    grid_height_ = static_cast<int>(std::ceil(height / cell_size));

    cells_.resize(grid_width_ * grid_height_);
}

void SpatialGrid::clear() {
    for (auto& cell : cells_) {
        cell.clear();
    }
}

void SpatialGrid::insert(size_t particle_idx, float x, float y) {
    int cell_x = get_cell_x(x);
    int cell_y = get_cell_y(y);

    if (cell_x >= 0 && cell_x < grid_width_ && cell_y >= 0 && cell_y < grid_height_) {
        int idx = get_cell_index(cell_x, cell_y);
        cells_[idx].push_back(particle_idx);
    }
}

void SpatialGrid::query_neighbors(float x, float y, std::vector<size_t>& neighbors) {
    neighbors.clear();

    int cell_x = get_cell_x(x);
    int cell_y = get_cell_y(y);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = cell_x + dx;
            int ny = cell_y + dy;

            if (nx >= 0 && nx < grid_width_ && ny >= 0 && ny < grid_height_) {
                int idx = get_cell_index(nx, ny);
                for (size_t particle_idx : cells_[idx]) {
                    neighbors.push_back(particle_idx);
                }
            }
        }
    }
}

int SpatialGrid::get_cell_x(float x) const {
    return static_cast<int>(x / cell_size_);
}

int SpatialGrid::get_cell_y(float y) const {
    return static_cast<int>(y / cell_size_);
}

int SpatialGrid::get_cell_index(int cell_x, int cell_y) const {
    return cell_y * grid_width_ + cell_x;
}

void SpatialGrid::insert_parallel(const std::vector<float>& x, const std::vector<float>& y, size_t count) {
    clear();

    std::vector<int> cell_counts(cells_.size(), 0);

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(count); ++i) {
        int cell_x = get_cell_x(x[i]);
        int cell_y = get_cell_y(y[i]);

        if (cell_x >= 0 && cell_x < grid_width_ && cell_y >= 0 && cell_y < grid_height_) {
            int idx = get_cell_index(cell_x, cell_y);
#pragma omp atomic
            cell_counts[idx]++;
        }
    }

    for (size_t i = 0; i < cells_.size(); ++i) {
        cells_[i].reserve(cell_counts[i]);
    }

    for (size_t i = 0; i < count; ++i) {
        int cell_x = get_cell_x(x[i]);
        int cell_y = get_cell_y(y[i]);

        if (cell_x >= 0 && cell_x < grid_width_ && cell_y >= 0 && cell_y < grid_height_) {
            int idx = get_cell_index(cell_x, cell_y);
            cells_[idx].push_back(i);
        }
    }
}