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
  mkdir WhatYouWantBuildDirectory
  cd WhatYouWantBuildDirectory
  cmake ..
  cmake --build .
  #Only Debug is possible, later add Release Build, maybe?
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
    - [ ] 오컬루전 컬링 + 옥트리(조금 더 추가적인 연산을 필요로 하기에 적절하게 써야함)
    - [x] ~~Chunk Object Rendering~~
    - [ ] 블럭 방향에 따라 다른 텍스처 로드
    - [ ] SkyBox
        * ~~Sun & Moon~~
        * Cloud
    - [ ] Map Generator
        * ~~Render Chunk When Player Move~~
        * ~~청크 랜덤 블록 렌더링~~
        * 세이브 로드 청크, 월드(SaveWorld, LoadWorld, SaveChunk, LoadChunk)
        * World Objects(like tree, catus etc..)
        * 그리디 메싱 + 비등방성문제
        * *Instancing(메모리관리 후 검토)*
        * *BSP(Maybe Cave? or Town? but i am not implement cave and town)*
        * *Region Weather System(later if possible)*
    - [x] Lighting
        * ~~Ambient Occlusion(+라이트컬링)~~
        * ~~Sun Light~~
        * ~~Object Light(Flood fill Algorithm)~~
        * *레이 트레이싱*
        * *DotProduct*
    - [?] ECS
    - [?] Object Pool(*다른 사이드 프로젝트로 할까 생각중*)
    - [ ] Collision
        * 레이케스트
        * AABB
        * delete and emplace Block
        * 쿼드 트리 -> 옥트리
    - [?] AI(Too Much, if i can, i do)
        * A* or JPS
        * 브레젠험
        * FSM(상태머신)

- BUG
  - 
    - [x] ~~16 * 16 Chunk is 15 * 15 Chunk~~
    - [x] ~~새로운 청크가 생성될때 기존의 청크의 벽면이 로드된 상태 등 서순설정이 필요함~~
    - [x] ~~라이팅 맵 생성중 프레임 드랍 발생~~
      ![라이팅맵 프로파일링 결과](./Screenshot/LightMapOptimize.PNG)
    - [ ] 최적화중에 CPU가 Lock에 의해서 많은 시간 놀고있는것을 발견했다 왜일까?
      ![약80%의 속도를 향상시켰다 하지만 CPU가 논다](./Screenshot/Optimize1.PNG)