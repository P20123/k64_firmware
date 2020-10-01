#include <util/debug.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>

extern char *xpc_scratch_region;
void txpc_send_msg(int, int, int, char *, int);
/**
 * Create a debug message from a format string, it's unique identifier, and
 * argument list.
 *
 *                       DO NOT CALL THIS FUNCTION DIRECTLY
 *
 * Seriously you will make a huge headache for yourself. Use the os_log macro.
 */
int vos_log(char *fmt, uint16_t id, ...) {
    va_list argv;
    char *currptr = fmt;
    char *endptr = fmt;
    void *currarg = NULL;
    char msgbuf[255];
    char *msgbuf_write;
    int curr_arg_bytes = 0;

    // put the ID into the message
    ((uint16_t*)msgbuf)[0] = id;
    msgbuf_write = msgbuf + 2;
    // handle the arg list
    va_start(argv, id);
    while(*currptr != '\0') {
        if(*currptr == '%') {
            endptr = currptr;
            curr_arg_bytes = yield_fmt_arg_bytes(currptr, &endptr);
            if(endptr == currptr) {
                // no bytes processed, error here
            }
            currptr = endptr + 1;
            // handle the argument
            switch(curr_arg_bytes) {
                case 0:break;

                case 1:
                    // va_arg promotes char to int
                    currarg = (void*)va_arg(argv, int);
                break;

                case 2:
                    // va_arg promotes short to int
                    currarg = (void*)va_arg(argv, int);
                break;

                case 4:
                    currarg = (void*)va_arg(argv, uint32_t);
                break;

                case 8:
                    currarg = (void*)va_arg(argv, uint64_t);
                break;
                
                case -1:
                    // call strlen on ptr
                    currarg = (void*)va_arg(argv, void *);
                    curr_arg_bytes = strlen(currarg);
                break;

                case -2:
                    // NOTE: we do not support this right now.
                    // call wcslen on ptr
                    currarg = (void*)va_arg(argv, void *);
                    /*curr_arg_bytes = wcslen(currarg);*/
                break;

                default: break;
            }
            memcpy(msgbuf_write, currarg, curr_arg_bytes);
            msgbuf_write += curr_arg_bytes;
            if(msgbuf_write - msgbuf > 255) {
                // too big for a TXPC_FRAME_TYPE_MSG, will need STREAM
                // messages for this debug log
            }
        }
        else {
            currptr++;
        }
    }
    va_end(argv);
    txpc_send_msg(stdout, 1, 1, msgbuf, msgbuf_write - msgbuf);
}
