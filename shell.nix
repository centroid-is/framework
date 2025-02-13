{ pkgs ? import <nixpkgs> {} }:

(pkgs.buildFHSUserEnv {
  name = "vcpkg";
  targetPkgs = pkgs: let
    pythonEnv = pkgs.python3.withPackages (ps: [
      ps.setuptools
      ps.jinja2
      ps.meson
    ]);
  in (with pkgs; [
      autoconf
      automake
      cmake
      gcc
      gettext
      glibc.dev
      gperf
      libtool
      libxkbcommon.dev
      m4
      ninja
      pkg-config
      zip
      zstd.dev
      pythonEnv
      vcpkg
    ]);
  runScript = ''

    export CMAKE_PROGRAM_PATH="/usr/bin"
    export CMAKE_TOOLCHAIN_FILE="/usr/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
    bash
  '';
}).env
