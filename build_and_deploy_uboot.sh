#!/bin/sh
# build u-boot
# copy u-boot into image binary directory

# get directory the script resides in
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

UBOOT_DIR=$SCRIPTPATH
UBOOT_BIN=u-boot-dtb-tegra.bin
UBOOT_CONFIG=apalis-tk1_defconfig
UBOOT_BIN_OUT_DIR=$SCRIPTPATH/../rc_visard_image/apalis-tk1_bin
UBOOT_BIN_RENAMED=u-boot-dtb-tegra-rc.bin

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
cp ${UBOOT_BIN} ${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED}
echo ""
echo "U-Boot modification date of (file in image)"
echo "${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED}"
stat ${UBOOT_BIN_OUT_DIR}/${UBOOT_BIN_RENAMED} -c %y
