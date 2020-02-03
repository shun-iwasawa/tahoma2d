#!/bin/bash
brew update
# from Homebrew 1.6.0 the old formula for obtaining Qt5.9.2 becomes invalid.
# so we start to use the latest version of Qt. (#1910)
brew install boost qt clang-format glew lz4 lzo libmypaint jpeg-turbo nasm yasm aom dav1d fontconfig freetype gnutls lame libass libbluray libsoxr libvorbis libvpx opencore-amr openh264 openjpeg opus rav1e sdl2 snappy speex tesseract theora webp xvid xz
