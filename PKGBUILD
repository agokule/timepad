# Maintainer: Your Name <your.email@example.com>
pkgname=timepad
pkgver=0.1.0
pkgrel=1
pkgdesc="A simple, elegant timer and stopwatch application built with C++23, SDL3, and ImGui"
arch=('x86_64')
url="https://github.com/agokule/timepad"
license=('custom')
depends=('gcc-libs' 'wayland' 'libxkbcommon' 'libpulse' 'alsa-lib')
makedepends=('cmake' 'gcc' 'git')
source=("$pkgname::git+https://github.com/agokule/timepad.git")
sha256sums=('SKIP')

prepare() {
    cd "$srcdir/$pkgname"
    
    # Initialize and update git submodules
    git submodule init
    git submodule update --recursive
}

build() {
    cd "$srcdir/$pkgname"
    
    # Configure CMake
    cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    
    # Build the project
    cmake --build build
}

package() {
    cd "$srcdir/$pkgname"
    
    # Install the binary to lib directory
    install -Dm755 "build/Timepad" "$pkgdir/usr/lib/$pkgname/Timepad"
    
    # Install assets directory
    install -dm755 "$pkgdir/usr/lib/$pkgname"
    cp -r assets "$pkgdir/usr/lib/$pkgname/"
    
    # Create wrapper script that runs from the correct directory
    install -dm755 "$pkgdir/usr/bin"
    cat > "$pkgdir/usr/bin/timepad" << 'EOF'
#!/bin/bash
cd /usr/lib/timepad
exec ./Timepad "$@"
EOF
    chmod 755 "$pkgdir/usr/bin/timepad"
    
    # Install documentation
    install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
    
    # TODO: Add license file installation once LICENSE file exists
    # install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
