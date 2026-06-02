{
  description = "C JSON:API spike for Trade Tariff commodity responses";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            clang-tools
            cmocka
            gcc
            gnumake
            pkg-config
            postgresql
            ruby
            yyjson
          ];
        };
      }
    );
}
