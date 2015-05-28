#!/bin/bash
gcc source/can_interceptor.c source/avl.c source/LinkedList.c source/parser.c source/MessageAVL.c source/translator.c source/main.c -o translator -pthread -DAF_CAN=PF_CAN -DPF_CAN=29 -DSO_RXQ_OVFL=40 -fno-strict-aliasing -Iinclude -Wno-parentheses -Wall -O2 -Iheaders
