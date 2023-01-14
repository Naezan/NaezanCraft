#include "../pch.h"
#include "FileMemory.h"

FileMemory::FileMemory() : rawdata(nullptr), datasize(0), in(data), out(data)
{
    //4바이트 블럭개수, 블럭id, 블럭개수
    //블럭 입력 순서는 x -> z -> y 루프는 y -> z -> x
}