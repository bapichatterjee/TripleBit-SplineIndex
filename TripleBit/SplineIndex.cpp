#include "SplineIndex.h"
#include "include/rs/serializer.h"
#include "MMapBuffer.h"
#include "MemoryBuffer.h"
#include "BitmapBuffer.h"
#include <iostream>
SplineIndex::SplineIndex(ChunkManager& _chunkManager, IndexType type) : 
    chunkManager(_chunkManager), indexType(type)
{
    std::cout << "Spline index constructor called" << std::endl;
}

Status SplineIndex::getOffsetByID(ID id, unsigned& offset, unsigned typeID) {
    std::cout << "Spline index getOffsetById() called" << std::endl;

    return OK;
}

void SplineIndex::insertEntriesIntoData(const ID& id, const unsigned& val) {
    mData.push_back({id, val});
}

Status SplineIndex::buildIndex(unsigned chunkType) {
    std::cerr << "Spline index buildIndex() called" << std::endl;
    const uchar *begin, *limit, *reader;
	ID x, y;

    if(chunkType == 1) {
        reader = chunkManager.getStartPtr(1);
        begin = reader;

        if (chunkManager.getStartPtr(chunkType) == chunkManager.getEndPtr(chunkType))
			return OK;
        
        x = 0;
        reader = Chunk::readXId(reader, x);
        insertEntriesIntoData(x, 0);
        reader = reader + (int)MemoryBuffer::pagesize;

        limit = chunkManager.getEndPtr(1);
        while(reader < limit) {
            x = 0;
            const uchar* temp = Chunk::skipBackward(reader);
			Chunk::readXId(temp, x);
			insertEntriesIntoData(x, temp - begin);

            reader = reader + (int) MemoryBuffer::pagesize;
        }

        reader = Chunk::skipBackward(limit);
		x = 0;
		Chunk::readXId(reader, x);
		insertEntriesIntoData(x, reader - begin);
    } 
    if(chunkType == 2) {
        // badme
        reader = chunkManager.getStartPtr(2);
        begin = reader;
        if (chunkManager.getStartPtr(chunkType) == chunkManager.getEndPtr(chunkType))
            return OK;
        x = 0; y = 0;
        Chunk::readYId(Chunk::readXId(reader, x), y);

        reader = reader + (int) MemoryBuffer::pagesize;

        limit = chunkManager.getEndPtr(2);
        while (reader < limit) {
            x = 0; y = 0;
            const uchar *temp = Chunk::skipBackward(reader);
            Chunk::readYId(Chunk::readXId(temp, x), y);
			insertEntriesIntoData(x + y, temp - begin);

            reader = reader + (int) MemoryBuffer::pagesize;
        }

        x = y = 0;
        reader = Chunk::skipBackward(limit);
        Chunk::readYId(Chunk::readXId(reader, x), y);
        insertEntriesIntoData(x + y, reader - begin);
        
    }

    // rs::MultiMap<ID, unsigned> 
    try {
        index = new rs::MultiMap<ID, unsigned>(mData.begin(), mData.end());
    } catch (int err) {
        return ERROR;
    }
    return OK;
}

void SplineIndex::save(MMapBuffer*& indexBuffer) {
    std::cout << "Spline index save called" << std::endl;
    char* writeBuf;

	if (indexBuffer == NULL) {
        //altersize later
		indexBuffer = MMapBuffer::create(string(string(DATABASE_PATH) + "/BitmapBuffer_index").c_str(),  16 * sizeof(ID));
		writeBuf = indexBuffer->get_address();
	} else {
		size_t size = indexBuffer->getSize();
        //altersize later
		// writeBuf = indexBuffer->resize(tableSize * 4 * 2 + 4 + 16 * sizeof(double) + 4 * sizeof(ID)) + size;
		writeBuf = indexBuffer->resize( 16 * sizeof(ID)) + size;
	}

    //call radix spline with writeBuf buffer
    
    indexBuffer->flush();
}

SplineIndex* SplineIndex::load(ChunkManager& manager, IndexType type, char* buffer, size_t& offset) {
    std::cout << "Spline index load() called" << std::endl;
    SplineIndex* index = new SplineIndex(manager, type);

}
SplineIndex::~SplineIndex() {
    std::cout << "Spline index destructor called" << std::endl;
}