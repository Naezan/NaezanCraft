#pragma once

class Renderer;
class Scene;
class Chunk;

class World
{
public:
	World();
	~World();

	void Update();
	void Render();
	void Shutdown();

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

	bool GetChunkByPos(const std::pair<int, int>& key, std::shared_ptr<Chunk>& outChunk);

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Scene> scene;

	struct Pair_IntHash
	{
		std::size_t operator () (const std::pair<int, int>& p) const {
			std::size_t hash1 = std::hash<int>{}(p.first);
			std::size_t hash2 = std::hash<int>{}(p.second);

			if (hash1 != hash2) {
				return hash1 ^ hash2;
			}

			return hash1;
		}
	};
	//I hate c++ hash, need to chage map?
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, Pair_IntHash> worldChunks;
};