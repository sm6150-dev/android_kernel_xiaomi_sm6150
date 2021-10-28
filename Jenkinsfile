pipeline {
  agent any
  stages {
    stage('Kernel') {
      parallel {
        stage('Kconfig') {
          steps {
            echo 'Cleaning out'
            sh 'rm -rf out'
            sh '''mkdir -p out
make O=out \\
  ARCH=arm64 \\
  vendor/lineage_davinci_defconfig'''
          }
        }

        stage('Kbuild') {
          environment {
            Path = '/home/pppig/binaries/toolchain/clang/bin:/home/pppig/binaries/toolchain/gcc-64/bin:/home/pppig/binaries/toolchain/gcc-32/bin/:/home/pppig/binaries/toolchain/dtc:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin'
          }
          steps {
            ws(dir: 'binaries') {
              git 'git@github.com:PIPIPIG233666/binaries.git'
            }

            echo 'Building kernel'
            sh '''make -j16 \\
  O=out \\
  ARCH=arm64 \\
  CC=clang \\
  CLANG_TRIPLE=aarch64-linux-gnu- \\
  CROSS_COMPILE=aarch64-linux-android- \\
  CROSS_COMPILE_ARM32=arm-linux-androideabi- \\
  DTC_EXT=dtc'''
          }
        }

      }
    }

  }
}