#include "../../pch.h"
#include "../../World/Block.h"
#include "../../World/Chunk.h"
#include "../Application.h"
#include "Ray.h"

Block Ray::BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir, glm::vec3& outPosition)
{
	std::weak_ptr<Chunk> curChunk;
	Block block(BlockType::Air);
	GET_World()->GetChunkByPos(std::make_pair(static_cast<int>(ray_start.x / CHUNK_X), static_cast<int>(ray_start.z / CHUNK_Z)), curChunk);

	if (Chunk::IsEmptyChunk(curChunk))
		return block;

	glm::vec3 blockPos;
	//청크의 블럭 로컬위치(내림처리)
	blockPos.x = static_cast<int>(ray_start.x) % CHUNK_X;
	blockPos.y = static_cast<int>(ray_start.y) % CHUNK_Y;
	blockPos.z = static_cast<int>(ray_start.z) % CHUNK_Z;

	float stepX = dir.x >= 0 ? 1 : -1;
	float stepY = dir.y >= 0 ? 1 : -1;
	float stepZ = dir.z >= 0 ? 1 : -1;

	//다음에 검사할 블럭 월드 위치
	float nextX = (blockPos.x + stepX) + curChunk.lock()->position.x * CHUNK_X;
	float nextY = (blockPos.y + stepY);
	float nextZ = (blockPos.z + stepZ) + curChunk.lock()->position.z * CHUNK_Z;

	float tMaxX = (nextX - ray_start.x) / dir.x;//다음블럭위치 - 광선의 월드위치
	float tMaxY = (nextY - ray_start.y) / dir.y;
	float tMaxZ = (nextZ - ray_start.z) / dir.z;

	float tDeltaX = stepX / dir.x;//한칸 전진할때의 대각선의 길이 단순히 dir의 역수로 이루어진다는걸 계산을 조금해보면 알 수 있다.
	float tDeltaY = stepY / dir.y;
	float tDeltaZ = stepZ / dir.z;

	float fDistance = 0;
	//RayBlock 만약에 광선범위를 벗어나거나 블럭을 찾으면 반복문을 빠져온다
	while (block.IsFluid() && fDistance < 10) {
		if (tMaxX < tMaxY) {
			//x가 제일 작다는 건 x면을 먼저 닿았다는 뜻
			if (tMaxX < tMaxZ) {
				blockPos.x += stepX;
				fDistance = tMaxX;
				tMaxX += tDeltaX;
				if (blockPos.x >= CHUNK_X || blockPos.x < 0)
				{
					//x방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
					return block;
				}
			}
			//z면을 닿았다는 뜻
			else {
				blockPos.z += stepZ;
				fDistance = tMaxZ;
				tMaxZ += tDeltaZ;
				if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
				{
					//z방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
					return block;
				}
			}
		}
		else {
			//y면을 닿았다는 뜻
			if (tMaxY < tMaxZ) {
				blockPos.y += stepY;
				fDistance = tMaxY;
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
				fDistance = tMaxZ;
				tMaxZ += tDeltaZ;
				if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
				{
					//z방향에 있는 다음청크로 변경, 다음청크가 없으면 return block
					return block;
				}
			}
		}

		block = curChunk.lock()->GetBlock(blockPos.x, blockPos.y, blockPos.z);
	}

	if (block.blockType != Air)
		outPosition = glm::vec3(blockPos.x + curChunk.lock()->position.x * CHUNK_X, blockPos.y, blockPos.z + curChunk.lock()->position.z * CHUNK_Z);

	return block;
}