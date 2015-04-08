{ clangStdenv
, SDL2
, mesa
, xlibs
}:

let
  stdenv = clangStdenv;
in stdenv.mkDerivation rec {
  name = "polyadvent";
  version = "0.1";

  src = ./.;

  makeFlags = "PREFIX=$(out)";

  buildInputs = [ SDL2 mesa ]
    ++ (with xlibs; [ libX11 libXi libXrandr libXxf86vm  ] );

  meta = with stdenv.lib; {
    description = "Procedural low poly fun";
    homepage = "https://jb55.com/polyadvent";
    maintainers = with maintainers; [ jb55 ];
    license = licenses.gpl2;
  };
}
