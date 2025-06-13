title_echo() {
    echo
    echo "========== $@"
}

run_text(){
    echo "$ $@"
    eval $@
}

title_echo umask 기본 값 확인
run_text "umask"

title_echo 파일 생성 후 권한 확인
run_text "touch 4.txt"
run_text "ls -l 4.txt"

title_echo umask 명령어로 새로 생성하는 파일의 특정 권한 제거
run_text "umask 420"
run_text "touch 5.txt"

title_echo "0020 -> 420 => 0666 & -420 => 0246"
run_text "ls -l 5.txt"