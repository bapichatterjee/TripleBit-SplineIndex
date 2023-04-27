#include "SplineIndex.h"
#include "include/rs/serializer.h"
#include "include/rs/multi_map.h"

#include "MMapBuffer.h"
#include "MemoryBuffer.h"
#include "BitmapBuffer.h"
#include <iostream>
SplineIndex::SplineIndex(ChunkManager& _chunkManager, IndexType type) : 
    chunkManager(_chunkManager), indexType(type)
{
    index = NULL;
    // TODO: remove this
    vector<pair<uint64_t, char>> dummy_data = {{0ull, 'a'}};
    index = new rs::MultiMap<ID, unsigned>(dummy_data.begin(), dummy_data.end());
    // std::cout << "Spline index constructor called" << std::endl;
}

Status SplineIndex::getOffsetByID(ID id, unsigned& offset, unsigned typeID) {
    // std::cout << "Spline index getOffsetById() called" << std::endl;
    offset = 0;
    // std::cout<<index->size()<<std::endl;
    
    // std::cout<<"Searching for ID : "<<id<<' '<<typeID<<std::endl;
    // std::cout << index->size() << std::endl;
    if(index->size() == 0) {
        offset = 0;
        return OK;
    }
    auto it = index->lower_bound(id);
    if (it == index->end()) {
        return NOT_FOUND;
    }
    if (it->first >= id && it != index->begin()) {
        it--;
    }
    // std::cout << it->first<<' '<<it->second<<std::endl;
    // std::cout << mData.size()<<": "<<mData[1].first << std::endl;
    auto it2 = it + 1;
    // std::cout << "Mdata is: "<<mData.size()<<": "<<std::endl;
    // for(auto p : mData) {
    //     std::cout << p.first<<", "<<p.second<<std::endl;
    // }

    unsigned pBegin = it->second;
    unsigned pEnd = it2->second;
    int low, high, mid = 0, lastmid = 0;
    const uchar* beginPtr = NULL, *reader = NULL;

    ID x, y;
    if(chunkManager.getTripleCount(typeID) == 0)
        return NOT_FOUND;
    if(typeID == 1) {
        // std::cout << "IM OFFSET TYPE 1" << std::endl;
        // offset = 0; return OK;
        low = pBegin;
        high = pEnd;
        // if(low > high) swap(low, high);
        // std::cout<<"low: "<<low<<" high: "<<high<<std::endl;

        reader = chunkManager.getStartPtr(1) + low;
		beginPtr = chunkManager.getStartPtr(1);
		Chunk::readXId(reader, x);

        if (x == id) {
            offset = low;
			lastmid = low;
			//cout<<__FUNCTION__<<"x==id"<<endl;
			while (low > 0) {
				//x = 0;
				reader = Chunk::skipBackward(beginPtr + low);
				Chunk::readXId(reader, x);
				if (reader < beginPtr ||x < id) {
					offset = lastmid;
					return OK;
				}
				lastmid = reader - beginPtr;
				low = lastmid - 1;
			}
			offset = lastmid;
			return OK;
        }
        else if(x > id) {
            return OK;
        }
        
        while(low <= high) {
            x = 0;
            mid = low + (high - low) / 2;
			if (lastmid == mid)
				break;
			lastmid = mid;
			reader = Chunk::skipBackward(beginPtr + mid);
			mid = reader - beginPtr;
			Chunk::readXId(reader, x);
			if (x == id) {
				lastmid = mid;
                int cnt =0;
				while (mid > 0) {
					//x = 0;
					reader = Chunk::skipBackward(beginPtr + mid);
					Chunk::readXId(reader, x);
					if (reader < beginPtr || x < id) {
                        // std::cout << "YOOOOOO" <<cnt++<< std::endl;
						offset = lastmid;
						return OK;
					}
					lastmid = reader - beginPtr;
					mid = lastmid - 1;
				}
				offset = lastmid;
				return OK;
			} else if (x > id) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
        }
        
        if(mid <=0){
                offset = 0;
                return OK;
        }
        const uchar* tempoff = Chunk::readYId(Chunk::readXId(reader, x), y);
        if(x>id ){
                offset = Chunk::skipBackward(reader-1) - beginPtr;
        }else if(x == id){
                offset = reader - beginPtr;
        }else{
                Chunk::readYId(Chunk::readXId(tempoff, x), y);
                if(x <= id)
                        offset = tempoff- beginPtr;
                else
                        offset = reader-beginPtr;

        }
        return OK;
        
    }
    else {
        // return index->lower_bound()
        offset = 0;
        // std::cout << "IM OFFSET TYPE 2" << std::endl;
        return OK;
    }
    
    return OK;
}

void SplineIndex::insertEntriesIntoData(ID id, unsigned val) {
    mData.push_back({id, val});
}

Status SplineIndex::buildIndex(unsigned chunkType) {
    static int cnt = 0;
    cnt++;
    // std::cerr << "Spline index buildIndex() called: chunk: "<<chunkType << std::endl;
    const uchar *begin, *limit, *reader;
	ID x, y;
    
    if(chunkType == 1) {
        reader = chunkManager.getStartPtr(1);
        begin = reader;
        if (chunkManager.getStartPtr(chunkType) == chunkManager.getEndPtr(chunkType))
			return OK;
        x = 0;
        reader = Chunk::readXId(reader, x);
        // std::cout<<"Inserting ID: "<<x<<std::endl;
        insertEntriesIntoData(x, 0);
        reader = reader + (int)MemoryBuffer::pagesize;

        limit = chunkManager.getEndPtr(1);
        while(reader < limit) {
            x = 0;
            const uchar* temp = Chunk::skipBackward(reader);
			Chunk::readXId(temp, x);
            // std::cout<<"Inserting ID in loop: "<<x<<std::endl;
			insertEntriesIntoData(x, temp - begin);
            reader = reader + (int) MemoryBuffer::pagesize;  
            // std::cout << "ITER: "<<x<<' '<<temp-begin<<" "<<cnt<<std::endl;          
        }

        reader = Chunk::skipBackward(limit);
		x = 0;
		Chunk::readXId(reader, x);
        // std::cout<<"Inserting ID after loop: "<<x<<std::endl;
		insertEntriesIntoData(x, reader - begin);
        // std::cout << "ITER: "<<x<<' '<<reader-begin<<" "<<cnt<<std::endl;
    } 
    if(chunkManager.getStartPtr(chunkType) != chunkManager.getEndPtr(chunkType) && chunkType == 2) {
        // std::cout<<"CHUNK TYPE 2 BHAIII"<<std::endl;
        // reader = chunkManager.getStartPtr(2);
        // begin = reader;
        // // if (chunkManager.getStartPtr(chunkType) == chunkManager.getEndPtr(chunkType))
        // //     return OK;
        // x = 0; y = 0;
        // Chunk::readYId(Chunk::readXId(reader, x), y);
        // reader = reader + (int) MemoryBuffer::pagesize;

        // limit = chunkManager.getEndPtr(2);
        // while (reader < limit) {
        //     x = 0; y = 0;
        //     const uchar *temp = Chunk::skipBackward(reader);
        //     Chunk::readYId(Chunk::readXId(temp, x), y);
		// 	insertEntriesIntoData(x + y, temp - begin);

        //     reader = reader + (int) MemoryBuffer::pagesize;
        // }

        // x = y = 0;
        // reader = Chunk::skipBackward(limit);
        // Chunk::readYId(Chunk::readXId(reader, x), y);
        // insertEntriesIntoData(x + y, reader - begin);
        
    }

    try {
        index = new rs::MultiMap<ID, unsigned>(mData.begin(), mData.end());
        // std::cout<<"Checking the size of the created index: "<<index->size()<<std::endl;
        // std::cout<<mData.size()<<std::endl;
    } catch (int err) {
        return ERROR;
    }
    return OK;
}

void SplineIndex::save(MMapBuffer*& indexBuffer) {
    static int cnt = 0;
    cnt++;
    // std::cout << "Spline index save call" << std::endl;
    char* writeBuf;
    // cout << "hello1 "<<cnt<<' '<<(void*)index << ' '<<sizeof(pair<ID, unsigned> )<<' ' << endl;
    // size_t size_of_m_data = index->size() * sizeof(pair<ID, unsigned> );
    // // std::cout<<"Size of index to be saved: "<<index->size()<<std::endl;
    // //TODO: check if index is null or not
    // size_t size_of_rs_ = index->size_of_rs_();

    size_t tot_size = index->total_size();

	if (indexBuffer == NULL) {
		indexBuffer = MMapBuffer::create(string(string(DATABASE_PATH) + "/BitmapBuffer_index").c_str(), tot_size);
		writeBuf = indexBuffer->get_address();
	} else {
		size_t size = indexBuffer->getSize();
		writeBuf = indexBuffer->resize(tot_size) + size;
	}    
    index->serialize(writeBuf);
    indexBuffer->flush();
}

SplineIndex* SplineIndex::load(ChunkManager& manager, IndexType type, char* buffer, size_t& offset) {
    // std::cout << "Spline index load() called" << std::endl;
    SplineIndex* spIndex = new SplineIndex(manager, type);
            // std::cout << "idhar" << std::endl;

    char *base = buffer + offset;
    // cout << "PTR: "<<(void*)&(spIndex->index) << endl;
    char *orig = (spIndex->index)->deserialize(base);
    // cout << "IDHAR tak" << endl;
    spIndex->mData = (spIndex->index)->data_; // TODO: optimize this
    
    // offset = buffer - orig;
    // buffer = orig;

    // offset += index->total_size();
    offset = (base - buffer);
	return spIndex;

}
SplineIndex::~SplineIndex() {
    // std::cout << "Spline index destructor called" << std::endl;
}