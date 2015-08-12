@0x952ac552a1954f33;

# implements: List of lists (LIL)
# http://en.wikipedia.org/wiki/Sparse_matrix

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("capnp_genes");

struct Everything {
  geneNames @0 :List(Text);
  positions @1 :List(Position);
  genes @2 :List(Gene);
}

struct Position {
  barcode @0 :Text;
  xcoord @1 :UInt16;
  ycoord @2 :UInt16;
}

struct Gene {
    ylinesManyHits @0 :List(YlineManyHits);
}

struct YlineManyHits {
  xcoord @0 :List(UInt16);
  hits @1 :List(UInt32);
}



#struct Yline {
#  xcoord @0 :List(UInt16);
#  hits @1 :List(UInt8);
#}

