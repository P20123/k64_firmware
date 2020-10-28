#include <environment.h>
#include <nongnu/unistd.h>
#include <string.h>
#include <drivers/kinetis/uart.h>
#include <stdint.h>
#include <kernel/process.h>
#include <kernel/stream_io.h>
#include <multitasking.h>
#include <kernel/private/static_memory.h>
#include <kernel/schedule.h>
#include <drivers/arm/cm4/systick.h>

pcb_t app1, app2, app3;
uint32_t app1_stack[500];
uint32_t app2_stack[500];
uint32_t app3_stack[500];
int app1_main(void);
int app2_main(void);
int app3_main(void);


/**
 * Entry point for the process switcher test.
 */
int init_main(void) {
    write(uart0_fileno, "started\r\n", 9);
    process_init(&app1, app1_main, app1_stack, 500, false, false);
    process_init(&app2, app2_main, app2_stack, 500, false, false);
    process_init(&app3, app3_main, app3_stack, 500, true, false);
    schedule_process(&process_table, &app1);
    schedule_process(&process_table, &app2);
    schedule_process(&process_table, &app3);
    return 0;
}

int recursion_tester(int stack_frames) {
    // rough recursive function
    char buf[20];
    buf[5] = stack_frames;
    return (stack_frames)? recursion_tester(buf[5]-1):0;
}

int app1_main(void) {
    char *msgs[] = {"entering app1\r\n", "app1 loop\r\n"};
    int bytes = 0;
    write(0, msgs[0], strlen(msgs[0]));
    bytes = 0;
    for(;;) {
        recursion_tester(20);
        while((bytes += write(0, msgs[1] + bytes, strlen(msgs[1]) - bytes)) < strlen(msgs[1]));
        bytes = 0;
#ifdef PROC_SW_TEST_MT_COOPERATIVE
        yield();
#endif
    }
    return 0;
}
int app2_main(void) {
    char *msgs[] = {"entering app2\r\n", "app2 loopfkajs;dlkfja;lskdjf;laksdjf;lksjd;lfkja;lksjdf;lkasjd;flkasjdf;lkasjd;lfkja;sldkfj;laskdjf;laksdjflkasdjflksdghjkfnbvkdfjnbgfhbniuetrgdkjfhgakljsfhdgiteubndibfnvjksdhnflkiudbrvirbnlalkjsflksdjfoaernuvrieubngitunvb;sedfjnvitunbirubnlitnubilstrunblioutnbiunrilbunisurntbisrutnbilsrtyuhgnosdjfhgkljnbknrtiubnslejkitnbkgfjbnilsuhgnaoebvitounls;iounbvt;iousnbitunkjvnb;ostunhbionosndfbo;sutnhbo;srukbtn;o\r\n"};
    int bytes = 0;
    write(0, msgs[0], strlen(msgs[0]));
    bytes = 0;
    for(;;) {
        recursion_tester(20);
        /*bytes = write(0, msgs[1], strlen(msgs[1]));*/
        while((bytes += write(0, msgs[1] + bytes, strlen(msgs[1]) - bytes)) < strlen(msgs[1]));
        bytes = 0;
#ifdef PROC_SW_TEST_MT_COOPERATIVE
        yield();
#endif
    }
    return 0;
}

int app3_main(void) {
    char *msgs[] = {"entering app3\r\n", "app3 loop\r\n"};
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);
    float x = 1.0f;
    int color = 0;
    int bytes = 0;
    write(0, msgs[0], strlen(msgs[0]));
    bytes = 0;
    for(;;) {
        recursion_tester(5);
        while((bytes += write(0, msgs[1] + bytes, strlen(msgs[1]) - bytes)) < strlen(msgs[1]));
        bytes = 0;
        x += 0.5f * x;
        if(x > 5.0f) {
            color ^= 1;
            if(color) {
                GPIOB->PSOR |= (1 << 21);
                GPIOB->PCOR |= (1 << 22);
            }
            else {
                GPIOB->PCOR |= (1 << 21);
                GPIOB->PSOR |= (1 << 22);
            }
            x = 1.0f;
        }
#ifdef PROC_SW_TEST_MT_COOPERATIVE
        yield();
#endif
    }
    return 0;
}
/**
 * This code is to use PIT0 instead of the SysTick timer. Disregard.
void init_pit(int period_us) {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR |= PIT_MCR_MDIS_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;

    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    // input is 20.97 MHz
    PIT->CHANNEL[0].LDVAL = period_us*((SystemCoreClock)/1000000);

    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    NVIC_EnableIRQ(PIT0_IRQn);
    NVIC_SetPriority(PIT0_IRQn, 16);
}


void PIT0_IRQHandler(void) {
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    yield();
}
*/
