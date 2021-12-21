pipeline {
  agent any
  stages {
    stage('Prep') {
      parallel {
        stage('Make directory out') {
          steps {
            echo 'Cleaning out'
            sh 'rm -rf out'
            sh 'mkdir -p out'
          }
        }

        stage('Make sure binaries are up to date') {
          environment {
            Path = '/home/pppig/binaries/toolchain/clang/bin:/home/pppig/binaries/toolchain/gcc-64/bin:/home/pppig/binaries/toolchain/gcc-32/bin/:/home/pppig/binaries/toolchain/dtc:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin'
          }
          steps {
            ws(dir: 'binaries') {
              git 'git@github.com:PIPIPIG233666/binaries.git'
            }

            sh '''git -C /home/pppig/binaries submodule init
git -C /home/pppig/binaries submodule update'''
          }
        }

        stage('Build mkbootimg') {
          steps {
            dir(path: '/home/pppig/binaries/mkbootimg') {
              sh 'make -j16'
            }

          }
        }

      }
    }

    stage('Kconfig') {
      steps {
        sh 'make O=out ARCH=arm64 vendor/lineage_davinci_defconfig'
      }
    }

    stage('Kbuild') {
      environment {
        Path = '/home/pppig/binaries/toolchain/clang/bin:/home/pppig/binaries/toolchain/gcc-64/bin:/home/pppig/binaries/toolchain/gcc-32/bin/:/home/pppig/binaries/toolchain/dtc:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin'
      }
      steps {
        sh 'make -j16 O=out ARCH=arm64 CC=clang LD=ld.lld CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-android- CROSS_COMPILE_ARM32=arm-linux-androideabi- DTC_EXT=dtc'
      }
    }

    stage('Finish up') {
      parallel {
        stage('Compress boot img') {
          steps {
            sh '''find out/arch/arm64/boot/dts -name \'*.dtb\' -exec cat {} + > /tmp/${device}.dtb
/home/pppig/binaries/mkbootimg/mkbootimg \\
    --kernel out/arch/arm64/boot/Image.gz-dtb \\
    --cmdline \'androidboot.hardware=qcom androidboot.console=ttyMSM0 service_locator.enable=1 swiotlb=1 earlycon=msm_geni_serial,0x880000 loop.max_part=7 cgroup.memory=nokmem,nosocket androidboot.vbmeta.avb_version=1.0\' \\
    --base           0x00000000 \\
    --pagesize       4096 \\
    --dtb            /tmp/${device}.dtb \\
    --os_version     11.0.0 \\
    --os_patch_level 2021-12 \\
    -o out/boot.img'''
          }
        }

        stage('Compress dtbo img') {
          steps {
            sh '''/home/pppig/binaries/libufdt/utils/src/mkdtboimg.py create out/dtbo-pre.img \\
  --page_size=4096 \\
  `find out/arch/arm64/boot/dts/xiaomi \\
  -type f -name \'*.dtbo\' | sort`'''
          }
        }

      }
    }

  }
}