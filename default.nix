{ nixpkgs ? import <nixpkgs> {}
}:

let
  pkgs = nixpkgs.pkgs;
  stdenv = pkgs.stdenv;
in
stdenv.mkDerivation rec {
  name = "polyadvent";
  version = "0.1";

  src = ./.;

  makeFlags = "PREFIX=$(out)";

  buildInputs = with pkgs; [ SDL2 mesa ];

  meta = with stdenv.lib; {
    description = "Procedural low poly fun";
    homepage = "https://jb55.com/polyadvent";
    maintainers = with maintainers; [ jb55 ];
    license = licenses.gpl2;
  };
}
