%include "dict.inc"

global find_word

extern string_equals

section .text

; Принимает два аргумента:
;   - Указатель на нуль-терминированную строку.
;   - Указатель на начало словаря.
; Возвращает:
;   - Если подходящее вхождение найдено, 
;     вернёт адрес начала вхождения в словарь, 
;     иначе вернёт 0.
find_word:
    ; Сравнение ключа элемента с искомым значением
    push rdi
    push rsi
    add rsi, POINTER_SIZE   ; rsi указывает на ключ
    call string_equals
    pop rsi
    pop rdi

    test rax, rax           ; Если строки равны,
    jne .on_equal           ; возвращаем указатель на элемент

    mov rsi, [rsi]          ; Переходим на следующий элемент,
    test rsi, rsi           ; проверяем, что оно существует
    jne find_word

    ; Обработка результатов поиска
    xor rax, rax
    ret

    .on_equal:
        mov rax, rsi
        ret
