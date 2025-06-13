title_echo() {
    echo
    echo "========== $@"
}

run_text(){
    echo "$ $@"
    eval $@
}

title_echo 1.txt 파일 생성
run_text "touch 1.txt"


title_echo 1.txt 파일을 2.txt 에 하드링크
run_text "ln 1.txt 2.txt"


title_echo 1.txt 파일을 3.txt 파일에 소프트링크
run_text "ln -s 1.txt 3.txt"


title_echo inode 까지 포함하여 1~3.txt 파일 표시
run_text "ls -il [1-3].txt"


title_echo 2.txt에 파일 write하고 다른 파일에서 확인하고 용량 확인 해보기
run_text "echo "123" > 2.txt"
run_text "cat 1.txt"
run_text "ls -il [1-3].txt"
