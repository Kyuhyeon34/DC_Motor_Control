#include "device_driver.h"
#include "timer.h"
#include "key.h"
#include "motor.h"
#include <stdio.h>

static void Sys_Init(int baud);
static void On_Uart_Input(MotorController *motor, char c);
static void On_Short_Click(MotorController *motor);
static void On_Long_Click(MotorController *motor);

void (*key_event_funcs[])(MotorController *) = {
	NULL,
	On_Short_Click,
	On_Long_Click};

void Main(void)
{
	MotorController motor = {
		.state = MOTOR_STATE_STOP,
		.last_dir = MOTOR_STATE_FORWARD,
		.duty = MOTOR_BASE_DUTY};
	KeyEvent key_evt;

	Sys_Init(115200);
	Timer_Init();
	Key_Init();
	Motor_Init();

	printf("\n====================================\n");
	printf(" State-Based DC Motor Controller\n");
	printf(" - Base Duty: %d%%, Step: %d%%\n", MOTOR_BASE_DUTY, MOTOR_DUTY_STEP);
	printf("====================================\n");

	// 초기 상태: 모터 정지
	Motor_Update(&motor);

	for (;;)
	{
		// UART 입력 처리
		char c = Uart2_Get_Pressed();
		if (c != 0)
		{
			On_Uart_Input(&motor, c);
		}

		// 스위치 이벤트 처리
		key_evt = Key_Get_Event();
		if (key_evt != KEY_EVENT_NONE && key_event_funcs[key_evt] != NULL)
		{
			key_event_funcs[key_evt](&motor);
		}
	}
}

static void Sys_Init(int baud)
{
	SCB->CPACR |= (0x3 << 10 * 2) | (0x3 << 11 * 2);
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
}

static void On_Uart_Input(MotorController *motor, char c)
{
	// 속도 설정
	if (c >= '1' && c <= '9')
	{
		motor->duty = MOTOR_BASE_DUTY + (c - '1') * MOTOR_DUTY_STEP;
		printf("[UART] Speed Level: %c -> Duty: %d%%\n", c, motor->duty);
		Motor_Update(motor);
	}
	// 모터 상태 제어
	else
	{
		switch (c)
		{
		case 'S':
		case 's':
			motor->state = MOTOR_STATE_STOP;
			printf("[UART] Command: STOP\n");
			Motor_Update(motor);
			break;

		case 'F':
		case 'f':
			motor->state = MOTOR_STATE_FORWARD;
			motor->last_dir = MOTOR_STATE_FORWARD;
			printf("[UART] Command: FORWARD (Duty: %d%%)\n", motor->duty);
			Motor_Update(motor);
			break;

		case 'R':
		case 'r':
			motor->state = MOTOR_STATE_REVERSE;
			motor->last_dir = MOTOR_STATE_REVERSE;
			printf("[UART] Command: REVERSE (Duty: %d%%)\n", motor->duty);
			Motor_Update(motor);
			break;

		default:
			// 그 외 키 입력은 무시
			break;
		}
	}
}

static void On_Short_Click(MotorController *motor)
{
	switch (motor->state)
	{
	case MOTOR_STATE_STOP:
		motor->state = motor->last_dir;
		break;

	case MOTOR_STATE_FORWARD:
		motor->state = MOTOR_STATE_REVERSE;
		motor->last_dir = MOTOR_STATE_REVERSE;
		break;

	case MOTOR_STATE_REVERSE:
		motor->state = MOTOR_STATE_FORWARD;
		motor->last_dir = MOTOR_STATE_FORWARD;
		break;

	default:
		motor->state = motor->last_dir;
		break;
	}

	Motor_Update(motor);
}

static void On_Long_Click(MotorController *motor)
{
	motor->state = MOTOR_STATE_STOP;
	Motor_Update(motor);
}
