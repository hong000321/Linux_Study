        .data            @ 데이터 섹션
string: .asciz "\nHello, World!\n"
        
        .text            @ 코드 섹션
        .global main        @ 메인 함수

main:
         push {ip, lr}        @ 현재 반환 레지스터와 ip를 스택에 저장 

         mov r0, #1        @ 1 : 표준 출력(stdout)
         ldr r1, =string        @ 데이터 섹션에 있는 문자열(상수)을 r0에 저장
         mov r2, #16        @ 출력할 글자의 수는 16자
         mov r7, #4        @ write 함수는 시스템콜 번호 4번

         SWI 0x00

        pop {ip, pc}        @ 앞에서 스택에 저장한 값을 ip, pc로 로드
       .end
