cmake_minimum_required(VERSION 3.21)
project(Esempi_Lezione)

set(CMAKE_CXX_STANDARD 14)

add_executable(Es_signal esercizi/signals.c)
add_executable(Es_signal_Sender esercizi/signal_sender.c)
add_executable(Fifo_Test1 ex_fifo_test_1.c)
add_executable(Fifo_Test4 ex_fifo_test_4.c)
add_executable(Signal_Test ex_signal_test.c)
add_executable(Pipe_Test1 ex_pipe_test_1.c)
add_executable(Pipe_Test2 ex_pipe_test_2.c)
add_executable(Pipe_Test3 ex_pipe_test_3.c)
add_executable(ShmV_Test1 ex_shmV_test_1.c)
add_executable(Sock_Cli ex_sock_cli.c)
add_executable(Sock_Serv_1 ex_sock_serv_1.c)
add_executable(Sock_Serv_2 ex_sock_serv_2.c)

