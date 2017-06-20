#!/bin/sh
# build u-boot env image
# copy u-boot env image into image binary directory
# depends on .build_and_deploy_uboot.sh which needs to be executed before to generate mkenvimage

# get directory the script resides in
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

UBOOT_DIR=$SCRIPTPATH
UBOOT_ENV_BIN=uboot-env.bin
UBOOT_BIN_OUT_DIR=$SCRIPTPATH/../rc_visard_image/apalis-tk1_bin

cd ${UBOOT_DIR}
./tools/mkenvimage -r -s 0x20000 -o uboot-env.bin uboot-env.txt
cp ${UBOOT_ENV_BIN} ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}

echo "${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}"
stat ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN} -c %y

