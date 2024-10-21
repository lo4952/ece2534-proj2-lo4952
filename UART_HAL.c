#include "UART_HAL.h"

void InitUART(uint32_t moduleInstance,
              const eUSCI_UART_Config *uartConfig_p,
              uint_fast8_t selectedPort, uint_fast16_t selectedPins)
{
    UART_initModule(moduleInstance, uartConfig_p);
    UART_enableModule(moduleInstance);

    // make sure Tx and Rx pins of EUSCI_A0 work for UART and not a regular GPIO pin
    GPIO_setAsPeripheralModuleFunctionInputPin(selectedPort,
          selectedPins, GPIO_PRIMARY_MODULE_FUNCTION);

}

void UpdateUART(uint32_t moduleInstance,
                const eUSCI_UART_Config *uartConfig_p) {
    UART_initModule(moduleInstance, uartConfig_p);
    UART_enableModule(moduleInstance);

}

bool UARTHasChar(uint32_t moduleInstance)
{
    return (UART_getInterruptStatus (moduleInstance, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                    == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}

/* Returns the 8-bit character received by the given UART instance */
uint8_t UARTGetChar(uint32_t moduleInstance)
{
    return UART_receiveData(moduleInstance);
}

/* Returns TRUE if UART instance moduleInstance can transmit a character, otherwise returns FALSE */
bool UARTCanSend(uint32_t moduleInstance)
{
    return (UART_getInterruptStatus (moduleInstance, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                        == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
}

/* Sends the character tChar to the UART instance moduleInstance */
void UARTPutChar(uint32_t moduleInstance, uint8_t tChar)
{
    UART_transmitData(moduleInstance, tChar);
}

void UARTSetBaud(uint32_t moduleInstance,
                 eUSCI_UART_Config *uartConfig_p,
                 UARTBaudRate_t newBaud)
{


    switch(newBaud)
    {
    case baud19200:
        uartConfig_p->clockPrescalar = 156;
        uartConfig_p->firstModReg = 4;
        uartConfig_p->secondModReg = 0x00;
        UpdateUART(moduleInstance, uartConfig_p);
        break;

    case baud38400:
        uartConfig_p->clockPrescalar = 78;
        uartConfig_p->firstModReg = 2;
        uartConfig_p->secondModReg = 0x00;
        UpdateUART(moduleInstance, uartConfig_p);
        break;

    case baud57600:
        uartConfig_p->clockPrescalar = 52;
        uartConfig_p->firstModReg = 1;
        uartConfig_p->secondModReg = 0x25;
        UpdateUART(moduleInstance, uartConfig_p);
        break;

    case baud9600:
        uartConfig_p->clockPrescalar = 312;
        uartConfig_p->firstModReg = 8;
        uartConfig_p->secondModReg = 0x00;
        UpdateUART(moduleInstance, uartConfig_p);
        break;

    default: // should never get here
    } // end switch

} // end UARTSetBaud
