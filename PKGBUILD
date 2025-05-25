pkgname=brilho
pkgver=1.0
pkgrel=1
pkgdesc="Interface gráfica para controle de brilho com SDL2"
arch=('x86_64')
license=('MIT')
depends=('sdl2' 'sdl2_ttf' 'polkit' 'libx11' 'ttf-dejavu')
makedepends=('gcc')
source=('brilho.c'
        'brilho.policy'
        'brilho.desktop' 'arch/brilho.sh') md5sums=('SKIP' 'SKIP' 'SKIP' 'SKIP')

build() {
  gcc brilho.c -o brilho `sdl2-config --cflags --libs` -lSDL2_ttf
}

package() {
  install -Dm755 brilho "$pkgdir/usr/bin/brilho"
  install -Dm755 brilho.sh "$pkgdir/usr/bin/brilho-gui"

  install -Dm644 brilho.policy "$pkgdir/usr/share/polkit-1/actions/com.devcurumin.brilho.policy"
  install -Dm644 brilho.desktop "$pkgdir/usr/share/applications/brilho.desktop"
}

