# stm32-motor-project
STM32 DC Motor Controller Team Project
자세한 구현 내용 참고 : "https://blushing-addition-5ca.notion.site/v3-0-3a51d46c18fc80859512f7798826be27?pvs=74"

# 큰 글씨
**두꺼운 글씨**
## 적당히 큰 글씨 

# STM32F411RE DC Motor Control Project

## 1. 프로젝트 개요

본 프로젝트는 STM32F411RE 보드에서 다음 기능을 수행하는 DC 모터 제어 시스템이다.

- DC 모터 정회전
- DC 모터 역회전
- DC 모터 정지
- PWM 듀티비를 이용한 속도 제어
- 사용자 버튼의 숏클릭 및 롱클릭 처리
- UART 명령을 이용한 모터 상태 및 속도 제어
- 상태 머신 기반 모터 제어
- 타이머 인터럽트를 이용한 시간 측정

---

# 2. 사용 MCU 및 주변장치

| 구분 | 사용 장치 | 용도 |
| --- | --- | --- |
| MCU | STM32F411RE | 전체 시스템 제어 |
| GPIOA | PA0, PA1 | 모터 드라이버 방향 및 PWM 출력 |
| GPIOA | PA2, PA3 | USART2 TX/RX |
| GPIOC | PC13 | 사용자 버튼 입력 |
| TIM2 | General Purpose Timer | 버튼 롱클릭 시간 측정 |
| TIM4 | General Purpose Timer | 1ms 시스템 틱 생성 |
| TIM5 | General Purpose Timer | 모터 PWM 출력 |
| USART2 | UART 통신 장치 | PC와 명령 및 상태 메시지 송수신 |
| RCC | Reset and Clock Control | GPIO, Timer, UART 클럭 활성화 |
| NVIC | Interrupt Controller | Timer 및 UART 인터럽트 관리 |
| FLASH | Flash Controller | 시스템 클럭에 맞는 Flash 접근 설정 |
| SCB | System Control Block | FPU 활성화 |

---

# 3. 사용 핀
