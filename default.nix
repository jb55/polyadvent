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

  nativeBuildInputs = with pkgs; [ tinycc pkg-config gdb ];

  buildInputs = with pkgs; [ SDL2 mesa libglvnd ] ++ 
                (with xorg; [ libX11 libxcb libXau libXdmcp libXext libXcursor 
		              libXrender libXfixes libXinerama libXi libXrandr 
			      libXScrnSaver libXxf86vm ]);

  shellHook = ''
	  export LD_LIBRARY_PATH=${pkgs.libglvnd}/lib;
  '';

  meta = with stdenv.lib; {
    description = "Procedural low poly fun";
    homepage = "https://jb55.com/polyadvent";
    maintainers = with maintainers; [ jb55 ];
    license = licenses.gpl2;
  };
}
