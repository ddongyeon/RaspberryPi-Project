## RaspberryPi 와 Socket 통신을 이용한 실시간 스케치 퀴즈 게임 구현
### Sketch Quiz Game Using Raspberry-Pi & Socket Programming (김근남, 김동연)

### 개발 환경
- C++
- Raspberry pi + Ubuntu
- QT
- QPainter Class


## 1. 스케치 퀴즈 게임
![image](https://user-images.githubusercontent.com/80874455/124722593-27e52c00-df45-11eb-8b48-c9672bd5c561.png)
- 문제 출제자와 정답자 2명이서 게임 진행
- 출제자가 주어지는 제시어를 화면에 마우스로 그림
- 정답자가 그림을 보고 정답을 유추
- 정답이 맞으면 출제자와 정답자의 역할을 교대


## 2. GUI 소개
![image](https://user-images.githubusercontent.com/80874455/124722371-f40a0680-df44-11eb-9e62-8fccfbe8b363.png)


## 3. 소켓 통신
![image](https://user-images.githubusercontent.com/80874455/124722809-60850580-df45-11eb-8d03-0ef02f6d94b6.png)
- 출제자와 정답자의 프로그램에서는 2개의 프로세스 실행
- 출제자의 부모 프로세스는 그림 그려지는 event에 대응하는 data를 정답자 프로그램으로 전송
- 출제자의 자식 프로세스는 상대방의 정답 여부 응답 다기
- 정답자의 부모 프로세스는 문제에 대한 정답 여부를 확인
- 정답자의 자식 프로세스는 출제자로부터 전송되는 data를 수신
![image](https://user-images.githubusercontent.com/80874455/124722943-7e526a80-df45-11eb-8f48-9e948b5ec453.png)
- socket에 포함되어 전송되는 데이터는 그리기 도구 정보, 그림을 그리는데 필요한 data들이 포함
- 수신자는 수신받은 data를 분할하여 대응하는 값으로 똑같은 그림을 그림


## 4. 실제 게임 화면
![image](https://user-images.githubusercontent.com/80874455/124724861-56640680-df47-11eb-8e5e-68928ffd1811.png)


