{clangStdenv, glfw, mesa, xlibs}:

let
  stdenv = clangStdenv;
in stdenv.mkDerivation rec {
  name = "polycraft";
  version = "0.1";

  src = ./.;

  makeFlags = "PREFIX=$(out)";

  buildInputs = [ glfw mesa ]
    ++ (with xlibs; [ libX11 libXi libXrandr libXxf86vm  ] );

  meta = with stdenv.lib; {
    description = "Procedural low poly fun";
    homepage = "https://jb55.com/polycraft";
    maintainers = with maintainers; [ jb55 ];
    license = licenses.gpl2;
  };
}
