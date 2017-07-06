#!/bin/bash
# build u-boot
# copy u-boot into image binary directory

# get directory the script resides in
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

UBOOT_DIR=$SCRIPTPATH
UBOOT_BIN=u-boot-dtb-tegra.bin
UBOOT_CONFIG=apalis-tk1_defconfig
UBOOT_BIN_RENAMED=u-boot-dtb-tegra-rc.bin
UBOOT_ENV_BIN=uboot-env.bin
# default output dir
UBOOT_BIN_OUT_DIR=$SCRIPTPATH/../rc_visard_image/apalis-tk1_bin

function usage() {
  echo "Usage: build_and_deploy_uboot.sh [--output_dir /path/to/rc_visard_image/apalis-tk1_bin]"
}

args=1
while [ $args == 1 ]
do
  case "$1" in
    "--help" | "-h")
      usage
      exit 0
      ;;
    "--output_dir" | "-o")
      UBOOT_BIN_OUT_DIR="$2"
      shift
      shift
      ;;
      *) # default
        args=0
  esac
done

if [ "$CROSS_COMPILE" == "" ]; then
  export CROSS_COMPILE=arm-linux-gnueabihf-
fi

get_git_branch() {
  git rev-parse --abbrev-ref HEAD
}

get_git_commit() {
  git log --format="%H" -n 1
}

echo ""
echo "U-Boot branch and commit"
get_git_branch
get_git_commit

echo ""
echo "Build U-Boot"
cd ${UBOOT_DIR}
make distclean
make ${UBOOT_CONFIG}
make -j3 2>&1 | tee build.log

# extract the default environment from the u-boot source code
# (is used as input to generate the u-boot environment image)
echo "generate uboot-env.txt"
./scripts/get_default_envs.sh > uboot-env.txt
# generate the u-boot environment image from the extracted u-boot default environment
./tools/mkenvimage -r -s 0x20000 -o ${UBOOT_ENV_BIN} uboot-env.txt
chmod 644 ${UBOOT_ENV_BIN}

if [ -e $UBOOT_BIN_OUT_DIR ]; then
  cp ${UBOOT_BIN} ${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED}
  echo ""
  echo "U-Boot modification date of (file in image)"
  echo "${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED}"
  stat ${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED} -c %y

  cp ${UBOOT_ENV_BIN} ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}
  echo "${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}"
  stat ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN} -c %y
else
  echo "built but not deployed as $UBOOT_BIN_OUT_DIR does not exist"
  echo "result files are:"
  echo "  ${UBOOT_BIN}"
  echo "  ${UBOOT_ENV_BIN}"
fi
