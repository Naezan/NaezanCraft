#pragma once

#ifdef EDITOR_ENGINE_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif // ENGINE_DLL