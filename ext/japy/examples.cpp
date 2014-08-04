/* 

JAPY is a new JSON parser in C++.

This file contains examples of JAPY in action. 
For description of japy path language see README.

The MIT License (MIT)

Copyright (c) 2014 Artur Brugeman, brugeman.artur@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "japy.hpp"

struct hello_t
{
   std::string str;
};

static const japy::node_t &
operator>> (const japy::node_t & node, hello_t & hello)
{
   hello.str = "hello ";
   hello.str += node.body ();
   return node;
}

static void
test1 ()
{
   // this is the simplest thing you can do:
   // get a single value out of json document

   int i = 0;

   // JAPY uses 'paths' to address parts of json documents
   // 'a' path refers to 'child members named a'
   japy::parse ("{\"a\":1}", "a") >> i;
   assert (i == 1);

   // how about a more complex document?
   // '//b' refers to 'descendant members named b'
   japy::parse ("{\"a\":{\"b\":\"2\"}}", "//b") >> i;
   // note how string "2" was converted to int
   assert (i == 2);

   // what if required field does not exist?
   try
   {
      // exception is thrown if we don't find a match
      japy::parse ("{\"a\":3}", "b") >> i;
      assert (0);
   }
   catch (...)
   {}
   
   // what if we want a default value to be used instead of exception?
   i = -1; // default
   // ? in path means 'optional', if no match - i is not written
   japy::parse ("{\"a\":4}", "?b") >> i;
   assert (i == -1);

   // what if matched value is out of range of our numeric variable?
   try
   {
      // out_of_range exception is thrown (if your ints are 32 bit, of course)
      japy::parse ("{\"a\":5555555555}", "a") >> i;
      assert (0);

      // what if we don't want an out_of_range exception thrown here?
      // 1. I believe that is most probably a design error - 
      //    fix it, i.e. by using a wider type.
      // 2. Use try-catch if only you really must - 'i' is not 
      //    modified in case of exception.
   }
   catch (...)
   {}

   // how about an even more complex document?
   // '/#/$/a' refers to "root array's child objects' members named a"
   // notice how leading '/' makes first selector match agains root, not children
   japy::parse ("[{\"a\":6}]", "/#/$/a") >> i;
   assert (i == 6);

   // what if we want to make sure the selected value is of integral type?
   // 'i:' in path means 'node type - integral'
   japy::parse ("{\"a\":7}", "i:a") >> i;
   assert (i == 7);   
   japy::parse ("{\"a\":\"77\"}", "?i:a") >> i;
   assert (i == 7);

   // ok, one more thing
   // '/#a/*' refers to 'elements of child array named a'
   japy::parse ("{\"a\":[8, 88]}", "#a/*") >> i;
   // note that both 8 and 88 are a match, but >> extracts only the first one
   assert (i == 8);

   // now, escaped sequences decoding
   std::string s;
   japy::parse ("{\"a\":\"\\n\\u20AC\\uD834\\uDD1E\\ud834ab\\c\"}", "a") >> s;
   assert (s == "\n\xE2\x82\xAC\xF0\x9D\x84\x9E\xED\xA0\xB4""abc");

   // now, do not decode the value, get the raw bytes
   std::string raw;
   japy::parse ("{\"a\":\"\\n\\u20AC\"}", "a") >> japy::raw (raw);
   assert (raw == "\\n\\u20AC");

   // now, get the full body of an object as a string
   std::string body;
   const char * json = "{\"a\":\"\\n\\u20AC\"}";
   japy::parse (json, "/$") >> japy::body (body);
   assert (body == json);

   // now, our custom object and overloaded >> (see above)
   hello_t hello;
   japy::parse ("[\"world\"]", "*") >> hello;
   assert (hello.str == "hello world");

   // that's pretty it
   printf ("test1 done\n");
}

static void
test2 ()
{
   // now, how about iterating over a collection?

   // easy!
   int sum = 0;
   // /#/$ means 'object elements of array root'
   for (auto object: japy::parse ("[{\"a\":1}, {\"a\":2}]", "/#/$"))
   {
      int v = 0;
      // see the sub-query - 'a' works on the pre-selected object
      object["a"] >> v;
      sum += v;
   }
   assert (sum == 3);

   // or one by one extraction of nodes
   sum = 0;
   auto node_set (japy::parse ("[{\"a\":1}, {\"a\":2}]", "$/a"));
   int v1 = 0;
   int v2 = 0;
   node_set >> v1 
	    >> v2;
   sum += v1 + v2;
   assert (sum == 3);

   // a bit more complex document
   sum = 0;
   // '//$/a' means "all descendant objects' children named a"
   for (auto a_member: japy::parse ("[{\"a\":1}, [{\"a\":2}]]", "//$/a"))
   {
      int v = 0;
      // note that we don't need a sub-query here
      a_member >> v;
      sum += v;
   }
   assert (sum == 3);

   // and what if some parts of document do not match?
   sum = 0;
   // same path as in previous example
   try
   {
      const char * json = "[{\"a\":1}, [{\"b\":2}], {\"a\":3}]";
      for (auto a_member: japy::parse (json, "//$/a"))
      {
	 int v = 0;
	 a_member >> v;
	 sum += v;
      }
      assert (0);
   }
   catch (...)
   {
      // Path matching is done in 'all or nothing' manner,
      // that is - if first selector matches - than all path must match.
      // {\"b\":2} matches path head '//$', but does not match the tail 'a',
      // so exception is thrown as soon as japy finishes parsing it.
   }
   assert (sum == 1);

   // to avoid exceptions on partially matching branches, 
   // use ? on selector that may observe mismatch
   sum = 0;
   for (auto a_member: japy::parse ("[{\"a\":1}, [{\"b\":2}]]", "//$/?a"))
   {
      int v = 0;
      a_member >> v;
      sum += v;
   }
   assert (sum == 1);

   // how about sub-iterating?
   // # only selects the child array, and skips the object 
   sum = 0;
   for (auto arr: japy::parse ("[{\"a\":1}, [2, 3]]", "#"))
   {
      for (auto element: arr["*"])
      {
	 int v = 0;
	 element >> v;
	 sum += v;
      }
   }
   assert (sum == 5);

   // that's it on iterating
   printf ("test2 done\n");
}

static void
test3 ()
{
   // now how about parsing a json stream?

   {
   // a bit more to do, but still easy!
   const char * part1 = "[1, 2";
   const char * part2 = ", 3]";

   // will be 6 when we're done
   int sum = 0;

   // create a parser, specify a 'scope-path'
   // objects referred by scope-path are buffered internally 
   // (if seen on the parts' borders)
   japy::parser_t parser ("*");

   // feed a part of your json into parser
   parser.put (part1);

   // iterate over elements found in first part
   for (auto element: parser)
   {
      int v = 0;
      element >> v;
      sum += v;
   }

   // only the first element was returned, as japy didn't yet see
   // the end of second element
   assert (sum == 1);

   // feed the second part of your json
   parser.put (part2);

   // iterate over elements found in second part
   for (auto element: parser)
   {
      int v = 0;
      element >> v;
      sum += v;
   }

   // now we're done!
   assert (sum == 6);
   }

   {

   // So, the structure of your stream parser will most 
   // probably be like this:

   // 0. A container to store results of json processing.
   int sum = 0;

   // 1. Some input stream that produces parts of json.
   // Here, we'll use a plain string that will produce single chars.
   // I recommend you to choose the size of your parts (your read buffer)
   // such that it could allocate >10 of the typical node size
   // that is addressed by the scope-path - this will make 
   // japy internal buffering costs insignificant.
   std::string stream ("[11, 22, 33]");

   // 2. Parser with a proper scope
   japy::parser_t parser ("*");
   for (auto part: stream)
   {
      // 3. Feed part to the parser
      parser.put (part);

      // 4. Iterate over nodes addressed by scope-path
      for (auto node: parser)
      {
	 int v = 0;
	 node >> v;
	 sum += v;
      }
   }

   // that's it
   assert (sum == 66);

   }

   {
   // normally a json document must have a single root,
   // and an exception is thrown in presence of 'trailing garbage'.
   // however, sometimes you have streams of json documents, 
   // each having a root. use 'allow_many_roots' option for that case:
   int sum = 0;
   japy::parser_t parser ("a");
   parser.allow_many_roots (true);
   parser.put ("{\"a\":1} {\"a\":2}"); // two json docs in one stream
   for (auto node: parser)
   {
      int v = 0;
      node >> v;
      sum += v;
   }
   assert (sum == 3);
   }

   // I guess I'll add more stuff here when questions arise.
   printf ("test3 done\n");
}

int
main ()
{
   try
   {
      test1 ();
      test2 ();
      test3 ();   
   }
   catch (std::exception & e)
   {
      printf ("ERROR: %s\n", e.what ());
   }
}
