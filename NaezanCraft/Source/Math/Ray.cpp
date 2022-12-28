#include "../../pch.h"
#include "../../World/Block.h"
#include "../../World/Chunk.h"
#include "../Application.h"
#include "Ray.h"

Block Ray::BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir)
{
    std::weak_ptr<Chunk> curChunk;
    GET_World()->GetChunkByPos(std::make_pair(static_cast<int>(ray_start.x / CHUNK_X), static_cast<int>(ray_start.z / CHUNK_Z)), curChunk);
    glm::vec3 blockPos;
    //내림처리
    blockPos.x = static_cast<int>(ray_start.x) % CHUNK_X;
    blockPos.y = static_cast<int>(ray_start.y) % CHUNK_Y;
    blockPos.z = static_cast<int>(ray_start.z) % CHUNK_Z;

	float stepX = dir.x >= 0 ? 1 : -1;
	float stepY = dir.y >= 0 ? 1 : -1;
	float stepZ = dir.z >= 0 ? 1 : -1;

    //다음에 검사할 블럭
	float nextX = (std::round(ray_start.x) + stepX) * CHUNK_X;
	float nextY = (std::round(ray_start.y) + stepY) * CHUNK_Y;
	float nextZ = (std::round(ray_start.z) + stepZ) * CHUNK_Z;

	float tMaxX = (nextX - ray_start.x) / dir.x;//dir로 나누는 이유가 뭐지? x의 진척정도를 보여주는것 같은데 기본적인 연산보다 빨라 보인다
	float tMaxY = (nextY - ray_start.y) / dir.y;
	float tMaxZ = (nextZ - ray_start.z) / dir.z;

	float tDeltaX = CHUNK_X / dir.x * stepX;
	float tDeltaY = CHUNK_Y / dir.y * stepY;
	float tDeltaZ = CHUNK_Z / dir.z * stepZ;

    //RayBlock 만약에 광선범위를 벗어나거나 블럭을 찾으면 반복문을 빠져온다
    Block block(BlockType::Air);
    while (block.IsFluid() /*&& glm::length("distance") < 10.f*/) {
        if (tMaxX < tMaxY) {
            //x가 제일 작다는 건 x면을 먼저 닿았다는 뜻
            if (tMaxX < tMaxZ) {
                blockPos.x += stepX;
                tMaxX += tDeltaX;
                if (blockPos.x >= CHUNK_X || blockPos.x < 0)
                {
                    //x방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
                }
            }
            //z면을 닿았다는 뜻
            else {
                blockPos.z += stepZ;
                tMaxZ += tDeltaZ;
                if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
                {
                    //z방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
                }
            }
        }
        else {
            //y면을 닿았다는 뜻
            if (tMaxY < tMaxZ) {
                blockPos.y += stepY;
                tMaxY += tDeltaY;
                if (blockPos.y >= CHUNK_Y || blockPos.y < 0)
                {
                    //못찾음
                    return block;
                }
            }
            //z면을닿았다는 뜻
            else {
                blockPos.z += stepZ;
                tMaxZ += tDeltaZ;
                if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
                {
                    //z방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
                }
            }
        }

        block = curChunk.lock()->GetBlock(blockPos.x, blockPos.y, blockPos.z);
    }

    return block;
}