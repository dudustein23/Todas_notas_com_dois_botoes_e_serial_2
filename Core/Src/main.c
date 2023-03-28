/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <ctype.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	BUTTON_PULL_DOWN = 0,
	BUTTON_PULL_UP
}BUTTON_PUPD;
typedef enum
{
	SOLTO = 0,
	AO_PRESSIONAR,
	PRESSIONADO,
	AO_SOLTAR
}ESTADO_BOTAO;
typedef struct {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	BUTTON_PUPD pull;
	uint8_t atual;
	uint8_t anterior;
	ESTADO_BOTAO estado;
} BUTTON_CONTROLLER;
typedef enum
{
	DESLIGADO=0,
	A,
	B,
	C,
}ESTADO_NOTAS;
typedef enum
{
		DESLIGA=0,
		LIGA,
} ESTADO_LIGA_DESLIGA;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ESTADO_LIGA_DESLIGA liga_desliga = DESLIGA;
BUTTON_CONTROLLER controller1;
BUTTON_CONTROLLER controller2;
#define TCMD 1
uint8_t comando;
char comando_letra = '\0';
uint8_t comando_numero = 0;
char frase1[] = "\r\nEsta tocando Do\r\n";
char frase2[] = "\r\nEsta tocando Do#\r\n";
char frase3[] = "\r\nEsta tocando Re\r\n";
char frase4[] = "\r\nDesligado\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
BUTTON_CONTROLLER start_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BUTTON_PUPD pull);
void gerenciador_botao_struct(BUTTON_CONTROLLER * controller);
void zera_notas();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	controller1 = start_controller(Button1_GPIO_Port, Button1_Pin, BUTTON_PULL_UP);
	controller2 = start_controller(Button2_GPIO_Port, Button2_Pin, BUTTON_PULL_UP);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_UART_Receive_IT(&huart2, &comando, TCMD);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
BUTTON_CONTROLLER start_controller(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BUTTON_PUPD pull){
	BUTTON_CONTROLLER controller;
	controller.GPIO_Pin = GPIO_Pin;
	controller.GPIOx = GPIOx;
	controller.pull = pull;
	controller.atual = 0;
	controller.anterior = 0;
	controller.estado = SOLTO;
	return controller;
}

void gerenciador_botao(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BUTTON_PUPD pull, uint8_t * atual, uint8_t * anterior, ESTADO_BOTAO * estado)
{
	ESTADO_BOTAO estado_botao(uint8_t atual, uint8_t * anerior);
	if ((*atual == 0) && (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) != (GPIO_PinState) pull)){
		for (int i = 0; i < 80000; i++);
		if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) != (GPIO_PinState) pull){
			*atual = 1;
		}
	}
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == (GPIO_PinState) pull) {
		*atual = 0;
	}
	*estado = estado_botao(*atual, anterior);
}

void gerenciador_botao_struct(BUTTON_CONTROLLER * controller)
{
	void gerenciador_botao(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BUTTON_PUPD pull, uint8_t * atual, uint8_t * anterior, ESTADO_BOTAO * estado);
	gerenciador_botao(controller->GPIOx, controller->GPIO_Pin, controller->pull, &(controller->atual), &(controller->anterior), &(controller->estado));
}

ESTADO_BOTAO estado_botao(uint8_t atual, uint8_t * anterior)
{
	if ((*anterior == 0) && (atual == 1)) {
		*anterior = atual;
		return AO_PRESSIONAR;
	} else if ((*anterior == 1) && (atual == 1)) {
		*anterior = atual;
		return PRESSIONADO;
	} else if ((*anterior == 1) && (atual == 0)) {
		*anterior = atual;
		return AO_SOLTAR;
	}
	*anterior = atual;
	return SOLTO;

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	void inverte_pinos();
	__HAL_TIM_CLEAR_IT(&htim10,TIM_IT_UPDATE);
	if (liga_desliga == LIGA) inverte_pinos();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	long int pegaFrequenciaTim(TIM_HandleTypeDef * htim);
	if (GPIO_Pin == controller1.GPIO_Pin){
	    gerenciador_botao_struct(&controller1);
		if (controller1.estado == AO_PRESSIONAR){
			switch(pegaFrequenciaTim(&htim10)){
			case 227:
				HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase2, (uint16_t) sizeof(frase2));
				break;
			case 293:
				HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase3, (uint16_t) sizeof(frase3));
				break;
			case 261:
			default:
				HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase1, (uint16_t) sizeof(frase1));
				break;
			}
			liga_desliga = LIGA;
		}
	}
	if (GPIO_Pin == controller2.GPIO_Pin){
		gerenciador_botao_struct(&controller2);
		if (controller2.estado == AO_PRESSIONAR){
			HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase4, (uint16_t) sizeof(frase4));
			liga_desliga = DESLIGA;
			zera_notas();
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	void gerencia_estados();
	if (isalpha(comando)) {
		comando_letra = comando;
	} else if (isdigit(comando)) {
		comando_numero = comando - '0';
		gerencia_estados();
	}
	HAL_UART_Receive_IT(&huart2, &comando, TCMD);
}

void gerencia_estados()
{
	void setaFrequenciaTim(TIM_HandleTypeDef * htim, int frequencia);

	switch(comando_numero){
	case 1:
		setaFrequenciaTim(&htim10, 261);
		HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase1, (uint16_t) sizeof(frase1));
		break;
	case 2:
		setaFrequenciaTim(&htim10, 227);
		HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase2, (uint16_t) sizeof(frase2));
		break;
	case 3:
		setaFrequenciaTim(&htim10, 293);
		HAL_UART_Transmit_IT(&huart2, (uint8_t *) frase3, (uint16_t) sizeof(frase3));
		break;
	default:
		break;
	}
}

void setaFrequenciaTim(TIM_HandleTypeDef * htim, int frequencia)
{
	double temp = 84000000/(frequencia * 2);
	double arr = temp / (htim->Instance->PSC + 1) - 1;
	__HAL_TIM_SET_COUNTER(htim, 0);
	__HAL_TIM_SET_AUTORELOAD(htim, arr);
}

long int pegaFrequenciaTim(TIM_HandleTypeDef * htim)
{
	return (long int) 84000000/((htim->Instance->PSC + 1) * (htim->Instance->ARR + 1)) / 2;
}

void inverte_pinos()
{
	HAL_GPIO_TogglePin(Audio_GPIO_Port, Audio_Pin);
}

void zera_notas()
{
	HAL_GPIO_WritePin(Audio_GPIO_Port, Audio_Pin, 0);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
