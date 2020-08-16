# postmaster

- boot.img 만드는법
1. android 9 platform source 를 아무 경로에 놓는다.
2. 다음과같은 명령어를 입력함으로써 platform source code의 path 등록
	"export PLATFORM_PATH_AND9={PATH_TO_ANDROID9_PLATFORM_DIR}"
3. cd {PATH_TO_KERNEL_DIR}
4. build/build.sh
5. {PATH_TO_KERNEL_DIR}/out//android-msm-bluecross-4.9/private/msm-google/target/product/ 경로에 boot.img 생성됨.

- device에 kernel flashing 하는 법
1. fastboot 진입
2. fastboot flash boot boot.img
3. fastboot reboot
