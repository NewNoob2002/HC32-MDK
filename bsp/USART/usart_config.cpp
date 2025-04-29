#include "usart_config.h"
#include <usart_handlers.h>
#include <WVariant.h>
#include "gpio.h"
//
// USART configurations
//
usart_config_t USART1_config = 
{
    .peripheral = {
        .register_base = CM_USART1,
        .clock_id = FCG1_PERIPH_USART1,
        .tx_pin_function = Func_Usart1_Tx,
        .rx_pin_function = Func_Usart1_Rx,
    },
    .interrupts = {
        .rx_data_full = {
            .interrupt_source = INT_SRC_USART1_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<1>,
        },        
        .rx_error = {
            .interrupt_source = INT_SRC_USART1_EI,
            .interrupt_handler = USARTx_rx_error_irq<1>,
        },
        .tx_buffer_empty = {
            .interrupt_source = INT_SRC_USART4_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<1>,
        },
        .tx_complete = {
            .interrupt_source = INT_SRC_USART1_TCI,
            .interrupt_handler = USARTx_tx_complete_irq<1>,
        },
    },
    .state = {
        .rx_error = usart_receive_error_t::None,
    },
    #ifdef USART_RX_DMA_SUPPORT
    .dma = {
        .rx_data_available_event = EVT_USART1_RI,
        .rx_data_available_dma_btc = {
            .interrupt_handler = USARTx_rx_da_dma_btc_irq<1>,
        },
    }
    #endif
};