#ifndef SPLINE_INDEX_H_
#define SPLINE_INDEX_H_

#include "include/rs/multi_map.h"
#include "TripleBit.h"
#include "EntityIDBuffer.h"

class SplineIndex {
public:
    enum IndexType { SUBJECT_INDEX, OBJECT_INDEX };
    SplineIndex(ChunkManager& _chunkManager, IndexType type);
    Status buildIndex(unsigned chunkType);
    Status getOffsetByID(ID id, unsigned& offset, unsigned typeID);
    void save(MMapBuffer*& indexBuffer);
    static SplineIndex* load(ChunkManager& manager, IndexType type, char* buffer, size_t& offset);
    ~SplineIndex();
private:
    ChunkManager& chunkManager;
    IndexType indexType;
    // rs::multi_map<ID, unsigned> index;
};

#endif // SPLINE_INDEX_H_