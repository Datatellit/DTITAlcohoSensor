;* 文件名          : startup_stm32f10x_hd.s
;* 库版本           : V3.5.0
;* 说明:             此文件为STM32F10x高密度设备的MDK工具链的启动文件
;* 该模块执行以下操作:
;* -设置初始堆栈指针（SP）
;* -设置初始程序计数器（PC）为复位向量，并在执行main函数前初始化系统时钟
;* -设置向量表入口为异常事件的入口地址
;* -复位之后处理器为线程模式，优先级为特权级，堆栈设置为MSP主堆栈
;*

Stack_Size      EQU     0x00000200  ;定义堆栈的大小

;AREA 命令指示汇编器汇编一个新的代码段或数据段。
;段是独立的、指定的、不可见的代码或数据块，它们由链接器处理.
;段是独立的、命名的、不可分割的代码或数据序列。一个代码段是生成一个应用程序的最低要求

;默认情况下，ELF 段在四字节边界上对齐。expression 可以拥有 0 到 31 的任何整数。
;段在 2expression 字节边界上对齐
                AREA    STACK, NOINIT, READWRITE, ALIGN=3  ;堆栈段，未初始化，允许读写，8字节边界对齐
; 说明: Cortex-M3的指令地址要求是字边界对齐（4字节）；但是代码段是8字节边界对齐的

Stack_Mem       SPACE   Stack_Size ;分配堆栈空间，把首地址赋给Stack_Mem
__initial_sp ;初始化堆栈指针，指向堆栈顶.

; 此处有个一个问题讨论，关于栈顶在RAM中所处位置问题，很多初学者一直以为是编译器特意放在HEAP段之后是有意为之，并且认为这样可以利用heap未分配空间来防止未知的栈溢出问题
; 这种理解是错误的，链接器并不会为栈的位置做特殊的处理，而且这样做也并不会利用heap段，在此文件的最后对堆栈的初始化代码中可以看出他们是两个互相独立的数据区。此处出现的现
; 象是因为MDK按数据段的字母顺序链接数据段的地址的，所以此处造成了堆的地址在栈的前面的假象，不要窃以为是有某种特殊的约定。
                                                 
; Heap Configuration
;     Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>

Heap_Size       EQU     0x00000200  ;定义堆的大小

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3 ;堆段,未初始化,允许读写,堆数据段8字节边界对齐
__heap_base
Heap_Mem        SPACE   Heap_Size  ;分配堆空间
__heap_limit    ;与__heap_base配合限制堆的大小

                PRESERVE8  ; 命令指定当前文件保持栈的八字节对齐
                THUMB      ; 指令集，THUMB 必须位于使用新语法的任何Thumb代码之前

; EXPORT 命令声明一个符号，可由链接器用于解释各个目标和库文件中的符号引用，相当于声明了一个全局变量。 GLOBAL 于 EXPORT相同。
; 以下为向量表，在复位时被映射到FLASH的0地址
                AREA    RESET, DATA, READONLY ;复位段,只包含数据，只读
                EXPORT  __Vectors             ;标号输出，中断向量表开始
                EXPORT  __Vectors_End         ;中断向量表结束
                EXPORT  __Vectors_Size        ;中断向量表大小
               
; DCD 命令分配一个或多个字的存储器，在四个字节的边界上对齐，并定义存储器的运行时初值。
__Vectors       DCD     __initial_sp               ; Top of Stack 栈顶指针，被放在向量表的开始，FLASH的0地址，复位后首先装载栈顶指针
                DCD     Reset_Handler              ; Reset Handler 复位异常，装载完栈顶后，第一个执行的，并且不返回。
                DCD     NMI_Handler                ; NMI Handler    不可屏蔽中断
                DCD     HardFault_Handler          ; Hard Fault Handler 硬件错误中断
                DCD     MemManage_Handler          ; MPU Fault Handler 内存管理错误中断
                DCD     BusFault_Handler           ; Bus Fault Handler  总线错误中断，一般发生在数据访问异常，比如fsmc访问不当
                DCD     UsageFault_Handler         ; Usage Fault Handler 用法错误中断，一般是预取值，或者位置指令，数据处理等错误
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler  系统调用异常，主要是为了调用操作系统内核服务
                DCD     DebugMon_Handler           ; Debug Monitor Handler 调试监视异常
                DCD     0                          ; Reserved
                ;DCD     PendSV_Handler             ; PendSV Handler    挂起异常，此处可以看见用作了uCOS-II的上下文切换异常，这是被推荐使用的，因为Cortex-M3会在异常发生时自动保存R0-R3，
                                          ; R12,R13(堆栈指针SP)，R14(链接地址，也叫返回地址LR，在异常返回时使用)，R15（程序计数器PC，为当前应用程序+4）和中断完成时自动回复，
                                          ;我们只需保存R4-R11,大大减少了中断响应和上下文切换的时间。
          ;说明：此处涉及到一个中断保存寄存器问题：因为在所有的运行模式下，未分组寄存器都指向同一个物理寄存器，他们未被系统用作特殊的用途，因此，在中断或者异常处理进行模式转换
          ;         时，由于不同模式（此处为"线程"和"特权"）均使用相同的物理寄存器，可能会造成寄存器中数据的破坏。这也是常说的"关键代码段"和"l临界区"保护的原因。
                ;DCD     SysTick_Handler            ; SysTick Handler  滴答定时器，为操作系统内核时钟
                DCD     OS_CPU_PendSVHandler
                DCD     OS_CPU_SysTickHandler

                ; External Interrupts                    以下为外部中断向量表
                DCD     WWDG_IRQHandler            ; Window Watchdog
                DCD     PVD_IRQHandler             ; PVD through EXTI Line detect
                DCD     TAMPER_IRQHandler          ; Tamper
                DCD     RTC_IRQHandler             ; RTC
                DCD     FLASH_IRQHandler           ; Flash
                DCD     RCC_IRQHandler             ; RCC
                DCD     EXTI0_IRQHandler           ; EXTI Line 0
                DCD     EXTI1_IRQHandler           ; EXTI Line 1
                DCD     EXTI2_IRQHandler           ; EXTI Line 2
                DCD     EXTI3_IRQHandler           ; EXTI Line 3
                DCD     EXTI4_IRQHandler           ; EXTI Line 4
                DCD     DMA1_Channel1_IRQHandler   ; DMA1 Channel 1
                DCD     DMA1_Channel2_IRQHandler   ; DMA1 Channel 2
                DCD     DMA1_Channel3_IRQHandler   ; DMA1 Channel 3
                DCD     DMA1_Channel4_IRQHandler   ; DMA1 Channel 4
                DCD     DMA1_Channel5_IRQHandler   ; DMA1 Channel 5
                DCD     DMA1_Channel6_IRQHandler   ; DMA1 Channel 6
                DCD     DMA1_Channel7_IRQHandler   ; DMA1 Channel 7
                DCD     ADC1_2_IRQHandler          ; ADC1 & ADC2
                DCD     USB_HP_CAN1_TX_IRQHandler  ; USB High Priority or CAN1 TX
                DCD     USB_LP_CAN1_RX0_IRQHandler ; USB Low  Priority or CAN1 RX0
                DCD     CAN1_RX1_IRQHandler        ; CAN1 RX1
                DCD     CAN1_SCE_IRQHandler        ; CAN1 SCE
                DCD     EXTI9_5_IRQHandler         ; EXTI Line 9..5
                DCD     TIM1_BRK_IRQHandler        ; TIM1 Break
                DCD     TIM1_UP_IRQHandler         ; TIM1 Update
                DCD     TIM1_TRG_COM_IRQHandler    ; TIM1 Trigger and Commutation
                DCD     TIM1_CC_IRQHandler         ; TIM1 Capture Compare
                DCD     TIM2_IRQHandler            ; TIM2
                DCD     TIM3_IRQHandler            ; TIM3
                DCD     TIM4_IRQHandler            ; TIM4
                DCD     I2C1_EV_IRQHandler         ; I2C1 Event
                DCD     I2C1_ER_IRQHandler         ; I2C1 Error
                DCD     I2C2_EV_IRQHandler         ; I2C2 Event
                DCD     I2C2_ER_IRQHandler         ; I2C2 Error
                DCD     SPI1_IRQHandler            ; SPI1
                DCD     SPI2_IRQHandler            ; SPI2
                DCD     USART1_IRQHandler          ; USART1
                DCD     USART2_IRQHandler          ; USART2
                DCD     USART3_IRQHandler          ; USART3
                DCD     EXTI15_10_IRQHandler       ; EXTI Line 15..10
                DCD     RTCAlarm_IRQHandler        ; RTC Alarm through EXTI Line
                DCD     USBWakeUp_IRQHandler       ; USB Wakeup from suspend
                DCD     TIM8_BRK_IRQHandler        ; TIM8 Break
                DCD     TIM8_UP_IRQHandler         ; TIM8 Update
                DCD     TIM8_TRG_COM_IRQHandler    ; TIM8 Trigger and Commutation
                DCD     TIM8_CC_IRQHandler         ; TIM8 Capture Compare
                DCD     ADC3_IRQHandler            ; ADC3
                DCD     FSMC_IRQHandler            ; FSMC
                DCD     SDIO_IRQHandler            ; SDIO
                DCD     TIM5_IRQHandler            ; TIM5
                DCD     SPI3_IRQHandler            ; SPI3
                DCD     UART4_IRQHandler           ; UART4
                DCD     UART5_IRQHandler           ; UART5
                DCD     TIM6_IRQHandler            ; TIM6
                DCD     TIM7_IRQHandler            ; TIM7
                DCD     DMA2_Channel1_IRQHandler   ; DMA2 Channel1
                DCD     DMA2_Channel2_IRQHandler   ; DMA2 Channel2
                DCD     DMA2_Channel3_IRQHandler   ; DMA2 Channel3
                DCD     DMA2_Channel4_5_IRQHandler ; DMA2 Channel4 & Channel5
__Vectors_End   ;向量表结束标志

__Vectors_Size  EQU  __Vectors_End - __Vectors ;计算向量表地址空间大小

;|.text|  用于表示由 C 编译程序产生的代码段，或用于以某种方式与 C 库关联的代码段。
                AREA    |.text|, CODE, READONLY   ;定义C编译器源代码的代码段，只读
               
; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]  ;此处[WEAK]表示弱定义，优先执行其他文件的定义
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit   ; 装载寄存器指令
                BLX     R0    ; 带链接的跳转，切换指令集          
                LDR     R0, =__main
                BX      R0    ; 切换指令集,main函数不返回
                ENDP
               
; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
OS_CPU_PendSVHandler  PROC
                EXPORT  OS_CPU_PendSVHandler       [WEAK]
                B       .
                ENDP
OS_CPU_SysTickHandler PROC
                EXPORT  OS_CPU_SysTickHandler            [WEAK]
                B       .
                ENDP

Default_Handler PROC
                ; 输出异常向量表标号，方便外部实现异常的具体功能 ， [WEAK] 是弱定义的意思，如果外部定义了，优先执行外部定义，否则下面的函数定义
                EXPORT  WWDG_IRQHandler            [WEAK]
                EXPORT  PVD_IRQHandler             [WEAK]
                EXPORT  TAMPER_IRQHandler          [WEAK]
                EXPORT  RTC_IRQHandler             [WEAK]
                EXPORT  FLASH_IRQHandler           [WEAK]
                EXPORT  RCC_IRQHandler             [WEAK]
                EXPORT  EXTI0_IRQHandler           [WEAK]
                EXPORT  EXTI1_IRQHandler           [WEAK]
                EXPORT  EXTI2_IRQHandler           [WEAK]
                EXPORT  EXTI3_IRQHandler           [WEAK]
                EXPORT  EXTI4_IRQHandler           [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel2_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel3_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel4_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel5_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel6_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel7_IRQHandler   [WEAK]
                EXPORT  ADC1_2_IRQHandler          [WEAK]
                EXPORT  USB_HP_CAN1_TX_IRQHandler  [WEAK]
                EXPORT  USB_LP_CAN1_RX0_IRQHandler [WEAK]
                EXPORT  CAN1_RX1_IRQHandler        [WEAK]
                EXPORT  CAN1_SCE_IRQHandler        [WEAK]
                EXPORT  EXTI9_5_IRQHandler         [WEAK]
                EXPORT  TIM1_BRK_IRQHandler        [WEAK]
                EXPORT  TIM1_UP_IRQHandler         [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler    [WEAK]
                EXPORT  TIM1_CC_IRQHandler         [WEAK]
                EXPORT  TIM2_IRQHandler            [WEAK]
                EXPORT  TIM3_IRQHandler            [WEAK]
                EXPORT  TIM4_IRQHandler            [WEAK]
                EXPORT  I2C1_EV_IRQHandler         [WEAK]
                EXPORT  I2C1_ER_IRQHandler         [WEAK]
                EXPORT  I2C2_EV_IRQHandler         [WEAK]
                EXPORT  I2C2_ER_IRQHandler         [WEAK]
                EXPORT  SPI1_IRQHandler            [WEAK]
                EXPORT  SPI2_IRQHandler            [WEAK]
                EXPORT  USART1_IRQHandler          [WEAK]
                EXPORT  USART2_IRQHandler          [WEAK]
                EXPORT  USART3_IRQHandler          [WEAK]
                EXPORT  EXTI15_10_IRQHandler       [WEAK]
                EXPORT  RTCAlarm_IRQHandler        [WEAK]
                EXPORT  USBWakeUp_IRQHandler       [WEAK]
                EXPORT  TIM8_BRK_IRQHandler        [WEAK]
                EXPORT  TIM8_UP_IRQHandler         [WEAK]
                EXPORT  TIM8_TRG_COM_IRQHandler    [WEAK]
                EXPORT  TIM8_CC_IRQHandler         [WEAK]
                EXPORT  ADC3_IRQHandler            [WEAK]
                EXPORT  FSMC_IRQHandler            [WEAK]
                EXPORT  SDIO_IRQHandler            [WEAK]
                EXPORT  TIM5_IRQHandler            [WEAK]
                EXPORT  SPI3_IRQHandler            [WEAK]
                EXPORT  UART4_IRQHandler           [WEAK]
                EXPORT  UART5_IRQHandler           [WEAK]
                EXPORT  TIM6_IRQHandler            [WEAK]
                EXPORT  TIM7_IRQHandler            [WEAK]
                EXPORT  DMA2_Channel1_IRQHandler   [WEAK]
                EXPORT  DMA2_Channel2_IRQHandler   [WEAK]
                EXPORT  DMA2_Channel3_IRQHandler   [WEAK]
                EXPORT  DMA2_Channel4_5_IRQHandler [WEAK]
; 如下只是定义一个空函数
WWDG_IRQHandler
PVD_IRQHandler
TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
ADC1_2_IRQHandler
USB_HP_CAN1_TX_IRQHandler
USB_LP_CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
USBWakeUp_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
ADC3_IRQHandler
FSMC_IRQHandler
SDIO_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_IRQHandler
TIM7_IRQHandler
DMA2_Channel1_IRQHandler
DMA2_Channel2_IRQHandler
DMA2_Channel3_IRQHandler
DMA2_Channel4_5_IRQHandler
                B       .

                ENDP

                ALIGN ; 默认是字对齐方式，也说明了代码是4字节对齐的

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB ;如果勾选了
               
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
               
                 ELSE
               
                 IMPORT  __use_two_region_memory  ; 两区堆栈空间，堆和栈有各自的空间地址
                 EXPORT  __user_initial_stackheap
                
__user_initial_stackheap
; 此处是初始化两区的堆栈空间，堆是从由低到高的增长，栈是由高向低生长的，两个是互相独立的数据段，并不能交叉使用。

                 LDR     R0, =  Heap_Mem
                 LDR     R1, = (Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END  ; END 命令指示汇编器，已到达一个源文件的末尾。

;******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE*****