// The tests here were generated using the 'abnfgen' tool with the official
// URI ABNF. The tool was asked to produce 100 tests with the "attempt full
// coverage"
// (-c) mode enabled.

#include <gmock/gmock.h>

#include "uri.hpp"

using testing::ElementsAre;
// NOLINTNEXTLINE
TEST (Uri, 0001) {
  auto const x = uri::split ("C://[::A:eE5c]:2194/&///@//:_/%aB//.////#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "C");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::A:eE5c]");
  EXPECT_EQ (x->port, "2194");
  EXPECT_THAT (x->path, ElementsAre ("/&", "/", "/", "/@", "/", "/:_", "/%aB",
                                     "/", "/.", "/", "/", "/", "/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0002) {
  auto const x = uri::split ("P-.:/?/?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P-.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_EQ (x->query, "/?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0003) {
  auto const x = uri::split ("i+V:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "i+V");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0004) {
  auto const x = uri::split ("L:%Cf#%dD/?H");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("%Cf"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "%dD/?H");
}
// NOLINTNEXTLINE
TEST (Uri, 0005) {
  auto const x = uri::split ("E07:/8=-~%bF//%36////'/%16N%78//)/%53/;?*!");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "E07");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path,
               ElementsAre ("/8=-~%bF", "/", "/%36", "/", "/", "/", "/'",
                            "/%16N%78", "/", "/)", "/%53", "/;"));
  EXPECT_EQ (x->query, "*!");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0006) {
  auto const x = uri::split ("v:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0007) {
  auto const x = uri::split ("YXa:/#B");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "YXa");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "B");
}
// NOLINTNEXTLINE
TEST (Uri, 0008) {
  auto const x = uri::split ("n:/,+?$#(+!)D");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "n");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/,+"));
  EXPECT_EQ (x->query, "$");
  EXPECT_EQ (x->fragment, "(+!)D");
}
// NOLINTNEXTLINE
TEST (Uri, 0009) {
  auto const x = uri::split ("m:/?cJ");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "m");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_EQ (x->query, "cJ");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0010) {
  auto const x = uri::split ("zR:d/M/kx/s/GTl///SgA/?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "zR");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("d", "/M", "/kx", "/s", "/GTl", "/", "/",
                                     "/SgA", "/"));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0011) {
  auto const x = uri::split ("t:W?p#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "t");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("W"));
  EXPECT_EQ (x->query, "p");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0012) {
  auto const x = uri::split ("QrIq:/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "QrIq");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0013) {
  auto const x = uri::split ("OuU:/?bZK");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "OuU");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_EQ (x->query, "bZK");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0014) {
  auto const x = uri::split ("Fjfe:?h");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Fjfe");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "h");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0015) {
  auto const x = uri::split ("y:w/o/b/?lKTF");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("w", "/o", "/b", "/"));
  EXPECT_EQ (x->query, "lKTF");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0016) {
  auto const x = uri::split ("P://=:_%bb%Cf%2F-8;~@230.109.31.250#.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P");
  EXPECT_EQ (x->userinfo, "=:_%bb%Cf%2F-8;~");
  EXPECT_EQ (x->host, "230.109.31.250");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, ".");
}
// NOLINTNEXTLINE
TEST (Uri, 0017) {
  auto const x = uri::split ("N://@=i%bD%Cb&*%Ea)%CE//:%cA//#?//");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "=i%bD%Cb&*%Ea)%CE");
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/", "/:%cA", "/", "/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "?//");
}
// NOLINTNEXTLINE
TEST (Uri, 0018) {
  auto const x = uri::split ("X:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "X");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0019) {
  auto const x = uri::split ("U:??");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0020) {
  auto const x = uri::split ("G:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "G");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0021) {
  auto const x = uri::split ("l6+:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "l6+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0022) {
  auto const x = uri::split ("T.-://:@[VD.~]:?/@#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "T.-");
  EXPECT_EQ (x->userinfo, ":");
  EXPECT_EQ (x->host, "[VD.~]");
  EXPECT_EQ (x->port, "");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/@");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0023) {
  auto const x = uri::split ("rC://3.76.206.5:8966?/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "rC");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "3.76.206.5");
  EXPECT_EQ (x->port, "8966");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0024) {
  auto const x = uri::split ("oNP:///::");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "oNP");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "");
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/::"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0025) {
  auto const x = uri::split ("g0:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g0");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0026) {
  auto const x = uri::split ("Do1-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Do1-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0027) {
  auto const x = uri::split ("K:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0028) {
  auto const x = uri::split ("tc://@[::F]:/::@~?@/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "tc");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[::F]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/::@~"));
  EXPECT_EQ (x->query, "@/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0029) {
  auto const x = uri::split ("N:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0030) {
  auto const x = uri::split ("o:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "o");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0031) {
  auto const x = uri::split ("k-0+:\?\?\?/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "k-0+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "\?\?/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0032) {
  auto const x = uri::split ("y://%DD@253.216.255.251//:./?\?/://;");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y");
  EXPECT_EQ (x->userinfo, "%DD");
  EXPECT_EQ (x->host, "253.216.255.251");
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/", "/:.", "/"));
  EXPECT_EQ (x->query, "?/://;");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0033) {
  auto const x = uri::split ("B://.@[AC::1:6DEb:14.97.229.249]:?/#??~(");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "B");
  EXPECT_EQ (x->userinfo, ".");
  EXPECT_EQ (x->host, "[AC::1:6DEb:14.97.229.249]");
  EXPECT_EQ (x->port, "");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "??~(");
}
// NOLINTNEXTLINE
TEST (Uri, 0034) {
  auto const x = uri::split ("p://@26.254.86.252://!;");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "26.254.86.252");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/", "/!;"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0035) {
  auto const x = uri::split ("P+-n:#/%f0");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P+-n");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "/%f0");
}
// NOLINTNEXTLINE
TEST (Uri, 0036) {
  auto const x = uri::split ("u:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0037) {
  auto const x = uri::split ("U://%Aa:@[::b:E:A:53.48.69.41]?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U");
  EXPECT_EQ (x->userinfo, "%Aa:");
  EXPECT_EQ (x->host, "[::b:E:A:53.48.69.41]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0038) {
  auto const x = uri::split ("h.P+9:?:#?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "h.P+9");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, ":");
  EXPECT_EQ (x->fragment, "?");
}
// NOLINTNEXTLINE
TEST (Uri, 0039) {
  auto const x = uri::split ("x:??#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "x");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0040) {
  auto const x = uri::split ("A:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "A");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0041) {
  auto const x = uri::split ("Lp.:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Lp.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0042) {
  auto const x = uri::split ("d-:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "d-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0043) {
  auto const x = uri::split ("h-.:?/?/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "h-.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/?/");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0044) {
  auto const x = uri::split ("d:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "d");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0045) {
  auto const x = uri::split ("L:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0046) {
  auto const x = uri::split ("Z5://@[9:BB:8:DAc:BbAA:E:a::]?#@$");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Z5");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[9:BB:8:DAc:BbAA:E:a::]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "@$");
}
// NOLINTNEXTLINE
TEST (Uri, 0047) {
  auto const x = uri::split ("C-://[::1E:BB:a:5c1:Dd:40.44.228.108]/;?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "C-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::1E:BB:a:5c1:Dd:40.44.228.108]");
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/;"));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0048) {
  auto const x = uri::split ("z://[c:BC:b:A:Bd:D:dC1f:cedB]?/#/:/%FA");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "z");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[c:BC:b:A:Bd:D:dC1f:cedB]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "/:/%FA");
}
// NOLINTNEXTLINE
TEST (Uri, 0049) {
  auto const x = uri::split ("x.2:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "x.2");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0050) {
  auto const x = uri::split ("p://@[::F:e:4b:eCBE:f:c]");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[::F:e:4b:eCBE:f:c]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0051) {
  auto const x = uri::split ("tmi://[e:C:Aa:eD::FDfD:b:F]:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "tmi");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[e:C:Aa:eD::FDfD:b:F]");
  EXPECT_EQ (x->port, "");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0052) {
  auto const x = uri::split ("G+:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "G+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0053) {
  auto const x = uri::split ("A://[vA5.+:=.p~=)=&_;-=7)(.;]:768295/+");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "A");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[vA5.+:=.p~=)=&_;-=7)(.;]");
  EXPECT_EQ (x->port, "768295");
  EXPECT_THAT (x->path, ElementsAre ("/+"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0054) {
  auto const x = uri::split ("n+://[::]:9831#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "n+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::]");
  EXPECT_EQ (x->port, "9831");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0055) {
  auto const x = uri::split ("v-2e.l:?:????:/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v-2e.l");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, ":????:/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0056) {
  auto const x = uri::split ("ka+://6.@[F::219.226.254.253]:900/'R#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "ka+");
  EXPECT_EQ (x->userinfo, "6.");
  EXPECT_EQ (x->host, "[F::219.226.254.253]");
  EXPECT_EQ (x->port, "900");
  EXPECT_THAT (x->path, ElementsAre ("/'R"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0057) {
  auto const x = uri::split ("P://[daf::B:7:e:b:D:F]:730");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[daf::B:7:e:b:D:F]");
  EXPECT_EQ (x->port, "730");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0058) {
  auto const x = uri::split ("H://-!:_%Bd@[::]:7");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "H");
  EXPECT_EQ (x->userinfo, "-!:_%Bd");
  EXPECT_EQ (x->host, "[::]");
  EXPECT_EQ (x->port, "7");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0059) {
  auto const x = uri::split ("u+://;@[::dFC:d:6:d]://#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u+");
  EXPECT_EQ (x->userinfo, ";");
  EXPECT_EQ (x->host, "[::dFC:d:6:d]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/", "/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0060) {
  auto const x = uri::split ("D://[dCDa:c:e:B:F::D:a]:/%Dc");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[dCDa:c:e:B:F::D:a]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/%Dc"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0061) {
  auto const x = uri::split ("mF2:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "mF2");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0062) {
  auto const x = uri::split ("f.://[d1b:CF:AbBa::F:d:11.246.155.253]?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "f.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[d1b:CF:AbBa::F:d:11.246.155.253]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0063) {
  auto const x = uri::split ("f5++://@[7d::6:df:f:245.95.78.9]:??");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "f5++");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[7d::6:df:f:245.95.78.9]");
  EXPECT_EQ (x->port, "");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0064) {
  auto const x = uri::split ("c.l://[::bba:B:6:1.255.161.3]:#?/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "c.l");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::bba:B:6:1.255.161.3]");
  EXPECT_EQ (x->port, "");
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "?/");
}
// NOLINTNEXTLINE
TEST (Uri, 0065) {
  auto const x = uri::split ("T://[fdF::f2]");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "T");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[fdF::f2]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0066) {
  auto const x = uri::split ("U-92.://[::A:C:c]/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U-92.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::A:C:c]");
  EXPECT_FALSE (x->port);
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0067) {
  auto const x = uri::split ("K:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0068) {
  auto const x = uri::split ("l.://[c:CEa:cd1B:f:f:D::ef]?#%bC@/:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "l.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[c:CEa:cd1B:f:f:D::ef]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "%bC@/:");
}
// NOLINTNEXTLINE
TEST (Uri, 0069) {
  auto const x =
    uri::split ("v+://@[::C:dEd:4:218.255.251.5]:8/@.;J\?\?Q\?\?%48/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v+");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[::C:dEd:4:218.255.251.5]");
  EXPECT_EQ (x->port, "8");
  EXPECT_THAT (x->path, ElementsAre ("/@.;J"));
  EXPECT_EQ (x->query, "?Q?\?%48/");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0070) {
  auto const x = uri::split ("I:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "I");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0071) {
  auto const x = uri::split ("t.+://[::Ec:AcA:9a]:92/%8a/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "t.+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::Ec:AcA:9a]");
  EXPECT_EQ (x->port, "92");
  EXPECT_THAT (x->path, ElementsAre ("/%8a", "/"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0072) {
  auto const x = uri::split ("N+:?~");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "~");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0073) {
  auto const x = uri::split ("B:?/.#?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "B");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/.");
  EXPECT_EQ (x->fragment, "?");
}
// NOLINTNEXTLINE
TEST (Uri, 0074) {
  auto const x = uri::split ("u8K.://.(@[d::Baa:dE:D]#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u8K.");
  EXPECT_EQ (x->userinfo, ".(");
  EXPECT_EQ (x->host, "[d::Baa:dE:D]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0075) {
  auto const x = uri::split ("E+.://@[::F:ab79:B:fa:C]#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "E+.");
  EXPECT_EQ (x->userinfo, "");
  EXPECT_EQ (x->host, "[::F:ab79:B:fa:C]");
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0076) {
  auto const x = uri::split ("S+://[::BBc:d0:EA:3.67.149.137]:/?#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "S+");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[::BBc:d0:EA:3.67.149.137]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/"));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0077) {
  auto const x = uri::split ("Y://[4Bbc:bb::cDcd:5:c4:e:B1]:/%CA@/./?\?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Y");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[4Bbc:bb::cDcd:5:c4:e:B1]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/%CA@", "/.", "/"));
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0078) {
  auto const x = uri::split ("W.-://[CF::]://!?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "W.-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_EQ (x->host, "[CF::]");
  EXPECT_EQ (x->port, "");
  EXPECT_THAT (x->path, ElementsAre ("/", "/!"));
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0079) {
  auto const x = uri::split ("SF6:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "SF6");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0080) {
  auto const x = uri::split (R"(R:?????////???/////#??@?_:?)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "R");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, R"(????////???/////)");
  EXPECT_EQ (x->fragment, "??@?_:?");
}
// NOLINTNEXTLINE
TEST (Uri, 0081) {
  auto const x = uri::split ("g:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0082) {
  auto const x = uri::split (R"(D-ir+.PA:??#)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D-ir+.PA");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0083) {
  auto const x = uri::split ("Z-.-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Z-.-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0084) {
  auto const x = uri::split ("y-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0085) {
  auto const x = uri::split ("p:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0086) {
  auto const x = uri::split ("M:#*.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "M");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "*.");
}
// NOLINTNEXTLINE
TEST (Uri, 0087) {
  auto const x = uri::split ("I:?%ab#/.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "I");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "%ab");
  EXPECT_EQ (x->fragment, "/.");
}
// NOLINTNEXTLINE
TEST (Uri, 0088) {
  auto const x = uri::split ("v6:#:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v6");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, ":?");
}
// NOLINTNEXTLINE
TEST (Uri, 0089) {
  auto const x = uri::split ("D:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0090) {
  auto const x = uri::split ("e.:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "e.");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
TEST (Uri, 0091) {
  auto const x = uri::split ("L:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
TEST (Uri, 0092) {
  auto const x = uri::split ("g-:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
TEST (Uri, 0093) {
  auto const x = uri::split ("H:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "H");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
TEST (Uri, 0094) {
  auto const x = uri::split (R"(K:??)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
TEST (Uri, 0095) {
  auto const x = uri::split (R"(c-:?#)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "c-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
TEST (Uri, 0096) {
  auto const x = uri::split ("Bw:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Bw");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
TEST (Uri, 0097) {
  auto const x = uri::split ("hC:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "hC");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
TEST (Uri, 0098) {
  auto const x = uri::split ("q:?/#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "q");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "/");
}
TEST (Uri, 0099) {
  auto const x = uri::split ("L:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
TEST (Uri, 0100) {
  auto const x = uri::split ("W-:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "W-");
  EXPECT_FALSE (x->userinfo);
  EXPECT_FALSE (x->host);
  EXPECT_FALSE (x->port);
  EXPECT_TRUE (x->path.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
