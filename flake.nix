{
  description = "Ray Multiply";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
        app = pkgs.stdenv.mkDerivation {
          name = "ray-multiply";
          src = ./.;
          buildInputs = with pkgs; [ raylib gcc ];
          buildPhase = ''
            gcc -o main main.c -lraylib
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp main $out/bin/ray-multiply
          '';
        };
        appFb = pkgs.stdenv.mkDerivation {
          name = "ray-multiply";
          src = ./.;
          buildInputs = with pkgs; [ raylib gcc ];
          buildPhase = ''
            gcc -o main main.c -lraylib -DPLATFORM_DRM
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp main $out/bin/ray-multiply-fb
          '';
        };
      in
      {
        defaultPackage = app;
        packages.ray-multiply = app;
        packages.ray-multiply-fb = appFb;
      }
    );
}
