#pragma once

class Chunk;

class ChunkThread
{
public:
	ChunkThread();

	void RunChunkThread();
	void SerializeChunk();

public:
	static std::vector<std::weak_ptr<Chunk>> saveChunks;
private:
	std::thread chunkThread;
	std::condition_variable chunkCV;
	std::mutex chunkMutex;
	bool IsThreadRunning;
};