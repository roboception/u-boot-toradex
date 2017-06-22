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
# extract the default environment from the u-boot source code
# (is used as input to generate the u-boot environment image)
./scripts/get_default_envs.sh > uboot-env.txt
# generate the u-boot environment image from the extracted u-boot default environment
./tools/mkenvimage -r -s 0x20000 -o uboot-env.bin uboot-env.txt
chmod 644 uboot-env.bin
cp ${UBOOT_ENV_BIN} ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}

echo "${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN}"
stat ${UBOOT_BIN_OUT_DIR}/${UBOOT_ENV_BIN} -c %y

