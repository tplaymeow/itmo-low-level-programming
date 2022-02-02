%define BUFFER_SIZE 256

%include "lib.inc"
%include "words.inc"
%include "dict.inc"

global _start

extern find_word

section .rodata

value_is_msg: db "Value: ", 0

; Сообщения об ошибке
key_not_found_msg: db "Key not found.", 0x0A, 0
buffer_overflow_msg: db "Buffer overflow.", 0x0A, 0


section .text
_start:
    ; Выделение места под клич, чтение ключа
    sub rsp, BUFFER_SIZE
    mov rdi, rsp
    mov rsi, BUFFER_SIZE
    call read_word

    ; Проверка на переполнение буффера
    test rax, rax
    je .on_buffer_overflow

    ; Вызов `find_word`
    mov rdi, rax
    mov rsi, LAST_ELEMENT
    call find_word

    ; Проверка на наличие элемента
    test rax, rax
    je .on_key_not_found

    ; Вывод приветсвенной строки - "Value: "
    mov rdi, value_is_msg
    push rax
    call print_string
    pop rax

    ; Подготовка к выводу значения
    add rax, POINTER_SIZE          ; Прибавляем размер указателя на след. эл. 
                        ; В rax - указатель на ключ
    push rax
    mov rdi, [rsp]
    call string_length  ; Считаем длину ключа
    pop rdi
    add rdi, rax        ; Прибавляем размер ключа
    inc rdi             ; Увеличиваем rdi на 1 из-за нуля-терминатора.
                        ; В rax - указатель на значение
    
    call print_string
    call print_newline
    xor rdi, rdi
    call exit

    .on_buffer_overflow:
        mov rdi, buffer_overflow_msg
        call print_error_string
        mov rdi, 1
        call exit
    
    .on_key_not_found:
        mov rdi, key_not_found_msg
        call print_error_string
        mov rdi, 1
        call exit
