# A client library for communicating with Ayxia Trace

## Synopsis

This is an implementation of the Ayxia Trace Client C/C++ API that provides encapsulation of the protocol used to send trace messages to the Ayxia Trace GUI.

The C++ API provides the best performance since it can take advantage of variadic templates to gather type information which can be used to perform text formatting in the viewer application instead of in the code being traced.  This can result in significant speedups.

## Log Viewer

The Ayxia Trace log viewer application is available here:

[https://www.ayxia.com](https://www.ayxia.com)

## Code Example

### C++

The C++ API uses {arg*n*} type formatting often seen in languages like C# and Java.  By default, the API can format char pointers, numbers, std::string and std::wstring.  Unicode characters should be fine and std::string is considered UTF-8 encoded. 

```C++
#include <trace/trace.h>

int main() 
{
  TRACE_ERROR("some.channel",
    "this is a test {0} {1} {2,16:4} {3:4} 0x{0:x}",
    123, "test", float(123), double(123));
}
```

### C

Using C isn't quite as easy because it uses printf format to format the string, so all of the caveats that come with printf also apply, for example using %s with an integer value will likely cause a fault, or trying to format a floating point value with %d will give you incorrect results.

```C
TRACE_INFO("some.channel", "this is a test %d", i);

```

## License

The software in this repository (not including submodules) is licensed under Apache License 2.0, included here by reference.

[Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)


