# NaezanCraft
MineCraft Copy Naezan's Engine

- How to build using CMake(Testing)
  - 
    - git clone
  ```shell
  git clone https://github.com/Meizoa/NaezanCraft.git
  ```

    - Build (Find the folder where "CMakeList.txt" exists)
  ```shell
  mkdir BuildDir
  cd BuildDir
  cmake ..
  cmake --build .
  ```

    - git command
  ```shell
  git status -> current git status like new file, modified, deleted etc..
  git add * -> register current status
  git reset . -> reset add *
  git commit -m "Text What You Want" -> commiting..
  git push origin main -> push to main branch
  ```

- TO DO List
  - 
    - [x] ~~Create Rectangle~~
    - [x] ~~Create RenderBuffer & RenderShader~~
    - [x] ~~Create Camera & Player~~
    - [x] ~~프러스텀컬링(AABB)~~
    - [ ] 오컬루전 컬링 + 옥트리
    - [x] ~~Chunk Object Rendering~~
    - [ ] SkyBox
        * ~~Sun & Moon~~
        * Cloud
    - [ ] Map Generator
        * ~~Render Chunk When Player Move~~
        * 청크 랜덤 블록 렌더링
        * Instancing
        * 세이브 로드 청크, 월드(SaveWorld, LoadWorld, SaveChunk, LoadChunk)
        * BSP?
    - [ ] Lighting
        * 라이트컬링
        * 레이 트레이싱
        * dot
    - [?] ECS
    - [ ] Object Pool(*다른 사이드 프로젝트로 할까 생각중*)
    - [ ] Collision
        * 레이케스트
        * AABB
        * 쿼드 트리 -> 옥트리
    - [?] AI(Too Much, if i can, i do)
        * A* or JPS
        * 브레젠험
        * FSM(상태머신)

- BUG
  - 
    - [ ] 16 * 16 Chunk is 15 * 15 Chunk
