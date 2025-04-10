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

        qt = pkgs.qt6;
        llvm = pkgs.llvmPackages_16;

        buildInputs = with pkgs; [
          cmake
          gdb
          ninja
          python3
          llvm.clang
          llvm.lld

          qt.qtbase
          qt.qttools
          qt.qtwayland

          xorg.libX11
          xorg.libXext
          xorg.libXrender
          xorg.libxcb
          xorg.libXrandr
          xorg.libXi
        ];

      in {
        devShells.default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
          name = "yuito-dev";

          packages = buildInputs;

          shellHook = ''
            export CC=clang
            export CXX=clang++
            export QT_QPA_PLATFORM=xcb
            echo "Welcome to the Yuito dev shell (Qt6 + OpenGL + Clang)"
          '';
        };
      });
}
