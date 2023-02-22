#include "SplineIndex.h"
#include <iostream>
SplineIndex(ChunkManager& _chunkManager, IndexType type) : 
    chunkManager(_chunkManager), indexType(type)
{
    std::cout << "Spline index constructor called" << std::endl;
}

Status SplineIndex::getOffsetByID(ID id, unsigned& offset, unsigned typeID) {
    std::cout << "Spline index getOffsetById() called" << std::endl;
    return OK;
}

Status SplineIndex::buildIndex(unsigned chunkType) {
    std::cout << "Spline index buildIndex() called" << std::endl;
    return OK;
}

void SplineIndex::save(MMapBuffer*& indexBuffer) {
    std::cout << "Spline index save called" << std::endl;
}

static SplineIndex* SplineIndex::load(ChunkManager& manager, IndexType type, char* buffer, size_t& offset) {
    std::cout << "Spline index load() called" << std::endl;
}
~SplineIndex() {
    std::cout << "Spline index destructor called" << std::endl;
}