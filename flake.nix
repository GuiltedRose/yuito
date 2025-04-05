{
  description = "Yuito project flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;
        };

        qtPackages = pkgs.qt6;

        buildInputs = with pkgs; [
          cmake
          ninja
          gcc13               # For C++23
          python3
          qtPackages.qtbase
          qtPackages.qttools
          qtPackages.qtmultimedia
          qtPackages.qtdeclarative
          qtPackages.qtquickcontrols2
          qtPackages.qtwayland
          libGL               # OpenGL
          xorg.libX11
          xorg.libXext
          xorg.libXrender
          xorg.libxcb
          xorg.libXrandr
          xorg.libXi
        ];

      in {
        devShells.default = pkgs.mkShell {
          name = "qt-opengl-dev-shell";

          packages = buildInputs;

          shellHook = ''
            export CXX=g++
            export CC=gcc
            export QT_QPA_PLATFORM=wayland;x11
            echo "Dev shell for Qt6 + OpenGL + Python3 + C++23 ready!"
          '';
        };
      });
}
