#pragma once

class Chunk;

class ChunkThread
{
public:
	ChunkThread();
	~ChunkThread();

	void RunChunkThread();
	void SerializeChunk();
	void BeginThread();

public:
	std::vector<std::weak_ptr<Chunk>> saveChunks;
private:
	std::thread chunkThread;
	std::condition_variable chunkCV;
	std::mutex chunkMutex;
	bool IsThreadRunning;
};